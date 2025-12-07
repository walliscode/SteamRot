/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersConfigurator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersConfigurator.h"
#include "CUIState.h"
#include "CUserInterface.h"
#include "EntityConfigurator.h"
#include "subscriber_factory.h"
#include "UIElementFactory.h"
#include "entity_memory.h"

#include "user_interface_generated.h"
#include <expected>
#include <variant>
#include <vector>

namespace steamrot {
/////////////////////////////////////////////////
FlatbuffersConfigurator::FlatbuffersConfigurator(EventHandler &event_handler)
    : EntityConfigurator(event_handler), m_data_loader() {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersConfigurator::ConfigureEntitiesFromDefaultData(
    EntityMemoryPool &entity_memory_pool, const SceneType scene_type) {

  // get the entity collection from the flatbuffers data loader
  const SceneDataData *scene_data =
      m_data_loader.ProvideDefaultSceneData(scene_type).value();

  // delegate to ConfigureEntitiesFromCollection
  return ConfigureEntitiesFromCollection(entity_memory_pool,
                                         scene_data->entity_collection());
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
    ui_component.m_name = ui_data->ui_name()->str();

  if (ui_data->is_visible())
    ui_component.m_visible = ui_data->is_visible();

  // data must contain a root element so throw unexpected if it is not set
  if (!ui_data->root_ui_element()) {
    FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                       "Root ui element not found in UserInterfaceData."};
    return std::unexpected(fail_info);
  }

  // create the root UI element using the factory function, this will
  // recursively create the nested structure
  auto root_element_result =
      CreateUIElement(UIElementDataUnion::UIElementDataUnion_PanelData,
                      ui_data->root_ui_element(), m_event_handler);
  if (!root_element_result.has_value())
    return std::unexpected(root_element_result.error());

  ui_component.m_root_element = std::move(root_element_result.value());
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersConfigurator::ConfigureComponent(
    const GrimoireMachinaData *grimoire_data,
    CGrimoireMachina &grimoire_component) {

  // configure the underlying Component type
  auto configure_result =
      ConfigureComponent(static_cast<Component &>(grimoire_component));

  if (!configure_result.has_value())
    return std::unexpected(configure_result.error());

  // configure the CGrimoireMachina specific data
  std::vector<std::string> fragment_names;
  if (grimoire_data->fragments()) {
    for (const auto &name : *grimoire_data->fragments()) {
      fragment_names.push_back(name->str());
    }
  }
  // attempt to load the fragments
  auto fragment_load_result = m_data_loader.ProvideAllFragments(fragment_names);

  if (!fragment_load_result.has_value()) {
    FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                       "Failed to load fragments for CGrimoireMachina."};
    return std::unexpected(fail_info);
  }
  // assign the loaded fragments to the CGrimoireMachina component
  grimoire_component.m_all_fragments = fragment_load_result.value();

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersConfigurator::ConfigureComponent(
    const UIStateCollectionData *ui_state_collection_data,
    CUIState &ui_state_component, const EntityMemoryPool &entity_memory_pool) {

  // configure the underlying Component type
  auto configure_result =
      ConfigureComponent(static_cast<Component &>(ui_state_component));

  if (!configure_result.has_value())
    return std::unexpected(configure_result.error());

  // cycle through each UIStateData in the collection
  for (auto ui_state_data : *ui_state_collection_data->ui_states()) {

    // pull out state key
    std::string state_key = ui_state_data->state_key()->str();

    //
    // check that data for mapping ui visi
    if (!ui_state_data->state_to_ui_visibility()) {
      FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                         "No mappings found in UIStateData for ui visibility."};
      return std::unexpected(fail_info);
    }

    // get all UI components from the entity memory pool
    const auto &ui_components =
        entity::memory::GetComponentVector<CUserInterface>(entity_memory_pool);

    UIVisibilityState visibility_state;

    // check that there are ui names to map
    if (ui_state_data->state_to_ui_visibility()->ui_names_on()) {

      std::vector<std::string> ui_names_vec;
      // create a vector of ui names for easier interaction
      for (const auto &ui_name :
           *ui_state_data->state_to_ui_visibility()->ui_names_on()) {

        ui_names_vec.push_back(ui_name->str());
      }

      for (size_t i = 0; i < ui_components.size(); ++i) {
        const auto &ui_component = ui_components[i];

        // check if name exists in the ui names vector
        if (std::find(ui_names_vec.begin(), ui_names_vec.end(),
                      ui_component.m_name) != ui_names_vec.end()) {

          // if it does, add the index to the on list
          visibility_state.m_ui_indices_on.push_back(i);
        }
      }
    }

    // repeat for ui names off
    if (ui_state_data->state_to_ui_visibility()->ui_names_off()) {
      std::vector<std::string> ui_names_vec;
      // create a vector of ui names for easier interaction
      for (const auto &ui_name :
           *ui_state_data->state_to_ui_visibility()->ui_names_off()) {
        ui_names_vec.push_back(ui_name->str());
      }
      for (size_t i = 0; i < ui_components.size(); ++i) {
        const auto &ui_component = ui_components[i];
        // check if name exists in the ui names vector
        if (std::find(ui_names_vec.begin(), ui_names_vec.end(),
                      ui_component.m_name) != ui_names_vec.end()) {
          // if it does, add the index to the off list
          visibility_state.m_ui_indices_off.push_back(i);
        }
      }
    }

    // Store the visibility state for this state key
    ui_state_component.m_state_to_ui_visibility[state_key] = visibility_state;

    // Create and register subscribers if provided
    if (ui_state_data->subscribers()) {

      // Collect all subscriber configs into a vector
      std::vector<const SubscriberConfigFbs *> configs;
      for (const auto *subscriber_data : *ui_state_data->subscribers()) {
        if (subscriber_data) {
          configs.push_back(subscriber_data);
        }
      }

      // Create and register all subscribers at once
      auto result = subscriber_factory::CreateAndRegisterSubscribers(
          configs, ui_state_component.m_state_subscribers[state_key],
          m_event_handler);

      if (!result.has_value()) {
        return std::unexpected(result.error());
      }
    }
  }
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersConfigurator::ConfigureEntitiesFromCollection(
    EntityMemoryPool &entity_memory_pool,
    const EntityCollection *entity_collection) {

  // check the list of entities is not empty
  if (!entity_collection) {
    FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                       "Entity data not found in the collection."};
    return std::unexpected(fail_info);
  }

  // check that the entity memory pool size has been added
  if (!entity_collection->entity_memory_pool_size()) {
    FailInfo fail_info{
        FailMode::FlatbuffersDataNotFound,
        "No entity memory pool size found in the entity collection."};
    return std::unexpected(fail_info);
  }

  // resize the entity memory pool to the size specified in the flatbuffers
  size_t pool_size = entity_collection->entity_memory_pool_size();

  std::apply(
      [pool_size](auto &...component_vector) {
        (component_vector.resize(pool_size), ...);
      },
      entity_memory_pool);

  // some helper values
  size_t entity_count = entity_collection->entities()->size();
  // check the entity memory pool is big enough
  if (entity::memory::GetMemoryPoolSize(entity_memory_pool) < entity_count) {
    std::string fail_msg = std::format(
        "Entity memory pool size: {}, required size: {}",
        entity::memory::GetMemoryPoolSize(entity_memory_pool), entity_count);

    FailInfo fail_info{FailMode::ParameterOutOfBounds, fail_msg};
    return std::unexpected(fail_info);
  }

  // configure entities from the flatbuffers data
  for (size_t i = 0; i < entity_count; ++i) {
    const EntityData *entity_data = entity_collection->entities()->Get(i);

    if (entity_data == nullptr) {
      continue; // Skip null entities
    }

    // CUserInterface component configuration
    if (entity_data->c_user_interface()) {
      auto configure_result = ConfigureComponent(
          entity_data->c_user_interface(),
          entity::memory::GetComponent<CUserInterface>(i, entity_memory_pool));

      if (!configure_result.has_value())
        return std::unexpected(configure_result.error());
    }

    // CGrimoireMachina component configuration
    if (entity_data->c_grimoire_machina()) {
      auto configure_result =
          ConfigureComponent(entity_data->c_grimoire_machina(),
                             entity::memory::GetComponent<CGrimoireMachina>(
                                 i, entity_memory_pool));

      if (!configure_result.has_value())
        return std::unexpected(configure_result.error());
    }
  }

  // Configure compound components after simpler components
  // CUIState needs to reference CUserInterface components by name
  for (size_t i = 0; i < entity_count; ++i) {
    const EntityData *entity_data = entity_collection->entities()->Get(i);

    if (entity_data == nullptr) {
      continue; // Skip null entities
    }

    // CUIState component configuration (compound component)
    if (entity_data->c_ui_state()) {

      auto configure_result = ConfigureComponent(
          entity_data->c_ui_state(),
          entity::memory::GetComponent<CUIState>(i, entity_memory_pool),
          entity_memory_pool);

      if (!configure_result.has_value())
        return std::unexpected(configure_result.error());
    }
  }

  return std::monostate{};
}

} // namespace steamrot
