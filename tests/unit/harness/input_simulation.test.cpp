/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for input_simulation harness functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "input_simulation.h"
#include "EventHandler.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <catch2/catch_test_macros.hpp>
#include <unordered_map>
#include <vector>

/////////////////////////////////////////////////
// execute_input_events_for_tick tests
/////////////////////////////////////////////////

TEST_CASE("execute_input_events_for_tick succeeds with an empty event list",
          "[unit][input_simulation]") {
  steamrot::EventHandler event_handler;
  std::vector<sf::Event> empty_events;

  auto result = steamrot::tests::execute_input_events_for_tick(empty_events,
                                                               event_handler);

  REQUIRE(result.has_value());
  // No events were fed so the waiting room bus should remain empty.
  REQUIRE(event_handler.GetWaitingRoomEventBus().empty());
}

TEST_CASE("execute_input_events_for_tick succeeds with a mouse move event",
          "[unit][input_simulation]") {
  steamrot::EventHandler event_handler;

  sf::Event::MouseMoved moved;
  moved.position = {10, 20};
  std::vector<sf::Event> events{sf::Event{moved}};

  auto result =
      steamrot::tests::execute_input_events_for_tick(events, event_handler);

  REQUIRE(result.has_value());
}

TEST_CASE("execute_input_events_for_tick succeeds with a key press event",
          "[unit][input_simulation]") {
  steamrot::EventHandler event_handler;

  sf::Event::KeyPressed pressed;
  pressed.code = sf::Keyboard::Key::Space;
  pressed.alt = false;
  pressed.control = false;
  pressed.shift = false;
  std::vector<sf::Event> events{sf::Event{pressed}};

  auto result =
      steamrot::tests::execute_input_events_for_tick(events, event_handler);

  REQUIRE(result.has_value());
}

TEST_CASE("execute_input_events_for_tick succeeds with multiple events",
          "[unit][input_simulation]") {
  steamrot::EventHandler event_handler;

  sf::Event::MouseButtonPressed click;
  click.button = sf::Mouse::Button::Left;
  click.position = {5, 5};

  sf::Event::KeyPressed kp;
  kp.code = sf::Keyboard::Key::Enter;
  kp.alt = false;
  kp.control = false;
  kp.shift = false;

  sf::Event::MouseButtonReleased release;
  release.button = sf::Mouse::Button::Left;
  release.position = {5, 5};

  std::vector<sf::Event> events{sf::Event{click}, sf::Event{kp},
                                 sf::Event{release}};

  auto result =
      steamrot::tests::execute_input_events_for_tick(events, event_handler);

  REQUIRE(result.has_value());
}

/////////////////////////////////////////////////
// execute_input_sequence tests
/////////////////////////////////////////////////

TEST_CASE("execute_input_sequence returns success when tick is not in the map",
          "[unit][input_simulation]") {
  steamrot::EventHandler event_handler;

  // Tick 3 has events; we query tick 99.
  sf::Event::MouseMoved moved;
  moved.position = {1, 2};
  std::unordered_map<size_t, std::vector<sf::Event>> input_map;
  input_map[3] = {sf::Event{moved}};

  auto result =
      steamrot::tests::execute_input_sequence(input_map, 99, event_handler);

  REQUIRE(result.has_value());
  // The waiting room bus should still be empty (no events injected).
  REQUIRE(event_handler.GetWaitingRoomEventBus().empty());
}

TEST_CASE("execute_input_sequence succeeds when tick matches",
          "[unit][input_simulation]") {
  steamrot::EventHandler event_handler;

  sf::Event::KeyPressed kp;
  kp.code = sf::Keyboard::Key::A;
  kp.alt = false;
  kp.control = false;
  kp.shift = false;
  std::unordered_map<size_t, std::vector<sf::Event>> input_map;
  input_map[2] = {sf::Event{kp}};

  auto result =
      steamrot::tests::execute_input_sequence(input_map, 2, event_handler);

  REQUIRE(result.has_value());
}

TEST_CASE("execute_input_sequence does not process a non-matching tick",
          "[unit][input_simulation]") {
  steamrot::EventHandler event_handler;

  sf::Event::KeyPressed kp;
  kp.code = sf::Keyboard::Key::Z;
  kp.alt = false;
  kp.control = false;
  kp.shift = false;
  std::unordered_map<size_t, std::vector<sf::Event>> input_map;
  input_map[0] = {sf::Event{kp}};

  // Process tick 1, which has no events registered
  auto result =
      steamrot::tests::execute_input_sequence(input_map, 1, event_handler);

  REQUIRE(result.has_value());
  // Waiting room should be empty since tick 1 had no events.
  REQUIRE(event_handler.GetWaitingRoomEventBus().empty());
}

TEST_CASE("execute_input_sequence succeeds with empty event list for the tick",
          "[unit][input_simulation]") {
  steamrot::EventHandler event_handler;

  // Register tick 5 with an empty list
  std::unordered_map<size_t, std::vector<sf::Event>> input_map;
  input_map[5] = {};

  auto result =
      steamrot::tests::execute_input_sequence(input_map, 5, event_handler);

  REQUIRE(result.has_value());
  REQUIRE(event_handler.GetWaitingRoomEventBus().empty());
}

TEST_CASE("execute_input_sequence processes only the matching tick from a "
          "multi-tick map",
          "[unit][input_simulation]") {
  steamrot::EventHandler event_handler;

  sf::Event::MouseMoved m1;
  m1.position = {10, 10};
  sf::Event::MouseMoved m2;
  m2.position = {20, 20};

  std::unordered_map<size_t, std::vector<sf::Event>> input_map;
  input_map[0] = {sf::Event{m1}};
  input_map[1] = {sf::Event{m2}};

  // Process tick 0 only
  auto result =
      steamrot::tests::execute_input_sequence(input_map, 0, event_handler);

  REQUIRE(result.has_value());
  // Both ticks are in the map but only tick 0 should have been processed.
  // The function does not modify the map, so both entries still exist.
  REQUIRE(input_map.size() == 2);
}
