/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the LogicFactory class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Logic.h"
#include "LogicType.h"
#include "SceneContext.h"
#include "SceneType.h"
#include <expected>
#include <memory>
#include <variant>

namespace steamrot::logic {

using LogicVector = std::vector<std::unique_ptr<Logic>>;
using LogicCollection = std::unordered_map<LogicGrouping, LogicVector>;
/////////////////////////////////////////////////
/// @class LogicFactory
/// @brief Provides Logic objects for Scenes to store and use.
///
/////////////////////////////////////////////////
class LogicFactory {

private:
  const SceneContext &m_scene_context;

  /////////////////////////////////////////////////
  /// @brief Creates an empty LogicCollection with all LogicTypes initialized.
  ///
  /// @return LogicCollection with empty LogicVectors for each LogicType.
  /////////////////////////////////////////////////
  static LogicCollection CreateEmptyLogicCollection();

  /////////////////////////////////////////////////
  /// @brief Helper method to add multiple Logic objects to a LogicCollection.
  ///
  /// @param logic_collection The LogicCollection to add Logic objects to.
  /// @param grouping The LogicGrouping to add the Logic objects to.
  /// @param logic_types Vector of LogicTypes to create and add.
  /// @return std::monostate on success, FailInfo on failure.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  AddLogicsToCollection(LogicCollection &logic_collection,
                        LogicGrouping grouping,
                        const std::vector<LogicType> &logic_types);

  /////////////////////////////////////////////////
  /// @brief Configure the LogicCollection for the Title Scene.
  ///
  /// @param logic_collection A logic collection to configure.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureTitleLogics(LogicCollection &logic_collection);

  ////////////////////////////////////////////////
  /// @brief Configure the LogicCollection for the Crafting Scene.
  ///
  /// @param logic_collection A logic collection to configure.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureCraftingLogics(LogicCollection &logic_collection);

  ////////////////////////////////////////////////
  /// @brief Configure the LogicCollection for the Test Scene.
  ///
  /// @param logic_collection A logic collection to configure.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureTestLogics(LogicCollection &logic_collection);

  /////////////////////////////////////////////////
  /// @brief Create a configured a Logic object based on the LogicType.
  ///
  /// @param logic_type Logic object type to create.
  /////////////////////////////////////////////////
  std::expected<std::unique_ptr<Logic>, FailInfo>
  CreateLogicObject(LogicType logic_type);

public:
  /////////////////////////////////////////////////
  /// @brief Constructor
  ///
  /// @param scene_context SceneContext reference for context information.
  /////////////////////////////////////////////////
  LogicFactory(const SceneContext &scene_context);

  /////////////////////////////////////////////////
  /// @brief Provides a LogicCollection based on the SceneType
  ///
  /// @param scene_type SceneType for which to provide the LogicCollection.
  /////////////////////////////////////////////////
  std::expected<LogicCollection, FailInfo>
  ProvideLogicCollection(SceneType scene_type);

  /////////////////////////////////////////////////
  /// @brief Configures the provided Logic object
  ///
  /// @param logic_object Logic object to configure.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureLogicObject(Logic &logic_object);
};
} // namespace steamrot::logic
