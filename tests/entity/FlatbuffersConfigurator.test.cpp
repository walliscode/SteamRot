/////////////////////////////////////////////////
/// @file
/// @brief unit tests for FlatbuffersConfigurator
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "FlatbuffersConfigurator.h"
#include "CUserInterface.h"
#include "EntityManager.h"
#include "emp_helpers.h"
#include <catch2/catch_test_macros.hpp>

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
  REQUIRE(result.has_value() == true);
}

TEST_CASE("Data is configured correctly", "[FlatbuffersConfigurator]") {
  // create EntityManager with a big memory pool
  steamrot::EntityManager entity_manager{1000};
  // create configurator with test environment
  steamrot::FlatbuffersConfigurator configurator{
      steamrot::EnvironmentType::Test};
  auto result = configurator.ConfigureEntitiesFromDefaultData(
      entity_manager.GetEntityMemoryPool(),
      steamrot::SceneType::SceneType_TEST);
  REQUIRE(result.has_value() == true);
  // Check if entities are configured correctly
  steamrot::EntityMemoryPool &entity_memory_pool =
      entity_manager.GetEntityMemoryPool();

  // match to test.scenes.json
  REQUIRE(steamrot::emp_helpers::GetComponent<steamrot::CUserInterface>(
              0, entity_memory_pool)
              .m_active);
}
