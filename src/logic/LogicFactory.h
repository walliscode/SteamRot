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
#include "SubscriberFactory.h"
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
  /// @param collision_logic_data Vector of LogicData for collision logics
  /////////////////////////////////////////////////
  std::expected<LogicVector, FailInfo>
  CreateCollisionLogics(const flatbuffers::Vector<flatbuffers::Offset<LogicData>> *collision_logic_data);

  /////////////////////////////////////////////////
  /// @brief Create a vector of logic objects specifically for rendering
  ///
  /// @param render_logic_data Vector of LogicData for render logics
  /////////////////////////////////////////////////
  std::expected<LogicVector, FailInfo>
  CreateRenderLogics(const flatbuffers::Vector<flatbuffers::Offset<LogicData>> *render_logic_data);

  /////////////////////////////////////////////////
  /// @brief Create a vector of logic objects specifically for actions
  ///
  /// @param action_logic_data Vector of LogicData for action logics
  /////////////////////////////////////////////////
  std::expected<LogicVector, FailInfo>
  CreateActionLogics(const flatbuffers::Vector<flatbuffers::Offset<LogicData>> *action_logic_data);

  /////////////////////////////////////////////////
  /// @brief Attach subscribers to a Logic instance based on LogicData
  ///
  /// @param logic Logic instance to attach subscribers to
  /// @param logic_data LogicData containing subscriber configuration
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  AttachSubscribers(Logic &logic, const LogicData *logic_data);

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
