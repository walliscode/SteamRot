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
  /////////////////////////////////////////////////
  /// @brief Fill color of the background
  /////////////////////////////////////////////////
  sf::Color background_color;

  /////////////////////////////////////////////////
  /// @brief Fill color of the border
  /////////////////////////////////////////////////
  sf::Color border_color;

  /////////////////////////////////////////////////
  /// @brief How thick the the element border is
  /////////////////////////////////////////////////
  float border_thickness;

  /////////////////////////////////////////////////
  /// @brief How many points a corner radius is drawn with
  /////////////////////////////////////////////////
  size_t radius_resolution;

  /////////////////////////////////////////////////
  /// @brief Distance from the outer edge of the element to its children
  /////////////////////////////////////////////////
  sf::Vector2f inner_margin;

  /////////////////////////////////////////////////
  /// @brief Minium size of the element
  /////////////////////////////////////////////////
  sf::Vector2f minimum_size;

  /////////////////////////////////////////////////
  /// @brief Maximum size of the element
  /////////////////////////////////////////////////
  sf::Vector2f maximum_size;
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

struct DropDownStyle : public Style {
  sf::Color text_color;
  sf::Color hover_color;
  std::string font;

  /////////////////////////////////////////////////
  /// @brief The ratio of the drop symbol to the dropdown width
  /////////////////////////////////////////////////
  float drop_symbol_ratio{0.2f};

  /////////////////////////////////////////////////
  /// @brief The drop symbol container fill color
  /////////////////////////////////////////////////
  sf::Color drop_symbol_container_color{
      sf::Color::Transparent}; // Color of the container for the drop symbol

  /////////////////////////////////////////////////
  /// @brief The color of the drop symbol
  /////////////////////////////////////////////////
  sf::Color drop_symbol_color{sf::Color::Black}; // Color of the drop symbol
};

class UIRenderLogic : public Logic<CUserInterface> {
private:
  /////////////////////////////////////////////////
  /// @brief Draws a Panel struct with PanelStyle styling
  ///
  /// @param element UIElement struct containing any data
  /////////////////////////////////////////////////
  void DrawPanel(UIElement &element);

  /////////////////////////////////////////////////
  /// @brief Draws a Button structu with ButtonStyle styling
  ///
  /// @param element UIElement structu containging any data
  /////////////////////////////////////////////////
  void DrawButton(UIElement &element);

  /////////////////////////////////////////////////
  /// @brief Draws a DropDown structu with DropDownStyle styling
  ///
  /// @param element UILement structure containin any data
  /////////////////////////////////////////////////
  void DrawDropDown(UIElement &element);

  ////////////////////////////////////////////////////////////
  // |brief add in styles from flatbuffer config
  ////////////////////////////////////////////////////////////
  void AddStyles(const themes::UIObjects *config);

  /////////////////////////////////////////////////
  /// @brief Configures the PanelStyle from the flatbuffer config
  ///
  /// @param panel_style Flatbuffer PanelStyle object
  /////////////////////////////////////////////////
  void ConfigurePanelStyle(const themes::PanelStyle *panel_style);

  /////////////////////////////////////////////////
  /// @brief Configures the ButtonStyle from the flatbuffer config
  ///
  /// @param button_style Flatbuffer ButtonStyle object
  /////////////////////////////////////////////////
  void ConfigureButtonStyle(const themes::ButtonStyle *button_style);

  /////////////////////////////////////////////////
  /// @brief Configures the DropDownStyle from the flatbuffer config
  ///
  /// @param dropdown_style Flatbuffer DropDownStyle object
  /////////////////////////////////////////////////
  void ConfigureDropDownStyle(const themes::DropDownStyle *dropdown_style);
  /**
   * @brief Member variable to hold the style for buttons in the UI.
   */
  ButtonStyle m_button_style;
  /**
   * @brief Member variable to hold the style for panels in the UI.
   */
  PanelStyle m_panel_style;

  /////////////////////////////////////////////////
  /// @brief Member variable tot hold the style for panels in the UI.
  /////////////////////////////////////////////////
  DropDownStyle m_dropdown_style;
  /**
   * @brief Draws all UIElements containted in the CUserInterface components.
   */
  void DrawUIElements();

  void RecursiveDrawUIElement(UIElement &element);

  /////////////////////////////////////////////////
  /// @brief Draws a box with rounded corners.
  ///
  /// Draws 4 boxes on the borders and then triangle fans for the corners.
  ///
  /// @param element Uses the UIElement to determine the styling
  /// @param origin top left corner of the box in the render texture
  /////////////////////////////////////////////////
  void DrawBoxWithRadiusCorners(UIElement &element);

  /////////////////////////////////////////////////
  /// @brief Adjusts the size of the UIElement if it beyond its maximum or
  /// minimum
  ///
  /// @param element Reference to the UIElement
  /////////////////////////////////////////////////
  void AdjustSize(UIElement &element);

  /**
   * @brief Gather all logic here, to be called by the Logic.RunLogic() function
   *
   * @param entities A reference to the EntityMemoryPool containing all
   * entities.
   * @param entity_indicies A mask of entity indiced to process the
   * EnittyMemoryPool.
   */
  void ProcessLogic() override;

public:
  ////////////////////////////////////////////////////////////
  // |brief Constructor taking in flatbuffer config
  ////////////////////////////////////////////////////////////
  UIRenderLogic(const LogicContext logic_context);

}; // namespace UIEngine
} // namespace steamrot
