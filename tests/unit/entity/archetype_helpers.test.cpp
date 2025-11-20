/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the archetype_helpers functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "archetype_helpers.h"
#include "CGrimoireMachina.h"
#include "CMeta.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("archetype_helpers::GenerateArchetypeIDfromTypes generates correct "
          "ArchetypeID",
          "[archetype_helpers]") {

  // Generate ArchetypeID from a single component type
  auto archetype_id_one =
      steamrot::GenerateArchetypeIDfromTypes<steamrot::CMeta>();
  REQUIRE(archetype_id_one.test(
      steamrot::IndexOf<steamrot::CMeta, steamrot::ComponentRegister>::value));
  REQUIRE(archetype_id_one.count() == 1);

  // Generate ArchetypeID from multiple component types
  auto archetype_id_two =
      steamrot::GenerateArchetypeIDfromTypes<steamrot::CMeta,
                                             steamrot::CGrimoireMachina>();
  REQUIRE(archetype_id_two.test(
      steamrot::IndexOf<steamrot::CMeta, steamrot::ComponentRegister>::value));
  REQUIRE(archetype_id_two.test(
      steamrot::IndexOf<steamrot::CGrimoireMachina,
                        steamrot::ComponentRegister>::value));
  REQUIRE(archetype_id_two.count() == 2);
}
TEST_CASE("archetype_helpers::GenerateArchetypeIDsFromTypeList generates "
          "correct number of ArchetypeIDs",
          "[archetype_helpers]") {

  // Generate ArchetypeIDs from the component types
  auto archetype_ids_one =
      steamrot::GenerateArchetypeIDsFromTypeList<steamrot::CMeta>();
  // Check that the number of generated ArchetypeIDs matches the number of
  // component types
  REQUIRE(archetype_ids_one.size() == 1);
  REQUIRE(archetype_ids_one[0].test(
      steamrot::IndexOf<steamrot::CMeta, steamrot::ComponentRegister>::value));

  auto archetype_ids_two =
      steamrot::GenerateArchetypeIDsFromTypeList<steamrot::CMeta,
                                                 steamrot::CGrimoireMachina>();
  REQUIRE(archetype_ids_two.size() == 2);
  REQUIRE(archetype_ids_two[0].test(
      steamrot::IndexOf<steamrot::CMeta, steamrot::ComponentRegister>::value));
  REQUIRE(archetype_ids_two[1].test(
      steamrot::IndexOf<steamrot::CGrimoireMachina,
                        steamrot::ComponentRegister>::value));
}

// creates a mcok     const std::unordered_map<ArchetypeID, Archetype>
// &archetypes for testing and grabs entity ids from it
TEST_CASE(
    "archetype_helpers::GenerateEntityIndexesFromComponents generates correct "
    "entity indexes with an exact match",
    "[archetype_helpers]") {

  // Create mock archetypes
  std::unordered_map<steamrot::ArchetypeID, steamrot::Archetype>
      mock_archetypes;

  // Archetype with CMeta component
  steamrot::ArchetypeID archetype_id_one =
      steamrot::GenerateArchetypeIDfromTypes<steamrot::CMeta>();
  steamrot::Archetype archetype_one;
  archetype_one = {1, 2, 3};
  mock_archetypes[archetype_id_one] = archetype_one;

  // Archetype with CMeta and CGrimoireMachina components
  steamrot::ArchetypeID archetype_id_two =
      steamrot::GenerateArchetypeIDfromTypes<steamrot::CMeta,
                                             steamrot::CGrimoireMachina>();
  steamrot::Archetype archetype_two;
  archetype_two = {4, 5, 6};
  mock_archetypes[archetype_id_two] = archetype_two;

  // Generate entity indexes for CMeta component
  auto entity_indexes_one =
      steamrot::GenerateEntityIndexesFromComponents<steamrot::CMeta>(
          mock_archetypes, true);

  REQUIRE(entity_indexes_one.size() == 3);
  REQUIRE(entity_indexes_one.count(1) == 1);
  REQUIRE(entity_indexes_one.count(2) == 1);
  REQUIRE(entity_indexes_one.count(3) == 1);

  // Generate entity indexes for CMeta and CGrimoireMachina components
  auto entity_indexes_two = steamrot::GenerateEntityIndexesFromComponents<
      steamrot::CMeta, steamrot::CGrimoireMachina>(mock_archetypes, true);
  REQUIRE(entity_indexes_two.size() == 3);
  REQUIRE(entity_indexes_two.count(4) == 1);
  REQUIRE(entity_indexes_two.count(5) == 1);
  REQUIRE(entity_indexes_two.count(6) == 1);
}

TEST_CASE(
    "archetype_helpers::GenerateEntityIndexesFromComponents generates correct "
    "entity indexes without an exact match",
    "[archetype_helpers]") {

  // Create mock archetypes
  std::unordered_map<steamrot::ArchetypeID, steamrot::Archetype>
      mock_archetypes;

  // Archetype with CMeta component
  steamrot::ArchetypeID archetype_id_one =
      steamrot::GenerateArchetypeIDfromTypes<steamrot::CMeta>();
  steamrot::Archetype archetype_one;
  archetype_one = {1, 2, 3};
  mock_archetypes[archetype_id_one] = archetype_one;

  // Archetype with CMeta and CGrimoireMachina components
  steamrot::ArchetypeID archetype_id_two =
      steamrot::GenerateArchetypeIDfromTypes<steamrot::CMeta,
                                             steamrot::CGrimoireMachina>();
  steamrot::Archetype archetype_two;
  archetype_two = {4, 5, 6};
  mock_archetypes[archetype_id_two] = archetype_two;

  // Generate entity indexes for CMeta component without exact match
  auto entity_indexes_one =
      steamrot::GenerateEntityIndexesFromComponents<steamrot::CMeta>(
          mock_archetypes, false);
  REQUIRE(entity_indexes_one.size() == 6);
  for (size_t i = 1; i <= 6; ++i) {
    REQUIRE(entity_indexes_one.count(i) == 1);
  }

  // Generate entity indexes for CGrimoireMachina component without exact match
  auto entity_indexes_two =
      steamrot::GenerateEntityIndexesFromComponents<steamrot::CGrimoireMachina>(
          mock_archetypes, false);
  REQUIRE(entity_indexes_two.size() == 3);
  REQUIRE(entity_indexes_two.count(4) == 1);
  REQUIRE(entity_indexes_two.count(5) == 1);
  REQUIRE(entity_indexes_two.count(6) == 1);
}
