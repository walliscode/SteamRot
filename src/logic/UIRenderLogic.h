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
#include "user_interface_generated.h"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>

namespace steamrot {

using SpacingStrategy =
    std::function<void(UIElement &ui_element, sf::Vector2f &inner_margin,
                       float &parent_border_thickness)>;
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

/////////////////////////////////////////////////
/// @class DropDownContainerStyle
/// @brief Structure containing the style properties for a dropdown container
///
/////////////////////////////////////////////////
struct DropDownContainerStyle : public Style {

  /////////////////////////////////////////////////
  /// @brief The ratio of the drop symbol to the dropdown width
  /////////////////////////////////////////////////
  float drop_symbol_ratio{0.2f};
};

struct DropDownListStyle : public Style {
  /////////////////////////////////////////////////
  /// @brief Text color of the dropdown list
  /////////////////////////////////////////////////
  sf::Color text_color;

  /////////////////////////////////////////////////
  /// @brief Background color of the dropdown list on hover
  /////////////////////////////////////////////////
  sf::Color hover_color;

  /////////////////////////////////////////////////
  /// @brief Font used for the dropdown list
  /////////////////////////////////////////////////
  std::string font;

  /////////////////////////////////////////////////
  /// @brief Font size used for the dropdown list
  /////////////////////////////////////////////////
  size_t font_size{12};
};

struct DropDownItemStyle : public Style {
  /////////////////////////////////////////////////
  /// @brief Text color of the dropdown item
  /////////////////////////////////////////////////
  sf::Color text_color;
  /////////////////////////////////////////////////
  /// @brief Background color of the dropdown item on hover
  /////////////////////////////////////////////////
  sf::Color hover_color;
  /////////////////////////////////////////////////
  /// @brief Font used for the dropdown item
  /////////////////////////////////////////////////
  std::string font;
};

struct DropDownButtonStyle : public Style {
  /////////////////////////////////////////////////
  /// @brief Coloer of the triangle in the dropdown button
  /////////////////////////////////////////////////
  sf::Color triangle_color;

  /////////////////////////////////////////////////
  /// @brief Background color of the dropdown button on hover
  /////////////////////////////////////////////////
  sf::Color hover_color;
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
  void DrawDropDownContainer(UIElement &element);

  /////////////////////////////////////////////////
  /// @brief Draws the top element of a DropDownList (not the items themselves)
  ///
  /// @param element UIElement structure containing any data
  /////////////////////////////////////////////////
  void DrawDropDownList(UIElement &element);

  /////////////////////////////////////////////////
  /// @brief Draws a DropDownItem with DropDownItemStyle styling
  ///
  /// @param element UIElement structure containing any data
  /////////////////////////////////////////////////
  void DrawDropDownItem(UIElement &element);

  /////////////////////////////////////////////////
  /// @brief Draws a DropDownButton with DropDownButtonStyle styling
  ///
  /// @param element UIElement structure containing any data
  /////////////////////////////////////////////////
  void DrawDropDownButton(UIElement &element);

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
  void ConfigureDropDownContainerStyle(
      const themes::DropDownContainerStyle *dropdown_style);

  /////////////////////////////////////////////////
  /// @brief Configures the DropDownListStyle from the flatbuffer config
  ///
  /// @param dropdown_list_style Flatbuffer DropDownListStyle object
  /////////////////////////////////////////////////
  void ConfigureDropDownListStyle(
      const themes::DropDownListStyle *dropdown_list_style);

  /////////////////////////////////////////////////
  /// @brief Configures the DropDownItemStyle from the flatbuffer config
  ///
  /// @param dropdown_item_style Flatbuffer DropDownItemStyle object
  /////////////////////////////////////////////////
  void ConfigureDropDownItemStyle(
      const themes::DropDownItemStyle *dropdown_item_style);

  /////////////////////////////////////////////////
  /// @brief Configures the DropDownButtonStyle from the flatbuffer config
  ///
  /// @param dropdown_button_style DropDownButtonStyle object
  /////////////////////////////////////////////////
  void ConfigureDropDownButtonStyle(
      const themes::DropDownButtonStyle *dropdown_button_style);

  /////////////////////////////////////////////////
  /// @brief Member variable to hold the style for buttons in the UI.
  /////////////////////////////////////////////////
  ButtonStyle m_button_style;

  /////////////////////////////////////////////////
  /// @brief Member variable to hold the style for panels in the UI.
  /////////////////////////////////////////////////
  PanelStyle m_panel_style;

  /////////////////////////////////////////////////
  /// @brief Member variable tot hold the style for panels in the UI.
  /////////////////////////////////////////////////
  DropDownContainerStyle m_dropdown_style;

  ////////////////////////////////////////////////////////////
  /// @brief Member variable to hold the style for DropDownLists in the UI.
  ////////////////////////////////////////////////////////////
  DropDownListStyle m_dropdown_list_style;

  /// /////////////////////////////////////////////////
  /// @brief Member variable to hold the style for DropDownItems in the UI.
  /// /////////////////////////////////////////////
  DropDownItemStyle m_dropdown_item_style;

  /////////////////////////////////////////////////
  /// @brief Member variable to hold the style for DropDownButtons in the UI.
  /////////////////////////////////////////////////
  DropDownButtonStyle m_dropdown_button_style;

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
   * @brief Gather all logic here, to be called by the Logic.RunLogic()
   * function
   *
   * @param entities A reference to the EntityMemoryPool containing all
   * entities.
   * @param entity_indicies A mask of entity indiced to process the
   * EnittyMemoryPool.
   */
  void ProcessLogic() override;

  /////////////////////////////////////////////////
  /// @brief Defines a spacing strategy for UI elements based on the provided
  /// enum
  ///
  /// @param spacing_strategy_name Enum defined in flatbuffers file
  /// @return Function which will modify positions and sizes of UI element
  /////////////////////////////////////////////////
  SpacingStrategy
  GetSpacingStrategy(const SpacingAndSizingType &spacing_strategy_name) const;

  /////////////////////////////////////////////////
  /// @brief Splits the provided space into even parts and sizes the children
  /// appropriately.
  ///
  /// @param ulement [TODO:parameter]
  /// @param inner_margin [TODO:parameter]
  /// @param parent_border_thickness [TODO:parameter]
  /////////////////////////////////////////////////
  static void EvenSpacingStrategy(UIElement &ul_element,
                                  sf::Vector2f &inner_margin,
                                  float &parent_border_thickness);

  /////////////////////////////////////////////////
  /// @brief Trawls children for any ratios present and calculates them
  ///
  /// @param ulement [TODO:parameter]
  /// @param inner_margin [TODO:parameter]
  /// @param parent_border_thickness [TODO:parameter]
  /////////////////////////////////////////////////
  static void RatioedSpacingStrategy(UIElement &ul_element,
                                     sf::Vector2f &inner_margin,
                                     float &parent_border_thickness);

public:
  ////////////////////////////////////////////////////////////
  // |brief Constructor taking in flatbuffer config
  ////////////////////////////////////////////////////////////
  UIRenderLogic(const LogicContext logic_context);

}; // namespace UIEngine
} // namespace steamrot
