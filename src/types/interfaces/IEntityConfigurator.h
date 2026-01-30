/////////////////////////////////////////////////
/// @file
/// @brief Declaration of IEntityConfigurator interface
///
/// This interface defines the Strategy pattern for entity configuration.
/// Different implementations can configure entities from different data sources
/// (FlatBuffers, JSON, in-memory test data, etc.).
///
/// **EventHandler Dependency**:
/// The configurator requires EventHandler at construction because entity
/// configuration includes creating and registering event subscribers. This
/// ensures atomic configuration where entities are fully set up in a single
/// operation. Without EventHandler, a separate registration phase would be
/// needed, complicating the API and making errors more likely.
///
/// **Two-Phase Configuration**:
/// Configuration is split into two phases to handle component dependencies:
/// - ConfigureFirstLayerComponents: Components with no dependencies
/// - ConfigureSecondLayerComponents: Components that reference first-layer
///   components (e.g., CUIState references entities by ID)
///
/// See documentation/architecture/ENTITY_CONFIGURATOR_DESIGN_ANALYSIS.md for
/// detailed analysis.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "CGrimoireMachina.h"
#include "CMachinaForm.h"
#include "CUIState.h"
#include "CUserInterface.h"
#include "EntityTransportVariant.h"
#include "EventHandler.h"
#include "containers.h"
namespace steamrot {

class IEntityConfigurator {
protected:
  /////////////////////////////////////////////////
  /// @brief Reference to the EventHandler for creating Subscribers
  ///
  /// EventHandler is injected via constructor (Dependency Injection pattern)
  /// because entity configuration includes creating and registering event
  /// subscribers. This enables atomic configuration where entities and their
  /// event subscriptions are set up together in a single operation.
  ///
  /// Examples of subscriber creation during configuration:
  /// - UI elements register event handlers for clicks, hover, etc.
  /// - CUIState components register state transition subscribers
  /// - Event-driven component behaviors are wired up
  ///
  /// The EventHandler reference is valid for the lifetime of the configurator,
  /// which exists only during scene creation. Subscribers created during
  /// configuration remain registered in EventHandler after the configurator
  /// is destroyed.
  /////////////////////////////////////////////////
  EventHandler &m_event_handler;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for abstract IEntityConfigurator
  ///
  /// @param event_handler Reference to global EventHandler
  ///
  /// EventHandler is required at construction to enable event subscriber
  /// creation during entity configuration. This ensures entities are fully
  /// configured (including event subscriptions) in a single atomic operation.
  /////////////////////////////////////////////////
  IEntityConfigurator(EventHandler &event_handler)
      : m_event_handler(event_handler) {}

  /////////////////////////////////////////////////
  /// @brief Virtual destructor
  /////////////////////////////////////////////////
  virtual ~IEntityConfigurator() = default;

  /////////////////////////////////////////////////
  /// @brief Configure the whole EntityMemoryPool with data
  ///
  /// @param emp EntityMemoryPool to configure
  /// @returns std::expected<std::monostate, FailInfo>
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureEntityMemoryPoolFromSource(
      EntityMemoryPool &emp, const EntityTransportVariant &entity_data) = 0;

  /////////////////////////////////////////////////
  /// @brief Configure Components that do not rely on other components
  ///
  /// @param emp EntityMemoryPool to configure first-layer components in
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureFirstLayerComponents(EntityMemoryPool &emp) = 0;

  /////////////////////////////////////////////////
  /// @brief Configure Components that rely on first-layer components
  ///
  /// @param emp EntityMemoryPool to configure second-layer components in
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSecondLayerComponents(EntityMemoryPool &emp) = 0;

  /////////////////////////////////////////////////
  /// @brief Configure base Component values
  ///
  /// @param component Base component passed through from derived class
  /// @returns std::expected<std::monostate, FailInfo>
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureComponent(Component &component) = 0;

  /////////////////////////////////////////////////
  /// @brief Configure CUserInterface component
  ///
  /// @param c_ui_component Reference to CUserInterface component to configure
  /// @returns std::expected<std::monostate, FailInfo>
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureCUserInterface(CUserInterface &c_ui_component) = 0;

  /////////////////////////////////////////////////
  /// @brief Configure CUIState component
  ///
  /// @param c_ui_state_component Reference to CUIState component to configure
  /// @returns std::expected<std::monostate, FailInfo>
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureCUIState(CUIState &c_ui_state_component, EntityMemoryPool &emp) = 0;

  /////////////////////////////////////////////////
  /// @brief Configure CGrimoireMachina component
  ///
  /// @param c_grimoire_component Reference to CGrimoireMachina component to
  /// configure
  /// @returns std::expected<std::monostate, FailInfo>
  //////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureCGrimoireMachina(CGrimoireMachina &c_grimoire_component) = 0;

  /////////////////////////////////////////////////
  /// @brief Configure CMachinaForm component
  ///
  /// @param c_machina_form_component Reference to CMachinaForm component to
  /// configure
  /// @returns std::expected<std::monostate, FailInfo>
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureCMachinaForm(CMachinaForm &c_machina_form_component) = 0;
};
} // namespace steamrot
// namespace steamrot
