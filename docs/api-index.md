## Filtering

Operations that output a subset of their input items.

[Drop](operators.md#drop) – [Take](operators.md#take) – [Dedup](operators.md#dedup) – [Dedup (expiring)](operators.md#dedup-expiring) – [Debounce](operators.md#debounce) – [Throttle](operators.md#throttle) – [Sample](operators.md#sample) – [Over](operators.md#over) – [Under](operators.md#under) – [AtLeast](operators.md#atleast) – [AtMost](operators.md#atmost) – [Positive](operators.md#positive) – [Negative](operators.md#negative) – [Zero](operators.md#zero) – [True](operators.md#true) – [False](operators.md#false) – [Filter](operators.md#filter)

## Transformation

Operations that transforms each input item to produce an output item.

[Const](operators.md#const) – [Abs](operators.md#abs) – [Scale](operators.md#scale) – [Add](operators.md#add) – [Clamp](operators.md#clamp) – [ClampLow](operators.md#clamplow) – [ClampHigh](operators.md#clamphigh) – [IsOver](operators.md#isover) – [IsUnder](operators.md#isunder) – [IsAtLeast](operators.md#isatleast) – [IsAtMost](operators.md#isatmost) – [IsPositive](operators.md#ispositive) – [IsNegative](operators.md#isnegative) – [IsZero](operators.md#iszero) – [Toggle](operators.md#toggle) – [Project1](operators.md#project1) – [Project2](operators.md#project2) – [Project3](operators.md#project3) – [Counted](operators.md#counted) – [Map](operators.md#map) – [Map (no copy)](operators.md#map-no-copy) – [Scan](operators.md#scan) – [Scan (no copy)](operators.md#scan-no-copy)

## Aggregation

Operators that aggregate or summarize information from multiple items in output items.

[Count](operators.md#count) – [WinSum](operators.md#winsum) – [SumOf](operators.md#sumof) – [SumOver](operators.md#sumover) – [WinAverage](operators.md#winaverage) – [AverageOf](operators.md#averageof) – [AverageOver](operators.md#averageover) – [WinMin](operators.md#winmin) – [MinOf](operators.md#minof) – [MinOver](operators.md#minover) – [WinMax](operators.md#winmax) – [MaxOf](operators.md#maxof) – [MaxOver](operators.md#maxover)

## Miscellaneous

[Iterate](operators.md#iterate) – [Poll](operators.md#poll) – [Format](operators.md#format) – [Lines](operators.md#lines) – [Split](operators.md#split) – [Build](operators.md#build)

## Generators

Classes that generated data on demand (e.g. via Iterate)

[Range](classes.md#range)

## Producers

Classes that produce a stream of data

[Interval](classes.md#interval) – [Repeat](classes.md#repeat)

## Inputs

Classes that receive or sample input data

[AnalogIn](classes.md#analogin) – [BitIn](classes.md#bitin) – [GpioSerial](classes.md#gpioserial)

## Outputs

Classes that send or output data

[AnalogOut](classes.md#analogout) – [BitOut](classes.md#bitout) – [Console](classes.md#console) – [HttpPost](classes.md#httppost) – [MqttPub](classes.md#mqttpub) – [SocketIO](classes.md#socketio)

## Consumers

[Consume](classes.md#consume)

## Configuration

Functions for device/system configuration.

[ConnectWifi](methods.md#connectwifi) – [NetworkTime](methods.md#networktime) – [Sleepy](methods.md#sleepy)

