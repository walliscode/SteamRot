
////////////////////////////////////////////////////////////
// preprocessor directives
////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "Component.h"
#include "user_interface_generated.h"

#include <vector>

namespace steamrot {

/**
 * @class UIElement
 * @brief Base struct for all UI elements
 *
 */
struct UIElement {
  /**
   * @brief UIElementType type for the UI element, allows for the UI engine to
   * select correct function
   */
  UIElementType element_type{};

  /**
   * @brief All child element indices for this UI element
   */
  std::vector<UIElement> child_elements;

  /**
   * @brief Are child elements in a column or row layout? if true then vertical
   * alignment
   */
  bool column_layout{true};
};

struct CUserInterface : public Component {

  ////////////////////////////////////////////////////////////
  // |brief: Default constructor
  ////////////////////////////////////////////////////////////
  CUserInterface() = default;

  /**
   * @brief Start of the UI element tree, every interface will have to have some
   * kind of base container
   */
  UIElement root_element;

  /**
   * @brief String representation of the component name, this will return a
   * static value so it is constant across all instances
   */
  const std::string &Name() override;
  void Configure(const nlohmann::json &data);
  void Configure(const UserInterface *user_interface_data);
};
} // namespace steamrot
