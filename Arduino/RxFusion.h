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

// required by <functional>
namespace std {
  void __throw_bad_function_call() { abort(); }
}

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

static unsigned serial_baud = 0;

#include "include/foundation/rxfusion.h"
#include "include/foundation/network.h"
#include "include/arduino/io.h"
#include "include/arduino/scheduler.h"
