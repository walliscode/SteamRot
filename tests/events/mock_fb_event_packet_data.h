/////////////////////////////////////////////////
/// @file
/// @brief Create static data for mocking flatbuffer EventPacket objects
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "event_packet_data_generated.h"
namespace steamrot::tests {

static flatbuffers::Offset<EventPacketData>
CreateTestQuitGameEventPacketData(flatbuffers::FlatBufferBuilder &builder) {
  return CreateEventPacketData(builder, 1, EventType_EVENT_QUIT_GAME,
                               EventDataData_NONE);
}
} // namespace steamrot::tests
