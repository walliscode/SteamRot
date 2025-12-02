/////////////////////////////////////////////////
/// @file
/// @brief Interface for save data provider.
///
/// ISaveDataProvider defines the contract for loading and saving
/// gameplay data (save files). Implementations handle the actual
/// storage mechanism.
///
/// ## Design Rationale
///
/// Save data is separate from:
/// - User preferences (settings like volume, display options)
/// - Engine configuration (static defaults)
/// - Default scene data (initial game state)
///
/// This allows:
/// - New games to start from default scene data
/// - Saved games to restore from save files
/// - Multiple save slots
/// - Save file metadata for management
///
/// ## Usage
/// ```cpp
/// ISaveDataProvider& provider = GetSaveProvider();
///
/// // List available saves
/// auto slots = provider.GetSaveSlots();
///
/// // Load a save
/// auto save_result = provider.LoadSave(0);
/// if (save_result.has_value()) {
///   // Restore game state from save
/// }
///
/// // Create a new save
/// SaveData save = CaptureSaveData();
/// provider.SaveGame(save);
/// ```
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "scene_types_generated.h"
#include <expected>
#include <string>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
/// @struct SaveSlotInfo
/// @brief Metadata about a save slot for UI display.
///
/// Lightweight structure for listing save slots without loading
/// the full save data.
/////////////////////////////////////////////////
struct SaveSlotInfo {
  /////////////////////////////////////////////////
  /// @brief Save slot index (0-based)
  /////////////////////////////////////////////////
  uint32_t slot_index{0};

  /////////////////////////////////////////////////
  /// @brief Display name for the save
  /////////////////////////////////////////////////
  std::string save_name;

  /////////////////////////////////////////////////
  /// @brief Timestamp when save was created
  /////////////////////////////////////////////////
  std::string created_at;

  /////////////////////////////////////////////////
  /// @brief Timestamp when save was last modified
  /////////////////////////////////////////////////
  std::string last_modified;

  /////////////////////////////////////////////////
  /// @brief Total play time in seconds
  /////////////////////////////////////////////////
  uint64_t play_time_seconds{0};

  /////////////////////////////////////////////////
  /// @brief Whether this slot has save data
  /////////////////////////////////////////////////
  bool is_populated{false};
};

/////////////////////////////////////////////////
/// @struct SaveData
/// @brief Runtime representation of save data.
///
/// This is the C++ representation of SaveData from FlatBuffers.
/// It provides a mutable structure for capturing and restoring
/// game state.
/////////////////////////////////////////////////
struct SaveData {
  /////////////////////////////////////////////////
  /// @struct Metadata
  /// @brief Save file metadata
  /////////////////////////////////////////////////
  struct Metadata {
    std::string save_name;
    std::string created_at;
    std::string last_modified;
    std::string game_version;
    uint64_t play_time_seconds{0};
    uint32_t slot_index{0};
  } metadata;

  /////////////////////////////////////////////////
  /// @brief The scene type the player was in when saving
  /////////////////////////////////////////////////
  SceneType current_scene_type{SceneType::SceneType_UNKNOWN};

  /////////////////////////////////////////////////
  /// @brief Version number for migration support
  /////////////////////////////////////////////////
  uint32_t version{1};

  // Note: Scene state data will be added when entity serialization is implemented
  // For now, this interface provides the structure for future implementation
};

/////////////////////////////////////////////////
/// @class ISaveDataProvider
/// @brief Interface for loading and saving gameplay data.
///
/// Implementations handle the actual storage mechanism (files, cloud, etc.)
/// The interface supports:
/// - Listing available save slots
/// - Loading save data from a slot
/// - Saving game data to a slot
/// - Deleting save data
/////////////////////////////////////////////////
class ISaveDataProvider {
public:
  /////////////////////////////////////////////////
  /// @brief Virtual destructor for proper cleanup.
  /////////////////////////////////////////////////
  virtual ~ISaveDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Get information about all save slots.
  ///
  /// Returns metadata for all save slots, including empty slots.
  /// This is useful for save/load UI screens.
  ///
  /// @return Vector of save slot information
  /////////////////////////////////////////////////
  virtual std::expected<std::vector<SaveSlotInfo>, FailInfo>
  GetSaveSlots() const = 0;

  /////////////////////////////////////////////////
  /// @brief Load save data from a specific slot.
  ///
  /// @param slot_index The save slot to load from (0-based)
  /// @return Save data or failure information
  /////////////////////////////////////////////////
  virtual std::expected<SaveData, FailInfo>
  LoadSave(uint32_t slot_index) const = 0;

  /////////////////////////////////////////////////
  /// @brief Save game data to a specific slot.
  ///
  /// @param save_data The save data to persist
  /// @return Success or failure information
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  SaveGame(const SaveData &save_data) = 0;

  /////////////////////////////////////////////////
  /// @brief Delete save data from a specific slot.
  ///
  /// @param slot_index The save slot to delete (0-based)
  /// @return Success or failure information
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  DeleteSave(uint32_t slot_index) = 0;

  /////////////////////////////////////////////////
  /// @brief Check if a specific save slot has data.
  ///
  /// @param slot_index The save slot to check (0-based)
  /// @return true if the slot has save data, false otherwise
  /////////////////////////////////////////////////
  virtual bool HasSaveData(uint32_t slot_index) const = 0;

  /////////////////////////////////////////////////
  /// @brief Get the maximum number of save slots.
  ///
  /// @return Maximum number of save slots supported
  /////////////////////////////////////////////////
  virtual uint32_t GetMaxSaveSlots() const = 0;
};

} // namespace steamrot
