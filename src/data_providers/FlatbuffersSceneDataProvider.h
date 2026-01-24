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
#include "scene_data_generated.h"
#include "scene_types_generated.h"
#include <expected>

namespace steamrot {

class FlatbuffersSceneDataProvider : public steamrot::ISceneDataProvider {

private:
  /////////////////////////////////////////////////
  /// @brief Reference to EventHandler
  /////////////////////////////////////////////////
  EventHandler &m_event_handler;

  /////////////////////////////////////////////////
  /// @brief Pointer to FlatBuffers SceneData
  /////////////////////////////////////////////////
  const SceneDataFbs *m_scene_data_fbs{nullptr};

public:
  /////////////////////////////////////////////////
  /// @brief Constructor
  ///
  /// @param event_handler Reference to EventHandler for event reporting.
  /////////////////////////////////////////////////
  FlatbuffersSceneDataProvider(EventHandler &event_handler);

  /////////////////////////////////////////////////
  /// @brief Constructor
  ///
  /// @param scene_data pointer to FlatBuffers SceneData
  /////////////////////////////////////////////////
  FlatbuffersSceneDataProvider(EventHandler &event_handler,
                               const SceneDataFbs *scene_data);

  /////////////////////////////////////////////////
  /// @brief Providers SceneData object
  ///
  /// @param scene_type ScenType enum
  /////////////////////////////////////////////////
  std::expected<SceneData, FailInfo>
  CreateSceneData(const SceneType scene_type) override;

  std::expected<std::monostate, FailInfo>
  ConfigureSceneData(SceneData &scene_data) const override;
};
} // namespace steamrot
