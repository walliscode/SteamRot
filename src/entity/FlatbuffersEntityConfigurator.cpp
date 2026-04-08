/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersConfigurator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersEntityConfigurator.h"
#include "EntityTransportVariant.h"
#include "FailInfo.h"
#include "FlatbuffersUIElementProvider.h"
#include "configure_subscriber.h"
#include "containers.h"
#include "entity_memory.h"
#include "ui_state_generated.h"
#include <expected>
#include <variant>

namespace steamrot {
/////////////////////////////////////////////////
FlatbuffersEntityConfigurator::FlatbuffersEntityConfigurator(
    EventHandler &event_handler)
    : IEntityConfigurator(event_handler) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersEntityConfigurator::ConfigureEntityMemoryPoolFromSource(
    EntityMemoryPool &emp, const EntityTransportVariant &entity_data) {

  // assign the flatbuffers data pointer based on the variant type
  if (std::holds_alternative<const EntityCollectionFbs *>(entity_data)) {
    m_entity_collection_data =
        std::get<const EntityCollectionFbs *>(entity_data);

    // check for null pointer
    if (!m_entity_collection_data) {
      return std::unexpected(FailInfo{
          FailMode::FlatbuffersDataNotFound,
          "EntityCollectionFbs pointer in EntityTransportVariant is null."});
    }
  } else {
    return std::unexpected(FailInfo{
        FailMode::VariantTypeMismatch,
        "EntityTransportVariant does not hold EntityCollectionFbs data."});
  }

  // che  // check that entity memory pool size exists in the flatbuffers data
  if (!m_entity_collection_data->entity_memory_pool_size())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "Entity memory pool size not found in EntityCollectionFbs."});
  // resize the entity memory pool based on the flatbuffers data
  entity::memory::ResizeEntityMemoryPool(
      emp, m_entity_collection_data->entity_memory_pool_size());

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

  // start off entity position at zero
  size_t entity_index = 0;
  // cycle through each entity in the collection and configure it
  for (const auto &entity_data : *m_entity_collection_data->entities()) {

    //  update the current EntityDataFbs pointer
    m_current_entity_data = entity_data;

    // check the data and configure component if data exists
    if (entity_data->c_user_interface()) {

      auto configure_result = ConfigureCUserInterface(
          entity::memory::GetComponent<CUserInterface>(entity_index, emp));
      if (!configure_result.has_value())
        return std::unexpected(configure_result.error());
    }

    // progress to next entity index
    entity_index++;
  }
  return std::monostate();
};

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersEntityConfigurator::ConfigureSecondLayerComponents(
    EntityMemoryPool &emp) {

  // reset entity index to zero
  size_t entity_index = 0;

  for (const auto &entity_data : *m_entity_collection_data->entities()) {
    //  update the current EntityDataFbs pointer
    m_current_entity_data = entity_data;

    /// CONFIGURE CUIState ///
    if (entity_data->c_ui_state()) {
      auto configure_result = ConfigureCUIState(
          entity::memory::GetComponent<CUIState>(entity_index, emp), emp);
      if (!configure_result.has_value())
        return std::unexpected(configure_result.error());
    }
    // progress to next entity index
    entity_index++;
  }

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
  auto configure_result = ConfigureComponent(ui_component);
  if (!configure_result.has_value())
    return std::unexpected(configure_result.error());

  // get the UserInterfaceData from the current entity data
  const UserInterfaceFbs *ui_data = m_current_entity_data->c_user_interface();

  // null check for ui_data
  if (!ui_data) {
    FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                       "UserInterfaceData not found in entity data."};
    return std::unexpected(fail_info);
  }

  // configure the CUserInterface specific data, wrap in if statements to avoid
  // any segfaults
  if (ui_data->ui_name())
    ui_component.m_name = ui_data->ui_name()->str();

  // Always read the boolean value, not just when it's true
  ui_component.m_visible = ui_data->is_visible();

  // Read priority (z-order) for rendering and collision ordering
  ui_component.m_priority = ui_data->priority();

  // data must contain a root element so throw unexpected if it is not set
  if (!ui_data->root_ui_element()) {
    FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                       "Root ui element not found in UserInterfaceData."};
    return std::unexpected(fail_info);
  }

  // create the root UI element using the provider, this will
  // recursively create the nested structure
  FlatbuffersUIElementProvider ui_provider(m_event_handler, *ui_data);
  auto root_element_result = ui_provider.CreateRootUIElement();
  if (!root_element_result.has_value())
    return std::unexpected(root_element_result.error());

  ui_component.m_root_element = std::move(root_element_result.value());

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

  // get the collection of UIStateDataFbs from the current entity data
  const UIStateCollectionDataFbs *ui_state_collection_data =
      m_current_entity_data->c_ui_state();

  // cycle through each UIStateDataFbs in the collection
  for (auto ui_state_data : *ui_state_collection_data->ui_states()) {

    // pull out state key
    std::string state_key = ui_state_data->state_key()->str();

    //
    // check that data for mapping ui visi
    if (!ui_state_data->state_to_ui_visibility()) {
      FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                         "No mappings found in UIStateDataFbs for ui visibility."};
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

      std::vector<std::shared_ptr<Subscriber>> subscribers_vector;
      // create subscribers from the flatbuffers data
      for (const auto &subscriber_data : *ui_state_data->subscribers()) {

        std::shared_ptr<Subscriber> subscriber = std::make_shared<Subscriber>();
        auto configure_result =
            data::configure::ConfigureSubscriber(*subscriber, subscriber_data);
        if (!configure_result.has_value()) {
          return std::unexpected(configure_result.error());
        }
        subscribers_vector.push_back(subscriber);
      }

      // assign the subscribers to the ui state component for this state key
      ui_state_component.m_state_subscribers[state_key] = subscribers_vector;
      // register each subscriber with the event handler
      for (const auto &subscriber : subscribers_vector) {
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
