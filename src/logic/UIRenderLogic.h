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
#include "UIStyle.h"
#include <unordered_map>

namespace steamrot {

using SpacingStrategy =
    std::function<void(UIElement &ui_element, sf::Vector2f &inner_margin,
                       float &parent_border_thickness)>;

class UIRenderLogic : public Logic {
private:
  //
  // /**
  //  * @brief Draws all UIElements containted in the CUserInterface components.
  //  */
  // void DrawUIElements();
  //
  // void RecursiveDrawUIElement(UIElement &element);
  //
  // /////////////////////////////////////////////////
  // /// @brief Draws a box with rounded corners.
  // ///
  // /// Draws 4 boxes on the borders and then triangle fans for the corners.
  // ///
  // /// @param element Uses the UIElement to determine the styling
  // /// @param origin top left corner of the box in the render texture
  // /////////////////////////////////////////////////
  // void DrawBoxWithRadiusCorners(UIElement &element);
  //
  // /////////////////////////////////////////////////
  // /// @brief Adjusts the size of the UIElement if it beyond its maximum or
  // /// minimum
  // ///
  // /// @param element Reference to the UIElement
  // /////////////////////////////////////////////////
  // void AdjustSize(UIElement &element);
  //
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
  //
  // /////////////////////////////////////////////////
  // /// @brief Defines a spacing strategy for UI elements based on the provided
  // /// enum
  // ///
  // /// @param spacing_strategy_name Enum defined in flatbuffers file
  // /// @return Function which will modify positions and sizes of UI element
  // /////////////////////////////////////////////////
  // SpacingStrategy
  // GetSpacingStrategy(const SpacingAndSizingType &spacing_strategy_name)
  // const;
  //
  // /////////////////////////////////////////////////
  // /// @brief Splits the provided space into even parts and sizes the children
  // /// appropriately.
  // ///
  // /// @param ulement [TODO:parameter]
  // /// @param inner_margin [TODO:parameter]
  // /// @param parent_border_thickness [TODO:parameter]
  // /////////////////////////////////////////////////
  // static void EvenSpacingStrategy(UIElement &ul_element,
  //                                 sf::Vector2f &inner_margin,
  //                                 float &parent_border_thickness);
  //
  // /////////////////////////////////////////////////
  // /// @brief Trawls children for any ratios present and calculates them
  // ///
  // /// @param ulement [TODO:parameter]
  // /// @param inner_margin [TODO:parameter]
  // /// @param parent_border_thickness [TODO:parameter]
  // /////////////////////////////////////////////////
  // static void RatioedSpacingStrategy(UIElement &ul_element,
  //                                    sf::Vector2f &inner_margin,
  //                                    float &parent_border_thickness);
  //
  // /////////////////////////////////////////////////
  // /// @brief Ignores parent bounds and draws over the top of whatever is
  // there.
  // ///
  // /// @param ul_element [TODO:parameter]
  // /// @param inner_margin [TODO:parameter]
  // /// @param parent_border_thickness [TODO:parameter]
  // /////////////////////////////////////////////////
  // static void DropDownSpacingStrategy(UIElement &ul_element,
  //                                     sf::Vector2f &inner_margin,
  //                                     float &parent_border_thickness);
  //
public:
  /////////////////////////////////////////////////
  /// @brief Constructor for UIRenderLogic
  ///
  /// @param logic_context Takes a LogicContext struct
  /////////////////////////////////////////////////
  UIRenderLogic(const LogicContext logic_context);

  /////////////////////////////////////////////////
  /// @brief Returns the map of all available UI styles
  /////////////////////////////////////////////////
  static const std::unordered_map<std::string, UIStyle> &GetUIStyles();
};
} // namespace steamrot
