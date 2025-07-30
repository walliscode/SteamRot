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
#include <string>
#include <unordered_map>

namespace steamrot {
class FlatbuffersDataLoader : public DataLoader {

public:
  FlatbuffersDataLoader(const EnvironmentType env_type = EnvironmentType::None);
  /////////////////////////////////////////////////
  /// @brief Provides Fragment object based on the fragment name
  ///
  /// Any fail modes are provided through std::expected as the error type
  /// @param fragment_name String representing the name of the fragment
  /////////////////////////////////////////////////
  std::expected<Fragment, FailureData>
  ProvideFragment(const std::string &fragment_name) const override;

  /////////////////////////////////////////////////
  /// @brief Provides all Fragments based on the provided names
  ///
  /// @param fragment_names Vector of strings representing the names of the
  /// fragments
  /////////////////////////////////////////////////
  std::expected<std::unordered_map<std::string, Fragment>, FailureData>
  ProvideAllFragments(std::vector<std::string> fragment_names) const override;

  /////////////////////////////////////////////////
  /// @brief Provides SceneData based on the SceneType
  ///
  /// @param scene_type Enum representing the type of scene
  /////////////////////////////////////////////////
  const std::expected<SceneData, FailureData>
  ProvideSceneData(const SceneType scene_type) const;
};

} // namespace steamrot
