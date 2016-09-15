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
Console&lt;*type*&gt;(*buad*)
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

