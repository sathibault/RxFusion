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
  // non-assignable
  bytebuf(const bytebuf& other) = delete;
  bytebuf& operator=(bytebuf const&) = delete;

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
  
  // Find first occurrence of pat, case insensitive.
  const char *findfirst(const char *s, const char *send, char pat) const {
    if (isupper(pat)) pat = tolower(pat);
    while (s < send) {
      char ch = *s;
      if (isupper(ch)) ch = tolower(ch);
      if (ch == pat)
	return s;
      s++;
    }
    return NULL;
  }

  bool glob(const char *s, const char *send, const char *p, const char *pend) const {
    while (s < send && p < pend) {
      char pat = *p++;
      if (pat == '*') {
	if (p == pend)
	  return true;
	pat = *p;
	while ((s = findfirst(s, send, pat)) != NULL) {
	  if (glob(s, send, p, pend))
	    return true;
	  s++;
	}
	return false;
      } else if (pat == '?') {
	s++;
      } else {
	char ch = *s++;
	if (isupper(ch)) ch = tolower(ch);
	if (isupper(pat)) pat = tolower(pat);
	if (ch != pat)
	  return false;
      }
    }
    return s == send && p == pend;
  }

 public:
  // non-assignable
  strbuf(const strbuf& other) = delete;
  strbuf& operator=(strbuf const&) = delete;

  strbuf(int block) {
    blockSize = block;
    size = block;
    buf = (char *)malloc(size * sizeof(char));
    end = 0;
  }
  strbuf() : strbuf(32) { }

  ~strbuf() { free(buf); }

  const char *c_str() const { buf[end] = 0; return buf; }
  const char *data() const { return buf; }
  uint16_t length() const { return end; }
  bool operator==(const char *str) const {
    size_t sz = strlen(str);
    return (sz == end) && memcmp(str, buf, sz) == 0;
  }
  void reset() { end = 0; }
  void reset(const char *s) { end = 0; append(s); }
  void reset(const char *s, int len) { end = 0; append(s, len); }
  void reset(const strbuf& other) { end = 0; append(other.data(), other.length()); }

  bool startswith(const char *prefix) const {
    size_t len = strlen(prefix);
    return (len <= end) ? memcmp(buf, prefix, len) == 0 : false;
  }

  bool endswith(const char *suffix) const {
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

  bool glob(const char *pattern, uint16_t patlen) const {
    return glob(buf, buf+end, pattern, pattern+patlen);
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

// A json string.  Having a distinct type allows us to know when a string
// should be quoted or already represents json.

class jsons : public strbuf {
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
inline void writeto(strbuf& buf, const strbuf& o) { buf.append(o.data(), o.length()); }
inline void writeto(strbuf& buf, xstring o) { o->writeto(buf); }

inline void writejson(strbuf& buf, int x) { buf.append(x); }
inline void writejson(strbuf& buf, unsigned int x) { buf.append(x); }
inline void writejson(strbuf& buf, float x) { buf.append(x); }
inline void writejson(strbuf& buf, const char *s) { buf.append('"'); buf.append(s); buf.append('"'); }
inline void writejson(strbuf& buf, const char *s, int len) { buf.append('"'); buf.append(s, len); buf.append('"'); }
inline void writejson(strbuf& buf, const strbuf& o) { buf.append('"'); buf.append(o.data(), o.length()); buf.append('"'); }
inline void writejson(strbuf& buf, const jsons& o) { buf.append(o.data(), o.length()); }
inline void writejson(strbuf& buf, xstring o) { buf.append('"'); o->writeto(buf); buf.append('"'); }
