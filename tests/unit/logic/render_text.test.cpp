/////////////////////////////////////////////////
/// @file
/// @brief unit tests for the free functions related to the rendering of text in
/// the game.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "render_text.h"
#include "test_fonts.h"
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

using namespace logic::render;

TEST_CASE("fit_text_to_box tests", "[render_text]") {

  // arrange
  auto font_result = ProvideDefaultFont();
  if (!font_result) {
    FAIL("Failed to load font for testing: " + font_result.error().message);
  }

  sf::Font &font = font_result.value();

  SECTION("Text is scaled down to fit within constraints") {
    sf::Text text(font, "Test Text", 60);
    // pull out the original local bounds before scaling down
    const sf::FloatRect original_bounds = text.getLocalBounds();
    // check that local bounds are larger than the box size to ensure scaling
    // down is needed
    REQUIRE(original_bounds.size.x > 200.f);
    REQUIRE(original_bounds.size.y > 25.f);

    sf::Vector2f box_size(200.f, 25.f);
    fit_text_to_box(text, box_size);
    REQUIRE(text.getLocalBounds().size.x <= box_size.x);
    REQUIRE(text.getLocalBounds().size.y <= box_size.y);
  }

  SECTION("Text is scaled up to fit within constraints") {
    sf::Text text(font, "Test Text", 10);

    // store the original local bounds before scaling up
    const sf::FloatRect original_bounds = text.getLocalBounds();
    REQUIRE(original_bounds.size.x < 200.f);
    REQUIRE(original_bounds.size.y < 50.f);

    sf::Vector2f box_size(200.f, 50.f);
    fit_text_to_box(text, box_size, 0, true);
    REQUIRE(text.getLocalBounds().size.x <= box_size.x);
    REQUIRE(text.getLocalBounds().size.y <= box_size.y);
    REQUIRE(text.getLocalBounds().size.x > original_bounds.size.x);
    REQUIRE(text.getLocalBounds().size.y > original_bounds.size.y);
  }

  SECTION("Text scaling does not go below minimum size") {
    sf::Text text(font, "Test Text", 4);
    // store the original local bounds before scaling down
    const sf::FloatRect original_bounds = text.getLocalBounds();

    sf::Vector2f box_size(1.f, 1.f);
    fit_text_to_box(text, box_size);
    REQUIRE(text.getCharacterSize() == 2);
  }
}

} // namespace steamrot::tests
