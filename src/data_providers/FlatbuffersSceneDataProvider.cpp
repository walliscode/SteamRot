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
#include "SaveData.h"
#include "scene_data_generated.h"
#include <expected>

namespace steamrot {

/////////////////////////////////////////////////
std::expected<std::unique_ptr<SceneData>, FailInfo>
FlatbuffersSceneDataProvider::ProvideDefaultSceneData(
    const SceneType scene_type) const {

  // Load SceneData based on scene type
  FlatbuffersDataLoader data_loader;
  auto load_data_result = data_loader.ProvideDefaultSceneData(scene_type);
  if (!load_data_result.has_value()) {
    return std::unexpected(load_data_result.error());
  }
  const SceneDataFbs &fb_data = *load_data_result.value();

  // create SceneData
  FbsSceneData scene_data;

  // fill out SceneData from FlatBuffers data

  // no UUID needed for default scene data
  scene_data.scene_info.type = scene_type;
  scene_data.scene_data_fbs = load_data_result.value();

  return std::make_unique<FbsSceneData>(scene_data);
}

/////////////////////////////////////////////////
std::expected<std::unique_ptr<SceneData>, FailInfo>
FlatbuffersSceneDataProvider::ProvideSceneDataFromSave(
    SaveData &save_data) const {

  // Validate that save_data contains scene_data
  if (!save_data.scene_data) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer, "SaveData contains null scene_data"});
  }

  // Move the scene_data out of SaveData and return it
  // This transfers ownership to the caller
  return std::move(save_data.scene_data);
}

} // namespace steamrot
