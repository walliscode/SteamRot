/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for EventPacketEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventPacketEqualsMatcher.h"
#include "EventPayload.h"
#include "EventType.h"
#include "conmat.h"
#include <catch2/catch_test_macros.hpp>
#include <random>

TEST_CASE("EventPacketEqualsMatcher works correctly",
          "[unit][Events][EventPacket][matcher]") {
  steamrot::EventPacket expected{};
  expected.type = steamrot::EventType::USER_INPUT;
  expected.context.lifetime = 1;
  expected.payload = steamrot::InputPayload{
      steamrot::InputPayload::InputAction::SELECT};

  steamrot::EventPacket actual{};

  SECTION("Matcher detects differences in type") {
    actual.type = steamrot::EventType::UI;
    actual.context.lifetime = 1;
    actual.payload = steamrot::InputPayload{
        steamrot::InputPayload::InputAction::SELECT};
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventPacket(expected));
  }

  SECTION("Matcher detects differences in context.lifetime") {
    actual.type = steamrot::EventType::USER_INPUT;
    actual.context.lifetime = 5;
    actual.payload = steamrot::InputPayload{
        steamrot::InputPayload::InputAction::SELECT};
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventPacket(expected));
  }

  SECTION("Matcher detects differences in payload") {
    actual.type = steamrot::EventType::USER_INPUT;
    actual.context.lifetime = 1;
    actual.payload = steamrot::SystemPayload{
        steamrot::SystemPayload::SystemAction::QUIT};
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventPacket(expected));
  }

  SECTION("Matcher detects equality") {
    actual.type = steamrot::EventType::USER_INPUT;
    actual.context.lifetime = 1;
    actual.payload = steamrot::InputPayload{
        steamrot::InputPayload::InputAction::SELECT};
    REQUIRE_THAT(actual, steamrot::tests::EqualsEventPacket(expected));
  }
}

TEST_CASE("EventPacketEqualsMatcher describe is as expected on success",
          "[unit][Events][EventPacket][matcher]") {
  steamrot::EventPacket expected{};
  expected.type = steamrot::EventType::USER_INPUT;
  expected.context.lifetime = 1;
  expected.payload = steamrot::InputPayload{
      steamrot::InputPayload::InputAction::SELECT};

  steamrot::EventPacket actual{};
  actual.type = steamrot::EventType::USER_INPUT;
  actual.context.lifetime = 1;
  actual.payload = steamrot::InputPayload{
      steamrot::InputPayload::InputAction::SELECT};

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
  steamrot::EventPacket expected{};
  expected.type = steamrot::EventType::USER_INPUT;
  expected.context.lifetime = 1;
  expected.payload = steamrot::InputPayload{
      steamrot::InputPayload::InputAction::SELECT};

  steamrot::EventPacket actual{};
  actual.type = steamrot::EventType::UI;
  actual.context.lifetime = 5;
  actual.payload = steamrot::SystemPayload{
      steamrot::SystemPayload::SystemAction::QUIT};

  auto matcher = steamrot::tests::EqualsEventPacket(expected);
  matcher.match(actual);

  std::string description = matcher.describe();
  REQUIRE(description.find("EventPacket Match:") != std::string::npos);
  REQUIRE(description.find("type:") != std::string::npos);
  REQUIRE(description.find("context.lifetime:") != std::string::npos);
}

