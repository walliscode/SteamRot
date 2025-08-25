/////////////////////////////////////////////////
/// @file
/// @brief Defines the UIRenderLogic class which handles rendering of UI
/// elements.
/////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Preprocessor directives
////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "Logic.h"
#include "styles/ButtonStyle.h"
#include "styles/DropDownButtonStyle.h"
#include "styles/DropDownContainerStyle.h"
#include "styles/DropDownItemStyle.h"
#include "styles/DropDownListStyle.h"
#include "styles/PanelStyle.h"

#include "themes_generated.h"
#include "user_interface_generated.h"
#include <SFML/Graphics.hpp>

namespace steamrot {

using SpacingStrategy =
    std::function<void(UIElement &ui_element, sf::Vector2f &inner_margin,
                       float &parent_border_thickness)>;
class UIRenderLogic : public Logic {
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

  /////////////////////////////////////////////////
  /// @brief Ignores parent bounds and draws over the top of whatever is there.
  ///
  /// @param ul_element [TODO:parameter]
  /// @param inner_margin [TODO:parameter]
  /// @param parent_border_thickness [TODO:parameter]
  /////////////////////////////////////////////////
  static void DropDownSpacingStrategy(UIElement &ul_element,
                                      sf::Vector2f &inner_margin,
                                      float &parent_border_thickness);

public:
  ////////////////////////////////////////////////////////////
  // |brief Constructor taking in flatbuffer config
  ////////////////////////////////////////////////////////////
  UIRenderLogic(const LogicContext logic_context);

}; // namespace UIEngine
} // namespace steamrot
