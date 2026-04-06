/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the UIExplorerScene class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UIExplorerScene.h"
#include "ButtonElement.h"
#include "DropDownButtonElement.h"
#include "DropDownContainerElement.h"
#include "DropDownItemElement.h"
#include "DropDownListElement.h"
#include "PanelElement.h"
#include "collision_mouse.h"
#include "paths.h"
#include "render_ui.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cctype>
#include <chrono>
#include <filesystem>
#include <format>
#include <iostream>

namespace steamrot {

/////////////////////////////////////////////////
UIExplorerScene::UIExplorerScene(const GameContext &game_context)
    : Scene(game_context) {
  BuildCatalog();
  LoadFont();
  SelectElement(0);
}

/////////////////////////////////////////////////
void UIExplorerScene::BuildCatalog() {
  m_catalog.push_back(
      {"PanelElement", []() -> std::unique_ptr<UIElement> {
         auto el = std::make_unique<PanelElement>();
         el->position = {0.0f, 0.0f};
         el->size = {200.0f, 150.0f};
         return el;
       }});

  m_catalog.push_back(
      {"ButtonElement", []() -> std::unique_ptr<UIElement> {
         auto el = std::make_unique<ButtonElement>();
         el->position = {0.0f, 0.0f};
         el->size = {160.0f, 50.0f};
         el->label = "Click Me";
         return el;
       }});

  m_catalog.push_back(
      {"DropDownContainerElement (collapsed)",
       []() -> std::unique_ptr<UIElement> {
         auto container = std::make_unique<DropDownContainerElement>();
         container->position = {0.0f, 0.0f};
         container->size = {220.0f, 45.0f};
         container->is_expanded = false;
         container->children_active = true;

         auto list = std::make_unique<DropDownListElement>();
         list->is_expanded = false;
         list->unexpanded_label = "Select item...";

         auto btn = std::make_unique<DropDownButtonElement>();
         btn->is_expanded = false;

         container->child_elements.push_back(std::move(list));
         container->child_elements.push_back(std::move(btn));
         return container;
       }});

  m_catalog.push_back(
      {"DropDownContainerElement (expanded)",
       []() -> std::unique_ptr<UIElement> {
         auto container = std::make_unique<DropDownContainerElement>();
         container->position = {0.0f, 0.0f};
         container->size = {220.0f, 45.0f};
         container->is_expanded = true;
         container->children_active = true;

         auto list = std::make_unique<DropDownListElement>();
         list->is_expanded = true;
         list->expanded_label = "Select item...";
         list->children_active = true;

         for (int i = 0; i < 3; ++i) {
           auto item = std::make_unique<DropDownItemElement>();
           item->label = std::format("Item {}", i + 1);
           item->value = std::format("value_{}", i + 1);
           list->child_elements.push_back(std::move(item));
         }

         auto btn = std::make_unique<DropDownButtonElement>();
         btn->is_expanded = true;

         container->child_elements.push_back(std::move(list));
         container->child_elements.push_back(std::move(btn));
         return container;
       }});

  m_catalog.push_back(
      {"DropDownListElement", []() -> std::unique_ptr<UIElement> {
         auto el = std::make_unique<DropDownListElement>();
         el->position = {0.0f, 0.0f};
         el->size = {180.0f, 40.0f};
         el->is_expanded = false;
         el->unexpanded_label = "Select...";
         el->expanded_label = "Choose:";
         return el;
       }});

  m_catalog.push_back(
      {"DropDownButtonElement", []() -> std::unique_ptr<UIElement> {
         auto el = std::make_unique<DropDownButtonElement>();
         el->position = {0.0f, 0.0f};
         el->size = {45.0f, 45.0f};
         el->is_expanded = false;
         return el;
       }});

  m_catalog.push_back(
      {"DropDownItemElement", []() -> std::unique_ptr<UIElement> {
         auto el = std::make_unique<DropDownItemElement>();
         el->position = {0.0f, 0.0f};
         el->size = {200.0f, 40.0f};
         el->label = "Example Item";
         el->value = "example_value";
         return el;
       }});
}

/////////////////////////////////////////////////
void UIExplorerScene::LoadFont() {
  // try AssetManager first (fonts should be loaded during Engine::StartUp)
  const auto &all_fonts =
      m_scene_resources.game_context.asset_manager.GetAllFonts();
  if (!all_fonts.empty()) {
    m_font = all_fonts.begin()->second;
    return;
  }

  // fall back to loading directly from the file system
  std::filesystem::path font_path =
      paths::GetFontsDirectory() / "Roboto-Regular.ttf";
  auto font = std::make_shared<sf::Font>();
  if (font->openFromFile(font_path.string())) {
    m_font = std::move(font);
  }
}

/////////////////////////////////////////////////
void UIExplorerScene::SelectElement(size_t index) {
  if (m_catalog.empty()) {
    return;
  }
  m_selected_index = index % m_catalog.size();
  m_active_element = m_catalog[m_selected_index].factory();
  m_canvas_offset = {0.0f, 0.0f};

  // try to refresh style from AssetManager
  try {
    m_style = m_scene_resources.game_context.asset_manager.GetDefaultUIStyle();
  } catch (...) {
    m_style = std::nullopt;
  }
}

/////////////////////////////////////////////////
sf::Vector2f
UIExplorerScene::ComputeElementPosition(const sf::Vector2f &element_size) const {
  // canvas area spans x: kSelectorWidth → kSceneWidth,
  //                   y: 0             → kSceneHeight - kHudHeight
  float canvas_x = kSelectorWidth;
  float canvas_w = kSceneWidth - kSelectorWidth;
  float canvas_h = kSceneHeight - kHudHeight;

  float centre_x = canvas_x + (canvas_w - element_size.x) * 0.5f;
  float centre_y = (canvas_h - element_size.y) * 0.5f;

  return {centre_x + m_canvas_offset.x, centre_y + m_canvas_offset.y};
}

/////////////////////////////////////////////////
sf::FloatRect UIExplorerScene::SelectorEntryRect(size_t entry_index) const {
  float y = kEntryPadding + static_cast<float>(entry_index) *
                                (kEntryHeight + kEntryPadding);
  // leave a small horizontal margin
  return {{8.0f, y}, {kSelectorWidth - 16.0f, kEntryHeight}};
}

/////////////////////////////////////////////////
void UIExplorerScene::sMovement() {
  // no movement logic
}

/////////////////////////////////////////////////
void UIExplorerScene::sCollision() {
  if (!m_active_element) {
    return;
  }
  const sf::Vector2i &mouse = m_scene_resources.game_context.mouse_position;

  // update hover state for the active element in the canvas
  logic::collision::mouse::CheckMouseOver(mouse, *m_active_element);
}

/////////////////////////////////////////////////
void UIExplorerScene::sAction() {
  // --- keyboard: Escape = quit / close window ---
  bool esc_now = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape);
  if (esc_now && !m_esc_was_pressed) {
    m_scene_resources.game_context.game_window.close();
  }
  m_esc_was_pressed = esc_now;

  // --- mouse click detection (rising edge) ---
  bool mouse_now_pressed =
      sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
  bool mouse_clicked = mouse_now_pressed && !m_mouse_was_pressed;
  m_mouse_was_pressed = mouse_now_pressed;

  if (mouse_clicked) {
    const sf::Vector2i &mouse_i = m_scene_resources.game_context.mouse_position;
    sf::Vector2f mouse_f{static_cast<float>(mouse_i.x),
                         static_cast<float>(mouse_i.y)};

    for (size_t i = 0; i < m_catalog.size(); ++i) {
      if (SelectorEntryRect(i).contains(mouse_f)) {
        SelectElement(i);
        break;
      }
    }
  }

  // --- keyboard: Left/Right arrows = navigate catalog ---
  bool left_now = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
  if (left_now && !m_left_was_pressed && !m_catalog.empty()) {
    size_t next = (m_selected_index == 0) ? m_catalog.size() - 1
                                           : m_selected_index - 1;
    SelectElement(next);
  }
  m_left_was_pressed = left_now;

  bool right_now = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);
  if (right_now && !m_right_was_pressed) {
    SelectElement(m_selected_index + 1);
  }
  m_right_was_pressed = right_now;

  // --- keyboard: R = reset current element ---
  bool r_now = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R);
  if (r_now && !m_r_was_pressed) {
    SelectElement(m_selected_index);
  }
  m_r_was_pressed = r_now;

  // --- keyboard: S = save canvas PNG ---
  bool s_now = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
  if (s_now && !m_s_was_pressed) {
    SaveCanvasToPng();
  }
  m_s_was_pressed = s_now;

  // --- arrow keys: pan the canvas ---
  constexpr float kPanStep = 4.0f;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
    m_canvas_offset.y -= kPanStep;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
    m_canvas_offset.y += kPanStep;
  }
}

/////////////////////////////////////////////////
void UIExplorerScene::sRender() {
  m_scene_resources.scene_texture.clear(sf::Color{20, 20, 20, 255});

  DrawSelectorPanel();
  DrawCanvas();
  DrawHud();

  m_scene_resources.scene_texture.display();
}

/////////////////////////////////////////////////
void UIExplorerScene::DrawSelectorPanel() {
  sf::RenderTexture &tex = m_scene_resources.scene_texture;

  // selector panel background
  sf::RectangleShape panel_bg(
      {kSelectorWidth, kSceneHeight - kHudHeight});
  panel_bg.setPosition({0.0f, 0.0f});
  panel_bg.setFillColor(sf::Color{35, 35, 45, 255});
  panel_bg.setOutlineColor(sf::Color{70, 70, 90, 255});
  panel_bg.setOutlineThickness(-1.0f);
  tex.draw(panel_bg);

  // title
  DrawText("UI Explorer", {10.0f, 8.0f}, sf::Color{220, 220, 255, 255}, 14u);

  // entry buttons
  for (size_t i = 0; i < m_catalog.size(); ++i) {
    sf::FloatRect rect = SelectorEntryRect(i);

    sf::RectangleShape entry_bg(rect.size);
    entry_bg.setPosition(rect.position);

    if (i == m_selected_index) {
      entry_bg.setFillColor(sf::Color{60, 100, 160, 255});
      entry_bg.setOutlineColor(sf::Color{120, 160, 220, 255});
    } else {
      entry_bg.setFillColor(sf::Color{50, 50, 65, 255});
      entry_bg.setOutlineColor(sf::Color{80, 80, 100, 255});
    }
    entry_bg.setOutlineThickness(-1.0f);
    tex.draw(entry_bg);

    DrawText(m_catalog[i].name,
             {rect.position.x + 8.0f,
              rect.position.y + (rect.size.y - 16.0f) * 0.5f},
             sf::Color{210, 210, 240, 255}, 13u);
  }
}

/////////////////////////////////////////////////
void UIExplorerScene::DrawCanvas() {
  if (!m_active_element) {
    return;
  }

  sf::RenderTexture &tex = m_scene_resources.scene_texture;

  // canvas background
  sf::RectangleShape canvas_bg(
      {kSceneWidth - kSelectorWidth, kSceneHeight - kHudHeight});
  canvas_bg.setPosition({kSelectorWidth, 0.0f});
  canvas_bg.setFillColor(sf::Color{28, 28, 32, 255});
  tex.draw(canvas_bg);

  // subtle grid for spatial reference
  sf::Color grid_color{40, 40, 50, 255};
  constexpr float kGridSpacing = 50.0f;
  for (float x = kSelectorWidth; x < kSceneWidth; x += kGridSpacing) {
    sf::RectangleShape line({1.0f, kSceneHeight - kHudHeight});
    line.setPosition({x, 0.0f});
    line.setFillColor(grid_color);
    tex.draw(line);
  }
  for (float y = 0.0f; y < kSceneHeight - kHudHeight; y += kGridSpacing) {
    sf::RectangleShape line({kSceneWidth - kSelectorWidth, 1.0f});
    line.setPosition({kSelectorWidth, y});
    line.setFillColor(grid_color);
    tex.draw(line);
  }

  // position element at centre of canvas + pan offset
  sf::Vector2f element_pos = ComputeElementPosition(m_active_element->size);
  m_active_element->position = element_pos;

  // render the element using the stored UIStyle
  if (m_style.has_value()) {
    logic::render::ui::DrawNestedUIElements(tex, *m_active_element,
                                           m_style.value());
  } else {
    // fall back to a minimal inline style if AssetManager had no style
    UIStyle fallback_style;
    logic::render::ui::DrawNestedUIElements(tex, *m_active_element,
                                           fallback_style);
  }
}

/////////////////////////////////////////////////
void UIExplorerScene::DrawHud() {
  sf::RenderTexture &tex = m_scene_resources.scene_texture;

  // HUD background
  sf::RectangleShape hud_bg({kSceneWidth, kHudHeight});
  hud_bg.setPosition({0.0f, kSceneHeight - kHudHeight});
  hud_bg.setFillColor(sf::Color{20, 20, 30, 240});
  hud_bg.setOutlineColor(sf::Color{60, 60, 80, 255});
  hud_bg.setOutlineThickness(-1.0f);
  tex.draw(hud_bg);

  // element name
  std::string element_name = m_catalog.empty()
                                 ? "(none)"
                                 : m_catalog[m_selected_index].name;
  DrawText("Element: " + element_name, {8.0f, kSceneHeight - kHudHeight + 6.0f},
           sf::Color{220, 220, 180, 255}, 14u);

  // mouse position
  const sf::Vector2i &mouse = m_scene_resources.game_context.mouse_position;
  std::string mouse_str =
      std::format("Mouse: ({},{})", mouse.x, mouse.y);
  DrawText(mouse_str, {500.0f, kSceneHeight - kHudHeight + 6.0f},
           sf::Color{180, 210, 180, 255}, 14u);

  // keyboard hints
  DrawText(
      "← → Navigate   R Reset   S Save PNG   ↑↓ Pan   Esc Quit",
      {820.0f, kSceneHeight - kHudHeight + 6.0f},
      sf::Color{160, 160, 180, 255}, 12u);
}

/////////////////////////////////////////////////
void UIExplorerScene::DrawText(const std::string &text, sf::Vector2f position,
                               sf::Color color, unsigned int character_size) {
  if (!m_font) {
    return;
  }
  sf::Text label(*m_font, text, character_size);
  label.setFillColor(color);
  label.setPosition(position);
  m_scene_resources.scene_texture.draw(label);
}

/////////////////////////////////////////////////
void UIExplorerScene::SaveCanvasToPng() {
  // build a timestamped filename in the user saves directory
  std::filesystem::path save_dir =
      paths::GetSavesDirectory() / "sandbox_captures";
  std::filesystem::create_directories(save_dir);

  // build a UTC-based timestamp using C++20 chrono (thread-safe, no localtime)
  auto now = std::chrono::system_clock::now();
  auto seconds = std::chrono::floor<std::chrono::seconds>(now);
  auto dp = std::chrono::floor<std::chrono::days>(seconds);
  std::chrono::year_month_day ymd{dp};
  std::chrono::hh_mm_ss<std::chrono::seconds> hms{seconds - dp};

  std::string timestamp =
      std::format("{:04}{:02}{:02}_{:02}{:02}{:02}",
                  static_cast<int>(ymd.year()),
                  static_cast<unsigned>(ymd.month()),
                  static_cast<unsigned>(ymd.day()),
                  static_cast<long long>(hms.hours().count()),
                  static_cast<long long>(hms.minutes().count()),
                  static_cast<long long>(hms.seconds().count()));

  std::string element_name =
      m_catalog.empty() ? "element" : m_catalog[m_selected_index].name;
  // sanitize element_name: keep only alphanumeric characters and underscores
  for (char &c : element_name) {
    if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_') {
      c = '_';
    }
  }

  std::filesystem::path save_path =
      save_dir / (element_name + "_" + timestamp + ".png");

  sf::Image capture = m_scene_resources.scene_texture.getTexture().copyToImage();
  if (capture.saveToFile(save_path.string())) {
    std::cout << "UIExplorerScene: saved canvas PNG to '"
              << save_path.string() << "'\n";
  } else {
    std::cerr << "UIExplorerScene: failed to save canvas PNG to '"
              << save_path.string() << "'\n";
  }
}

} // namespace steamrot
