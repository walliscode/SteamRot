/////////////////////////////////////////////////
/// @file
/// @brief Declaration of FlatbuffersConfigurator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "IEntityConfigurator.h"
#include "containers.h"
#include "entities_generated.h"

namespace steamrot {
class FlatbuffersEntityConfigurator : public IEntityConfigurator {
private:
  /////////////////////////////////////////////////
  /// @brief Reference to the EntityCollection flatbuffers data
  /////////////////////////////////////////////////
  const EntityCollectionFbs &m_entity_collection_data;

  /////////////////////////////////////////////////
  /// @brief Pointer to const current EntityData being configured
  ///
  /// The pointer is updated during the configuration of each entity
  /////////////////////////////////////////////////
  const EntityDataFbs *m_current_entity_data;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for FlatbuffersEntityConfigurator
  ///
  /// @param event_handler Reference to the EventHandler
  /// @param entity_collection_data Reference to the EntityCollection to
  /// configure from
  /////////////////////////////////////////////////
  FlatbuffersEntityConfigurator(
      EventHandler &event_handler,
      const EntityCollectionFbs &entity_collection_data);

  /////////////////////////////////////////////////
  /// @brief Configure the whole EntityMemoryPool with data
  ///
  /// @param emp EntityMemoryPool to configure
  /// @returns std::expected<std::monostate, FailInfo>
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureEntityMemoryPool(EntityMemoryPool &emp) override;

  /////////////////////////////////////////////////
  /// @brief Configure Components that do not rely on other components
  ///
  /// @param emp EntityMemoryPool to configure first-layer components in
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureFirstLayerComponents(EntityMemoryPool &emp) override;

  /////////////////////////////////////////////////
  /// @brief Configure Components that rely on first-layer components
  ///
  /// @param emp EntityMemoryPool to configure second-layer components in
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSecondLayerComponents(EntityMemoryPool &emp) override;

  /////////////////////////////////////////////////
  /// @brief Configure base Component values
  ///
  /// @param component Base component passed through from derived class
  /// @returns std::expected<std::monostate, FailInfo>
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureComponent(Component &component) override;

  /////////////////////////////////////////////////
  /// @brief Configure CUserInterface component
  ///
  /// @param c_ui_component Reference to CUserInterface component to configure
  /// @returns std::expected<std::monostate, FailInfo>
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureCUserInterface(CUserInterface &c_ui_component) override;

  /////////////////////////////////////////////////
  /// @brief Configure CUIState component
  ///
  /// @param c_ui_state_component Reference to CUIState component to configure
  /// @returns std::expected<std::monostate, FailInfo>
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureCUIState(CUIState &c_ui_state_component,
                    EntityMemoryPool &emp) override;

  /////////////////////////////////////////////////
  /// @brief Configure CGrimoireMachina component
  ///
  /// @param c_grimoire_component Reference to CGrimoireMachina component to
  /// configure
  /// @returns std::expected<std::monostate, FailInfo>
  //////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureCGrimoireMachina(CGrimoireMachina &c_grimoire_component) override;

  /////////////////////////////////////////////////
  /// @brief Configure CMachinaForm component
  ///
  /// @param c_machina_form_component Reference to CMachinaForm component to
  /// configure
  /// @returns std::expected<std::monostate, FailInfo>
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureCMachinaForm(CMachinaForm &c_machina_form_component) override;
};

} // namespace steamrot
