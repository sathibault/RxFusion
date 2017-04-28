////////// Originator

template <class T, class U> const PipeNode<U> operator>>(Originator<T>& x, Operator<T,U> *op) {  op->attach(&x);  return PipeNode<U>(op); }

template <class T, class U> const PipeNode<U> operator>>(Originator<T>& x, const Compound<T,U>& comp) {  comp.first->attach(&x);  return PipeNode<U>(comp.last); }

template <class T, class U> const StateNode<U> operator>>(Originator<T>& x, Intermediate<T,U> *interm) {  interm->attach(&x);  return StateNode<U>(interm); }

template <class T> void operator>>(Originator<T>& x, Consumer<T>& sink) {  sink.attach(&x);  CONSTRUCTED }

template <class T> void operator>>(Originator<T>& x, Consumer<T> *sink) {  sink->attach(&x);  CONSTRUCTED }

template <class T> const PipeMerge<T> operator+(const PipeMerge<T>& v, Originator<T>& x) {   return PipeMerge<T>(v, &x); }

template <class T, int len> const PipeVec<T,len+1> operator&(const PipeVec<T,len>& v, Originator<T>& x) {   return PipeVec<T,len+1>(v, &x); }

template <class T, class U, class V> const PipeTuple3<T,U,V> operator^(const PipeTuple2<T,U>& tp, Originator<V>& x) {   return PipeTuple3<T,U,V>(tp._1, tp._2, &x); }

template <class T, class U, class V, class W> const PipeTuple4<T,U,V,W> operator^(const PipeTuple3<T,U,V>& tp, Originator<W>& x) {   return PipeTuple4<T,U,V,W>(tp._1, tp._2, tp._3, &x); }

template <class T> const PipeMerge<T> operator+(Originator<T>& x, Originator<T>& y) {   return PipeMerge<T>(&x, &y); }

template <class T, class U> const PipeTuple2<T,U> operator^(Originator<T>& x, Originator<U>& y) {   return PipeTuple2<T,U>(&x, &y); }

template <class T> const PipeVec<T,2> operator&(Originator<T>& x, Originator<T>& y) {   return PipeVec<T,2>(&x, &y); }

template <class T> const PipeMerge<T> operator+(Originator<T>& x, Generator<T>& y) { auto y1 = autoIterator(y);  return PipeMerge<T>(&x, y1); }

template <class T, class U> const PipeTuple2<T,U> operator^(Originator<T>& x, Generator<U>& y) { auto y1 = autoIterator(y);  return PipeTuple2<T,U>(&x, y1); }

template <class T> const PipeVec<T,2> operator&(Originator<T>& x, Generator<T>& y) { auto y1 = autoIterator(y);  return PipeVec<T,2>(&x, y1); }

template <class T> const PipeMerge<T> operator+(Originator<T>& x, const PipeNode<T>& y) {   return PipeMerge<T>(&x, y.node); }

template <class T, class U> const PipeTuple2<T,U> operator^(Originator<T>& x, const PipeNode<U>& y) {   return PipeTuple2<T,U>(&x, y.node); }

template <class T> const PipeVec<T,2> operator&(Originator<T>& x, const PipeNode<T>& y) {   return PipeVec<T,2>(&x, y.node); }

////////// Generator

template <class T, class U> const PipeNode<U> operator>>(Generator<T>& x, Operator<T,U> *op) { auto x1 = autoIterator(x); op->attach(x1);  return PipeNode<U>(op); }

template <class T, class U> const PipeNode<U> operator>>(Generator<T>& x, const Compound<T,U>& comp) { auto x1 = autoIterator(x); comp.first->attach(x1);  return PipeNode<U>(comp.last); }

template <class T, class U> const StateNode<U> operator>>(Generator<T>& x, Intermediate<T,U> *interm) { auto x1 = autoIterator(x); interm->attach(x1);  return StateNode<U>(interm); }

template <class T> void operator>>(Generator<T>& x, Consumer<T>& sink) { auto x1 = autoIterator(x); sink.attach(x1);  CONSTRUCTED }

template <class T> void operator>>(Generator<T>& x, Consumer<T> *sink) { auto x1 = autoIterator(x); sink->attach(x1);  CONSTRUCTED }

template <class T> const PipeMerge<T> operator+(const PipeMerge<T>& v, Generator<T>& x) { auto x1 = autoIterator(x);  return PipeMerge<T>(v, x1); }

template <class T, int len> const PipeVec<T,len+1> operator&(const PipeVec<T,len>& v, Generator<T>& x) { auto x1 = autoIterator(x);  return PipeVec<T,len+1>(v, x1); }

template <class T, class U, class V> const PipeTuple3<T,U,V> operator^(const PipeTuple2<T,U>& tp, Generator<V>& x) { auto x1 = autoIterator(x);  return PipeTuple3<T,U,V>(tp._1, tp._2, x1); }

template <class T, class U, class V, class W> const PipeTuple4<T,U,V,W> operator^(const PipeTuple3<T,U,V>& tp, Generator<W>& x) { auto x1 = autoIterator(x);  return PipeTuple4<T,U,V,W>(tp._1, tp._2, tp._3, x1); }

template <class T> const PipeMerge<T> operator+(Generator<T>& x, Originator<T>& y) { auto x1 = autoIterator(x);  return PipeMerge<T>(x1, &y); }

template <class T, class U> const PipeTuple2<T,U> operator^(Generator<T>& x, Originator<U>& y) { auto x1 = autoIterator(x);  return PipeTuple2<T,U>(x1, &y); }

template <class T> const PipeVec<T,2> operator&(Generator<T>& x, Originator<T>& y) { auto x1 = autoIterator(x);  return PipeVec<T,2>(x1, &y); }

template <class T> const PipeMerge<T> operator+(Generator<T>& x, Generator<T>& y) { auto x1 = autoIterator(x);auto y1 = autoIterator(y);  return PipeMerge<T>(x1, y1); }

template <class T, class U> const PipeTuple2<T,U> operator^(Generator<T>& x, Generator<U>& y) { auto x1 = autoIterator(x);auto y1 = autoIterator(y);  return PipeTuple2<T,U>(x1, y1); }

template <class T> const PipeVec<T,2> operator&(Generator<T>& x, Generator<T>& y) { auto x1 = autoIterator(x);auto y1 = autoIterator(y);  return PipeVec<T,2>(x1, y1); }

template <class T> const PipeMerge<T> operator+(Generator<T>& x, const PipeNode<T>& y) { auto x1 = autoIterator(x);  return PipeMerge<T>(x1, y.node); }

template <class T, class U> const PipeTuple2<T,U> operator^(Generator<T>& x, const PipeNode<U>& y) { auto x1 = autoIterator(x);  return PipeTuple2<T,U>(x1, y.node); }

template <class T> const PipeVec<T,2> operator&(Generator<T>& x, const PipeNode<T>& y) { auto x1 = autoIterator(x);  return PipeVec<T,2>(x1, y.node); }

////////// const PipeNode

template <class T, class U> const PipeNode<U> operator>>(const PipeNode<T>& x, Operator<T,U> *op) {  op->attach(x.node);  return PipeNode<U>(op); }

template <class T, class U> const PipeNode<U> operator>>(const PipeNode<T>& x, const Compound<T,U>& comp) {  comp.first->attach(x.node);  return PipeNode<U>(comp.last); }

template <class T, class U> const StateNode<U> operator>>(const PipeNode<T>& x, Intermediate<T,U> *interm) {  interm->attach(x.node);  return StateNode<U>(interm); }

template <class T> void operator>>(const PipeNode<T>& x, Consumer<T>& sink) {  sink.attach(x.node);  CONSTRUCTED }

template <class T> void operator>>(const PipeNode<T>& x, Consumer<T> *sink) {  sink->attach(x.node);  CONSTRUCTED }

template <class T> const PipeMerge<T> operator+(const PipeMerge<T>& v, const PipeNode<T>& x) {   return PipeMerge<T>(v, x.node); }

template <class T, int len> const PipeVec<T,len+1> operator&(const PipeVec<T,len>& v, const PipeNode<T>& x) {   return PipeVec<T,len+1>(v, x.node); }

template <class T, class U, class V> const PipeTuple3<T,U,V> operator^(const PipeTuple2<T,U>& tp, const PipeNode<V>& x) {   return PipeTuple3<T,U,V>(tp._1, tp._2, x.node); }

template <class T, class U, class V, class W> const PipeTuple4<T,U,V,W> operator^(const PipeTuple3<T,U,V>& tp, const PipeNode<W>& x) {   return PipeTuple4<T,U,V,W>(tp._1, tp._2, tp._3, x.node); }

template <class T> const PipeMerge<T> operator+(const PipeNode<T>& x, Originator<T>& y) {   return PipeMerge<T>(x.node, &y); }

template <class T, class U> const PipeTuple2<T,U> operator^(const PipeNode<T>& x, Originator<U>& y) {   return PipeTuple2<T,U>(x.node, &y); }

template <class T> const PipeVec<T,2> operator&(const PipeNode<T>& x, Originator<T>& y) {   return PipeVec<T,2>(x.node, &y); }

template <class T> const PipeMerge<T> operator+(const PipeNode<T>& x, Generator<T>& y) { auto y1 = autoIterator(y);  return PipeMerge<T>(x.node, y1); }

template <class T, class U> const PipeTuple2<T,U> operator^(const PipeNode<T>& x, Generator<U>& y) { auto y1 = autoIterator(y);  return PipeTuple2<T,U>(x.node, y1); }

template <class T> const PipeVec<T,2> operator&(const PipeNode<T>& x, Generator<T>& y) { auto y1 = autoIterator(y);  return PipeVec<T,2>(x.node, y1); }

template <class T> const PipeMerge<T> operator+(const PipeNode<T>& x, const PipeNode<T>& y) {   return PipeMerge<T>(x.node, y.node); }

template <class T, class U> const PipeTuple2<T,U> operator^(const PipeNode<T>& x, const PipeNode<U>& y) {   return PipeTuple2<T,U>(x.node, y.node); }

template <class T> const PipeVec<T,2> operator&(const PipeNode<T>& x, const PipeNode<T>& y) {   return PipeVec<T,2>(x.node, y.node); }

////////// Tuple

template <class T,class U> void operator>>(const PipeTuple2<T,U>& t, Consumer<Tuple2<T,U>>& sink) { sink.attach(t.obs()); CONSTRUCTED }

template <class T,class U,class Z> const PipeNode<Z> operator>>(const PipeTuple2<T,U>& t, Operator<Tuple2<T,U>,Z> *op) { op->attach(t.obs()); return PipeNode<Z>(op); }

template <class T,class U,class Z> const PipeNode<Z> operator>>(const PipeTuple2<T,U>& t, Compound<Tuple2<T,U>,Z>& comp) { comp.first->attach(t.obs()); return PipeNode<Z>(comp.last); }

template <class T,class U,class V> void operator>>(const PipeTuple3<T,U,V>& t, Consumer<Tuple3<T,U,V>>& sink) { sink.attach(t.obs()); CONSTRUCTED }

template <class T,class U,class V,class Z> const PipeNode<Z> operator>>(const PipeTuple3<T,U,V>& t, Operator<Tuple3<T,U,V>,Z> *op) { op->attach(t.obs()); return PipeNode<Z>(op); }

template <class T,class U,class V,class Z> const PipeNode<Z> operator>>(const PipeTuple3<T,U,V>& t, Compound<Tuple3<T,U,V>,Z>& comp) { comp.first->attach(t.obs()); return PipeNode<Z>(comp.last); }

template <class T,class U,class V,class W> void operator>>(const PipeTuple4<T,U,V,W>& t, Consumer<Tuple4<T,U,V,W>>& sink) { sink.attach(t.obs()); CONSTRUCTED }

template <class T,class U,class V,class W,class Z> const PipeNode<Z> operator>>(const PipeTuple4<T,U,V,W>& t, Operator<Tuple4<T,U,V,W>,Z> *op) { op->attach(t.obs()); return PipeNode<Z>(op); }

template <class T,class U,class V,class W,class Z> const PipeNode<Z> operator>>(const PipeTuple4<T,U,V,W>& t, Compound<Tuple4<T,U,V,W>,Z>& comp) { comp.first->attach(t.obs()); return PipeNode<Z>(comp.last); }

