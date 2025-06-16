
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
   * @brief Position of the UI element, if not set then the UIRenderLogic will
   * use margins and padding to draw
   */
  sf::Vector2f position{0.f, 0.f};

  /**
   * @brief Size of the UI element
   */
  sf::Vector2f size{0.f, 0.f};

  /**
   * @brief All child element indices for this UI element
   */
  std::vector<UIElement> child_elements;

  /**
   * @brief Are child elements in a column or row layout? if true then vertical
   * alignment
   */
  LayoutType layout{0};
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

  /**
   * @brief Returns the index of the component in the component register
   * @return size_t index of the component in the component register
   */
  const size_t GetComponentRegisterIndex() const override;
};
} // namespace steamrot
