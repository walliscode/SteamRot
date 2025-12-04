////////////////////////////////////////////////////////////
/// @file
/// @brief Declaration of IGameDataProvider interface
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Preprocessor Directives
////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include "FailInfo.h"
#include "SceneType.h"
#include "core_data_generated.h"
#include "engine_config_generated.h"
#include "engine_data_generated.h"
#include "scene_data_generated.h"
#include "scene_manager_data_generated.h"
#include "ui_style_generated.h"
#include "user_preferences_generated.h"
#include <expected>
#include <string>

namespace steamrot {

////////////////////////////////////////////////////////////
/// @class IGameDataProvider
/// @brief Abstract interface for loading game data
///
/// This interface abstracts away the underlying data source
/// (FlatBuffers, Lua, JSON, etc.) and provides a consistent
/// API for loading game configuration and runtime data.
///
/// Future implementations can swap FlatBuffers for alternative
/// data formats without changing consuming code.
////////////////////////////////////////////////////////////
class IGameDataProvider {
public:
  ////////////////////////////////////////////////////////////
  /// @brief Virtual destructor for proper cleanup
  ////////////////////////////////////////////////////////////
  virtual ~IGameDataProvider() = default;

  ////////////////////////////////////////////////////////////
  /// @brief Load engine data containing core configuration
  ///
  /// @return Expected containing EngineData pointer or FailInfo
  ////////////////////////////////////////////////////////////
  virtual std::expected<const EngineData *, FailInfo>
  LoadEngineData() const = 0;

  ////////////////////////////////////////////////////////////
  /// @brief Load scene manager configuration data
  ///
  /// @return Expected containing SceneManagerData pointer or FailInfo
  ////////////////////////////////////////////////////////////
  virtual std::expected<const SceneManagerData *, FailInfo>
  LoadSceneManagerData() const = 0;

  ////////////////////////////////////////////////////////////
  /// @brief Load scene-specific data for a given scene type
  ///
  /// @param scene_type Type of scene to load data for
  /// @return Expected containing SceneDataData pointer or FailInfo
  ////////////////////////////////////////////////////////////
  virtual std::expected<const SceneDataData *, FailInfo>
  LoadSceneData(const SceneType scene_type) const = 0;

  ////////////////////////////////////////////////////////////
  /// @brief Load default user preferences
  ///
  /// @return Expected containing UserPreferencesData pointer or FailInfo
  ////////////////////////////////////////////////////////////
  virtual std::expected<const UserPreferencesData *, FailInfo>
  LoadUserPreferences() const = 0;

  ////////////////////////////////////////////////////////////
  /// @brief Load engine configuration settings
  ///
  /// Loads from user config if available, otherwise uses defaults.
  ///
  /// @return Expected containing EngineConfigData pointer or FailInfo
  ////////////////////////////////////////////////////////////
  virtual std::expected<const EngineConfigData *, FailInfo>
  LoadEngineConfig() const = 0;

  ////////////////////////////////////////////////////////////
  /// @brief Load UI style data by style name
  ///
  /// @param style_name Name of the UI style to load
  /// @return Expected containing UIStyleData pointer or FailInfo
  ////////////////////////////////////////////////////////////
  virtual std::expected<const UIStyleData *, FailInfo>
  LoadUIStyle(const std::string &style_name) const = 0;

  ////////////////////////////////////////////////////////////
  /// @brief Load engine core data from engine data
  ///
  /// @return Expected containing EngineCoreData pointer or FailInfo
  ////////////////////////////////////////////////////////////
  virtual std::expected<const EngineCoreData *, FailInfo>
  LoadEngineCoreData() const = 0;

  ////////////////////////////////////////////////////////////
  /// @brief Load scene core data for specific scene type
  ///
  /// @param scene_type Type of scene to load core data for
  /// @return Expected containing SceneCoreData pointer or FailInfo
  ////////////////////////////////////////////////////////////
  virtual std::expected<const SceneCoreData *, FailInfo>
  LoadSceneCoreData(const SceneType scene_type) const = 0;
};

} // namespace steamrot
