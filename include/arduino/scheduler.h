#if 1
#define currentMillis() millis()
#else
// for testing
#define currentMillis() (millis() + (unsigned long)-100000)
#endif

// 12 hours
#define MAX_ALARM 43200000
#define MIN_ALARM 2000

static unsigned long msHi, msLo;

static void logPair(char *prefix, unsigned start, unsigned delay) {
  Serial.print(prefix);
  Serial.print(start);
  Serial.print(",");
  Serial.println(delay);
}

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

  void setInterval(unsigned millis) {
    interval = millis;
  }

  void advance() {
    // from previous time rather that now so as to not accumulate error
    start += interval;
  }

  // How long to wait before next scheduled time or 0 if already expired.
  unsigned long waitTime(unsigned long now) {
    // Modulo math works here even if now has wrapped since start.
    unsigned long elapsed = now - start;
    return (interval > elapsed) ? interval - elapsed : 0;
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

void scheduleInterval(RxNode *node, unsigned intervalMillis) {
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
  if (serial_baud != 0)
    Serial.begin(serial_baud);
#endif
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
  rtc.begin();
#endif
  msHi = 0;
  msLo = currentMillis();
  //logPair("INIT ", msHi, msLo);
  app();
}

static bool sleepOn = false;

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
void Sleepy() {
  if (always.next == NULL) {
    if (interval.next != NULL) {
      unsigned long n;

      uint8_t hr = rtc.getHours();
      uint8_t min = rtc.getMinutes();
      uint8_t sec = rtc.getSeconds();
      unsigned long now = currentMillis();
      unsigned long wait = interval.next->waitTime(now);
      if (wait < MIN_ALARM) return;
      if (wait > MAX_ALARM) wait = MAX_ALARM;
      wait -= 1000; // don't be late

      //logPair("T ",min,sec);

      n = wait / 3600000;
      hr += n;
      if (hr >= 24) hr -= 24;
      wait -= n * 3600000;

      n = wait / 60000;
      min += n;
      if (min >= 60) {
	min -= 60;
	hr += 1;
	if (hr >= 24) hr -= 24;
      }

      n = wait / 1000;
      sec += n;
      if (sec >= 60) {
	sec -= 60;
	min += 1;
	if (min >= 60) {
	  min -= 60;
	  hr += 1;
	  if (hr >= 24) hr -= 24;
	}
      }
      //logPair("A ",min,sec);

      sleepOn = true;
      rtc.setAlarmTime(hr, min, sec);
      rtc.enableAlarm(rtc.MATCH_HHMMSS);
      rtc.standbyMode();
    } else
      abort();
  }
}
#endif

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

  if (sleepOn) Sleepy();
}
