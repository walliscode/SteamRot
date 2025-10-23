/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for entity test helper functions using data-driven approach
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "entity_test_helpers.h"
#include "FlatbuffersConfigurator.h"
#include "TestContext.h"
#include "TestDataLoader.h"
#include "entity_memory.h"
#include "containers.h"
#include "UIElementFactory.h"
#include <catch2/catch_test_macros.hpp>

/////////////////////////////////////////////////
/// @brief Helper function to create and configure an EntityMemoryPool from test data
///
/// @param test_data_name Name of the test data file to load
/// @param configurator FlatbuffersConfigurator instance to use
/// @return Configured EntityMemoryPool
/////////////////////////////////////////////////
steamrot::EntityMemoryPool CreatePoolFromTestData(
    const std::string &test_data_name,
    steamrot::FlatbuffersConfigurator &configurator) {
  
  steamrot::tests::TestDataLoader loader;
  
  // Load test data
  auto result = loader.LoadTestData(test_data_name, "unit/entity");
  REQUIRE(result.has_value());
  
  const auto *config = result.value();
  REQUIRE(config->entity_collection() != nullptr);
  
  // Get pool size from test data
  size_t pool_size = config->entity_collection()->entity_memory_pool_size();
  
  // Create and resize entity memory pool
  steamrot::EntityMemoryPool pool;
  std::apply(
      [pool_size](auto &...component_vector) {
        (component_vector.resize(pool_size), ...);
      },
      pool);
  
  // Configure entities from the test data manually
  const auto *entity_collection = config->entity_collection();
  size_t entity_count = entity_collection->entities()->size();
  
  for (size_t i = 0; i < entity_count; ++i) {
    const auto *entity_data = entity_collection->entities()->Get(i);
    
    // Configure CUserInterface if present
    if (entity_data->c_user_interface()) {
      auto &ui_component =
          steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(i, pool);
      
      const auto *ui_data = entity_data->c_user_interface();
      
      // Set basic properties
      ui_component.m_active = true;
      if (ui_data->ui_name()) {
        ui_component.m_name = ui_data->ui_name()->str();
      }
      ui_component.m_UI_visible = ui_data->start_visible();
      
      // Create root UI element if present
      if (ui_data->root_ui_element()) {
        steamrot::UIElementFactory factory;
        auto element_result = factory.CreateUIElement(ui_data->root_ui_element());
        if (element_result.has_value()) {
          ui_component.m_root_element = std::move(element_result.value());
        }
      }
    }
    
    // Configure CGrimoireMachina if present
    if (entity_data->c_grimoire_machina()) {
      auto &grimoire_component =
          steamrot::entity::memory::GetComponent<steamrot::CGrimoireMachina>(i, pool);
      
      grimoire_component.m_active = true;
      
      const auto *grimoire_data = entity_data->c_grimoire_machina();
      
      // Configure fragments if present
      if (grimoire_data->fragments()) {
        // Fragment configuration would go here
        // For now, just mark as active
      }
    }
  }
  
  return pool;
}

TEST_CASE("CompareEntityMemoryPools detects equal pools",
          "[unit][entity_test_helpers][data-driven]") {

  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  steamrot::tests::TestContext test_context;
  
  // Create configurator with test environment
  steamrot::FlatbuffersConfigurator configurator{
      test_context.GetGameContext().event_handler};

  // Create two identical pools from the same test data
  auto pool1 = CreatePoolFromTestData("pool_comparison_equal", configurator);
  auto pool2 = CreatePoolFromTestData("pool_comparison_equal", configurator);

  // This should pass without throwing - both pools are identical
  REQUIRE_NOTHROW(
      steamrot::tests::CompareEntityMemoryPools(pool1, pool2));
}

TEST_CASE("CompareEntityMemoryPools fails for different pool sizes",
          "[unit][entity_test_helpers][data-driven]") {

  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  steamrot::tests::TestContext test_context;
  
  // Create configurator with test environment
  steamrot::FlatbuffersConfigurator configurator{
      test_context.GetGameContext().event_handler};

  // Create pools with different sizes from different test data
  auto pool1 = CreatePoolFromTestData("pool_comparison_equal", configurator);
  auto pool2 = CreatePoolFromTestData("pool_comparison_different_size", configurator);

  // This should throw/fail because the pools have different sizes
  REQUIRE_THROWS(
      steamrot::tests::CompareEntityMemoryPools(pool1, pool2));
}

TEST_CASE("CompareEntityMemoryPools fails for different component values",
          "[unit][entity_test_helpers][data-driven]") {

  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  steamrot::tests::TestContext test_context;
  
  // Create configurator with test environment
  steamrot::FlatbuffersConfigurator configurator{
      test_context.GetGameContext().event_handler};

  // Create pools with different component values from different test data
  auto pool1 = CreatePoolFromTestData("pool_comparison_equal", configurator);
  auto pool2 = CreatePoolFromTestData("pool_comparison_different_values", configurator);

  // This should throw/fail because component values differ
  REQUIRE_THROWS(
      steamrot::tests::CompareEntityMemoryPools(pool1, pool2));
}
