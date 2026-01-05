/////////////////////////////////////////////////
/// @file
/// @brief Functions to load test scene collection data from binary file
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "scene_collection_data_generated.h"
#include <filesystem>
#include <fstream>
#include <memory>
#include <utility>

/////////////////////////////////////////////////
/// @brief Load scene collection test data from binary file
///
/// @return Pair of unique_ptr to data buffer and SceneCollectionDataFbs pointer
/////////////////////////////////////////////////
inline std::pair<std::unique_ptr<char[]>,
                 const steamrot::SceneCollectionDataFbs *>
LoadSceneCollectionTestData() {
  std::filesystem::path test_file_path = __FILE__;
  std::filesystem::path data_dir = test_file_path.parent_path() / "data";
  std::filesystem::path bin_file_path =
      data_dir / "three_scenes.scene_collection_data.bin";

  std::ifstream infile(bin_file_path, std::ios::binary | std::ios::in);
  if (!infile.is_open()) {
    throw std::runtime_error("Failed to open file: " + bin_file_path.string());
  }

  infile.seekg(0, std::ios::end);
  auto length = infile.tellg();
  if (length <= 0) {
    throw std::runtime_error("Empty or invalid file: " +
                             bin_file_path.string());
  }

  infile.seekg(0, std::ios::beg);
  auto data = std::make_unique<char[]>(static_cast<size_t>(length));
  infile.read(data.get(), length);
  infile.close();

  const steamrot::SceneCollectionDataFbs *scene_collection_data_fbs =
      steamrot::GetSceneCollectionDataFbs(data.get());

  return {std::move(data), scene_collection_data_fbs};
}
