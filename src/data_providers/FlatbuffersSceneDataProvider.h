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
#include "scene_types_generated.h"

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
