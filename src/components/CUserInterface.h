
////////////////////////////////////////////////////////////
// preprocessor directives
////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "Component.h"
#include "user_interface_generated.h"

#include <SFML/System/Vector2.hpp>
#include <variant>
#include <vector>

namespace steamrot {

struct Panel {};

struct Button {

  // Additional properties specific to Button can be added here
  std::string label{"Fill me in!"};
};

using ElementType = std::variant<Panel, Button>;

struct UIElement {

  /////////////////////////////////////////////////
  /// @brief UIELement type, contains extra data for the element, can only be
  /// one of the options.
  /////////////////////////////////////////////////
  ElementType element_type;

  /////////////////////////////////////////////////
  /// @brief Container for all child elements. Can be empty
  /////////////////////////////////////////////////
  std::vector<UIElement> child_elements;

  /////////////////////////////////////////////////
  /// @brief Position of the UI element in the window
  /////////////////////////////////////////////////
  sf::Vector2f position{0.f, 0.f};

  /////////////////////////////////////////////////
  /// @brief Size of the UI element.
  /////////////////////////////////////////////////
  sf::Vector2f size{0.f, 0.f};

  /////////////////////////////////////////////////
  /// @brief Layout type of the children elements
  /////////////////////////////////////////////////
  LayoutType layout{0};

  /////////////////////////////////////////////////
  /// @brief Is mouse over this element, false if it over a child element
  /////////////////////////////////////////////////
  bool mouse_over{false};

  /////////////////////////////////////////////////
  /// @brief Helper variable to check if the mouse is over a child element
  /////////////////////////////////////////////////
  bool mouse_over_child{false};
};

struct CUserInterface : public Component {

  ////////////////////////////////////////////////////////////
  // |brief: Default constructor
  ////////////////////////////////////////////////////////////
  CUserInterface() = default;

  /////////////////////////////////////////////////
  /// @brief String tag for the user interface component
  ///
  /// UI components should be few enough that manual unique naming is viable.
  /////////////////////////////////////////////////
  std::string UIName{"Default UI"};

  /////////////////////////////////////////////////
  /// @brief Root UI element of the user interface component
  /////////////////////////////////////////////////
  UIElement m_root_element;

  /////////////////////////////////////////////////
  /// @brief Is the this element of the user interface visible to Users.
  /////////////////////////////////////////////////
  bool m_UI_visible{false};

  /**
   * @brief String representation of the component name, this will return a
   * static value so it is constant across all instances
   */
  const std::string &Name() override;

  /////////////////////////////////////////////////
  /// @brief Confiure the component with flatbuffer data
  ///
  /// @param user_interface_data Flatbuffer data for the user interface
  /////////////////////////////////////////////////
  void Configure(const UserInterface *user_interface_data);

  /////////////////////////////////////////////////
  /// @brief  Configures a UI element from flatbuffer data
  ///
  /// @param element Flatbuffer data for the UI element
  /// @return UIElement with one of the element types
  /////////////////////////////////////////////////
  UIElement UIElementFactory(const UIElementData &element);

  /**
   * @brief Returns the index of the component in the component register
   * @return size_t index of the component in the component register
   */
  const size_t GetComponentRegisterIndex() const override;
};
} // namespace steamrot
