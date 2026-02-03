/////////////////////////////////////////////////
/// @file
/// @brief Delaration of IEntityConfigurator interface
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
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
  /////////////////////////////////////////////////
  EventHandler &m_event_handler;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for abstract IEntityConfigurator
  ///
  /// @param event_handler Refernce to global EventHandler
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
};
} // namespace steamrot
// namespace steamrot
