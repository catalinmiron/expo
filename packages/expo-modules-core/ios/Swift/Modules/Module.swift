// Copyright 2021-present 650 Industries. All rights reserved.

/**
 `BaseModule` is just a stub class that fulfils `AnyModule` protocol requirement of public default initializer,
 but doesn't implement that protocol explicitly, though — it would have to provide a definition which would require
 other modules to use `override` keyword in the function returning the definition.
 */
open class BaseModule {
  public private(set) weak var appContext: AppContext?
  public weak var javaScriptObject: JavaScriptObject?

  @available(*, unavailable, message: "Module's initializer cannot be overriden, use \"onCreate\" definition component instead.")
  public init() {}

  required public init(appContext: AppContext) {
    self.appContext = appContext
  }

  /**
   Sends an event with given name and payload to JavaScript.
   */
  public func sendEvent(_ eventName: String, _ payload: [String: Any?] = [:]) {
    // Send the event to the global event emitter that can be captured from the modules proxy (bridge)
    appContext?.eventEmitter?.sendEvent(withName: eventName, body: payload)

    appContext?.runOnJavaScriptThread { [weak self] in
      // Send the event to the underlying JavaScript object (JSI)
      self?.javaScriptObject?.emitEvent(eventName, payload: payload)
    }
  }
}

/**
 An alias for `AnyModule` extended by the `BaseModule` class that provides public default initializer.
 */
public typealias Module = AnyModule & BaseModule
