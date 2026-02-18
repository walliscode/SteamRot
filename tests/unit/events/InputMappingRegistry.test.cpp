/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for InputMappingRegistry class
/////////////////////////////////////////////////

#include "InputMappingRegistry.h"
#include <SFML/Window/Event.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("InputMappingRegistry: Default constructor initializes default "
          "mappings",
          "[unit][events][InputMappingRegistry]") {
  steamrot::events::InputMappingRegistry registry;

  // Check default keyboard mappings
  REQUIRE(registry.GetActionForKey(sf::Keyboard::Key::Enter) ==
          steamrot::InputPayload::InputAction::SELECT);
  REQUIRE(registry.GetActionForKey(sf::Keyboard::Key::Space) ==
          steamrot::InputPayload::InputAction::SELECT);

  // Check default mouse mappings
  REQUIRE(registry.GetActionForMouseButton(sf::Mouse::Button::Left) ==
          steamrot::InputPayload::InputAction::SELECT);
}

TEST_CASE("InputMappingRegistry: MapKeyboardKey adds new mapping",
          "[unit][events][InputMappingRegistry]") {
  steamrot::events::InputMappingRegistry registry;

  // Map a key that's not in the defaults
  registry.MapKeyboardKey(sf::Keyboard::Key::A,
                          steamrot::InputPayload::InputAction::SELECT);

  REQUIRE(registry.GetActionForKey(sf::Keyboard::Key::A) ==
          steamrot::InputPayload::InputAction::SELECT);
}

TEST_CASE("InputMappingRegistry: MapMouseButton adds new mapping",
          "[unit][events][InputMappingRegistry]") {
  steamrot::events::InputMappingRegistry registry;

  // Map a button that's not in the defaults
  registry.MapMouseButton(sf::Mouse::Button::Right,
                          steamrot::InputPayload::InputAction::SELECT);

  REQUIRE(registry.GetActionForMouseButton(sf::Mouse::Button::Right) ==
          steamrot::InputPayload::InputAction::SELECT);
}

TEST_CASE("InputMappingRegistry: GetActionForKey returns NONE for unmapped "
          "key",
          "[unit][events][InputMappingRegistry]") {
  steamrot::events::InputMappingRegistry registry;

  // Check an unmapped key
  REQUIRE(registry.GetActionForKey(sf::Keyboard::Key::Z) ==
          steamrot::InputPayload::InputAction::NONE);
}

TEST_CASE("InputMappingRegistry: GetActionForMouseButton returns NONE for "
          "unmapped button",
          "[unit][events][InputMappingRegistry]") {
  steamrot::events::InputMappingRegistry registry;

  // Check an unmapped button
  REQUIRE(registry.GetActionForMouseButton(sf::Mouse::Button::Right) ==
          steamrot::InputPayload::InputAction::NONE);
}

TEST_CASE("InputMappingRegistry: MapKeyboardKey can override existing mapping",
          "[unit][events][InputMappingRegistry]") {
  steamrot::events::InputMappingRegistry registry;

  // Override Enter key to NONE
  registry.MapKeyboardKey(sf::Keyboard::Key::Enter,
                          steamrot::InputPayload::InputAction::NONE);

  REQUIRE(registry.GetActionForKey(sf::Keyboard::Key::Enter) ==
          steamrot::InputPayload::InputAction::NONE);
}

TEST_CASE(
    "InputMappingRegistry: MapMouseButton can override existing mapping",
    "[unit][events][InputMappingRegistry]") {
  steamrot::events::InputMappingRegistry registry;

  // Override Left button to NONE
  registry.MapMouseButton(sf::Mouse::Button::Left,
                          steamrot::InputPayload::InputAction::NONE);

  REQUIRE(registry.GetActionForMouseButton(sf::Mouse::Button::Left) ==
          steamrot::InputPayload::InputAction::NONE);
}

TEST_CASE("InputMappingRegistry: UnmapKeyboardKey removes mapping",
          "[unit][events][InputMappingRegistry]") {
  steamrot::events::InputMappingRegistry registry;

  // Unmap Enter key
  registry.UnmapKeyboardKey(sf::Keyboard::Key::Enter);

  REQUIRE(registry.GetActionForKey(sf::Keyboard::Key::Enter) ==
          steamrot::InputPayload::InputAction::NONE);
}

TEST_CASE("InputMappingRegistry: UnmapMouseButton removes mapping",
          "[unit][events][InputMappingRegistry]") {
  steamrot::events::InputMappingRegistry registry;

  // Unmap Left button
  registry.UnmapMouseButton(sf::Mouse::Button::Left);

  REQUIRE(registry.GetActionForMouseButton(sf::Mouse::Button::Left) ==
          steamrot::InputPayload::InputAction::NONE);
}

TEST_CASE("InputMappingRegistry: ClearKeyboardMappings removes all keyboard "
          "mappings",
          "[unit][events][InputMappingRegistry]") {
  steamrot::events::InputMappingRegistry registry;

  registry.ClearKeyboardMappings();

  // Check that default mappings are gone
  REQUIRE(registry.GetActionForKey(sf::Keyboard::Key::Enter) ==
          steamrot::InputPayload::InputAction::NONE);
  REQUIRE(registry.GetActionForKey(sf::Keyboard::Key::Space) ==
          steamrot::InputPayload::InputAction::NONE);

  // Mouse mappings should still exist
  REQUIRE(registry.GetActionForMouseButton(sf::Mouse::Button::Left) ==
          steamrot::InputPayload::InputAction::SELECT);
}

TEST_CASE(
    "InputMappingRegistry: ClearMouseMappings removes all mouse mappings",
    "[unit][events][InputMappingRegistry]") {
  steamrot::events::InputMappingRegistry registry;

  registry.ClearMouseMappings();

  // Check that default mouse mappings are gone
  REQUIRE(registry.GetActionForMouseButton(sf::Mouse::Button::Left) ==
          steamrot::InputPayload::InputAction::NONE);

  // Keyboard mappings should still exist
  REQUIRE(registry.GetActionForKey(sf::Keyboard::Key::Enter) ==
          steamrot::InputPayload::InputAction::SELECT);
}

TEST_CASE("InputMappingRegistry: ResetToDefaults restores default mappings",
          "[unit][events][InputMappingRegistry]") {
  steamrot::events::InputMappingRegistry registry;

  // Clear all mappings
  registry.ClearKeyboardMappings();
  registry.ClearMouseMappings();

  // Verify they're gone
  REQUIRE(registry.GetActionForKey(sf::Keyboard::Key::Enter) ==
          steamrot::InputPayload::InputAction::NONE);
  REQUIRE(registry.GetActionForMouseButton(sf::Mouse::Button::Left) ==
          steamrot::InputPayload::InputAction::NONE);

  // Reset to defaults
  registry.ResetToDefaults();

  // Verify defaults are restored
  REQUIRE(registry.GetActionForKey(sf::Keyboard::Key::Enter) ==
          steamrot::InputPayload::InputAction::SELECT);
  REQUIRE(registry.GetActionForMouseButton(sf::Mouse::Button::Left) ==
          steamrot::InputPayload::InputAction::SELECT);
}

TEST_CASE("InputMappingRegistry: Multiple keys can map to same action",
          "[unit][events][InputMappingRegistry]") {
  steamrot::events::InputMappingRegistry registry;

  // Map multiple keys to SELECT
  registry.MapKeyboardKey(sf::Keyboard::Key::A,
                          steamrot::InputPayload::InputAction::SELECT);
  registry.MapKeyboardKey(sf::Keyboard::Key::B,
                          steamrot::InputPayload::InputAction::SELECT);

  REQUIRE(registry.GetActionForKey(sf::Keyboard::Key::A) ==
          steamrot::InputPayload::InputAction::SELECT);
  REQUIRE(registry.GetActionForKey(sf::Keyboard::Key::B) ==
          steamrot::InputPayload::InputAction::SELECT);
}
