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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "storage.h"

strbuf::strbuf(int block) {
  blockSize = block;
  size = block;
  buf = new char[size];
}

strbuf::~strbuf() {
  delete buf; 
}

void strbuf::fit(int len) {
  int need = end + len;
  if (size < need) {
    char *old = buf;
    while (size < need) size += blockSize;
    buf = new char[size];
    if (end > 0)
      memcpy(buf, old, end);
    delete old;
  }
}

void strbuf::append(const char *s, int len) {
  fit(len);
  memcpy(buf+end, s, len);
  end += len;
}

void strbuf::append(int x) {
  fit(12);
  sprintf(buf+end, "%d", x);
  end += strlen(buf+end);
}


void strbuf::append(float x) {
  fit(12);
  sprintf(buf+end, "%x", x);
  end += strlen(buf+end);
}
