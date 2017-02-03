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

template <class T> class Formated : public strgen {
 private:
  char *format;
  T *value;

  int nextSubst(const char *s, int pos) {
    while (s[pos] && s[pos] != '$') pos++;
    return pos;
  }
 public:
  Formated(const char *format) {
    int len = strlen(format) + 1;
    this->format = (char *)malloc(len);
    memcpy(this->format, format, len);
    value = NULL;
  }
  ~Formated() { free(format); }

  void set(T *val) { value = val; }

  void writeto(strbuf& buf) {
    int last = 0;
    int pos = nextSubst(format, last);
    while (format[pos] != 0) {
      if (pos > last)
	::writeto(buf, format+last, pos-last);
      pos++;
      switch (format[pos++]) {
      case '1':
	::writeto(buf, *value);
	break;
      case '$':
	::writeto(buf, '$');
	break;
      case 't':
	::writeto(buf, epoch_timestamp());
	break;
      }
      last = pos;
      pos = nextSubst(format, last);
    }
    if (pos > last)
      ::writeto(buf, format+last, pos-last);
  }
};

template <class T> class FormatOp : public Operator<T,xstring> {
 private:
  Formated<T> formatter;
 public:
  FormatOp(const char *format) : formatter(format) {}
  void onData(RxNode *pub, void *val) {
    formatter.set((T *)val);
    xstring x = &formatter;
    this->subscribers.push(this, &x);
  }
};

template <class T> Operator<T,xstring> *Format(const char *format) {
  return new FormatOp<T>(format);
}
