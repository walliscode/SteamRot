/////////////////////////////////////////////////
/// @file
/// @brief Declaration of abstract SceneData struct
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AssetConfig.h"
#include "IEntityImporter.h"
#include "SceneInfo.h"
#include "SceneResourcesConfig.h"
#include "containers.h"
#include <memory>

namespace steamrot {
/////////////////////////////////////////////////
/// @class SceneData
/// @brief Contains all data needed to configure a Scene and provide resources
/// for it.

/////////////////////////////////////////////////
struct SceneData {

  /////////////////////////////////////////////////
  /// @brief Scene information data.
  /////////////////////////////////////////////////
  SceneInfo scene_info;

  /////////////////////////////////////////////////
  /// @brief SceneResources  configuration data.
  /////////////////////////////////////////////////
  SceneResourcesConfig scene_resources_config;

  /////////////////////////////////////////////////
  /// @brief Contains data needed to load all assets for this Scene
  /////////////////////////////////////////////////
  AssetConfig scene_asset_config;

  /////////////////////////////////////////////////
  /// @brief Entity importer (wraps entity data source)
  /// @note Can be one of:
  /// - std::unique_ptr<IEntityImporter> for file-based importers
  /// - std::shared_ptr<EntityMemoryPool> for shared in-memory pool
  /// - a copy constructed EntityMemoryPool for storing mutliple instances of
  /// the EntityMemoryPool
  /////////////////////////////////////////////////
  std::variant<std::unique_ptr<IEntityImporter>,
               std::shared_ptr<EntityMemoryPool>, EntityMemoryPool>
      entity_transport;
};

using SceneCollectionData = std::vector<SceneData>;

} // namespace steamrot
