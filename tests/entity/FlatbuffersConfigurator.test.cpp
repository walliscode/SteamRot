/////////////////////////////////////////////////
/// @file
/// @brief unit tests for FlatbuffersConfigurator
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "FlatbuffersConfigurator.h"
#include "EntityManager.h"
#include "configuration_helpers.h"
#include "containers.h"

#include "scene_types_generated.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("Fails if memory pool is not big enough",
          "[FlatbuffersConfigurator]") {

  // create EntityManager with a small memory pool
  steamrot::EntityManager entity_manager{0};

  // create configurator with test environment
  steamrot::FlatbuffersConfigurator configurator{
      steamrot::EnvironmentType::Test};

  auto result = configurator.ConfigureEntitiesFromDefaultData(
      entity_manager.GetEntityMemoryPool(),
      steamrot::SceneType::SceneType_TEST);

  REQUIRE(result.has_value() == false);
  REQUIRE(result.error().mode == steamrot::FailMode::ParameterOutOfBounds);

  // due to changing test data, we will not check the exact size needed
  std::string partial_message = "Entity memory pool size: 0, required size: ";
  REQUIRE(result.error().message.find(partial_message) != std::string::npos);
}

TEST_CASE("Succeeds if memory pool is big enough",
          "[FlatbuffersConfigurator]") {
  // create EntityManager with a big memory pool
  steamrot::EntityManager entity_manager{1000};
  // create configurator with test environment
  steamrot::FlatbuffersConfigurator configurator{
      steamrot::EnvironmentType::Test};
  auto result = configurator.ConfigureEntitiesFromDefaultData(
      entity_manager.GetEntityMemoryPool(),
      steamrot::SceneType::SceneType_TEST);
  if (!result.has_value()) {
    FAIL(result.error().message);
  }
}

TEST_CASE("Data is configured correctly from default data",
          "[FlatbuffersConfigurator]") {

  // Set up EntityMemoryPool objects here so they outlive GENERATE
  size_t pool_size{100};
  steamrot::EntityMemoryPool entity_memory_pool_one;
  std::apply(
      [pool_size](auto &...component_vector) {
        (component_vector.resize(pool_size), ...);
      },
      entity_memory_pool_one);

  /////////////////////////////////////////////////
  /// Default value testing
  /////////////////////////////////////////////////

  steamrot::tests::TestEMPIsDefaultConstructed(entity_memory_pool_one);

  // Create configurator with test environment
  steamrot::FlatbuffersConfigurator configurator{
      steamrot::EnvironmentType::Test};

  auto result = configurator.ConfigureEntitiesFromDefaultData(
      entity_memory_pool_one, steamrot::SceneType::SceneType_TEST);
  REQUIRE(result.has_value() == true);
  /////////////////////////////////////////////////
  /// Post configuration testing
  /////////////////////////////////////////////////

  steamrot::tests::TestConfigurationOfEMPfromDefaultData(
      entity_memory_pool_one, steamrot::SceneType_TEST);
}
