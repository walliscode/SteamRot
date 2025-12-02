/////////////////////////////////////////////////
/// @file
/// @brief File-based implementation of save data provider.
///
/// FlatbuffersSaveDataProvider loads and saves gameplay data
/// using FlatBuffers binary files. It supports:
/// - Multiple save slots
/// - Save metadata for UI display
/// - Loading and saving game state
///
/// ## File Locations
/// - Save directory: {user_data_dir}/saves/
/// - Save files: slot_{index}.save.bin
///
/// ## Usage
/// ```cpp
/// FlatbuffersSaveDataProvider provider;
/// auto slots = provider.GetSaveSlots();
/// // Show save slots in UI...
/// auto save = provider.LoadSave(0);
/// ```
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ISaveDataProvider.h"
#include <filesystem>

namespace steamrot {

/////////////////////////////////////////////////
/// @class FlatbuffersSaveDataProvider
/// @brief FlatBuffers file-based save data provider.
///
/// This implementation:
/// - Manages save files in a dedicated saves directory
/// - Supports multiple save slots
/// - Provides metadata for save slot UI
/////////////////////////////////////////////////
class FlatbuffersSaveDataProvider : public ISaveDataProvider {
private:
  /////////////////////////////////////////////////
  /// @brief Maximum number of save slots
  /////////////////////////////////////////////////
  static constexpr uint32_t kMaxSaveSlots = 10;

  /////////////////////////////////////////////////
  /// @brief Get the path to saves directory.
  ///
  /// @return Path to saves directory
  /////////////////////////////////////////////////
  std::filesystem::path GetSavesDirectory() const;

  /////////////////////////////////////////////////
  /// @brief Get the path to a specific save file.
  ///
  /// @param slot_index The save slot index
  /// @return Path to the save file
  /////////////////////////////////////////////////
  std::filesystem::path GetSavePath(uint32_t slot_index) const;

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor.
  /////////////////////////////////////////////////
  FlatbuffersSaveDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Get information about all save slots.
  ///
  /// @return Vector of save slot information
  /////////////////////////////////////////////////
  std::expected<std::vector<SaveSlotInfo>, FailInfo>
  GetSaveSlots() const override;

  /////////////////////////////////////////////////
  /// @brief Load save data from a specific slot.
  ///
  /// @param slot_index The save slot to load from
  /// @return Save data or failure information
  /////////////////////////////////////////////////
  std::expected<SaveData, FailInfo> LoadSave(uint32_t slot_index) const override;

  /////////////////////////////////////////////////
  /// @brief Save game data to a specific slot.
  ///
  /// @param save_data The save data to persist
  /// @return Success or failure information
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  SaveGame(const SaveData &save_data) override;

  /////////////////////////////////////////////////
  /// @brief Delete save data from a specific slot.
  ///
  /// @param slot_index The save slot to delete
  /// @return Success or failure information
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> DeleteSave(uint32_t slot_index) override;

  /////////////////////////////////////////////////
  /// @brief Check if a specific save slot has data.
  ///
  /// @param slot_index The save slot to check
  /// @return true if the slot has save data
  /////////////////////////////////////////////////
  bool HasSaveData(uint32_t slot_index) const override;

  /////////////////////////////////////////////////
  /// @brief Get the maximum number of save slots.
  ///
  /// @return Maximum number of save slots supported
  /////////////////////////////////////////////////
  uint32_t GetMaxSaveSlots() const override;
};

} // namespace steamrot
