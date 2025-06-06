////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "CUserInterface.h"
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

  // lambda to recursively build UI elements
  std::function<UIElement(const UIElementData *)> build_ui_element =
      [&](const UIElementData *element) -> UIElement {
    UIElement ui_element;
    ui_element.element_type = element->type();
    ui_element.column_layout = element->column_layout();

    // iterate through child elements
    for (const auto *child : *element->children()) {
      ui_element.child_elements.push_back(build_ui_element(child));
    }
    return ui_element;
  };
  // start building from the root element
  root_element = build_ui_element(user_interface_data->root_ui_element());
};

} // namespace steamrot
