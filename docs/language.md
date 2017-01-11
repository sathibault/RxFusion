# RxFusion Language

RxFusion is a dataflow language; an RxFusion application is
essentially a definition of a directed graph that describes how data
flows from the inputs through various operations to the outputs.

RxFusion provides a library of input, output and operator components,
and a simple clean syntax to connect them together to form a dataflow
graph.

RxFusion is an embedded domain-specific language (DSL) because it has
a domain-specific syntax but that syntax is 100% compatible with a
general-purpose language.  The beauty of an embedded DSL is that it
does not require any special development tools and developers can use
all their favorite tools to write programs.

RxFusion is available in two flavors: RxFusion C++ (typically used for
MCUs) and RxFusion Javascript (typically used for more advanced MCUs
or CPUs).  The concepts and operators are the same whether you are
using C++ or Javascript, and there are only a few minor syntax
differences due to differences between C++ and Javascript themselves.

## RxFusion C++

An RxFusion C++ application consists of three parts: a preamble,
input/output declarations, and the app function.

### Preamble

The preamble consists of the include statements required by the
application.  At a minimum, this includes the RxFusion header files
but may also require other includes depending on the target platform.

For the Arduino MKR1000, we might have the following which includes
additional headers for the WiFi and real-time clock.

```cpp
#include <WiFi101.h>
#include <RTCZero.h>
#include <RxFusion.h>
```

### Input/output declarations

Inputs and outputs are declared as global variables of one of the
RxFusion provided types.  See the [Classes](classes.md) documentation
of a full list of the available input/output types.

Here's an example of the input/output declarations from an Arduino
application:

```cpp
AnalogIn<int> sensor(A1);

WiFiClient client;
MqttPub broker(client,"broker.hivemq.com",1883,"vtcc8/rxfusion/test","vtcc8");
```

`sensor` is defined as an input of type `AnalogIn<int>` on pin `A1`.
Some of the input/output types are templated like this one because
they support multiple value types. For example, the `AnalogIn` type
used here can produce values in any integer type.  Many of the types
also require or have optional configuration parameters.  In this case,
the pin is a required parameter of the `AnalogIn` type.

`broker` is an example of an output.  The `MqttPub` class publishes
data to a MQTT broker and has several configuration parameters.
Inputs/outputs that communicate with other devices over a network
typically take a transport object as a parameter like the `client`
object in this example.  The transport classes are specific to the
target platform.  Here, the `WiFiClient` class is part of the Arduino
platform and provides transport over a WiFi network.

### App function

The app function primarily consists of dataflow statements, but may
also include any necessary platform-specific initialization.  Here's
an example from the same Arduino application referenced above:

```cpp
void app() {
  ConnectWifi(SSID, PASS);
    
  sensor >> AverageOver<int>(1000) >> Format<int>("{\"sensor\":$1}") >> broker;
}
```

The `ConnectWifi` is required to initialize the WiFi connection, and
the `sensor >> ... >> broker` line is an example of a dataflow statement.

#### Dataflow statements

The core of the application is the dataflow statements in the app
function.  They declare how data should be filtered, transformed,
aggregated and combined to produce output streams from the input
streams.

##### Basic linear dataflow

Many applications consists of one or more simple statements that take
an input, transform or aggregate the data and send it to some output.
Let's start by dissecting the simple dataflow statement from the
example above:

```cpp
  sensor >> AverageOver<int>(1000) >> Format<int>("{\"sensor\":$1}") >> broker;
```

It's meaning is informally understood fairly easily; the input stream
from `sensor` flows through an average operator, a format operator and
is finally sent to an MQTT broker through the `broker` output.

Together the operators and `>>` combinator are used to manipulate
input streams to produce output streams.  There are many operators
available including higher-order operators like filter and map.  See
the [Operators documentation](operators.md) for the full list.

C++ is strongly typed and the operators are templated to accept the
expected input and/or output types.  For example, the `AverageOver`
operator produces an output stream of the same type as the input, so
it has a single type parameter that specifies both the input and
output types.  In this case, `sensor` produces a stream of type `int`,
so that is given as the type parameter to the `AverageOver` operator.

The `Format` operator accepts any type input and has a fixed output
type `xstring`, so it has a single type parameter to specify the input
type.  The `AverageOver<int>` operator produces values of `int`, so
that is given as the input type of the `Format` operator.  The
[Operators documentation](operators.md) indicates what the required
template parameters are.

The `>>` combinator simply connects the output stream of the left
operand to the input stream of the right operand.  There are also
other combinators described below.

#### Forking streams

Sometimes you may want to fork a stream into two different operators.
This can be easily done by creating a temporary variable to represent
the stream to fork and then using it in two dataflow statements.
Here's an example assuming `sensor` is some input, and `console` and
`post` are outputs.

```cpp
  auto data = sensor >> Poll<int>(250);
  data >> console;
  data >> AverageOver<int>(1000) >> post;
```

This represents a dataflow graph like:

```
                   /-> console
  sensor -> Poll ->
                   \-> AverageOver -> post
```

The application polls the sensors every 250 ms and sends the raw value
to the console as well as to an operator that outputs an average every
1000 ms to a web server via the `post` output.

The `sensor >> Poll<int>(250)` expression represents a sub-graph and
has an ugly templated type associated with it, but fortunately we can
avoid dealing with that by using the C++ `auto` keyword.

#### Merging streams

If we can fork streams then we should of course be able to merge them
as well.  RxFusion has three different combinators for merging
streams:

`&` (zip vector) collects values from the input streams until it has
one from each and outputs a vector of the values of each stream.
Vectors hold a fixed number of elements of the same data type, so the
input streams must have the same type.

`^` (zip tuple) collects values from the input streams until it has
one from each and outputs a tuple of the values of each stream.
Tuples can hold elements of different types, so the input streams do
not need to be of the same type.

`+` (union) simple outputs any value received from multiple input
streams on a single output stream.

To use the zip combinator, your input streams should have the same
sampling rate.  Here's an example using `&`:

```cpp
  auto data = sensor >> Poll<int>(250);
  ((data >> Drop<int>(7)) & (data >> WinAverage<int,8>()) >> post;
```

This example uses the `WinAverge` operator which outputs a rolling
average of the last 8 values.  Unlike `AverageOver`, this produces 1
average per input value.  Thus this stream can be combined with the
raw stream using zip.  However, we need to drop the first 7 values
from the raw input stream because `WinAverage` will not start until it
has the first 8 values.

A dataflow graph of this example looks like:

```
                   /-------> Drop \
  sensor -> Poll ->                & -> post
                   \-> WinAverage /
```

#### Higher-order operators

There are a number of higher-order operators that take functions as
arguments such as `Filter`, `Map` and `Scan`.  These are easier to use
thanks to the anonymous functions in C++11.

Here is a `Map` example building on the example of the previous section:

```cpp
  auto data = sensor >> Poll<int>(250);
  auto both = (data >> Drop<int>(7)) & (data >> WinAverage<int,8>());
  both >> Map<Vec<int,2>,int>([](Vec<int,2>& v)->int {
    return v[0] - v[1];
  }) >> post;
```

The `Map` operator takes input values of any type and produces output
values of any type, so it has a template parameter for each.  The zip
operator produces a `Vec<int,2>`, so that's the input type and we are
going to output the difference, so the output type is `int`.

The argument is a function that takes input values and computes output
values.  In C++11, an anonymous function begins with `[]`, followed
by the formal parameters, `->`, the return type and then a function
body.

#### Value types

Beyond scalar types of C++, the values of a stream can be one of the
following RxFusion types:

`Vec<T,size>` - a vector of _size_ elements of type _T_.  The elements
of vectors are accessed using the `[]` index operator with a zero
based index.

`Tuple2<T1,T2>`<br/>
`Tuple3<T1,T2,T3>`<br/>
`Tuple4<T1,T2,T3,T4>` - tuples of 2 to 4 elements of type _T1_, _T2_,
etc. The elements of a tuple are named fields `_1`, `_2`, etc.

`xstring` - a string value.  This is an opaque lazy string type
produced by the `Format` operator.

## RxFusion Javascript

An RxFusion Javascript application is a Node.js application with three
parts: a preamble, input/output declarations, and one or more dataflow
statements.

### Preamble

The preamble consists of the necessary require statements for the
application.  At a minimum, this includes the RxFusion npm module.
There is a generic npm module `rxfusion` as well has platform-specific
modules that include input/output types that a specific to that
platform.

The UDOO Neo has a platform-specific RxFusion module, so on the Neo
the preamble might be:

```javascript
var rx = require('rxfusion-udoo');
```

The name of the variable you use in the require statement is important
when it comes time to write dataflow statements.  The documentation
assumes it is `rx` as shown here.

### Input/output declarations

Inputs and outputs are declared as global variables of one of the
RxFusion provided types using the Javascript `new` operator.  See the
[Classes](classes.md) documentation of a full list of the available
input/output types.

Here's an example of the input/output declarations from a UDOO Neo
application:

```javascript
  var mcu = new Mcu();
  var browser = new SocketIO(io, 'hist');
```

Input and outputs are always created with the `new` operator.  The Neo
is a dual-core chip, and the `Mcu` input type is a Neo-specific input
which receives data from the MCU core.  The `SocketIO` output type
sends data to a WebSocket using the socket.io module.

#### Dataflow statements

The core of the application are the dataflow statements.  They declare
how data should be filtered, transformed, aggregated and combined to
produce output streams from the input streams.

##### rx() magic

RxFusion uses operator overloading to define combinators that are used
to build dataflow statements.  Unfortunately, Javascript does not
support overloaded operators!  Enter rx() ...

When you require the rxfusion module, it returns a function that you
assign to a variable, typically `rx`.  This is a special function that
returns a function which builds dataflow statements and enables the
overloading of the `>>`, `&`, `^`, and `+` operators.

__Every dataflow statement must be wrapped in `rx()(<statement>)`.__

The `rx()` part returns a statement builder function which is applied
to your dataflow statement to interpret the overloaded operators.  The
builder function can only interpret one kind of combinator at a time,
so if you are mixing combinators in a single statement then you need
to wrap the sub-expressions also as in:

```javascript
  rx()(data & rx()(mcu >> Drop(7)) & rx()(mcu >> WinAverage(8)));
```

Here we are zipping together three streams with `&`.  Two of them are
sub-expressions built with `>>`, so those get wrapped in `rx()(...)`.

##### Basic linear dataflow

Many applications consists of one or more simple statements that take
an input, transform or aggregate the data and send it to some output.
Let's start by dissecting a simple dataflow statement:

```javascript
  rx()(mcu >> AverageOf(5) >> Window(16,true) >> browser);
```

Apart from the `rx()` part described above, it's meaning is informally
understood fairly easily; the input stream from the `mcu` input flows
through an average operator, a window operator and is finally sent to
the browser through the `browser` output.

#### Forking streams

Sometimes you may want to fork a stream into two different operators.
This can be easily done by creating a temporary variable to represent
the stream to fork and then using it in two dataflow statements.

```javascript
  var averaged = rx()(mcu >> AverageOf(5));
  rx()(averaged >> console);
  rx()(averaged >> Window(16,true) >> browser);
```

Notice that each statement is wrapped in `rx()(...)`.  This example represents
a dataflow graph like:

```
                     /-> console
  mcu -> AverageOf ->
                     \-> Window -> browser
```

This application averages values coming from the `mcu` input and sends
those directly to the console, but also to a sliding window operator
which sends the last 16 values to the browser.

#### Merging streams

If we can fork streams then we should of course be able to merge them
as well.  RxFusion has three different combinators for merging
streams:

`&`, `^` (zip) collects values from the input streams until it has one
from each and outputs an array of the values of each stream.  There is
no difference between `&` and `^` in the Javascript version.

`+` (union) simple outputs any value received from multiple input
streams on a single output stream.

To use the zip combinator, your input streams should have the same
sampling rate.  Here's an example using `&`:

```javascript
  var raw = rx()(mcu >> Drop(7));
  var avg = rx()(mcu >> WinAverage(8));
  rx()(rx()(raw & avg) >> console);
```

Notice that subexpressions also need to be wrapped in `rx()(...)`.

This example uses the `WinAverge` operator which outputs a rolling
average of the last 8 values.  Unlike `AverageOver`, this produces 1
average per input value.  Thus this stream can be combined with the
raw stream using zip.  However, we need to drop the first 7 values
from the raw input stream because `WinAverage` will not start until it
has the first 8 values.

A dataflow graph of this example looks like:

```
        /-------> Drop \
  mcu ->                & -> console
        \-> WinAverage /
```

#### Higher-order operators

There are a number of higher-order operators that take functions as
arguments such as `Filter`, `Map` and `Scan`.  These are easy to use
thanks to the `=>` anonymous function in ES6.

Here is a `Map` example building on the example of the previous section:

```javascript
  var both = rx()(rx()(mcu >> Drop(7)) & rx()(mcu >> WinAverage(8)));
  both >> Map(v => v[0] - v[1]) >> browser;
```

The argument of `Map` is a function that takes input values and
computes output values.  These are written succinctly in ES6 with the
so called fat arrow operator, `=>`, which takes the parameter list on
the left and the return expression on the right.
