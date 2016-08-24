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

#ifdef __MINGW32__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <ctime>
#include <sstream>

#define CONSTRUCTED runAll();
void fatal(const char *msg) { puts(msg); exit(1); }
unsigned int epoch_timestamp() { return time(NULL); }

#include "rxfusion.h"

template <class T> class Console : public Consumer<T> {
 private:
  strbuf sbuf;
 public:
  Console() : sbuf(80) { }
  void onData(RxNode *publisher, void *value) {
    writeto(sbuf, *(T *)value);
    puts(sbuf.c_str());
    fflush(stdout);
    sbuf.reset();
  }
};

#define ISEARLIER(t1, t2) ((t1.tv_sec < t2.tv_sec) || (t1.tv_sec == t2.tv_sec && (t1.tv_nsec < t2.tv_nsec)))

class Scheduled {
 public:
  struct timespec time, interval;
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
    interval.tv_sec = millis / 1000;
    interval.tv_nsec = (millis - interval.tv_sec * 1000) * 1000000;
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

  void remove() {
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
  struct timespec now;
  Scheduled always,interval,unscheduled;
 public:
  void waitUntil(Scheduled *p) {
    unsigned millis;
    struct timespec delay;
    delay.tv_sec = p->time.tv_sec - now.tv_sec;
    delay.tv_nsec = p->time.tv_nsec - now.tv_nsec;
    if (delay.tv_nsec < 0) {
      delay.tv_sec--;
      delay.tv_nsec += 1000000000;
    }
    millis = delay.tv_sec * 1000;
    millis += delay.tv_nsec / 1000000;
    millis++; // a little over to be sure it runs
    Sleep(millis);
  }

  void schedule(Scheduled *p) {
    // from previous time rather that now so as to not accumulate error
    p->time.tv_sec += p->interval.tv_sec;
    p->time.tv_nsec += p->interval.tv_nsec;
    if (p->time.tv_nsec > 1000000000) {
      p->time.tv_sec++;
      p->time.tv_nsec -= 1000000000;
    }
    Scheduled *prev = &interval;
    for (Scheduled *pi = interval.next; pi != NULL; pi = pi->next) {
      if (!ISEARLIER(pi->time, p->time))
	break;
      prev = pi;
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
    p->remove();
    p->insAfter(&always);
  }

  void addInterval(RxNode *node, unsigned millis) {
    Scheduled *p = (Scheduled *)(node->schedule);
    p->setInterval(millis);
    clock_gettime(CLOCK_REALTIME,&(p->time));
    p->remove();
    schedule(p);
  }

  void remove(RxNode *node) {
    Scheduled *p = (Scheduled *)(node->schedule);
    if (p != NULL) {
      p->remove();
      p->clear();
    }
  }

  void run() {
    Scheduled *s, *next;

    while (always.next != NULL || interval.next != NULL) {
      // Run scheduled
      if (interval.next != NULL) {
	clock_gettime(CLOCK_REALTIME,&now);
	//printf("NOW %d.%09d\n", now.tv_sec,now.tv_nsec);
	for (s = interval.next; s != NULL; s = next) {
	  if (ISEARLIER(now, s->time)) break;
	  next = s->next;
	  if (s->node != NULL) {
	    s->remove();
	    if (s->node->run() && s->node != NULL)
	      schedule(s);
	    else if (s->node != NULL)
	      s->insAfter(&unscheduled);
	    else
	      delete s;
	  } else
	    delete s;
	}
      }

      // Run always
      for (s = always.next; s != NULL; s = next) {
	next = s->next;
	if (s->node != NULL) {
	  if (!s->node->run() && s->node != NULL) {
	    s->remove();
	    s->insAfter(&unscheduled);
	  }
	} else
	  delete s;
      }
      
      if (always.next == NULL)
	if (interval.next != NULL)
	  waitUntil(interval.next);
    }
    while (unscheduled.next != NULL) {
      s = unscheduled.next;
      s->remove();
      delete s;
    }
  }
};

Scheduler scheduler;

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
