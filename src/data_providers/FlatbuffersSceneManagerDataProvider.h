/////////////////////////////////////////////////
/// @file
/// @brief FlatBuffers implementation of ISceneManagerDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "FlatbuffersDataLoader.h"
#include "ISceneManagerDataProvider.h"
#include "SceneManagerData.h"
#include "scene_manager_state_generated.h"
#include <variant>

namespace steamrot {

class FlatbuffersSceneManagerDataProvider : public ISceneManagerDataProvider {

private:
  /////////////////////////////////////////////////
  /// @brief Instance of FlatbuffersDataLoader for loading FlatBuffers data
  /////////////////////////////////////////////////
  FlatbuffersDataLoader m_loader;

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor
  /////////////////////////////////////////////////
  FlatbuffersSceneManagerDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Configure SceneManagerState from FlatBuffers data.
  ///
  /// @param state SceneManagerState to configure
  /// @param state_data FlatBuffers data to use for configuration
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneManagerState(SceneManagerState &state,
                             const SceneManagerStateFbs *state_data) const;

  /////////////////////////////////////////////////
  /// @brief Implementation of LoadSceneManagerState
  /////////////////////////////////////////////////
  std::expected<SceneManagerData, FailInfo>
  ProvideSceneManagerData() const override;
};
} // namespace steamrot
