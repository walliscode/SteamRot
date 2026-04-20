/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the UIExplorerScene class.
///
/// UIExplorerScene is a developer-tool scene that provides an interactive
/// environment for viewing and testing individual UI elements in isolation.
///
/// Layout (1280 x 720):
///   ┌──────────────┬──────────────────────────────────────────────────────┐
///   │ Selector     │ Isolation Canvas                                     │
///   │ panel        │                                                      │
///   │ (x 0–280)    │ (x 280–1280)                                        │
///   │              │  • selected element rendered at its natural size     │
///   │              │  • mouse-over highlights are live                    │
///   └──────────────┴──────────────────────────────────────────────────────┘
///   ┌──────────────────────────────────────────────────────────────────────┐
///   │ HUD bar (bottom 40 px): element name, mouse pos, keyboard hints     │
///   └──────────────────────────────────────────────────────────────────────┘
///
/// Controls:
///   Mouse click on selector entry → select that element
///   Left / Right arrow keys       → previous / next element (with debounce)
///   R key                         → reset current element to defaults
///   S key                         → save canvas render to PNG
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GameContext.h"
#include "Scene.h"
#include "UIElement.h"
#include "UIStyle.h"
#include <SFML/Graphics.hpp>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
/// @struct ElementCatalogEntry
/// @brief A named factory for a UI element to display in the sandbox.
/////////////////////////////////////////////////
struct ElementCatalogEntry {
  /////////////////////////////////////////////////
  /// @brief Display name shown in the selector panel.
  /////////////////////////////////////////////////
  std::string name;

  /////////////////////////////////////////////////
  /// @brief Factory function that creates a fresh element instance.
  /////////////////////////////////////////////////
  std::function<std::unique_ptr<UIElement>()> factory;
};

/////////////////////////////////////////////////
/// @class UIExplorerScene
/// @brief Interactive sandbox scene for inspecting UI elements.
/////////////////////////////////////////////////
class UIExplorerScene : public Scene {
  friend class SceneFactory;

private:
  /////////////////////////////////////////////////
  /// @brief Width of the left selector panel in pixels.
  /////////////////////////////////////////////////
  static constexpr float kSelectorWidth = 280.0f;

  /////////////////////////////////////////////////
  /// @brief Height of the bottom HUD bar in pixels.
  /////////////////////////////////////////////////
  static constexpr float kHudHeight = 40.0f;

  /////////////////////////////////////////////////
  /// @brief Scene canvas width and height (must match texture dimensions).
  /////////////////////////////////////////////////
  static constexpr float kSceneWidth = 800.0f;
  static constexpr float kSceneHeight = 600.0f;

  /////////////////////////////////////////////////
  /// @brief Vertical padding between selector entries.
  /////////////////////////////////////////////////
  static constexpr float kEntryPadding = 8.0f;

  /////////////////////////////////////////////////
  /// @brief Height of each selector entry button.
  /////////////////////////////////////////////////
  static constexpr float kEntryHeight = 42.0f;

  /////////////////////////////////////////////////
  /// @brief Registered element catalog.
  /////////////////////////////////////////////////
  std::vector<ElementCatalogEntry> m_catalog;

  /////////////////////////////////////////////////
  /// @brief Index of the currently selected catalog entry.
  /////////////////////////////////////////////////
  size_t m_selected_index{0};

  /////////////////////////////////////////////////
  /// @brief Current element instance being displayed.
  /////////////////////////////////////////////////
  std::unique_ptr<UIElement> m_active_element;

  /////////////////////////////////////////////////
  /// @brief UIStyle used for rendering the active element.
  /////////////////////////////////////////////////
  std::optional<UIStyle> m_style;

  /////////////////////////////////////////////////
  /// @brief Font used for HUD and selector text.
  /////////////////////////////////////////////////
  std::shared_ptr<const sf::Font> m_font;

  /////////////////////////////////////////////////
  /// @brief Canvas offset for panning (arrow keys).
  /////////////////////////////////////////////////
  sf::Vector2f m_canvas_offset{0.0f, 0.0f};

  /////////////////////////////////////////////////
  /// @brief Previous left-mouse-button pressed state for click detection.
  /////////////////////////////////////////////////
  bool m_mouse_was_pressed{false};

  /////////////////////////////////////////////////
  /// @brief Whether the Escape key was pressed last tick (for debounce).
  /////////////////////////////////////////////////
  bool m_esc_was_pressed{false};

  /////////////////////////////////////////////////
  /// @brief Whether the R key was pressed last tick (for debounce).
  /////////////////////////////////////////////////
  bool m_r_was_pressed{false};

  /////////////////////////////////////////////////
  /// @brief Whether the S key was pressed last tick (for debounce).
  /////////////////////////////////////////////////
  bool m_s_was_pressed{false};

  /////////////////////////////////////////////////
  /// @brief Whether the Left arrow was pressed last tick (for debounce).
  /////////////////////////////////////////////////
  bool m_left_was_pressed{false};

  /////////////////////////////////////////////////
  /// @brief Whether the Right arrow was pressed last tick (for debounce).
  /////////////////////////////////////////////////
  bool m_right_was_pressed{false};

  /////////////////////////////////////////////////
  /// @brief Populate m_catalog with one entry per concrete UIElement type.
  /////////////////////////////////////////////////
  void BuildCatalog();

  /////////////////////////////////////////////////
  /// @brief Load m_font from the asset manager (or fall back to file path).
  /////////////////////////////////////////////////
  void LoadFont();

  /////////////////////////////////////////////////
  /// @brief Replace m_active_element with a fresh instance from the catalog.
  ///
  /// @param index Catalog entry index.
  /////////////////////////////////////////////////
  void SelectElement(size_t index);

  /////////////////////////////////////////////////
  /// @brief Compute the canvas-space position for the active element.
  ///
  /// The element is centred in the canvas area, then m_canvas_offset applied.
  ///
  /// @param element_size Natural size of the element before positioning.
  /// @return Top-left position for the element in scene (window) coordinates.
  /////////////////////////////////////////////////
  sf::Vector2f ComputeElementPosition(const sf::Vector2f &element_size) const;

  /////////////////////////////////////////////////
  /// @brief Return the screen rect for a selector entry.
  ///
  /// @param entry_index Zero-based index of the entry.
  /////////////////////////////////////////////////
  sf::FloatRect SelectorEntryRect(size_t entry_index) const;

  /////////////////////////////////////////////////
  /// @brief Draw the left selector panel.
  /////////////////////////////////////////////////
  void DrawSelectorPanel();

  /////////////////////////////////////////////////
  /// @brief Draw the isolation canvas with the active element.
  /////////////////////////////////////////////////
  void DrawCanvas();

  /////////////////////////////////////////////////
  /// @brief Draw the bottom HUD bar.
  /////////////////////////////////////////////////
  void DrawHud();

  /////////////////////////////////////////////////
  /// @brief Draw a single line of text to the scene texture.
  /////////////////////////////////////////////////
  void DrawText(const std::string &text, sf::Vector2f position, sf::Color color,
                unsigned int character_size = 16u);

  /////////////////////////////////////////////////
  /// @brief Save the current canvas render to a PNG file.
  /////////////////////////////////////////////////
  void SaveCanvasToPng();

  UIExplorerScene(const GameContext &game_context);

  void sRender() override;
  void sPositioning() override;
  void sCollision() override;
  void sAction() override;
};

} // namespace steamrot
