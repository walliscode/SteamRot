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

  SECTION("Text fits within the box") {
    sf::Text text(font, "Test Text", 30);
    sf::FloatRect box({0.f, 0.f}, {100.f, 20.f});
    fit_text_to_box(text, box);
    REQUIRE(text.getLocalBounds().size.x <= box.size.x);
    REQUIRE(text.getLocalBounds().size.y <= box.size.y);
  }
}

} // namespace steamrot::tests
