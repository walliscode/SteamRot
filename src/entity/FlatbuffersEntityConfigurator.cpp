/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersConfigurator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersEntityConfigurator.h"
#include "CMachinaForm.h"
#include "FailInfo.h"
#include "FlatbuffersSubscriberViewer.h"
#include "UIElementFactory.h"
#include "containers.h"
#include "entity_memory.h"
#include "ui_state_generated.h"
#include <expected>
#include <variant>

namespace steamrot {
/////////////////////////////////////////////////
FlatbuffersEntityConfigurator::FlatbuffersEntityConfigurator(
    EventHandler &event_handler,
    const EntityCollectionFbs &entity_collection_data)
    : m_entity_collection_data(entity_collection_data),
      IEntityConfigurator(event_handler) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersEntityConfigurator::ConfigureEntityMemoryPool(
    EntityMemoryPool &emp) {

  // resize the entity memory pool based on the flatbuffers data
  entity::memory::ResizeEntityMemoryPool(
      emp, m_entity_collection_data.entity_memory_pool_size());

  // configure first layer components
  auto first_layer_result = ConfigureFirstLayerComponents(emp);
  if (!first_layer_result.has_value())
    return std::unexpected(first_layer_result.error());

  // configure second layer components
  auto second_layer_result = ConfigureSecondLayerComponents(emp);
  if (!second_layer_result.has_value())
    return std::unexpected(second_layer_result.error());

  return std::monostate();
};

/////////////////////////////////////////////////

std::expected<std::monostate, FailInfo>
FlatbuffersEntityConfigurator::ConfigureFirstLayerComponents(
    EntityMemoryPool &emp) {

  ///// JUST FOR FIRST LAYER COMPONENTS /////

  // cycle through each entity in the collection and configure it
  for (const auto &entity_data : *m_entity_collection_data.entities()) {

    //  update the current EntityDataFbs pointer
    m_current_entity_data = entity_data;

    // check the data and configure component if data exists
    if (entity_data->c_user_interface()) {
      auto configure_result = ConfigureCUserInterface(
          entity::memory::GetComponent<CUserInterface>(entity_data->index(),
                                                        emp));
      if (!configure_result.has_value())
        return std::unexpected(configure_result.error());
    }

    if (entity_data->c_grimoire_machina()) {
      auto configure_result = ConfigureCGrimoireMachina(
          entity::memory::GetComponent<CGrimoireMachina>(entity_data->index(),
                                                          emp));
      if (!configure_result.has_value())
        return std::unexpected(configure_result.error());
    }
  }
  return std::monostate();
};

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersEntityConfigurator::ConfigureSecondLayerComponents(
    EntityMemoryPool &emp) {

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersEntityConfigurator::ConfigureComponent(Component &component) {
  // any general configuration logic for the base Component class
  // can be added here if needed
  component.m_active = true;

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersEntityConfigurator::ConfigureCUserInterface(
    CUserInterface &ui_component) {
  // configure the underlying Component type
  auto configure_result =
      ConfigureComponent(static_cast<Component &>(ui_component));

  if (!configure_result.has_value())
    return std::unexpected(configure_result.error());

  // get the UserInterfaceData from the current entity data
  const UserInterfaceData *ui_data = m_current_entity_data->c_user_interface();

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
FlatbuffersEntityConfigurator::ConfigureCMachinaForm(
    CMachinaForm &machina_form_component) {

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersEntityConfigurator::ConfigureCGrimoireMachina(
    CGrimoireMachina &grimoire_component) {

  // configure the underlying Component type
  auto configure_result =
      ConfigureComponent(static_cast<Component &>(grimoire_component));

  if (!configure_result.has_value())
    return std::unexpected(configure_result.error());

  // get the GrimoireMachinaData from the current entity data
  const GrimoireMachinaData *grimoire_data =
      m_current_entity_data->c_grimoire_machina();

  // configure the CGrimoireMachina specific data
  std::vector<std::string> fragment_names;
  if (grimoire_data->fragments()) {
    for (const auto &name : *grimoire_data->fragments()) {
      fragment_names.push_back(name->str());
    }
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersEntityConfigurator::ConfigureCUIState(CUIState &ui_state_component,
                                                 EntityMemoryPool &emp) {

  // configure the underlying Component type
  auto configure_result =
      ConfigureComponent(static_cast<Component &>(ui_state_component));

  if (!configure_result.has_value())
    return std::unexpected(configure_result.error());

  // get the collection of UIStateData from the current entity data
  const UIStateCollectionData *ui_state_collection_data =
      m_current_entity_data->c_ui_state();

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
        entity::memory::GetComponentVector<CUserInterface>(emp);

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

      // create a FlatbuffersSubscriberViewer instance
      FlatbuffersSubscriberViewer subscriber_viewer(
          ui_state_data->subscribers());

      // get the subscribers from the viewer
      auto subscribers_result = subscriber_viewer.GetSubscribers();
      if (!subscribers_result.has_value()) {
        return std::unexpected(subscribers_result.error());
      }
      // assign the subscribers to the ui state component for this state key
      ui_state_component.m_state_subscribers[state_key] =
          subscribers_result.value();

      // register each subscriber with the event handler
      for (const auto &subscriber : subscribers_result.value()) {
        auto reg_result = m_event_handler.RegisterSubscriber(subscriber);
        if (!reg_result.has_value()) {
          return std::unexpected(reg_result.error());
        }
      }
    }
  }
  return std::monostate{};
}

} // namespace steamrot
