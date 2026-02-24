/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the SFMLEventConverter class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventPacket.h"
#include "EventPayload.h"
#include "EventType.h"
#include "SFMLEventConverter.h"
#include "UserInputBitset.h"
#include "input_action_config_generated.h"
#include "user_input_generated.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <catch2/catch_test_macros.hpp>
#include <flatbuffers/flatbuffers.h>

/////////////////////////////////////////////////
// Helpers
/////////////////////////////////////////////////

/// Build a minimal InputActionConfigFbs with a single LEFT_CLICK → SELECT
/// mapping, returning both the builder and the root pointer.
static std::pair<flatbuffers::FlatBufferBuilder, const steamrot::InputActionConfigFbs *>
BuildLeftClickSelectConfig() {
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

  const auto *config = flatbuffers::GetRoot<steamrot::InputActionConfigFbs>(
      builder.GetBufferPointer());
  return {std::move(builder), config};
}

/////////////////////////////////////////////////
// Tests
/////////////////////////////////////////////////

TEST_CASE("SFMLEventConverter default-constructs without error",
          "[unit][SFMLEventConverter]") {
  steamrot::SFMLEventConverter converter;
  SUCCEED("SFMLEventConverter default-constructed successfully");
}

TEST_CASE("SFMLEventConverter::Configure returns success for valid config",
          "[unit][SFMLEventConverter]") {
  auto [builder, config] = BuildLeftClickSelectConfig();
  steamrot::SFMLEventConverter converter;
  auto result = converter.Configure(config);
  REQUIRE(result.has_value());
}

TEST_CASE("SFMLEventConverter::Configure fails for null config",
          "[unit][SFMLEventConverter]") {
  steamrot::SFMLEventConverter converter;
  auto result = converter.Configure(nullptr);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("SFMLEventConverter::ConvertSFMLEvents returns empty vector when "
          "registry is empty",
          "[unit][SFMLEventConverter]") {
  steamrot::SFMLEventConverter converter;
  // No Configure() call → empty registry.

  std::vector<sf::Event> events;
  sf::Event::MouseButtonPressed press;
  press.button = sf::Mouse::Button::Left;
  press.position = {0, 0};
  events.push_back(sf::Event{press});

  auto result = converter.ConvertSFMLEvents(events);
  REQUIRE(result.empty());
}

TEST_CASE("SFMLEventConverter::ConvertSFMLEvents converts matching SFML event "
          "to InputEventPacket",
          "[unit][SFMLEventConverter]") {
  auto [builder, config] = BuildLeftClickSelectConfig();
  steamrot::SFMLEventConverter converter;
  REQUIRE(converter.Configure(config).has_value());

  // Simulate a left-click SFML event.
  sf::Event::MouseButtonPressed press;
  press.button = sf::Mouse::Button::Left;
  press.position = {0, 0};
  std::vector<sf::Event> sfml_events{sf::Event{press}};

  auto result = converter.ConvertSFMLEvents(sfml_events);

  REQUIRE(result.size() == 1);
  REQUIRE(result[0].type == steamrot::EventType::USER_INPUT);

  auto *payload = std::get_if<steamrot::InputPayload>(&result[0].payload);
  REQUIRE(payload != nullptr);
  REQUIRE(payload->action == steamrot::InputPayload::InputAction::SELECT);
}

TEST_CASE("SFMLEventConverter::ConvertSFMLEvents returns empty when event does "
          "not match registry",
          "[unit][SFMLEventConverter]") {
  // Registry maps LEFT_CLICK → SELECT; send right-click instead.
  auto [builder, config] = BuildLeftClickSelectConfig();
  steamrot::SFMLEventConverter converter;
  REQUIRE(converter.Configure(config).has_value());

  sf::Event::MouseButtonPressed press;
  press.button = sf::Mouse::Button::Right;
  press.position = {0, 0};
  std::vector<sf::Event> sfml_events{sf::Event{press}};

  auto result = converter.ConvertSFMLEvents(sfml_events);
  REQUIRE(result.empty());
}

TEST_CASE("SFMLEventConverter::ConvertSFMLEvents resets waiting-room bitset "
          "between calls",
          "[unit][SFMLEventConverter]") {
  auto [builder, config] = BuildLeftClickSelectConfig();
  steamrot::SFMLEventConverter converter;
  REQUIRE(converter.Configure(config).has_value());

  // First tick: left-click → produces one event.
  sf::Event::MouseButtonPressed press;
  press.button = sf::Mouse::Button::Left;
  press.position = {0, 0};
  auto result1 = converter.ConvertSFMLEvents({sf::Event{press}});
  REQUIRE(result1.size() == 1);

  // Second tick: no events → waiting room should be clear → no events.
  auto result2 = converter.ConvertSFMLEvents({});
  REQUIRE(result2.empty());
}
