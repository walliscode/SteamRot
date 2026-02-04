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
#include "SceneType.h"
#include "engine_data_generated.h"
#include "fragment_generated.h"
#include "joint_generated.h"
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
  /// @brief find and load all ui styles from flatbuffers data
  /////////////////////////////////////////////////
  std::expected<std::vector<const UIStyleData *>, FailInfo>
  ProvideUIStylesData() const;

  /////////////////////////////////////////////////
  /// @brief Provides EngineDataFbs from binary file
  /////////////////////////////////////////////////
  std::expected<const EngineDataFbs *, FailInfo> ProvideEngineDataFbs() const;

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

  /////////////////////////////////////////////////
  /// @brief Provies all FragmentFbs from the fragments directory
  /////////////////////////////////////////////////
  std::expected<std::vector<const FragmentFbs *>, FailInfo>
  ProvideAllFragmentData() const;

  /////////////////////////////////////////////////
  /// @brief Provides all JointFbs from the joints directory
  /////////////////////////////////////////////////
  std::expected<std::vector<const JointFbs *>, FailInfo>
  ProvideAllJointData() const;
};

} // namespace steamrot
