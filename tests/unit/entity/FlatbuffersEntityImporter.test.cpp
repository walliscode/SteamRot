/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersEntityImporter class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersEntityImporter.h"
#include "FlatbuffersDataLoader.h"
#include "TestFixture.h"
#include "entity_memory.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FlatbuffersEntityImporter constructor",
          "[unit][FlatbuffersEntityImporter]") {

  steamrot::tests::TestFixture test_fixture;
  test_fixture.Initialize();

  // Load test scene data
  steamrot::FlatbuffersDataLoader loader;
  auto scene_data_result =
      loader.ProvideDefaultSceneData(steamrot::SceneType::TEST);
  REQUIRE(scene_data_result.has_value());

  const auto *entity_collection =
      scene_data_result.value()->entity_collection();
  REQUIRE(entity_collection != nullptr);

  // Create importer
  steamrot::FlatbuffersEntityImporter importer(
      test_fixture.GetGameContext().event_handler, *entity_collection);

  SUCCEED("FlatbuffersEntityImporter created successfully");
}

TEST_CASE("FlatbuffersEntityImporter imports entities",
          "[unit][FlatbuffersEntityImporter]") {

  steamrot::tests::TestFixture test_fixture;
  test_fixture.Initialize();

  // Load test scene data
  steamrot::FlatbuffersDataLoader loader;
  auto scene_data_result =
      loader.ProvideDefaultSceneData(steamrot::SceneType::TEST);
  REQUIRE(scene_data_result.has_value());

  const auto *entity_collection =
      scene_data_result.value()->entity_collection();
  REQUIRE(entity_collection != nullptr);

  // Create importer
  steamrot::FlatbuffersEntityImporter importer(
      test_fixture.GetGameContext().event_handler, *entity_collection);

  // Create empty EntityMemoryPool
  steamrot::EntityMemoryPool emp;
  REQUIRE(steamrot::entity::memory::GetMemoryPoolSize(emp) == 0);

  // Import entities
  auto result = importer.ImportEntities(emp);
  REQUIRE(result.has_value());

  // Verify entities were imported
  size_t pool_size = steamrot::entity::memory::GetMemoryPoolSize(emp);
  REQUIRE(pool_size > 0);
}
