/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersSaveDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSaveDataProvider.h"
#include "DataLoader.h"
#include "FbsSceneData.h"
#include "paths.h"
#include "save_data_generated.h"
#include <filesystem>
#include <format>

namespace steamrot {

/////////////////////////////////////////////////
std::expected<SaveData, FailInfo>
FlatbuffersSaveDataProvider::LoadSaveData(uint32_t slot_index) const {

  // Construct file path: data/user/saves/save_slot_N.save
  std::filesystem::path saves_dir = paths::GetSavesDirectory();
  std::filesystem::path save_path =
      saves_dir / std::format("save_slot_{}.save", slot_index);

  // Check if file exists
  if (!std::filesystem::exists(save_path)) {
    std::string error_message =
        std::format("Save file not found: {}", save_path.string());
    return std::unexpected(FailInfo{FailMode::FileNotFound, error_message});
  }

  // Load binary data
  DataLoader loader;
  const auto *buffer_data = loader.LoadBinaryData(save_path);

  // Parse FlatBuffers SaveData
  const SaveDataFbs *fb_save_data = GetSaveData(buffer_data);
  if (!fb_save_data) {
    std::string error_message = std::format(
        "Failed to parse FlatBuffers SaveData from: {}", save_path.string());
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound, error_message});
  }

  // Convert to native SaveData struct
  SaveData save_data;

  // Extract metadata
  if (fb_save_data->metadata()) {
    const SaveMetadataFbs *fb_metadata = fb_save_data->metadata();

    if (fb_metadata->save_name()) {
      save_data.metadata.save_name = fb_metadata->save_name()->str();
    }
    if (fb_metadata->created_at()) {
      save_data.metadata.created_at = fb_metadata->created_at()->str();
    }
    if (fb_metadata->last_modified()) {
      save_data.metadata.last_modified = fb_metadata->last_modified()->str();
    }
    if (fb_metadata->game_version()) {
      save_data.metadata.game_version = fb_metadata->game_version()->str();
    }
    save_data.metadata.play_time_seconds = fb_metadata->play_time_seconds();
    save_data.metadata.slot_index = fb_metadata->slot_index();
  }

  // Extract scene data
  if (fb_save_data->scene_data()) {
    FbsSceneData fbs_scene_data;
    fbs_scene_data.scene_data_fbs = fb_save_data->scene_data();

    // Extract scene type from scene_data if available
    if (fb_save_data->scene_data()->scene_type()) {
      fbs_scene_data.scene_info.type = fb_save_data->scene_data()->scene_type();
    }

    save_data.scene_data = std::make_unique<FbsSceneData>(fbs_scene_data);
  }

  // Extract engine state
  if (fb_save_data->engine_state()) {
    const EngineStateFbs *fb_engine_state = fb_save_data->engine_state();

    save_data.engine_state.running = fb_engine_state->running();
    save_data.engine_state.paused = fb_engine_state->paused();
    save_data.engine_state.quit_requested = fb_engine_state->quit_requested();

    // Note: subscriptions would need conversion from SubscriberFbs
    // This is left for future implementation when subscriber
    // serialization is needed
  }

  // Extract scene manager state
  if (fb_save_data->scene_manager_state()) {
    // Note: subscriptions would need conversion from SubscriberFbs
    // This is left for future implementation
  }

  // Extract scene state
  if (fb_save_data->scene_state()) {
    const SceneStateFbs *fb_scene_state = fb_save_data->scene_state();
    save_data.scene_state.active = fb_scene_state->active();
  }

  return save_data;
}

/////////////////////////////////////////////////
bool FlatbuffersSaveDataProvider::SaveExists(uint32_t slot_index) const {

  std::filesystem::path saves_dir = paths::GetSavesDirectory();
  std::filesystem::path save_path =
      saves_dir / std::format("save_slot_{}.save", slot_index);

  return std::filesystem::exists(save_path);
}

/////////////////////////////////////////////////
std::expected<SaveMetadata, FailInfo>
FlatbuffersSaveDataProvider::GetSaveMetadata(uint32_t slot_index) const {

  // For FlatBuffers implementation, we need to load the full file
  // to access the metadata table. A future optimization could use
  // file offsets to read only the metadata portion.

  auto save_result = LoadSaveData(slot_index);
  if (!save_result.has_value()) {
    return std::unexpected(save_result.error());
  }

  return save_result.value().metadata;
}

} // namespace steamrot
