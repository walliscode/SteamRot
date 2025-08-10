/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the EntityManager class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EntityManager.h"
#include "PathProvider.h"
#include "catch2/generators/catch_generators.hpp"
#include "configuration_helpers.h"
#include "emp_helpers.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("EntityManager creates various pool sizes", "[EntityManager]") {
  SECTION("Default pool size") {
    steamrot::EntityManager entity_manager{};
    REQUIRE(steamrot::emp_helpers::GetMemoryPoolSize(
                entity_manager.GetEntityMemoryPool()) == 100);
  }
  SECTION("Custom pool size") {
    const size_t custom_size = GENERATE(10, 26, 39, 100, 750);
    steamrot::EntityManager entity_manager{custom_size};
    REQUIRE(steamrot::emp_helpers::GetMemoryPoolSize(
                entity_manager.GetEntityMemoryPool()) == custom_size);
  }
}

TEST_CASE("EntityManager calls configurator with no errors",
          "[EntityManager]") {

  steamrot::EntityManager entity_manager;

  // test the EntityMemoryPool pre configuration
  steamrot::TestEMPIsDefaultConstructed(entity_manager.GetEntityMemoryPool());

  // configure entities from default data
  auto result = entity_manager.ConfigureEntitiesFromDefaultData(
      steamrot::SceneType::SceneType_TEST, steamrot::EnvironmentType::Test,
      steamrot::DataType::Flatbuffers);

  REQUIRE(result.has_value() == true);

  // test the EntityMemoryPool post configuration
  steamrot::TestConfigurationOfEMPfromDefaultData(
      entity_manager.GetEntityMemoryPool(),
      steamrot::SceneType::SceneType_TEST);
}
