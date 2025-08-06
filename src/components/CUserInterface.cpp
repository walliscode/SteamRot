////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "CUserInterface.h"
#include "UIElementFactory.h"
#include "containers.h"
#include "log_handler.h"
#include "spdlog/common.h"
#include "user_interface_generated.h"

namespace steamrot {

/////////////////////////////////////////////////
void CUserInterface::Configure(const UserInterfaceData *user_interface_data) {

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
  // create instance of UIElementFactory for use.
  UIElementFactory element_factory;

  // start building from the root element
  m_root_element = element_factory.CreateUIStructure(
      *user_interface_data->root_ui_element());
};

/////////////////////////////////////////////////
size_t CUserInterface::GetComponentRegisterIndex() const {

  // Get the index of this component in the component register
  static constexpr size_t index =
      TupleTypeIndex<CUserInterface, ComponentRegister>;
  return index;
}
} // namespace steamrot
