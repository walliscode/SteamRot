/////////////////////////////////////////////////
/// @file
/// @brief functions to provide test data for entity unit tests
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EntityTransportVariant.h"
#include "entities_generated.h"
#include <filesystem>
#include <fstream>
#include <memory>
#include <utility>
/////////////////////////////////////////////////
/// @brief Load entity test data from binary file
///
/// @return Pair of unique_ptr to data buffer and EntityTransportVariant
/////////////////////////////////////////////////
inline std::pair<std::unique_ptr<char[]>, steamrot::EntityTransportVariant>
LoadEntityTestData() {
  std::filesystem::path test_file_path = __FILE__;
  std::filesystem::path data_dir = test_file_path.parent_path() / "data";
  std::filesystem::path bin_file_path = data_dir / "entity_test_data.bin";

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

  const steamrot::EntityCollectionFbs *entity_collection =
      steamrot::GetEntityCollectionFbs(data.get());

  // Wrap the pointer in EntityTransportVariant
  steamrot::EntityTransportVariant entity_transport = entity_collection;

  return {std::move(data), entity_transport};
}
