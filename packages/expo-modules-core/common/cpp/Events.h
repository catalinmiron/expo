// Copyright 2022-present 650 Industries. All rights reserved.

#include <unordered_map>
#include <list>
#include <jsi/jsi.h>

namespace jsi = facebook::jsi;

namespace expo {
namespace events {

using ListenerValue = std::shared_ptr<jsi::Value>;
using ListenersList = std::list<ListenerValue>;
using ListenersMap = std::unordered_map<std::string, ListenersList>;
using SharedListenersMap = std::shared_ptr<ListenersMap>;

/**
 Decorates the given JavaScript object to make it an event emitter,
 so simply implements `addListener` and `removeListener` functions.
 */
void decorateEventEmitter(jsi::Runtime &runtime, jsi::Object &jsObject);

/**
 Checks if it is possible to emit events to the given JavaScript object.
 */
bool canEmitEvents(jsi::Runtime &runtime, jsi::Object &jsObject);

/**
 Emits an event with the given payload to the JavaScript object. Before you call it,
 make sure that the object can emit events (use `canEmitEvents`) and run it from the JavaScript thread.
 */
void emitEvent(jsi::Runtime &runtime, jsi::Object &jsObject, std::string eventName, jsi::Value payload);

} // namespace events
} // namespace expo
