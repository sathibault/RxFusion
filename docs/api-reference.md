## Filtering

Operations that output a subset of their input items.

* [Drop](operators.md#drop) Output all input items except the first `n`
* [Take](operators.md#take) Output only the first `n` input items.
* [Dedup](operators.md#dedup) Output every input item that is not equal to the previous input item.
* [Dedup (expiring)](operators.md#dedup-expiring) Output the input item if it is not equal to the previous input or it has been more than `milli` milliseconds since the last output.
* [Debounce](operators.md#debounce) Outputs each input after `millis` milliseconds if no other input has been received.
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
* [Scan](operators.md#scan) The scan operator keeps a state of `state-type` which is initially `init`.  For each input item, the given `function` is called with the input item and the current state as arguments.  The returned value is output and becomes the new state.

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
