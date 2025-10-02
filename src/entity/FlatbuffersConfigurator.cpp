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
#include "SubscriberFactory.h"
#include "UIElementFactory.h"
#include "emp_helpers.h"

#include "user_interface_generated.h"
#include <expected>
#include <iostream>
#include <variant>

namespace steamrot {
/////////////////////////////////////////////////
FlatbuffersConfigurator::FlatbuffersConfigurator(EventHandler &event_handler)
    : EntityConfigurator(event_handler) {}

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

  // check that the entity memory pool size has been added
  if (!scene_data->entity_collection()->entity_memory_pool_size()) {
    FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                       "No entity memory pool size found in the scene data."};
    return std::unexpected(fail_info);
  }

  // resize the entity memory pool to the size specified in the flatbuffers
  size_t pool_size = scene_data->entity_collection()->entity_memory_pool_size();

  std::apply(
      [pool_size](auto &...component_vector) {
        (component_vector.resize(pool_size), ...);
      },
      entity_memory_pool);

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
    std::cout << "Configuring entity index: " << i << std::endl;
    // CUserInterface component configuration
    if (entity_data->c_user_interface()) {
      auto configure_result = ConfigureComponent(
          entity_data->c_user_interface(),
          emp_helpers::GetComponent<CUserInterface>(i, entity_memory_pool));

      if (!configure_result.has_value())
        return std::unexpected(configure_result.error());
    }

    // CGrimoireMachina component configuration
    if (entity_data->c_grimoire_machina()) {

      auto configure_result = ConfigureComponent(
          entity_data->c_grimoire_machina(),
          emp_helpers::GetComponent<CGrimoireMachina>(i, entity_memory_pool));

      if (!configure_result.has_value())
        return std::unexpected(configure_result.error());
    }
  }

  // Configure compound components after simpler components
  // CUIState needs to reference CUserInterface components by name
  for (size_t i = 0; i < entity_count; ++i) {

    const EntityData *entity_data =
        scene_data->entity_collection()->entities()->Get(i);

    if (entity_data == nullptr) {
      continue; // Skip null entities
    }

    // CUIState component configuration (compound component)
    if (entity_data->c_ui_state()) {
      auto configure_result = ConfigureComponent(
          entity_data->c_ui_state(),
          emp_helpers::GetComponent<CUIState>(i, entity_memory_pool),
          entity_memory_pool);

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
    ui_component.m_name = ui_data->ui_name()->str();

  if (ui_data->start_visible())
    ui_component.m_UI_visible = ui_data->start_visible();

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
    const UIStateData *ui_state_data, CUIState &ui_state_component,
    const EntityMemoryPool &entity_memory_pool) {

  // configure the underlying Component type
  auto configure_result =
      ConfigureComponent(static_cast<Component &>(ui_state_component));

  if (!configure_result.has_value())
    return std::unexpected(configure_result.error());

  // check that mappings exist
  if (!ui_state_data->mappings()) {
    FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                       "No mappings found in UIStateData."};
    return std::unexpected(fail_info);
  }

  // Build a map of UI names to entity indices for quick lookup
  std::unordered_map<std::string, size_t> ui_name_to_index;
  const auto &ui_components =
      std::get<std::vector<CUserInterface>>(entity_memory_pool);

  for (size_t i = 0; i < ui_components.size(); ++i) {
    if (ui_components[i].m_active) {
      ui_name_to_index[ui_components[i].m_name] = i;
    }
  }

  // Create SubscriberFactory for registering subscribers
  SubscriberFactory subscriber_factory(m_event_handler);

  // Process each mapping
  for (const auto *mapping : *ui_state_data->mappings()) {
    if (!mapping) {
      continue;
    }

    // Check state_key exists
    if (!mapping->state_key()) {
      FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                         "State key not found in UIStateMapping."};
      return std::unexpected(fail_info);
    }

    std::string state_key = mapping->state_key()->str();
    UIVisibilityState visibility_state;

    // Process ui_names_on (UIs that should be visible)
    if (mapping->ui_names_on()) {
      for (const auto *ui_name_fb : *mapping->ui_names_on()) {
        if (!ui_name_fb) {
          continue;
        }

        std::string ui_name = ui_name_fb->str();

        // Look up the index for this UI name
        auto it = ui_name_to_index.find(ui_name);
        if (it == ui_name_to_index.end()) {
          std::string error_msg =
              "UI component with name '" + ui_name + "' not found in ui_names_on.";
          FailInfo fail_info{FailMode::FlatbuffersDataNotFound, error_msg};
          return std::unexpected(fail_info);
        }

        visibility_state.m_ui_indices_on.push_back(it->second);
      }
    }

    // Process ui_names_off (UIs that should be hidden)
    if (mapping->ui_names_off()) {
      for (const auto *ui_name_fb : *mapping->ui_names_off()) {
        if (!ui_name_fb) {
          continue;
        }

        std::string ui_name = ui_name_fb->str();

        // Look up the index for this UI name
        auto it = ui_name_to_index.find(ui_name);
        if (it == ui_name_to_index.end()) {
          std::string error_msg =
              "UI component with name '" + ui_name + "' not found in ui_names_off.";
          FailInfo fail_info{FailMode::FlatbuffersDataNotFound, error_msg};
          return std::unexpected(fail_info);
        }

        visibility_state.m_ui_indices_off.push_back(it->second);
      }
    }

    // Store the visibility state for this state key
    ui_state_component.m_state_to_ui_visibility[state_key] = visibility_state;

    // Initialize state value to false
    ui_state_component.m_state_values[state_key] = false;

    // Create and register subscribers if provided
    if (mapping->subscribers()) {
      std::vector<std::shared_ptr<Subscriber>> subscribers;
      
      for (const auto *subscriber_data : *mapping->subscribers()) {
        if (!subscriber_data) {
          continue;
        }

        auto subscriber_result = subscriber_factory.CreateAndRegisterSubscriber(
            *subscriber_data);

        if (!subscriber_result.has_value()) {
          return std::unexpected(subscriber_result.error());
        }

        subscribers.push_back(subscriber_result.value());
      }

      // Store all subscribers for this state key
      ui_state_component.m_state_subscribers[state_key] = std::move(subscribers);
    }
  }

  return std::monostate{};
}
} // namespace steamrot
