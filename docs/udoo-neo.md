# RxFusion for UDOO Neo

RxFusion for UDOO Neo allows you to easily create dual-core applications
in C++ and Javascript that collect data via the Cortex M4 and connect
to the Internet via the Cortex A9.

## Installation

### For the Cortex M4

1.  Follow the general instructions for
[Arduino installation](arduino.md#installation) to program the UDOO's M4
core using the standard Arduino IDE.
2. Review the [Arduino tutorial](arduino.md#tutorial) for an introduction
to RxFusion for Arduino
3. Take a look a the examples below for UDOO specifics

### For the Cortex A9

Install the RxFusion UDOO Node.js module using npm:

```bash
$ npm install rxfusion-udoo
```

The examples below demonstrate how you can receive data from the M4 on
the A9 in a Node.js app.

## Dual-core example

This example application collects data from a light sensor attached to
the M4, which is sent to the A9, and then to a browser client over a
WebSocket.

### M4 MCU C++11 code

The following code collects data from the sensor and sends it to the
A9 CPU.  Any type of data can be sent the CPU via the built-in Cpu
class.  This example also drives an LED to fade in/out to provide some
interesting data from the light sensor.

```cpp
#include <RxFusion.h>

Range<int> ramp(-200,200,5);
AnalogIn<int> ambient(A0);
AnalogOut<int> led(6);
Cpu<int> cpu;

void app() {
  ramp >> Iterate<int>(100,true) >> Abs<int>() >> led;
  ambient >> Poll<int>(250) >> cpu;
}
```

The first statement iterates through the `ramp` range (values -200 to
200 by 5) and takes the absolute value to send to the PWM pin that
drives the led.  The Iterate operator takes two arguments, the first
is the delay in milliseconds and the second indicates whether to
repeat or not.  Since repeat is true, the LED will fade in and out
repeatedly.

The second statement polls the value of the light sensor every 250ms
and sends the value directly the A9 CPU.

### A9 CPU Javascript code

While MCUs require the use of a compiled language like C++, the A9 CPU
is capable of running higher-level languages that are easier to
program.  RxFusion supports both C++ and Node.js, so the A9 code is
written in Javascript for Node.js.

```javascript
var http = require('http');
var rx = require('rxfusion');

http.listen(8090, function(){
    var mcu = new Mcu();
    var browser = new SocketIO(io, 'hist');

    // Average 5 samples at a time with sliding window of data to socket.io clients
    rx()(mcu >> AverageOf(5) >> Window(16,true) >> browser);
});
```

The built-in Mcu object receives the data from the M4 sent via the Cpu
class.  The example does some simple post-processing of the data to
smooth out the data values by averaging windows of 5 values.  The
Window operator creates a sliding window of the last 16 values which
gets sent to the browser using SocketIO, a popular WebSocket
implementation.

The browser repeatedly receives an array of the last 16 values
received that can be easily plotted using a client side library.  For
example, the following provides a real-time graph using the c3
library:

```html
<script>
var chart = c3.generate({
  data: { columns: [], type: 'spline' }
});

var socket = io();
socket.on('hist', function(data) {
  chart.load({ columns: [ Array.concat('ambient', data) ] });
  socket.emit('ack', 'ok');
});
</script>
```

The `data` argument of the `socket.on` callback function receives the
WebSocket data sent to it via the browser object in the A9 Javascript
code above, which in this case is an array of the last 16 values.


## Multi-value example

If you have multiple values from say multiple sensors to send to the
CPU, you can zip them together into an array like this example:

```cpp
#include <Arduino.h>
#include <RxFusion.h>

static AnalogIn<int> ambient(A0);
static AnalogIn<int> temperature(A2);
static Cpu<Vec<int,2>> cpu;

void app() {
  auto data1 = ambient >> Poll<int>(250);
  auto data2 = temperature >> Poll<int>(250);
  (data1 & data2) >> cpu;
}
```

The zip operator `&` combines pairs of values from two streams into a
single stream of vectors.  Note the type of the Cpu stream is
Vec&lt;int,2&gt;, a vector of two integers.  On the A9, this will be
received as Javascript array.

## Multi-stream example

If you have multiple streams of data that are not sampled at the same
rate (i.e. can't be zipped), then you need to send them to the CPU
separately, but how can you tell them apart on the CPU?  The answer is
to format them as JSON with an identifying field name.

The following example sends two streams via JSON.

```cpp
#include <Arduino.h>
#include <RxFusion.h>

static AnalogIn<int> ambient(A0);
static Cpu<xstring> cpu;

void app() {
  auto raw = ambient >> Poll<int>(1000);
  raw >> Format<int>("{\"raw\": $1}") >> cpu;
  raw >> AverageOver<int>(5000) >> Format<int>("{\"smooth\": $1}") >> cpu;
}
```

The raw data is sent directly to the cpu every 1000ms and a smoothed
average value every 5000ns.

On the CPU, you could use `Filter` and `Map` to split out either the
raw or the smooth data as in this example:

```javascript
  var mcu = new Mcu();
  var console = new Console();

  rx()(mcu >> Filter(x => x.raw) >> Map(x => x.raw) >> console);
```

Here, `Filter` outputs just the incoming objects that have a `raw`
property, and `Map` extracts the value.
