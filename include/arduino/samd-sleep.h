// Atmel SAMD sleep management

#include <time.h>

static uint32_t wokeRTC;
static unsigned long wokeMillis;

void initSleep(uint8_t flags) {
  wokeRTC = rtc.getEpoch();
  // At this point, we have no idea what millis() was at the beginning of
  // this RTC second, so this initial value of wokeMillis is kind of arbitrary.
  // After the first sleep though, wokeMillis will be properly aligned
  // with wokeRTC.
  // Since its arbitrary, try to pick a value that will line up the program's
  // schedule early in the second so we are not wasting time waiting when
  // we wake up.
  unsigned long first;
  if (interval.next != NULL) {
    // User called Sleepy after construction so we know the schedule
    first = interval.next->lastMillis();
  } else {
    // User hasn't constructed yet, but is probably about to so assume now
    // is the start time.
    first = currentMillis();
  }
  // Wake up just before first scheduled time.
  if (!_sleepyUSB)
    wokeMillis = (first >= 10) ? first - 10 : 0;
  else
    wokeMillis = (first >= USB_WAKE_DELAY) ? first - USB_WAKE_DELAY : 0;

#ifdef RX_DEBUG_OUT
  RX_DEBUG_OUT.print("I ");
  RX_DEBUG_OUT.print(wokeRTC);
  RX_DEBUG_OUT.print(' ');
  RX_DEBUG_OUT.println(wokeMillis);
#endif
}

// 12 hours in seconds
#define MAX_ALARM 43200

// precondition: interval.next != NULL
// All computations are based off wokeRTC/wokeMillis which represent the
// same time point and are aligned to a second boundary.

void doSleep() {
  time_t target;
  struct tm* timep;
  unsigned long now, t, delta, secs;

  // precheck before we shut things down, 2s minimum sleep
  now = currentMillis();
  if (interval.next->waitTime(now) < 2100)
    return;

#ifdef RX_DEBUG_OUT
  RX_DEBUG_OUT.print("S ");
  RX_DEBUG_OUT.print(currentMillis());
  RX_DEBUG_OUT.print(' ');
  RX_DEBUG_OUT.println(interval.next->nextMillis());
#endif

  sleepDevices();

  delta = (interval.next->nextMillis() - wokeMillis) / 1000;
  target = wokeRTC + delta;

  t = rtc.getEpoch();
  secs = (target > t) ? target - t : 0;

#ifdef RX_DEBUG_OUT
  RX_DEBUG_OUT.print(delta);
  RX_DEBUG_OUT.print(' ');
  RX_DEBUG_OUT.print(t);
  RX_DEBUG_OUT.print(' ');
  RX_DEBUG_OUT.println(target);
#endif

  if (secs >= 2) { // Safety check
    if (secs > MAX_ALARM) {
      secs = MAX_ALARM;
      target = t + MAX_ALARM;
    }
    // Sleep
    timep = gmtime(&target);
    rtc.setAlarmTime(timep->tm_hour, timep->tm_min, timep->tm_sec);
    rtc.enableAlarm(rtc.MATCH_HHMMSS);
    rtc.standbyMode();
  
    // Wake up
    wokeMillis += (target - wokeRTC) * 1000;
    milliOffset = wokeMillis - millis();
    // should be target, but during first sleep after power up RTC jumps ahead
    wokeRTC = rtc.getEpoch();
    
#ifdef RX_DEBUG_OUT
    RX_DEBUG_OUT.print(wokeRTC);
    RX_DEBUG_OUT.print(' ');
    RX_DEBUG_OUT.println(wokeMillis);
#endif
  }

  wakeDevices();
}
