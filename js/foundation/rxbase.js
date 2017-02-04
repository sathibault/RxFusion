/*

Copyright 2016 Green Mountain Computing Systems, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/

var chains = [];
var buildStack = [];

var always = [];
var nodeid = 0;

function arrayRemove(ary, elm) {
    var idx = ary.indexOf(elm);
    if (idx >= 0) {
	if (idx > 0)
	    ary[idx] = ary[0];
	ary.shift();
    }
}

function runAlways() {
    always = always.filter(x => x.run());
    if (always.length != 0)
	setImmediate(runAlways);
}

function runInterval(node) {
    if (!node.run()) {
	clearInterval(node.schedule);
	node.schedule = null;
    }
}

function registerOrigin(node) {
    if (this.schedule == null)
	node.init();
}

function scheduleAlways(node) {
    node.schedule = always;
    always.push(node);
    if (always.length == 1)
	setImmediate(runAlways);
}

function scheduleInterval(node, interval) {
    if (node.schedule != null)
	clearInterval(node.schedule);
    node.schedule = setInterval(runInterval, interval, node);
}

function RxNode() {
    nodeid += 1;
    this.rxid = nodeid;
    this.sources = [];
    this.subscribers = [];
    this.schedule = null;
}

RxNode.prototype.getRxID = function() {
    return this.rxid;
}

RxNode.prototype.onData = function(publisher, value) {
}

RxNode.prototype.onClose = function(publisher) {
}

RxNode.prototype.push = function(value) {
    for (var i in this.subscribers)
	this.subscribers[i].onData(this, value);
}

RxNode.prototype.subscribe = function(node) {
    //console.log('subscribe');
    this.subscribers.push(node);
    return this;
}

RxNode.prototype.unsubscribe = function(node) {
    //console.log('unsubscribe');
    arrayRemove(this.subscribers, node);
    if (this.subscribers.length == 0 && this.sources.length > 0)
	this.detach();
}

RxNode.prototype.close = function(src) {
    while (this.subscribers.length > 0)
	this.subscribers.pop().onClose(this);
}

RxNode.prototype.attach = function(src) {
    //console.log('attach ' + src.getRxID() + ' -> ' + this.rxid);
    this.sources.push(src.subscribe(this));
}

RxNode.prototype.detach = function(src) {
    //console.log('detach');
    while (this.sources.length > 0)
	this.sources.pop().unsubscribe(this);
}

RxNode.prototype.toString = function() {
    buildStack.push(this);
    return buildStack.length > 1 ? 2 : 3;
}

RxNode.prototype.valueOf = function() {
    return this;
}

function Originator() {
    RxNode.apply(this, []);
}

Originator.prototype = new RxNode();

Originator.prototype.subscribe = function(node) {
    if (this.subscribers.length == 0)
	registerOrigin(this);
    return RxNode.prototype.subscribe.apply(this, [node]);
}

function Operator() {
    RxNode.apply(this, []);
}

Operator.prototype = new RxNode();

Operator.prototype.onClose = function(source) {
    arrayRemove(this.sources, source);
    if (this.sources.length ==0)
	this.close();
}

function Consumer() {
    RxNode.apply(this, []);
}

Consumer.prototype = new RxNode();

Consumer.prototype.onClose = function(source) {
    arrayRemove(this.sources, source);
    if (this.waiter) {
	//console.log('waiter');
	this.waiter();
    }
}

function Generator() {
    RxNode.apply(this, []);
}

Generator.prototype = new RxNode();

Generator.prototype.subscribe = function(node) {
    if (this.subscribers.length > 0)
	throw new Error("Generator cannot have multiple iterators");
    return RxNode.prototype.subscribe.apply(this, [node]);
}

Generator.prototype.getAutoIterator = function() {
    if (this.autoIterator)
	return this.autoIterator;
    this.autoIterator = new Iterator();
    this.autoIterator.attach(this);
    return this.autoIterator;
}

function State() {
    RxNode.apply(this, []);
    this.complete = false;
}

State.prototype = new RxNode();

State.prototype.onClose = function (source) {
    arrayRemove(this.sources, source);
    if (this.sources.length ==0)
	this.complete = true;
}

State.prototype.closed = function () {
    return this.complete;
}

function Compound(front, back) {
    this.front = front;
    this.back = back;
}

// pick up toString/valueOf
Compound.prototype = new RxNode();

Compound.prototype.attach = function (src) {
    //console.log('compound attach');
    this.front.attach(src);
}

Compound.prototype.subscribe = function (node) {
    //console.log('compound subscribe');
    this.back.subscribe(node);
}

function Iterator(millis, repeat, autoRepeat) {
    Originator.apply(this, []);
    this.interval = millis;
    this.repeat = repeat;
    this.autoRepeat = autoRepeat;
}

Iterator.prototype = new Originator();

Iterator.prototype.attach = function(src) {
    RxNode.prototype.attach.apply(this, [src]);
    this.generator = src;
}

Iterator.prototype.onData = function(publisher, value) {
    this.push(value);
}

Iterator.prototype.init = function() {
    this.generator.reset();
    if (this.generator.more()) {
	if (this.interval == 0)
	    scheduleAlways(this);
	else
	    scheduleInterval(this, this.interval);
    }
}

Iterator.prototype.run = function() {
    this.generator.next();
    if (this.generator.more())
	return true;
    if (this.repeat) {
	this.generator.reset();
	return this.autoRepeat;
    }
    this.detach(); // unsubscribe to generator
    this.close();
    return false;
}

Iterate = function(millis, repeat, autoRepeat) {
    return new Iterator(millis === undefined ? 0 : millis,
			repeat === undefined ? false : repeat,
			autoRepeat === undefined ? false : autoRepeat
		       );
}


//////////////////////////////////////// Join components

function Zipper(elements) {
    Operator.apply(this, []);
    for (var i = 0; i < elements.length; i++)
	this.attach(elements[i]);
    this.vec = elements.map(x => null);
    this.count = 0;
}

Zipper.prototype = new Operator();

Zipper.prototype.onData = function (source, data) {
    var idx = this.sources.indexOf(source);
    if (idx >= 0) {
	if (this.vec[idx] == null)
	    this.count += 1;
	this.vec[idx] = data;
    }
    if (this.count == this.vec.length) {
	this.push(this.vec);
	this.vec = this.vec.map(x => null);
	this.count = 0;
    }
}

function Merger(elements) {
    Operator.apply(this, []);
    for (var i = 0; i < elements.length; i++)
	this.attach(elements[i]);
}

Merger.prototype = new Operator();

Merger.prototype.onData = function (source, data) {
    this.push(data);
}

//////////////////////////////////////// Builder

function Pipe(o,x) {
    this.op = o;
    this.chain = x;
    this.back = x[x.length-1];
}

Pipe.prototype.valueOf = function() {
    return this;
}

Pipe.prototype.toString = function() {
    buildStack.push(this);
    return buildStack.length > 1 ? 2 : 3;
}

Pipe.prototype.getRxID = function() {
    return this.back.rxid;
}

Pipe.prototype.subscribe = function(node) {
    this.back.subscribe(node);
    return this.back;
}

function autoIterate(ary) {
    for (var i in ary) {
	var node = ary[i];
	if (node instanceof Generator) {
	    //console.log('auto iterate');
	    ary[i] = node.getAutoIterator();
	}
    }
    return ary;
}

function chain(magic) {
    //console.log(buildStack.map(x => x instanceof Generator));
    if (magic == 0) {
	for (var i = 1; i < buildStack.length; i++) {
	    var pred = buildStack[i-1];
	    var node = buildStack[i];
	    //console.log(i);
	    if (pred instanceof Generator && !(node instanceof Iterator))
		pred = pred.getAutoIterator();
	    node.attach(pred);
	}
	pipe = new Pipe('>>', buildStack);
    } else if (magic == 2) {
	pipe = new Pipe('&', [new Zipper(autoIterate(buildStack))]);
    } else if (magic >= 5) {
	pipe = new Pipe('+', [new Merger(autoIterate(buildStack))]);
    } else {
	// 1,3
	pipe = new Pipe('^', [new Zipper(autoIterate(buildStack))]);
    }
    buildStack = [];
    return pipe;
}

rxbase = {
    rx: function() {
	chains.push(buildStack);
	buildStack = [];
	return (magic) => {
	    pipe = chain(magic);
	    buildStack = chains.pop();
	return pipe;
	};
    },
    test: function() {
	buildStack = [];
	return (magic) => {
	    return new Promise((resolve, reject) => {
		var pipe = chain(magic);
		pipe.chain[pipe.chain.length-1].waiter = resolve;
	    });
	};
    },
    RxNode: RxNode,
    Originator: Originator,
    Operator: Operator,
    Consumer: Consumer,
    Generator: Generator,
    State: State,
    Compound: Compound,
    registerOrigin: registerOrigin,
    scheduleAlways: scheduleAlways,
    scheduleInterval: scheduleInterval,
    arrayRemove: arrayRemove
};
