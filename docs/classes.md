## Generators

Classes that generated data on demand (e.g. via Iterate)

### Range

<img align="left" src="../img/cpp.png">
Range&lt;*type*&gt;(*from*, *to*, *increment*)
<br>
<img align="left" src="../img/javascript.png">
new Range(*from*, *to*, *increment*)

Generates integers in the range `from` to `to` by `increment`.  The increment is optional and defaults to 1.  Combine with `Iterate` to generate a stream.

## Producers

Classes that produce a stream of data

### Interval

<img align="left" src="../img/cpp.png">
Interval&lt;*type*&gt;(*millis*)
<br>
<img align="left" src="../img/javascript.png">
new Interval(*millis*)

Produces integers from 0 on every `millis` milliseconds.

### Repeat

<img align="left" src="../img/cpp.png">
Repeat&lt;*type*&gt;(*value*)
<br>
<img align="left" src="../img/javascript.png">
new Repeat(*value*)

Produces `value` every cycle.

## Inputs

Classes that receive or sample input data

### AnalogIn

<img align="left" src="../img/cpp.png">
AnalogIn&lt;*type*&gt;(*pin*)
<br>
<img align="left" src="../img/javascript.png">
N/A

Reads analog values from an ADC `pin`.

### BitIn

<img align="left" src="../img/cpp.png">
BitIn&lt;*type*&gt;(*pin*)
<br>
<img align="left" src="../img/javascript.png">
N/A

Reads values from a digital `pin`.

### GpioSerial

<img align="left" src="../img/cpp.png">
GpioSerial(*rxPin*, *txPin*, *baud*)
<br>
<img align="left" src="../img/javascript.png">
N/A

Reads serial input from `rxPin` and outputs char values.  A `txPin` is required, and will be put in output mode, but is never set.

## Outputs

Classes that send or output data

### AnalogOut

<img align="left" src="../img/cpp.png">
AnalogOut&lt;*type*&gt;(*pin*)
<br>
<img align="left" src="../img/javascript.png">
N/A

Writes values to a PWM `pin`.

### BitOut

<img align="left" src="../img/cpp.png">
BitOut&lt;*type*&gt;(*pin*)
<br>
<img align="left" src="../img/javascript.png">
N/A

Writes values to a digital `pin`.

### Console

<img align="left" src="../img/cpp.png">
Console&lt;*type*&gt;(*baud*)
<br>
<img align="left" src="../img/javascript.png">
new Console()

*C++:* Writes value to the serial output at the given `baud`.  *Javascript:* Writes value to the console.

### HttpPost

<img align="left" src="../img/cpp.png">
HttpPost(*transport*, *host*, *port*, *path*, *numHeaders*, *headers*)
<br>
<img align="left" src="../img/javascript.png">
N/A

Sends data to an HTTP endpoint using the HTTP POST method using transport (e.g. an instance of WiFiClient).  Optional headers are provided as n array of strings.

### MqttPub

<img align="left" src="../img/cpp.png">
MqttPub&lt;*type*&gt;(*transport*, *host*, *port*, *topic*, *clientId*, *username*, *password*, *keepAlive*)
<br>
<img align="left" src="../img/javascript.png">
N/A

Publishs data to an MQTT endpoint with QOS 0.  The username, password, and keepAlive parameters are optional.

### SocketIO

<img align="left" src="../img/cpp.png">
N/A
<br>
<img align="left" src="../img/javascript.png">
new SocketIO(*io*, *event*)

Sends data to a WebSocket via the socket.io module `io` with the event name `event`.

## Consumers

### Consume

<img align="left" src="../img/cpp.png">
Consume&lt;*type*&gt;(*function*)
<br>
<img align="left" src="../img/javascript.png">
new Consume(*function*)

Calls the function for each input value with that value as an argument.

