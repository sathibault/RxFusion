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

template <class T> class Doc : public Consumer<T> {
 private:
  strbuf sbuf;
 public:
  Doc() : sbuf(256) { }

  void clear() { sbuf.reset(); }
  const char *text() { sbuf.c_str(); }

  void onData(RxNode *publisher, void *value) {
    if (sbuf.length() > 0)
      sbuf.append(' ');
    writeto(sbuf, *(T *)value);
  }
};
