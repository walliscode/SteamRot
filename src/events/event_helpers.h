#pragma once

#include "EventPacket.h"
#include "FailInfo.h"
#include "user_input_generated.h"
#include <SFML/Graphics.hpp>
#include <expected>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Convert the flatbuffers UserInputBitsetData to a UserInputBitset
///
/// @param data Data to convert
/////////////////////////////////////////////////
std::expected<UserInputBitset, FailInfo>
ConvertFBDataToUserInputBitset(const UserInputBitsetData &data);

/////////////////////////////////////////////////
/// @brief Given the flatbuffers EventData union type and data pointer, convert
/// to EventData
///
/// @param data_type Enum type of the data in the union
/// @param data Flatbuffers data pointer
/////////////////////////////////////////////////
std::expected<EventData, FailInfo>
ConvertFlatbuffersEventDataDataToEventData(const EventDataData data_type,
                                           const void *data);
} // namespace steamrot
