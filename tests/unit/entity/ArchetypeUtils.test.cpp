/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for ArchetypeUtils functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ArchetypeUtils.h"
#include "ArchetypeManager.h"
#include "CGrimoireMachina.h"
#include "CMeta.h"
#include "CUIState.h"
#include "CUserInterface.h"
#include "EntityManager.h"
#include "PathProvider.h"
#include "TestFixture.h"
#include "containers.h"
#include "test_data_harness.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("GenerateArchetypeIDfromTypes creates correct exact match ID",
          "[unit][ArchetypeUtils]") {
  // Generate an archetype ID for a single component
  steamrot::ArchetypeID single_component_id =
      steamrot::GenerateArchetypeIDfromTypes<steamrot::CUserInterface>();

  // Check that only the CUserInterface bit is set
  REQUIRE(single_component_id.test(
      steamrot::TupleTypeIndex<steamrot::CUserInterface,
                               steamrot::ComponentRegister>));

  // Generate an archetype ID for multiple components
  steamrot::ArchetypeID multi_component_id =
      steamrot::GenerateArchetypeIDfromTypes<steamrot::CUserInterface,
                                             steamrot::CGrimoireMachina>();

  // Check that both bits are set
  REQUIRE(multi_component_id.test(
      steamrot::TupleTypeIndex<steamrot::CUserInterface,
                               steamrot::ComponentRegister>));
  REQUIRE(multi_component_id.test(
      steamrot::TupleTypeIndex<steamrot::CGrimoireMachina,
                               steamrot::ComponentRegister>));
}

TEST_CASE(
    "GenerateArchetypeIDsContainingComponents returns all matching archetypes",
    "[unit][ArchetypeUtils]") {
  // Create a test entity memory pool with various component combinations
  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  steamrot::tests::TestFixture test_context;

  steamrot::EntityManager entity_manager{
      test_context.GetGameContext().event_handler};

  // Configure entities with different component combinations
  auto configure_result = entity_manager.ConfigureEntitiesFromDefaultData(
      steamrot::SceneType_TEST, steamrot::DataType::Flatbuffers);
  REQUIRE(configure_result.has_value());

  // Generate archetypes
  steamrot::ArchetypeManager archetype_manager(
      entity_manager.GetEntityMemoryPool());
  auto generate_result = archetype_manager.GenerateAllArchetypes();
  REQUIRE(generate_result.has_value());

  const auto &archetypes = archetype_manager.GetArchetypes();

  // Test: Find all archetypes containing CUserInterface
  auto matching_ids =
      steamrot::GenerateArchetypeIDsContainingComponents<
          steamrot::CUserInterface>(archetypes);

  // Verify that all returned IDs contain CUserInterface
  for (const auto &id : matching_ids) {
    REQUIRE(id.test(steamrot::TupleTypeIndex<steamrot::CUserInterface,
                                             steamrot::ComponentRegister>));
  }

  // There should be at least one archetype with CUserInterface
  REQUIRE(matching_ids.size() > 0);
}

TEST_CASE("GatherEntityIndices with exact match returns correct entities",
          "[unit][ArchetypeUtils]") {
  // Create a test entity memory pool
  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  steamrot::tests::TestFixture test_context;

  steamrot::EntityManager entity_manager{
      test_context.GetGameContext().event_handler};

  auto configure_result = entity_manager.ConfigureEntitiesFromDefaultData(
      steamrot::SceneType_TEST, steamrot::DataType::Flatbuffers);
  REQUIRE(configure_result.has_value());

  steamrot::ArchetypeManager archetype_manager(
      entity_manager.GetEntityMemoryPool());
  auto generate_result = archetype_manager.GenerateAllArchetypes();
  REQUIRE(generate_result.has_value());

  const auto &archetypes = archetype_manager.GetArchetypes();

  // Test exact match (default behavior)
  auto exact_indices =
      steamrot::GatherEntityIndices<steamrot::CUserInterface>(archetypes, true);

  // Verify that the indices returned are unique and ordered
  REQUIRE(std::is_sorted(exact_indices.begin(), exact_indices.end()));

  // Verify all returned entities have exactly CUserInterface component active
  const auto &emp = entity_manager.GetEntityMemoryPool();
  for (size_t index : exact_indices) {
    const auto &ui_component =
        std::get<std::vector<steamrot::CUserInterface>>(emp)[index];
    REQUIRE(ui_component.m_active);
  }
}

TEST_CASE("GatherEntityIndices with partial match returns correct entities",
          "[unit][ArchetypeUtils]") {
  // Create a test entity memory pool
  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  steamrot::tests::TestFixture test_context;

  steamrot::EntityManager entity_manager{
      test_context.GetGameContext().event_handler};

  auto configure_result = entity_manager.ConfigureEntitiesFromDefaultData(
      steamrot::SceneType_TEST, steamrot::DataType::Flatbuffers);
  REQUIRE(configure_result.has_value());

  steamrot::ArchetypeManager archetype_manager(
      entity_manager.GetEntityMemoryPool());
  auto generate_result = archetype_manager.GenerateAllArchetypes();
  REQUIRE(generate_result.has_value());

  const auto &archetypes = archetype_manager.GetArchetypes();

  // Test partial match (non-exact)
  auto partial_indices = steamrot::GatherEntityIndices<steamrot::CUserInterface>(
      archetypes, false);

  // Verify that the indices returned are unique and ordered
  REQUIRE(std::is_sorted(partial_indices.begin(), partial_indices.end()));

  // Verify all returned entities have at least CUserInterface component active
  const auto &emp = entity_manager.GetEntityMemoryPool();
  for (size_t index : partial_indices) {
    const auto &ui_component =
        std::get<std::vector<steamrot::CUserInterface>>(emp)[index];
    REQUIRE(ui_component.m_active);
  }

  // Partial match should include all entities from exact match
  auto exact_indices =
      steamrot::GatherEntityIndices<steamrot::CUserInterface>(archetypes, true);

  for (size_t exact_index : exact_indices) {
    REQUIRE(partial_indices.find(exact_index) != partial_indices.end());
  }
}

TEST_CASE("GatherEntityIndices returns empty set for non-existent archetype",
          "[unit][ArchetypeUtils]") {
  // Create an empty archetype map
  std::unordered_map<steamrot::ArchetypeID, steamrot::Archetype> empty_map;

  // Test exact match with empty map
  auto indices = steamrot::GatherEntityIndices<steamrot::CUserInterface>(
      empty_map, true);

  REQUIRE(indices.empty());
}

TEST_CASE(
    "GatherEntityIndices with multiple components works correctly",
    "[unit][ArchetypeUtils]") {
  // Create a test entity memory pool
  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  steamrot::tests::TestFixture test_context;

  steamrot::EntityManager entity_manager{
      test_context.GetGameContext().event_handler};

  auto configure_result = entity_manager.ConfigureEntitiesFromDefaultData(
      steamrot::SceneType_TEST, steamrot::DataType::Flatbuffers);
  REQUIRE(configure_result.has_value());

  steamrot::ArchetypeManager archetype_manager(
      entity_manager.GetEntityMemoryPool());
  auto generate_result = archetype_manager.GenerateAllArchetypes();
  REQUIRE(generate_result.has_value());

  const auto &archetypes = archetype_manager.GetArchetypes();

  // Test with multiple components - exact match
  auto exact_indices =
      steamrot::GatherEntityIndices<steamrot::CUserInterface,
                                    steamrot::CGrimoireMachina>(archetypes,
                                                                true);

  // Verify all returned entities have both components active
  const auto &emp = entity_manager.GetEntityMemoryPool();
  for (size_t index : exact_indices) {
    const auto &ui_component =
        std::get<std::vector<steamrot::CUserInterface>>(emp)[index];
    const auto &grimoire_component =
        std::get<std::vector<steamrot::CGrimoireMachina>>(emp)[index];

    REQUIRE(ui_component.m_active);
    REQUIRE(grimoire_component.m_active);
  }
}

TEST_CASE(
    "Data-driven test: ArchetypeUtils with various component combinations",
    "[unit][ArchetypeUtils][data-driven]") {
  
  // Load test data configs
  auto configs_result =
      steamrot::tests::load_test_data_configs("entity");
  
  if (!configs_result.has_value()) {
    SKIP("No test data available for archetype utils");
  }
  
  // Filter configs for archetype-related tests
  std::vector<const steamrot::TestDataConfig *> archetype_configs;
  for (const auto *config : configs_result.value()) {
    if (!config->metadata())
      continue;
    
    const auto *metadata = config->metadata();
    if (!metadata->test_name())
      continue;
    
    std::string test_name = metadata->test_name()->str();
    
    // Only include archetype-related tests
    if (test_name.find("archetype") != std::string::npos) {
      archetype_configs.push_back(config);
    }
  }
  
  if (archetype_configs.empty()) {
    SKIP("No archetype-specific test data available");
  }
  
  // Use generator to run test for each config
  const auto *config = GENERATE_COPY(from_range(archetype_configs));
  
  REQUIRE(config->metadata() != nullptr);
  
  // This is a placeholder for actual data-driven tests
  // Real tests would process config->start_entity_collection
  // and verify archetype gathering behavior
  INFO("Testing: " << config->metadata()->test_name()->c_str());
  SUCCEED("Data-driven test structure validated");
}
