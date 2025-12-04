/////////////////////////////////////////////////
/// @file
/// @brief Interface for loading scene configuration data.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "scene_types_generated.h"
#include <cstdint>
#include <expected>
#include <string>

namespace steamrot {

/////////////////////////////////////////////////
/// @struct SceneCoreData
/// @brief Native C++ struct for scene core configuration.
///
/// This replaces the FlatBuffers SceneCoreData type in game code.
/// Provides a simple, mutable structure for scene configuration.
/////////////////////////////////////////////////
struct SceneCoreData {
  uint32_t render_texture_width{800};
  uint32_t render_texture_height{600};
};

/////////////////////////////////////////////////
/// @struct SceneData
/// @brief Complete scene data including core and metadata.
///
/// Note: Entity data is handled by EntityConfigurator
/// Note: Logic data is handled by LogicFactory
/// Note: Assets are handled by IAssetDataProvider
/////////////////////////////////////////////////
struct SceneData {
  SceneType scene_type{SceneType::SceneType_UNKNOWN};
  std::string scene_id;
  SceneCoreData core;
};

/////////////////////////////////////////////////
/// @class ISceneDataProvider
/// @brief Interface for loading scene configuration data.
///
/// Implementations handle the actual data source (files, network, etc.)
/// and format (FlatBuffers, JSON, XML, Lua, etc.).
///
/// Usage:
/// ```cpp
/// ISceneDataProvider& provider = GetSceneDataProvider();
/// auto result = provider.LoadSceneCoreData(SceneType::SceneType_TITLE);
/// if (result.has_value()) {
///   const SceneCoreData& data = result.value();
///   // Use native C++ struct
/// }
/// ```
/////////////////////////////////////////////////
class ISceneDataProvider {
public:
  virtual ~ISceneDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Load scene data for a specific scene type.
  ///
  /// @param scene_type The type of scene to load
  /// @return Scene data or failure information
  /////////////////////////////////////////////////
  virtual std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType scene_type) const = 0;

  /////////////////////////////////////////////////
  /// @brief Load scene core configuration for a specific scene type.
  ///
  /// @param scene_type The type of scene to load
  /// @return Scene core data or failure information
  /////////////////////////////////////////////////
  virtual std::expected<SceneCoreData, FailInfo>
  LoadSceneCoreData(SceneType scene_type) const = 0;
};

} // namespace steamrot
