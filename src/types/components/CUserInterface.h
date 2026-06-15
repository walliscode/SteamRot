
/////////////////////////////////////////////////
/// Preprocessor directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Component.h"
#include "UIPriorityTier.h"
#include "UIElement.h"
#include <memory>

namespace steamrot {

struct CUserInterface : public Component {

  /////////////////////////////////////////////////
  /// @brief Default constructor
  /////////////////////////////////////////////////
  CUserInterface() = default;

  /////////////////////////////////////////////////
  /// @brief Copy constructor for deep copying
  ///
  /// @param other The CUserInterface to copy from
  /////////////////////////////////////////////////
  CUserInterface(const CUserInterface &other)
      : Component(other), m_name(other.m_name),
        m_root_element(other.m_root_element ? other.m_root_element->Clone()
                                            : nullptr),
        m_visible(other.m_visible), m_priority_tier(other.m_priority_tier),
        m_style_name(other.m_style_name) {}

  /////////////////////////////////////////////////
  /// @brief Copy assignment operator for deep copying
  ///
  /// @param other The CUserInterface to copy from
  /// @return Reference to this CUserInterface
  /////////////////////////////////////////////////
  CUserInterface &operator=(const CUserInterface &other) {
    if (this != &other) {
      Component::operator=(other);
      m_name = other.m_name;
      m_root_element =
          other.m_root_element ? other.m_root_element->Clone() : nullptr;
      m_visible = other.m_visible;
      m_priority_tier = other.m_priority_tier;
      m_style_name = other.m_style_name;
    }
    return *this;
  }

  /////////////////////////////////////////////////
  /// @brief String tag for the user interface component
  ///
  /// UI components should be few enough that manual unique naming is viable.
  /////////////////////////////////////////////////
  std::string m_name{"Default UI"};

  /////////////////////////////////////////////////
  /// @brief Root UI element of the user interface component
  /////////////////////////////////////////////////
  std::unique_ptr<UIElement> m_root_element;

  /////////////////////////////////////////////////
  /// @brief Is the this element of the user interface visible to Users.
  /////////////////////////////////////////////////
  bool m_visible{false};

  /////////////////////////////////////////////////
  /// @brief UI tier used for multipass rendering/collision/action ordering.
  ///
  /// Rendering processes tiers from Background -> Modal, while collision/action
  /// process tiers from Modal -> Background.
  /////////////////////////////////////////////////
  UIPriorityTier m_priority_tier{UIPriorityTier::Normal};

  /////////////////////////////////////////////////
  /// @brief Name of the UIStyle to use when rendering this component.
  ///
  /// Must match a key in AssetManager's UI style map. Falls back to "default"
  /// if the named style is not found.
  /////////////////////////////////////////////////
  std::string m_style_name{"default"};
};
} // namespace steamrot
