/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for entity test helper functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "entity_test_helpers.h"
#include "entity_memory.h"
#include "containers.h"
#include "CMeta.h"
#include "CUserInterface.h"
#include "CMachinaForm.h"
#include "CGrimoireMachina.h"
#include "CUIState.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CompareEntityMemoryPools detects equal pools",
          "[unit][entity_test_helpers]") {

  steamrot::EntityMemoryPool pool1;
  steamrot::EntityMemoryPool pool2;

  // Initialize both pools with same number of entities
  const size_t num_entities = 3;

  // Add components to pool1
  auto &cmeta_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool1);
  cmeta_vec1.resize(num_entities);

  auto &cui_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CUserInterface>(
          pool1);
  cui_vec1.resize(num_entities);

  auto &cform_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CMachinaForm>(
          pool1);
  cform_vec1.resize(num_entities);

  auto &cgrim_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CGrimoireMachina>(
          pool1);
  cgrim_vec1.resize(num_entities);

  auto &cstate_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CUIState>(pool1);
  cstate_vec1.resize(num_entities);

  // Add components to pool2 (same configuration)
  auto &cmeta_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool2);
  cmeta_vec2.resize(num_entities);

  auto &cui_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CUserInterface>(
          pool2);
  cui_vec2.resize(num_entities);

  auto &cform_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CMachinaForm>(
          pool2);
  cform_vec2.resize(num_entities);

  auto &cgrim_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CGrimoireMachina>(
          pool2);
  cgrim_vec2.resize(num_entities);

  auto &cstate_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CUIState>(pool2);
  cstate_vec2.resize(num_entities);

  // This should pass without throwing
  REQUIRE_NOTHROW(
      steamrot::tests::CompareEntityMemoryPools(pool1, pool2));
}

TEST_CASE("CompareEntityMemoryPools fails for different pool sizes",
          "[unit][entity_test_helpers]") {

  steamrot::EntityMemoryPool pool1;
  steamrot::EntityMemoryPool pool2;

  // Initialize pools with different sizes
  auto &cmeta_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool1);
  cmeta_vec1.resize(5);

  auto &cui_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CUserInterface>(
          pool1);
  cui_vec1.resize(5);

  auto &cform_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CMachinaForm>(
          pool1);
  cform_vec1.resize(5);

  auto &cgrim_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CGrimoireMachina>(
          pool1);
  cgrim_vec1.resize(5);

  auto &cstate_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CUIState>(pool1);
  cstate_vec1.resize(5);

  // pool2 with different size
  auto &cmeta_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool2);
  cmeta_vec2.resize(3);

  auto &cui_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CUserInterface>(
          pool2);
  cui_vec2.resize(3);

  auto &cform_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CMachinaForm>(
          pool2);
  cform_vec2.resize(3);

  auto &cgrim_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CGrimoireMachina>(
          pool2);
  cgrim_vec2.resize(3);

  auto &cstate_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CUIState>(pool2);
  cstate_vec2.resize(3);

  // This should throw/fail because the pools are different sizes
  REQUIRE_THROWS(
      steamrot::tests::CompareEntityMemoryPools(pool1, pool2));
}

TEST_CASE("CompareEntityMemoryPools fails for different component values",
          "[unit][entity_test_helpers]") {

  steamrot::EntityMemoryPool pool1;
  steamrot::EntityMemoryPool pool2;

  // Initialize both pools with same number of entities
  const size_t num_entities = 3;

  // Set up pool1
  auto &cmeta_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool1);
  cmeta_vec1.resize(num_entities);
  cmeta_vec1[0].m_active = true;  // Set different value

  auto &cui_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CUserInterface>(
          pool1);
  cui_vec1.resize(num_entities);

  auto &cform_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CMachinaForm>(
          pool1);
  cform_vec1.resize(num_entities);

  auto &cgrim_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CGrimoireMachina>(
          pool1);
  cgrim_vec1.resize(num_entities);

  auto &cstate_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CUIState>(pool1);
  cstate_vec1.resize(num_entities);

  // Set up pool2 with different component values
  auto &cmeta_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool2);
  cmeta_vec2.resize(num_entities);
  cmeta_vec2[0].m_active = false;  // Different from pool1

  auto &cui_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CUserInterface>(
          pool2);
  cui_vec2.resize(num_entities);

  auto &cform_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CMachinaForm>(
          pool2);
  cform_vec2.resize(num_entities);

  auto &cgrim_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CGrimoireMachina>(
          pool2);
  cgrim_vec2.resize(num_entities);

  auto &cstate_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CUIState>(pool2);
  cstate_vec2.resize(num_entities);

  // This should throw/fail because component values differ
  REQUIRE_THROWS(
      steamrot::tests::CompareEntityMemoryPools(pool1, pool2));
}
