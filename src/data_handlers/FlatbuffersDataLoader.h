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
#include "game_engine_generated.h"
#include "logic_data_generated.h"
#include "resource_data_generated.h"
#include "scene_change_packet_generated.h"
#include "scene_manager_generated.h"
#include "scenes_generated.h"
#include "ui_style_generated.h"
#include <expected>
#include <map>
#include <string>

namespace steamrot {
class FlatbuffersDataLoader : public DataLoader {

public:
/////////////////////////////////////////////////
  /// @brief Default constructor for FlatbuffersDataLoader
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
  /// @brief Provides GameEngineData from binary file
/////////////////////////////////////////////////
  std::expected<const GameEngineData *, FailInfo> ProvideGameEngineData() const;

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
  std::expected<const SceneData *, FailInfo>
  ProvideSceneData(const SceneType scene_type) const;

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
  /// @brief Provides GameResourcesData from binary file
/////////////////////////////////////////////////
  std::expected<const GameResourcesData *, FailInfo>
  ProvideGameResourcesData() const;

/////////////////////////////////////////////////
  /// @brief Provides SceneResourcesData from SceneData for a specific scene
/////////////////////////////////////////////////
  std::expected<const SceneResourcesData *, FailInfo>
  ProvideSceneResourcesData(const SceneType scene_type) const;

/////////////////////////////////////////////////
  /// @brief Provides LogicCollectionData from SceneData for a specific scene
  ///
  /// @param scene_type SceneType enum representing the type of scene
/////////////////////////////////////////////////
  std::expected<const LogicCollectionData *, FailInfo>
  ProvideLogicCollectionData(const SceneType scene_type) const;
};

} // namespace steamrot
