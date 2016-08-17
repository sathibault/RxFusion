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

/*
  RxFusion is a DSL.  The type system is designed to represent the
  composability of the various objects, constraining the construction
  of pipelines to the desired grammar.  The types do NOT necessarily
  represent the logical inheritance relations between objects.

  This is the RxFusion grammar:

  STATEMENT := FLOW ">>" SINK
  FLOW := PIPELINE | "(" FLOW { "&" FLOW } ")" | "(" FLOW { "^" FLOW } ")"
  PIPELINE := SOURCE { ">>" TRANSFORM }
  SOURCE := Originator | Generator ">>" Iterator | State ">>" Poller
  TRANSFORM := Operator | Intermediate ">>" Poller
  SINK := Consumer

  The following type productions generate the above grammar:

  Generator x Interator -> PipeNode
  State x Poller -> PipeNode

  Originator x Operator -> PipeNode
  PipeNode x Operator -> PipeNode

  Originator x Intermediate -> StateNode
  PipeNode x Intermediate -> StateNode
  StateNode x Poller -> PipeNode

  Originator x Consumer -> PipeEnd
  PipeNode x Consumer -> PipeEnd

  Store is both a Generator & Consumer.

  Implicit puller rule:
  Generator x Operator -> PipeNode
 */


// Base class of all nodes in the stream graph.

class RxNode {
 protected:
  // Common list for sources and subscribers
  class NodeList {
   protected:
    RxNode *element;
    NodeList *tail;
    NodeList(RxNode *x, NodeList *rest) { element = x; tail = rest; }

   public:
    RxNode *operator->() const { return element; }
    NodeList *next() { return tail; }
    void destroy() {
      if (tail != NULL) tail->destroy();
      delete this;
    }

    static void prepend(NodeList **head, RxNode *x) {
      *head = new NodeList(x, *head);
    }
    void remove(NodeList **head, RxNode *x) {
      if (element == x) {
	*head = tail;
	delete this;
      } else
	tail->remove(&tail, x);
    }
  };

  // Source specific node list
  class SourceList {
   private:
    NodeList *list;
   public:
    SourceList() { list = NULL; }
    ~SourceList() { if (list != NULL) list->destroy(); }

    bool empty() { return list == NULL; }
    void prepend(RxNode *node) { NodeList::prepend(&list, node); }
    void remove(RxNode *node) { if (list != NULL) list->remove(&list, node); }

    void unlink(RxNode *sink) {
      for (NodeList *node = list; node != NULL; node = node->next())
	(*node)->unsubscribe(sink);
      list->destroy();
      list = NULL;
    }
  };

  // Subscriber specific list
  class Subscribers {
   private:
    NodeList *list;
   public:
    Subscribers() { list = NULL; }
    ~Subscribers() { if (list != NULL) list->destroy(); }

    bool empty() { return list == NULL; }
    void prepend(RxNode *node) { NodeList::prepend(&list, node); }
    void remove(RxNode *node) { list->remove(&list, node); }

    void push(RxNode *source, void *value) {
      NodeList *next;
      for (NodeList *node = list; node != NULL; node = next) {
	next = node->next();
	(*node)->onData(source, value); // node may be removed during call
      }
    }

    void close(RxNode *source) {
      for (NodeList *node = list; node != NULL; node = node->next())
	(*node)->onClose(source);
      list->destroy();
      list = NULL;
    }
  };

  SourceList sources;
  Subscribers subscribers;

 public:
  // For platform-specific use
  void *schedule;

  RxNode() { schedule = NULL; }
  virtual ~RxNode() { }

  // Runnable interface
  virtual void init() {}
  virtual bool run() { return false; }

  // Observable interface
  virtual void onData(RxNode *publisher, void *value) = 0;
  virtual void onClose(RxNode *publisher) = 0;

  // Producer interface
  virtual void subscribe(RxNode *node) {  subscribers.prepend(node); }
  virtual void unsubscribe(RxNode *node) {
    if (!subscribers.empty()) {
      subscribers.remove(node);
      if (subscribers.empty() && !sources.empty()) {
	sources.unlink(this);
	delete this;
      }
    }
  }

  // Call onClose for each subscriber and make subscriber list
  // empty.  Receivers of onClose do not need to unsubscribe themselves.
  void close() {
    if (!subscribers.empty())
      subscribers.close(this);
  }

  // Subscribe to the source and add to our source list.
  void attach(RxNode *source) {
    source->subscribe(this);
    sources.prepend(source);
  }

  // Unsubscribe to any sources and make source list empty.
  void detach() {
    if (!sources.empty())
      sources.unlink(this);
  }
};

// This base class carries the output output type.  Construction operators
// do not reference this class, but temporary objects use it to hold a
// pointers.
template <class T> class Observable : public RxNode {
};

template <class T> class Originator : public Observable<T> {
 public:
  virtual ~Originator() { unregisterScheduled(this); }

  virtual void onData(RxNode *pub, void *val) {}
  virtual void onClose(RxNode *source) {}

  virtual void subscribe(RxNode *node) {
    if (this->subscribers.empty()) registerScheduled(this);
    Observable<T>::subscribe(node);
  }
};

template <class T> class EmbeddedOrigin : public Originator<T> {
 public:
  void push(const T data) { this->subscribers.push(this, (void *)&data); }
};

template <class T> class Generator : public RxNode {
 protected:
  Observable<T> *implicitFeed;
 public:
  Generator() { implicitFeed = NULL; }
  virtual ~Generator() {
    if (!sources.empty())
      fatal("Oops, tried to delete attached node.  Use globals for sources and consumers.");
  }

  void setImplicit(Observable<T> *feed) { implicitFeed = feed; }
  Observable<T> *Implicit() { return implicitFeed; }
  virtual void unsubscribe(RxNode *node) {
    if (node == implicitFeed)
      implicitFeed = NULL;
    RxNode::unsubscribe(node);
  }
  virtual void onData(RxNode *pub, void *val) {}
  virtual void onClose(RxNode *source) {}
  virtual void reset() = 0;
  virtual void next() = 0;
  virtual bool more() = 0;
};

// Fully typed operator T -> U
template <class T, class U> class Operator : public Observable<U> {
 public:
  virtual void onClose(RxNode *source) {
    this->sources.remove(source);
    this->close();
    delete this;
  }
};

template <class T> class Consumer : public RxNode {
 public:
  virtual ~Consumer() {
    if (!sources.empty())
      fatal("Oops, tried to delete attached node.  Use globals for sources and consumers.");
  }
  void subscribe(RxNode *subscriber) { }
  void unsubscribe(RxNode *subscriber) { }
  void onClose(RxNode *source) {
    this->sources.remove(source);
  }
};


template <class T> class State : public RxNode {
 private:
  bool complete;
 public:
  State() : complete(false) {}
  virtual void onData(RxNode *pub, void *val) {}
  virtual void onClose(RxNode *source) {
    this->sources.remove(source);
    complete = true;
  }
  virtual void get() = 0;
  virtual bool closed() { return complete; }
};

template <class T, class U> class Intermediate : public State<U> {
};

template <class T> class PipeNode {
 public:
  Observable<T> *node;
  PipeNode(Observable<T> *op) : node(op) {}
};

template <class T> class StateNode {
 public:
  State<T> *node;
  StateNode(State<T> *st) : node(st) {}
};

template <class T, class U> class Compound {
 public:
  RxNode *first;
  Observable<U> *last;
  Compound(RxNode *i, Observable<U> *o) : first(i), last(o) {}
};

void registerScheduled(RxNode *node);
void unregisterScheduled(RxNode *node);
void scheduleAlways(RxNode *node);
void scheduleInterval(RxNode *node, unsigned millis);
void runAll();
