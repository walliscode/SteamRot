/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for resize logic operations.
///
/// These functions contain the specific resize logic for updating scene
/// resources when the window is resized. Kept as free functions for easy
/// unit testing.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>
#include <expected>

namespace steamrot::logic::resize {

/////////////////////////////////////////////////
/// @brief Resize the scene render texture to the given dimensions.
///
/// Resizes the provided RenderTexture to match @p new_size. If the resize
/// fails, the texture is left in its previous state and an error is returned.
///
/// @param scene_texture  The render texture to resize.
/// @param new_size       The new dimensions for the texture.
/// @return std::monostate on success, or a FailInfo describing the error.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ResizeSceneTexture(sf::RenderTexture &scene_texture,
                   const sf::Vector2u &new_size);

} // namespace steamrot::logic::resize
