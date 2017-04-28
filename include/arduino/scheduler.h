static unsigned long milliOffset = 0; // adjustment for sleeping
#define currentMillis() (milliOffset + millis())

static unsigned long msHi, msLo;

#ifdef RX_DEBUG_OUT
static void logPair(char *prefix, unsigned long start, unsigned long delay) {
  RX_DEBUG_OUT.print(prefix);
  RX_DEBUG_OUT.print(start);
  RX_DEBUG_OUT.print(",");
  RX_DEBUG_OUT.println(delay);
}
#endif

class Scheduled {
 public:
  unsigned long start, interval;
  RxNode *node;
  Scheduled *prev, *next;

  Scheduled() { node = NULL; prev = next = NULL; }
  Scheduled(RxNode *src) {
    node = src;
    prev = next = NULL;
    node->schedule = this;
  }

  void clear() { node = NULL; }

  void setInterval(unsigned long millis) {
    interval = millis;
  }

  void advance() {
    // from previous time rather that now so as to not accumulate error
    start += interval;
  }

  // How long to wait before next scheduled time or 0 if already expired.
  // precondition: now >= start

  unsigned long waitTime(unsigned long now) {
    // Modulo math works here even if now has wrapped since start.
    unsigned long elapsed = now - start;
    return (interval > elapsed) ? interval - elapsed : 0;
  }

  // Last run time
  unsigned long lastMillis() {
    return start;
  }

  // Scheduled time
  unsigned long nextMillis() {
    unsigned long t = currentMillis();
    return t + waitTime(t);
  }

  void insBefore(Scheduled *pl) {
    prev = pl->prev;
    next = pl;
    next->prev = this;
    if (prev != NULL)
      prev->next = this;
  }

  void insAfter(Scheduled *pl) {
    prev = pl;
    next = pl->next;
    prev->next = this;
    if (next != NULL)
      next->prev = this;
  }

  void safeRemove() {
    if (prev != NULL) {
      prev->next = next;
      if (next != NULL) {
	next->prev = prev;
	next = NULL;
      }
      prev = NULL;
    }
  }
};

// PROTOTYPE
void app();

Scheduled unscheduled;
Scheduled always;
Scheduled interval;

void schedule(Scheduled *p) {
  unsigned long now = currentMillis();
  //logPair("NEXT ", p->start, p->interval);
  Scheduled *prev = &interval;
  for (Scheduled *si = interval.next; si != NULL; si = si->next) {
    //logPair("CMP ", si->waitTime(now), p->waitTime(now));
    if (si->waitTime(now) > p->waitTime(now)) break;
    prev = si;
  }
  p->insAfter(prev);
}

void registerScheduled(RxNode *node) {
  if (node->schedule == NULL) {
    Scheduled *p = new Scheduled(node);
    node->schedule = p;
    p->insAfter(&unscheduled);
    node->init();
  }
}

// This may be called within the invokation of the node's
// run method, and elsewhere.

void unregisterScheduled(RxNode *node) {
  if (node->schedule != NULL) {
    Scheduled *p = new Scheduled(node);
    p->clear();
  }
}

void scheduleAlways(RxNode *node) {
  Scheduled *p = (Scheduled *)(node->schedule);
  p->safeRemove();
  p->insAfter(&always);
}

void scheduleInterval(RxNode *node, unsigned long intervalMillis) {
  Scheduled *p = (Scheduled *)(node->schedule);
  p->safeRemove();
  p->start = currentMillis();
  p->setInterval(intervalMillis);
  schedule(p);
}

void setup() {
#ifdef __MQX__
  // This has to be done in setup on Udoo Neo
  Serial.begin(115200);
  if (serial_baud != 0)
    Serial0.begin(serial_baud);
#else
  if (serial_baud != 0) {
    Serial.begin(serial_baud);
    while (! Serial); // Wait until Serial is ready
  }
#endif
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
  rtc.begin(true);
#endif
  msHi = 0;
  msLo = currentMillis();
  //logPair("INIT ", msHi, msLo);
  app();
}

////////// Sleep management

// Sleepy flags
#define SLEEP_MGR_USB 0x01

static bool _sleepy = false;
static bool _sleepyUSB = false;

#define USB_WAKE_DELAY 500

void sleepDevices() {
  // Wait until pending data is sent.
  if (serial_baud != 0) {
    Serial.flush();
    Serial.end();
  }
#ifdef USBCON
  if (_sleepyUSB)
    USBDevice.detach();
#endif
}

void wakeDevices() {
#ifdef USBCON
  if (_sleepyUSB) {
    USBDevice.attach();
    delay(USB_WAKE_DELAY);
  }
#endif

  if (serial_baud != 0) {
    Serial.begin(serial_baud);
    while (! Serial);
  }
}

#if defined(ARDUINO_ARCH_AVR)
#include "avr-sleep.h"
#else
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
#include "samd-sleep.h"
#else
void initSleep(uint8_t flags) { }
#endif
#endif

void Sleepy(uint8_t flags) {
  initSleep(flags);
  _sleepyUSB = (flags & SLEEP_MGR_USB) != 0;
#ifdef USBCON
  if (!_sleepyUSB)
    USBDevice.detach();
#endif
  _sleepy = true;
}

void Sleepy() {
  Sleepy(0);
}


//////////////////// Main loop

void loop() {
  Scheduled *si, *next;

  if (always.next == NULL && interval.next == NULL) abort();

  unsigned long now = currentMillis();

  // detect when millis overflows
  if (now < msLo) msHi++;
  msLo = now;

  if (interval.next != NULL) {
    for (si = interval.next; si != NULL; si = next) {
      if (si->waitTime(now) > 0) break;
      next = si->next;
      si->safeRemove();
      if (si->node != NULL) {
	//logPair("CYCLE ", msHi, msLo);
	if (si->node->run() && si->node != NULL) {
	  si->advance();
	  schedule(si);
	} else if (si->node != NULL)
	  si->insAfter(&unscheduled);
	else
	  delete si;
      } else
	delete si;
    }
  }

  for (si = always.next; si != NULL; si = next) {
    next = si->next;
    if (si->node != NULL) {
      if (!si->node->run())
	si->safeRemove();
    } else {
      si->safeRemove();
      delete si;
    }
  }

  if (_sleepy && always.next == NULL && interval.next != NULL)
    doSleep();
}
