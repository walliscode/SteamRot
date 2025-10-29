/////////////////////////////////////////////////
/// @file
/// @brief Example demonstrating TestFixture integration with harness
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "test_data_harness.h"
#include "CUserInterface.h"
#include "emp_helpers.h"
#include <catch2/catch_test_macros.hpp>

/////////////////////////////////////////////////
/// Example: Basic usage of run_test_with_fixture
/////////////////////////////////////////////////
TEST_CASE("Example: Basic TestFixture integration", "[unit][harness][example]") {

  // Run all tests from adjacent data directory with a simple simulation
  auto result = steamrot::tests::run_test_with_fixture(
      [](steamrot::tests::TestFixture &fixture) {
        // Access entity manager
        auto &entity_mgr = fixture.GetEntityManager();
        auto &pool = entity_mgr.GetEntityMemoryPool();

        // Verify we can access the pool
        REQUIRE(pool.GetSize() > 0);

        // Example: You could run logic simulations here
        // Example: You could modify entity states here
        // The final state will be compared with expected_entity_collection
      });

  REQUIRE(result.has_value());
}

/////////////////////////////////////////////////
/// Example: Modifying entities in simulation
/////////////////////////////////////////////////
TEST_CASE("Example: Modify entities during simulation", "[unit][harness][example]") {

  // Load test configs
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  REQUIRE(configs.value().size() >= 1);

  const auto *config = configs.value()[0];

  // Run test with entity modification
  auto result = steamrot::tests::run_single_test_with_fixture(
      config,
      [](steamrot::tests::TestFixture &fixture) {
        auto &entity_mgr = fixture.GetEntityManager();
        auto &pool = entity_mgr.GetEntityMemoryPool();

        // Example: Access and modify UI component
        if (pool.GetSize() > 0) {
          auto &ui_component = 
              steamrot::emp_helpers::GetComponent<steamrot::CUserInterface>(0, pool);
          
          // Verify component exists
          INFO("UI component active: " << ui_component.m_active);
        }
      });

  REQUIRE(result.has_value());
}

/////////////////////////////////////////////////
/// Example: Access game and scene resources
/////////////////////////////////////////////////
TEST_CASE("Example: Access fixture resources", "[unit][harness][example]") {

  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());

  const auto *config = configs.value()[0];

  auto result = steamrot::tests::run_single_test_with_fixture(
      config,
      [](steamrot::tests::TestFixture &fixture) {
        // Access different resource types
        auto &game_resources = fixture.GetGameResources();
        auto &scene_resources = fixture.GetSceneResources();
        auto &entity_mgr = fixture.GetEntityManager();

        // Access game context
        auto &game_context = fixture.GetGameContext();
        REQUIRE(&game_context.game_resources == &game_resources);

        // Access scene context
        auto &scene_context = fixture.GetSceneContext();
        REQUIRE(&scene_context.scene_resources == &scene_resources);
        REQUIRE(&scene_context.game_resources == &game_resources);

        // Example: You could use these contexts to:
        // - Trigger events via game_resources.event_handler
        // - Access assets via game_resources.asset_manager
        // - Check scene configuration
        // - Run logic with full context
      });

  REQUIRE(result.has_value());
}

/////////////////////////////////////////////////
/// Example: Manual fixture setup for complex scenarios
/////////////////////////////////////////////////
TEST_CASE("Example: Manual fixture setup", "[unit][harness][example]") {

  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());

  const auto *config = configs.value()[0];

  // Setup fixture manually for more control
  auto fixture_result = steamrot::tests::setup_fixture_from_test_data(config);
  REQUIRE(fixture_result.has_value());

  auto &fixture = fixture_result.value();

  // Now you have full control over the fixture
  auto &entity_mgr = fixture.GetEntityManager();
  auto &pool = entity_mgr.GetEntityMemoryPool();

  // Run multiple simulation steps
  // Step 1: Initial setup
  REQUIRE(pool.GetSize() > 0);

  // Step 2: Simulate some logic
  // ... your logic here ...

  // Step 3: Verify intermediate state
  // ... your verification here ...

  // Step 4: Run more logic
  // ... more simulation ...

  // Manual comparison if needed
  if (config->expected_entity_collection()) {
    steamrot::EntityMemoryPool expected_pool;
    steamrot::FlatbuffersConfigurator configurator(
        fixture.GetGameResources().event_handler);
    
    auto configure_result = configurator.ConfigureEntitiesFromCollection(
        expected_pool, config->expected_entity_collection());
    
    REQUIRE(configure_result.has_value());
    
    // Compare pools
    steamrot::tests::run_entity_memory_pool_comparison_test(pool, expected_pool);
  }
}
