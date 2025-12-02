/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for EventData factory functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "event_factory.h"
#include "EventPacket.h"
#include "events_generated.h"
#include "flatbuffers/buffer.h"
#include "user_input_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <variant>

/////////////////////////////////////////////////
// CreateUserInputBitset tests
/////////////////////////////////////////////////

TEST_CASE("CreateUserInputBitset creates empty bitset from empty data",
          "[unit][event_factory]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create empty UserInputBitsetData
  auto data_offset = steamrot::CreateUserInputBitsetData(builder);
  builder.Finish(data_offset);

  const steamrot::UserInputBitsetData *data =
      flatbuffers::GetRoot<steamrot::UserInputBitsetData>(
          builder.GetBufferPointer());

  auto result = steamrot::event::CreateUserInputBitset(*data);

  REQUIRE(result.has_value());
  REQUIRE(result.value().none()); // No bits should be set
}

TEST_CASE("CreateUserInputBitset handles keyboard pressed",
          "[unit][event_factory]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create UserInputBitsetData with A key pressed
  std::vector<uint8_t> keyboard_pressed = {steamrot::KeyboardInput_A};
  auto keyboard_pressed_vec = builder.CreateVector(keyboard_pressed);
  auto data_offset =
      steamrot::CreateUserInputBitsetData(builder, keyboard_pressed_vec);
  builder.Finish(data_offset);

  const steamrot::UserInputBitsetData *data =
      flatbuffers::GetRoot<steamrot::UserInputBitsetData>(
          builder.GetBufferPointer());

  auto result = steamrot::event::CreateUserInputBitset(*data);

  REQUIRE(result.has_value());
  REQUIRE(result.value().any());
  // A key pressed should set bit at position A (0)
  REQUIRE(result.value().test(static_cast<size_t>(sf::Keyboard::Key::A)));
}

TEST_CASE("CreateUserInputBitset handles keyboard released",
          "[unit][event_factory]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create UserInputBitsetData with A key released
  std::vector<uint8_t> keyboard_released = {steamrot::KeyboardInput_A};
  auto keyboard_released_vec = builder.CreateVector(keyboard_released);
  auto data_offset = steamrot::CreateUserInputBitsetData(builder,
                                                         0, // keyboard_pressed
                                                         keyboard_released_vec);
  builder.Finish(data_offset);

  const steamrot::UserInputBitsetData *data =
      flatbuffers::GetRoot<steamrot::UserInputBitsetData>(
          builder.GetBufferPointer());

  auto result = steamrot::event::CreateUserInputBitset(*data);

  REQUIRE(result.has_value());
  REQUIRE(result.value().any());
  // A key released should set bit at position A + KeyCount
  REQUIRE(result.value().test(static_cast<size_t>(sf::Keyboard::Key::A) +
                              sf::Keyboard::KeyCount));
}

TEST_CASE("CreateUserInputBitset handles mouse pressed",
          "[unit][event_factory]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create UserInputBitsetData with left click pressed
  std::vector<uint8_t> mouse_pressed = {steamrot::MouseInput_LEFT_CLICK};
  auto mouse_pressed_vec = builder.CreateVector(mouse_pressed);
  auto data_offset = steamrot::CreateUserInputBitsetData(builder,
                                                         0, // keyboard_pressed
                                                         0, // keyboard_released
                                                         mouse_pressed_vec);
  builder.Finish(data_offset);

  const steamrot::UserInputBitsetData *data =
      flatbuffers::GetRoot<steamrot::UserInputBitsetData>(
          builder.GetBufferPointer());

  auto result = steamrot::event::CreateUserInputBitset(*data);

  REQUIRE(result.has_value());
  REQUIRE(result.value().any());
  // Left click pressed should set bit at position Left + KeyCount * 2
  REQUIRE(result.value().test(static_cast<size_t>(sf::Mouse::Button::Left) +
                              sf::Keyboard::KeyCount * 2));
}

TEST_CASE("CreateUserInputBitset handles mouse released",
          "[unit][event_factory]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create UserInputBitsetData with left click released
  std::vector<uint8_t> mouse_released = {steamrot::MouseInput_LEFT_CLICK};
  auto mouse_released_vec = builder.CreateVector(mouse_released);
  auto data_offset = steamrot::CreateUserInputBitsetData(builder,
                                                         0, // keyboard_pressed
                                                         0, // keyboard_released
                                                         0, // mouse_pressed
                                                         mouse_released_vec);
  builder.Finish(data_offset);

  const steamrot::UserInputBitsetData *data =
      flatbuffers::GetRoot<steamrot::UserInputBitsetData>(
          builder.GetBufferPointer());

  auto result = steamrot::event::CreateUserInputBitset(*data);

  REQUIRE(result.has_value());
  REQUIRE(result.value().any());
  // Left click released should set bit at position Left + KeyCount * 2 +
  // ButtonCount
  REQUIRE(result.value().test(static_cast<size_t>(sf::Mouse::Button::Left) +
                              sf::Keyboard::KeyCount * 2 +
                              sf::Mouse::ButtonCount));
}

/////////////////////////////////////////////////
// CreateSceneChangePacket tests
/////////////////////////////////////////////////

TEST_CASE("CreateSceneChangePacket creates packet with valid scene type",
          "[unit][event_factory]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create SceneChangePacketData with just scene type
  auto data_offset =
      steamrot::CreateSceneChangePacketData(builder,
                                            0, // uuid
                                            steamrot::SceneType_TITLE);
  builder.Finish(data_offset);

  const steamrot::SceneChangePacketData *data =
      flatbuffers::GetRoot<steamrot::SceneChangePacketData>(
          builder.GetBufferPointer());

  auto result = steamrot::event::CreateSceneChangePacket(*data);

  REQUIRE(result.has_value());
  REQUIRE(result.value().second == steamrot::SceneType_TITLE);
  REQUIRE_FALSE(result.value().first.has_value()); // No UUID
}

TEST_CASE("CreateSceneChangePacket creates packet with valid UUID",
          "[unit][event_factory]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create SceneChangePacketData with valid UUID and scene type
  auto uuid_str = builder.CreateString("550e8400-e29b-41d4-a716-446655440000");
  auto data_offset = steamrot::CreateSceneChangePacketData(
      builder, uuid_str, steamrot::SceneType_TITLE);
  builder.Finish(data_offset);

  const steamrot::SceneChangePacketData *data =
      flatbuffers::GetRoot<steamrot::SceneChangePacketData>(
          builder.GetBufferPointer());

  auto result = steamrot::event::CreateSceneChangePacket(*data);

  REQUIRE(result.has_value());
  REQUIRE(result.value().second == steamrot::SceneType_TITLE);
  REQUIRE(result.value().first.has_value());
}

TEST_CASE("CreateSceneChangePacket fails with invalid UUID",
          "[unit][event_factory]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create SceneChangePacketData with invalid UUID
  auto uuid_str = builder.CreateString("not-a-valid-uuid");
  auto data_offset = steamrot::CreateSceneChangePacketData(
      builder, uuid_str, steamrot::SceneType_TITLE);
  builder.Finish(data_offset);

  const steamrot::SceneChangePacketData *data =
      flatbuffers::GetRoot<steamrot::SceneChangePacketData>(
          builder.GetBufferPointer());

  auto result = steamrot::event::CreateSceneChangePacket(*data);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::InvalidUUID);
}

TEST_CASE("CreateSceneChangePacket fails with missing scene type",
          "[unit][event_factory]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create SceneChangePacketData with scene type NONE (invalid)
  auto data_offset =
      steamrot::CreateSceneChangePacketData(builder,
                                            0, // uuid
                                            steamrot::SceneType_UNKNOWN);
  builder.Finish(data_offset);

  const steamrot::SceneChangePacketData *data =
      flatbuffers::GetRoot<steamrot::SceneChangePacketData>(
          builder.GetBufferPointer());

  auto result = steamrot::event::CreateSceneChangePacket(*data);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

/////////////////////////////////////////////////
// CreateUserInterfaceName tests
/////////////////////////////////////////////////

TEST_CASE("CreateUserInterfaceName creates name from valid data",
          "[unit][event_factory]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create UserInterfaceNameData with valid name
  auto name_str = builder.CreateString("test_ui_element");
  auto data_offset = steamrot::CreateUserInterfaceNameData(builder, name_str);
  builder.Finish(data_offset);

  const steamrot::UserInterfaceNameData *data =
      flatbuffers::GetRoot<steamrot::UserInterfaceNameData>(
          builder.GetBufferPointer());

  auto result = steamrot::event::CreateUserInterfaceName(*data);

  REQUIRE(result.has_value());
  REQUIRE(result.value() == "test_ui_element");
}

TEST_CASE("CreateUserInterfaceName fails with missing id",
          "[unit][event_factory]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create UserInterfaceNameData without id
  auto data_offset = steamrot::CreateUserInterfaceNameData(builder);
  builder.Finish(data_offset);

  const steamrot::UserInterfaceNameData *data =
      flatbuffers::GetRoot<steamrot::UserInterfaceNameData>(
          builder.GetBufferPointer());

  auto result = steamrot::event::CreateUserInterfaceName(*data);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

/////////////////////////////////////////////////
// CreateEventData tests
/////////////////////////////////////////////////

TEST_CASE("CreateEventData creates EventData from UserInputBitsetData",
          "[unit][event_factory]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create UserInputBitsetData with A key pressed
  std::vector<uint8_t> keyboard_pressed = {steamrot::KeyboardInput_A};
  auto keyboard_pressed_vec = builder.CreateVector(keyboard_pressed);
  auto data_offset =
      steamrot::CreateUserInputBitsetData(builder, keyboard_pressed_vec);
  builder.Finish(data_offset);

  const steamrot::UserInputBitsetData *data =
      flatbuffers::GetRoot<steamrot::UserInputBitsetData>(
          builder.GetBufferPointer());

  auto result = steamrot::event::CreateEventData(
      steamrot::EventDataData_UserInputBitsetData, data);

  REQUIRE(result.has_value());
  REQUIRE(std::holds_alternative<steamrot::UserInputBitset>(result.value()));
}

TEST_CASE("CreateEventData creates EventData from SceneChangePacketData",
          "[unit][event_factory]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create SceneChangePacketData
  auto data_offset =
      steamrot::CreateSceneChangePacketData(builder,
                                            0, // uuid
                                            steamrot::SceneType_TITLE);
  builder.Finish(data_offset);

  const steamrot::SceneChangePacketData *data =
      flatbuffers::GetRoot<steamrot::SceneChangePacketData>(
          builder.GetBufferPointer());

  auto result = steamrot::event::CreateEventData(
      steamrot::EventDataData_SceneChangePacketData, data);

  REQUIRE(result.has_value());
  REQUIRE(std::holds_alternative<steamrot::SceneChangePacket>(result.value()));
}

TEST_CASE("CreateEventData creates EventData from UserInterfaceNameData",
          "[unit][event_factory]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create UserInterfaceNameData
  auto name_str = builder.CreateString("test_ui");
  auto data_offset = steamrot::CreateUserInterfaceNameData(builder, name_str);
  builder.Finish(data_offset);

  const steamrot::UserInterfaceNameData *data =
      flatbuffers::GetRoot<steamrot::UserInterfaceNameData>(
          builder.GetBufferPointer());

  auto result = steamrot::event::CreateEventData(
      steamrot::EventDataData_UserInterfaceNameData, data);

  REQUIRE(result.has_value());
  REQUIRE(std::holds_alternative<steamrot::UserInterfaceName>(result.value()));
  REQUIRE(std::get<steamrot::UserInterfaceName>(result.value()) == "test_ui");
}

TEST_CASE("CreateEventData creates monostate from NONE type",
          "[unit][event_factory]") {
  auto result =
      steamrot::event::CreateEventData(steamrot::EventDataData_NONE, nullptr);

  REQUIRE(result.has_value());
  REQUIRE(std::holds_alternative<std::monostate>(result.value()));
}

TEST_CASE("CreateEventData fails for unhandled enum value",
          "[unit][event_factory]") {
  // Use an invalid enum value by casting
  auto invalid_type = static_cast<steamrot::EventDataData>(999);

  auto result = steamrot::event::CreateEventData(invalid_type, nullptr);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::EnumValueNotHandled);
}

TEST_CASE("CreateEventData propagates error from CreateSceneChangePacket",
          "[unit][event_factory]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create SceneChangePacketData with invalid UUID
  auto uuid_str = builder.CreateString("invalid-uuid");
  auto data_offset = steamrot::CreateSceneChangePacketData(
      builder, uuid_str, steamrot::SceneType_TITLE);
  builder.Finish(data_offset);

  const steamrot::SceneChangePacketData *data =
      flatbuffers::GetRoot<steamrot::SceneChangePacketData>(
          builder.GetBufferPointer());

  auto result = steamrot::event::CreateEventData(
      steamrot::EventDataData_SceneChangePacketData, data);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::InvalidUUID);
}

TEST_CASE("CreateEventData propagates error from CreateUserInterfaceName",
          "[unit][event_factory]") {
  flatbuffers::FlatBufferBuilder builder;

  // Create UserInterfaceNameData without id
  auto data_offset = steamrot::CreateUserInterfaceNameData(builder);
  builder.Finish(data_offset);

  const steamrot::UserInterfaceNameData *data =
      flatbuffers::GetRoot<steamrot::UserInterfaceNameData>(
          builder.GetBufferPointer());

  auto result = steamrot::event::CreateEventData(
      steamrot::EventDataData_UserInterfaceNameData, data);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}
