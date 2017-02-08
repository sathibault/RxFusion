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

//////////////////////////////////////// Operators

Project1 = function () {
    return Map(x => x[0]);
}

Project2 = function () {
    return Map(x => x[1]);
}

Project3 = function () {
    return Map(x => x[2]);
}

Count = function () {
    return Scan((x, acc) => acc + 1, 0);
}

Delay = function (width) {
    var win = Window(width, false);
    var prj = Map(x => x[0]);
    prj.attach(win);
    return new rxbase.Compound(win, prj);
}

WinSum = function(width) {
    if (width == undefined)
	throw new Error("WinSum missing width argument");
    return Sliding(width, (x,acc) => acc + x, (x,acc) => acc - x, 0);
}

SumOf = function (count) {
    if (count == undefined)
	throw new Error("SumOf missing count argument");
    return BatchFold1(count, (x,acc) => acc + x);
}

SumOver = function (millis) {
    if (millis == undefined)
	throw new Error("SumOver missing millis argument");
    var sum = IntermFold((elm, accum) => {
	return accum + elm;
    }, 0);
    var poll = Poll(millis);
    poll.setSource(sum);
    return new rxbase.Compound(sum, poll);
}

WinAverage = function(width) {
    if (width == undefined)
	throw new Error("WinAverage missing width argument");
    var sum = Sliding(width, (x,acc) => acc + x, (x,acc) => acc - x, 0);
    var map = Map(x => x / width);
    map.attach(sum);
    return new rxbase.Compound(sum, map);
}

AverageOf = function (count) {
    if (count == undefined)
	throw new Error("AverageOf missing count argument");
    var sum = BatchFold1(count, (x,y) => x + y);
    var map = Map(x => x / count);
    map.attach(sum);
    return new rxbase.Compound(sum, map);
}

AverageOver = function (millis) {
    if (millis == undefined)
	throw new Error("AverageOver missing millis argument");
    var sum = IntermFold((elm, accum) => {
	return [accum[0] + elm, accum[1] + 1];
    }, [0,0]);
    var poll = Poll(millis);
    poll.setSource(sum);
    var avg = Map((x) => x[0] / x[1]);
    avg.attach(poll);
    return new rxbase.Compound(sum, avg);
}

WinMin = function(width) {
    if (width == undefined)
	throw new Error("WinMin missing width argument");
    var win = Window(width, false);
    var map = Map((x) => {
	var n = x.length;
	var m = x[0];
	for (var i = 1; i < n; i++) {
	    if (x[i] < m)
		m = x[i];
	}
	return m;
    });
    map.attach(win);
    return new rxbase.Compound(win, map);
}

MinOf = function (count) {
    if (count == undefined)
	throw new Error("MinOf missing count argument");
    return BatchFold1(count, (x,acc) => (acc <= x) ? acc : x);
}

MinOver = function (millis) {
    if (millis == undefined)
	throw new Error("MinOver missing millis argument");
    var minop = IntermFold((elm, accum) => {
	return (elm < accum) ? elm : accum;
    }, 2147483647);
    var poll = Poll(millis);
    poll.setSource(minop);
    return new rxbase.Compound(minop, poll);
}

WinMax = function(width) {
    if (width == undefined)
	throw new Error("WinMax missing width argument");
    var win = Window(width, false);
    var map = Map((x) => {
	var n = x.length;
	var m = x[0];
	for (var i = 1; i < n; i++) {
	    if (x[i] > m)
		m = x[i];
	}
	return m;
    });
    map.attach(win);
    return new rxbase.Compound(win, map);
}

MaxOf = function (count) {
    if (count == undefined)
	throw new Error("MaxOf missing count argument");
    return BatchFold1(count, (x,acc) => (acc >= x) ? acc : x);
}

MaxOver = function (millis) {
    if (millis == undefined)
	throw new Error("MaxOver missing millis argument");
    var maxop = IntermFold((elm, accum) => {
	return (elm > accum) ? elm : accum;
    }, -2147483648);
    var poll = Poll(millis);
    poll.setSource(maxop);
    return new rxbase.Compound(maxop, poll);
}

//////////////////////////////////////// Math

Const = function (c) {
    if (c == undefined)
	throw new Error("Cons missing argument c");
    return Map(x => c);
}

Abs = function () {
    return Map(x => x < 0 ? -x : x);
}

Scale = function (k) {
    if (k == undefined)
	throw new Error("Scale missing argument k");
    return Map(x => k * x);
}

Add = function (b) {
    if (b == undefined)
	throw new Error("Add missing argument b");
    return Map(x => b + x);
}

ClampHigh = function (u) {
    if (u == undefined)
	throw new Error("ClampHigh missing argument u");
    return Map(x => x > u ? u : x);
}

ClampLow = function (l) {
    if (l == undefined)
	throw new Error("ClampLow missing argument l");
    return Map(x => x < l ? l : x);
}

Clamp = function (l, u) {
    if (l == undefined)
	throw new Error("Clamp missing argument l");
    if (u == undefined)
	throw new Error("Clamp missing argument u");
    return Map(x => x < l ? l : (x > u ? u : x));
}

IsOver = function (th) {
    if (th == undefined)
	throw new Error("Missing required threshold argument");
    return Map((x) => x > th);
}

IsUnder = function (th) {
    if (th == undefined)
	throw new Error("Missing required threshold argument");
    return Map((x) => x < th);
}

IsAtLeast = function (th) {
    if (th == undefined)
	throw new Error("Missing required threshold argument");
    return Map((x) => x >= th);
}

IsAtMost = function (th) {
    if (th == undefined)
	throw new Error("Missing required threshold argument");
    return Map((x) => x <= th);
}

IsPositive = function () {
    return Map((x) => x > 0);
}

IsNegative = function () {
    return Map((x) => x < 0);
}

IsZero = function () {
    return Map((x) => x == 0);
}

Toggle = function (init) {
    if (init == undefined)
	throw new Error("Const missing argument initial");
    return Scan((x, state) => !state, init);
}


//////////////////////////////////////// Filtering

Drop = function (n) {
    var cnt = Counted();
    var filter = Filter((x) => x[1] > n);
    filter.attach(cnt);
    var prj = Project1();
    prj.attach(filter);
    return new rxbase.Compound(cnt, prj);
}

Take = function (n) {
    var cnt = Counted();
    var filter = Traffic((x) => {
	return x[1] <= n ? 1 : 0;
    });
    filter.attach(cnt);
    var prj = Project1();
    prj.attach(filter);
    return new rxbase.Compound(cnt, prj);
}

Dedup = function (expire) {
    if (expire == undefined) {
	var scan = Scan((x,state) => [x, state[0]], [null,null]);
	var filter = Filter((x) => x[0] != x[1]);
	filter.attach(scan);
	var prj = Project1();
	prj.attach(filter);
	return new rxbase.Compound(scan, prj);
    } else {
	var scan = Scan((x,state) => {
	    var now = process.uptime();
	    var age = Math.floor((now - state[1]) * 1000);
	    var emit = (x != state[0]) || (age >= expire);
	    var t = (emit) ? now : state[1];
	    return [x, t, emit];
	}, [null,0,false]);
	var filter = Filter((x) => x[2]);
	filter.attach(scan);
	var prj = Project1();
	prj.attach(filter);
	return new rxbase.Compound(scan, prj);
    }
}


True = function () {
    return Filter((x) => x);
}

False = function () {
    return Filter((x) => !x);
}

Over = function (th) {
    if (th == undefined)
	throw new Error("Missing required threshold argument");
    return Filter((x) => x > th);
}

Under = function (th) {
    if (th == undefined)
	throw new Error("Missing required threshold argument");
    return Filter((x) => x < th);
}

AtLeast = function (th) {
    if (th == undefined)
	throw new Error("Missing required threshold argument");
    return Filter((x) => x >= th);
}

AtMost = function (th) {
    if (th == undefined)
	throw new Error("Missing required threshold argument");
    return Filter((x) => x <= th);
}

Positive = function () {
    return Filter((x) => x > 0);
}

Negative = function () {
    return Filter((x) => x < 0);
}

Zero = function () {
    return Filter((x) => x == 0);
}


//////////////////////////////////////// String

ToString = function() {
    return Map(x => x.toString());
}

ToJson = function() {
    return Map(x => JSON.stringify(x));
}

Split = function(delimiter) {
    return Map(x => x.split(delimiter));
}


//////////////////////////////////////// Misc

SocketIO = function Constructor(io, event) {
    rxbase.Consumer.apply(this, []);
    this.io = io;
    this.event = event;
}

SocketIO.prototype = new rxbase.Consumer();

SocketIO.prototype.onData = function(source, value) {
    this.io.emit(this.event, value);
}
