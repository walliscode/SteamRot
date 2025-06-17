////////////////////////////////////////////////////////////
// Preprocessor directives
////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "BaseLogic.h"
#include "CUserInterface.h"
#include "Logic.h"
#include "themes_generated.h"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
namespace steamrot {

struct Style {
  sf::Color background_color;
  sf::Color border_color;
  float border_thickness;
  size_t radius_resolution;
  sf::Vector2f inner_margin;
};
/**
 * @class PanelStyle
 * @brief Contains the style properties for a panel in the UI.
 *
 */
struct PanelStyle : public Style {};

/**
 * @class ButtonStyle
 * @brief Contains the style properties for a button in the UI.
 *
 */
struct ButtonStyle : public Style {
  sf::Color text_color;
  sf::Color hover_color;
  std::string font;
};

class UIRenderLogic : public Logic<CUserInterface> {
private:
  void DrawPanel(const UIElement &element, const sf::Vector2f &origin,
                 const sf::Vector2f &size);

  ////////////////////////////////////////////////////////////
  // |brief draw drop down menu
  ////////////////////////////////////////////////////////////
  void DrawDropDownMenu();

  /////////////////////////////////////////////////
  /// @brief Draw a button element to the render texture.
  ///
  /// @param element Struct provided by the CUserInterface component that stores
  /// element specific data.
  /// @param origin Top left corner of the button in the render texture
  /// @param size Total size of the button including borders.
  /////////////////////////////////////////////////
  void DrawButton(const Button &element, const sf::Vector2f &origin,
                  const sf::Vector2f &size);

  ////////////////////////////////////////////////////////////
  // |brief add in styles from flatbuffer config
  ////////////////////////////////////////////////////////////
  void AddStyles(const themes::UIObjects *config);

  /**
   * @brief Member variable to hold the style for buttons in the UI.
   */
  ButtonStyle m_button_style;
  /**
   * @brief Member variable to hold the style for panels in the UI.
   */
  PanelStyle m_panel_style;

  /**
   * @brief Draws all UIElements containted in the CUserInterface components.
   */
  void DrawUIElements();

  void RecursiveDrawUIElement(UIElement &element, const sf::Vector2f &origin,
                              const sf::Vector2f &size);
  /////////////////////////////////////////////////
  /// @brief Draws a box with rounded corners.
  ///
  /// Draws 4 boxes on the borders and then triangle fans for the corners.
  ///
  /// @param element Uses the UIElement to determine the styling
  /// @param origin top left corner of the box in the render texture
  /////////////////////////////////////////////////
  void DrawBoxWithRadiusCorners(const UIElement &element,
                                const sf::Vector2f &origin,
                                const sf::Vector2f &size,
                                const size_t &resolution);

public:
  ////////////////////////////////////////////////////////////
  // |brief Constructor taking in flatbuffer config
  ////////////////////////////////////////////////////////////
  UIRenderLogic(const LogicContext logic_context);

  /**
   * @brief Gather all logic here, to be called by the Logic.RunLogic() function
   *
   * @param entities A reference to the EntityMemoryPool containing all
   * entities.
   * @param entity_indicies A mask of entity indiced to process the
   * EnittyMemoryPool.
   */
  void ProcessLogic() override;

}; // namespace UIEngine
} // namespace steamrot
