/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersSaveDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSaveDataProvider.h"
#include "paths.h"
#include <format>

namespace steamrot {

/////////////////////////////////////////////////
std::filesystem::path FlatbuffersSaveDataProvider::GetSavesDirectory() const {
  // For now, use the data directory. In a full implementation,
  // this would use a user-specific directory (e.g., ~/.local/share/steamrot/)
  return paths::GetDataDirectory() / "saves";
}

/////////////////////////////////////////////////
std::filesystem::path
FlatbuffersSaveDataProvider::GetSavePath(uint32_t slot_index) const {
  std::string filename = std::format("slot_{}.save.bin", slot_index);
  return GetSavesDirectory() / filename;
}

/////////////////////////////////////////////////
std::expected<std::vector<SaveSlotInfo>, FailInfo>
FlatbuffersSaveDataProvider::GetSaveSlots() const {
  std::vector<SaveSlotInfo> slots;
  slots.reserve(kMaxSaveSlots);

  for (uint32_t i = 0; i < kMaxSaveSlots; ++i) {
    SaveSlotInfo slot_info;
    slot_info.slot_index = i;
    slot_info.is_populated = HasSaveData(i);

    if (slot_info.is_populated) {
      // TODO: Load metadata from save file
      // For now, use placeholder values
      slot_info.save_name = std::format("Save Slot {}", i + 1);
      slot_info.created_at = "";
      slot_info.last_modified = "";
      slot_info.play_time_seconds = 0;
    } else {
      slot_info.save_name = std::format("Empty Slot {}", i + 1);
    }

    slots.push_back(std::move(slot_info));
  }

  return slots;
}

/////////////////////////////////////////////////
std::expected<SaveData, FailInfo>
FlatbuffersSaveDataProvider::LoadSave(uint32_t slot_index) const {
  if (slot_index >= kMaxSaveSlots) {
    return std::unexpected(
        FailInfo(FailMode::InvalidInput,
                 std::format("Invalid save slot index: {}", slot_index)));
  }

  if (!HasSaveData(slot_index)) {
    return std::unexpected(
        FailInfo(FailMode::FileNotFound,
                 std::format("No save data in slot {}", slot_index)));
  }

  // TODO: Implement loading from FlatBuffers binary file
  // For now, return a stub SaveData
  SaveData save_data;
  save_data.metadata.slot_index = slot_index;
  save_data.metadata.save_name = std::format("Save Slot {}", slot_index + 1);
  save_data.current_scene_type = SceneType::SceneType_TITLE;
  save_data.version = 1;

  return save_data;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersSaveDataProvider::SaveGame(const SaveData &save_data) {
  if (save_data.metadata.slot_index >= kMaxSaveSlots) {
    return std::unexpected(
        FailInfo(FailMode::InvalidInput,
                 std::format("Invalid save slot index: {}",
                             save_data.metadata.slot_index)));
  }

  // TODO: Implement saving to FlatBuffers binary file
  // For now, this is a stub implementation
  // In a full implementation:
  // 1. Create the saves directory if it doesn't exist
  // 2. Build a FlatBuffers SaveData object
  // 3. Write to binary file

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersSaveDataProvider::DeleteSave(uint32_t slot_index) {
  if (slot_index >= kMaxSaveSlots) {
    return std::unexpected(
        FailInfo(FailMode::InvalidInput,
                 std::format("Invalid save slot index: {}", slot_index)));
  }

  std::filesystem::path save_path = GetSavePath(slot_index);

  if (!std::filesystem::exists(save_path)) {
    // Already deleted, not an error
    return std::monostate{};
  }

  std::error_code ec;
  std::filesystem::remove(save_path, ec);

  if (ec) {
    return std::unexpected(
        FailInfo(FailMode::FileOperationFailed,
                 std::format("Failed to delete save file: {}", ec.message())));
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
bool FlatbuffersSaveDataProvider::HasSaveData(uint32_t slot_index) const {
  if (slot_index >= kMaxSaveSlots) {
    return false;
  }

  return std::filesystem::exists(GetSavePath(slot_index));
}

/////////////////////////////////////////////////
uint32_t FlatbuffersSaveDataProvider::GetMaxSaveSlots() const {
  return kMaxSaveSlots;
}

} // namespace steamrot
