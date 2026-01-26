/////////////////////////////////////////////////
/// @file
/// @brief Helper function to load engine snapshot test data
/////////////////////////////////////////////////

#pragma once

#include "engine_snapshot_generated.h"
#include <filesystem>
#include <fstream>
#include <memory>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Load EngineSnapshot test data from binary file
///
/// Loads the compiled engine_snapshot_test_data.bin file and returns
/// a pair containing the raw data buffer and a pointer to the
/// FlatBuffers EngineSnapshotFbs object.
///
/// @return Pair of unique_ptr to data buffer and const pointer to
///         EngineSnapshotFbs
/////////////////////////////////////////////////
inline std::pair<std::unique_ptr<char[]>, const steamrot::EngineSnapshotFbs *>
LoadEngineSnapshotTestData() {
  // Get path to this file
  std::filesystem::path test_file_path = __FILE__;

  // Navigate to the binary file in the data/ subdirectory
  std::filesystem::path bin_file_path =
      test_file_path.parent_path() / "data" / "engine_snapshot_test_data.bin";

  // Open file
  std::ifstream infile(bin_file_path, std::ios::binary | std::ios::ate);
  if (!infile) {
    throw std::runtime_error("Failed to open engine_snapshot_test_data.bin");
  }

  // Get file size
  std::streamsize length = infile.tellg();
  infile.seekg(0, std::ios::beg);

  // Read file into buffer
  auto data = std::make_unique<char[]>(static_cast<size_t>(length));
  infile.read(data.get(), length);

  // Get FlatBuffers root
  const steamrot::EngineSnapshotFbs *engine_snapshot_fbs =
      steamrot::GetEngineSnapshotFbs(data.get());

  return {std::move(data), engine_snapshot_fbs};
}

} // namespace steamrot::tests
