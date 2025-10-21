/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for Component and EntityMemoryPool custom matchers
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CMeta.h"
#include "CUserInterface.h"
#include "component_matchers.h"
#include "containers.h"
#include "entity_memory.h"
#include "entity_memory_pool_matchers.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

TEST_CASE("CMeta custom matcher works correctly",
          "[unit][Components][CMeta][matcher]") {
  steamrot::CMeta expected;
  expected.m_active = true;
  expected.m_entity_active = true;

  steamrot::CMeta actual;

  SECTION("Matcher detects differences") {
    actual.m_active = false;
    actual.m_entity_active = false;
    REQUIRE_FALSE(actual == expected);
  }

  SECTION("Matcher detects equality") {
    actual.m_active = true;
    actual.m_entity_active = true;
    REQUIRE_THAT(actual, steamrot::tests::EqualsCMeta(expected));
  }
}

TEST_CASE("CUserInterface custom matcher works correctly",
          "[unit][Components][CUserInterface][matcher]") {
  steamrot::CUserInterface expected;
  expected.m_name = "TestUI";
  expected.m_UI_visible = true;

  steamrot::CUserInterface actual;

  SECTION("Matcher detects differences in m_name") {
    actual.m_name = "DifferentUI";
    actual.m_UI_visible = true;
    REQUIRE_FALSE(actual == expected);
  }

  SECTION("Matcher detects equality") {
    actual.m_name = "TestUI";
    actual.m_UI_visible = true;
    REQUIRE_THAT(actual, steamrot::tests::EqualsCUserInterface(expected));
  }
}

TEST_CASE("Generic Component matcher works correctly",
          "[unit][Components][matcher]") {
  SECTION("Works with CMeta") {
    steamrot::CMeta expected;
    expected.m_active = true;

    steamrot::CMeta actual;
    actual.m_active = true;

    REQUIRE_THAT(actual, steamrot::tests::EqualsComponent(expected));
  }

  SECTION("Detects differences with CMeta") {
    steamrot::CMeta expected;
    expected.m_active = true;

    steamrot::CMeta actual;
    actual.m_active = false;

    REQUIRE_FALSE(actual == expected);
  }
}

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
