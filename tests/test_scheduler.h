class Scheduled {
 public:
  uint32_t time, interval;
  RxNode *node;
  Scheduled *next;
  Scheduled(RxNode *src, Scheduled *nxt) { node=src; next=nxt; }
  Scheduled(RxNode *src, unsigned millis) {
    node=src;
    next=NULL;
    interval = millis;
  }
  Scheduled *prepend(Scheduled *pl) { next = pl; return this; }
  void insertAfter(Scheduled *pl) { next = pl->next; pl->next = this; }
};

class Scheduler {
  uint32_t now;
  Scheduled *runnables;
  Scheduled *always,*interval;
 public:
  Scheduler() { runnables = NULL; always = NULL; interval = NULL; }

  void schedule(Scheduled *p) {
    p->time += p->interval;
    Scheduled *prev = NULL;
    for (Scheduled *pi = interval; pi != NULL; pi = pi->next) {
      if (pi->time > p->time)
	break;
      prev = pi;
    }
    if (prev != NULL)
      p->insertAfter(prev);
    else
      interval = p->prepend(interval);
  }

  void add(RxNode *node) {
    Scheduled *s = runnables;
    while (s != NULL && s->node != node) s = s->next;
    if (s == NULL) {
      runnables = new Scheduled(node, runnables);
      node->init();
    }
  }

  void addAlways(RxNode *node) {
    always = new Scheduled(node, always);
  }

  void addInterval(RxNode *node, unsigned millis) {
    Scheduled *p = new Scheduled(node, millis);
    p->time = now;
    schedule(p);
  }

  void run() {
    Scheduled *s, *prev;
    
    now = 0;
    while (always != NULL || interval != NULL) {
      // Run scheduled
      if (interval != NULL) {
	for (s = interval; s != NULL; s = interval) {
	  if (now < s->time) break;
	  interval = interval->next;
	  if (s->node->run())
	    schedule(s);
	  else
	    delete s;
	}
      }
      // Run always
      prev = NULL;
      for (s = always; s != NULL; ) {
        if (s->node->run()) {
          s = s->next;
        } else {
          if (prev != NULL) {
            prev->next = s->next;
	    delete s;
            s = prev->next;
          } else {
            always = s->next;
	    delete s;
            s = always;
          }
	}
	prev = s;
      }
      if (interval != NULL)
	now = interval->time;
    }
    while (runnables != NULL) {
      s = runnables;
      runnables = runnables->next;
      delete s;
    }
  }
};

Scheduler scheduler;

void registerOrigin(RxNode *node) {
  scheduler.add(node);
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
