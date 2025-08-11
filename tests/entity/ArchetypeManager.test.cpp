/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the ArchetypeManager class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ArchetypeManager.h"
#include "EntityManager.h"
#include "PathProvider.h"
#include "configuration_helpers.h"
#include "containers.h"
#include "scene_types_generated.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ArchetypeManager is constructed without errors",
          "[ArchetypeManager]") {

  // create an instance of the EMP to pass to the ArchetypeManager
  steamrot::EntityMemoryPool emp;
  steamrot::ArchetypeManager archetype_manager(emp);
  REQUIRE_NOTHROW(archetype_manager);
}

TEST_CASE("ArchetypeManager archetype map is empty with a non configured EMP",
          "[ArchetypeManager]") {

  // create an instance of the entity manager but don't configure it
  steamrot::EntityManager entity_manager{100};
  // check that the EMP is default constructed
  steamrot::tests::TestEMPIsDefaultConstructed(
      entity_manager.GetEntityMemoryPool());

  // create an instance of the ArchetypeManager
  steamrot::ArchetypeManager archetype_manager(
      entity_manager.GetEntityMemoryPool());
  // attempt to generate all archetype IDs
  auto generate_result = archetype_manager.GenerateAllArchetypes();
  // check that the result is not an error
  if (!generate_result.has_value())
    FAIL(generate_result.error().message);

  // check that the only archetype member is 0 and that it contains all the
  // entity indexes
  REQUIRE(archetype_manager.GetArchetypes().size() == 1);
  REQUIRE(archetype_manager.GetArchetypes().at(0).size() == 100);
}

TEST_CASE("ArchetypeManager generates archetype IDs correctly",
          "[ArchetypeManager]") {
  // create an instance of the entity manager and configure it
  steamrot::EntityManager entity_manager;
  steamrot::tests::TestEMPIsDefaultConstructed(
      entity_manager.GetEntityMemoryPool());

  steamrot::ArchetypeManager archetype_manager(
      entity_manager.GetEntityMemoryPool());

  // configure the entity memory pool and then generate archetypes
  auto configure_result = entity_manager.ConfigureEntitiesFromDefaultData(
      steamrot::SceneType_TEST, steamrot::EnvironmentType::Test,
      steamrot::DataType::Flatbuffers);
  if (!configure_result.has_value())
    FAIL(configure_result.error().message);

  auto generate_result = archetype_manager.GenerateAllArchetypes();
  if (!generate_result.has_value())
    FAIL(generate_result.error().message);

  // check that the archetypes are generated correctly
  const auto &archetypes = archetype_manager.GetArchetypes();
  steamrot::tests::TestArchetypesOfConfiguredEMPfromDefaultData(
      archetypes, steamrot::SceneType_TEST);
}
