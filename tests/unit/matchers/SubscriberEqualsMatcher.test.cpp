/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for SubscriberEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SubscriberEqualsMatcher.h"
#include "EventPayload.h"
#include "EventType.h"
#include "conmat.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SubscriberEqualsMatcher works correctly",
          "[unit][Events][Subscriber][matcher]") {
  steamrot::Subscriber expected{};
  expected.event_type = steamrot::EventType::USER_INPUT;

  SECTION("Matcher detects differences in active state") {
    steamrot::Subscriber actual{};
    actual.event_type = steamrot::EventType::USER_INPUT;
    expected.m_active = true;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsSubscriber(expected));
  }

  SECTION("Matcher detects differences in event type") {
    steamrot::Subscriber actual{};
    actual.event_type = steamrot::EventType::UI;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsSubscriber(expected));
  }

  SECTION("Matcher detects equality with same event type") {
    steamrot::Subscriber actual{};
    actual.event_type = steamrot::EventType::USER_INPUT;
    REQUIRE_THAT(actual, steamrot::tests::EqualsSubscriber(expected));
  }

  SECTION("Matcher detects equality with both active") {
    steamrot::Subscriber actual{};
    actual.event_type = steamrot::EventType::USER_INPUT;
    expected.m_active = true;
    actual.m_active = true;
    REQUIRE_THAT(actual, steamrot::tests::EqualsSubscriber(expected));
  }
}

TEST_CASE("SubscriberEqualsMatcher works with filter_payload",
          "[unit][Events][Subscriber][matcher]") {
  steamrot::Subscriber expected{};
  expected.event_type = steamrot::EventType::UI;
  steamrot::UIPayload filter{};

  filter.action = steamrot::UIPayload::UIAction::TOGGLE;
  expected.filter_payload = filter;

  SECTION("Matcher detects differences in filter_payload presence") {
    steamrot::Subscriber actual{};
    actual.event_type = steamrot::EventType::UI;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsSubscriber(expected));
  }

  SECTION("Matcher detects equality with same filter_payload") {
    steamrot::Subscriber actual{};
    actual.event_type = steamrot::EventType::UI;
    actual.filter_payload = filter;
    REQUIRE_THAT(actual, steamrot::tests::EqualsSubscriber(expected));
  }

  SECTION("Matcher detects differences in filter_payload value") {
    steamrot::UIPayload different_filter{};
    different_filter.action = steamrot::UIPayload::UIAction::TOGGLE;

    steamrot::Subscriber actual{};
    actual.event_type = steamrot::EventType::UI;
    actual.filter_payload = different_filter;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsSubscriber(expected));
  }
}

TEST_CASE("SubscriberEqualsMatcher works with captured_payload",
          "[unit][Events][Subscriber][matcher]") {
  steamrot::Subscriber expected{};
  expected.event_type = steamrot::EventType::LOGIC;
  steamrot::LogicPayload captured{};
  captured.toggle_name =
      steamrot::LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD;
  expected.captured_payload = captured;

  SECTION("Matcher detects differences in captured_payload presence") {
    steamrot::Subscriber actual{};
    actual.event_type = steamrot::EventType::LOGIC;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsSubscriber(expected));
  }

  SECTION("Matcher detects equality with same captured_payload") {
    steamrot::Subscriber actual{};
    actual.event_type = steamrot::EventType::LOGIC;
    actual.captured_payload = captured;
    REQUIRE_THAT(actual, steamrot::tests::EqualsSubscriber(expected));
  }
}

TEST_CASE("SubscriberEqualsMatcher describe is as expected on success",
          "[unit][Events][Subscriber][matcher]") {
  steamrot::Subscriber expected{};
  expected.event_type = steamrot::EventType::USER_INPUT;
  steamrot::Subscriber actual{};
  actual.event_type = steamrot::EventType::USER_INPUT;
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
  steamrot::Subscriber expected{};
  expected.event_type = steamrot::EventType::USER_INPUT;
  expected.m_active = true;

  steamrot::Subscriber actual{};
  actual.event_type = steamrot::EventType::UI;

  auto matcher = steamrot::tests::EqualsSubscriber(expected);
  matcher.match(actual);

  std::string description = matcher.describe();
  REQUIRE(description.find("Subscriber Match:") != std::string::npos);
  REQUIRE(description.find("m_active:") != std::string::npos);
  REQUIRE(description.find("event_type:") != std::string::npos);
}
