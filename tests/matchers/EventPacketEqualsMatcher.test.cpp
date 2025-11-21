/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for EventPacketEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventPacketEqualsMatcher.h"
#include "conmat.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("EventPacketEqualsMatcher works correctly",
          "[unit][Events][EventPacket][matcher]") {
  steamrot::EventPacket expected;
  expected.m_event_type = steamrot::EventType::EventType_UserInputPress;
  expected.event_id = 1;
  expected.source_id = 100;
  expected.event_lifetime = steamrot::EventLifetime::OneTick;

  steamrot::EventPacket actual;

  SECTION("Matcher detects differences in m_event_type") {
    actual.m_event_type = steamrot::EventType::EventType_UserInputRelease;
    actual.event_id = 1;
    actual.source_id = 100;
    actual.event_lifetime = steamrot::EventLifetime::OneTick;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventPacket(expected));
  }

  SECTION("Matcher detects differences in event_id") {
    actual.m_event_type = steamrot::EventType::EventType_UserInputPress;
    actual.event_id = 2;
    actual.source_id = 100;
    actual.event_lifetime = steamrot::EventLifetime::OneTick;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventPacket(expected));
  }

  SECTION("Matcher detects differences in source_id") {
    actual.m_event_type = steamrot::EventType::EventType_UserInputPress;
    actual.event_id = 1;
    actual.source_id = 200;
    actual.event_lifetime = steamrot::EventLifetime::OneTick;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventPacket(expected));
  }

  SECTION("Matcher detects equality") {
    actual.m_event_type = steamrot::EventType::EventType_UserInputPress;
    actual.event_id = 1;
    actual.source_id = 100;
    actual.event_lifetime = steamrot::EventLifetime::OneTick;
    REQUIRE_THAT(actual, steamrot::tests::EqualsEventPacket(expected));
  }
}

TEST_CASE("EventPacketEqualsMatcher describe is as expected on success",
          "[unit][Events][EventPacket][matcher]") {
  steamrot::EventPacket expected;
  expected.m_event_type = steamrot::EventType::EventType_UserInputPress;
  steamrot::EventPacket actual;
  actual.m_event_type = steamrot::EventType::EventType_UserInputPress;
  auto matcher = steamrot::tests::EqualsEventPacket(expected);
  matcher.match(actual);

  std::ostringstream oss;
  oss << conmat::Divider("-", 40) << "\n";
  oss << conmat::TestPassed() << "EventPacket Match" << "\n";
  oss << conmat::Divider("-", 40) << "\n";

  REQUIRE(matcher.describe() == oss.str());
}

TEST_CASE("EventPacketEqualsMatcher describe is as expected on failure",
          "[unit][Events][EventPacket][matcher]") {
  steamrot::EventPacket expected;
  expected.m_event_type = steamrot::EventType::EventType_UserInputPress;
  expected.event_id = 1;

  steamrot::EventPacket actual;
  actual.m_event_type = steamrot::EventType::EventType_UserInputRelease;
  actual.event_id = 2;

  auto matcher = steamrot::tests::EqualsEventPacket(expected);
  matcher.match(actual);

  std::string description = matcher.describe();
  REQUIRE(description.find("EventPacket Match:") != std::string::npos);
  REQUIRE(description.find("m_event_type:") != std::string::npos);
  REQUIRE(description.find("event_id differs;") != std::string::npos);
}
