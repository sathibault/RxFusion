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

template <class T, class U> MapOp<T,U> *Project1() {
  return new MapOp<T,U>([](T& t) -> U { return t._1; });
}
template <class T, class U> MapOp<T,U> *Project2() {
  return new MapOp<T,U>([](T& t) -> U { return t._2; });
}
template <class T, class U> MapOp<T,U> *Project3() {
  return new MapOp<T,U>([](T& t) -> U { return t._3; });
}

template <class T, class U> Operator<T,U> *Count() {
  return Scan<T,U>([](T& cur, U& count) -> U { return count + 1; }, 0);
}

template <class T, int width> const Compound<T,T> Delay() {
  auto *win = new WindowOp<T,width>();
  auto *map = Map<fifo<T,width>,T>([](fifo<T,width>& win) -> T {
      return win.get(win.begin());
    });
  map->attach(win);
  return Compound<T,T>(win, map);
}

// Sliding-window foldl with non-invertable function.

template <class T, class U, int width> const Compound<T,U> WinFold(const std::function<U (T&, U&)>& fun, U init, bool all=false) {
  auto *win = new WindowOp<T,width>(all);
  auto *map = Map<fifo<T,width>,U>([init,fun](fifo<T,width>& win, U& accum)->U {
      int n = win.count();
      U res = init;
      for (int i = win.begin(); n > 0; i = win.next(i), n--)
	res = fun(win[i], res);
      return res;
    });
  map->attach(win);
  return Compound<T,U>(win, map);
};

template <class T, class U, int width> const Compound<T,U> WinFold1(const std::function<U (T&, U&)>& fun, bool all=false) {
  auto *win = new WindowOp<T,width>(all);
  auto *map = Map<fifo<T,width>,U>([fun](fifo<T,width>& win, U& accum)->U {
      int n = win.count();
      int i = win.begin();
      U res = win[i];
      for (i = win.next(i), n--; n > 0; i = win.next(i), n--)
	res = fun(win[i], res);
      return res;
    });
  map->attach(win);
  return Compound<T,U>(win, map);
};

//////////////////////////////////////// Aggregations

template <class T, int width> Operator<T,T> *WinSum() {
  return Sliding<T,T,width>([](T& elm, T& accum) -> T {
      return accum + elm;
    }, [](T& elm, T& accum) -> T {
      return accum - elm;
    }, T(0));
}

template <class T, int width> Operator<T,T> *SumOf() {
  return BatchFold1<T,T,width>([](T& cur, T& accum)->T { return accum+cur; });
}

template <class T> const Compound<T,T> SumOver(unsigned millis) {
  auto *sum = IntermFold<T,T>([](T& elm, T& accum) -> T {
      return accum + elm;
    }, T(0));
  auto *poll = Poll<T>(millis);
  poll->setSource(sum);
  return Compound<T,T>(sum, poll);
}

template <class T, int width> const Compound<T,T> WinAverage() {
  auto *sum = Sliding<T,T,width>([](T& elm, T& accum)->T {
      return accum + elm;
    }, [](T& elm, T& accum)->T {
      return accum - elm;
    }, T(0));
  auto *map = Map<T,T>([](T& accum)->T { return accum/width; });
  map->attach(sum);
  return Compound<T,T>(sum, map);
}

template <class T, int width> const Compound<T,T> AverageOf() {
  auto *sum = BatchFold1<T,T,width>([](T& cur, T& accum)->T { return accum+cur; });
  auto *map = Map<T,T>([](T& accum)->T { return accum/width; });
  map->attach(sum);
  return Compound<T,T>(sum, map);
}

template <class T> const Compound<T,T> AverageOver(unsigned millis) {
  auto *sum = IntermFold<T,Tuple2<T,int>>([](T& elm, Tuple2<T,int>& t)->Tuple2<T,int> {
      return Tuple2<T,int>(t._1 + elm, t._2 + 1);
    }, Tuple2<T,int>(0,0));
  auto *poll = Poll<Tuple2<T,int>>(millis);
  auto *map = Map<Tuple2<T,int>,T>([](Tuple2<T,int>& t)->T {
      return (t._2 > 0) ? t._1/t._2 : 0;
    });
  poll->setSource(sum);
  map->attach(poll);
  return Compound<T,T>(sum, map);
}

template <class T, int width> const Compound<T,T> WinMin() {
  auto *win = new WindowOp<T,width>();
  auto *map = Map<fifo<T,width>,T>([](fifo<T,width>& win) -> T {
      int n = win.count();
      int i = win.begin();
      T res = win[i];
      for (i = win.next(i), n--; n > 0; i = win.next(i), n--) {
	T b = win[i];
	if (b < res) res = b;
      }
      return res;
    });
  map->attach(win);
  return Compound<T,T>(win, map);
}

template <class T, int width> Operator<T,T> *MinOf() {
  return BatchFold1<T,T,width>([](T& cur, T& accum)->T {
      return (cur < accum) ? cur : accum;
    });
}

template <class T> const Compound<T,T> MinOver(unsigned millis) {
  auto *fold = IntermFold<T,T>([](T& elm, T& accum) -> T {
      return (elm < accum) ? elm : accum;
    }, std::numeric_limits<T>::max());
  auto *poll = Poll<T>(millis);
  poll->setSource(fold);
  return Compound<T,T>(fold, poll);
}

template <class T, int width> const Compound<T,T> WinMax() {
  auto *win = new WindowOp<T,width>();
  auto *map = Map<fifo<T,width>,T>([](fifo<T,width>& win) -> T {
      int n = win.count();
      int i = win.begin();
      T res = win[i];
      for (i = win.next(i), n--; n > 0; i = win.next(i), n--) {
	T b = win[i];
	if (b > res) res = b;
      }
      return res;
    });
  map->attach(win);
  return Compound<T,T>(win, map);
}

template <class T, int width> Operator<T,T> *MaxOf() {
  return BatchFold1<T,T,width>([](T& cur, T& accum)->T {
      return (cur > accum) ? cur : accum;
    });
}

template <class T> const Compound<T,T> MaxOver(unsigned millis) {
  auto *fold = IntermFold<T,T>([](T& elm, T& accum) -> T {
      return (elm > accum) ? elm : accum;
    }, std::numeric_limits<T>::min());
  auto *poll = Poll<T>(millis);
  poll->setSource(fold);
  return Compound<T,T>(fold, poll);
}


//////////////////////////////////////// Math

template <class T> Operator<T,T> *Scale(T k) {
  return new MapOp<T,T>([k](T& x) -> T { return k*x; });
}

template <class T> Operator<T,T> *Add(T b) {
  return new MapOp<T,T>([b](T& x) -> T { return x+b; });
}

template <class T> Operator<T,T> *ClampHigh(T u) {
  return new MapOp<T,T>([u](T& x) -> T { return x > u ? u : x; });
}

template <class T> Operator<T,T> *ClampLow(T l) {
  return new MapOp<T,T>([l](T& x) -> T { return x < l ? l : x; });
}

template <class T> Operator<T,T> *Clamp(T l, T u) {
  return new MapOp<T,T>([l,u](T& x) -> T { return x < l ? l : (x > u ? u : x); });
}

template <class T, class U> Operator<T,U> *Toggle(U initial) {
  return new ScanOp<T,U>([](T& data, U& state) -> U { return !state; }, initial);
}

template <class T, class U> Operator<T,U> *Const(U c) {
  return new MapOp<T,U>([c](T& x) -> T { return c; });
}


//////////////////////////////////////// Filtering

template <class T> const Compound<T,T> Drop(int drop) {
  auto *cnt = Counted<T,int>();
  auto *filter = Filter<Tuple2<T,int>>([drop](Tuple2<T,int>& t) -> bool {
      return t._2 > drop;
    });
  auto *prj = Project1<Tuple2<T,int>,T>();
  filter->attach(cnt);
  prj->attach(filter);
  return Compound<T,T>(cnt, prj);
}

template <class T> const Compound<T,T> Take(int take) {
  auto *cnt = Counted<T,int>();
  auto *traffic = Traffic<Tuple2<T,int>>([take](Tuple2<T,int>& t) -> Action {
      return (t._2 <= take) ? TPass : TClose;
    });
  auto *prj = Project1<Tuple2<T,int>,T>();
  traffic->attach(cnt);
  prj->attach(traffic);
  return Compound<T,T>(cnt, prj);
}

template <class T> const Compound<T,T> Dedup() {
  auto *scan = Scan<T,Tuple2<T,Maybe<T>>>([](T& cur, Tuple2<T,Maybe<T>>& last) -> Tuple2<T,Maybe<T>> {
      return Tuple2<T,Maybe<T>>(cur, Maybe<T>(last._1));
    }, Tuple2<T,Maybe<T>>());
  auto *filter = Filter<Tuple2<T,Maybe<T>>>([](Tuple2<T,Maybe<T>>& x) -> bool {
      return !x._2.exists || x._1 != x._2.value;
    });
  auto *prj = Project1<Tuple2<T,Maybe<T>>,T>();
  filter->attach(scan);
  prj->attach(filter);
  return Compound<T,T>(scan, prj);
}

Operator<bool,bool> *True() {
  return new FilterOp<bool>([](bool& x) -> bool { return x; });
}

Operator<bool,bool> *False() {
  return new FilterOp<bool>([](bool& x) -> bool { return !x; });
}

template <class T> Operator<T,T> *Over(T th) {
  return new FilterOp<T>([th](T& x) -> bool { return x > th; });
}

template <class T> Operator<T,T> *Under(T th) {
  return new FilterOp<T>([th](T& x) -> bool { return x < th; });
}

template <class T> Operator<T,T> *AtLeast(T th) {
  return new FilterOp<T>([th](T& x) -> bool { return x >= th; });
}

template <class T> Operator<T,T> *AtMost(T th) {
  return new FilterOp<T>([th](T& x) -> bool { return x <= th; });
}

template <class T> Operator<T,T> *Positive() {
  return new FilterOp<T>([](T& x) -> bool { return x > 0; });
}

template <class T> Operator<T,T> *Negative() {
  return new FilterOp<T>([](T& x) -> bool { return x < 0; });
}

template <class T> Operator<T,T> *Zero() {
  return new FilterOp<T>([](T& x) -> bool { return x == 0; });
}

template <class T> Operator<T,bool> *IsOver(T th) {
  return new MapOp<T,bool>([th](T& x) -> bool { return x > th; });
}

template <class T> Operator<T,bool> *IsUnder(T th) {
  return new MapOp<T,bool>([th](T& x) -> bool { return x < th; });
}

template <class T> Operator<T,bool> *IsAtLeast(T th) {
  return new MapOp<T,bool>([th](T& x) -> bool { return x >= th; });
}

template <class T> Operator<T,bool> *IsAtMost(T th) {
  return new MapOp<T,bool>([th](T& x) -> bool { return x <= th; });
}

template <class T> Operator<T,bool> *IsPositive() {
  return new MapOp<T,bool>([](T& x) -> bool { return x > 0; });
}

template <class T> Operator<T,bool> *IsNegative() {
  return new MapOp<T,bool>([](T& x) -> bool { return x < 0; });
}

template <class T> Operator<T,bool> *IsZero() {
  return new MapOp<T,bool>([](T& x) -> bool { return x == 0; });
}
