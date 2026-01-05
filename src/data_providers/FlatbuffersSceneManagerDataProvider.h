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
  /// @brief Implementation of ProvideSceneManagerData - loads from file
  /////////////////////////////////////////////////
  std::expected<SceneManagerData, FailInfo>
  ProvideSceneManagerData() const override;

  /////////////////////////////////////////////////
  /// @brief Convert FlatBuffers SceneManagerDataFbs to native
  /// SceneManagerData.
  ///
  /// This method allows passing pre-loaded FlatBuffers data directly
  /// for conversion without file I/O.
  ///
  /// @param fb_scene_manager_data Pointer to FlatBuffers SceneManagerDataFbs
  /// @return Native SceneManagerData object, or error
  /////////////////////////////////////////////////
  std::expected<SceneManagerData, FailInfo>
  ConvertSceneManagerData(
      const SceneManagerDataFbs *fb_scene_manager_data) const override;
};
} // namespace steamrot
