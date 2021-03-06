/*

Copyright 2016 Green Mountain Computing Systems, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/

template <class T> class Interval : public Originator<T> {
 private:
  T current;
  unsigned long interval;
 public:
  Interval(unsigned long millis) : interval(millis) { }

  void init() {
    current = 0;
    scheduleInterval(this, interval);
  }

  bool run() {
    this->subscribers.push(this, &current);
    current += 1;
    return !this->subscribers.empty();
  }
};

template <class T> class Repeat : public Originator<T> {
 private:
  T value;
 public:
  Repeat(T value) : value(value) { }

  void init() {
    scheduleAlways(this);
  }

  bool run() {
    this->subscribers.push(this, &value);
    return !this->subscribers.empty();
  }
};
