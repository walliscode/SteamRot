/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for sfml_event_convert free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventPayload.h"
#include "UserInputBitset.h"
#include "sfml_event_convert.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <catch2/catch_test_macros.hpp>
#include <flatbuffers/flatbuffers.h>

// ---------------------------------------------------------------------------
// CollectInputEvents
// ---------------------------------------------------------------------------

TEST_CASE("CollectInputEvents returns empty bitset for empty event list",
          "[unit][sfml_event_convert]") {
  std::vector<sf::Event> no_events;
  steamrot::UserInputBitset result =
      steamrot::events::convert::CollectInputEvents(no_events);
  REQUIRE(result.none());
}

TEST_CASE("CollectInputEvents sets key-pressed bit for KeyPressed event",
          "[unit][sfml_event_convert]") {
  sf::Event::KeyPressed key_event;
  key_event.code = sf::Keyboard::Key::A;
  key_event.scancode = sf::Keyboard::Scan::A;
  key_event.alt = false;
  key_event.control = false;
  key_event.shift = false;
  key_event.system = false;

  std::vector<sf::Event> events{sf::Event{key_event}};
  steamrot::UserInputBitset result =
      steamrot::events::convert::CollectInputEvents(events);

  steamrot::UserInputBitset expected;
  expected.setKeyPressed(sf::Keyboard::Key::A);
  REQUIRE(result == expected);
}

TEST_CASE("CollectInputEvents sets mouse-pressed bit for MouseButtonPressed",
          "[unit][sfml_event_convert]") {
  sf::Event::MouseButtonPressed mouse_event;
  mouse_event.button = sf::Mouse::Button::Left;
  mouse_event.position = {0, 0};

  std::vector<sf::Event> events{sf::Event{mouse_event}};
  steamrot::UserInputBitset result =
      steamrot::events::convert::CollectInputEvents(events);

  steamrot::UserInputBitset expected;
  expected.setMousePressed(sf::Mouse::Button::Left);
  REQUIRE(result == expected);
}

// ---------------------------------------------------------------------------
// ResolveInputAction
// ---------------------------------------------------------------------------

TEST_CASE("ResolveInputAction returns nullopt for empty registry",
          "[unit][sfml_event_convert]") {
  steamrot::UserInputBitset bitset;
  bitset.setMousePressed(sf::Mouse::Button::Left);

  steamrot::events::convert::InputActionRegistry empty_registry;
  auto result =
      steamrot::events::convert::ResolveInputAction(bitset, empty_registry);
  REQUIRE_FALSE(result.has_value());
}

TEST_CASE("ResolveInputAction returns nullopt when no pattern matches",
          "[unit][sfml_event_convert]") {
  // Registry maps key A → SELECT.
  steamrot::UserInputBitset pattern;
  pattern.setKeyPressed(sf::Keyboard::Key::A);

  steamrot::events::convert::InputActionRegistry registry;
  registry.emplace(pattern, steamrot::InputPayload::InputAction::SELECT);

  // Accumulated bitset has key B, not A.
  steamrot::UserInputBitset accumulated;
  accumulated.setKeyPressed(sf::Keyboard::Key::B);

  auto result =
      steamrot::events::convert::ResolveInputAction(accumulated, registry);
  REQUIRE_FALSE(result.has_value());
}

TEST_CASE("ResolveInputAction matches when pattern bits are a subset of "
          "accumulated bits",
          "[unit][sfml_event_convert]") {
  // Registry maps LEFT_CLICK → SELECT.
  steamrot::UserInputBitset pattern;
  pattern.setMousePressed(sf::Mouse::Button::Left);

  steamrot::events::convert::InputActionRegistry registry;
  registry.emplace(pattern, steamrot::InputPayload::InputAction::SELECT);

  // Accumulated also has a keyboard key — should still match.
  steamrot::UserInputBitset accumulated;
  accumulated.setMousePressed(sf::Mouse::Button::Left);
  accumulated.setKeyPressed(sf::Keyboard::Key::B);

  auto result =
      steamrot::events::convert::ResolveInputAction(accumulated, registry);
  REQUIRE(result.has_value());
  REQUIRE(result.value() == steamrot::InputPayload::InputAction::SELECT);
}

TEST_CASE("ResolveInputAction returns first matching action when multiple "
          "patterns match",
          "[unit][sfml_event_convert]") {
  steamrot::UserInputBitset pattern_select;
  pattern_select.setMousePressed(sf::Mouse::Button::Left);

  steamrot::UserInputBitset pattern_select2;
  pattern_select2.setKeyPressed(sf::Keyboard::Key::B);

  steamrot::events::convert::InputActionRegistry registry;
  registry.emplace(pattern_select,
                   steamrot::InputPayload::InputAction::SELECT);
  registry.emplace(pattern_select2,
                   steamrot::InputPayload::InputAction::SELECT);

  // Both patterns are satisfied.
  steamrot::UserInputBitset accumulated;
  accumulated.setMousePressed(sf::Mouse::Button::Left);
  accumulated.setKeyPressed(sf::Keyboard::Key::B);

  auto result =
      steamrot::events::convert::ResolveInputAction(accumulated, registry);
  REQUIRE(result.has_value());
  REQUIRE(result.value() == steamrot::InputPayload::InputAction::SELECT);
}

TEST_CASE("ResolveInputAction does not match an all-zeros pattern",
          "[unit][sfml_event_convert]") {
  // An empty pattern should never fire (pat.any() guard).
  steamrot::UserInputBitset empty_pattern;

  steamrot::events::convert::InputActionRegistry registry;
  registry.emplace(empty_pattern,
                   steamrot::InputPayload::InputAction::SELECT);

  steamrot::UserInputBitset accumulated;
  accumulated.setMousePressed(sf::Mouse::Button::Left);

  auto result =
      steamrot::events::convert::ResolveInputAction(accumulated, registry);
  REQUIRE_FALSE(result.has_value());
}

// ---------------------------------------------------------------------------
// ConfigureInputAction
// ---------------------------------------------------------------------------

TEST_CASE("ConfigureInputAction maps InputActionFbs_SELECT to SELECT",
          "[unit][sfml_event_convert]") {
  steamrot::InputPayload::InputAction action{
      steamrot::InputPayload::InputAction::NONE};
  auto result = steamrot::events::convert::ConfigureInputAction(
      action, steamrot::InputActionFbs_SELECT);
  REQUIRE(result.has_value());
  REQUIRE(action == steamrot::InputPayload::InputAction::SELECT);
}

TEST_CASE("ConfigureInputAction fails for unknown enum value",
          "[unit][sfml_event_convert]") {
  steamrot::InputPayload::InputAction action{
      steamrot::InputPayload::InputAction::NONE};
  auto result = steamrot::events::convert::ConfigureInputAction(
      action, static_cast<steamrot::InputActionFbs>(99));
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NonExistentEnumValue);
}

// ---------------------------------------------------------------------------
// ConfigureInputActionMapping
// ---------------------------------------------------------------------------

TEST_CASE("ConfigureInputActionMapping fails with null data",
          "[unit][sfml_event_convert]") {
  steamrot::UserInputBitset bitset;
  steamrot::InputPayload::InputAction action{
      steamrot::InputPayload::InputAction::NONE};

  auto result =
      steamrot::events::convert::ConfigureInputActionMapping(bitset, action,
                                                             nullptr);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("ConfigureInputActionMapping sets mouse-pressed bit and SELECT action",
          "[unit][sfml_event_convert]") {
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

  auto result =
      steamrot::events::convert::ConfigureInputActionMapping(bitset, action,
                                                             mapping_data);
  REQUIRE(result.has_value());
  REQUIRE(action == steamrot::InputPayload::InputAction::SELECT);

  steamrot::UserInputBitset expected;
  expected.setMousePressed(sf::Mouse::Button::Left);
  REQUIRE(bitset == expected);
}

TEST_CASE("ConfigureInputActionMapping sets keyboard-pressed bit",
          "[unit][sfml_event_convert]") {
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

  auto result =
      steamrot::events::convert::ConfigureInputActionMapping(bitset, action,
                                                             mapping_data);
  REQUIRE(result.has_value());

  steamrot::UserInputBitset expected;
  expected.setKeyPressed(sf::Keyboard::Key::A);
  REQUIRE(bitset == expected);
}

// ---------------------------------------------------------------------------
// ConfigureInputActionRegistry
// ---------------------------------------------------------------------------

TEST_CASE("ConfigureInputActionRegistry fails with null data",
          "[unit][sfml_event_convert]") {
  steamrot::events::convert::InputActionRegistry registry;
  auto result =
      steamrot::events::convert::ConfigureInputActionRegistry(registry, nullptr);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("ConfigureInputActionRegistry succeeds with no mappings",
          "[unit][sfml_event_convert]") {
  flatbuffers::FlatBufferBuilder builder;
  steamrot::InputActionConfigFbsBuilder config_builder(builder);
  auto config_offset = config_builder.Finish();
  builder.Finish(config_offset);

  const auto *config_data =
      flatbuffers::GetRoot<steamrot::InputActionConfigFbs>(
          builder.GetBufferPointer());

  steamrot::events::convert::InputActionRegistry registry;
  auto result =
      steamrot::events::convert::ConfigureInputActionRegistry(registry,
                                                              config_data);
  REQUIRE(result.has_value());
  REQUIRE(registry.empty());
}

TEST_CASE("ConfigureInputActionRegistry populates registry with one mapping",
          "[unit][sfml_event_convert]") {
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

  steamrot::events::convert::InputActionRegistry registry;
  auto result =
      steamrot::events::convert::ConfigureInputActionRegistry(registry,
                                                              config_data);
  REQUIRE(result.has_value());
  REQUIRE(registry.size() == 1);

  steamrot::UserInputBitset expected_pattern;
  expected_pattern.setMousePressed(sf::Mouse::Button::Left);
  REQUIRE(registry.count(expected_pattern) == 1);
  REQUIRE(registry.at(expected_pattern) ==
          steamrot::InputPayload::InputAction::SELECT);
}
