/////////////////////////////////////////////////
/// @file
/// @brief Interface for loading scene manager data.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "SceneManagerData.h"
#include <expected>

namespace steamrot {

class ISceneManagerDataProvider {

private:
public:
  /////////////////////////////////////////////////
  /// @brief Virtual destructor for proper cleanup
  /////////////////////////////////////////////////
  virtual ~ISceneManagerDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Load the scene manager state from file.
  /////////////////////////////////////////////////
  virtual std::expected<SceneManagerData, FailInfo>
  ProvideSceneManagerData() const = 0;

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
  virtual std::expected<SceneManagerData, FailInfo>
  ConvertSceneManagerData(
      const SceneManagerDataFbs *fb_scene_manager_data) const = 0;
};
} // namespace steamrot
