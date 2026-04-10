/////////////////////////////////////////////////
/// @file
/// @brief unit tests for logic_render namespace
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "render_ui.h"
#include "FailInfo.h"
#include "logic_render_test_helpers.h"
#include "paths.h"
#include <SFML/Graphics.hpp>
#include <catch2/catch_test_macros.hpp>
#include <expected>

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

// TEST_CASE("steamrot::logic_render::DrawBorderAndBackground draws the hover "
//           "color for a button",
//
//           "[unit][logic_render]") {
//
//   // create a RenderTexture
//   size_t width = 100;
//   size_t height = 100;
//   sf::RenderTexture render_texture{sf::Vector2u(
//       {static_cast<unsigned int>(width), static_cast<unsigned
//       int>(height)})};
//   // create a ButtonElement
//   steamrot::ButtonElement button;
//   button.position = {25.0f, 25.0f};
//   button.size = {50.0f, 50.0f};
//   button.is_mouse_over = true;
//
//   // load the default UIStyle
//   steamrot::AssetManager asset_manager;
//   auto load_default_assets_result = asset_manager.LoadDefaultAssets();
//   if (!load_default_assets_result) {
//     FAIL(load_default_assets_result.error().message);
//   }
//   auto style = asset_manager.GetDefaultUIStyle();
//   // clear the RenderTexture
//   render_texture.clear(sf::Color::Black);
//   // draw the button on the RenderTexture
//   steamrot::logic::render::ui::DrawBorderAndBackground(render_texture, button,
//                                                    style.button_style);
//
//   // get the image from the RenderTexture
//   sf::Image image = render_texture.getTexture().copyToImage();
//   // test that the correct pixels are drawn
//   auto pixel_color = image.getPixel({50, 50});
//   REQUIRE(pixel_color == style.button_style.hover_color);
// }

// TEST_CASE("steamrot::logic_render::DrawUIELement draws a panel on a "
//           "RenderTexture",
//           "[unit][logic_render][.visual]") {
//
//   // create a RenderTexture
//   size_t width = 100;
//   size_t height = 100;
//   sf::RenderTexture render_texture{sf::Vector2u(
//       {static_cast<unsigned int>(width), static_cast<unsigned
//       int>(height)})};
//
//   // create a PanelElement
//   steamrot::PanelElement panel;
//   panel.position = {25.0f, 25.0f};
//   panel.size = {50.0f, 50.0f};
//
//   // load the default UIStyle
//   steamrot::AssetManager asset_manager;
//   auto load_default_assets_result = asset_manager.LoadDefaultAssets();
//   if (!load_default_assets_result) {
//     FAIL(load_default_assets_result.error().message);
//   }
//   auto style = asset_manager.GetDefaultUIStyle();
//
//   // clear the RenderTexture
//   render_texture.clear(sf::Color::Black);
//
//   // draw the panel on the RenderTexture
//   steamrot::logic::render::ui::DrawPanelElement(render_texture, panel, style);
//
//   // display the Panel for visual inspection
//   steamrot::tests::DisplayRenderTexture(render_texture);
//   // get the image from the RenderTexture
//   sf::Image image = render_texture.getTexture().copyToImage();
//
//   steamrot::tests::TestDrawPanel(image, panel, style);
// }
//
// TEST_CASE("steamrot::logic_render::DrawUIELement draws a ButtonElement on a "
//           "RenderTexture",
//           "[unit][logic_render][.visual]") {
//
//   // create a RenderTexture
//   size_t width = 200;
//   size_t height = 200;
//   sf::RenderTexture render_texture{sf::Vector2u(
//       {static_cast<unsigned int>(width), static_cast<unsigned
//       int>(height)})};
//   // create a ButtonElement
//   steamrot::ButtonElement button;
//   button.label = "Click Me";
//   button.position = {25.0f, 25.0f};
//   button.size = {50.0f, 50.0f};
//
//   // load the default UIStyle
//   steamrot::AssetManager asset_manager;
//   auto load_default_assets_result = asset_manager.LoadDefaultAssets();
//   if (!load_default_assets_result) {
//     FAIL(load_default_assets_result.error().message);
//   }
//   auto style = asset_manager.GetDefaultUIStyle();
//
//   // clear the RenderTexture
//   render_texture.clear(sf::Color::Black);
//
//   // draw the button on the RenderTexture
//   steamrot::logic::render::ui::DrawButtonElement(render_texture, button, style);
//
//   // display the button for visual inspection
//   steamrot::tests::DisplayRenderTexture(render_texture);
//   // get the image from the RenderTexture
//   sf::Image image = render_texture.getTexture().copyToImage();
//   // test that the correct pixels are drawn
//   // steamrot::tests::TestDrawButton(image, button, style);
// }
//
// TEST_CASE("steamrot::logic_render::DrawUIElement draws a "
//           "DropdownContainerElement "
//           "on a RenderTexture",
//           "[unit][logic_render][.visual]") {
//
//   // create a RenderTexture
//   size_t width = 200;
//   size_t height = 200;
//   sf::RenderTexture render_texture{sf::Vector2u(
//       {static_cast<unsigned int>(width), static_cast<unsigned
//       int>(height)})};
//
//   // create a DropDownContainerElement
//   steamrot::DropDownContainerElement dd_container;
//   dd_container.position = {25.0f, 25.0f};
//   dd_container.size = {150.0f, 50.0f};
//   dd_container.is_expanded = false;
//
//   // load the default UIStyle
//   steamrot::AssetManager asset_manager;
//   auto load_default_assets_result = asset_manager.LoadDefaultAssets();
//   if (!load_default_assets_result) {
//     FAIL(load_default_assets_result.error().message);
//   }
//
//   auto style = asset_manager.GetDefaultUIStyle();
//
//   // clear the RenderTexture
//   render_texture.clear(sf::Color::Black);
//
//   // draw the DropDownContainerElement on the RenderTexture
//   steamrot::logic::render::ui::DrawDropDownContainerElement(render_texture,
//                                                         dd_container, style);
//
//   // display the button for visual
//   // inspection
//   steamrot::tests::DisplayRenderTexture(render_texture);
// }
// TEST_CASE("steamrot::logic_render::DrawUIElement draws an unexpanded "
//           "DropDownListElement "
//           "on a RenderTexture",
//           "[unit][logic_render][.visual]") {
//   // create a RenderTexture
//   size_t width = 200;
//   size_t height = 200;
//   sf::RenderTexture render_texture{sf::Vector2u(
//       {static_cast<unsigned int>(width), static_cast<unsigned
//       int>(height)})};
//   // create a DropDownListElement
//   steamrot::DropDownListElement dd_list;
//   dd_list.position = {25.0f, 25.0f};
//   dd_list.size = {100.0f, 25.0f};
//   dd_list.is_expanded = false;
//   dd_list.unexpanded_label = "...open up";
//   dd_list.expanded_label = "...select";
//   // load the default UIStyle
//   steamrot::AssetManager asset_manager;
//   auto load_default_assets_result = asset_manager.LoadDefaultAssets();
//   if (!load_default_assets_result) {
//     FAIL(load_default_assets_result.error().message);
//   }
//   auto style = asset_manager.GetDefaultUIStyle();
//   // clear the RenderTexture
//   render_texture.clear(sf::Color::Black);
//   // draw the DropDownListElement on the RenderTexture
//   steamrot::logic::render::ui::DrawDropDownListElement(render_texture, dd_list,
//                                                    style);
//   // display the button for visual inspection
//   steamrot::tests::DisplayRenderTexture(render_texture);
// }
// TEST_CASE("steamrot::logic_render::DrawUIElement draws an expanded "
//           "DropDownListElement "
//           "on a RenderTexture",
//           "[unit][logic_render][.visual]") {
//   // create a RenderTexture
//   size_t width = 200;
//   size_t height = 200;
//   sf::RenderTexture render_texture{sf::Vector2u(
//       {static_cast<unsigned int>(width), static_cast<unsigned
//       int>(height)})};
//   // create a DropDownListElement
//   steamrot::DropDownListElement dd_list;
//   dd_list.position = {25.0f, 25.0f};
//   dd_list.size = {100.0f, 100.0f};
//   dd_list.is_expanded = true;
//   dd_list.unexpanded_label = "...open up";
//   dd_list.expanded_label = "...select";
//   // load the default UIStyle
//   steamrot::AssetManager asset_manager;
//   auto load_default_assets_result = asset_manager.LoadDefaultAssets();
//   if (!load_default_assets_result) {
//     FAIL(load_default_assets_result.error().message);
//   }
//   auto style = asset_manager.GetDefaultUIStyle();
//   // clear the RenderTexture
//   render_texture.clear(sf::Color::Black);
//   // draw the DropDownListElement on the RenderTexture
//   steamrot::logic::render::ui::DrawDropDownListElement(render_texture, dd_list,
//                                                    style);
//   // display the button for visual inspection
//   steamrot::tests::DisplayRenderTexture(render_texture);
// }
// TEST_CASE("steamrot::logic_render::DrawUIElement draws an unexpanded "
//           "DropDownButtonElement on a RenderTexture",
//           "[unit][logic_render][.visual]") {
//   // create a RenderTexture
//   size_t width = 200;
//   size_t height = 200;
//   sf::RenderTexture render_texture{sf::Vector2u(
//       {static_cast<unsigned int>(width), static_cast<unsigned
//       int>(height)})};
//   // create a DropDownButtonElement
//   steamrot::DropDownButtonElement dd_button;
//
//   dd_button.position = {25.0f, 25.0f};
//   dd_button.size = {100.0f, 100.0f};
//   // load the default UIStyle
//   steamrot::AssetManager asset_manager;
//   auto load_default_assets_result = asset_manager.LoadDefaultAssets();
//   if (!load_default_assets_result) {
//     FAIL(load_default_assets_result.error().message);
//   }
//   auto style = asset_manager.GetDefaultUIStyle();
//   // clear the RenderTexture
//   render_texture.clear(sf::Color::Black);
//
//   // draw the button on the RenderTexture
//   steamrot::logic::render::ui::DrawDropDownButtonElement(render_texture,
//   dd_button,
//                                                      style);
//   // display the button for visual inspection
//   steamrot::tests::DisplayRenderTexture(render_texture);
// }
//
// TEST_CASE("steamrot::logic_render::DrawUIElement draws an expanded "
//           "DropdownButtonElement "
//           "on a RenderTexture",
//           "[unit][logic_render][.visual]") {
//   // create a RenderTexture
//   size_t width = 200;
//   size_t height = 200;
//   sf::RenderTexture render_texture{sf::Vector2u(
//       {static_cast<unsigned int>(width), static_cast<unsigned
//       int>(height)})};
//   // create a DropDownButtonElement
//   steamrot::DropDownButtonElement dd_button;
//   dd_button.position = {25.0f, 25.0f};
//   dd_button.size = {100.0f, 100.0f};
//   dd_button.is_expanded = true;
//   // load the default UIStyle
//   steamrot::AssetManager asset_manager;
//   auto load_default_assets_result = asset_manager.LoadDefaultAssets();
//   if (!load_default_assets_result) {
//     FAIL(load_default_assets_result.error().message);
//   }
//   auto style = asset_manager.GetDefaultUIStyle();
//   // clear the RenderTexture
//   render_texture.clear(sf::Color::Black);
//
//   // draw the button on the RenderTexture
//   steamrot::logic::render::ui::DrawDropDownButtonElement(render_texture,
//   dd_button,
//                                                      style);
//
//   // display the button for visual inspection
//   steamrot::tests::DisplayRenderTexture(render_texture);
// }
//
// TEST_CASE("steamrot::logic_render::DrawNestedUIElements draws a unexpanded "
//           "drop down setup",
//           "[unit][logic_render][.visual]") {
//   // create a RenderTexture
//   size_t width = 200;
//   size_t height = 200;
//   sf::RenderTexture render_texture{sf::Vector2u(
//       {static_cast<unsigned int>(width), static_cast<unsigned
//       int>(height)})};
//   // create a DropDownContainerElement
//   steamrot::DropDownContainerElement dd_container;
//   dd_container.position = {25.0f, 25.0f};
//   dd_container.size = {150.0f, 25.0f};
//   dd_container.is_expanded = false;
//   dd_container.children_active = true;
//   dd_container.layout = steamrot::LayoutType::LayoutType_Horizontal;
//   // create a DropDownListElement
//   std::unique_ptr<steamrot::DropDownListElement> dd_list =
//       std::make_unique<steamrot::DropDownListElement>();
//   dd_list->position = {0.0f, 50.0f};
//   dd_list->size = {150.0f, 100.0f};
//   dd_list->is_expanded = false;
//
//   // add the DropDownListElement to the DropDownContainerElement
//   // as a child
//   dd_container.child_elements.push_back(std::move(dd_list));
//   // create a DropDownButtonElement
//   std::unique_ptr<steamrot::DropDownButtonElement> dd_button =
//       std::make_unique<steamrot::DropDownButtonElement>();
//   dd_button->position = {0.0f, 0.0f};
//   dd_button->size = {150.0f, 50.0f};
//   dd_button->is_expanded = false;
//   // add the DropDownButtonElement to the DropDownContainerElement
//   // as a child
//   dd_container.child_elements.push_back(std::move(dd_button));
//   // load the default UIStyle
//   steamrot::AssetManager asset_manager;
//   auto load_default_assets_result = asset_manager.LoadDefaultAssets();
//   if (!load_default_assets_result) {
//     FAIL(load_default_assets_result.error().message);
//   }
//   auto style = asset_manager.GetDefaultUIStyle();
//   // clear the RenderTexture
//   render_texture.clear(sf::Color::Black);
//   // draw the DropDownContainerElement and its children on the RenderTexture
//   steamrot::logic::render::ui::DrawNestedUIElements(render_texture, dd_container,
//                                                 style);
//   // display the button for visual inspection
//   steamrot::tests::DisplayRenderTexture(render_texture);
// }
//
// TEST_CASE("steamrot::logic_render::DrawNestedUIElements draws an expanded "
//           "drop down setup",
//           "[unit][logic_render][.visual]") {
//   // create a RenderTexture
//   size_t width = 200;
//   size_t height = 200;
//   sf::RenderTexture render_texture{sf::Vector2u(
//       {static_cast<unsigned int>(width), static_cast<unsigned
//       int>(height)})};
//   // create a DropDownContainerElement
//   steamrot::DropDownContainerElement dd_container;
//   dd_container.position = {25.0f, 25.0f};
//   dd_container.size = {150.0f, 100.0f};
//   dd_container.is_expanded = true;
//   dd_container.children_active = true;
//   dd_container.layout = steamrot::LayoutType::LayoutType_Horizontal;
//   // create a DropDownListElement
//   std::unique_ptr<steamrot::DropDownListElement> dd_list =
//       std::make_unique<steamrot::DropDownListElement>();
//   dd_list->position = {0.0f, 50.0f};
//   dd_list->size = {150.0f, 100.0f};
//   dd_list->is_expanded = true;
//   // add the DropDownListElement to the DropDownContainerElement
//   // as a child
//   dd_container.child_elements.push_back(std::move(dd_list));
//   // create a DropDownButtonElement
//   std::unique_ptr<steamrot::DropDownButtonElement> dd_button =
//       std::make_unique<steamrot::DropDownButtonElement>();
//   dd_button->position = {0.0f, 0.0f};
//   dd_button->size = {150.0f, 50.0f};
//   dd_button->is_expanded = false;
//   // add the DropDownButtonElement to the DropDownContainerElement
//   // as a child
//   dd_container.child_elements.push_back(std::move(dd_button));
//
//   // add three DropDownL
//   // load the default UIStyle
//   steamrot::AssetManager asset_manager;
//   auto load_default_assets_result = asset_manager.LoadDefaultAssets();
//   if (!load_default_assets_result) {
//     FAIL(load_default_assets_result.error().message);
//   }
//   auto style = asset_manager.GetDefaultUIStyle();
//   // clear the RenderTexture
//   render_texture.clear(sf::Color::Black);
// }

// ---------------------------------------------------------------------------
// is_disabled rendering tests
// ---------------------------------------------------------------------------

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
