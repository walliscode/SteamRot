/////////////////////////////////////////////////
/// @file
/// @brief Declaration of FlatbuffersSaveDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ISaveDataProvider.h"
#include "SaveData.h"
#include "SaveMetadata.h"
#include <cstdint>
#include <expected>

namespace steamrot {

/////////////////////////////////////////////////
/// @class FlatbuffersSaveDataProvider
/// @brief Concrete implementation of ISaveDataProvider using FlatBuffers.
///
/// FlatbuffersSaveDataProvider loads save data from FlatBuffers binary
/// files stored in the saves/ directory. Save files are named:
/// - save_slot_0.save
/// - save_slot_1.save
/// - etc.
///
/// Implementation details:
/// - Uses FlatbuffersDataLoader for file I/O
/// - Converts FlatBuffers data to native SaveData struct
/// - Handles missing save files gracefully
/// - Validates FlatBuffers data before conversion
///
/// File locations:
/// - Production: data/saves/
/// - Test: tests/data/saves/
/////////////////////////////////////////////////
class FlatbuffersSaveDataProvider : public ISaveDataProvider {

public:
  /////////////////////////////////////////////////
  /// @brief Load complete save data from a save slot.
  ///
  /// Loads the save file for the specified slot and converts
  /// the FlatBuffers data to a native SaveData struct.
  ///
  /// @param slot_index The save slot to load (0-based)
  /// @return SaveData struct or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<SaveData, FailInfo>
  LoadSaveData(uint32_t slot_index) const override;

  /////////////////////////////////////////////////
  /// @brief Check if a save slot contains data.
  ///
  /// Checks if a save file exists for the given slot by
  /// attempting to locate the file in the saves/ directory.
  ///
  /// @param slot_index The save slot to check (0-based)
  /// @return true if save file exists, false otherwise
  /////////////////////////////////////////////////
  bool SaveExists(uint32_t slot_index) const override;

  /////////////////////////////////////////////////
  /// @brief Get metadata for a save slot.
  ///
  /// Loads only the metadata portion of the save file.
  /// This is implemented by loading the full FlatBuffers data
  /// but only extracting the metadata table.
  ///
  /// @param slot_index The save slot to query (0-based)
  /// @return SaveMetadata or FailInfo if save doesn't exist
  /////////////////////////////////////////////////
  std::expected<SaveMetadata, FailInfo>
  GetSaveMetadata(uint32_t slot_index) const override;
};

} // namespace steamrot
