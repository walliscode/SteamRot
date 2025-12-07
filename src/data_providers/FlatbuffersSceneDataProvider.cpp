/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersSceneDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneDataProvider.h"
#include "scene_data_generated.h"

namespace steamrot {

/////////////////////////////////////////////////
std::expected<SceneData, FailInfo>
FlatbuffersSceneDataProvider::LoadSceneData(SceneType scene_type) const {
  // Use existing loader
  auto fb_result = m_loader.ProvideDefaultSceneData(scene_type);
  if (!fb_result.has_value()) {
    return std::unexpected(fb_result.error());
  }

  const SceneDataFbs *scene_data = fb_result.value();

  // Convert FlatBuffers type to native struct
  SceneData native_data;

  return native_data;
}

} // namespace steamrot
