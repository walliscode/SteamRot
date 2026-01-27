/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for EventPacketEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventPacketEqualsMatcher.h"
#include "conmat.h"
#include "uuid.h"
#include <catch2/catch_test_macros.hpp>
#include <random>

static uuids::uuid GenerateTestUUID() {
  std::random_device rd;
  auto seed_data = std::array<int, std::mt19937::state_size>{};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  std::mt19937 generator(seq);
  uuids::uuid_random_generator gen{generator};
  return gen();
}

TEST_CASE("EventPacketEqualsMatcher works correctly",
          "[unit][Events][EventPacket][matcher]") {
  steamrot::EventPacket expected{1};
  expected.m_event_type = steamrot::EventType::EVENT_USER_INPUT;
  expected.event_id = GenerateTestUUID();
  expected.source_id = GenerateTestUUID();

  steamrot::EventPacket actual{1};

  SECTION("Matcher detects differences in m_event_type") {
    actual.m_event_type = steamrot::EventType::EVENT_TEST;
    actual.event_id = expected.event_id;
    actual.source_id = expected.source_id;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventPacket(expected));
  }

  SECTION("Matcher detects differences in event_id") {
    actual.m_event_type = steamrot::EventType::EVENT_USER_INPUT;
    actual.event_id = GenerateTestUUID();
    actual.source_id = expected.source_id;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventPacket(expected));
  }

  SECTION("Matcher detects differences in source_id") {
    actual.m_event_type = steamrot::EventType::EVENT_USER_INPUT;
    actual.event_id = expected.event_id;
    actual.source_id = GenerateTestUUID();
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventPacket(expected));
  }

  SECTION("Matcher detects equality") {
    actual.m_event_type = steamrot::EventType::EVENT_USER_INPUT;
    actual.event_id = expected.event_id;
    actual.source_id = expected.source_id;
    REQUIRE_THAT(actual, steamrot::tests::EqualsEventPacket(expected));
  }
}

TEST_CASE("EventPacketEqualsMatcher describe is as expected on success",
          "[unit][Events][EventPacket][matcher]") {
  steamrot::EventPacket expected{1};
  expected.m_event_type = steamrot::EventType::EVENT_USER_INPUT;
  steamrot::EventPacket actual{1};
  actual.m_event_type = steamrot::EventType::EVENT_USER_INPUT;
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
  steamrot::EventPacket expected{1};
  expected.m_event_type = steamrot::EventType::EVENT_USER_INPUT;
  expected.event_id = GenerateTestUUID();

  steamrot::EventPacket actual{1};
  actual.m_event_type = steamrot::EventType::EVENT_TEST;
  actual.event_id = GenerateTestUUID();

  auto matcher = steamrot::tests::EqualsEventPacket(expected);
  matcher.match(actual);

  std::string description = matcher.describe();
  REQUIRE(description.find("EventPacket Match:") != std::string::npos);
  REQUIRE(description.find("m_event_type:") != std::string::npos);
  REQUIRE(description.find("event_id differs;") != std::string::npos);
}
