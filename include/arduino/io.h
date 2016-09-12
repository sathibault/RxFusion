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

template <class T> class BitIn : public State<T> {
 private:
  unsigned pin;
 public:
  BitIn(unsigned pin) : pin(pin) { pinMode(pin, INPUT); }
  void get() {
    T current = (T)digitalRead(pin);
    this->subscribers.push(this, &current);
  }
};

template <class T> class AnalogIn : public State<T> {
 private:
  unsigned pin;
 public:
  AnalogIn(unsigned pin) : pin(pin) { pinMode(pin, INPUT); }
  void get() {
    T current = (T)analogRead(pin);
    this->subscribers.push(this, &current);
  }
};

template <class T> class BitOut : public Consumer<T> {
 private:
  unsigned pin;
 public:
   BitOut(unsigned pin) : pin(pin) { pinMode(pin, OUTPUT); }
  void onData(RxNode *publisher, void *value) {
    if (*(T *)value)
      digitalWrite(pin, HIGH);
    else
      digitalWrite(pin, LOW);
  }
};

template <class T> class AnalogOut : public Consumer<T> {
 private:
  unsigned pin;
 public:
  AnalogOut(unsigned pin) : pin(pin) {
#ifndef __MQX__
    pinMode(pin, OUTPUT);
#endif
  }
  void onData(RxNode *publisher, void *value) {
    analogWrite(pin, *(T *)value);
  }
};

#ifdef __MQX__
template <class T> class Cpu : public Consumer<T> {
 private:
  strbuf sbuf;
 public:
  Cpu() : sbuf(80) {}
  void onData(RxNode *publisher, void *value) {
    sbuf.reset();
    writeto(sbuf, *(T *)value);
    sbuf.append("\r\n");
    Serial.write(sbuf.c_str(),sbuf.length());
  }
};

template <class T> class Console : public Consumer<T> {
 private:
  strbuf sbuf;
 public:
  Console(unsigned baud) : sbuf(80) {
    serial_baud = baud;
  }
  void onData(RxNode *publisher, void *value) {
    sbuf.reset();
    writeto(sbuf, *(T *)value);
    Serial0.println(sbuf.c_str());
  }
};
#else
template <class T> class Console : public Consumer<T> {
 private:
  strbuf sbuf;
 public:
  Console(unsigned baud) : sbuf(80) {
    serial_baud=baud;
  }
  void onData(RxNode *publisher, void *value) {
    sbuf.reset();
    writeto(sbuf, *(T *)value);
    Serial.println(sbuf.c_str());
  }
};
#endif
