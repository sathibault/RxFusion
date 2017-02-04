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

//////////////////////////////////////// Basic components

function CounterOp() {
    rxbase.Operator.apply(this, []);
    this.count = 0;
}

CounterOp.prototype = new rxbase.Operator();

CounterOp.prototype.onData = function (source, data) {
    this.count += 1;
    this.push([data, this.count]);
}

Counted = function () { return new CounterOp(); }

function MapOp(fun) {
    rxbase.Operator.apply(this, []);
    this.fun = fun;
}

MapOp.prototype = new rxbase.Operator();

MapOp.prototype.onData = function (source, data) {
    this.push(this.fun(data));
}

Map = function (fun) { return new MapOp(fun); }

function FilterOp(fun) {
    rxbase.Operator.apply(this, []);
    this.fun = fun;
}

FilterOp.prototype = new rxbase.Operator();

FilterOp.prototype.onData = function (source, data) {
    if (this.fun(data))
	this.push(data);
}

Filter = function (fun) { return new FilterOp(fun); }

function ScanOp(fun, init) {
    rxbase.Operator.apply(this, []);
    this.fun = fun;
    this.accum = init;
}

ScanOp.prototype = new rxbase.Operator();

ScanOp.prototype.onData = function (source, data) {
    this.accum = this.fun(data, this.accum);
    this.push(this.accum);
}

Scan = function (fun, init) { return new ScanOp(fun, init); }

function WindowOp(width, all) {
    rxbase.Operator.apply(this, []);
    this.width = width;
    this.all = all;
    this.window = [];
}

WindowOp.prototype = new rxbase.Operator();

WindowOp.prototype.onData = function (source, data) {
    this.window.push(data);
    if (this.window.length > this.width)
	this.window.shift();
    if (this.all || this.window.length == this.width)
	this.push(this.window);
}

Window = function(width, all) {
    return new WindowOp(width,
			all === undefined ? false : all);
}

function SlidingOp(width, adder, remover, initial, all) {
    rxbase.Operator.apply(this, []);
    this.width = width;
    this.all = all;
    this.adder = adder;
    this.remover = remover;
    this.window = [];
    this.accum = initial;
}

SlidingOp.prototype = new rxbase.Operator();

SlidingOp.prototype.onData = function (source, data) {
    this.window.push(data);
    if (this.window.length > this.width) {
	this.accum = this.remover(this.window[0], this.accum);
	this.window.shift();
    }
    this.accum = this.adder(data, this.accum);
    if (this.all || this.window.length == this.width)
	this.push(this.accum);
}

Sliding = function(width, adder, remover, initial, all) {
    return new SlidingOp(width, adder, remover, initial,
			 all === undefined ? false : all);
}

function BatchOp(width) {
    rxbase.Operator.apply(this, []);
    this.width = width;
    this.window = [];
}

BatchOp.prototype = new rxbase.Operator();

BatchOp.prototype.onData = function (source, data) {
    this.window.push(data);
    if (this.window.length == this.width) {
	this.push(this.window);
	this.window = [];
    }
}

BatchOf = function(width) {
    return new BatchOp(width);
}

function BatchFoldOp(width, fun, init) {
    rxbase.Operator.apply(this, []);
    this.width = width;
    this.fun = fun;
    this.init = init;
    this.accum = init;
    this.count = 0;
}

BatchFoldOp.prototype = new rxbase.Operator();

BatchFoldOp.prototype.onData = function (source, data) {
    this.accum = this.fun(data, this.accum);
    this.count += 1;
    if (this.count == this.width) {
	this.push(this.accum);
	this.accum = this.init;
	this.count = 0;
    }
}

BatchFold = function(width, fun, init) {
    return new BatchFoldOp(width, fun, init);
}

function BatchFold1Op(width, fun) {
    rxbase.Operator.apply(this, []);
    this.width = width;
    this.fun = fun;
    this.count = 0;
}

BatchFold1Op.prototype = new rxbase.Operator();

BatchFold1Op.prototype.onData = function (source, data) {
    if (this.count > 0)
	this.accum = this.fun(data, this.accum);
    else
	this.accum = data;
    this.count += 1;
    if (this.count == this.width) {
	this.push(this.accum);
	this.count = 0;
    }
}

BatchFold1 = function(width, fun) {
    return new BatchFold1Op(width, fun);
}

function BatchScanOp(width, fun, init) {
    rxbase.Operator.apply(this, []);
    this.width = width;
    this.fun = fun;
    this.init = init;
    this.count = 0;
    this.accum = init;
}

BatchScanOp.prototype = new rxbase.Operator();

BatchScanOp.prototype.onData = function (source, data) {
    this.accum = this.fun(data, this.accum);
    this.count += 1;
    if (this.count == this.width) {
	this.push(this.accum);
	this.count = 0;
	this.accum = init;
    }
}

BatchScan = function(width, fun, init) {
    return new BatchScanOp(width, fun, init);
}

function TrafficOp(fun) {
    rxbase.Operator.apply(this, []);
    this.fun = fun;
}

TrafficOp.prototype = new rxbase.Operator();

TrafficOp.prototype.onData = function (source, data) {
    var op = this.fun(data);
    if (op > 0)
	this.push(data);
    else if (op == 0) {
	this.detach();
	this.close();
    }
}

Traffic = function (fun) {
    return new TrafficOp(fun);
}

//////////////////////////////////////// Timer operators

function SampleOp(millis, repeat) {
    rxbase.Operator.apply(this, []);
    this.timeout = millis;
    this.repeat = false;
    this.latest = null;
    rxbase.registerOrigin(this);
}

SampleOp.prototype = new rxbase.Operator();

SampleOp.prototype.init = function() {
    rxbase.scheduleInterval(this, this.timeout);
}

SampleOp.prototype.onData = function (source, data) {
    this.latest = data;
}

SampleOp.prototype.onClose = function (source) {
    rxbase.arrayRemove(this.sources, source);
    if (this.sources.length ==0)
	this.complete = true;
}

SampleOp.prototype.run = function() {
    if (this.latest != null) {
	this.push(this.latest);
	if (!this.repeat)
	    this.latest = null;
    }
    if (this.complete) {
	this.close();
	return false;
    }
    return true;
}

Sample = function(millis, repeat) {
    return new SampleOp(millis,
			repeat === undefined ? false : repeat);
}

function DebounceOp(millis) {
    rxbase.Operator.apply(this, []);
    this.timeout = millis;
    this.complete = false;
    rxbase.registerOrigin(this);
}

DebounceOp.prototype = new rxbase.Operator();

DebounceOp.prototype.init = function() {
}

DebounceOp.prototype.onData = function (source, data) {
    this.latest = data;
    rxbase.scheduleInterval(this, this.timeout);
}

DebounceOp.prototype.onClose = function (source) {
    rxbase.arrayRemove(this.sources, source);
    if (this.sources.length ==0)
	this.complete = true;
}

DebounceOp.prototype.run = function() {
    this.push(this.latest);
    if (this.complete)
	this.close();
    return false;
}

Debounce = function(millis) {
    return new DebounceOp(millis);
}

function ThrottleOp(millis) {
    rxbase.Operator.apply(this, []);
    this.timeout = millis;
    this.drop = false;
    rxbase.registerOrigin(this);
}

ThrottleOp.prototype = new rxbase.Operator();

ThrottleOp.prototype.init = function() {
    rxbase.scheduleInterval(this, this.timeout);
}

ThrottleOp.prototype.onData = function (source, data) {
    if (!this.drop)
	this.push(data);
    this.drop = true;
}

ThrottleOp.prototype.onClose = function (source, data) {
    rxbase.Operator.prototype.onClose.apply(this, [source, data]);
    clearInterval(this.schedule);
}

ThrottleOp.prototype.run = function() {
    this.drop = false;
    return true;
}

Throttle = function(millis) {
    return new ThrottleOp(millis);
}


////////////////////////////////////////

function IntermFoldOp(fun, init) {
    rxbase.State.apply(this, []);
    this.fun = fun;
    this.initial = init;
    this.accum = init;
}

IntermFoldOp.prototype = new rxbase.State();

IntermFoldOp.prototype.onData = function (source, data) {
    this.accum = this.fun(data, this.accum);
}

IntermFoldOp.prototype.get = function () {
    this.push(this.accum);
    this.accum = this.initial;
}

IntermFold = function (fun, init) {
    return new IntermFoldOp(fun, init);
}

function Poller(millis) {
    rxbase.Originator.apply(this, []);
    this.interval = millis;
}

Poller.prototype = new rxbase.Originator();

Poller.prototype.setSource = function(state) {
    this.state = state;
    this.attach(state);
}

Poller.prototype.onData = function(source, value) {
    this.push(value);
}

Poller.prototype.onClose = function (source) {
    rxbase.arrayRemove(this.sources, source);
    if (this.sources.length ==0)
	this.close();
}

Poller.prototype.init = function() {
    if (this.interval == 0)
	rxbase.scheduleAlways(this);
    else
	rxbase.scheduleInterval(this, this.interval);
}

Poller.prototype.run = function () {
    this.state.get();
    if (this.state.closed()) {
	this.close();
	return false;
    }
    return true;
}

Poll = function(millis) {
    return new Poller(millis);
}

function ConsumeOp(fun) {
    rxbase.Consumer.apply(this, []);
    this.fun = fun;
}

ConsumeOp.prototype = new rxbase.Consumer();

ConsumeOp.prototype.onData = function(source, value) {
    this.fun(value);
}

Consume = function (fun) {
    return new ConsumeOp(fun);
}


//////////////////////////////////////// User instantiated

Range = function Constructor(x, y) {
    rxbase.Generator.apply(this, []);
    if (y === undefined) {
	this.start = 0;
	this.end = x;
    } else {
	this.start = x;
	this.end = y+1;
    }
}

Range.prototype = new rxbase.Generator();

Range.prototype.reset = function () {
    this.current = this.start;
}

Range.prototype.next = function () {
    this.push(this.current);
    this.current += 1;
}

Range.prototype.more = function () {
    return this.current < this.end;
}

Doc = function Constructor() {
    rxbase.Consumer.apply(this, []);
    this.values = [];
}

Doc.prototype = new rxbase.Consumer();

Doc.prototype.onData = function(source, value) {
    this.values.push(value.toString());
}

Doc.prototype.text = function() {
    return this.values.join(' ');
}

Console = function Constructor() {
    rxbase.Consumer.apply(this, []);
}

Console.prototype = new rxbase.Consumer();

Console.prototype.onData = function(source, value) {
    console.log(value);
}
