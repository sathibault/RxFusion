#include <Client.h>

// Network transport base class (referenced in foundation/network.h)
#define TRANSPORT_BASE Client

// Handle fatal errors
#define fatal(x) abort()

#ifdef __MQX__

// required by <functional>
namespace std {
  void __throw_bad_function_call() { while (true); }
}

unsigned int epoch_timestamp() {
  TIME_STRUCT time;
  _time_get(&time);
  return time.SECONDS;
}

#else

#ifdef ARDUINO_ARCH_AVR
#define MISSING_STDLIB
#include "include/arduino/avr-stdlib.h"
#else
// required by <functional>
namespace std {
  void __throw_bad_function_call() { abort(); }
}
#endif

#ifdef ARDUINO_ARCH_AVR
#ifndef SoftwareSerial_h
#error Please add SoftwareSerial to your project
#endif
#endif

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
#ifndef RTC_ZERO_H
#error Please include RTCZero library
#endif

RTCZero rtc;
unsigned int epoch_timestamp() { return rtc.getEpoch(); }
#else
unsigned int epoch_timestamp() { return 0; }
#endif

#endif

static unsigned long serial_baud = 0;

#include "include/foundation/rxfusion.h"
#include "include/foundation/network.h"
#include "include/arduino/io.h"
#include "include/arduino/scheduler.h"
#include "include/arduino/utils.h"
