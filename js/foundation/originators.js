Interval = function Constructor(millis) {
    rxbase.Originator.apply(this, []);
    this.interval = millis;
    this.current = 0;
}

Interval.prototype = new rxbase.Originator();

Interval.prototype.init = function() {
    rxbase.scheduleInterval(this, this.interval);
}

Interval.prototype.run = function () {
    this.push(this.current);
    this.current += 1;
    return this.subscribers.length > 0;
}

Repeat = function Constructor(value) {
    rxbase.Originator.apply(this, []);
    this.value = value;
}

Repeat.prototype = new rxbase.Originator();

Repeat.prototype.init = function() {
    rxbase.scheduleAlways(this);
}

Repeat.prototype.run = function () {
    this.push(this.value);
    return this.subscribers.length > 0;
}
