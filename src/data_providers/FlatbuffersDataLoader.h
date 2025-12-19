/////////////////////////////////////////////////
/// @file
/// @brief Definition of FlatbuffersDataLoader class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "DataLoader.h"
#include "FailInfo.h"
#include "engine_config_generated.h"
#include "engine_resources_config_generated.h"
#include "engine_state_generated.h"
#include "logic_data_generated.h"
#include "scene_data_generated.h"
#include "scene_manager_data_generated.h"
#include "ui_style_generated.h"
#include "user_preferences_generated.h"
#include <expected>

namespace steamrot {
class FlatbuffersDataLoader : public DataLoader {

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor
  /////////////////////////////////////////////////
  FlatbuffersDataLoader() = default;

  /////////////////////////////////////////////////
  /// @brief Provides SceneData based on the SceneType
  ///
  /// @param scene_type Enum representing the type of scene
  /////////////////////////////////////////////////
  std::expected<const SceneDataFbs *, FailInfo>
  ProvideDefaultSceneData(const SceneType scene_type) const;

  /////////////////////////////////////////////////
  /// @brief Provide default AssetCollection data
  /////////////////////////////////////////////////
  std::expected<const AssetCollection *, FailInfo> ProvideAssetData() const;

  /////////////////////////////////////////////////
  /// @brief Provides AssetCollection data as a subset of SceneData
  ///
  /// @param scene_type An enum representing the type of scene
  /////////////////////////////////////////////////
  std::expected<const AssetCollection *, FailInfo>
  ProvideAssetData(const SceneType scene_type) const;

  /////////////////////////////////////////////////
  /// @brief find and load all ui styles from flatbuffers data
  /////////////////////////////////////////////////
  std::expected<std::vector<const UIStyleData *>, FailInfo>
  ProvideUIStylesData() const;

  /////////////////////////////////////////////////
  /// @brief Provides EngineResourcesConfigFbs from binary file
  ///
  /// Loads engine resources configuration from
  /// defaults/engine/engine_resources_config.bin
  /////////////////////////////////////////////////
  std::expected<const EngineResourcesConfigFbs *, FailInfo>
  ProvideEngineResourcesConfigFbs() const;

  /////////////////////////////////////////////////
  /// @brief Provides EngineConfigFbs from binary file
  ///
  /// Loads engine configuration from defaults/engine/default.engine_config.bin.
  /// If user configuration exists at user/engine/engine_config.bin, it
  /// will be loaded instead.
  /////////////////////////////////////////////////
  std::expected<const EngineConfigFbs *, FailInfo>
  ProvideEngineConfigFbs() const;

  /////////////////////////////////////////////////
  /// @brief Provides EngineStateFbs from binary file
  /////////////////////////////////////////////////
  std::expected<const EngineStateFbs *, FailInfo> ProvideEngineStateFbs() const;

  /////////////////////////////////////////////////
  /// @brief Retrieves SceneManagerDataFbs from binary file
  /////////////////////////////////////////////////
  std::expected<const SceneManagerDataFbs *, FailInfo>
  ProvideSceneManagerData() const;

  /////////////////////////////////////////////////
  /// @brief Provides LogicCollectionData from SceneData for a specific
  /// scene
  ///
  /// @param scene_type SceneType enum representing the type of scene
  /////////////////////////////////////////////////
  std::expected<const LogicCollectionData *, FailInfo>
  ProvideLogicCollectionData(const SceneType scene_type) const;

  /////////////////////////////////////////////////
  /// @brief Provides default UserPreferencesData from binary file
  ///
  /// This loads the default preferences from the binary file generated
  /// from the default.preferences.json file.
  /////////////////////////////////////////////////
  std::expected<const UserPreferencesData *, FailInfo>
  ProvideDefaultUserPreferencesData() const;
};

} // namespace steamrot
