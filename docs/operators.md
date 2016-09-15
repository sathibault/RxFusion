## Filtering

Operations that output a subset of their input items.

### Drop

<img src="img/Drop_diag.png">

<img align="left" src="img/cpp.png">
Drop&lt;*type*&gt;(*n*)
<br>
<img align="left" src="img/javascript.png">
Drop(*n*)

Output all input items except the first `n`

### Take

<img src="img/Take_diag.png">

<img align="left" src="img/cpp.png">
Take&lt;*type*&gt;(*n*)
<br>
<img align="left" src="img/javascript.png">
Take(*n*)

Output only the first `n` input items.

### Dedup

<img src="img/Dedup_diag.png">

<img align="left" src="img/cpp.png">
Dedup&lt;*type*&gt;()
<br>
<img align="left" src="img/javascript.png">
Dedup()

Output every input item that is not equal to the previous input item.

### Dedup (expiring)

<img src="img/Dedup (expiring)_diag.png">

<img align="left" src="img/cpp.png">
Dedup (expiring)&lt;*type*&gt;()
<br>
<img align="left" src="img/javascript.png">
Dedup (expiring)()

Output the input item if it is not equal to the previous input or it has been more than `milli` milliseconds since the last output.

### Debounce

<img src="img/Debounce_diag.png">

<img align="left" src="img/cpp.png">
Debounce&lt;*type*&gt;(*millis*)
<br>
<img align="left" src="img/javascript.png">
Debounce(*millis*)

Outputs each input after `millis` milliseconds if no other input has been received.

### Sample

<img src="img/Sample_diag.png">

<img align="left" src="img/cpp.png">
Sample&lt;*type*&gt;(*millis*)
<br>
<img align="left" src="img/javascript.png">
Sample(*millis*)

Generates a stream of values with the latest value from its input stream every `millis` milliseconds.

### Over

<img src="img/Over_diag.png">

<img align="left" src="img/cpp.png">
Over&lt;*type*&gt;(*threshold*)
<br>
<img align="left" src="img/javascript.png">
Over(*threshold*)

Output only input items greater than `threshold`.

### Under

<img src="img/Under_diag.png">

<img align="left" src="img/cpp.png">
Under&lt;*type*&gt;(*threshold*)
<br>
<img align="left" src="img/javascript.png">
Under(*threshold*)

Output only input items less than `threshold`.

### AtLeast

<img src="img/AtLeast_diag.png">

<img align="left" src="img/cpp.png">
AtLeast&lt;*type*&gt;(*threshold*)
<br>
<img align="left" src="img/javascript.png">
AtLeast(*threshold*)

Output only input items greater than or equal to `threshold`.

### AtMost

<img src="img/AtMost_diag.png">

<img align="left" src="img/cpp.png">
AtMost&lt;*type*&gt;(*threshold*)
<br>
<img align="left" src="img/javascript.png">
AtMost(*threshold*)

Output only input items less than or equal to `threshold`.

### Positive

<img src="img/Positive_diag.png">

<img align="left" src="img/cpp.png">
Positive&lt;*type*&gt;()
<br>
<img align="left" src="img/javascript.png">
Positive()

Output only input items that are greater than 0.

### Negative

<img src="img/Negative_diag.png">

<img align="left" src="img/cpp.png">
Negative&lt;*type*&gt;()
<br>
<img align="left" src="img/javascript.png">
Negative()

Output only input items that are less than 0.

### Zero

<img src="img/Zero_diag.png">

<img align="left" src="img/cpp.png">
Zero&lt;*type*&gt;()
<br>
<img align="left" src="img/javascript.png">
Zero()

Output only input items that are less than 0.

### True

<img src="img/True_diag.png">

<img align="left" src="img/cpp.png">
True&lt;*type*&gt;()
<br>
<img align="left" src="img/javascript.png">
True()

Output only input items equal to true.

### False

<img src="img/False_diag.png">

<img align="left" src="img/cpp.png">
False&lt;*type*&gt;()
<br>
<img align="left" src="img/javascript.png">
False()

Output only input items equal to false.

### Filter

<img src="img/Filter_diag.png">

<img align="left" src="img/cpp.png">
Filter&lt;*type*&gt;(*function*)
<br>
<img align="left" src="img/javascript.png">
Filter(*function*)

For each input item, apply the given function and output the input if the function returns true.

## Transformation

Operations that transforms each input item to produce an output item.

### Const

<img src="img/Const_diag.png">

<img align="left" src="img/cpp.png">
Const&lt;*type*&gt;(*value*)
<br>
<img align="left" src="img/javascript.png">
Const(*value*)

For each input item, output `value`.

### Scale

<img src="img/Scale_diag.png">

<img align="left" src="img/cpp.png">
Scale&lt;*type*&gt;(*k*)
<br>
<img align="left" src="img/javascript.png">
Scale(*k*)

For each input item, output `k` __×__ input value.

### Add

<img src="img/Add_diag.png">

<img align="left" src="img/cpp.png">
Add&lt;*type*&gt;(*b*)
<br>
<img align="left" src="img/javascript.png">
Add(*b*)

For each input item, output `b` __+__ input value.

### Clamp

<img src="img/Clamp_diag.png">

<img align="left" src="img/cpp.png">
Clamp&lt;*type*&gt;(*lo*, *hi*)
<br>
<img align="left" src="img/javascript.png">
Clamp(*lo*, *hi*)

For each input item, output `lo` if the input is less than `lo`, `hi` if the input more than `hi`, or the input value if it is between `lo` and `hi`.

### ClampLow

<img src="img/ClampLow_diag.png">

<img align="left" src="img/cpp.png">
ClampLow&lt;*type*&gt;(*lo*)
<br>
<img align="left" src="img/javascript.png">
ClampLow(*lo*)

For each input item, output the maximum of the input value or `lo`.

### ClampHigh

<img src="img/ClampHigh_diag.png">

<img align="left" src="img/cpp.png">
ClampHigh&lt;*type*&gt;(*hi*)
<br>
<img align="left" src="img/javascript.png">
ClampHigh(*hi*)

For each input item, output the minimum of the input value or `hi`.

### IsOver

<img src="img/IsOver_diag.png">

<img align="left" src="img/cpp.png">
IsOver&lt;*type*&gt;(*threshold*)
<br>
<img align="left" src="img/javascript.png">
IsOver(*threshold*)

Output true for each input item greater than `threshold` otherwise false.

### IsUnder

<img src="img/IsUnder_diag.png">

<img align="left" src="img/cpp.png">
IsUnder&lt;*type*&gt;(*threshold*)
<br>
<img align="left" src="img/javascript.png">
IsUnder(*threshold*)

Output true for each input item less than `threshold` otherwise false.

### IsAtLeast

<img src="img/IsAtLeast_diag.png">

<img align="left" src="img/cpp.png">
IsAtLeast&lt;*type*&gt;(*threshold*)
<br>
<img align="left" src="img/javascript.png">
IsAtLeast(*threshold*)

Output true for each input item greater than or equal to `threshold` otherwise false.

### IsAtMost

<img src="img/IsAtMost_diag.png">

<img align="left" src="img/cpp.png">
IsAtMost&lt;*type*&gt;(*threshold*)
<br>
<img align="left" src="img/javascript.png">
IsAtMost(*threshold*)

Output true for each input item less than or equal to `threshold` otherwise false.

### IsPositive

<img src="img/IsPositive_diag.png">

<img align="left" src="img/cpp.png">
IsPositive&lt;*type*&gt;()
<br>
<img align="left" src="img/javascript.png">
IsPositive()

Output true for each input item that is greater than 0 otherwise false.

### IsNegative

<img src="img/IsNegative_diag.png">

<img align="left" src="img/cpp.png">
IsNegative&lt;*type*&gt;()
<br>
<img align="left" src="img/javascript.png">
IsNegative()

Output true for each input item that is less than 0 otherwise false.

### IsZero

<img src="img/IsZero_diag.png">

<img align="left" src="img/cpp.png">
IsZero&lt;*type*&gt;()
<br>
<img align="left" src="img/javascript.png">
IsZero()

Output true for each input item that is less than 0 otherwise false.

### Toggle

<img src="img/Toggle_diag.png">

<img align="left" src="img/cpp.png">
Toggle&lt;*in-type*,*out-type*&gt;(*initial*)
<br>
<img align="left" src="img/javascript.png">
Toggle(*initial*)

On the first input, output __!__ `initial` and thereafter output __!__ previous output.

### Project1

<img src="img/Project1_diag.png">

<img align="left" src="img/cpp.png">
Project1&lt;*type*&gt;()
<br>
<img align="left" src="img/javascript.png">
Project1()

For each tuple input item, output the 1st element of the tuple

### Project2

<img src="img/Project2_diag.png">

<img align="left" src="img/cpp.png">
Project2&lt;*type*&gt;()
<br>
<img align="left" src="img/javascript.png">
Project2()

For each tuple input item, output the 2nd element of the tuple

### Project3

<img src="img/Project3_diag.png">

<img align="left" src="img/cpp.png">
Project3&lt;*type*&gt;()
<br>
<img align="left" src="img/javascript.png">
Project3()

For each tuple input item, output the 3rd element of the tuple

### Counted

<img src="img/Counted_diag.png">

<img align="left" src="img/cpp.png">
Counted&lt;*in-type*,*counter-type*&gt;()
<br>
<img align="left" src="img/javascript.png">
Counted()

For each tuple input item, output a tuple with the input item and total number of inputs seen.

### Map

<img src="img/Map_diag.png">

<img align="left" src="img/cpp.png">
Map&lt;*in-type*,*out-type*&gt;(*function*)
<br>
<img align="left" src="img/javascript.png">
Map(*function*)

For each tuple input item, apply the given `function` to the input and output the returned value.

### Scan

<img src="img/Scan_diag.png">

<img align="left" src="img/cpp.png">
Scan&lt;*in-type*,*state-type*&gt;(*function*, *init*)
<br>
<img align="left" src="img/javascript.png">
Scan(*function*, *init*)

The scan operator keeps a state of `state-type` which is initially `init`.  For each input item, the given `function` is called with the input item and the current state as arguments.  The returned value is output and becomes the new state.

## Aggregation

Operators that aggregate or summarize information from multiple items in output items.

### Count

<img src="img/Count_diag.png">

<img align="left" src="img/cpp.png">
Count&lt;*type*&gt;()
<br>
<img align="left" src="img/javascript.png">
Count()

For each item received, output the total number of items received since the start.

### WinSum

<img src="img/WinSum_diag.png">

<img align="left" src="img/cpp.png">
WinSum&lt;*type*,*width*&gt;()
<br>
<img align="left" src="img/javascript.png">
WinSum(*width*)

Outputs the sum of a `width` size sliding window over the input stream.

### SumOf

<img src="img/SumOf_diag.png">

<img align="left" src="img/cpp.png">
SumOf&lt;*type*,*count*&gt;()
<br>
<img align="left" src="img/javascript.png">
SumOf(*count*)

Outputs the sum of each batch of `count` input items.

### SumOver

<img src="img/SumOver_diag.png">

<img align="left" src="img/cpp.png">
SumOver&lt;*type*&gt;(*millis*)
<br>
<img align="left" src="img/javascript.png">
SumOver(*millis*)

Outputs the sum of each batch of input items received over `millis` milliseconds.

### WinAverage

<img src="img/WinAverage_diag.png">

<img align="left" src="img/cpp.png">
WinAverage&lt;*type*,*width*&gt;()
<br>
<img align="left" src="img/javascript.png">
WinAverage(*width*)

Outputs the average of a `width` size sliding window over the input stream.

### AverageOf

<img src="img/AverageOf_diag.png">

<img align="left" src="img/cpp.png">
AverageOf&lt;*type*,*count*&gt;()
<br>
<img align="left" src="img/javascript.png">
AverageOf(*count*)

Outputs the average of each batch of `count` input items.

### AverageOver

<img src="img/AverageOver_diag.png">

<img align="left" src="img/cpp.png">
AverageOver&lt;*type*&gt;(*millis*)
<br>
<img align="left" src="img/javascript.png">
AverageOver(*millis*)

Outputs the average of each batch of input items received over `millis` milliseconds.

### WinMin

<img src="img/WinMin_diag.png">

<img align="left" src="img/cpp.png">
WinMin&lt;*type*,*width*&gt;()
<br>
<img align="left" src="img/javascript.png">
WinMin(*width*)

Outputs the minimum of a `width` size sliding window over the input stream.

### MinOf

<img src="img/MinOf_diag.png">

<img align="left" src="img/cpp.png">
MinOf&lt;*type*,*count*&gt;()
<br>
<img align="left" src="img/javascript.png">
MinOf(*count*)

Outputs the minimum of each batch of `count` input items.

### MinOver

<img src="img/MinOver_diag.png">

<img align="left" src="img/cpp.png">
MinOver&lt;*type*&gt;(*millis*)
<br>
<img align="left" src="img/javascript.png">
MinOver(*millis*)

Outputs the minimum of each batch of input items received over `millis` milliseconds.

### WinMax

<img src="img/WinMax_diag.png">

<img align="left" src="img/cpp.png">
WinMax&lt;*type*,*width*&gt;()
<br>
<img align="left" src="img/javascript.png">
WinMax(*width*)

Outputs the maximum of a `width` size sliding window over the input stream.

### MaxOf

<img src="img/MaxOf_diag.png">

<img align="left" src="img/cpp.png">
MaxOf&lt;*type*,*count*&gt;()
<br>
<img align="left" src="img/javascript.png">
MaxOf(*count*)

Outputs the maximum of each batch of `count` input items.

### MaxOver

<img src="img/MaxOver_diag.png">

<img align="left" src="img/cpp.png">
MaxOver&lt;*type*&gt;(*millis*)
<br>
<img align="left" src="img/javascript.png">
MaxOver(*millis*)

Outputs the maximum of each batch of input items received over `millis` milliseconds.

## Miscellaneous

### Range

<img align="left" src="img/cpp.png">
Range&lt;*type*&gt;(*from*, *to*, *increment*)
<br>
<img align="left" src="img/javascript.png">
Range(*from*, *to*, *increment*)

Generates integers in the range `from` to `to` by `increment`.  The increment is optional and defaults to 1.  Combine with `Iterate` to generate a stream.

### Iterate

<img align="left" src="img/cpp.png">
Iterate&lt;*type*&gt;(*millis*, *repeat*)
<br>
<img align="left" src="img/javascript.png">
Iterate(*millis*, *repeat*)

Generates a stream of values from a generator (e.g. `Range`).  The values are generated one every `millis` milliseconds.  If `repeat` is true then the iterater will start over with the 1st value after it reaches the last.  The millis and repeat parameters are optional with defaults 0 and false respectively.

### Poll

<img align="left" src="img/cpp.png">
Poll&lt;*type*&gt;(*millis*)
<br>
<img align="left" src="img/javascript.png">
Poll(*millis*)

Generates a stream of values from an input by polling the current value every `millis` milliseconds.

### Format

<img align="left" src="img/cpp.png">
Format&lt;*type*&gt;(*format*)
<br>
<img align="left" src="img/javascript.png">
Format(*format*)

Build a formatted string from input items using the `format` template.  Any occurrence of `$1` in the template is replaced by the input value.  Occurrences of `$t` are replaced by the current time as a Unix timestamp (number of seconds since Jan. 1, 1970).

