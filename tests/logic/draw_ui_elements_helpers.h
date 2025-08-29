/////////////////////////////////////////////////
/// @file
/// @brief Delclaration of helper functions for testing draw_ui_elements
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

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
                           const sf::Vector2f &size);

/////////////////////////////////////////////////
/// @brief Test that the correct pixels are drawn on the RenderTexture
/////////////////////////////////////////////////
void TestDrawPanel(sf::Image &image, const PanelElement &panel,
                   const UIStyle &style);
} // namespace steamrot::tests
