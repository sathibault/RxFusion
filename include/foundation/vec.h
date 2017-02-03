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

template <class T, int len> class Vec {
 protected:
  T data[len];
 public:
  T& operator[](const uint16_t idx) {
    return data[idx];
  }
  const T& operator[](const uint16_t idx) const {
    return data[idx];
  }
};

template <class T, int len> void writeto(strbuf& sbuf, Vec<T,len>& t) {
  writeto(sbuf, '[');
  if (len > 0) {
    writeto(sbuf, t[0]);
    for (int i = 1; i < len; i++) {
      writeto(sbuf, ',');
      writeto(sbuf, t[i]);
    }
  }
  writeto(sbuf, ']');
}
