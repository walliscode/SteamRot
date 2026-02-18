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
  steamrot::EventPacket event_packet;
  REQUIRE(event_packet.type == steamrot::EventType::NONE);
  REQUIRE(event_packet.context.lifetime == 1);
}
