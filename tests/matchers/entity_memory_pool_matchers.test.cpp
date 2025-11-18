/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for EntityMemoryPool custom matchers
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "entity_memory_pool_matchers.h"
#include "CGrimoireMachina.h"
#include "CMachinaForm.h"
#include "CMeta.h"
#include "CUIState.h"
#include "CUserInterface.h"
#include "containers.h"
#include "entity_memory.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

TEST_CASE("EntityMemoryPool matcher works correctly",
          "[unit][EntityMemoryPool][matcher]") {
  steamrot::EntityMemoryPool pool1;
  steamrot::EntityMemoryPool pool2;

  // Initialize both pools with same number of entities
  const size_t num_entities = 5;

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

  // Add components to pool2
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

  SECTION("Matcher detects equal pools") {
    REQUIRE_THAT(pool1, steamrot::tests::EqualsEntityMemoryPool(pool2));
  }

  SECTION("Matcher detects different pool sizes") {
    // Add one more entity to pool1
    cmeta_vec1.resize(num_entities + 1);

    auto matcher = steamrot::tests::EqualsEntityMemoryPool(pool2);
    REQUIRE_FALSE(matcher.match(pool1));
  }

  SECTION("Matcher detects different component values") {
    // Modify a component in pool1
    cmeta_vec1[0].m_active = true;
    cmeta_vec2[0].m_active = false;

    auto matcher = steamrot::tests::EqualsEntityMemoryPool(pool2);
    REQUIRE_FALSE(matcher.match(pool1));
  }
}

TEST_CASE("EntityMemoryPool matcher provides detailed feedback",
          "[unit][EntityMemoryPool][matcher][feedback]") {
  steamrot::EntityMemoryPool pool1;
  steamrot::EntityMemoryPool pool2;

  // Create pools with different sizes
  auto &cmeta_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool1);
  cmeta_vec1.resize(5);

  auto &cmeta_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool2);
  cmeta_vec2.resize(3);

  auto &cui_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CUserInterface>(
          pool1);
  cui_vec1.resize(5);

  auto &cui_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CUserInterface>(
          pool2);
  cui_vec2.resize(3);

  auto &cform_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CMachinaForm>(
          pool1);
  cform_vec1.resize(5);

  auto &cform_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CMachinaForm>(
          pool2);
  cform_vec2.resize(3);

  auto &cgrim_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CGrimoireMachina>(
          pool1);
  cgrim_vec1.resize(5);

  auto &cgrim_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CGrimoireMachina>(
          pool2);
  cgrim_vec2.resize(3);

  auto &cstate_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CUIState>(pool1);
  cstate_vec1.resize(5);

  auto &cstate_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CUIState>(pool2);
  cstate_vec2.resize(3);

  SECTION("Matcher provides size mismatch feedback") {
    auto matcher = steamrot::tests::EqualsEntityMemoryPool(pool2);
    REQUIRE_FALSE(matcher.match(pool1));

    std::string desc = matcher.get_mismatch_description();
    REQUIRE_FALSE(desc.empty());
    REQUIRE(desc.find("Pool sizes differ") != std::string::npos);
  }
}

TEST_CASE("EntityMemoryPool matcher with metadata",
          "[unit][EntityMemoryPool][matcher][metadata]") {
  steamrot::EntityMemoryPool pool1;
  steamrot::EntityMemoryPool pool2;

  const size_t num_entities = 5;

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

  // Add components to pool2 with different size
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

  SECTION("Matcher includes metadata in description") {
    std::string test_metadata =
        "Test: my_test_name, Description: Test description";
    auto matcher =
        steamrot::tests::EqualsEntityMemoryPool(pool2, test_metadata);
    REQUIRE_FALSE(matcher.match(pool1));
  }
}
