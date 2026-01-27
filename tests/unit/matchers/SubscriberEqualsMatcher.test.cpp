/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for SubscriberEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SubscriberEqualsMatcher.h"
#include "conmat.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SubscriberEqualsMatcher works correctly",
          "[unit][Events][Subscriber][matcher]") {
  steamrot::Subscriber expected{steamrot::EventTypeFbs_EVENT_TEST};

  SECTION("Matcher detects differences in active state") {
    steamrot::Subscriber actual{steamrot::EventTypeFbs_EVENT_TEST};
    expected.m_active = true;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsSubscriber(expected));
  }

  SECTION("Matcher detects differences in event type") {
    steamrot::Subscriber actual{
        steamrot::EventTypeFbs_EVENT_USER_INPUT};
    REQUIRE_THAT(actual, !steamrot::tests::EqualsSubscriber(expected));
  }

  SECTION("Matcher detects equality with same event type") {
    steamrot::Subscriber actual{steamrot::EventTypeFbs_EVENT_TEST};
    REQUIRE_THAT(actual, steamrot::tests::EqualsSubscriber(expected));
  }

  SECTION("Matcher detects equality with both active") {
    steamrot::Subscriber actual{steamrot::EventTypeFbs_EVENT_TEST};
    expected.m_active = true;
    actual.m_active = true;
    REQUIRE_THAT(actual, steamrot::tests::EqualsSubscriber(expected));
  }
}

TEST_CASE("SubscriberEqualsMatcher works with trigger data",
          "[unit][Events][Subscriber][matcher]") {
  steamrot::UserInterfaceName trigger_name = "test_ui";
  steamrot::Subscriber expected{steamrot::EventTypeFbs_EVENT_TEST,
                                trigger_name};

  SECTION("Matcher detects differences in trigger data presence") {
    steamrot::Subscriber actual{steamrot::EventTypeFbs_EVENT_TEST};
    REQUIRE_THAT(actual, !steamrot::tests::EqualsSubscriber(expected));
  }

  SECTION("Matcher detects equality with same trigger data") {
    steamrot::Subscriber actual{steamrot::EventTypeFbs_EVENT_TEST,
                                trigger_name};
    REQUIRE_THAT(actual, steamrot::tests::EqualsSubscriber(expected));
  }

  SECTION("Matcher detects differences in trigger data value") {
    steamrot::UserInterfaceName different_name = "different_ui";
    steamrot::Subscriber actual{steamrot::EventTypeFbs_EVENT_TEST,
                                different_name};
    REQUIRE_THAT(actual, !steamrot::tests::EqualsSubscriber(expected));
  }
}

TEST_CASE("SubscriberEqualsMatcher describe is as expected on success",
          "[unit][Events][Subscriber][matcher]") {
  steamrot::Subscriber expected{steamrot::EventTypeFbs_EVENT_TEST};
  steamrot::Subscriber actual{steamrot::EventTypeFbs_EVENT_TEST};
  auto matcher = steamrot::tests::EqualsSubscriber(expected);
  matcher.match(actual);

  std::ostringstream oss;
  oss << conmat::Divider("-", 40) << "\n";
  oss << conmat::TestPassed() << "Subscriber Match" << "\n";
  oss << conmat::Divider("-", 40) << "\n";

  REQUIRE(matcher.describe() == oss.str());
}

TEST_CASE("SubscriberEqualsMatcher describe is as expected on failure",
          "[unit][Events][Subscriber][matcher]") {
  steamrot::Subscriber expected{steamrot::EventTypeFbs_EVENT_TEST};
  steamrot::Subscriber actual{
      steamrot::EventTypeFbs_EVENT_USER_INPUT};

  expected.m_active = true;

  auto matcher = steamrot::tests::EqualsSubscriber(expected);
  matcher.match(actual);

  std::string description = matcher.describe();
  REQUIRE(description.find("Subscriber Match:") != std::string::npos);
  REQUIRE(description.find("m_active:") != std::string::npos);
  REQUIRE(description.find("m_trigger_event_type:") != std::string::npos);
}
