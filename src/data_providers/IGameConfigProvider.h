/////////////////////////////////////////////////
/// @file
/// @brief Interface for loading game configuration data that still uses FlatBuffers.
///
/// This interface provides access to complex configuration data (subscriptions,
/// event buses, scene manager config) that hasn't yet been converted to native
/// C++ structs. This is temporary until Phase 4 of the data loading migration.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "engine_data_generated.h"
#include "scene_manager_data_generated.h"
#include <expected>

namespace steamrot {

/////////////////////////////////////////////////
/// @class IGameConfigProvider
/// @brief Interface for loading game configuration data.
///
/// This interface provides access to EngineDataFbs and SceneManagerData
/// which contain complex configuration like subscriptions and event buses.
///
/// NOTE: This is a temporary interface for Phase 3 of the migration.
/// Phase 4 will convert these to native C++ structs, making this interface
/// unnecessary.
///
/// Usage:
/// ```cpp
/// IGameConfigProvider& provider = GetGameConfigProvider();
/// auto result = provider.LoadEngineConfig();
/// if (result.has_value()) {
///   const EngineDataFbs* data = result.value();
///   // Configure subscriptions, event buses, etc.
/// }
/// ```
/////////////////////////////////////////////////
class IGameConfigProvider {
public:
  virtual ~IGameConfigProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Load complete engine configuration.
  ///
  /// Returns EngineDataFbs which contains:
  /// - Subscriptions for engine-level events
  /// - SceneManagerData for scene management
  /// - EventBusData for global and waiting room event buses
  /// - EngineCoreData for window/framerate config
  ///
  /// @return EngineDataFbs pointer or failure information
  /////////////////////////////////////////////////
  virtual std::expected<const EngineDataFbs *, FailInfo>
  LoadEngineConfig() const = 0;

  /////////////////////////////////////////////////
  /// @brief Load scene manager configuration.
  ///
  /// Returns SceneManagerData which contains:
  /// - Subscriptions for scene-level events
  /// - SceneData for all scene types
  ///
  /// @return SceneManagerData pointer or failure information
  /////////////////////////////////////////////////
  virtual std::expected<const SceneManagerData *, FailInfo>
  LoadSceneManagerConfig() const = 0;
};

} // namespace steamrot
