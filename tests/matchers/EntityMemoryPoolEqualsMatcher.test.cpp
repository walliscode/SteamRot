/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for EntityMemoryPoolEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EntityMemoryPoolEqualsMatcher.h"
#include "CGrimoireMachina.h"
#include "CMachinaForm.h"
#include "CMeta.h"
#include "CUIState.h"
#include "CUserInterface.h"
#include "conmat.h"
#include "containers.h"
#include "emp_helpers.h"
#include "entity_memory.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("EntityMemoryPoolEqualsMatcher works correctly",
          "[unit][EntityMemoryPool][matcher]") {
  const size_t pool_size = 5;
  steamrot::EntityMemoryPool expected;
  steamrot::EntityMemoryPool actual;

  // Resize all component vectors for expected pool
  auto &cmeta_vec_expected =
      steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(expected);
  cmeta_vec_expected.resize(pool_size);

  auto &cui_vec_expected =
      steamrot::entity::memory::GetComponentVector<steamrot::CUserInterface>(
          expected);
  cui_vec_expected.resize(pool_size);

  auto &cform_vec_expected =
      steamrot::entity::memory::GetComponentVector<steamrot::CMachinaForm>(
          expected);
  cform_vec_expected.resize(pool_size);

  auto &cgrim_vec_expected =
      steamrot::entity::memory::GetComponentVector<steamrot::CGrimoireMachina>(
          expected);
  cgrim_vec_expected.resize(pool_size);

  auto &cstate_vec_expected =
      steamrot::entity::memory::GetComponentVector<steamrot::CUIState>(
          expected);
  cstate_vec_expected.resize(pool_size);

  // Resize all component vectors for actual pool
  auto &cmeta_vec_actual =
      steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(actual);
  cmeta_vec_actual.resize(pool_size);

  auto &cui_vec_actual =
      steamrot::entity::memory::GetComponentVector<steamrot::CUserInterface>(
          actual);
  cui_vec_actual.resize(pool_size);

  auto &cform_vec_actual =
      steamrot::entity::memory::GetComponentVector<steamrot::CMachinaForm>(
          actual);
  cform_vec_actual.resize(pool_size);

  auto &cgrim_vec_actual =
      steamrot::entity::memory::GetComponentVector<steamrot::CGrimoireMachina>(
          actual);
  cgrim_vec_actual.resize(pool_size);

  auto &cstate_vec_actual =
      steamrot::entity::memory::GetComponentVector<steamrot::CUIState>(actual);
  cstate_vec_actual.resize(pool_size);

  SECTION("Matcher detects equality for same size pools") {
    REQUIRE_THAT(actual, steamrot::tests::EqualsEntityMemoryPool(expected));
  }

  SECTION("Matcher detects differences in pool size") {
    steamrot::EntityMemoryPool different_pool;
    auto &cmeta_vec_diff =
        steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(
            different_pool);
    cmeta_vec_diff.resize(3);

    auto &cui_vec_diff =
        steamrot::entity::memory::GetComponentVector<steamrot::CUserInterface>(
            different_pool);
    cui_vec_diff.resize(3);

    auto &cform_vec_diff =
        steamrot::entity::memory::GetComponentVector<steamrot::CMachinaForm>(
            different_pool);
    cform_vec_diff.resize(3);

    auto &cgrim_vec_diff = steamrot::entity::memory::GetComponentVector<
        steamrot::CGrimoireMachina>(different_pool);
    cgrim_vec_diff.resize(3);

    auto &cstate_vec_diff =
        steamrot::entity::memory::GetComponentVector<steamrot::CUIState>(
            different_pool);
    cstate_vec_diff.resize(3);

    REQUIRE_THAT(different_pool,
                 !steamrot::tests::EqualsEntityMemoryPool(expected));
  }

  SECTION("Matcher detects differences in component data") {
    // Activate a component in actual but not in expected
    auto &meta = steamrot::emp_helpers::GetComponent<steamrot::CMeta>(0, actual);
    meta.m_active = true;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEntityMemoryPool(expected));
  }
}

TEST_CASE("EntityMemoryPoolEqualsMatcher describe is as expected on success",
          "[unit][EntityMemoryPool][matcher]") {
  const size_t pool_size = 5;
  steamrot::EntityMemoryPool expected;
  steamrot::EntityMemoryPool actual;

  // Resize all component vectors for expected pool
  auto &cmeta_vec_expected =
      steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(expected);
  cmeta_vec_expected.resize(pool_size);

  auto &cui_vec_expected =
      steamrot::entity::memory::GetComponentVector<steamrot::CUserInterface>(
          expected);
  cui_vec_expected.resize(pool_size);

  auto &cform_vec_expected =
      steamrot::entity::memory::GetComponentVector<steamrot::CMachinaForm>(
          expected);
  cform_vec_expected.resize(pool_size);

  auto &cgrim_vec_expected =
      steamrot::entity::memory::GetComponentVector<steamrot::CGrimoireMachina>(
          expected);
  cgrim_vec_expected.resize(pool_size);

  auto &cstate_vec_expected =
      steamrot::entity::memory::GetComponentVector<steamrot::CUIState>(
          expected);
  cstate_vec_expected.resize(pool_size);

  // Resize all component vectors for actual pool
  auto &cmeta_vec_actual =
      steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(actual);
  cmeta_vec_actual.resize(pool_size);

  auto &cui_vec_actual =
      steamrot::entity::memory::GetComponentVector<steamrot::CUserInterface>(
          actual);
  cui_vec_actual.resize(pool_size);

  auto &cform_vec_actual =
      steamrot::entity::memory::GetComponentVector<steamrot::CMachinaForm>(
          actual);
  cform_vec_actual.resize(pool_size);

  auto &cgrim_vec_actual =
      steamrot::entity::memory::GetComponentVector<steamrot::CGrimoireMachina>(
          actual);
  cgrim_vec_actual.resize(pool_size);

  auto &cstate_vec_actual =
      steamrot::entity::memory::GetComponentVector<steamrot::CUIState>(actual);
  cstate_vec_actual.resize(pool_size);

  auto matcher = steamrot::tests::EqualsEntityMemoryPool(expected);
  matcher.match(actual);

  std::string description = matcher.describe();
  REQUIRE(description.find("[PASSED]") != std::string::npos);
  REQUIRE(description.find("EntityMemoryPool Match") != std::string::npos);
}

TEST_CASE("EntityMemoryPoolEqualsMatcher describe is as expected on failure",
          "[unit][EntityMemoryPool][matcher]") {
  const size_t expected_size = 5;
  const size_t actual_size = 3;
  steamrot::EntityMemoryPool expected;
  steamrot::EntityMemoryPool actual;

  // Resize all component vectors for expected pool
  auto &cmeta_vec_expected =
      steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(expected);
  cmeta_vec_expected.resize(expected_size);

  auto &cui_vec_expected =
      steamrot::entity::memory::GetComponentVector<steamrot::CUserInterface>(
          expected);
  cui_vec_expected.resize(expected_size);

  auto &cform_vec_expected =
      steamrot::entity::memory::GetComponentVector<steamrot::CMachinaForm>(
          expected);
  cform_vec_expected.resize(expected_size);

  auto &cgrim_vec_expected =
      steamrot::entity::memory::GetComponentVector<steamrot::CGrimoireMachina>(
          expected);
  cgrim_vec_expected.resize(expected_size);

  auto &cstate_vec_expected =
      steamrot::entity::memory::GetComponentVector<steamrot::CUIState>(
          expected);
  cstate_vec_expected.resize(expected_size);

  // Resize all component vectors for actual pool
  auto &cmeta_vec_actual =
      steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(actual);
  cmeta_vec_actual.resize(actual_size);

  auto &cui_vec_actual =
      steamrot::entity::memory::GetComponentVector<steamrot::CUserInterface>(
          actual);
  cui_vec_actual.resize(actual_size);

  auto &cform_vec_actual =
      steamrot::entity::memory::GetComponentVector<steamrot::CMachinaForm>(
          actual);
  cform_vec_actual.resize(actual_size);

  auto &cgrim_vec_actual =
      steamrot::entity::memory::GetComponentVector<steamrot::CGrimoireMachina>(
          actual);
  cgrim_vec_actual.resize(actual_size);

  auto &cstate_vec_actual =
      steamrot::entity::memory::GetComponentVector<steamrot::CUIState>(actual);
  cstate_vec_actual.resize(actual_size);

  auto matcher = steamrot::tests::EqualsEntityMemoryPool(expected);
  matcher.match(actual);

  std::string description = matcher.describe();
  REQUIRE(description.find("[FAILED]") != std::string::npos);
  REQUIRE(description.find("EntityMemoryPool Match") != std::string::npos);
  REQUIRE(description.find("Size Mismatch:") != std::string::npos);
}
