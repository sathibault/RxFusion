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

class bytebuf {
 private:
  uint8_t *buf;
  int size,end,blockSize;

  void fit(int len) {
    int need = end + len;
    if (size < need) {
      while (size < need) size += blockSize;
      buf = (uint8_t *)realloc(buf, size * sizeof(uint8_t));
    }
  }

 public:
  bytebuf(int block) {
    blockSize = block;
    size = block;
    buf = (uint8_t *)malloc(size * sizeof(uint8_t));
    end = 0;
  }
  ~bytebuf() { free(buf); }

  const uint8_t *data() { return buf; }
  uint16_t length() { return end; }

  void reset() { end = 0; }

  void append(const char *s) {
    while (*s) {
      if (end == size) fit(1);
      buf[end++] = *(s++);
    }
  }

  void append(const char *s, int len) {
    fit(len);
    memcpy(buf+end, s, len);
    end += len;
  }

  void append(uint8_t byte) {
    fit(1);
    buf[end++] = byte;
  }
};

class strbuf {
 private:
  char *buf;
  int size,end,blockSize;

  void fit(int len) {
    int need = end + len + 1; // room for terminator
    if (size < need) {
      while (size < need) size += blockSize;
      buf = (char *)realloc(buf, size * sizeof(char));
    }
  }

 public:
  strbuf(int block=80) {
    blockSize = block;
    size = block;
    buf = (char *)malloc(size * sizeof(char));
    end = 0;
  }
  ~strbuf() { free(buf); }

  const char *c_str() { buf[end] = 0; return buf; }
  uint16_t length() { return end; }

  void reset() { end = 0; }
  void reset(const char *s) { end = 0; append(s); }

  bool startswith(const char *prefix) {
    size_t len = strlen(prefix);
    return (len <= end) ? memcmp(buf, prefix, len) == 0 : false;
  }

  bool endswith(const char *suffix) {
    size_t len = strlen(suffix);
    return (len <= end) ? memcmp(buf-len, suffix, len) == 0 : false;
  }

  void append(const char *s) {
    while (*s) {
      if (end == size) fit(1);
      buf[end++] = *(s++);
    }
    if (end == size) fit(1); // always leave 1 available for nul
  }

  void append(const char *s, int len) {
    fit(len);
    memcpy(buf+end, s, len);
    end += len;
  }

  void append(char c) {
    fit(1);
    buf[end++] = c;
  }

  void append(int x) {
    fit(12);
    sprintf(buf+end, "%d", x);
    end += strlen(buf+end);
  }

  void append(unsigned int x) {
    fit(12);
    sprintf(buf+end, "%u", x);
    end += strlen(buf+end);
  }

  void append(float x) {
    fit(12);
    sprintf(buf+end, "%f", x);
    end += strlen(buf+end);
  }
};

// A lazy string type implemented as an abstract string generator
// class.  These are considered temporary and may be stack allocated.  They
// should not be stored, write them to a strbuf to copy the value.
class strgen {
 public:
  virtual void writeto(strbuf& buf) = 0;
};

// Make a pointer type so we can get it in/out vectors.  All string values
// in streams are this type.
typedef strgen *xstring;

// An xstring suitable substring.
class strslice : public strgen {
  const char *str;
  int len;
 public:
  void set(const char *s, int start, int slen) {
    str = s + start;
    len = slen;
  }
  void set(const char *s, int slen) {
    set(s, 0, slen);
  }
  void writeto(strbuf& buf) {
    buf.append(str, len);
  }
};

template<class T, int capacity> class xvec {
 protected:
  T data[capacity];
  uint16_t endPtr, n;
 public:
  xvec() { endPtr = capacity-1; n = 0; }
  bool isEmpty() { return n == 0; }
  bool isFull() { return n == capacity; }
  uint16_t count() { return n; }
  T& operator[](const uint16_t idx) {
    return (idx <= endPtr) ? data[endPtr-idx] : data[capacity-(idx-endPtr)];
  }
};

template<class T, int capacity> class xvecWr : public xvec<T,capacity> {
 private:
  int next(int i) { i++; return i<capacity ? i : 0; }
 public:
  void push(T& sample) {
    this->endPtr = next(this->endPtr);
    this->data[this->endPtr] = sample;
    if (this->n < capacity) this->n++;
  }
};

template<class T, int capacity> class fifo {
 protected:
  T data[capacity];
  int iptr, n;
 public:
  fifo() { iptr = 0; n = 0; }
  bool isEmpty() { return n == 0; }
  bool isFull() { return n == capacity; }
  int count() { return n; }
  // This is only valid b/c there is no pop operation
  int begin() { return (n == capacity) ? iptr : 0; }
  int end() { return previous(iptr); }
  int previous(int i) { return (i>0 ? i : capacity) - 1; }
  int next(int i) { i++; return i<capacity ? i : 0; }
  T& operator[](const int idx) { return data[idx]; }
};

template<class T, int capacity> class fifoWr : public fifo<T,capacity> {
 public:
  void reset() { this->iptr = 0; this->n = 0; }
  void push(T& sample) {
    this->data[this->iptr] = sample;
    this->iptr = this->next(this->iptr);
    if (this->n < capacity) this->n++;
  }
};

inline void writeto(strbuf& buf, const char *s) { buf.append(s); }
inline void writeto(strbuf& buf, const char *s, int len) { buf.append(s, len); }
inline void writeto(strbuf& buf, char x) { buf.append(x); }
inline void writeto(strbuf& buf, int x) { buf.append(x); }
inline void writeto(strbuf& buf, unsigned int x) { buf.append(x); }
inline void writeto(strbuf& buf, float x) { buf.append(x); }
inline void writeto(strbuf& buf, xstring o) { o->writeto(buf); }
