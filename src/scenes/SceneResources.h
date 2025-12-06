/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the SceneResources struct.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ActionManager.h"
#include "EntityManager.h"
#include "Logic.h"
#include "SceneCore.h"
#include <memory>
#include <unordered_map>
#include <vector>

namespace steamrot {

// Forward declarations - LogicCollection is defined in LogicFactory.h
using LogicVector = std::vector<std::unique_ptr<Logic>>;
using LogicCollection = std::unordered_map<LogicType, LogicVector>;

/////////////////////////////////////////////////
/// @struct SceneResources
/// @brief Scene-level resources struct containing long-lived objects.
///
/// SceneResources owns all scene-level resources (managers, logic systems,
/// render texture). These resources have a lifetime matching the Scene instance.
///
/// All members are concrete objects (no references/pointers to other
/// scene members). This struct is default-constructible.
/////////////////////////////////////////////////
struct SceneResources {
  SceneResources() = default;

  /////////////////////////////////////////////////
  /// @brief Entity Manager instance for this scene
  /////////////////////////////////////////////////
  EntityManager entity_manager;

  /////////////////////////////////////////////////
  /// @brief Action Manager instance for this scene
  /////////////////////////////////////////////////
  ActionManager action_manager;

  /////////////////////////////////////////////////
  /// @brief Map of all logic objects needed by the scene
  /////////////////////////////////////////////////
  LogicCollection logic_map;

  /////////////////////////////////////////////////
  /// @brief Scene-level core objects (render texture, etc.)
  /////////////////////////////////////////////////
  SceneCore scene_core;
};

} // namespace steamrot
