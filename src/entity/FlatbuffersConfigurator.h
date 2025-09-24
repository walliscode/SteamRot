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
#include "CUserInterface.h"
#include "EntityConfigurator.h"
#include "EventHandler.h"
#include "FailInfo.h"
#include "FlatbuffersDataLoader.h"
#include "containers.h"
#include "grimoire_machina_generated.h"
#include "user_interface_generated.h"

#include <expected>
#include <variant>

namespace steamrot {
class FlatbuffersConfigurator : public EntityConfigurator {

private:
  /////////////////////////////////////////////////
  /// @brief FlatbuffersDataLoader instance for providing flatbuffers data
  /////////////////////////////////////////////////
  FlatbuffersDataLoader m_data_loader;

  /////////////////////////////////////////////////
  /// @brief For Configuring values sitting on the abstract Component class
  ///
  /// @param entity_data Any flatbuffers table data
  /// @param component Instance of derived Component to be configured
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureComponent(Component &component);

  /////////////////////////////////////////////////
  /// @brief Overloaded method for configuring CUserInterface component
  ///
  /// @param ui_data Flatbuffers table data for UserInterface
  /// @param ui_component CUserInterface instance to be configured
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureComponent(const UserInterfaceData *ui_data,
                     CUserInterface &ui_component);

  /////////////////////////////////////////////////
  /// @brief Overloaded method for configuring CGrimoireMachina component
  ///
  /// @param grimoire_data Flatbuffers table data for GrimoireMachina
  /// @param grimoire_component CGrimoireMachina instance to be configured
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureComponent(const GrimoireMachinaData *grimoire_data,
                     CGrimoireMachina &grimoire_component);

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for FlatbuffersConfigurator
  ///
  /// @param event_handler Gamewide EventHandler reference
  /////////////////////////////////////////////////
  FlatbuffersConfigurator(EventHandler &event_handler);

  /////////////////////////////////////////////////
  /// @brief Configure the default entities based on the SceneType
  ///
  /// @param entity_memory_pool EntityMemoryPool to configure
  /// @param scene_type SceneType enum indicating which scene to load
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureEntitiesFromDefaultData(EntityMemoryPool &entity_memory_pool,
                                   const SceneType scene_type);
};

} // namespace steamrot
