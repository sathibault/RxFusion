# Operators

## Filtering

Operations that output a subset of their input items.

* [Drop](operators.md#drop) Output all input items except the first `n`
* [Take](operators.md#take) Output only the first `n` input items.
* [Dedup](operators.md#dedup) Output every input item that is not equal to the previous input item.
* [Dedup (expiring)](operators.md#dedup-expiring) Output the input item if it is not equal to the previous input or it has been more than `milli` milliseconds since the last output.
* [Debounce](operators.md#debounce) Outputs each input after `millis` milliseconds if no other input has been received.
* [Throttle](operators.md#throttle) Rate limits the input stream to *average* at most one value per `millis` milliseconds.  It functions by outputing the first input received (if any) in successive windows of `millis` milliseconds.
* [Sample](operators.md#sample) Generates a stream of values with the latest value from its input stream every `millis` milliseconds.
* [Over](operators.md#over) Output only input items greater than `threshold`.
* [Under](operators.md#under) Output only input items less than `threshold`.
* [AtLeast](operators.md#atleast) Output only input items greater than or equal to `threshold`.
* [AtMost](operators.md#atmost) Output only input items less than or equal to `threshold`.
* [Positive](operators.md#positive) Output only input items that are greater than 0.
* [Negative](operators.md#negative) Output only input items that are less than 0.
* [Zero](operators.md#zero) Output only input items that are less than 0.
* [True](operators.md#true) Output only input items equal to true.
* [False](operators.md#false) Output only input items equal to false.
* [Filter](operators.md#filter) For each input item, apply the given function and output the input if the function returns true.

## Transformation

Operations that transforms each input item to produce an output item.

* [Const](operators.md#const) For each input item, output `value`.
* [Abs](operators.md#abs) For each input item, output it's absolute value.
* [Scale](operators.md#scale) For each input item, output `k` __×__ input value.
* [Add](operators.md#add) For each input item, output `b` __+__ input value.
* [Clamp](operators.md#clamp) For each input item, output `lo` if the input is less than `lo`, `hi` if the input more than `hi`, or the input value if it is between `lo` and `hi`.
* [ClampLow](operators.md#clamplow) For each input item, output the maximum of the input value or `lo`.
* [ClampHigh](operators.md#clamphigh) For each input item, output the minimum of the input value or `hi`.
* [IsOver](operators.md#isover) Output true for each input item greater than `threshold` otherwise false.
* [IsUnder](operators.md#isunder) Output true for each input item less than `threshold` otherwise false.
* [IsAtLeast](operators.md#isatleast) Output true for each input item greater than or equal to `threshold` otherwise false.
* [IsAtMost](operators.md#isatmost) Output true for each input item less than or equal to `threshold` otherwise false.
* [IsPositive](operators.md#ispositive) Output true for each input item that is greater than 0 otherwise false.
* [IsNegative](operators.md#isnegative) Output true for each input item that is less than 0 otherwise false.
* [IsZero](operators.md#iszero) Output true for each input item that is less than 0 otherwise false.
* [Toggle](operators.md#toggle) On the first input, output __!__ `initial` and thereafter output __!__ previous output.
* [Project1](operators.md#project1) For each tuple input item, output the 1st element of the tuple
* [Project2](operators.md#project2) For each tuple input item, output the 2nd element of the tuple
* [Project3](operators.md#project3) For each tuple input item, output the 3rd element of the tuple
* [Counted](operators.md#counted) For each tuple input item, output a tuple with the input item and total number of inputs seen.
* [Map](operators.md#map) For each tuple input item, apply the given `function` to the input and output the returned value.

The signature of function is
```c++
out-type function(const in-type&)
```

* [Map (no copy)](operators.md#map-no-copy) For each tuple input item, call the given `function` with the input and result and output the result.

The signature of function is
```c++
void function(const in-type&, out-type&)
```

* [Scan](operators.md#scan) The scan operator keeps a state of `state-type` which is initially `init`.  For each input item, the given `function` is called with the input item and the current state as arguments.  The returned value is output and becomes the new state.

The function signature of the update function is
```c++
state-type update(const in-type&, const state-type&)
```

* [Scan (no copy)](operators.md#scan-no-copy) The scan operator keeps a state of `state-type` which is initialized by `init` function (optional).  For each input item, the given `update` function is called with the input item and the current state as arguments.  The update can modify the state argument and the new state is the output of the operator.

The function signature of the update function is
```c++
void update(const in-type&, state-type&)
```
and the function signature of the init funciton is
```c++
void init(state-type&)
```


## Aggregation

Operators that aggregate or summarize information from multiple items in output items.

* [Count](operators.md#count) For each item received, output the total number of items received since the start.
* [WinSum](operators.md#winsum) Outputs the sum of a `width` size sliding window over the input stream.
* [SumOf](operators.md#sumof) Outputs the sum of each batch of `count` input items.
* [SumOver](operators.md#sumover) Outputs the sum of each batch of input items received over `millis` milliseconds.
* [WinAverage](operators.md#winaverage) Outputs the average of a `width` size sliding window over the input stream.
* [AverageOf](operators.md#averageof) Outputs the average of each batch of `count` input items.
* [AverageOver](operators.md#averageover) Outputs the average of each batch of input items received over `millis` milliseconds.
* [WinMin](operators.md#winmin) Outputs the minimum of a `width` size sliding window over the input stream.
* [MinOf](operators.md#minof) Outputs the minimum of each batch of `count` input items.
* [MinOver](operators.md#minover) Outputs the minimum of each batch of input items received over `millis` milliseconds.
* [WinMax](operators.md#winmax) Outputs the maximum of a `width` size sliding window over the input stream.
* [MaxOf](operators.md#maxof) Outputs the maximum of each batch of `count` input items.
* [MaxOver](operators.md#maxover) Outputs the maximum of each batch of input items received over `millis` milliseconds.

## String operators

* [Lines](operators.md#lines) Output a stream of lines (strbuf class) from an input stream of characters (char).
* [Format](operators.md#format) Build a formatted string from input items using the `format` template.  Any occurrence of `$1` in the template is replaced by the input value.  Occurrences of `$t` are replaced by the current time as a Unix timestamp (number of seconds since Jan. 1, 1970).
* [Split](operators.md#split) Splits each input string at the given `delimiter` and outputs the resulting strings in a vector.  The C++ types are `strbuf` input and `Vec<strbuf,max-fields>` output.
* [ToString](operators.md#tostring) Converts the input values to a string and outputs it as a `strbuf`
* [ToJson](operators.md#tojson) Converts the input values to a JSON string and outputs it as a `jsons`

## Miscellaneous

* [Iterate](operators.md#iterate) Generates a stream of values from a generator (e.g. `Range`).  The values are generated one every `millis` milliseconds.  If `repeat` is true then the iterater will start over with the 1st value after it reaches the last.  The millis and repeat parameters are optional with defaults 0 and false respectively.
* [Poll](operators.md#poll) Generates a stream of values from an input by polling the current value every `millis` milliseconds.
* [Build](operators.md#build) This operator replaces the scan -> filter -> map pattern with a single operator and also only allocates one result object at creation time.  The operator keeps a state of `state-type` which is initially `init`.  For each input item, the given `function` is called with the input item and the current state and result as reference arguments.  If the function returns true, the current value of result is output otherwise nothing is output.

# Classes

## Generators

Classes that generated data on demand (e.g. via Iterate)

* [Range](classes.md#range) Generates integers in the range `from` to `to` by `increment`.  The increment is optional and defaults to 1.  Combine with `Iterate` to generate a stream.

## Producers

Classes that produce a stream of data

* [Interval](classes.md#interval) Produces integers from 0 on every `millis` milliseconds.
* [Repeat](classes.md#repeat) Produces `value` every cycle.

## Inputs

Classes that receive or sample input data

* [AnalogIn](classes.md#analogin) Reads analog values from an ADC `pin`.
* [BitIn](classes.md#bitin) Reads values from a digital `pin`.
* [GpioSerial](classes.md#gpioserial) Reads serial input from `rxPin` and outputs char values.  A `txPin` is required, and will be put in output mode, but is never set.

## Outputs

Classes that send or output data

* [AnalogOut](classes.md#analogout) Writes values to a PWM `pin`.
* [BitOut](classes.md#bitout) Writes values to a digital `pin`.
* [Console](classes.md#console) *C++:* Writes value to the serial output at the given `baud`.  *Javascript:* Writes value to the console.
* [HttpPost](classes.md#httppost) Sends data to an HTTP endpoint using the HTTP POST method using transport (e.g. an instance of WiFiClient).  Optional headers are provided as n array of strings.
* [MqttPub](classes.md#mqttpub) Publishs data to an MQTT endpoint with QOS 0.  The username, password, and keepAlive parameters are optional.
* [SocketIO](classes.md#socketio) Sends data to a WebSocket via the socket.io module `io` with the event name `event`.

## Consumers

* [Consume](classes.md#consume) Calls the function for each input value with that value as an argument.

# Methods

## Configuration

Functions for device/system configuration.

* [ConnectWifi](methods.md#connectwifi) Connect to a WiFi access point with the given `ssid` and `password`.
* [NetworkTime](methods.md#networktime) Set the date/time using an Internet time server with the given `transport` (e.g. a `WiFiClient`)
* [Sleepy](methods.md#sleepy) The `Sleepy` function enables automatic sleep management on
both AVR and Atmel SAMD (ARM) devices.  When sleep
management is enabled, the scheduler will attempt to put the
CPU in sleep/standby mode whenever there is 2 seconds or
more before the next scheduled event.  Tests on both
architectures showed slightly over 2mA current draw while
sleeping, which is probably a 1/10th or less of your awake
mode usage.

The sleep manager uses the RTC on SAMD architectures and the
WDT on AVR.  `Sleepy` can be called from the beginning or
end of the `app` function to enable sleep management.  If
you set the RTC time during setup, you must do that before
calling `Sleepy`.

The resolution of both the RTC and WDT is 1 second.  Using
even second values in your program schedule will minimize
the amount of time the CPU will be idle between the time it
wakes and the next event happens.

**USB Interaction** - The USB interface tends to interfere
with applications using sleep mode, and it is disabled by
default when you enable sleep management.  I recommend using
the SPI interface for output when using sleep mode.  If you
must use the USB you can call `Sleepy` with the
`SLEEP_MGR_USB` argument to enable the management of the
USB.  In this case, the USB will be detached before sleeping
and reattached upon wake up.


