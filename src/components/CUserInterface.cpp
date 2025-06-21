////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "CUserInterface.h"
#include "containers.h"
#include "event_helpers.h"
#include "log_handler.h"
#include "spdlog/common.h"
#include "user_interface_generated.h"
#include <iostream>
namespace steamrot {
////////////////////////////////////////////////////////////
const std::string &CUserInterface::Name() {

  static const std::string name = "CUserInterface";
  return name;
}

/////////////////////////////////////////////////
void CUserInterface::Configure(const UserInterface *user_interface_data) {

  // guard statement to ensure data is not null
  if (!user_interface_data) {

    log_handler::ProcessLog(spdlog::level::info, log_handler::LogCode::kNoCode,
                            "Configuration data is null for CUserInterface");
    return;
  }
  // set component level data
  // set the component to active, will be picked up by the archetype manager
  m_active = true;

  // set the UI name
  if (user_interface_data->ui_name()) {
    UIName = user_interface_data->ui_name()->str();
  }

  // set the UI visibility
  if (user_interface_data->start_visible()) {
    m_UI_visible = user_interface_data->start_visible();
  } else {
    m_UI_visible = false; // default to false if not specified
  }

  std::cout << "Configuring CUserInterface with data: " << std::endl;

  // lambda to recursively build UI elements
  std::function<UIElement(const UIElementData *)> build_ui_element =
      [&](const UIElementData *element) -> UIElement {
    // create required UIElement using the factory method
    UIElement ui_element = UIElementFactory(*element);

    // iterate through child elements and recursively build them
    for (const auto *child : *element->children()) {
      ui_element.child_elements.push_back(build_ui_element(child));
    }
    return ui_element;
  };

  // start building from the root element
  m_root_element = build_ui_element(user_interface_data->root_ui_element());
  std::cout << "CUserInterface configured successfully." << std::endl;
};

/////////////////////////////////////////////////
UIElement CUserInterface::UIElementFactory(const UIElementData &element) {
  // create UIElement
  UIElement ui_element;

  // general configuration for the UIElement (only variables specificied in the
  // UIElement struct)
  ui_element.layout = element.layout();
  if (element.position()) {
    ui_element.position =
        sf::Vector2f(element.position()->x(), element.position()->y());
  }
  if (element.size()) {
    ui_element.size = sf::Vector2f(element.size()->x(), element.size()->y());
  }

  if (element.action()) {
    ui_element.action = element.action()->action_name();

    std::cout << "Configuring UIElement with action: " << ui_element.action
              << std::endl;
    // convert action keys to EventBitset
    ui_element.trigger_event = ConvertActionKeysToEvent(*element.action());
  }

  std::cout << "Configued general UIElement properties" << std::endl;

  // Configure the UIElement based on its type
  switch (element.type()) {

    // Panel
  case UIElementType::UIElementType_Panel: {
    std::cout << "Configuring Panel UIElement" << std::endl;
    Panel panel_element;

    // configure the panel element

    // add to the UIElement
    ui_element.element_type = panel_element;

    break;
  }
    // Button
  case UIElementType::UIElementType_Button: {
    std::cout << "Configuring Button UIElement" << std::endl;
    Button button_element;

    // configure the button element
    button_element.label = element.element_as_ButtonData()->label()->str();

    // add to the UIElement
    ui_element.element_type = button_element;

    break;
  }
  case UIElementType::UIElementType_None: {
    // add an empty Panel element to the UIElement for the None type
    ui_element.element_type = Panel{};

    break;
  }
  }
  std::cout << "UIElement configured successfully." << std::endl;
  return ui_element;
}

/////////////////////////////////////////////////
const size_t CUserInterface::GetComponentRegisterIndex() const {

  // Get the index of this component in the component register
  static constexpr size_t index = components::containers::TupleTypeIndex<
      CUserInterface, components::containers::ComponentRegister>;
  return index;
}
} // namespace steamrot
