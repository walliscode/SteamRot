////////////////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the SFMLEventConverter class.
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include "EventPacket.h"
#include "EventPayload.h"
#include "EventType.h"
#include "InputActionRegistry.h"
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
  // No SetInputActionRegistry() call → empty registry.

  std::vector<sf::Event> events;
  sf::Event::MouseButtonPressed press;
  press.button = sf::Mouse::Button::Left;
  press.position = {0, 0};
  events.push_back(sf::Event{press});

  auto result = converter.ConvertSFMLEvents(events);
  REQUIRE(result.empty());
}

TEST_CASE("SFMLEventConverter::SetInputActionRegistry and ConvertSFMLEvents "
          "converts matching SFML event to InputEventPacket",
          "[unit][SFMLEventConverter]") {
  steamrot::InputActionRegistry registry;
  steamrot::UserInputBitset pattern;
  pattern.setMousePressed(sf::Mouse::Button::Left);
  registry.emplace(pattern, steamrot::InputPayload::InputAction::SELECT);

  steamrot::SFMLEventConverter converter;
  converter.SetInputActionRegistry(std::move(registry));

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
  steamrot::InputActionRegistry registry;
  steamrot::UserInputBitset pattern;
  pattern.setMousePressed(sf::Mouse::Button::Left);
  registry.emplace(pattern, steamrot::InputPayload::InputAction::SELECT);

  steamrot::SFMLEventConverter converter;
  converter.SetInputActionRegistry(std::move(registry));

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
  steamrot::InputActionRegistry registry;
  steamrot::UserInputBitset pattern;
  pattern.setMousePressed(sf::Mouse::Button::Left);
  registry.emplace(pattern, steamrot::InputPayload::InputAction::SELECT);

  steamrot::SFMLEventConverter converter;
  converter.SetInputActionRegistry(std::move(registry));

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

TEST_CASE("SFMLEventConverter::ConvertSFMLEvents converts window-close event "
          "to SystemPayload::QUIT",
          "[unit][SFMLEventConverter]") {
  steamrot::SFMLEventConverter converter;

  std::vector<sf::Event> events{sf::Event{sf::Event::Closed{}}};
  auto result = converter.ConvertSFMLEvents(events);

  REQUIRE(result.size() == 1);
  REQUIRE(result[0].type == steamrot::EventType::SYSTEM);

  const auto *payload =
      std::get_if<steamrot::SystemPayload>(&result[0].payload);
  REQUIRE(payload != nullptr);
  REQUIRE(payload->action == steamrot::SystemPayload::SystemAction::QUIT);
}

TEST_CASE(
    "SFMLEventConverter::ConvertSFMLEvents converts window-resize event to "
    "SystemPayload::RESIZE carrying the new size",
    "[unit][SFMLEventConverter]") {
  steamrot::SFMLEventConverter converter;

  sf::Event::Resized resized_event;
  resized_event.size = {1280u, 720u};

  std::vector<sf::Event> events{sf::Event{resized_event}};
  auto result = converter.ConvertSFMLEvents(events);

  REQUIRE(result.size() == 1);
  REQUIRE(result[0].type == steamrot::EventType::SYSTEM);

  const auto *payload =
      std::get_if<steamrot::SystemPayload>(&result[0].payload);
  REQUIRE(payload != nullptr);
  REQUIRE(payload->action == steamrot::SystemPayload::SystemAction::RESIZE);
  REQUIRE(payload->optional_resize_size.has_value());
  REQUIRE(payload->optional_resize_size.value().x == 1280u);
  REQUIRE(payload->optional_resize_size.value().y == 720u);
}

TEST_CASE("SFMLEventConverter::ConvertSFMLEvents emits both close and resize "
          "events when both are present in one tick",
          "[unit][SFMLEventConverter]") {
  steamrot::SFMLEventConverter converter;

  sf::Event::Resized resized_event;
  resized_event.size = {800u, 600u};

  std::vector<sf::Event> events{sf::Event{sf::Event::Closed{}},
                                sf::Event{resized_event}};
  auto result = converter.ConvertSFMLEvents(events);

  // Exactly two SYSTEM events: QUIT and RESIZE.
  REQUIRE(result.size() == 2);

  bool found_quit = false;
  bool found_resize = false;
  for (const auto &packet : result) {
    REQUIRE(packet.type == steamrot::EventType::SYSTEM);
    const auto *payload =
        std::get_if<steamrot::SystemPayload>(&packet.payload);
    REQUIRE(payload != nullptr);
    if (payload->action == steamrot::SystemPayload::SystemAction::QUIT) {
      found_quit = true;
    }
    if (payload->action == steamrot::SystemPayload::SystemAction::RESIZE) {
      found_resize = true;
    }
  }
  REQUIRE(found_quit);
  REQUIRE(found_resize);
}
