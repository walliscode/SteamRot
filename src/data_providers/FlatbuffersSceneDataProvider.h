/////////////////////////////////////////////////
/// @file
/// @brief Declaration of FlatbuffersSceneDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventHandler.h"
#include "ISceneDataProvider.h"
#include "SceneResourcesConfig.h"
#include "scene_resources_config_generated.h"
#include "scene_types_generated.h"
#include <memory>

namespace steamrot {
class FlatbuffersSceneDataProvider : public steamrot::ISceneDataProvider {

private:
  EventHandler &m_event_handler;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor
  ///
  /// @param event_handler Reference to EventHandler for event reporting.
  /////////////////////////////////////////////////
  FlatbuffersSceneDataProvider(EventHandler &event_handler);

  /////////////////////////////////////////////////
  /// @brief Configures SceneInfo from FlatBuffers data.
  ///
  /// @param info SceneInfo to configure.
  /// @param fb_info FlatBuffers SceneInfo data.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(SceneInfo &info, const SceneInfoFbs *fb_info) const;

  /////////////////////////////////////////////////
  /// @brief Configures SceneResourcesConfig from FlatBuffers data.
  ///
  /// @param config SceneResourcesConfig to configure.
  /// @param fb_config FlatBuffers SceneResourcesConfig data.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneResourcesConfig(SceneResourcesConfig &config,
                                const SceneResourcesConfigFbs *fb_config) const;

  /////////////////////////////////////////////////
  /// @brief Loads SceneData from FlatBuffers.
  ///
  /// @return Unique pointer to SceneData object.
  /////////////////////////////////////////////////
  std::expected<std::unique_ptr<SceneData>, FailInfo>
  ProvideDefaultSceneData(const SceneType scene_type) const override;

  /////////////////////////////////////////////////
  /// @brief Provides SceneData from FlatBuffers data.
  ///
  /// @param scene_data_fbs FlatBuffers SceneData for scene.
  /////////////////////////////////////////////////
  std::expected<std::unique_ptr<SceneData>, FailInfo>
  ProvideSceneDataFromData(const SceneDataFbs *scene_data_fbs) const override;

  std::expected<SceneLoadData, FailInfo>
  ProvideSceneLoadData(const SceneType scene_type) const override;
};
} // namespace steamrot
