/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for new logic_render free functions (Option 6a)
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "logic_render.h"
#include "AssetManager.h"
#include "ButtonElement.h"
#include "DropDownButtonElement.h"
#include "DropDownContainerElement.h"
#include "DropDownItemElement.h"
#include "DropDownListElement.h"
#include "PanelElement.h"
#include "logic_render_test_helpers.h"
#include <SFML/Graphics.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>

TEST_CASE("DrawButtonElement draws a button correctly", "[unit][logic_render]") {
  // Create a RenderTexture
  size_t width = 200;
  size_t height = 100;
  sf::RenderTexture render_texture{sf::Vector2u(
      {static_cast<unsigned int>(width), static_cast<unsigned int>(height)})};
  
  // Create a ButtonElement
  steamrot::ButtonElement button;
  button.position = {50.0f, 25.0f};
  button.size = {100.0f, 50.0f};
  button.label = "Test Button";
  
  // Load default assets
  steamrot::AssetManager asset_manager;
  auto load_result = asset_manager.LoadDefaultAssets();
  REQUIRE(load_result.has_value());
  
  auto style = asset_manager.GetDefaultUIStyle();
  
  // Clear and draw
  render_texture.clear(sf::Color::Black);
  steamrot::logic::render::DrawButtonElement(render_texture, button, style);
  
  // Verify something was drawn (pixels changed from black)
  sf::Image image = render_texture.getTexture().copyToImage();
  bool found_non_black = false;
  for (size_t x = 50; x < 150 && !found_non_black; x++) {
    for (size_t y = 25; y < 75 && !found_non_black; y++) {
      if (image.getPixel({static_cast<unsigned int>(x),
                         static_cast<unsigned int>(y)}) != sf::Color::Black) {
        found_non_black = true;
      }
    }
  }
  REQUIRE(found_non_black);
}

TEST_CASE("DrawPanelElement draws a panel correctly", "[unit][logic_render]") {
  // Create a RenderTexture
  size_t width = 200;
  size_t height = 100;
  sf::RenderTexture render_texture{sf::Vector2u(
      {static_cast<unsigned int>(width), static_cast<unsigned int>(height)})};
  
  // Create a PanelElement
  steamrot::PanelElement panel;
  panel.position = {50.0f, 25.0f};
  panel.size = {100.0f, 50.0f};
  
  // Load default assets
  steamrot::AssetManager asset_manager;
  auto load_result = asset_manager.LoadDefaultAssets();
  REQUIRE(load_result.has_value());
  
  auto style = asset_manager.GetDefaultUIStyle();
  
  // Clear and draw
  render_texture.clear(sf::Color::Black);
  steamrot::logic::render::DrawPanelElement(render_texture, panel, style);
  
  // Verify something was drawn
  sf::Image image = render_texture.getTexture().copyToImage();
  bool found_non_black = false;
  for (size_t x = 50; x < 150 && !found_non_black; x++) {
    for (size_t y = 25; y < 75 && !found_non_black; y++) {
      if (image.getPixel({static_cast<unsigned int>(x),
                         static_cast<unsigned int>(y)}) != sf::Color::Black) {
        found_non_black = true;
      }
    }
  }
  REQUIRE(found_non_black);
}

TEST_CASE("DrawDropDownListElement draws unexpanded list correctly",
          "[unit][logic_render]") {
  // Create a RenderTexture
  size_t width = 200;
  size_t height = 100;
  sf::RenderTexture render_texture{sf::Vector2u(
      {static_cast<unsigned int>(width), static_cast<unsigned int>(height)})};
  
  // Create a DropDownListElement
  steamrot::DropDownListElement list;
  list.position = {50.0f, 25.0f};
  list.size = {100.0f, 50.0f};
  list.is_expanded = false;
  list.unexpanded_label = "Select...";
  list.expanded_label = "Options";
  
  // Load default assets
  steamrot::AssetManager asset_manager;
  auto load_result = asset_manager.LoadDefaultAssets();
  REQUIRE(load_result.has_value());
  
  auto style = asset_manager.GetDefaultUIStyle();
  
  // Clear and draw
  render_texture.clear(sf::Color::Black);
  steamrot::logic::render::DrawDropDownListElement(render_texture, list, style);
  
  // Verify something was drawn
  sf::Image image = render_texture.getTexture().copyToImage();
  bool found_non_black = false;
  for (size_t x = 50; x < 150 && !found_non_black; x++) {
    for (size_t y = 25; y < 75 && !found_non_black; y++) {
      if (image.getPixel({static_cast<unsigned int>(x),
                         static_cast<unsigned int>(y)}) != sf::Color::Black) {
        found_non_black = true;
      }
    }
  }
  REQUIRE(found_non_black);
}

TEST_CASE("DrawDropDownItemElement draws item correctly",
          "[unit][logic_render]") {
  // Create a RenderTexture
  size_t width = 200;
  size_t height = 100;
  sf::RenderTexture render_texture{sf::Vector2u(
      {static_cast<unsigned int>(width), static_cast<unsigned int>(height)})};
  
  // Create a DropDownItemElement
  steamrot::DropDownItemElement item;
  item.position = {50.0f, 25.0f};
  item.size = {100.0f, 50.0f};
  item.label = "Item 1";
  item.value = "value1";
  
  // Load default assets
  steamrot::AssetManager asset_manager;
  auto load_result = asset_manager.LoadDefaultAssets();
  REQUIRE(load_result.has_value());
  
  auto style = asset_manager.GetDefaultUIStyle();
  
  // Clear and draw
  render_texture.clear(sf::Color::Black);
  steamrot::logic::render::DrawDropDownItemElement(render_texture, item, style);
  
  // Verify something was drawn
  sf::Image image = render_texture.getTexture().copyToImage();
  bool found_non_black = false;
  for (size_t x = 50; x < 150 && !found_non_black; x++) {
    for (size_t y = 25; y < 75 && !found_non_black; y++) {
      if (image.getPixel({static_cast<unsigned int>(x),
                         static_cast<unsigned int>(y)}) != sf::Color::Black) {
        found_non_black = true;
      }
    }
  }
  REQUIRE(found_non_black);
}

TEST_CASE("DrawDropDownButtonElement draws button correctly",
          "[unit][logic_render]") {
  // Create a RenderTexture
  size_t width = 200;
  size_t height = 100;
  sf::RenderTexture render_texture{sf::Vector2u(
      {static_cast<unsigned int>(width), static_cast<unsigned int>(height)})};
  
  // Create a DropDownButtonElement
  steamrot::DropDownButtonElement button;
  button.position = {50.0f, 25.0f};
  button.size = {100.0f, 50.0f};
  button.is_expanded = false;
  
  // Load default assets
  steamrot::AssetManager asset_manager;
  auto load_result = asset_manager.LoadDefaultAssets();
  REQUIRE(load_result.has_value());
  
  auto style = asset_manager.GetDefaultUIStyle();
  
  // Clear and draw
  render_texture.clear(sf::Color::Black);
  steamrot::logic::render::DrawDropDownButtonElement(render_texture, button, style);
  
  // Verify something was drawn
  sf::Image image = render_texture.getTexture().copyToImage();
  bool found_non_black = false;
  for (size_t x = 50; x < 150 && !found_non_black; x++) {
    for (size_t y = 25; y < 75 && !found_non_black; y++) {
      if (image.getPixel({static_cast<unsigned int>(x),
                         static_cast<unsigned int>(y)}) != sf::Color::Black) {
        found_non_black = true;
      }
    }
  }
  REQUIRE(found_non_black);
}

TEST_CASE("DrawDropDownContainerElement draws container correctly",
          "[unit][logic_render]") {
  // Create a RenderTexture
  size_t width = 200;
  size_t height = 100;
  sf::RenderTexture render_texture{sf::Vector2u(
      {static_cast<unsigned int>(width), static_cast<unsigned int>(height)})};
  
  // Create a DropDownContainerElement
  steamrot::DropDownContainerElement container;
  container.position = {50.0f, 25.0f};
  container.size = {100.0f, 50.0f};
  container.is_expanded = false;
  
  // Load default assets
  steamrot::AssetManager asset_manager;
  auto load_result = asset_manager.LoadDefaultAssets();
  REQUIRE(load_result.has_value());
  
  auto style = asset_manager.GetDefaultUIStyle();
  
  // Clear and draw
  render_texture.clear(sf::Color::Black);
  steamrot::logic::render::DrawDropDownContainerElement(render_texture, container, style);
  
  // Verify something was drawn
  sf::Image image = render_texture.getTexture().copyToImage();
  bool found_non_black = false;
  for (size_t x = 50; x < 150 && !found_non_black; x++) {
    for (size_t y = 25; y < 75 && !found_non_black; y++) {
      if (image.getPixel({static_cast<unsigned int>(x),
                         static_cast<unsigned int>(y)}) != sf::Color::Black) {
        found_non_black = true;
      }
    }
  }
  REQUIRE(found_non_black);
}

TEST_CASE("DrawUIElementDispatch dispatches to correct drawing function",
          "[unit][logic_render]") {
  // Create a RenderTexture
  size_t width = 200;
  size_t height = 100;
  sf::RenderTexture render_texture{sf::Vector2u(
      {static_cast<unsigned int>(width), static_cast<unsigned int>(height)})};
  
  // Load default assets
  steamrot::AssetManager asset_manager;
  auto load_result = asset_manager.LoadDefaultAssets();
  REQUIRE(load_result.has_value());
  
  auto style = asset_manager.GetDefaultUIStyle();
  
  SECTION("Dispatches ButtonElement correctly") {
    steamrot::ButtonElement button;
    button.position = {50.0f, 25.0f};
    button.size = {100.0f, 50.0f};
    button.label = "Test";
    
    render_texture.clear(sf::Color::Black);
    steamrot::logic::render::DrawUIElementDispatch(render_texture, button, style);
    
    // Verify something was drawn
    sf::Image image = render_texture.getTexture().copyToImage();
    bool found_non_black = false;
    for (size_t x = 50; x < 150 && !found_non_black; x++) {
      for (size_t y = 25; y < 75 && !found_non_black; y++) {
        if (image.getPixel({static_cast<unsigned int>(x),
                           static_cast<unsigned int>(y)}) != sf::Color::Black) {
          found_non_black = true;
        }
      }
    }
    REQUIRE(found_non_black);
  }
  
  SECTION("Dispatches PanelElement correctly") {
    steamrot::PanelElement panel;
    panel.position = {50.0f, 25.0f};
    panel.size = {100.0f, 50.0f};
    
    render_texture.clear(sf::Color::Black);
    steamrot::logic::render::DrawUIElementDispatch(render_texture, panel, style);
    
    // Verify something was drawn
    sf::Image image = render_texture.getTexture().copyToImage();
    bool found_non_black = false;
    for (size_t x = 50; x < 150 && !found_non_black; x++) {
      for (size_t y = 25; y < 75 && !found_non_black; y++) {
        if (image.getPixel({static_cast<unsigned int>(x),
                           static_cast<unsigned int>(y)}) != sf::Color::Black) {
          found_non_black = true;
        }
      }
    }
    REQUIRE(found_non_black);
  }
}

TEST_CASE("DrawNestedUIElements uses dispatcher correctly",
          "[unit][logic_render]") {
  // Create a RenderTexture
  size_t width = 200;
  size_t height = 200;
  sf::RenderTexture render_texture{sf::Vector2u(
      {static_cast<unsigned int>(width), static_cast<unsigned int>(height)})};
  
  // Create a PanelElement with ButtonElement child
  steamrot::PanelElement panel;
  panel.position = {25.0f, 25.0f};
  panel.size = {150.0f, 150.0f};
  panel.children_active = true;
  panel.layout = steamrot::LayoutType::LayoutType_Vertical;
  
  auto button = std::make_unique<steamrot::ButtonElement>();
  button->label = "Child Button";
  panel.child_elements.push_back(std::move(button));
  
  // Load default assets
  steamrot::AssetManager asset_manager;
  auto load_result = asset_manager.LoadDefaultAssets();
  REQUIRE(load_result.has_value());
  
  auto style = asset_manager.GetDefaultUIStyle();
  
  // Clear and draw
  render_texture.clear(sf::Color::Black);
  steamrot::logic::render::DrawNestedUIElements(render_texture, panel, style);
  
  // Verify something was drawn (both panel and child)
  sf::Image image = render_texture.getTexture().copyToImage();
  bool found_non_black = false;
  for (size_t x = 25; x < 175 && !found_non_black; x++) {
    for (size_t y = 25; y < 175 && !found_non_black; y++) {
      if (image.getPixel({static_cast<unsigned int>(x),
                         static_cast<unsigned int>(y)}) != sf::Color::Black) {
        found_non_black = true;
      }
    }
  }
  REQUIRE(found_non_black);
}
