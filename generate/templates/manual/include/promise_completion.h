#ifndef PROMISE_COMPLETION
#define PROMISE_COMPLETION

#include <napi.h>
#include <uv.h>

#include "async_baton.h"

// PromiseCompletion forwards either the resolved result or the rejection reason
// to the native layer, once the promise completes
//
// inherits ObjectWrap so it can be used in v8 and managed by the garbage collector
// it isn't wired up to be instantiated or accessed from the JS layer other than
// for the purpose of promise result forwarding
class PromiseCompletion : public Napi::ObjectWrap<PromiseCompletion>
{
  // callback type called when a promise completes
  typedef void (*Callback) (bool isFulfilled, AsyncBaton *baton, Napi::Value resultOfPromise);

  static Napi::Value New(const Napi::CallbackInfo& info);
  static Napi::Value PromiseFulfilled(const Napi::CallbackInfo& info);
  static Napi::Value PromiseRejected(const Napi::CallbackInfo& info);

  // persistent handles for NAN_METHODs
  static Napi::FunctionReference newFn;
  static Napi::FunctionReference promiseFulfilled;
  static Napi::FunctionReference promiseRejected;

  static Napi::Value Bind(Napi::FunctionReference &method, Napi::Object object);
  static void CallCallback(bool isFulfilled, const Napi::CallbackInfo& info);

  // callback and baton stored for the promise that this PromiseCompletion is
  // attached to.  when the promise completes, the callback will be called with
  // the result, and the stored baton.
  Callback callback;
  AsyncBaton *baton;

  void Setup(Napi::Function thenFn, Napi::Value result, AsyncBaton *baton, Callback callback);
public:
  // If result is a promise, this will instantiate a new PromiseCompletion
  // and have it forward the promise result / reason via the baton and callback
  static bool ForwardIfPromise(Napi::Value result, AsyncBaton *baton, Callback callback);

  static void InitializeComponent();
};

#endif
