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
#include "ISceneLoadDataProvider.h"
#include "SceneResourcesConfig.h"
#include "scene_resources_config_generated.h"
#include "scene_types_generated.h"
#include <variant>

namespace steamrot {
class FlatbuffersSceneLoadDataProvider
    : public steamrot::ISceneLoadDataProvider {

private:
  EventHandler &m_event_handler;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor
  ///
  /// @param event_handler Reference to EventHandler for event reporting.
  /////////////////////////////////////////////////
  FlatbuffersSceneLoadDataProvider(EventHandler &event_handler);

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
  /// @brief Provides SceneData from FlatBuffers data.
  ///
  /// @param scene_data_fbs FlatBuffers SceneData for scene.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneDataFromData(SceneData &scene_data,
                             const SceneDataFbs *scene_data_fbs) const;

  /////////////////////////////////////////////////
  /// @brief Provide
  ///
  /// @param scene_type [TODO:parameter]
  /////////////////////////////////////////////////
  std::expected<SceneData, FailInfo>
  ProvideSceneDataFromData(const SceneDataFbs *fb_data) const override;

  /////////////////////////////////////////////////
  /// @brief Providers SceneData object
  ///
  /// @param scene_type ScenType enum
  /////////////////////////////////////////////////
  std::expected<SceneData, FailInfo>
  ProvideDefaultSceneData(const SceneType scene_type) const override;
};
} // namespace steamrot
