
/////////////////////////////////////////////////
/// Preprocessor directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Component.h"
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
        m_visible(other.m_visible) {}

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
  /// @brief returns the index of the component in the component register
  ///
  /// @return Index of the component in the component register
  /////////////////////////////////////////////////
  size_t GetComponentRegisterIndex() const override;
};
} // namespace steamrot
