// Copyright 2022-present 650 Industries. All rights reserved.

#include "Events.h"

using namespace facebook;

namespace expo {
namespace events {

#pragma mark - Privates

static const char *addListenerFunctionName = "addListener";
static const char *removeListenersFunctionName = "removeListeners";
static const char *emitFunctionName = "__emit__";

/**
 Creates an event listener subscription.
 It's a plain object containing a "remove" function that removes the listener from the object.
 */
jsi::Value createListenerSubscription(jsi::Runtime &runtime, SharedListenersMap listenersMap, std::string eventName, ListenerValue listener) {
  jsi::HostFunctionType function = [=](jsi::Runtime &runtime,
                                       const jsi::Value &thisValue,
                                       const jsi::Value *args,
                                       size_t count) -> jsi::Value {
    ListenersList &list = (*listenersMap)[eventName];
    size_t listSizeBefore = list.size();

    list.remove_if([&](const ListenerValue &value) {
      return jsi::Value::strictEquals(runtime, *listener, *value);
    });

    // It's been the last listener, so let's stop observing.
    if (listSizeBefore > 0 && list.size() == 0) {
      listenersMap->erase(eventName);

      jsi::Object thisObject = thisValue.getObject(runtime);

      thisObject
        .getPropertyAsFunction(runtime, "stopObserving")
        .callWithThis(runtime, thisObject, {});
    }
    return jsi::Value::undefined();
  };
  jsi::Object subscription = jsi::Object(runtime);
  jsi::PropNameID functionName = jsi::PropNameID::forAscii(runtime, "remove");

  subscription.setProperty(runtime,
                           functionName,
                           jsi::Function::createFromHostFunction(runtime, functionName, 0, function));
  return subscription;
}

/**
 Creates a JavaScript function that adds event listeners to the given map.
 */
jsi::Function createAddListenerFunction(jsi::Runtime &runtime, SharedListenersMap listenersMap) {
  jsi::HostFunctionType function = [=](jsi::Runtime &runtime,
                                       const jsi::Value &thisValue,
                                       const jsi::Value *args,
                                       size_t count) -> jsi::Value {
    // TODO: Validate argument types
    std::string eventName = args[0].getString(runtime).utf8(runtime);
    ListenerValue listener = std::make_shared<jsi::Value>(runtime, args[1]);
    ListenersList &list = (*listenersMap)[eventName];

    list.push_back(listener);

    // It's the first listener
    if (list.size() == 1) {
      jsi::Object thisObject = thisValue.getObject(runtime);

      thisObject
        .getPropertyAsFunction(runtime, "startObserving")
        .callWithThis(runtime, thisObject, {});
    }
    return createListenerSubscription(runtime, listenersMap, eventName, listener);
  };
  jsi::PropNameID functionName = jsi::PropNameID::forAscii(runtime, addListenerFunctionName);
  return jsi::Function::createFromHostFunction(runtime, functionName, 2, function);
}

/**
 Creates a JavaScript function that removes all event listeners from the given map.
 */
jsi::Function createRemoveListenersFunction(jsi::Runtime &runtime, SharedListenersMap listenersMap) {
  jsi::HostFunctionType function = [=](jsi::Runtime &runtime,
                                       const jsi::Value &thisValue,
                                       const jsi::Value *args,
                                       size_t count) -> jsi::Value {
    listenersMap->clear();
    return jsi::Value::undefined();
  };
  jsi::PropNameID functionName = jsi::PropNameID::forAscii(runtime, removeListenersFunctionName);
  return jsi::Function::createFromHostFunction(runtime, functionName, 2, function);
}

/**
 Creates a JavaScript function that emits the event and calls registered listeners.
 */
jsi::Function createEmitFunction(jsi::Runtime &runtime, SharedListenersMap listenersMap) {
  jsi::HostFunctionType function = [=](jsi::Runtime &runtime,
                                       const jsi::Value &thisValue,
                                       const jsi::Value *args,
                                       size_t count) -> jsi::Value {
    // TODO: Validate argument types
    std::string eventName = args[0].getString(runtime).utf8(runtime);
    jsi::Object thisObject = thisValue.getObject(runtime);
    ListenersList &list = (*listenersMap)[eventName];

    for (const ListenerValue &listener : list) {
      if (!listener->isObject()) {
        continue;
      }
      listener->getObject(runtime)
        .getFunction(runtime)
        .callWithThis(runtime, thisObject, {
          jsi::Value(runtime, args[1]),
        });
    }
    return jsi::Value::undefined();
  };
  jsi::PropNameID functionName = jsi::PropNameID::forAscii(runtime, emitFunctionName);
  return jsi::Function::createFromHostFunction(runtime, functionName, 2, function);
}

#pragma mark - Publics

void decorateEventEmitter(jsi::Runtime &runtime, jsi::Object &jsObject) {
  SharedListenersMap listenersMap = std::make_shared<ListenersMap>();

  jsObject.setProperty(runtime, addListenerFunctionName, createAddListenerFunction(runtime, listenersMap));
  jsObject.setProperty(runtime, removeListenersFunctionName, createRemoveListenersFunction(runtime, listenersMap));

  // TODO: Make it a non-enumerable property
  jsObject.setProperty(runtime, emitFunctionName, createEmitFunction(runtime, listenersMap));
}

bool canEmitEvents(jsi::Runtime &runtime, jsi::Object &jsObject) {
  return jsObject.hasProperty(runtime, emitFunctionName);
}

void emitEvent(jsi::Runtime &runtime, jsi::Object &jsObject, std::string eventName, jsi::Value payload) {
  jsObject.getPropertyAsFunction(runtime, emitFunctionName).callWithThis(runtime, jsObject, {
    jsi::Value(runtime, jsi::String::createFromUtf8(runtime, eventName)),
    jsi::Value(runtime, payload)
  });
}

} // namespace events
} // namespace expo
