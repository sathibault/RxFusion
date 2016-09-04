class Scheduled {
 public:
  uint32_t time, interval;
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

class Scheduler {
  uint32_t now;

  Scheduled unscheduled,deleted;
  Scheduled always;
  Scheduled interval;
 public:
  Scheduler() { }

  unsigned long millis() { return now; }

  void schedule(Scheduled *p) {
    // from previous time rather that now so as to not accumulate error
    p->time += p->interval;
    Scheduled *prev = &interval;
    for (Scheduled *si = interval.next; si != NULL; si = si->next) {
      if (si->time > p->time)
	break;
      prev = si;
    }
    p->insAfter(prev);
    //printf("NEXT %d.%09d\n", p->time.tv_sec,p->time.tv_nsec);
  }

  void add(RxNode *node) {
    if (node->schedule == NULL) {
      Scheduled *p = new Scheduled(node);
      node->schedule = p;
      p->insAfter(&unscheduled);
      node->init();
    }
  }

  void addAlways(RxNode *node) {
    Scheduled *p = (Scheduled *)(node->schedule);
    p->safeRemove();
    p->insAfter(&always);
  }

  void addInterval(RxNode *node, unsigned millis) {
    Scheduled *p = (Scheduled *)(node->schedule);
    p->safeRemove();
    p->time = now;
    p->setInterval(millis);
    schedule(p);
  }

  void remove(RxNode *node) {
    Scheduled *p = (Scheduled *)(node->schedule);
    if (p != NULL)
      p->clear();
  }

  void run() {
    Scheduled *si, *next;

    now = 0;
    while (always.next != NULL || interval.next != NULL) {
      // Run scheduled
      if (interval.next != NULL) {
	for (si = interval.next; si != NULL; si = next) {
	  if (now < si->time) break;
	  next = si->next;
	  si->safeRemove();
	  if (si->node != NULL) {
	    if (si->node->run() && si->node != NULL)
	      schedule(si);
	    else if (si->node != NULL)
	      si->insAfter(&unscheduled);
	    else
	      delete si;
	  } else
	    delete si;
	}
      }

      // Run always
      for (si = always.next; si != NULL; si = next) {
	next = si->next;
	if (si->node != NULL) {
	  if (!si->node->run() && si->node != NULL) {
	    si->safeRemove();
	    si->insAfter(&unscheduled);
	  }
	} else {
	  si->safeRemove();
	  delete si;
	}
      }
      if (interval.next != NULL)
	now = interval.next->time;
    }

    while (unscheduled.next != NULL) {
      si = unscheduled.next;
      si->safeRemove();
      delete si;
    }
  }
};

Scheduler scheduler;

unsigned long millis() {
  return scheduler.millis();
}

void registerScheduled(RxNode *node) {
  scheduler.add(node);
}
void unregisterScheduled(RxNode *node) {
  scheduler.remove(node);
}
void scheduleAlways(RxNode *node) {
  scheduler.addAlways(node);
}
void scheduleInterval(RxNode *node, unsigned millis) {
  scheduler.addInterval(node, millis);
}
void runAll() {
  scheduler.run();
}
