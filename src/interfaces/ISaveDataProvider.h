/////////////////////////////////////////////////
/// @file
/// @brief Interface for loading save data.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "SaveData.h"
#include "SaveMetadata.h"
#include <cstdint>
#include <expected>

namespace steamrot {

/////////////////////////////////////////////////
/// @class ISaveDataProvider
/// @brief Interface for loading game save data.
///
/// ISaveDataProvider defines the contract for loading saved game state.
/// Implementations handle the actual data source (files, cloud, etc.)
/// and format (FlatBuffers, JSON, binary, etc.).
///
/// The interface provides three main operations:
/// 1. LoadSaveData - Load complete save data for a slot
/// 2. SaveExists - Check if a save slot contains data
/// 3. GetSaveMetadata - Load only metadata (for save selection UI)
///
/// Usage:
/// ```cpp
/// ISaveDataProvider& provider = GetSaveDataProvider();
///
/// // Check if save exists
/// if (provider.SaveExists(0)) {
///   // Load metadata for display
///   auto metadata = provider.GetSaveMetadata(0);
///   if (metadata.has_value()) {
///     std::cout << "Save: " << metadata.value().save_name << std::endl;
///   }
///
///   // Load full save data
///   auto save_data = provider.LoadSaveData(0);
///   if (save_data.has_value()) {
///     // Configure game from save
///     ConfigureGameFromSave(save_data.value());
///   }
/// }
/// ```
/////////////////////////////////////////////////
class ISaveDataProvider {
public:
  /////////////////////////////////////////////////
  /// @brief Virtual destructor for interface
  /////////////////////////////////////////////////
  virtual ~ISaveDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Load complete save data from a save slot.
  ///
  /// Loads all game state from the specified save slot, including:
  /// - Save metadata
  /// - Scene data (entity pool, components)
  /// - Engine state (running flags, subscriptions)
  /// - Scene manager state
  /// - Scene state
  ///
  /// @param slot_index The save slot to load (0-based)
  /// @return SaveData struct or FailInfo on error
  /////////////////////////////////////////////////
  virtual std::expected<SaveData, FailInfo>
  LoadSaveData(uint32_t slot_index) const = 0;

  /////////////////////////////////////////////////
  /// @brief Check if a save slot contains data.
  ///
  /// Fast check to determine if a save file exists for the given slot.
  /// Used for save slot UI to show which slots are occupied.
  ///
  /// @param slot_index The save slot to check (0-based)
  /// @return true if save exists, false otherwise
  /////////////////////////////////////////////////
  virtual bool SaveExists(uint32_t slot_index) const = 0;

  /////////////////////////////////////////////////
  /// @brief Get metadata for a save slot without loading full data.
  ///
  /// Loads only the metadata portion of a save file (display name,
  /// timestamps, play time) for save selection screens. This is faster
  /// than loading the complete save data.
  ///
  /// @param slot_index The save slot to query (0-based)
  /// @return SaveMetadata or FailInfo if save doesn't exist or can't be read
  /////////////////////////////////////////////////
  virtual std::expected<SaveMetadata, FailInfo>
  GetSaveMetadata(uint32_t slot_index) const = 0;
};

} // namespace steamrot
