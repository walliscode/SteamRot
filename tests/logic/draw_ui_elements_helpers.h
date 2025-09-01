/////////////////////////////////////////////////
/// @file
/// @brief Delclaration of helper functions for testing draw_ui_elements
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "ButtonElement.h"
#include "PanelElement.h"
#include "UIStyle.h"
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
namespace steamrot::tests {

UIStyle CreateTestUIStyle();
/////////////////////////////////////////////////
/// @brief Test that drawing a box with a border produces the correct pixels
///
/// @param image Image of the texture after drawing
/// @param base_style Style object that is the base for the UIStyle
/// @param position Position of the top-left corner of the box
/// @param size Size of the box
/////////////////////////////////////////////////
void TestDrawBoxWithBorder(const sf::Image &image, const Style &base_style,
                           const sf::Vector2f &position,
                           const sf::Vector2f &size,
                           bool test_inner_area = false);

/////////////////////////////////////////////////
/// @brief Tests if coloured pixels are present in the area where text should be
///
/// This is not pixel-perfect, but checks that some pixels are drawn in the
/// area. Could definitely be improved.
///
/// @param image [TODO:parameter]
/// @param position [TODO:parameter]
/// @param size [TODO:parameter]
/// @param text_color [TODO:parameter]
/////////////////////////////////////////////////
void TestTextIsPresent(const sf::Image &image, const sf::Vector2f &position,
                       const sf::Vector2f &size, const sf::Color &text_color);

/////////////////////////////////////////////////
/// @brief Test that the correct pixels are drawn on the RenderTexture
/////////////////////////////////////////////////
void TestDrawPanel(sf::Image &image, const PanelElement &panel,
                   const UIStyle &style);

/////////////////////////////////////////////////
/// @brief Test that the correct pixels are drawn on the RenderTexture for a
/// button
///
/// @param image Image of the texture after drawing
/// @param button Instance of ButtonElement being drawn
/// @param style UIStyle used for drawing
/////////////////////////////////////////////////
void TestDrawButton(sf::Image &image, const ButtonElement &button,
                    const UIStyle &style);

/////////////////////////////////////////////////
/// @brief Display a RenderTexture in a window for a number of cycles
///
/// @param renderTexture [TODO:parameter]
/// @param numCycles [TODO:parameter]
/////////////////////////////////////////////////
void DisplayRenderTextureForCycles(const sf::RenderTexture &renderTexture);

} // namespace steamrot::tests
