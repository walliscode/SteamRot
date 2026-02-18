/////////////////////////////////////////////////
/// @file
/// @brief unit tests for payload matchers
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "payload_matchers.h"
#include "EventPayload.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("MatchPayload deals with various configurations of InputPayload",
          "[MatchPayload]") {

  using namespace steamrot::events;
  steamrot::InputPayload filter_payload;
  steamrot::InputPayload event_payload;

  SECTION("Empty payloads match") {
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Mismatched actions do not match") {
    filter_payload.action = steamrot::InputPayload::InputAction::NONE;
    event_payload.action = steamrot::InputPayload::InputAction::SELECT;
    REQUIRE_FALSE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Matching payloads match") {
    filter_payload.action = steamrot::InputPayload::InputAction::SELECT;
    event_payload.action = steamrot::InputPayload::InputAction::SELECT;
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }
}
