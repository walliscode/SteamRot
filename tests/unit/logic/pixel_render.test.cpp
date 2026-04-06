/////////////////////////////////////////////////
/// @file
/// @brief Pixel-level tests for UI element rendering.
///
/// These tests verify that UI elements render pixel-identically to their
/// stored baseline images.  They act as a visual regression guard:
/// if the pixels change, the test fails and a diff PNG is written.
///
/// Baseline generation workflow:
///   STEAMROT_RECORD_BASELINES=1 ctest --preset Debug -L pixel
///
/// This writes new .png files to tests/data/ui_baselines/.
/// Commit the PNGs to version control as the new ground truth.
///
/// Tags:
///   [unit][pixel] — excluded from default CI runs via a hidden label;
///                   add -L pixel to a ctest invocation to run them.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ButtonElement.h"
#include "DropDownButtonElement.h"
#include "DropDownContainerElement.h"
#include "DropDownItemElement.h"
#include "DropDownListElement.h"
#include "FailInfo.h"
#include "PanelElement.h"
#include "PixelImageEqualsMatcher.h"
#include "UIStyle.h"
#include "paths.h"
#include "pixel_capture.h"
#include "render_ui.h"
#include <SFML/Graphics.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <filesystem>
#include <memory>

namespace {

/////////////////////////////////////////////////
/// @brief Canvas size used for all pixel tests.
///
/// Deliberately small to keep baseline files tiny and diffs readable.
/////////////////////////////////////////////////
constexpr unsigned int kCanvasWidth = 256;
constexpr unsigned int kCanvasHeight = 256;

/////////////////////////////////////////////////
/// @brief Background colour used in all pixel test renders.
/////////////////////////////////////////////////
const sf::Color kBackground = sf::Color{30, 30, 30, 255};

/////////////////////////////////////////////////
/// @brief Build a minimal UIStyle suitable for pixel tests.
///
/// Uses solid, well-known colours so baselines are platform-independent
/// (no font rendering involved except for elements that explicitly opt in).
/////////////////////////////////////////////////
steamrot::UIStyle BuildMinimalStyle() {
  steamrot::UIStyle style;

  // Panel
  style.panel_style.background_color = sf::Color{60, 60, 60, 255};
  style.panel_style.border_color = sf::Color{200, 200, 200, 255};
  style.panel_style.border_thickness = 2.0f;
  style.panel_style.inner_margin = {4.0f, 4.0f};

  // DropDownContainer
  style.drop_down_container_style.background_color =
      sf::Color{50, 50, 80, 255};
  style.drop_down_container_style.border_color =
      sf::Color{150, 150, 220, 255};
  style.drop_down_container_style.border_thickness = 2.0f;
  style.drop_down_container_style.drop_symbol_ratio = 0.2f;

  // DropDownList
  style.drop_down_list_style.background_color = sf::Color{70, 70, 70, 255};
  style.drop_down_list_style.border_color = sf::Color{180, 180, 180, 255};
  style.drop_down_list_style.border_thickness = 1.0f;
  style.drop_down_list_style.inner_margin = {4.0f, 4.0f};

  // DropDownButton
  style.drop_down_button_style.background_color = sf::Color{80, 80, 100, 255};
  style.drop_down_button_style.border_color = sf::Color{150, 150, 200, 255};
  style.drop_down_button_style.border_thickness = 1.0f;
  style.drop_down_button_style.inner_margin = {2.0f, 2.0f};
  style.drop_down_button_style.triangle_color = sf::Color{220, 220, 255, 255};

  // DropDownItem
  style.drop_down_item_style.background_color = sf::Color{65, 65, 65, 255};
  style.drop_down_item_style.border_color = sf::Color{160, 160, 160, 255};
  style.drop_down_item_style.border_thickness = 1.0f;
  style.drop_down_item_style.inner_margin = {4.0f, 4.0f};

  // Button (no font required for geometry-only rendering)
  style.button_style.background_color = sf::Color{80, 120, 80, 255};
  style.button_style.hover_color = sf::Color{100, 160, 100, 255};
  style.button_style.border_color = sf::Color{200, 230, 200, 255};
  style.button_style.border_thickness = 2.0f;
  style.button_style.inner_margin = {6.0f, 6.0f};
  // font is intentionally left as nullptr — text drawing is skipped when
  // no font is available; geometry tests remain font-independent

  return style;
}

/////////////////////////////////////////////////
/// @brief Render a single UIElement into a fresh 256x256 texture.
///
/// @param element Element to render.
/// @param style   UIStyle to use.
/// @return Rendered image.
/////////////////////////////////////////////////
sf::Image RenderElement(const steamrot::UIElement &element,
                        const steamrot::UIStyle &style) {
  sf::RenderTexture rt{
      sf::Vector2u{kCanvasWidth, kCanvasHeight}};
  rt.clear(kBackground);
  steamrot::logic::render::ui::DrawNestedUIElements(rt, element, style);
  rt.display();
  return steamrot::tests::CaptureImage(rt);
}

/////////////////////////////////////////////////
/// @brief Helper: run a baseline test for a named element.
///
/// In record mode, saves the rendered image as the new baseline.
/// In compare mode, loads the baseline and asserts pixel equality.
///
/// @param baseline_name  Identifier for this test case (used as filename).
/// @param element        Element to render.
/// @param style          UIStyle to use.
/////////////////////////////////////////////////
void RunBaselineTest(const std::string &baseline_name,
                     const steamrot::UIElement &element,
                     const steamrot::UIStyle &style) {
  sf::Image actual = RenderElement(element, style);

  if (steamrot::tests::IsRecordMode()) {
    auto save_result = steamrot::tests::SaveBaseline(actual, baseline_name);
    if (!save_result.has_value()) {
      FAIL("Failed to save baseline '" + baseline_name +
           "': " + save_result.error().message);
    }
    SUCCEED("Baseline recorded: " + baseline_name);
    return;
  }

  auto load_result = steamrot::tests::LoadBaseline(baseline_name);
  if (!load_result.has_value()) {
    FAIL("Baseline not found for '" + baseline_name +
         "'. Run with STEAMROT_RECORD_BASELINES=1 to generate it. Error: " +
         load_result.error().message);
    return;
  }

  // write diff PNGs next to the baselines so failures are inspectable
  std::filesystem::path diff_dir =
      steamrot::tests::GetBaselineDirectory() / "diffs";

  REQUIRE_THAT(actual,
               steamrot::tests::EqualsImage(load_result.value(), 0,
                                            baseline_name, diff_dir));
}

} // anonymous namespace

/////////////////////////////////////////////////
TEST_CASE("PanelElement renders pixel-identically to baseline",
          "[unit][pixel][PanelElement]") {

  steamrot::UIStyle style = BuildMinimalStyle();
  steamrot::PanelElement panel;
  panel.position = {28.0f, 28.0f};
  panel.size = {200.0f, 100.0f};

  RunBaselineTest("panel_default", panel, style);
}

/////////////////////////////////////////////////
TEST_CASE("DropDownContainerElement (collapsed) renders pixel-identically",
          "[unit][pixel][DropDownContainerElement]") {

  steamrot::UIStyle style = BuildMinimalStyle();

  steamrot::DropDownContainerElement container;
  container.position = {28.0f, 100.0f};
  container.size = {200.0f, 40.0f};
  container.is_expanded = false;
  container.children_active = true;

  auto dd_list = std::make_unique<steamrot::DropDownListElement>();
  dd_list->is_expanded = false;
  dd_list->unexpanded_label = "Select...";

  auto dd_button = std::make_unique<steamrot::DropDownButtonElement>();
  dd_button->is_expanded = false;

  container.child_elements.push_back(std::move(dd_list));
  container.child_elements.push_back(std::move(dd_button));

  RunBaselineTest("dropdown_container_collapsed", container, style);
}

/////////////////////////////////////////////////
TEST_CASE("DropDownContainerElement (expanded) renders pixel-identically",
          "[unit][pixel][DropDownContainerElement]") {

  steamrot::UIStyle style = BuildMinimalStyle();

  steamrot::DropDownContainerElement container;
  container.position = {28.0f, 28.0f};
  container.size = {200.0f, 40.0f};
  container.is_expanded = true;
  container.children_active = true;

  auto dd_list = std::make_unique<steamrot::DropDownListElement>();
  dd_list->is_expanded = true;
  dd_list->expanded_label = "Fragment list";

  auto dd_button = std::make_unique<steamrot::DropDownButtonElement>();
  dd_button->is_expanded = true;

  container.child_elements.push_back(std::move(dd_list));
  container.child_elements.push_back(std::move(dd_button));

  RunBaselineTest("dropdown_container_expanded", container, style);
}

/////////////////////////////////////////////////
TEST_CASE("DropDownButtonElement (collapsed) renders pixel-identically",
          "[unit][pixel][DropDownButtonElement]") {

  steamrot::UIStyle style = BuildMinimalStyle();
  steamrot::DropDownButtonElement button;
  button.position = {108.0f, 108.0f};
  button.size = {40.0f, 40.0f};
  button.is_expanded = false;

  RunBaselineTest("dropdown_button_collapsed", button, style);
}

/////////////////////////////////////////////////
TEST_CASE("DropDownButtonElement (expanded) renders pixel-identically",
          "[unit][pixel][DropDownButtonElement]") {

  steamrot::UIStyle style = BuildMinimalStyle();
  steamrot::DropDownButtonElement button;
  button.position = {108.0f, 108.0f};
  button.size = {40.0f, 40.0f};
  button.is_expanded = true;

  RunBaselineTest("dropdown_button_expanded", button, style);
}

/////////////////////////////////////////////////
TEST_CASE(
    "PanelElement with nested children renders pixel-identically to baseline",
    "[unit][pixel][PanelElement]") {

  steamrot::UIStyle style = BuildMinimalStyle();

  steamrot::PanelElement panel;
  panel.position = {28.0f, 28.0f};
  panel.size = {200.0f, 180.0f};
  panel.layout = steamrot::Layout::Vertical;
  panel.children_active = true;

  for (int i = 0; i < 3; ++i) {
    auto child = std::make_unique<steamrot::PanelElement>();
    // size/position computed by UpdateSizeAndPositionOfChildElements
    panel.child_elements.push_back(std::move(child));
  }

  RunBaselineTest("panel_with_children", panel, style);
}
