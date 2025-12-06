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

  const auto *fb_data = fb_result.value();

  // Convert FlatBuffers type to native struct
  SceneData native_data;
  native_data.scene_type = fb_data->scene_type();
  if (fb_data->scene_id()) {
    native_data.scene_id = fb_data->scene_id()->str();
  }

  // Extract render texture dimensions directly from scene data
  native_data.render_texture_width = fb_data->render_texture_width();
  native_data.render_texture_height = fb_data->render_texture_height();

  return native_data;
}

} // namespace steamrot
