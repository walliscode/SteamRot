/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for EventBusEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventBusEqualsMatcher.h"
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

TEST_CASE("EventBusEqualsMatcher works correctly",
          "[unit][Events][EventBus][matcher]") {
  steamrot::EventBus expected;
  steamrot::EventPacket packet1;
  packet1.m_event_type = steamrot::EventType::EventType_EVENT_USER_INPUT;
  packet1.event_id = GenerateTestUUID();
  expected.push_back(packet1);

  steamrot::EventBus actual;

  SECTION("Matcher detects differences in size") {
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventBus(expected));
  }

  SECTION("Matcher detects differences in EventPacket") {
    steamrot::EventPacket packet2;
    packet2.m_event_type = steamrot::EventType::EventType_EVENT_TEST;
    packet2.event_id = GenerateTestUUID();
    actual.push_back(packet2);
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventBus(expected));
  }

  SECTION("Matcher detects equality") {
    steamrot::EventPacket packet3;
    packet3.m_event_type = steamrot::EventType::EventType_EVENT_USER_INPUT;
    packet3.event_id = packet1.event_id;
    actual.push_back(packet3);
    REQUIRE_THAT(actual, steamrot::tests::EqualsEventBus(expected));
  }
}

TEST_CASE("EventBusEqualsMatcher describe is as expected on success",
          "[unit][Events][EventBus][matcher]") {
  steamrot::EventBus expected;
  steamrot::EventBus actual;
  auto matcher = steamrot::tests::EqualsEventBus(expected);
  matcher.match(actual);

  std::string description = matcher.describe();
  REQUIRE(description.find("[PASSED]") != std::string::npos);
  REQUIRE(description.find("EventBus Match") != std::string::npos);
}

TEST_CASE("EventBusEqualsMatcher describe is as expected on failure",
          "[unit][Events][EventBus][matcher]") {
  steamrot::EventBus expected;
  steamrot::EventPacket packet;
  packet.m_event_type = steamrot::EventType::EventType_EVENT_USER_INPUT;
  expected.push_back(packet);

  steamrot::EventBus actual;

  auto matcher = steamrot::tests::EqualsEventBus(expected);
  matcher.match(actual);

  std::string description = matcher.describe();
  REQUIRE(description.find("[FAILED]") != std::string::npos);
  REQUIRE(description.find("EventBus Match") != std::string::npos);
  REQUIRE(description.find("size mismatch:") != std::string::npos);
}
