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
#include "SceneContext.h"
#include <memory>
#include <unordered_map>
#include <variant>

namespace steamrot {

enum class LogicType {
  Collision,
  Render,
  Action,
  Movement,
};
using LogicVector = std::vector<std::unique_ptr<Logic>>;
using LogicCollection = std::unordered_map<LogicType, LogicVector>;
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
};
} // namespace steamrot
