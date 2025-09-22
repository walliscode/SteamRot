/////////////////////////////////////////////////
/// @file
/// @brief Create static data for mocking flatbuffer SubscriberData objects
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "events_generated.h"
#include "subscriber_config_generated.h"
#include "user_input_generated.h"
namespace steamrot::tests {

static flatbuffers::Offset<SubscriberData>
CreateTestUserInputSubscriberData(flatbuffers::FlatBufferBuilder &builder) {

  // create UserInputBitsetData with some keys pressed and released

  auto key_press_vec = builder.CreateVector(
      std::vector<uint8_t>{static_cast<uint8_t>(KeyboardInput_A),
                           static_cast<uint8_t>(KeyboardInput_W),
                           static_cast<uint8_t>(KeyboardInput_S)});
  auto key_release_vec = builder.CreateVector(std::vector<uint8_t>{});
  auto mouse_press_vec = builder.CreateVector(
      std::vector<uint8_t>{static_cast<uint8_t>(MouseInput_LEFT_CLICK)});
  auto mouse_release_vec = builder.CreateVector(std::vector<uint8_t>{});

  auto user_input_bitset_data =
      CreateUserInputBitsetData(builder, key_press_vec, key_release_vec,
                                mouse_press_vec, mouse_release_vec);

  return CreateSubscriberData(builder, EventType_EVENT_USER_INPUT);
}
} // namespace steamrot::tests
