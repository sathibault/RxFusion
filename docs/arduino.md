# RxFusion for Arduino

RxFusion currently supports the Arduino Zero and MKR1000.

## Tutorial

There are three parts to every RxFusion application for Arduino:

**Part 1 - Header files**

```c++
#include <WiFi101.h>
#include <RTCZero.h>
#include <RxFusion.h>
```

The `WiFi101` header is only required if you plan to use WiFi.

**Part 2 - Globals**

* All inputs and outputs are also instantiated as global variables.
* If you are using networking then you need to instantiate a transport class (e.g. `WifiClient`).

This part varies for each application, but an example for a pulsing LED might be:

```c++
Range<int> ramp(-200,200,5);
AnalogOut<int> led(6)
```

The `Range` class generates integers within some range.  The `AnalogOut` class drives a PWM ping, in this case pin 6.

See [Classes](api-reference.md#classes) for a complete list of input/output classes.  Application setup methods are documented in [Methods](api-reference#methods).

**Part 3 - Main function**

The `app` function is the main function that performs any device configuration (e.g. connecting to WiFi) and stream expressions.

For the pulsing LED example, this function might be:

```c++
void app() {
  ramp >> Iterate<int>(100,true) >> Abs<int>() >> led;
}
```

It contains a single stream expression. This stream iterates over the `ramp` range every 100 ms with repeat.  The `Abs` operator takes the absolute value of the iterated numbers and sends the output to the LED.  The absoulte value produces a saw-tooth waveform from 0 to 200 fading the LED on and off.

There are a large number of operators you can combine to form stream expressions.  See [Operators](api-reference.md#operators) for documentation.
