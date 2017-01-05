#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <uv.h>
#include <node.h>
#include <node_object_wrap.h>
#include <node_buffer.h>

using namespace v8;

class SerialTTY : public node::ObjectWrap {
public:
  static void Init(Local<Object> exports);

private:
  static Persistent<Function> onData;

  explicit SerialTTY(const char *tty);
  ~SerialTTY();

  static void New(const FunctionCallbackInfo<Value>& args);
  static void On(const v8::FunctionCallbackInfo<v8::Value>& args);

  static Persistent<Function> constructor;
  int _fd;
  uv_poll_t _poll;
public:
  void onReadable();
};

extern "C" int open_tty(const char *ttyname);

static void tty_poll(uv_poll_t* handle, int status, int events) {
  //printf("%d %d\n", status,events);
  ((SerialTTY *)handle->data)->onReadable();
}

Persistent<Function> SerialTTY::constructor;
Persistent<Function> SerialTTY::onData;


SerialTTY::SerialTTY(const char *tty) {
  _fd = open_tty(tty);
  if (_fd > 0) {
    uv_poll_init(uv_default_loop(), &_poll, _fd);
    _poll.data = this;
    uv_poll_start(&_poll, UV_READABLE, tty_poll);
  }
}

SerialTTY::~SerialTTY() {
  if (_fd > 0)
    close(_fd);
}

void SerialTTY::Init(Local<Object> exports) {
  Isolate* isolate = exports->GetIsolate();

  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "SerialTTY"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "on", On);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "SerialTTY"),
	       tpl->GetFunction());
}

void SerialTTY::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (args.IsConstructCall()) {
    String::Utf8Value tty(args[0]);
    SerialTTY *obj = new SerialTTY(*tty);
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    const int argc = 1;
    Local<Value> argv[argc] = { args[0] };
    Local<Context> context = isolate->GetCurrentContext();
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    Local<Object> result =
      cons->NewInstance(context, argc, argv).ToLocalChecked();
    args.GetReturnValue().Set(result);
  }
}

void SerialTTY::On(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate *isolate = args.GetIsolate();

  SerialTTY *tty = ObjectWrap::Unwrap<SerialTTY>(args.Holder());

  if (args.Length() < 2) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  String::Utf8Value event(args[0]);
  Local<Function> cb = Local<Function>::Cast(args[1]);

  if (strcmp(*event, "data") == 0)
    tty->onData.Reset(isolate, cb);
}

void SerialTTY::onReadable() {
  int res;
  char buf[256];
  
  res = read(_fd,buf,255);
  while (res > 0) {
    if (!onData.IsEmpty()) {
      Isolate *isolate = Isolate::GetCurrent();
      
      HandleScope scope(isolate);
      Local<Function> cb = Local<Function>::New(isolate, onData);
      MaybeLocal<Object> buffer = node::Buffer::Copy(isolate, buf, res);
      if (!buffer.IsEmpty()) {
	Local<Value> argv[1] = { buffer.ToLocalChecked() };
	cb->Call(Null(isolate), 1, argv);
      }
    }
    res = read(_fd,buf,255);
  }
}

void InitAll(Local<Object> exports) {
  SerialTTY::Init(exports);
}

NODE_MODULE(udoo, InitAll)
