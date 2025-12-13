/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersEntityConfigurator
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersEntityConfigurator.h"
#include "EventHandler.h"
#include "containers.h"
#include "entities_generated.h"
#include "entity_memory.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <memory>

/////////////////////////////////////////////////
/// Constructor Tests
/////////////////////////////////////////////////

TEST_CASE("FlatbuffersEntityConfigurator::Constructor",
          "[unit][FlatbuffersEntityConfigurator]") {

  // set up resources
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;

  // Load entity test data
  // Get the path to the data directory adjacent to this test file
  std::filesystem::path test_file_path = __FILE__;
  std::filesystem::path data_dir = test_file_path.parent_path() / "data";
  std::filesystem::path bin_file_path = data_dir / "entity_test_data.bin";

  // Open and read the binary file
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

  // Get the root and verify
  const steamrot::EntityCollectionFbs *entity_collection =
      steamrot::GetEntityCollectionFbs(data.get());

  // pre tests
  REQUIRE(steamrot::entity::memory::GetMemoryPoolSize(emp) == 0);

  // pointer is not null
  REQUIRE(entity_collection != nullptr);
  REQUIRE(entity_collection->entity_memory_pool_size() == 100);

  // set up configurator
  steamrot::FlatbuffersEntityConfigurator configurator(event_handler,
                                                       *entity_collection);

  SECTION("EntityMemoryPool is resized correctly") {

    auto config_result = configurator.ConfigureEntityMemoryPool(emp);
    if (!config_result.has_value()) {
      FAIL("ConfigureEntityMemoryPool failed: " +
           config_result.error().message);
    }

    REQUIRE(steamrot::entity::memory::GetMemoryPoolSize(emp) == 100);
  }
}
