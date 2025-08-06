/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersConfigurator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersConfigurator.h"
#include "CUserInterface.h"
#include "UIElementFactory.h"
#include "emp_helpers.h"
#include "scene_types_generated.h"
#include <expected>
#include <variant>

namespace steamrot {
/////////////////////////////////////////////////
FlatbuffersConfigurator::FlatbuffersConfigurator(const EnvironmentType env_type)
    : m_data_loader(env_type) {}

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersConfigurator::ConfigureEntitiesFromDefaultData(
    EntityMemoryPool &entity_memory_pool, const SceneType scene_type) {

  // get the entity collection from the flatbuffers data loader
  const SceneData *scene_data =
      m_data_loader.ProvideSceneData(scene_type).value();

  // check the list of entities is not empty
  if (scene_data->entity_collection()->entities()->empty()) {

    FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                       "Entity data not found in the collection."};

    return std::unexpected(fail_info);
  }

  // some helper values
  size_t entity_count = scene_data->entity_collection()->entities()->size();
  // check the entity memory pool is big enough
  if (emp_helpers::GetMemoryPoolSize(entity_memory_pool) < entity_count) {

    std::string fail_msg = std::format(
        "Entity memory pool size: {}, required size: {}",
        emp_helpers::GetMemoryPoolSize(entity_memory_pool), entity_count);

    FailInfo fail_info{FailMode::ParameterOutOfBounds, fail_msg};
    return std::unexpected(fail_info);
  }

  // configure entities from the flatbuffers data
  for (size_t i = 0; i < entity_count; ++i) {

    const EntityData *entity_data =
        scene_data->entity_collection()->entities()->Get(i);

    if (entity_data == nullptr) {
      continue; // Skip null entities
    }

    if (entity_data->c_user_interface()) {
      auto configure_result = ConfigureComponent(
          entity_data->c_user_interface(),
          emp_helpers::GetComponent<CUserInterface>(i, entity_memory_pool));

      if (!configure_result.has_value())
        return std::unexpected(configure_result.error());
    }
  }

  return std::monostate{};
};

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersConfigurator::ConfigureComponent(Component &component) {
  // any general configuration logic for the base Component class
  // can be added here if needed
  component.m_active = true;

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersConfigurator::ConfigureComponent(const UserInterfaceData *ui_data,
                                            CUserInterface &ui_component) {
  // configure the underlying Component type
  auto configure_result =
      ConfigureComponent(static_cast<Component &>(ui_component));

  if (!configure_result.has_value())
    return std::unexpected(configure_result.error());

  // configure the CUserInterface specific data, wrap in if statements to avoid
  // any segfaults
  if (ui_data->ui_name())
    ui_component.UIName = ui_data->ui_name()->str();

  if (ui_data->start_visible())
    ui_component.m_UI_visible = ui_data->start_visible();

  // data must contain a root element so throw unexpected if it is not set
  if (!ui_data->root_ui_element()) {
    FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                       "Root ui element not found in UserInterfaceData."};
    return std::unexpected(fail_info);
  }

  // create instance of UIElementFactory for use.
  UIElementFactory element_factory;

  // start building from the root element
  // ui_component.m_root_element =
  // element_factory.CreateUIStructure(*ui_data->root_ui_element());

  return std::monostate{};
}
} // namespace steamrot
