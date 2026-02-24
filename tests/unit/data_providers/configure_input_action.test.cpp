/////////////////////////////////////////////////
/// @file
/// @brief Tests for input-action configure functions.
///
/// These functions have moved to sfml_event_convert (steamrot::events::convert
/// namespace). This file re-runs the same scenarios using the new namespace
/// for regression coverage from the data_providers test suite.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "sfml_event_convert.h"
#include "EventPayload.h"
#include "SFMLEventConverter.h"
#include "UserInputBitset.h"
#include "input_action_config_generated.h"
#include "user_input_generated.h"
#include <SFML/Window/Mouse.hpp>
#include <catch2/catch_test_macros.hpp>
#include <flatbuffers/flatbuffers.h>

// Bring the functions into scope for readability.
using namespace steamrot::events::convert;

TEST_CASE("ConfigureInputActionMapping fails with null data",
          "[unit][configure_input_action]") {
  steamrot::UserInputBitset bitset;
  steamrot::InputPayload::InputAction action{
      steamrot::InputPayload::InputAction::NONE};

  auto result = ConfigureInputActionMapping(bitset, action, nullptr);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("ConfigureInputActionMapping sets mouse-pressed bit and SELECT action",
          "[unit][configure_input_action]") {
  flatbuffers::FlatBufferBuilder builder;

  std::vector<steamrot::MouseInput> mouse_pressed_vec{
      steamrot::MouseInput_LEFT_CLICK};
  auto mouse_pressed = builder.CreateVector(mouse_pressed_vec);

  steamrot::InputActionMappingFbsBuilder mapping_builder(builder);
  mapping_builder.add_mouse_pressed(mouse_pressed);
  mapping_builder.add_action(steamrot::InputActionFbs_SELECT);
  auto mapping_offset = mapping_builder.Finish();
  builder.Finish(mapping_offset);

  const auto *mapping_data =
      flatbuffers::GetRoot<steamrot::InputActionMappingFbs>(
          builder.GetBufferPointer());

  steamrot::UserInputBitset bitset;
  steamrot::InputPayload::InputAction action{
      steamrot::InputPayload::InputAction::NONE};

  auto result = ConfigureInputActionMapping(bitset, action, mapping_data);
  REQUIRE(result.has_value());
  REQUIRE(action == steamrot::InputPayload::InputAction::SELECT);

  steamrot::UserInputBitset expected;
  expected.setMousePressed(sf::Mouse::Button::Left);
  REQUIRE(bitset == expected);
}

TEST_CASE("ConfigureInputActionMapping sets keyboard-pressed bit",
          "[unit][configure_input_action]") {
  flatbuffers::FlatBufferBuilder builder;

  std::vector<steamrot::KeyboardInput> kb_pressed_vec{
      steamrot::KeyboardInput_A};
  auto kb_pressed = builder.CreateVector(kb_pressed_vec);

  steamrot::InputActionMappingFbsBuilder mapping_builder(builder);
  mapping_builder.add_keyboard_pressed(kb_pressed);
  mapping_builder.add_action(steamrot::InputActionFbs_SELECT);
  auto mapping_offset = mapping_builder.Finish();
  builder.Finish(mapping_offset);

  const auto *mapping_data =
      flatbuffers::GetRoot<steamrot::InputActionMappingFbs>(
          builder.GetBufferPointer());

  steamrot::UserInputBitset bitset;
  steamrot::InputPayload::InputAction action{
      steamrot::InputPayload::InputAction::NONE};

  auto result = ConfigureInputActionMapping(bitset, action, mapping_data);
  REQUIRE(result.has_value());

  steamrot::UserInputBitset expected;
  expected.setKeyPressed(sf::Keyboard::Key::A);
  REQUIRE(bitset == expected);
}

TEST_CASE("ConfigureInputActionRegistry fails with null data",
          "[unit][configure_input_action]") {
  InputActionRegistry registry;
  auto result = ConfigureInputActionRegistry(registry, nullptr);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("ConfigureInputActionRegistry succeeds with no mappings",
          "[unit][configure_input_action]") {
  flatbuffers::FlatBufferBuilder builder;
  steamrot::InputActionConfigFbsBuilder config_builder(builder);
  auto config_offset = config_builder.Finish();
  builder.Finish(config_offset);

  const auto *config_data =
      flatbuffers::GetRoot<steamrot::InputActionConfigFbs>(
          builder.GetBufferPointer());

  InputActionRegistry registry;
  auto result = ConfigureInputActionRegistry(registry, config_data);
  REQUIRE(result.has_value());
  REQUIRE(registry.empty());
}

TEST_CASE("ConfigureInputActionRegistry populates registry with one mapping",
          "[unit][configure_input_action]") {
  flatbuffers::FlatBufferBuilder builder;

  std::vector<steamrot::MouseInput> mouse_pressed_vec{
      steamrot::MouseInput_LEFT_CLICK};
  auto mouse_pressed = builder.CreateVector(mouse_pressed_vec);

  steamrot::InputActionMappingFbsBuilder mapping_builder(builder);
  mapping_builder.add_mouse_pressed(mouse_pressed);
  mapping_builder.add_action(steamrot::InputActionFbs_SELECT);
  auto mapping_offset = mapping_builder.Finish();

  std::vector<flatbuffers::Offset<steamrot::InputActionMappingFbs>> mappings{
      mapping_offset};
  auto mappings_vec = builder.CreateVector(mappings);

  steamrot::InputActionConfigFbsBuilder config_builder(builder);
  config_builder.add_mappings(mappings_vec);
  auto config_offset = config_builder.Finish();
  builder.Finish(config_offset);

  const auto *config_data =
      flatbuffers::GetRoot<steamrot::InputActionConfigFbs>(
          builder.GetBufferPointer());

  InputActionRegistry registry;
  auto result = ConfigureInputActionRegistry(registry, config_data);
  REQUIRE(result.has_value());
  REQUIRE(registry.size() == 1);

  steamrot::UserInputBitset expected_pattern;
  expected_pattern.setMousePressed(sf::Mouse::Button::Left);
  REQUIRE(registry[0].first == expected_pattern);
  REQUIRE(registry[0].second == steamrot::InputPayload::InputAction::SELECT);
}
