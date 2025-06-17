////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "CUserInterface.h"
#include "containers.h"
#include "log_handler.h"
#include "spdlog/common.h"
#include "user_interface_generated.h"
#include <iostream>
#include <memory>
namespace steamrot {
////////////////////////////////////////////////////////////
const std::string &CUserInterface::Name() {

  static const std::string name = "CUserInterface";
  return name;
}

void CUserInterface::Configure(const UserInterface *user_interface_data) {

  // guard statement to ensure data is not null
  if (!user_interface_data) {

    log_handler::ProcessLog(spdlog::level::info, log_handler::LogCode::kNoCode,
                            "Configuration data is null for CUserInterface");
    return;
  }
  // set the component to active
  m_active = true;

  std::cout << "Configuring CUserInterface with data" << std::endl;
  // lambda to recursively build UI elements
  std::function<std::unique_ptr<UIElement>(const UIElementData *)>
      build_ui_element =
          [&](const UIElementData *element) -> std::unique_ptr<UIElement> {
    // create required UIElement using the factory method
    std::unique_ptr<UIElement> ui_element = UIElementFactory(*element);

    // iterate through child elements and recursively build them
    for (const auto *child : *element->children()) {
      ui_element->child_elements.push_back(build_ui_element(child));
    }
    return ui_element;
  };
  // start building from the root element
  m_root_element = build_ui_element(user_interface_data->root_ui_element());
};

std::unique_ptr<UIElement>
CUserInterface::UIElementFactory(const UIElementData &element) {
  // Create a UIElement based on the type
  switch (element.type()) {
    // Panel
  case UIElementType::UIElementType_Panel: {
    std::unique_ptr<Panel> panel_element = std::make_unique<Panel>();
    panel_element->element_type = element.type();
    panel_element->layout = element.layout();
    if (element.position()) {
      panel_element->position =
          sf::Vector2f(element.position()->x(), element.position()->y());
    }
    if (element.size()) {
      panel_element->size =
          sf::Vector2f(element.size()->x(), element.size()->y());
    }

    return panel_element;
  }
    // Button
  case UIElementType::UIElementType_Button: {
    std::unique_ptr<Button> button_element = std::make_unique<Button>();
    button_element->element_type = element.type();
    button_element->layout = element.layout();
    if (element.position()) {
      button_element->position =
          sf::Vector2f(element.position()->x(), element.position()->y());
    }
    if (element.size()) {
      button_element->size =
          sf::Vector2f(element.size()->x(), element.size()->y());
    }
    if (element.label()) {
      button_element->label = element.label()->str();
    } else {
      button_element->label = "Button"; // Default label if none provided
    }
    return button_element;
  }

    // Default case for unknown types
  default: {
    std::cout << "Unknown UIElement type, returning nullptr" << std::endl;
    return nullptr;
  }
  }
}
/////////////////////////////////////////////////
const size_t CUserInterface::GetComponentRegisterIndex() const {

  // Get the index of this component in the component register
  static constexpr size_t index = components::containers::TupleTypeIndex<
      CUserInterface, components::containers::ComponentRegister>;
  return index;
}
} // namespace steamrot
