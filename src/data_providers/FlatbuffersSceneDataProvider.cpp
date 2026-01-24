/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersSceneDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneDataProvider.h"
#include "FlatbuffersDataLoader.h"
#include "FlatbuffersEntityImporter.h"
#include "SceneData.h"
#include "scene_data_generated.h"
#include <expected>
#include <variant>

namespace steamrot {
/////////////////////////////////////////////////
FlatbuffersSceneDataProvider::FlatbuffersSceneDataProvider(
    EventHandler &event_handler)
    : m_event_handler(event_handler) {}

/////////////////////////////////////////////////
std::expected<SceneData, FailInfo>
FlatbuffersSceneDataProvider::ProvideDefaultSceneData(
    const SceneType scene_type) const {

  // Load SceneData based on scene type
  FlatbuffersDataLoader data_loader;
  auto load_data_result = data_loader.ProvideDefaultSceneData(scene_type);
  if (!load_data_result.has_value()) {
    return std::unexpected(load_data_result.error());
  }
  const SceneDataFbs &fb_data = *load_data_result.value();

  // Pass to ProvideSceneDataFromData to create SceneData
  return ProvideSceneDataFromData(&fb_data);
}
/////////////////////////////////////////////////
std::expected<SceneData, FailInfo>
FlatbuffersSceneDataProvider::ProvideSceneDataFromData(
    const SceneDataFbs *fb_data) const {

  // create SceneData to return
  SceneData scene_data;

  // configure SceneData
  auto configure_data_result = ConfigureSceneDataFromData(scene_data, fb_data);
  if (!configure_data_result)
    return std::unexpected(configure_data_result.error());

  // Create EntityImporter with FlatBuffers entity collection
  auto entity_importer = std::make_unique<FlatbuffersEntityImporter>(
      m_event_handler, *fb_data->entity_collection());
  if (!entity_importer) {
    return std::unexpected(FailInfo{
        FailMode::NullPointer, "Failed to create FlatbuffersEntityImporter"});
  }
  scene_data.entity_transport = std::move(entity_importer);

  // return the SceneData
  return scene_data;
}

} // namespace steamrot
