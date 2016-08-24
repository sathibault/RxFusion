## Aggregation

Operators that aggregate or summarize information from multiple items in output items.

#### Count

<img src="img/Count_diag.png">

<img align="left" src="img/cpp.png">
Count&lt;*class*&gt;()
<br>
<img align="left" src="img/javascript.png">
Count()

For each item received, output the total number of items received since the start.

#### WinSum

<img src="img/WinSum_diag.png">

<img align="left" src="img/cpp.png">
WinSum&lt;*type*,*width*&gt;()
<br>
<img align="left" src="img/javascript.png">
WinSum(*width*)

Outputs the sum of a `width` size sliding window over the input stream.

#### SumOf

<img src="img/SumOf_diag.png">

<img align="left" src="img/cpp.png">
SumOf&lt;*type*,*count*&gt;()
<br>
<img align="left" src="img/javascript.png">
SumOf(*count*)

Outputs the sum of each batch of count input items.

#### SumOver

<img src="img/SumOver_diag.png">

<img align="left" src="img/cpp.png">
SumOver&lt;*type*&gt;(*millis*)
<br>
<img align="left" src="img/javascript.png">
SumOver(*millis*)

Outputs the sum of each batch of count input items.

