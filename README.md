# RxFusion

A Reactive programming framework for IoT from edge to cloud.

## Project goals

* Lightweight reactive C++ extensions for embedded devices
* Consistent reactive extensions across C++ and Javascript
* Multi-node and polyglotinous programming (i.e. connecting edge, gateway, and data center devices)
* Public repository and management tools for reactive components:
  * Sensor data acquisition components
  * Data transformation and aggregation library
  * Upstream connectors to IoT cloud platforms

## Roadmap

* 2016 Q2
  * Concept development
  * C++ core component development
  * Proof-of-concept Arduino/ARM
* 2016 Q3
  * C++ core components alpha release
  * JS development
  * Cloud connection development
  * PlatformIO integration

## Preview

RxFusion uses a natural and readable syntax for data flow programming.
The following example illustrates the C++ syntax for processing
accelerometer data:

```c++
  // Extract x-axis and calculate a moving average with a window of 4.
  auto smooth = accelerometer >> Index<Vec<short>>(0) >> Average<short,4>();

  // Produce tuples of original value, min and max over a window of 8.
  auto box = smooth & (smooth >> Min<short,8>()) & (smooth >> Max<short,9>());

  // Custom function using C++ 2011 anonymous function uses the original
  // value with the min/max to detect rising and falling edges.
  auto edges = box >> Map<xyz,int>([](xyz& vec) -> int {
    int pos = (vec._1 - vec._2) > 30;
    int neg = (vec._3 - vec._1) > 30;
    return pos - neg;
  })
```

To see what's going on here visually, take a look at the RxFusion Lab
[demonstration video](https://www.youtube.com/watch?v=mB-89ANRdmU).

Javascript code uses the same data flow syntax without the typing.

## Concepts

These are the basic concepts of RxFusion based heavily on reactive extensions.

### Data Sources

There are two basic sources of data:

* __Observables__ - these observables generate a non-ending stream of
data regardless of whether there are any observers or not.  Sensors are
observables.
* __Generators__ - these produce a stream of data on demand.  Examples
include range generators and collections.  Generators are connected to
Feeders to make them observables.  __Feeders__ make generators into
observables producing data according to some schedule (e.g. one item
per cycle).

### Operators

Operators are used to transform, filter, or combine data from
observables or other operators.

### Consumers

Data originates from an observable, passes through one or more
operators and is finally consumed by a consumer.  There are three
types of consumers:

* __Subscribers__ - these consist of custom processing provided by a
user-defined consumer function and an optional close function.
* __Stores__ - these store data in an object that can be used later as
a generator in a second pipeline.

### Memory management

The general rule is if you create the object then you are responsible
for deleting it.  If the object is returned by an RxFusion function
then the framework is responsible for deleting it.

_Observables_, _generators_ and _stores_ are created using their type
constructor and the application is responsible for deletion.

_Operators_, _feeders_, and _subscribers_ are created via RxFusion
functions and thus automatically deleted by the framework.
