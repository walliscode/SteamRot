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
#include "context_data_generated.h"
#include "core_data_generated.h"
#include "engine_config_generated.h"
#include "engine_data_generated.h"
#include "logic_data_generated.h"
#include "scene_data_generated.h"
#include "scene_manager_data_generated.h"
#include "ui_style_generated.h"
#include "user_preferences_generated.h"
#include <expected>
#include <map>
#include <string>

namespace steamrot {
class FlatbuffersDataLoader : public DataLoader {

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor
  /////////////////////////////////////////////////
  FlatbuffersDataLoader() = default;

  /////////////////////////////////////////////////
  /// @brief Provides Fragment object based on the fragment name
  ///
  /// Any fail modes are provided through std::expected as the error type
  /// @param fragment_name String representing the name of the fragment
  /////////////////////////////////////////////////
  std::expected<Fragment, FailInfo>
  ProvideFragment(const std::string &fragment_name) const override;

  /////////////////////////////////////////////////
  /// @brief Provides all Fragments based on the provided names
  ///
  /// @param fragment_names Vector of strings representing the names of the
  /// fragments
  /////////////////////////////////////////////////
  std::expected<std::map<std::string, Fragment>, FailInfo>
  ProvideAllFragments(std::vector<std::string> fragment_names) const override;

  /////////////////////////////////////////////////
  /// @brief Provides EngineData from binary file
  /////////////////////////////////////////////////
  std::expected<const EngineData *, FailInfo> ProvideEngineData() const;

  /////////////////////////////////////////////////
  /// @brief Provides SceneManagerData from binary file
  /////////////////////////////////////////////////
  std::expected<const SceneManagerData *, FailInfo>
  ProvideSceneManagerData() const;

  /////////////////////////////////////////////////
  /// @brief Provides SceneData based on the SceneType
  ///
  /// @param scene_type Enum representing the type of scene
  /////////////////////////////////////////////////
  std::expected<const SceneDataData *, FailInfo>
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
  /// @brief Provide a single UIStyleData object
  /////////////////////////////////////////////////
  std::expected<const UIStyleData *, FailInfo>
  ProvideUIStylesData(const std::string &style_name) const;

  /////////////////////////////////////////////////
  /// @brief Provides ContextData from binary file
  /////////////////////////////////////////////////
  std::expected<const ContextData *, FailInfo> ProvideContextData() const;

  /////////////////////////////////////////////////
  /// @brief Provides EngineCoreData from EngineData
  /////////////////////////////////////////////////
  std::expected<const EngineCoreData *, FailInfo>
  ProvideEngineCoreData() const;

  /////////////////////////////////////////////////
  /// @brief Provides SceneCoreData from SceneData for a specific scene
  /////////////////////////////////////////////////
  std::expected<const SceneCoreData *, FailInfo>
  ProvideSceneCoreData(const SceneType scene_type) const;

  /////////////////////////////////////////////////
  /// @brief Provides LogicCollectionData from SceneData for a specific scene
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

  /////////////////////////////////////////////////
  /// @brief Provides EngineConfigData from binary file
  ///
  /// Loads engine configuration from defaults/engine/engine_config.bin.
  /// If user configuration exists at user/engine/engine_config.bin, it
  /// will be loaded instead.
  /////////////////////////////////////////////////
  std::expected<const EngineConfigData *, FailInfo>
  ProvideEngineConfigData() const;
};

} // namespace steamrot
