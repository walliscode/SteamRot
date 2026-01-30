/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the ArchetypeManager class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "ArchetypeManager.h"
#include "EventHandler.h"
#include "FlatbuffersEntityConfigurator.h"
#include "containers.h"
#include "load_entity_data.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

TEST_CASE("ArchetypeManager initializes correctly and manages archetypes",
          "[ArchetypeManager]") {

  // set up resources
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;
  auto [data, entity_variant] = LoadEntityTestData();
  steamrot::FlatbuffersEntityConfigurator configurator(event_handler);

  auto result =
      configurator.ConfigureEntityMemoryPoolFromSource(emp, entity_variant);
  if (!result.has_value()) {
    FAIL("Failed to configure entity memory pool");
  }

  steamrot::ArchetypeManager archetype_manager(emp);
  // map contains arcehyptes to check
  const std::unordered_map<steamrot::ArchetypeID, steamrot::Archetype>
      &archetypes = archetype_manager.GetArchetypes();

  REQUIRE(archetypes.empty());

  auto gen_result = archetype_manager.GenerateAllArchetypes();
  if (!gen_result.has_value()) {
    FAIL("Failed to generate all archetypes");
  }

  REQUIRE(!archetypes.empty());
}
