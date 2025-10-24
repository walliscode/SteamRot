/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for Component custom matchers
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "cmeta_matchers.h"
#include "component_matchers.h"
#include "cuser_interface_matchers.h"
#include "CMeta.h"
#include "CUserInterface.h"
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
    REQUIRE_THAT(actual, !steamrot::tests::EqualsCMeta(expected));
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
    REQUIRE_THAT(actual, !steamrot::tests::EqualsCUserInterface(expected));
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

    REQUIRE_THAT(actual, !steamrot::tests::EqualsComponent(expected));
  }
}
