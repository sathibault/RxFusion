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

template <class T> Observable<T> *autoIterator(Generator<T>& g) {
  Observable<T> *obs = g.Implicit();
  if (obs == NULL) {
    Iterator<T> *iter = new Iterator<T>(0, false, false);
    iter->setSource(&g);
    g.setImplicit(iter);
    obs = iter;
  }
  return obs;
}

//////////////////////////////////////// Join operators

template <class T> class PipeMerge {
 private:
  Merger<T> *merger;
 public:
  PipeMerge(Observable<T> *a, Observable<T> *b) {
    merger = new Merger<T>();
    merger->attach(a);
    merger->attach(b);
  }
  PipeMerge(const PipeMerge& other, Observable<T> *a) {
    merger = other.merger;
    merger->attach(a);
  }
  Observable<T> *obs() const { return merger; }
};

template <class T> void operator>>(const PipeMerge<T>& m, Consumer<T>& sink) { sink.attach(m.obs()); CONSTRUCTED }

template <class T, class U> const PipeNode<U> operator>>(const PipeMerge<T>& m, Operator<T,U> *op) {  op->attach(m.obs());  return PipeNode<U>(op); }

template <class T, int len> class PipeVec {
 public:
  Vec<Observable<T>*,len> args;
  PipeVec(Observable<T> *a, Observable<T> *b) { args[0]=a; args[1]=b; }
  PipeVec(const PipeVec<int, len-1>& a, Observable<T> *b) {
    for (int i = 0; i < (len-1); i++)
      args[i] = a.args[i];
    args[len-1] = b;
  }
  
  Observable<Vec<T,len>> *obs() const { return new ZipVec<T,len>(args); }
};

template <class T, int len> void operator>>(const PipeVec<T,len>& v, Consumer<Vec<T,len>>& sink) { sink.attach(v.obs()); CONSTRUCTED }

template <class T, int len, class U> const PipeNode<U> operator>>(const PipeVec<T,len>& v, Operator<T,U> *op) {  op->attach(v.obs());  return PipeNode<U>(op); }

template <class T, class U> class PipeTuple2 {
 public:
  Observable<T> *_1;
  Observable<U> *_2;
  PipeTuple2(Observable<T> *a, Observable<U> *b) { _1 = a; _2 = b; }

  Observable<Tuple2<T,U>> *obs() const { return new Zipper<T,U>(_1,_2); }
};

template <class T, class U, class V> class PipeTuple3 {
 public:
  Observable<T> *_1;
  Observable<U> *_2;
  Observable<V> *_3;
  PipeTuple3(Observable<T> *a, Observable<U> *b, Observable<V> *c) { _1 = a; _2 = b; _3 = c; }

  Observable <Tuple3<T,U,V>> *obs() const { return new Zipper3<T,U,V>(_1,_2,_3); }
};

template <class T, class U, class V, class W> class PipeTuple4 {
 public:
  Observable<T> *_1;
  Observable<U> *_2;
  Observable<V> *_3;
  Observable<W> *_4;
  PipeTuple4(Observable<T> *a, Observable<U> *b, Observable<V> *c, Observable<W> *d) { _1 = a; _2 = b; _3 = c; _4 = d;}

  Observable<Tuple4<T,U,V,W>> *obs() const { return new Zipper4<T,U,V,W>(_1,_2,_3,_4); }
};

//////////////////////////////////////// Special combinations

template <class T> const PipeNode<T> operator>>(Generator<T>& gen, Iterator<T> *iter) {
  iter->setSource(&gen);
  return PipeNode<T>(iter);
}

template <class T> const PipeNode<T> operator>>(State<T>& state, Poller<T> *poll) {
  poll->setSource(&state);
  return PipeNode<T>(poll);
}

template <class T, class U> PipeNode<U> operator>>(State<T>& state, const Compound<T,U>& comp) {
  Poller<T> *poll = Poll<T>();
  poll->setSource(&state);
  comp.first->attach(poll);
  return PipeNode<U>(comp.last);
}

template <class T> PipeNode<T> operator>>(const StateNode<T>& state, Poller<T> *poll) {
  poll->setSource(state.node);
  return PipeNode<T>(poll);
}
