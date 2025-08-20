/////////////////////////////////////////////////
/// @file
/// @brief Declarations for asset helper functions used in tests.
////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "AssetManager.h"
#include "assets_generated.h"
#include "scene_types_generated.h"

namespace steamrot::tests {
/////////////////////////////////////////////////
/// @brief Checks that the default assets are configured correctly for a given
/// scene type.
///
/// This is a container function that verifies if the default assets for a
/// SceneType have been configured correctly in the AssetManager. It groups
/// functions for each asset type.
/// @param scene_type Enumeration representing the scene type
/// @param asset_manager Instance of AssetManager to check against
/////////////////////////////////////////////////
void check_asset_configuration(const SceneType &scene_type,
                               const AssetManager &asset_manager);

/////////////////////////////////////////////////
/// @brief Checks that the default fonts have been configured correctly
///
/// @param asset_collection Flatbuffer asset collection containing font config
/// data
/// @param fonts Map of sf::Font objects indexed by font names
/////////////////////////////////////////////////
void check_font_configuration(const AssetCollection &asset_collection,
                              const AssetManager &asset_manager);

} // namespace steamrot::tests
