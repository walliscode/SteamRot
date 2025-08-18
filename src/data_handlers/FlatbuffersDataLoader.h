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
#include "scene_types_generated.h"
#include "scenes_generated.h"
#include <expected>
#include <map>
#include <string>

namespace steamrot {
class FlatbuffersDataLoader : public DataLoader {

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for FlatbuffersDataLoader taking an EnvironmentType
  ///
  /// @param env_type Enumeration representing the environment type
  /////////////////////////////////////////////////
  FlatbuffersDataLoader(const EnvironmentType env_type = EnvironmentType::None);

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
  /// @brief Provides SceneData based on the SceneType
  ///
  /// @param scene_type Enum representing the type of scene
  /////////////////////////////////////////////////
  std::expected<const SceneData *, FailInfo>
  ProvideSceneData(const SceneType scene_type) const;

  /////////////////////////////////////////////////
  /// @brief Provides AssetCollection data as a subset of SceneData
  ///
  /// @param scene_type An enum representing the type of scene
  /////////////////////////////////////////////////
  std::expected<const AssetCollection *, FailInfo>
  ProvideAssetData(const SceneType scene_type) const;
};

} // namespace steamrot
