/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for sfml_event_convert free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventPayload.h"
#include "InputActionRegistry.h"
#include "UserInputBitset.h"
#include "sfml_event_convert.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <catch2/catch_test_macros.hpp>

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

  steamrot::InputActionRegistry empty_registry;
  auto result =
      steamrot::events::convert::ResolveInputAction(bitset, empty_registry);
  REQUIRE_FALSE(result.has_value());
}

TEST_CASE("ResolveInputAction returns nullopt when no pattern matches",
          "[unit][sfml_event_convert]") {
  // Registry maps key A → SELECT.
  steamrot::UserInputBitset pattern;
  pattern.setKeyPressed(sf::Keyboard::Key::A);

  steamrot::InputActionRegistry registry;
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

  steamrot::InputActionRegistry registry;
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

  steamrot::InputActionRegistry registry;
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

  steamrot::InputActionRegistry registry;
  registry.emplace(empty_pattern,
                   steamrot::InputPayload::InputAction::SELECT);

  steamrot::UserInputBitset accumulated;
  accumulated.setMousePressed(sf::Mouse::Button::Left);

  auto result =
      steamrot::events::convert::ResolveInputAction(accumulated, registry);
  REQUIRE_FALSE(result.has_value());
}

// ---------------------------------------------------------------------------
// HasWindowCloseEvent
// ---------------------------------------------------------------------------

TEST_CASE("HasWindowCloseEvent returns false for empty event list",
          "[unit][sfml_event_convert]") {
  std::vector<sf::Event> no_events;
  REQUIRE_FALSE(
      steamrot::events::convert::HasWindowCloseEvent(no_events));
}

TEST_CASE("HasWindowCloseEvent returns false when only input events present",
          "[unit][sfml_event_convert]") {
  sf::Event::KeyPressed key_event;
  key_event.code = sf::Keyboard::Key::Escape;
  key_event.scancode = sf::Keyboard::Scan::Escape;
  key_event.alt = false;
  key_event.control = false;
  key_event.shift = false;
  key_event.system = false;

  std::vector<sf::Event> events{sf::Event{key_event}};
  REQUIRE_FALSE(
      steamrot::events::convert::HasWindowCloseEvent(events));
}

TEST_CASE("HasWindowCloseEvent returns true when sf::Event::Closed is present",
          "[unit][sfml_event_convert]") {
  std::vector<sf::Event> events{sf::Event{sf::Event::Closed{}}};
  REQUIRE(steamrot::events::convert::HasWindowCloseEvent(events));
}

TEST_CASE(
    "HasWindowCloseEvent returns true when Closed event is mixed with others",
    "[unit][sfml_event_convert]") {
  sf::Event::KeyPressed key_event;
  key_event.code = sf::Keyboard::Key::A;
  key_event.scancode = sf::Keyboard::Scan::A;
  key_event.alt = false;
  key_event.control = false;
  key_event.shift = false;
  key_event.system = false;

  std::vector<sf::Event> events{sf::Event{key_event},
                                sf::Event{sf::Event::Closed{}}};
  REQUIRE(steamrot::events::convert::HasWindowCloseEvent(events));
}

// ---------------------------------------------------------------------------
// ExtractWindowResizeSize
// ---------------------------------------------------------------------------

TEST_CASE("ExtractWindowResizeSize returns nullopt for empty event list",
          "[unit][sfml_event_convert]") {
  std::vector<sf::Event> no_events;
  REQUIRE_FALSE(
      steamrot::events::convert::ExtractWindowResizeSize(no_events)
          .has_value());
}

TEST_CASE("ExtractWindowResizeSize returns nullopt when only input events "
          "are present",
          "[unit][sfml_event_convert]") {
  sf::Event::MouseButtonPressed mouse_event;
  mouse_event.button = sf::Mouse::Button::Left;
  mouse_event.position = {0, 0};

  std::vector<sf::Event> events{sf::Event{mouse_event}};
  REQUIRE_FALSE(
      steamrot::events::convert::ExtractWindowResizeSize(events)
          .has_value());
}

TEST_CASE(
    "ExtractWindowResizeSize returns the new size from an Resized event",
    "[unit][sfml_event_convert]") {
  sf::Event::Resized resized_event;
  resized_event.size = {1024u, 768u};

  std::vector<sf::Event> events{sf::Event{resized_event}};
  auto result = steamrot::events::convert::ExtractWindowResizeSize(events);

  REQUIRE(result.has_value());
  REQUIRE(result.value().x == 1024u);
  REQUIRE(result.value().y == 768u);
}

TEST_CASE("ExtractWindowResizeSize returns the first resize size when multiple "
          "resize events are present",
          "[unit][sfml_event_convert]") {
  sf::Event::Resized first_resize;
  first_resize.size = {800u, 600u};

  sf::Event::Resized second_resize;
  second_resize.size = {1920u, 1080u};

  std::vector<sf::Event> events{sf::Event{first_resize},
                                sf::Event{second_resize}};
  auto result = steamrot::events::convert::ExtractWindowResizeSize(events);

  REQUIRE(result.has_value());
  REQUIRE(result.value().x == 800u);
  REQUIRE(result.value().y == 600u);
}
