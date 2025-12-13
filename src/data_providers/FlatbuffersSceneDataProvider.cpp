/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersSceneDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneDataProvider.h"
#include "FbsSceneData.h"
#include "FlatbuffersDataLoader.h"
#include "scene_data_generated.h"

namespace steamrot {

/////////////////////////////////////////////////
std::unique_ptr<SceneData>
FlatbuffersSceneDataProvider::ProvideDefaultSceneData(
    const SceneType scene_type) const {

  // Load SceneData based on scene type
  FlatbuffersDataLoader data_loader;
  auto load_data_result = data_loader.ProvideDefaultSceneData(scene_type);
  if (!load_data_result.has_value()) {
  }
  const SceneDataFbs &fb_data = *load_data_result.value();

  // create SceneData
  FbsSceneData scene_data;

  // fill out SceneData from FlatBuffers data

  // no UUID needed for default scene data
  scene_data.scene_info.type = scene_type;
  scene_data.scene_data_fbs = load_data_result.value();

  return std::make_unique<SceneData>(scene_data);
}
} // namespace steamrot
