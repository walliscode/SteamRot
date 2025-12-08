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

#include "CGrimoireMachina.h"
#include "CMachinaForm.h"
#include "CMeta.h"
#include "CUIState.h"
#include "CUserInterface.h"
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
  /// @brief Configure the whole EntityMemoryPool with data
  ///
  /// @param emp EntityMemoryPool to configure
  /// @returns std::expected<std::monostate, FailInfo>
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureEntityMemoryPool(EntityMemoryPool &emp) = 0;

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
  /// @brief Configure CMeta component
  ///
  /// @param c_meta_component Reference to CMeta component to configure
  /// @returns std::expected<std::monostate, FailInfo>
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureCMeta(CMeta &c_meta_component) = 0;

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
  ConfigureCMachineForm(CMachinaForm &c_machina_form_component) = 0;
};
} // namespace steamrot
// namespace steamrot
