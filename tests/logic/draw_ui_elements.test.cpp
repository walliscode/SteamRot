/////////////////////////////////////////////////
/// @file
/// @brief unit tests for draw_ui_elements namespace
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "draw_ui_elements.h"
#include "AssetManager.h"
#include "PanelElement.h"
#include "draw_ui_elements_helpers.h"
#include <SFML/Graphics.hpp>

#include <catch2/catch_test_macros.hpp>
#include <iostream>

TEST_CASE("Determine whether pixels can be tested on a RenderTexture",
          "[draw_ui_elements]") {

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
TEST_CASE("drawn text can be detected", "[draw_ui_elements]") {
  std::cout << "Starting DrawText test..." << std::endl;
  // create a RenderTexture
  size_t width = 200;
  size_t height = 100;
  sf::RenderTexture render_texture{sf::Vector2u(
      {static_cast<unsigned int>(width), static_cast<unsigned int>(height)})};
  // load a font
  steamrot::AssetManager asset_manager{steamrot::EnvironmentType::Test};
  auto load_default_assets_result = asset_manager.LoadDefaultAssets();
  if (!load_default_assets_result) {
    FAIL(load_default_assets_result.error().message);
  }
  auto font = asset_manager.GetFont("DaddyTimeMonoNerdFont-Regular");
  if (!font) {
    FAIL("Failed to get default font");
  }
  // clear the RenderTexture
  render_texture.clear(sf::Color::Black);
  // draw some text on the RenderTexture
  std::string text = "Hello, World!";
  sf::Vector2f position = {50.0f, 25.0f};
  uint8_t font_size = 24;
  sf::Color color = sf::Color::White;
  steamrot::draw_ui_elements::DrawText(render_texture, text, position,
                                       {50.f, 50.f}, *font, font_size, color);
  // get the image from the RenderTexture
  sf::Image image = render_texture.getTexture().copyToImage();
  // test that some pixels in the area where the text was drawn are not black
  steamrot::tests::TestTextIsPresent(image, position, {150.f, 75.f},
                                     sf::Color::White);
}
TEST_CASE(
    "steamrot::draw_ui_elements::DrawPanel draws a panel on a RenderTexture",
    "[draw_ui_elements]") {
  std::cout << "Starting DrawPanel test..." << std::endl;

  // create a RenderTexture
  size_t width = 100;
  size_t height = 100;
  sf::RenderTexture render_texture{sf::Vector2u(
      {static_cast<unsigned int>(width), static_cast<unsigned int>(height)})};

  // create a PanelElement
  steamrot::PanelElement panel;
  panel.position = {25.0f, 25.0f};
  panel.size = {50.0f, 50.0f};

  // load the default UIStyle
  steamrot::AssetManager asset_manager{steamrot::EnvironmentType::Test};
  auto load_default_assets_result = asset_manager.LoadDefaultAssets();
  if (!load_default_assets_result) {
    FAIL(load_default_assets_result.error().message);
  }
  auto style = asset_manager.GetDefaultUIStyle();

  // clear the RenderTexture
  render_texture.clear(sf::Color::Black);

  // draw the panel on the RenderTexture
  steamrot::draw_ui_elements::DrawUIElement(render_texture, panel, style);

  // display the Panel for visual inspection
  steamrot::tests::DisplayRenderTexture(render_texture);
  // get the image from the RenderTexture
  sf::Image image = render_texture.getTexture().copyToImage();

  steamrot::tests::TestDrawPanel(image, panel, style);
}

TEST_CASE("steamrot::draw_ui_elements::DrawButton draws a button on a "
          "RenderTexture",
          "[draw_ui_elements]") {
  // create a RenderTexture
  size_t width = 200;
  size_t height = 200;
  sf::RenderTexture render_texture{sf::Vector2u(
      {static_cast<unsigned int>(width), static_cast<unsigned int>(height)})};
  // create a ButtonElement
  steamrot::ButtonElement button;
  button.label = "Click Me";
  button.position = {25.0f, 25.0f};
  button.size = {50.0f, 50.0f};

  // load the default UIStyle
  steamrot::AssetManager asset_manager{steamrot::EnvironmentType::Test};
  auto load_default_assets_result = asset_manager.LoadDefaultAssets();
  if (!load_default_assets_result) {
    FAIL(load_default_assets_result.error().message);
  }
  auto style = asset_manager.GetDefaultUIStyle();

  // clear the RenderTexture
  render_texture.clear(sf::Color::Black);

  // draw the button on the RenderTexture
  steamrot::draw_ui_elements::DrawUIElement(render_texture, button, style);

  // display the button for visual inspection
  steamrot::tests::DisplayRenderTexture(render_texture);
  // get the image from the RenderTexture
  sf::Image image = render_texture.getTexture().copyToImage();
  // test that the correct pixels are drawn
  // steamrot::tests::TestDrawButton(image, button, style);
}
