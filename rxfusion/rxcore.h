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

template <class T> class Maybe {
 public:
  T value;
  bool exists;
  Maybe() { exists = false; }
  Maybe(T &a) : value(a) { exists = true; }
};

//////////////////////////////////////// Basic components

template <class T, class U> class CounterOp : public Operator<T,Tuple2<T,U>> {
 private:
  U count;
 public:
  CounterOp() { count = 0; }
  void onData(RxNode *source, void *value) {
    Tuple2<T,U> cur(*(T *)value, ++count);
    this->subscribers.push(this, &cur);
  }
};

template <class T, class U> CounterOp<T,U> *Counted() {
  return new CounterOp<T,U>();
}

template <class T, class U> class MapOp : public Operator<T,U> {
 private:
  std::function<U (T&)> fun;
 public:
  MapOp(const std::function<U (T&)>& transform) : fun(transform) {}
  void onData(RxNode *source, void *value) {
    U b = fun(*(T *)value);
    this->subscribers.push(this, &b);
  }
};

template <class T, class U> MapOp<T,U> *Map(const std::function<U (T&)>& transform) {
  return new MapOp<T,U>(transform);
}

template <class T> class FilterOp : public Operator<T,T> {
 private:
  std::function<bool (T&)> fun;
 public:
  FilterOp(const std::function<bool (T&)>& filter) : fun(filter) { }
  void onData(RxNode *source, void *value) {
    T current = *(T *)value;
    if (fun(current))
      this->subscribers.push(this, &current);
  }
};

template <class T> FilterOp<T> *Filter(const std::function<bool (T&)>& filter) {
  return new FilterOp<T>(filter);
}

template <class T, class U> class ScanOp : public Operator<T,U> {
 private:
  U accum;
  std::function<U (T&, U&)> fun;
 public:
  ScanOp(const std::function<U (T&, U&)>& transform, U init) : fun(transform) {
    accum = init;
  }
  void onData(RxNode *source, void *value) {
    accum = fun(*(T *)value, accum);
    this->subscribers.push(this, &accum);
  }
};

template <class T, class U> ScanOp<T,U> *Scan(const std::function<U (T&, U&)>& transform, U init) {
  return new ScanOp<T,U>(transform, init);
}

enum Action { TPass, TDrop, TClose };

template <class T> class TrafficOp : public Operator<T,T> {
 private:
  std::function<Action (T&)> fun;
 public:
  TrafficOp(const std::function<Action (T&)>& control) : fun(control) { }
  void onData(RxNode *source, void *value) {
    T current = *(T *)value;
    Action state = fun(current);
    if (state == TPass) {
      this->subscribers.push(this, &current);
    } else if (state == TClose) {
      this->detach(); // unsubscribe sources
      this->close(); // close downstream
      delete this;
    }
  }
};

template <class T> TrafficOp<T> *Traffic(const std::function<Action (T&)>& control) {
  return new TrafficOp<T>(control);
}


//////////////////////////////////////// Sliding window components

template <class T, int capacity> class WindowOp : public Operator<T,fifo<T,capacity>> {
 private:
  bool partials;
  fifoWr<T,capacity> window;
 public:
  WindowOp(bool all=false) { partials = all; }
  void onData(RxNode *source, void *value) {
    window.push(*(T *)value);
    if (partials || window.isFull())
      this->subscribers.push(this, &window);
  }
};

// Optimized foldl for operators with an inverse.

template <class T, class U, int capacity> class SlidingOp : public Operator<T,U> {
 private:
  U accum;
  std::function<U (T&, U&)> inc;
  std::function<U (T&, U&)> dec;
  bool partials;
  fifoWr<T,capacity> window;
 public:
  SlidingOp(const std::function<U (T&, U&)>& add,
	    const std::function<U (T&, U&)>& rem,
	    U init, bool all) : accum(init), inc(add), dec(rem) {
    partials = all;
  }
  void onData(RxNode *source, void *value) {
    if (window.isFull())
      accum = dec(window[window.begin()], accum);
    window.push(*(T *)value);
    accum = inc(*(T *)value, accum);
    if (partials || window.isFull())
      this->subscribers.push(this, &accum);
  }
};

template <class T, class U, int capacity> SlidingOp<T,U,capacity> *Sliding(const std::function<U (T&, U&)>& add, const std::function<U (T&, U&)>& rem, U init, bool all=false) {
  return new SlidingOp<T,U,capacity>(add, rem, init, all);
}

//////////////////////////////////////// End-to-end window operators

template <class T, int capacity> class BatchOp : public Operator<T,fifo<T,capacity>> {
 private:
  fifoWr<T,capacity> window;
 public:
  void onData(RxNode *source, void *value) {
    window.push(*(T *)value);
    if (window.isFull()) {
      this->subscribers.push(this, &window);
      window.reset();
    }
  }
};

// Optimized storage-free foldl/scan.

template <class T, class U, int width> class BatchFoldOp : public Operator<T,U> {
 private:
  int count;
  U accum, initial;
  std::function<U (T&, U&)> fun;
 public:
  BatchFoldOp(const std::function<U (T&, U&)>& fun, U init)
    : accum(init), initial(init), fun(fun) { count = 0; }

  void onData(RxNode *source, void *value) {
    accum = fun(*(T *)value, accum);
    count++;
    if (count == width) {
      this->subscribers.push(this, &accum);
      accum = initial;
      count = 0;
    }
  }
};

template <class T, class U, int width> BatchFoldOp<T,U,width> *BatchFold(const std::function<U (T&, U&)>& fun, U init) {
  return new BatchFoldOp<T,U,width>(fun, init);
}

template <class T, class U, int width> class BatchFold1Op : public Operator<T,U> {
 private:
  int count;
  U accum;
  std::function<U (T&, U&)> fun;
 public:
  BatchFold1Op(const std::function<U (T&, U&)>& fun) : fun(fun) {
    count = 0;
  }

  void onData(RxNode *source, void *value) {
    if (count > 0)
      accum = fun(*(T *)value, accum);
    else
      accum = *(T *)value;
    count++;
    if (count == width) {
      this->subscribers.push(this, &accum);
      count = 0;
    }
  }
};

template <class T, class U, int width> BatchFold1Op<T,U,width> *BatchFold1(const std::function<U (T&, U&)>& fun) {
  return new BatchFold1Op<T,U,width>(fun);
}

template <class T, class U, int width> class BatchScanOp : public Operator<T,U> {
 private:
  int count;
  U accum, initial;
  std::function<U (T&, U&)> fun;
 public:
  BatchScanOp(const std::function<U (T&, U&)>& fun, U init)
    : accum(init), initial(init), fun(fun) { count = 0; }

  void onData(RxNode *source, void *value) {
    accum = fun(*(T *)value, accum);
    count++;
    this->subscribers.push(this, &accum);
    if (count == width) {
      accum = initial;
      count = 0;
    }
  }
};

template <class T, class U, int width> BatchScanOp<T,U,width> *BatchScan(const std::function<U (T&, U&)>& fun, U init) {
  return new BatchScanOp<T,U,width>(fun, init);
}


//////////////////////////////////////// Join components

template <class T, class U> class WithOp : public Operator<T,Tuple2<T,U>> {
 private:
  Observable<T> *other;
  Tuple2<T,U> data;

 public:
  WithOp(U init, Observable<U> *b) {
    data._2 = init;
    this->attach(b);
    other = b;
  }

  void onData(RxNode *source, void *value) {
    if (source != other) {
      data._1 = *(T *)value;
      this->subscribers.push(this, &data);
    } else
      data._2 = *(U *)value;
  }

  void onClose(RxNode *source) {
    this->sources.remove(source);
    if (source != other) // close downstream independent of other
      this->close();
    if (this->sources.empty()) // delete with both sources closed
      delete this;
  }
};

template <class T, class U> WithOp<T,U> *With(U init, Observable<U> *other) {
  return new WithOp<T,U>(init, other);
}

template <class T, class U> WithOp<T,U> *With(U init, const PipeNode<U>& pipe) {
  return new WithOp<T,U>(init, pipe.node);
}

template <class T, int len> class ZipVec : public Observable<Vec<T,len>> {
 private:
  Vec<Observable<T>*,len> obs;
  VecWr<T,len> data;
  VecWr<bool,len> full;

 public:
  ZipVec(const Vec<Observable<T>*,len>& args) : obs(args) {
    for (int i = 0; i < len; i++) {
      this->attach(obs[i]);
      full.set(i, false);
    }
  }

  void onData(RxNode *source, void *value) {
    int n = 0;
    for (int i = 0; i < len; i++) {
      if (obs[i] == source) {
	data.set(i, *(T *)value);
	full.set(i, true);
      }
      if (full[i]) n += 1;
    }
    if (n == len) {
      for (int i = 0; i < len; i++)
	full.set(i, false);
      this->subscribers.push(this, &data);
    }
  }
  void onClose(RxNode *source) {
    this->sources.remove(source);
    if (this->sources.empty()) {
      this->close();
      delete this;
    }
  }
};

template <class T> class Merger : public Observable<T> {
 public:
  void onData(RxNode *pub, void *val) { this->subscribers.push(this, val); }
  void onClose(RxNode *source) {
    this->sources.remove(source);
    if (this->sources.empty()) {
      this->close();
      delete this;
    }
  }
};

template <class T, class U> class Zipper : public Observable<Tuple2<T,U>> {
 private:
  Observable<T> *_1;
  Observable<U> *_2;
  Tuple2<T,U> data;
  bool full[2];

 public:
  Zipper(Observable<T> *a, Observable<U> *b) {
    _1 = a;
    _2 = b;
    this->attach(a);
    this->attach(b);
    full[0] = false;
    full[1] = false;
  }

  void onData(RxNode *source, void *value) {
    if (source == _1) {
      data._1 = *(T *)value;
      full[0] = true;
    } else {
      data._2 = *(U *)value;
      full[1] = true;
    }
    if (full[0] && full[1]) {
      full[0] = false;
      full[1] = false;
      this->subscribers.push(this, &data);
    }
  }
  void onClose(RxNode *source) {
    this->sources.remove(source);
    if (this->sources.empty()) {
      this->close();
      delete this;
    }
  }
};

template <class T, class U, class V> class Zipper3 : public Observable<Tuple3<T,U,V>> {
 private:
  Observable<T> *_1;
  Observable<U> *_2;
  Observable<V> *_3;
  Tuple3<T,U,V> data;
  bool full[3];

 public:
  Zipper3(Observable<T> *a, Observable<U> *b, Observable<V> *c) {
    _1 = a; _2 = b; _3 = c;
    this->attach(a);
    this->attach(b);
    this->attach(c);
    full[0] = full[1] = full[2] = false;
  }

  void onData(RxNode *source, void *value) {
    if (source == _1) {
      data._1 = *(T *)value;
      full[0] = true;
    } else if (source == _2) {
      data._2 = *(U *)value;
      full[1] = true;
    } else {
      data._3 = *(V *)value;
      full[2] = true;
    }
    if (full[0] && full[1] && full[2]) {
      full[0] = full[1] = full[2] = false;
      this->subscribers.push(this, &data);
    }
  }

  void onClose(RxNode *source) {
    this->sources.remove(source);
    if (this->sources.empty()) {
      this->close();
      delete this;
    }
  }
};

template <class T, class U, class V, class W> class Zipper4 : public Observable<Tuple4<T,U,V,W>> {
 private:
  Observable<T> *_1;
  Observable<U> *_2;
  Observable<V> *_3;
  Observable<W> *_4;
  Tuple4<T,U,V,W> data;
  bool full[4];

 public:
  Zipper4(Observable<T> *a, Observable<U> *b, Observable<V> *c, Observable<W> *d) {
    _1 = a; _2 = b; _3 = c; _4 = d;
    this->attach(a);
    this->attach(b);
    this->attach(c);
    this->attach(d);
    full[0] = full[1] = full[2] = full[3] = false;
  }

  void onData(RxNode *source, void *value) {
    if (source == _1) {
      data._1 = *(T *)value;
      full[0] = true;
    } else if (source == _2) {
      data._2 = *(U *)value;
      full[1] = true;
    } else if (source == _3) {
      data._3 = *(V *)value;
      full[2] = true;
    } else {
      data._4 = *(W *)value;
      full[3] = true;
    }
    if (full[0] && full[1] && full[2] && full[3]) {
      full[0] = full[1] = full[2] = full[3] = false;
      this->subscribers.push(this, &data);
    }
  }

  void onClose(RxNode *source) {
    this->sources.remove(source);
    if (this->sources.empty()) {
      this->close();
      delete this;
    }
  }
};


//////////////////////////////////////// Timer operators

template <class T> class SampleOp : public Operator<T,T> {
 private:
  T latest;
  unsigned interval;
  bool empty,repeat,complete;
 public:
  SampleOp(unsigned millis, bool repeat) : interval(millis),repeat(repeat) {
    empty = true;
    complete=false;
  }
  virtual ~SampleOp() { unregisterScheduled(this); }

  void subscribe(RxNode *node) {
    if (this->subscribers.empty()) registerScheduled(this);
    Operator<T,T>::subscribe(node);
  }

  void init() {
    scheduleInterval(this, interval);
  }

  void onData(RxNode *source, void *value) {
    latest = *(T *)value;
    empty = false;
  }

  void onClose(RxNode *source) {
    this->sources.remove(source);
    complete = true;
  }

  bool run() {
    if (!empty) {
      this->subscribers.push(this, &latest);
      empty = !repeat;
    }
    if (complete) {
      this->close();
      return false;
    }
    return true;
  }
};

template <class T> SampleOp<T> *Sample(unsigned millis, bool repeat=false) {
  return new SampleOp<T>(millis, repeat);
}

template <class T> class DebounceOp : public Operator<T,T> {
 private:
  T latest;
  unsigned timeout;
  bool complete;
 public:
  DebounceOp(unsigned millis) : timeout(millis) {
    registerScheduled(this);
    complete=false;
  }
  virtual ~DebounceOp() { unregisterScheduled(this); }

  void onData(RxNode *source, void *value) {
    latest = *(T *)value;
    scheduleInterval(this, timeout);
  }

  void onClose(RxNode *source) {
    this->sources.remove(source);
    complete = true;
  }

  bool run() {
    this->subscribers.push(this, &latest);
    if (complete) {
      this->close();
      return false;
    }
    return false;
  }
};

template <class T> DebounceOp<T> *Debounce(unsigned millis) {
  return new DebounceOp<T>(millis);
}


//////////////////////////////////////// Generator/Intermediate components

template <class T, class U> class IntermFoldOp : public Intermediate<T,U> {
 private:
  U accum, initial;
  std::function<U (T&, U&)> fun;
 public:
  IntermFoldOp(const std::function<U (T&, U&)>& fun, U init)
    : accum(init), initial(init), fun(fun) { }

  void onData(RxNode *source, void *value) {
    accum = fun(*(T *)value, accum);
  }

  void get() {
    this->subscribers.push(this, &accum);
    accum = initial;
  }
};

template <class T, class U> IntermFoldOp<T,U> *IntermFold(const std::function<U (T&, U&)>& fun, U init) {
  return new IntermFoldOp<T,U>(fun, init);
}

template <class T> class Iterator : public Originator<T> {
 private:
  Generator<T> *generator;
  unsigned interval;
  bool repeat, autoRepeat;
 public:
  Iterator(unsigned millis, bool repeat, bool autoRepeat)
    : interval(millis), repeat(repeat), autoRepeat(autoRepeat) { generator = NULL; }
  void setSource(Generator<T> *gen) { generator = gen; this->attach(gen); }

  void onData(RxNode *source, void *value) {
    this->subscribers.push(this, value);
  }

  // never called
  void onClose(RxNode *source) { }

  void init() {
    generator->reset();
    if (generator->more()) {
      if (interval == 0)
	scheduleAlways(this);
      else
	scheduleInterval(this, interval);
    }
  }

  bool run() {
    generator->next();
    if (generator->more())
      return true;
    if (repeat) {
      generator->reset();
      return autoRepeat;
    }
    this->detach(); // unsubscribe to generator
    this->close();
    delete this;
    return false;
  }
};

template <class T> Iterator<T> *Iterate(unsigned millis=0, bool repeat=false, bool autoRepeat=true) {
  return new Iterator<T>(millis, repeat, autoRepeat);
}

template <class T> class Poller : public Originator<T> {
 private:
  State<T> *state;
  unsigned interval;
 public:
  Poller(unsigned millis) : interval(millis) { state = NULL; }
  void setSource(State<T> *st) { state = st; this->attach(st); }

  void onData(RxNode *source, void *value) {
    this->subscribers.push(this, value);
  }

  void onClose(RxNode *source) {
    this->sources.remove(source);
    this->close();
    delete this;
  }

  void init() {
    if (interval == 0)
      scheduleAlways(this);
    else
      scheduleInterval(this, interval);
  }

  bool run() {
    this->state->get();
    if (this->state->closed()) {
      this->close();
      return false;
    }
    return true;
  }
};

template <class T> Poller<T> *Poll(unsigned millis=0) {
  return new Poller<T>(millis);
}
