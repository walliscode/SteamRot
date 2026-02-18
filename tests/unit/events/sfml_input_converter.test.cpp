/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for SFML input converter functions
/////////////////////////////////////////////////

#include "sfml_input_converter.h"
#include "InputMappingRegistry.h"
#include <SFML/Window/Event.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConvertSFMLEventToInputPayload: Converts Enter key press to SELECT",
          "[unit][events][sfml_input_converter]") {
  steamrot::events::InputMappingRegistry registry;

  sf::Event event;
  event = sf::Event::KeyPressed{sf::Keyboard::Key::Enter, false, false, false,
                                 false, false};

  auto result =
      steamrot::events::ConvertSFMLEventToInputPayload(event, registry);

  REQUIRE(result.has_value());
  REQUIRE(result->action == steamrot::InputPayload::InputAction::SELECT);
}

TEST_CASE("ConvertSFMLEventToInputPayload: Converts Space key press to SELECT",
          "[unit][events][sfml_input_converter]") {
  steamrot::events::InputMappingRegistry registry;

  sf::Event event;
  event = sf::Event::KeyPressed{sf::Keyboard::Key::Space, false, false, false,
                                 false, false};

  auto result =
      steamrot::events::ConvertSFMLEventToInputPayload(event, registry);

  REQUIRE(result.has_value());
  REQUIRE(result->action == steamrot::InputPayload::InputAction::SELECT);
}

TEST_CASE("ConvertSFMLEventToInputPayload: Converts left mouse button press "
          "to SELECT",
          "[unit][events][sfml_input_converter]") {
  steamrot::events::InputMappingRegistry registry;

  sf::Event event;
  event = sf::Event::MouseButtonPressed{sf::Mouse::Button::Left, {100, 200}};

  auto result =
      steamrot::events::ConvertSFMLEventToInputPayload(event, registry);

  REQUIRE(result.has_value());
  REQUIRE(result->action == steamrot::InputPayload::InputAction::SELECT);
}

TEST_CASE("ConvertSFMLEventToInputPayload: Returns nullopt for unmapped key",
          "[unit][events][sfml_input_converter]") {
  steamrot::events::InputMappingRegistry registry;

  sf::Event event;
  event = sf::Event::KeyPressed{sf::Keyboard::Key::Z, false, false, false,
                                 false, false};

  auto result =
      steamrot::events::ConvertSFMLEventToInputPayload(event, registry);

  REQUIRE_FALSE(result.has_value());
}

TEST_CASE(
    "ConvertSFMLEventToInputPayload: Returns nullopt for unmapped mouse button",
    "[unit][events][sfml_input_converter]") {
  steamrot::events::InputMappingRegistry registry;

  sf::Event event;
  event = sf::Event::MouseButtonPressed{sf::Mouse::Button::Right, {100, 200}};

  auto result =
      steamrot::events::ConvertSFMLEventToInputPayload(event, registry);

  REQUIRE_FALSE(result.has_value());
}

TEST_CASE("ConvertSFMLEventToInputPayload: Returns nullopt for non-input "
          "events",
          "[unit][events][sfml_input_converter]") {
  steamrot::events::InputMappingRegistry registry;

  // Test with a Closed event
  sf::Event event;
  event = sf::Event::Closed{};

  auto result =
      steamrot::events::ConvertSFMLEventToInputPayload(event, registry);

  REQUIRE_FALSE(result.has_value());
}

TEST_CASE("ConvertSFMLEventToInputPayload: Uses custom mappings from registry",
          "[unit][events][sfml_input_converter]") {
  steamrot::events::InputMappingRegistry registry;

  // Add custom mapping
  registry.MapKeyboardKey(sf::Keyboard::Key::A,
                          steamrot::InputPayload::InputAction::SELECT);

  sf::Event event;
  event = sf::Event::KeyPressed{sf::Keyboard::Key::A, false, false, false,
                                 false, false};

  auto result =
      steamrot::events::ConvertSFMLEventToInputPayload(event, registry);

  REQUIRE(result.has_value());
  REQUIRE(result->action == steamrot::InputPayload::InputAction::SELECT);
}

TEST_CASE(
    "ConvertSFMLEventToInputPayload: Respects overridden default mappings",
    "[unit][events][sfml_input_converter]") {
  steamrot::events::InputMappingRegistry registry;

  // Override Enter key to NONE
  registry.MapKeyboardKey(sf::Keyboard::Key::Enter,
                          steamrot::InputPayload::InputAction::NONE);

  sf::Event event;
  event = sf::Event::KeyPressed{sf::Keyboard::Key::Enter, false, false, false,
                                 false, false};

  auto result =
      steamrot::events::ConvertSFMLEventToInputPayload(event, registry);

  // Should return nullopt because action is NONE
  REQUIRE_FALSE(result.has_value());
}

TEST_CASE("ConvertSFMLEventToInputPayload: Single-parameter version uses "
          "default mappings",
          "[unit][events][sfml_input_converter]") {
  sf::Event event;
  event = sf::Event::KeyPressed{sf::Keyboard::Key::Enter, false, false, false,
                                 false, false};

  // Use single-parameter version (creates temporary registry with defaults)
  auto result = steamrot::events::ConvertSFMLEventToInputPayload(event);

  REQUIRE(result.has_value());
  REQUIRE(result->action == steamrot::InputPayload::InputAction::SELECT);
}

TEST_CASE("ConvertSFMLEventToInputPayload: Handles key release events "
          "properly",
          "[unit][events][sfml_input_converter]") {
  steamrot::events::InputMappingRegistry registry;

  sf::Event event;
  event = sf::Event::KeyReleased{sf::Keyboard::Key::Enter, false, false, false,
                                  false, false};

  auto result =
      steamrot::events::ConvertSFMLEventToInputPayload(event, registry);

  // Key release events are not converted (only KeyPressed events)
  REQUIRE_FALSE(result.has_value());
}

TEST_CASE("ConvertSFMLEventToInputPayload: Handles mouse button release "
          "events properly",
          "[unit][events][sfml_input_converter]") {
  steamrot::events::InputMappingRegistry registry;

  sf::Event event;
  event = sf::Event::MouseButtonReleased{sf::Mouse::Button::Left, {100, 200}};

  auto result =
      steamrot::events::ConvertSFMLEventToInputPayload(event, registry);

  // Mouse release events are not converted (only MouseButtonPressed events)
  REQUIRE_FALSE(result.has_value());
}

TEST_CASE("ConvertSFMLEventToInputPayload: Multiple conversions with same "
          "registry",
          "[unit][events][sfml_input_converter]") {
  steamrot::events::InputMappingRegistry registry;

  // First conversion
  sf::Event event1;
  event1 = sf::Event::KeyPressed{sf::Keyboard::Key::Enter, false, false, false,
                                  false, false};
  auto result1 =
      steamrot::events::ConvertSFMLEventToInputPayload(event1, registry);

  // Second conversion
  sf::Event event2;
  event2 = sf::Event::MouseButtonPressed{sf::Mouse::Button::Left, {100, 200}};
  auto result2 =
      steamrot::events::ConvertSFMLEventToInputPayload(event2, registry);

  // Both should succeed
  REQUIRE(result1.has_value());
  REQUIRE(result1->action == steamrot::InputPayload::InputAction::SELECT);
  REQUIRE(result2.has_value());
  REQUIRE(result2->action == steamrot::InputPayload::InputAction::SELECT);
}
