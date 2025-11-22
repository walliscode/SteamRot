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
#include "logic_data_generated.h"
#include "scene_change_packet_generated.h"
#include <expected>
#include <memory>
#include <unordered_map>
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
  /////////////////////////////////////////////////
  /// @brief SceneType of the scene for which logics are created.
  /////////////////////////////////////////////////
  const SceneType m_scene_type;

  /////////////////////////////////////////////////
  /// @brief member variable that holds the SceneContext
  /////////////////////////////////////////////////
  SceneContext m_scene_context;

  /////////////////////////////////////////////////
  /// @brief Create a vector of logic objects specifically for collision
  ///
  /////////////////////////////////////////////////
  std::expected<LogicVector, FailInfo> CreateCollisionLogics();

  /////////////////////////////////////////////////
  /// @brief Create a vector of logic objects specifically for rendering
  /////////////////////////////////////////////////
  std::expected<LogicVector, FailInfo> CreateRenderLogics();

  /////////////////////////////////////////////////
  /// @brief Create a vector of logic objects specifically for actions
  /////////////////////////////////////////////////
  std::expected<LogicVector, FailInfo> CreateActionLogics();

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for the LogicFactory class.
  ///
  /// @param scene_type SceneType of the scene for which logics are created
  /// @param scene_context SceneContext object containing references to the
  /// scene
  /////////////////////////////////////////////////
  LogicFactory(const SceneType scene_type, const SceneContext &scene_context);

  /////////////////////////////////////////////////
  /// @brief Create and return a map of logic objects.
  ///
  /////////////////////////////////////////////////
  std::expected<LogicCollection, FailInfo>
  CreateLogicMap(const LogicCollectionData &logic_collection_data);
};
} // namespace steamrot
