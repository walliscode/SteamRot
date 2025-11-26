/////////////////////////////////////////////////
/// @file
/// @brief Declaration of EventData factory functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#pragma once

#include "EventPacket.h"
#include "FailInfo.h"
#include "user_input_generated.h"
#include <SFML/Graphics.hpp>
#include <expected>

namespace steamrot::event {

/////////////////////////////////////////////////
/// @brief Convert the flatbuffers UserInputBitsetData to a UserInputBitset
///
/// @param data Data to convert
/// @return UserInputBitset on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<UserInputBitset, FailInfo>
CreateUserInputBitset(const UserInputBitsetData &data);

/////////////////////////////////////////////////
/// @brief Convert the flatbuffers SceneChangePacketData to a SceneChangePacket
///
/// @param data Data to convert
/// @return SceneChangePacket on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<SceneChangePacket, FailInfo>
CreateSceneChangePacket(const SceneChangePacketData &data);

/////////////////////////////////////////////////
/// @brief Convert the flatbuffers UserInterfaceNameData to a UserInterfaceName
///
/// @param data Data to convert
/// @return UserInterfaceName on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<UserInterfaceName, FailInfo>
CreateUserInterfaceName(const UserInterfaceNameData &data);

/////////////////////////////////////////////////
/// @brief Given the flatbuffers EventData union type and data pointer, create
/// an EventData variant
///
/// @param data_type Enum type of the data in the union
/// @param data Flatbuffers data pointer
/// @return EventData on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<EventData, FailInfo>
CreateEventData(const EventDataData data_type, const void *data);

} // namespace steamrot::event
