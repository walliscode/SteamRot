/////////////////////////////////////////////////
/// @file
/// @brief unit tests for logic_render namespace
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "render_ui.h"
#include "ColorEqualsMatcher.h"
#include "FailInfo.h"
#include "logic_render_test_helpers.h"
#include "paths.h"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/System/Vector2.hpp>
#include <catch2/catch_test_macros.hpp>
#include <expected>

namespace steamrot::tests {
namespace {
// Helper functions
std::expected<sf::Font, steamrot::FailInfo> ProvideDefaultFont() {
  sf::Font font;
  std::filesystem::path font_path =
      steamrot::paths::GetDataDirectory() /
      std::filesystem::path{"assets/fonts/Roboto-Regular.ttf"};
  if (!font.openFromFile(font_path.string())) {
    return std::unexpected(
        steamrot::FailInfo{steamrot::FailMode::FileNotFound,
                           "Failed to load font from " + font_path.string()});
  }
  return font;
}

Style ProvideDefaultStyle() {
  Style style;
  style.background_color = sf::Color::White;
  style.border_color = sf::Color::Blue;
  style.border_thickness = 2.f;
  style.radius_resolution = 5;
  style.inner_margin = {5.f, 5.f};
  style.minimum_size = {10.f, 10.f};
  style.maximum_size = {100.f, 100.f};
  style.hover_color = sf::Color::Green;

  return style;
}
} // namespace
TEST_CASE("Determine whether pixels can be tested on a RenderTexture",
          "[unit][logic_render]") {

  size_t width = 100;
  size_t height = 100;
  sf::RenderTexture render_texture{sf::Vector2u(
      {static_cast<unsigned int>(width), static_cast<unsigned int>(height)})};
  render_texture.clear(sf::Color::Black);
  // check each pixel is black
  sf::Image image_before_drawing = render_texture.getTexture().copyToImage();

  for (size_t x = 0; x < width; x++) {
    for (size_t y = 0; y < height; y++) {
      REQUIRE(image_before_drawing.getPixel({static_cast<unsigned int>(x),
                                             static_cast<unsigned int>(y)}) ==
              sf::Color::Black);
    }
  }

  // draw a white rectangle and check pixels again
  sf::RectangleShape rectangle(sf::Vector2f(50.0f, 50.0f));
  rectangle.setFillColor(sf::Color::White);
  rectangle.setPosition({25.0f, 25.0f});
  render_texture.draw(rectangle);
  sf::Image image_after_drawing = render_texture.getTexture().copyToImage();
  for (size_t x = 0; x < width; x++) {
    for (size_t y = 0; y < height; y++) {
      if (x >= 25 && x < 75 && y >= 25 && y < 75) {
        REQUIRE(image_after_drawing.getPixel({static_cast<unsigned int>(x),
                                              static_cast<unsigned int>(y)}) ==
                sf::Color::White);
      } else {
        REQUIRE(image_after_drawing.getPixel({static_cast<unsigned int>(x),
                                              static_cast<unsigned int>(y)}) ==
                sf::Color::Black);
      }
    }
  }
}
TEST_CASE("drawn text can be detected", "[unit][logic_render]") {

  // create a RenderTexture
  size_t width = 200;
  size_t height = 100;
  sf::RenderTexture render_texture{sf::Vector2u(
      {static_cast<unsigned int>(width), static_cast<unsigned int>(height)})};

  // create and load a font
  auto load_font_result = ProvideDefaultFont();
  if (!load_font_result) {
    FAIL(load_font_result.error().message);
  }

  auto font = std::make_shared<sf::Font>(load_font_result.value());

  // clear the RenderTexture
  render_texture.clear(sf::Color::Black);
  // draw some text on the RenderTexture
  std::string text = "Hello, World!";
  sf::Vector2f position = {50.0f, 25.0f};
  uint8_t font_size = 24;
  sf::Color color = sf::Color::White;
  steamrot::logic::render::ui::DrawText(render_texture, text, position,
                                        {50.f, 50.f}, font, font_size, color);
  // get the image from the RenderTexture
  sf::Image image = render_texture.getTexture().copyToImage();
  // test that some pixels in the area where the text was drawn are not black
  steamrot::tests::TestTextIsPresent(image, position, {150.f, 75.f},
                                     sf::Color::White);
}

TEST_CASE("DrawBorderAndBackground tests") {

  // arrange
  Style style = ProvideDefaultStyle();
  sf::RenderTexture render_texture{sf::Vector2u{600, 800}};
  render_texture.clear(sf::Color::Black);

  SECTION("DrawBorderAndBackground draws PanelElement with correct background "
          "and border") {
    // arrange
    PanelElement panel;
    panel.position = {100.f, 100.f};
    panel.size = {200.f, 150.f};

    // act
    steamrot::logic::render::ui::DrawBorderAndBackground(render_texture, panel,
                                                         style);

    // test pixels in the border and background areas are correct
    sf::Image image = render_texture.getTexture().copyToImage();
    REQUIRE_THAT(image.getPixel({101, 101}),
                 steamrot::tests::EqualsColor(style.border_color));
  }
}
TEST_CASE(
    "DrawDisabledOverlay paints a semi-transparent grey rectangle over the "
    "element bounds",
    "[unit][logic_render][disabled]") {
  const size_t width = 100;
  const size_t height = 100;
  sf::RenderTexture render_texture{sf::Vector2u(
      {static_cast<unsigned int>(width), static_cast<unsigned int>(height)})};

  // Fill with a known background colour (red) so we can confirm the overlay
  // blends on top
  render_texture.clear(sf::Color::Red);
  render_texture.display();

  // Create a panel that covers the entire texture
  steamrot::PanelElement panel;
  panel.position = {0.f, 0.f};
  panel.size = {static_cast<float>(width), static_cast<float>(height)};

  steamrot::logic::render::ui::DrawDisabledOverlay(render_texture, panel);
  render_texture.display();

  sf::Image image = render_texture.getTexture().copyToImage();
  sf::Color centre_pixel = image.getPixel({width / 2, height / 2});

  // The overlay is drawn over the red background. The result must not be pure
  // red (the overlay changed the pixels) and must not be pure black (the
  // background still contributes through alpha blending).
  REQUIRE(centre_pixel != sf::Color::Red);
  REQUIRE(centre_pixel != sf::Color::Black);
  // The grey overlay colour has R==G==B so the red channel should equal green
  // after blending over red (approximately).  For an exact check: the overlay
  // is Color(128,128,128,160) blended over Color(255,0,0,255).
  // Result R ≈ 128 + (255-128)*95/255 ≈ 175, G ≈ 128*(160/255) ≈ 80,
  // so R > G which we can verify.
  REQUIRE(centre_pixel.r > centre_pixel.g);
}

TEST_CASE(
    "DrawNestedUIElements draws a grey overlay on top of a disabled element",
    "[unit][logic_render][disabled]") {
  const size_t width = 100;
  const size_t height = 100;
  sf::RenderTexture render_texture{sf::Vector2u(
      {static_cast<unsigned int>(width), static_cast<unsigned int>(height)})};
  render_texture.clear(sf::Color::Black);

  // A panel that fills the whole texture, with a white background style and
  // is_disabled = true
  steamrot::PanelElement panel;
  panel.position = {0.f, 0.f};
  panel.size = {static_cast<float>(width), static_cast<float>(height)};
  panel.is_disabled = true;

  // Use a minimal UIStyle with a white panel background
  steamrot::UIStyle style;
  style.panel_style.background_color = sf::Color::White;

  steamrot::logic::render::ui::DrawNestedUIElements(render_texture, panel,
                                                    style);
  render_texture.display();

  sf::Image image = render_texture.getTexture().copyToImage();
  sf::Color centre_pixel = image.getPixel({width / 2, height / 2});

  // The white background should be visible beneath the semi-transparent overlay
  REQUIRE(centre_pixel != sf::Color::Black);
  // The overlay should have dimmed the white, so the pixel must not be pure
  // white either
  REQUIRE(centre_pixel != sf::Color::White);
}
} // namespace steamrot::tests
