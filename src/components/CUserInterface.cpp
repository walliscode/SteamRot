////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "CUserInterface.h"
#include "containers.h"
#include "log_handler.h"
#include "spdlog/common.h"
#include "user_interface_generated.h"
namespace steamrot {
////////////////////////////////////////////////////////////
const std::string &CUserInterface::Name() {

  static const std::string name = "CUserInterface";
  return name;
}

////////////////////////////////////////////////////////////
void CUserInterface::Configure(const json &data) {
  // configure variables
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

  // lambda to recursively build UI elements
  std::function<UIElement(const UIElementData *)> build_ui_element =
      [&](const UIElementData *element) -> UIElement {
    UIElement ui_element;
    ui_element.element_type = element->type();
    ui_element.column_layout = element->column_layout();
    if (element->position()) {

      ui_element.position =
          sf::Vector2f(element->position()->x(), element->position()->y());
    };

    if (element->size()) {
      ui_element.size =
          sf::Vector2f(element->size()->x(), element->size()->y());
    };

    // iterate through child elements
    for (const auto *child : *element->children()) {
      ui_element.child_elements.push_back(build_ui_element(child));
    }
    return ui_element;
  };
  // start building from the root element
  root_element = build_ui_element(user_interface_data->root_ui_element());
};

const size_t CUserInterface::GetComponentRegisterIndex() const {

  // Get the index of this component in the component register
  static constexpr size_t index = components::containers::TupleTypeIndex<
      CUserInterface, components::containers::ComponentRegister>;
  return index;
}

} // namespace steamrot
