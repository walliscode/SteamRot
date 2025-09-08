/////////////////////////////////////////////////
/// @file
/// @brief Declaration of helper functions for logic tests
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "LogicFactory.h"
#include "scene_types_generated.h"
namespace steamrot::tests {
/////////////////////////////////////////////////
/// @brief Checks LogicCollection for each Scene Type
///
/// Logic collections are hard coded into the game, this function checks against
/// those "static" setups
/// @param collection LogicCollection to check
/// @param scene_type Scene Type to check against
/////////////////////////////////////////////////
void CheckStaticLogicCollections(const steamrot::LogicCollection &collection,
                                 const steamrot::SceneType &scene_type);
} // namespace steamrot::tests
