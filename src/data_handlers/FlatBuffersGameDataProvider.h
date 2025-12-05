////////////////////////////////////////////////////////////
/// @file
/// @brief Declaration of FlatBuffersGameDataProvider
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Preprocessor Directives
////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include "FlatbuffersDataLoader.h"
#include "IGameDataProvider.h"

namespace steamrot {

////////////////////////////////////////////////////////////
/// @class FlatBuffersGameDataProvider
/// @brief Concrete implementation of IGameDataProvider using FlatBuffers
///
/// @deprecated This class has been replaced by domain-specific providers:
/// - FlatbuffersEngineDataProvider for engine data
/// - FlatbuffersSceneDataProvider for scene data
/// - FlatbuffersAssetDataProvider for asset data
/// - FlatbuffersFragmentDataProvider for fragment data
/// Use the new provider classes from data_providers/ instead.
///
/// Wraps FlatbuffersDataLoader and implements the IGameDataProvider
/// interface, hiding FlatBuffers implementation details from consuming code.
////////////////////////////////////////////////////////////
[[deprecated("Use domain-specific providers (FlatbuffersEngineDataProvider, FlatbuffersSceneDataProvider, etc.) instead")]]
class FlatBuffersGameDataProvider : public IGameDataProvider {
private:
  ////////////////////////////////////////////////////////////
  /// @brief Internal FlatBuffers data loader
  ////////////////////////////////////////////////////////////
  FlatbuffersDataLoader m_loader;

public:
  ////////////////////////////////////////////////////////////
  /// @brief Default constructor
  ////////////////////////////////////////////////////////////
  FlatBuffersGameDataProvider() = default;

  ////////////////////////////////////////////////////////////
  /// @brief Load engine data containing core configuration
  ///
  /// @return Expected containing EngineData pointer or FailInfo
  ////////////////////////////////////////////////////////////
  std::expected<const EngineDataFbs *, FailInfo>
  LoadEngineData() const override;

  ////////////////////////////////////////////////////////////
  /// @brief Load scene manager configuration data
  ///
  /// @return Expected containing SceneManagerData pointer or FailInfo
  ////////////////////////////////////////////////////////////
  std::expected<const SceneManagerData *, FailInfo>
  LoadSceneManagerData() const override;

  ////////////////////////////////////////////////////////////
  /// @brief Load scene-specific data for a given scene type
  ///
  /// @param scene_type Type of scene to load data for
  /// @return Expected containing SceneDataData pointer or FailInfo
  ////////////////////////////////////////////////////////////
  std::expected<const SceneDataData *, FailInfo>
  LoadSceneData(const SceneType scene_type) const override;

  ////////////////////////////////////////////////////////////
  /// @brief Load default user preferences
  ///
  /// @return Expected containing UserPreferencesData pointer or FailInfo
  ////////////////////////////////////////////////////////////
  std::expected<const UserPreferencesData *, FailInfo>
  LoadUserPreferences() const override;

  ////////////////////////////////////////////////////////////
  /// @brief Load engine configuration settings
  ///
  /// Loads from user config if available, otherwise uses defaults.
  ///
  /// @return Expected containing EngineConfigData pointer or FailInfo
  ////////////////////////////////////////////////////////////
  std::expected<const EngineConfigData *, FailInfo>
  LoadEngineConfig() const override;

  ////////////////////////////////////////////////////////////
  /// @brief Load UI style data by style name
  ///
  /// @param style_name Name of the UI style to load
  /// @return Expected containing UIStyleData pointer or FailInfo
  ////////////////////////////////////////////////////////////
  std::expected<const UIStyleData *, FailInfo>
  LoadUIStyle(const std::string &style_name) const override;

  ////////////////////////////////////////////////////////////
  /// @brief Load engine core data from engine data
  ///
  /// @return Expected containing EngineCoreDataFbs pointer or FailInfo
  ////////////////////////////////////////////////////////////
  std::expected<const EngineCoreDataFbs *, FailInfo>
  LoadEngineCoreData() const override;

  ////////////////////////////////////////////////////////////
  /// @brief Load scene core data for specific scene type
  ///
  /// @param scene_type Type of scene to load core data for
  /// @return Expected containing SceneCoreDataFbs pointer or FailInfo
  ////////////////////////////////////////////////////////////
  std::expected<const SceneCoreDataFbs *, FailInfo>
  LoadSceneCoreData(const SceneType scene_type) const override;
};

} // namespace steamrot
