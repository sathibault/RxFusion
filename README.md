# RxFusion

A ReactiveX framework for IoT from edge to cloud.

## Project goals

* Lightweight reactive C++ extensions for embedded devices
* Consistent reactive extensions across C++ and Javascript
* Multi-node and polyglotinous programming (i.e. connecting edge, gateway, and data center devices)
* Public repository and management tools for reactive components:
  * Sensor data acquisition components
  * Data transformation and aggregation library
  * Upstream connectors to IoT cloud platforms

## Preview

RxFusion uses a natural and readable syntax for data flow programming.
The following example illustrates the C++ syntax for processing
accelerometer data:

```
  // Extract x-axis and calculate a moving average with a window of 4.
  auto smooth = accelerometer >> Index<Vec<short>>(0) >> Average<short,4>();

  // Produce tuples of original value, min and max over a window of 8.
  auto box = smooth & (smooth >> Min<short,8>()) & (smooth >> Max<short,9>());

  // Custom function using C++ 2011 anyomous function uses the original
  // value with the min/max to detect rising and falling edges.
  auto edges = Map<xyz,int>([](xyz& vec) -> int {
    int pos = (vec._1 - vec._2) > 30;
    int neg = (vec._3 - vec._1) > 30;
    return pos - neg;
  })
```

To see what's going on here visually, take a look at the RxFusion Lab
[demonstration video](https://www.youtube.com/watch?v=mB-89ANRdmU).

Javascript code uses the same data flow syntax without the typing.
