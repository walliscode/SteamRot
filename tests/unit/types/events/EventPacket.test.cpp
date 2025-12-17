/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for EventPacket struct
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventPacket.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("EventPacket: Default Constructor", "[types]") {
  steamrot::EventPacket event_packet(5);
  REQUIRE(event_packet.event_type == steamrot::EventType::EventType_NONE);
  REQUIRE(std::holds_alternative<std::monostate>(event_packet.event_data));
  REQUIRE(event_packet.event_lifetime == 5);
  REQUIRE(event_packet.event_id.is_nil());
}
