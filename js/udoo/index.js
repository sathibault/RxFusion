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

var rx = require('rxfusion');

var serialtty = require('./build/Release/serialtty');

Mcu = function Constructor() {
    rxbase.Originator.apply(this, []);
}

Mcu.prototype = new rxbase.Originator();

Mcu.prototype.subscribe = function(node) {
    if (this.subscribers.length == 0) {
	this.tty = new serialtty.SerialTTY('/dev/ttyMCC');
	var cb = function (chunk) {
	    this.push(JSON.parse(chunk.toString().trim()));
	}
	console.log('bind');
	this.tty.on('data', cb.bind(this));
    }
    rxbase.RxNode.prototype.subscribe.apply(this, [node]);
}

module.exports = rx;
