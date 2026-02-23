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
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SFMLEventConverter default-constructs without error",
          "[unit][SFMLEventConverter]") {
  steamrot::SFMLEventConverter converter;
  SUCCEED("SFMLEventConverter default-constructed successfully");
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
  // Build registry: LEFT_CLICK → SELECT.
  steamrot::UserInputBitset pattern;
  pattern.setMousePressed(sf::Mouse::Button::Left);

  steamrot::SFMLEventConverter::InputActionRegistry registry;
  registry.emplace_back(pattern, steamrot::InputPayload::InputAction::SELECT);

  steamrot::SFMLEventConverter converter;
  converter.Configure(std::move(registry));

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
  // Registry maps key A → SELECT.
  steamrot::UserInputBitset pattern;
  pattern.setKeyPressed(sf::Keyboard::Key::A);

  steamrot::SFMLEventConverter::InputActionRegistry registry;
  registry.emplace_back(pattern, steamrot::InputPayload::InputAction::SELECT);

  steamrot::SFMLEventConverter converter;
  converter.Configure(std::move(registry));

  // Send a right-click — should not match.
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
  // Registry: LEFT_CLICK → SELECT.
  steamrot::UserInputBitset pattern;
  pattern.setMousePressed(sf::Mouse::Button::Left);

  steamrot::SFMLEventConverter::InputActionRegistry registry;
  registry.emplace_back(pattern, steamrot::InputPayload::InputAction::SELECT);

  steamrot::SFMLEventConverter converter;
  converter.Configure(std::move(registry));

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
