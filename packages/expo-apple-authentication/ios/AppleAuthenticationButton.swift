import AuthenticationServices
import ExpoModulesCore

public final class AppleAuthenticationButton: ExpoView {
  let onButtonPress = EventDispatcher()

  var type: ASAuthorizationAppleIDButton.ButtonType = .default
  var style: ASAuthorizationAppleIDButton.Style = .white
  var childView: ASAuthorizationAppleIDButton?

  var needsUpdate = true

  var cornerRadius: Double = 0.0 {
    didSet {
      childView?.cornerRadius = cornerRadius
    }
  }

  public override var bounds: CGRect {
    didSet {
      childView?.frame = bounds
    }
  }

  func updateChildIfNeeded() {
    guard needsUpdate else {
      return
    }
    unmountChild()
    mountNewChild()
    needsUpdate = false
  }

  private func mountNewChild() {
    let newChildView = ASAuthorizationAppleIDButton(authorizationButtonType: type, authorizationButtonStyle: style)

    newChildView.frame = bounds
    newChildView.cornerRadius = cornerRadius
    newChildView.addTarget(self, action: #selector(onTouchUp), for: .touchUpInside)
    addSubview(newChildView)
    childView = newChildView
  }

  private func unmountChild() {
    childView?.removeTarget(self, action: #selector(onTouchUp), for: .touchUpInside)
    childView?.removeFromSuperview()
    childView = nil
  }

  @objc
  private func onTouchUp() {
    onButtonPress()
  }
}
