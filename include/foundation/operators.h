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
  return Map<T,U>([](const T& t) -> U { return t._1; });
}
template <class T, class U> MapOp<T,U> *Project2() {
  return Map<T,U>([](const T& t) -> U { return t._2; });
}
template <class T, class U> MapOp<T,U> *Project3() {
  return Map<T,U>([](const T& t) -> U { return t._3; });
}

template <class T, class U> MapOp<T,U> *Project4() {
  return Map<T,U>([](const T& t) -> U { return t._4; });
}

template <class T, class U> Operator<T,U> *Count() {
  return Scan<T,U>([](const T& cur, U& count) { count += 1; },
		   [](U& count) { count = 0; });
}

template <class T, int width> const Compound<T,T> Delay() {
  auto *win = new WindowOp<T,width>();
  auto *map = Map<fifo<T,width>,T>([](const fifo<T,width>& win) -> T {
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
  return Sliding<T,T,width>([](const T& elm, const T& accum) -> T {
      return accum + elm;
    }, [](const T& elm, T& accum) -> T {
      return accum - elm;
    }, T(0));
}

template <class T, int width> Operator<T,T> *SumOf() {
  return BatchFold1<T,T,width>([](const T& cur, const T& accum)->T { return accum+cur; });
}

template <class T> const Compound<T,T> SumOver(unsigned long millis) {
  auto *sum = IntermFold<T,T>([](const T& elm, const T& accum) -> T {
      return accum + elm;
    }, T(0));
  auto *poll = Poll<T>(millis);
  poll->setSource(sum);
  return Compound<T,T>(sum, poll);
}

template <class T, int width> const Compound<T,T> WinAverage() {
  auto *sum = Sliding<T,T,width>([](const T& elm, const T& accum)->T {
      return accum + elm;
    }, [](const T& elm, T& accum)->T {
      return accum - elm;
    }, T(0));
  auto *map = Map<T,T>([](const T& accum)->T { return accum/width; });
  map->attach(sum);
  return Compound<T,T>(sum, map);
}

template <class T, int width> const Compound<T,T> AverageOf() {
  auto *sum = BatchFold1<T,T,width>([](const T& cur, const T& accum)->T { return accum+cur; });
  auto *map = Map<T,T>([](const T& accum)->T { return accum/width; });
  map->attach(sum);
  return Compound<T,T>(sum, map);
}

template <class T> const Compound<T,T> AverageOver(unsigned long millis) {
  auto *sum = IntermFold<T,Tuple2<T,int>>([](const T& elm, const Tuple2<T,int>& t)->Tuple2<T,int> {
      return Tuple2<T,int>(t._1 + elm, t._2 + 1);
    }, Tuple2<T,int>(0,0));
  auto *poll = Poll<Tuple2<T,int>>(millis);
  auto *map = Map<Tuple2<T,int>,T>([](const Tuple2<T,int>& t)->T {
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
  return BatchFold1<T,T,width>([](const T& cur, const T& accum)->T {
      return (cur < accum) ? cur : accum;
    });
}

template <class T> const Compound<T,T> MinOver(unsigned long millis) {
  auto *fold = IntermFold<T,T>([](const T& elm, const T& accum) -> T {
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
  return BatchFold1<T,T,width>([](const T& cur, const T& accum)->T {
      return (cur > accum) ? cur : accum;
    });
}

template <class T> const Compound<T,T> MaxOver(unsigned long millis) {
  auto *fold = IntermFold<T,T>([](const T& elm, const T& accum) -> T {
      return (elm > accum) ? elm : accum;
    }, std::numeric_limits<T>::min());
  auto *poll = Poll<T>(millis);
  poll->setSource(fold);
  return Compound<T,T>(fold, poll);
}

//////////////////////////////////////// Math

template <class T> Operator<T,T> *Scale(T k) {
  return Map<T,T>([k](const T& x) -> T { return k*x; });
}

template <class T> Operator<T,T> *Add(T b) {
  return Map<T,T>([b](const T& x) -> T { return x+b; });
}

template <class T> Operator<T,T> *ClampHigh(T u) {
  return Map<T,T>([u](const T& x) -> T { return x > u ? u : x; });
}

template <class T> Operator<T,T> *ClampLow(T l) {
  return Map<T,T>([l](const T& x) -> T { return x < l ? l : x; });
}

template <class T> Operator<T,T> *Clamp(T l, T u) {
  return Map<T,T>([l,u](const T& x) -> T { return x < l ? l : (x > u ? u : x); });
}

template <class T, class U> Operator<T,U> *Toggle(U initial) {
  return new ScanOp<T,U>([](const T& data, const U& state) -> U {
      return !state;
    }, initial);
}

template <class T> Operator<T,T> *Abs() {
  return Map<T,T>([](const T& x) -> T { return x < 0 ? -x : x; });
}

template <class T, class U> Operator<T,U> *Const(U c) {
  return Map<T,U>([c](const T& x) -> T { return c; });
}


//////////////////////////////////////// Filtering

template <class T> const Compound<T,T> Drop(int drop) {
  auto *cnt = Counted<T,int>();
  auto *filter = Filter<Tuple2<T,int>>([drop](const Tuple2<T,int>& t) -> bool {
      return t._2 > drop;
    });
  auto *prj = Project1<Tuple2<T,int>,T>();
  filter->attach(cnt);
  prj->attach(filter);
  return Compound<T,T>(cnt, prj);
}

template <class T> const Compound<T,T> Take(int take) {
  auto *cnt = Counted<T,int>();
  auto *traffic = Traffic<Tuple2<T,int>>([take](const Tuple2<T,int>& t) -> Action {
      return (t._2 <= take) ? TPass : TClose;
    });
  auto *prj = Project1<Tuple2<T,int>,T>();
  traffic->attach(cnt);
  prj->attach(traffic);
  return Compound<T,T>(cnt, prj);
}

template <class T> const Compound<T,T> Dedup() {
  auto *scan = Scan<T,Tuple2<Maybe<T>,bool>>([](const T& cur, const Tuple2<Maybe<T>,bool>& state) -> Tuple2<Maybe<T>,bool> {
      return Tuple2<Maybe<T>,bool>(Maybe<T>(cur),
				   !state._1.exists || cur != state._1.value);
    }, Tuple2<Maybe<T>,bool>());
  auto *filter = Filter<Tuple2<Maybe<T>,bool>>([](const Tuple2<Maybe<T>,bool>& x) -> bool {
      return x._2;
    });
  auto *prj = Map<Tuple2<Maybe<T>,bool>,T>([](const Tuple2<Maybe<T>,bool>& x) -> T {
      return x._1.value;
    });
  filter->attach(scan);
  prj->attach(filter);
  return Compound<T,T>(scan, prj);
}

template <class T> const Compound<T,T> Dedup(unsigned long expire) {
  // Previous value, timestamp, emit
  typedef Tuple3<Maybe<T>,unsigned long,bool> DedupState;

  auto *scan = Scan<T,DedupState>([expire](const T& cur, const DedupState& state) -> DedupState {
      unsigned long now = millis();
      unsigned long age = now - state._2; // indepotent to wrapping
      bool emit = !state._1.exists || cur != state._1.value || age >= expire;
      unsigned long t = emit ? now : state._2;
      return DedupState(Maybe<T>(cur),t,emit);
    }, DedupState(Maybe<T>(),0,false));
  auto *filter = Filter<DedupState>([](const DedupState& x) -> bool {
      return x._3;
    });
  auto *prj = Map<DedupState,T>([](const DedupState& x) -> T { return x._1.value; });
  filter->attach(scan);
  prj->attach(filter);
  return Compound<T,T>(scan, prj);
}

Operator<bool,bool> *True() {
  return Filter<bool>([](const bool& x) -> bool { return x; });
}

Operator<bool,bool> *False() {
  return Filter<bool>([](const bool& x) -> bool { return !x; });
}

template <class T> Operator<T,T> *Over(T th) {
  return Filter<T>([th](const T& x) -> bool { return x > th; });
}

template <class T> Operator<T,T> *Under(T th) {
  return Filter<T>([th](const T& x) -> bool { return x < th; });
}

template <class T> Operator<T,T> *AtLeast(T th) {
  return Filter<T>([th](const T& x) -> bool { return x >= th; });
}

template <class T> Operator<T,T> *AtMost(T th) {
  return Filter<T>([th](const T& x) -> bool { return x <= th; });
}

template <class T> Operator<T,T> *Positive() {
  return Filter<T>([](const T& x) -> bool { return x > 0; });
}

template <class T> Operator<T,T> *Negative() {
  return Filter<T>([](const T& x) -> bool { return x < 0; });
}

template <class T> Operator<T,T> *Zero() {
  return Filter<T>([](const T& x) -> bool { return x == 0; });
}

template <class T> Operator<T,bool> *IsOver(T th) {
  return Map<T,bool>([th](const T& x) -> bool { return x > th; });
}

template <class T> Operator<T,bool> *IsUnder(T th) {
  return Map<T,bool>([th](const T& x) -> bool { return x < th; });
}

template <class T> Operator<T,bool> *IsAtLeast(T th) {
  return Map<T,bool>([th](const T& x) -> bool { return x >= th; });
}

template <class T> Operator<T,bool> *IsAtMost(T th) {
  return Map<T,bool>([th](const T& x) -> bool { return x <= th; });
}

template <class T> Operator<T,bool> *IsPositive() {
  return Map<T,bool>([](const T& x) -> bool { return x > 0; });
}

template <class T> Operator<T,bool> *IsNegative() {
  return Map<T,bool>([](const T& x) -> bool { return x < 0; });
}

template <class T> Operator<T,bool> *IsZero() {
  return Map<T,bool>([](const T& x) -> bool { return x == 0; });
}

template <class T> Operator<T,strbuf> *String() {
  return Scan<T,strbuf>([](const T& src, strbuf& str) {
      str.reset();
      writeto(str, src);
      puts(str.c_str());
    });
}

Operator<char,strbuf> *Lines() {
  return Build<char,bool,strbuf>([](const char& ch, bool& nl, strbuf& result) -> bool {
    if (nl)
      result.reset();
    if (ch != '\r' && ch != '\n')
      result.append(ch);
    nl = (ch == '\n');
    return nl;
  });
}

template <int max> Operator<strbuf,Vec<strbuf,max>> *Split(char delim) {
  return Scan<strbuf,Vec<strbuf,max>>([delim](const strbuf& src, Vec<strbuf,max>& fields) {
      int idx = 0;
      int last = 0;
      const char *s = src.c_str();
      for (int i = 0; s[i] != 0; i++) {
	if (s[i] == delim) {
	  fields[idx].reset(s+last, i-last);
	  idx++;
	  last = i + 1;
	}
      }
      if (last < src.length()) {
	fields[idx].reset(s+last, src.length()-last);
	idx++;
      }
      while (idx < max) {
	fields[idx].reset();
	idx++;
      }
      return true;
    });
}
