## Filtering

Operations that output a subset of their input items.

### Drop

![Drop](img/Drop_diag.png)

<img align="left" src="../img/cpp.png">
Drop&lt;*type*&gt;(*n*)
<br>
<img align="left" src="../img/javascript.png">
Drop(*n*)

Output all input items except the first `n`

### Take

![Take](img/Take_diag.png)

<img align="left" src="../img/cpp.png">
Take&lt;*type*&gt;(*n*)
<br>
<img align="left" src="../img/javascript.png">
Take(*n*)

Output only the first `n` input items.

### Dedup

![Dedup](img/Dedup_diag.png)

<img align="left" src="../img/cpp.png">
Dedup&lt;*type*&gt;()
<br>
<img align="left" src="../img/javascript.png">
Dedup()

Output every input item that is not equal to the previous input item.

### Dedup (expiring)

![Dedup (expiring)](img/Dedup-expiring_diag.png)

<img align="left" src="../img/cpp.png">
Dedup&lt;*type*&gt;()
<br>
<img align="left" src="../img/javascript.png">
Dedup()

Output the input item if it is not equal to the previous input or it has been more than `milli` milliseconds since the last output.

### Debounce

![Debounce](img/Debounce_diag.png)

<img align="left" src="../img/cpp.png">
Debounce&lt;*type*&gt;(*millis*)
<br>
<img align="left" src="../img/javascript.png">
Debounce(*millis*)

Outputs each input after `millis` milliseconds if no other input has been received.

### Throttle

![Throttle](img/Throttle_diag.png)

<img align="left" src="../img/cpp.png">
Throttle&lt;*type*&gt;(*millis*)
<br>
<img align="left" src="../img/javascript.png">
Throttle(*millis*)

Rate limits the input stream to *average* at most one value per `millis` milliseconds.  It functions by outputing the first input received (if any) in successive windows of `millis` milliseconds.

### Sample

![Sample](img/Sample_diag.png)

<img align="left" src="../img/cpp.png">
Sample&lt;*type*&gt;(*millis*)
<br>
<img align="left" src="../img/javascript.png">
Sample(*millis*)

Generates a stream of values with the latest value from its input stream every `millis` milliseconds.

### Over

![Over](img/Over_diag.png)

<img align="left" src="../img/cpp.png">
Over&lt;*type*&gt;(*threshold*)
<br>
<img align="left" src="../img/javascript.png">
Over(*threshold*)

Output only input items greater than `threshold`.

### Under

![Under](img/Under_diag.png)

<img align="left" src="../img/cpp.png">
Under&lt;*type*&gt;(*threshold*)
<br>
<img align="left" src="../img/javascript.png">
Under(*threshold*)

Output only input items less than `threshold`.

### AtLeast

![AtLeast](img/AtLeast_diag.png)

<img align="left" src="../img/cpp.png">
AtLeast&lt;*type*&gt;(*threshold*)
<br>
<img align="left" src="../img/javascript.png">
AtLeast(*threshold*)

Output only input items greater than or equal to `threshold`.

### AtMost

![AtMost](img/AtMost_diag.png)

<img align="left" src="../img/cpp.png">
AtMost&lt;*type*&gt;(*threshold*)
<br>
<img align="left" src="../img/javascript.png">
AtMost(*threshold*)

Output only input items less than or equal to `threshold`.

### Positive

![Positive](img/Positive_diag.png)

<img align="left" src="../img/cpp.png">
Positive&lt;*type*&gt;()
<br>
<img align="left" src="../img/javascript.png">
Positive()

Output only input items that are greater than 0.

### Negative

![Negative](img/Negative_diag.png)

<img align="left" src="../img/cpp.png">
Negative&lt;*type*&gt;()
<br>
<img align="left" src="../img/javascript.png">
Negative()

Output only input items that are less than 0.

### Zero

![Zero](img/Zero_diag.png)

<img align="left" src="../img/cpp.png">
Zero&lt;*type*&gt;()
<br>
<img align="left" src="../img/javascript.png">
Zero()

Output only input items that are less than 0.

### True

![True](img/True_diag.png)

<img align="left" src="../img/cpp.png">
True&lt;*type*&gt;()
<br>
<img align="left" src="../img/javascript.png">
True()

Output only input items equal to true.

### False

![False](img/False_diag.png)

<img align="left" src="../img/cpp.png">
False&lt;*type*&gt;()
<br>
<img align="left" src="../img/javascript.png">
False()

Output only input items equal to false.

### Filter

![Filter](img/Filter_diag.png)

<img align="left" src="../img/cpp.png">
Filter&lt;*type*&gt;(*function*)
<br>
<img align="left" src="../img/javascript.png">
Filter(*function*)

For each input item, apply the given function and output the input if the function returns true.

## Transformation

Operations that transforms each input item to produce an output item.

### Const

![Const](img/Const_diag.png)

<img align="left" src="../img/cpp.png">
Const&lt;*type*&gt;(*value*)
<br>
<img align="left" src="../img/javascript.png">
Const(*value*)

For each input item, output `value`.

### Scale

![Scale](img/Scale_diag.png)

<img align="left" src="../img/cpp.png">
Scale&lt;*type*&gt;(*k*)
<br>
<img align="left" src="../img/javascript.png">
Scale(*k*)

For each input item, output `k` __×__ input value.

### Add

![Add](img/Add_diag.png)

<img align="left" src="../img/cpp.png">
Add&lt;*type*&gt;(*b*)
<br>
<img align="left" src="../img/javascript.png">
Add(*b*)

For each input item, output `b` __+__ input value.

### Clamp

![Clamp](img/Clamp_diag.png)

<img align="left" src="../img/cpp.png">
Clamp&lt;*type*&gt;(*lo*, *hi*)
<br>
<img align="left" src="../img/javascript.png">
Clamp(*lo*, *hi*)

For each input item, output `lo` if the input is less than `lo`, `hi` if the input more than `hi`, or the input value if it is between `lo` and `hi`.

### ClampLow

![ClampLow](img/ClampLow_diag.png)

<img align="left" src="../img/cpp.png">
ClampLow&lt;*type*&gt;(*lo*)
<br>
<img align="left" src="../img/javascript.png">
ClampLow(*lo*)

For each input item, output the maximum of the input value or `lo`.

### ClampHigh

![ClampHigh](img/ClampHigh_diag.png)

<img align="left" src="../img/cpp.png">
ClampHigh&lt;*type*&gt;(*hi*)
<br>
<img align="left" src="../img/javascript.png">
ClampHigh(*hi*)

For each input item, output the minimum of the input value or `hi`.

### IsOver

![IsOver](img/IsOver_diag.png)

<img align="left" src="../img/cpp.png">
IsOver&lt;*type*&gt;(*threshold*)
<br>
<img align="left" src="../img/javascript.png">
IsOver(*threshold*)

Output true for each input item greater than `threshold` otherwise false.

### IsUnder

![IsUnder](img/IsUnder_diag.png)

<img align="left" src="../img/cpp.png">
IsUnder&lt;*type*&gt;(*threshold*)
<br>
<img align="left" src="../img/javascript.png">
IsUnder(*threshold*)

Output true for each input item less than `threshold` otherwise false.

### IsAtLeast

![IsAtLeast](img/IsAtLeast_diag.png)

<img align="left" src="../img/cpp.png">
IsAtLeast&lt;*type*&gt;(*threshold*)
<br>
<img align="left" src="../img/javascript.png">
IsAtLeast(*threshold*)

Output true for each input item greater than or equal to `threshold` otherwise false.

### IsAtMost

![IsAtMost](img/IsAtMost_diag.png)

<img align="left" src="../img/cpp.png">
IsAtMost&lt;*type*&gt;(*threshold*)
<br>
<img align="left" src="../img/javascript.png">
IsAtMost(*threshold*)

Output true for each input item less than or equal to `threshold` otherwise false.

### IsPositive

![IsPositive](img/IsPositive_diag.png)

<img align="left" src="../img/cpp.png">
IsPositive&lt;*type*&gt;()
<br>
<img align="left" src="../img/javascript.png">
IsPositive()

Output true for each input item that is greater than 0 otherwise false.

### IsNegative

![IsNegative](img/IsNegative_diag.png)

<img align="left" src="../img/cpp.png">
IsNegative&lt;*type*&gt;()
<br>
<img align="left" src="../img/javascript.png">
IsNegative()

Output true for each input item that is less than 0 otherwise false.

### IsZero

![IsZero](img/IsZero_diag.png)

<img align="left" src="../img/cpp.png">
IsZero&lt;*type*&gt;()
<br>
<img align="left" src="../img/javascript.png">
IsZero()

Output true for each input item that is less than 0 otherwise false.

### Toggle

![Toggle](img/Toggle_diag.png)

<img align="left" src="../img/cpp.png">
Toggle&lt;*in-type*,*out-type*&gt;(*initial*)
<br>
<img align="left" src="../img/javascript.png">
Toggle(*initial*)

On the first input, output __!__ `initial` and thereafter output __!__ previous output.

### Project1

![Project1](img/Project1_diag.png)

<img align="left" src="../img/cpp.png">
Project1&lt;*type*&gt;()
<br>
<img align="left" src="../img/javascript.png">
Project1()

For each tuple input item, output the 1st element of the tuple

### Project2

![Project2](img/Project2_diag.png)

<img align="left" src="../img/cpp.png">
Project2&lt;*type*&gt;()
<br>
<img align="left" src="../img/javascript.png">
Project2()

For each tuple input item, output the 2nd element of the tuple

### Project3

![Project3](img/Project3_diag.png)

<img align="left" src="../img/cpp.png">
Project3&lt;*type*&gt;()
<br>
<img align="left" src="../img/javascript.png">
Project3()

For each tuple input item, output the 3rd element of the tuple

### Counted

![Counted](img/Counted_diag.png)

<img align="left" src="../img/cpp.png">
Counted&lt;*in-type*,*counter-type*&gt;()
<br>
<img align="left" src="../img/javascript.png">
Counted()

For each tuple input item, output a tuple with the input item and total number of inputs seen.

### Map

![Map](img/Map_diag.png)

<img align="left" src="../img/cpp.png">
Map&lt;*in-type*,*out-type*&gt;(*function*)
<br>
<img align="left" src="../img/javascript.png">
Map(*function*)

For each tuple input item, apply the given `function` to the input and output the returned value.

### Scan

![Scan](img/Scan_diag.png)

<img align="left" src="../img/cpp.png">
Scan&lt;*in-type*,*state-type*&gt;(*function*, *init*)
<br>
<img align="left" src="../img/javascript.png">
Scan(*function*, *init*)

The scan operator keeps a state of `state-type` which is initially `init`.  For each input item, the given `function` is called with the input item and the current state as arguments.  The returned value is output and becomes the new state.

## Aggregation

Operators that aggregate or summarize information from multiple items in output items.

### Count

![Count](img/Count_diag.png)

<img align="left" src="../img/cpp.png">
Count&lt;*type*&gt;()
<br>
<img align="left" src="../img/javascript.png">
Count()

For each item received, output the total number of items received since the start.

### WinSum

![WinSum](img/WinSum_diag.png)

<img align="left" src="../img/cpp.png">
WinSum&lt;*type*,*width*&gt;()
<br>
<img align="left" src="../img/javascript.png">
WinSum(*width*)

Outputs the sum of a `width` size sliding window over the input stream.

### SumOf

![SumOf](img/SumOf_diag.png)

<img align="left" src="../img/cpp.png">
SumOf&lt;*type*,*count*&gt;()
<br>
<img align="left" src="../img/javascript.png">
SumOf(*count*)

Outputs the sum of each batch of `count` input items.

### SumOver

![SumOver](img/SumOver_diag.png)

<img align="left" src="../img/cpp.png">
SumOver&lt;*type*&gt;(*millis*)
<br>
<img align="left" src="../img/javascript.png">
SumOver(*millis*)

Outputs the sum of each batch of input items received over `millis` milliseconds.

### WinAverage

![WinAverage](img/WinAverage_diag.png)

<img align="left" src="../img/cpp.png">
WinAverage&lt;*type*,*width*&gt;()
<br>
<img align="left" src="../img/javascript.png">
WinAverage(*width*)

Outputs the average of a `width` size sliding window over the input stream.

### AverageOf

![AverageOf](img/AverageOf_diag.png)

<img align="left" src="../img/cpp.png">
AverageOf&lt;*type*,*count*&gt;()
<br>
<img align="left" src="../img/javascript.png">
AverageOf(*count*)

Outputs the average of each batch of `count` input items.

### AverageOver

![AverageOver](img/AverageOver_diag.png)

<img align="left" src="../img/cpp.png">
AverageOver&lt;*type*&gt;(*millis*)
<br>
<img align="left" src="../img/javascript.png">
AverageOver(*millis*)

Outputs the average of each batch of input items received over `millis` milliseconds.

### WinMin

![WinMin](img/WinMin_diag.png)

<img align="left" src="../img/cpp.png">
WinMin&lt;*type*,*width*&gt;()
<br>
<img align="left" src="../img/javascript.png">
WinMin(*width*)

Outputs the minimum of a `width` size sliding window over the input stream.

### MinOf

![MinOf](img/MinOf_diag.png)

<img align="left" src="../img/cpp.png">
MinOf&lt;*type*,*count*&gt;()
<br>
<img align="left" src="../img/javascript.png">
MinOf(*count*)

Outputs the minimum of each batch of `count` input items.

### MinOver

![MinOver](img/MinOver_diag.png)

<img align="left" src="../img/cpp.png">
MinOver&lt;*type*&gt;(*millis*)
<br>
<img align="left" src="../img/javascript.png">
MinOver(*millis*)

Outputs the minimum of each batch of input items received over `millis` milliseconds.

### WinMax

![WinMax](img/WinMax_diag.png)

<img align="left" src="../img/cpp.png">
WinMax&lt;*type*,*width*&gt;()
<br>
<img align="left" src="../img/javascript.png">
WinMax(*width*)

Outputs the maximum of a `width` size sliding window over the input stream.

### MaxOf

![MaxOf](img/MaxOf_diag.png)

<img align="left" src="../img/cpp.png">
MaxOf&lt;*type*,*count*&gt;()
<br>
<img align="left" src="../img/javascript.png">
MaxOf(*count*)

Outputs the maximum of each batch of `count` input items.

### MaxOver

![MaxOver](img/MaxOver_diag.png)

<img align="left" src="../img/cpp.png">
MaxOver&lt;*type*&gt;(*millis*)
<br>
<img align="left" src="../img/javascript.png">
MaxOver(*millis*)

Outputs the maximum of each batch of input items received over `millis` milliseconds.

## Miscellaneous

### Range

<img align="left" src="../img/cpp.png">
Range&lt;*type*&gt;(*from*, *to*, *increment*)
<br>
<img align="left" src="../img/javascript.png">
Range(*from*, *to*, *increment*)

Generates integers in the range `from` to `to` by `increment`.  The increment is optional and defaults to 1.  Combine with `Iterate` to generate a stream.

### Iterate

<img align="left" src="../img/cpp.png">
Iterate&lt;*type*&gt;(*millis*, *repeat*)
<br>
<img align="left" src="../img/javascript.png">
Iterate(*millis*, *repeat*)

Generates a stream of values from a generator (e.g. `Range`).  The values are generated one every `millis` milliseconds.  If `repeat` is true then the iterater will start over with the 1st value after it reaches the last.  The millis and repeat parameters are optional with defaults 0 and false respectively.

### Poll

<img align="left" src="../img/cpp.png">
Poll&lt;*type*&gt;(*millis*)
<br>
<img align="left" src="../img/javascript.png">
Poll(*millis*)

Generates a stream of values from an input by polling the current value every `millis` milliseconds.

### Format

<img align="left" src="../img/cpp.png">
Format&lt;*type*&gt;(*format*)
<br>
<img align="left" src="../img/javascript.png">
N/A

Build a formatted string from input items using the `format` template.  Any occurrence of `$1` in the template is replaced by the input value.  Occurrences of `$t` are replaced by the current time as a Unix timestamp (number of seconds since Jan. 1, 1970).

### Lines

<img align="left" src="../img/cpp.png">
Lines()
<br>
<img align="left" src="../img/javascript.png">
N/A

Output a stream of lines (strbuf class) from an input stream of characters (char).

### Build

<img align="left" src="../img/cpp.png">
Build&lt;*in-type*,*state-type*,*result-type*&gt;(*function*, *init*)
<br>
<img align="left" src="../img/javascript.png">
N/A

This operator replaces the scan -> filter -> map pattern with a single operator and also only allocates one result object at creation time.  The operator keeps a state of `state-type` which is initially `init`.  For each input item, the given `function` is called with the input item and the current state and result as reference arguments.  If the function returns true, the current value of result is output otherwise nothing is output.

