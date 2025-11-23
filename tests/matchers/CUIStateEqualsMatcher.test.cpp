/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for CUIStateEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CUIStateEqualsMatcher.h"
#include "conmat.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CUIStateEqualsMatcher works correctly",
          "[unit][Components][CUIState][matcher]") {
  steamrot::CUIState expected;
  expected.m_active = true;

  steamrot::CUIState actual;

  SECTION("Matcher detects differences in m_active") {
    actual.m_active = false;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsCUIState(expected));
  }

  SECTION("Matcher detects equality") {
    actual.m_active = true;
    REQUIRE_THAT(actual, steamrot::tests::EqualsCUIState(expected));
  }
}

TEST_CASE("CUIStateEqualsMatcher describe is as expected on success",
          "[unit][Components][CUIState][matcher]") {
  steamrot::CUIState expected;
  expected.m_active = true;
  steamrot::CUIState actual;
  actual.m_active = true;
  auto matcher = steamrot::tests::EqualsCUIState(expected);

  std::ostringstream oss;
  oss << conmat::Header(conmat::TestPassed() + "CUIState Match:", 3) << "\n";

  REQUIRE(matcher.describe() == oss.str());
}

TEST_CASE("CUIStateEqualsMatcher describe is as expected on failure",
          "[unit][Components][CUIState][matcher]") {
  steamrot::CUIState expected;
  expected.m_active = true;
  steamrot::CUIState actual;
  actual.m_active = false;

  auto matcher = steamrot::tests::EqualsCUIState(expected);
  matcher.match(actual);

  std::ostringstream oss;
  oss << conmat::Header(conmat::TestFailed() + "CUIState Match:", 3) << "\n";
  oss << conmat::Indent(1) << conmat::TestFailed() << "m_active:"
      << "\n";
  oss << conmat::Indent(2)
      << "actual: " << conmat::Colorize(actual.m_active, conmat::Color::Red)
      << "\n";
  oss << conmat::Indent(2) << "expected: "
      << conmat::Colorize(expected.m_active, conmat::Color::Blue) << "\n";
  REQUIRE(matcher.describe() == oss.str());
}

TEST_CASE("CUIStateEqualsMatcher detects state_to_ui_visibility size "
          "differences",
          "[unit][Components][CUIState][matcher]") {
  steamrot::CUIState expected;
  expected.m_active = true;
  expected.m_state_to_ui_visibility["state1"] = steamrot::UIVisibilityState{};
  expected.m_state_to_ui_visibility["state2"] = steamrot::UIVisibilityState{};

  steamrot::CUIState actual;
  actual.m_active = true;
  actual.m_state_to_ui_visibility["state1"] = steamrot::UIVisibilityState{};

  REQUIRE_THAT(actual, !steamrot::tests::EqualsCUIState(expected));
}

TEST_CASE("CUIStateEqualsMatcher detects state_subscribers size differences",
          "[unit][Components][CUIState][matcher]") {
  steamrot::CUIState expected;
  expected.m_active = true;
  expected.m_state_subscribers["key1"] = {};
  expected.m_state_subscribers["key2"] = {};
  expected.m_state_subscribers["key3"] = {};

  steamrot::CUIState actual;
  actual.m_active = true;
  actual.m_state_subscribers["key1"] = {};

  REQUIRE_THAT(actual, !steamrot::tests::EqualsCUIState(expected));
}

TEST_CASE("CUIStateEqualsMatcher detects UIVisibilityState content "
          "differences",
          "[unit][Components][CUIState][matcher]") {
  steamrot::CUIState expected;
  expected.m_active = true;
  expected.m_state_to_ui_visibility["state1"] = steamrot::UIVisibilityState{};
  expected.m_state_to_ui_visibility["state1"].m_ui_indices_on = {0, 1, 2};
  expected.m_state_to_ui_visibility["state1"].m_ui_indices_off = {3, 4};

  SECTION("Matcher detects differences in m_ui_indices_on") {
    steamrot::CUIState actual;
    actual.m_active = true;
    actual.m_state_to_ui_visibility["state1"] = steamrot::UIVisibilityState{};
    actual.m_state_to_ui_visibility["state1"].m_ui_indices_on = {0, 1};
    actual.m_state_to_ui_visibility["state1"].m_ui_indices_off = {3, 4};

    REQUIRE_THAT(actual, !steamrot::tests::EqualsCUIState(expected));
  }

  SECTION("Matcher detects differences in m_ui_indices_off") {
    steamrot::CUIState actual;
    actual.m_active = true;
    actual.m_state_to_ui_visibility["state1"] = steamrot::UIVisibilityState{};
    actual.m_state_to_ui_visibility["state1"].m_ui_indices_on = {0, 1, 2};
    actual.m_state_to_ui_visibility["state1"].m_ui_indices_off = {3};

    REQUIRE_THAT(actual, !steamrot::tests::EqualsCUIState(expected));
  }

  SECTION("Matcher detects equality with same visibility state") {
    steamrot::CUIState actual;
    actual.m_active = true;
    actual.m_state_to_ui_visibility["state1"] = steamrot::UIVisibilityState{};
    actual.m_state_to_ui_visibility["state1"].m_ui_indices_on = {0, 1, 2};
    actual.m_state_to_ui_visibility["state1"].m_ui_indices_off = {3, 4};

    REQUIRE_THAT(actual, steamrot::tests::EqualsCUIState(expected));
  }
}

TEST_CASE("CUIStateEqualsMatcher detects state_values differences",
          "[unit][Components][CUIState][matcher]") {
  steamrot::CUIState expected;
  expected.m_active = true;
  expected.m_state_values["state1"] = true;
  expected.m_state_values["state2"] = false;

  SECTION("Matcher detects size differences in state_values") {
    steamrot::CUIState actual;
    actual.m_active = true;
    actual.m_state_values["state1"] = true;

    REQUIRE_THAT(actual, !steamrot::tests::EqualsCUIState(expected));
  }

  SECTION("Matcher detects value differences in state_values") {
    steamrot::CUIState actual;
    actual.m_active = true;
    actual.m_state_values["state1"] = false;
    actual.m_state_values["state2"] = false;

    REQUIRE_THAT(actual, !steamrot::tests::EqualsCUIState(expected));
  }

  SECTION("Matcher detects equality with same state_values") {
    steamrot::CUIState actual;
    actual.m_active = true;
    actual.m_state_values["state1"] = true;
    actual.m_state_values["state2"] = false;

    REQUIRE_THAT(actual, steamrot::tests::EqualsCUIState(expected));
  }
}

TEST_CASE("CUIStateEqualsMatcher detects missing keys in actual maps",
          "[unit][Components][CUIState][matcher]") {
  steamrot::CUIState expected;
  expected.m_active = true;
  expected.m_state_to_ui_visibility["state1"] = steamrot::UIVisibilityState{};
  expected.m_state_to_ui_visibility["state2"] = steamrot::UIVisibilityState{};
  expected.m_state_values["key1"] = true;
  expected.m_state_values["key2"] = false;
  expected.m_state_subscribers["sub1"] = {};
  expected.m_state_subscribers["sub2"] = {};

  steamrot::CUIState actual;
  actual.m_active = true;
  actual.m_state_to_ui_visibility["state1"] = steamrot::UIVisibilityState{};
  actual.m_state_values["key1"] = true;
  actual.m_state_subscribers["sub1"] = {};

  // Should detect missing "state2", "key2", and "sub2"
  REQUIRE_THAT(actual, !steamrot::tests::EqualsCUIState(expected));
}

TEST_CASE("CUIStateEqualsMatcher works with empty maps",
          "[unit][Components][CUIState][matcher]") {
  steamrot::CUIState expected;
  expected.m_active = true;

  SECTION("Matcher detects equality with both empty") {
    steamrot::CUIState actual;
    actual.m_active = true;

    REQUIRE_THAT(actual, steamrot::tests::EqualsCUIState(expected));
  }

  SECTION("Matcher allows extra keys in actual (subset matching)") {
    steamrot::CUIState actual;
    actual.m_active = true;
    actual.m_state_values["key"] = true;

    // Matcher is designed to check if actual contains all expected keys,
    // extra keys in actual are allowed (subset matching behavior)
    REQUIRE_THAT(actual, steamrot::tests::EqualsCUIState(expected));
  }
}

TEST_CASE("CUIStateEqualsMatcher detects state_subscribers content "
          "differences",
          "[unit][Components][CUIState][matcher]") {
  auto subscriber1 =
      std::make_shared<steamrot::Subscriber>(steamrot::EventType::EventType_EVENT_TEST);
  auto subscriber2 = std::make_shared<steamrot::Subscriber>(
      steamrot::EventType::EventType_EVENT_USER_INPUT);

  steamrot::CUIState expected;
  expected.m_active = true;
  expected.m_state_subscribers["sub1"] = {subscriber1};

  SECTION("Matcher detects differences in subscriber count") {
    steamrot::CUIState actual;
    actual.m_active = true;
    actual.m_state_subscribers["sub1"] = {subscriber1, subscriber2};

    REQUIRE_THAT(actual, !steamrot::tests::EqualsCUIState(expected));
  }

  SECTION("Matcher detects equality with same subscribers") {
    steamrot::CUIState actual;
    actual.m_active = true;
    actual.m_state_subscribers["sub1"] = {subscriber1};

    REQUIRE_THAT(actual, steamrot::tests::EqualsCUIState(expected));
  }
}

TEST_CASE("CUIStateEqualsMatcher handles multiple simultaneous differences",
          "[unit][Components][CUIState][matcher]") {
  steamrot::CUIState expected;
  expected.m_active = true;
  expected.m_state_to_ui_visibility["state1"] = steamrot::UIVisibilityState{};
  expected.m_state_to_ui_visibility["state1"].m_ui_indices_on = {0, 1};
  expected.m_state_values["key1"] = true;
  expected.m_state_values["key2"] = false;
  expected.m_state_subscribers["sub1"] = {};

  steamrot::CUIState actual;
  actual.m_active = false; // Different
  actual.m_state_to_ui_visibility["state1"] = steamrot::UIVisibilityState{};
  actual.m_state_to_ui_visibility["state1"].m_ui_indices_on = {0}; // Different
  actual.m_state_values["key1"] = false; // Different
  actual.m_state_values["key2"] = false;
  // Missing "sub1"

  REQUIRE_THAT(actual, !steamrot::tests::EqualsCUIState(expected));
}

TEST_CASE("CUIStateEqualsMatcher comprehensive describe() test",
          "[unit][Components][CUIState][matcher]") {
  steamrot::CUIState expected;
  expected.m_active = true;
  expected.m_state_to_ui_visibility["state1"] = steamrot::UIVisibilityState{};
  expected.m_state_to_ui_visibility["state1"].m_ui_indices_on = {0, 1};
  expected.m_state_values["key1"] = true;
  expected.m_state_subscribers["sub1"] = {};

  steamrot::CUIState actual;
  actual.m_active = false;
  actual.m_state_to_ui_visibility["state1"] = steamrot::UIVisibilityState{};
  actual.m_state_to_ui_visibility["state1"].m_ui_indices_on = {0};
  actual.m_state_values["key1"] = false;
  // Missing sub1

  auto matcher = steamrot::tests::EqualsCUIState(expected);
  matcher.match(actual);

  std::string description = matcher.describe();

  // Verify all differences are reported in describe()
  REQUIRE(description.find("CUIState Match:") != std::string::npos);
  REQUIRE(description.find("m_active:") != std::string::npos);
  REQUIRE(description.find("state1 m_ui_indices_on:") != std::string::npos);
  REQUIRE(description.find("key1 value mismatch:") != std::string::npos);
  REQUIRE(description.find("Missing key in actual: sub1") != std::string::npos);
}

TEST_CASE("CUIStateEqualsMatcher handles all maps being empty",
          "[unit][Components][CUIState][matcher]") {
  steamrot::CUIState expected;
  expected.m_active = false;

  steamrot::CUIState actual;
  actual.m_active = false;

  // Both have empty maps and same m_active - should match
  REQUIRE_THAT(actual, steamrot::tests::EqualsCUIState(expected));
}

TEST_CASE("CUIStateEqualsMatcher handles complex UIVisibilityState",
          "[unit][Components][CUIState][matcher]") {
  steamrot::CUIState expected;
  expected.m_active = true;
  expected.m_state_to_ui_visibility["complex_state"] =
      steamrot::UIVisibilityState{};
  expected.m_state_to_ui_visibility["complex_state"].m_ui_indices_on = {
      0, 5, 10, 15, 20};
  expected.m_state_to_ui_visibility["complex_state"].m_ui_indices_off = {
      1, 2, 3, 4, 6, 7, 8, 9};

  steamrot::CUIState actual;
  actual.m_active = true;
  actual.m_state_to_ui_visibility["complex_state"] =
      steamrot::UIVisibilityState{};
  actual.m_state_to_ui_visibility["complex_state"].m_ui_indices_on = {
      0, 5, 10, 15, 20};
  actual.m_state_to_ui_visibility["complex_state"].m_ui_indices_off = {
      1, 2, 3, 4, 6, 7, 8, 9};

  REQUIRE_THAT(actual, steamrot::tests::EqualsCUIState(expected));
}

TEST_CASE("CUIStateEqualsMatcher handles multiple state keys",
          "[unit][Components][CUIState][matcher]") {
  steamrot::CUIState expected;
  expected.m_active = true;
  expected.m_state_to_ui_visibility["state1"] = steamrot::UIVisibilityState{};
  expected.m_state_to_ui_visibility["state2"] = steamrot::UIVisibilityState{};
  expected.m_state_to_ui_visibility["state3"] = steamrot::UIVisibilityState{};
  expected.m_state_values["value1"] = true;
  expected.m_state_values["value2"] = false;
  expected.m_state_values["value3"] = true;
  expected.m_state_subscribers["sub1"] = {};
  expected.m_state_subscribers["sub2"] = {};

  SECTION("Matcher detects equality with all matching") {
    steamrot::CUIState actual;
    actual.m_active = true;
    actual.m_state_to_ui_visibility["state1"] = steamrot::UIVisibilityState{};
    actual.m_state_to_ui_visibility["state2"] = steamrot::UIVisibilityState{};
    actual.m_state_to_ui_visibility["state3"] = steamrot::UIVisibilityState{};
    actual.m_state_values["value1"] = true;
    actual.m_state_values["value2"] = false;
    actual.m_state_values["value3"] = true;
    actual.m_state_subscribers["sub1"] = {};
    actual.m_state_subscribers["sub2"] = {};

    REQUIRE_THAT(actual, steamrot::tests::EqualsCUIState(expected));
  }

  SECTION("Matcher detects difference in one of many values") {
    steamrot::CUIState actual;
    actual.m_active = true;
    actual.m_state_to_ui_visibility["state1"] = steamrot::UIVisibilityState{};
    actual.m_state_to_ui_visibility["state2"] = steamrot::UIVisibilityState{};
    actual.m_state_to_ui_visibility["state3"] = steamrot::UIVisibilityState{};
    actual.m_state_values["value1"] = true;
    actual.m_state_values["value2"] = true; // Different!
    actual.m_state_values["value3"] = true;
    actual.m_state_subscribers["sub1"] = {};
    actual.m_state_subscribers["sub2"] = {};

    REQUIRE_THAT(actual, !steamrot::tests::EqualsCUIState(expected));
  }
}
