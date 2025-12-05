/////////////////////////////////////////////////
/// @file
/// @brief Factory functions for data providers.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "IAssetDataProvider.h"
#include "IEngineDataProvider.h"
#include "IFragmentDataProvider.h"
#include "IGameConfigProvider.h"
#include "ISceneDataProvider.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Get the engine data provider instance.
///
/// Returns a singleton instance of the FlatBuffers engine provider.
/// Future: Could be made configurable to return different implementations.
/////////////////////////////////////////////////
IEngineDataProvider &GetEngineDataProvider();

/////////////////////////////////////////////////
/// @brief Get the scene data provider instance.
/////////////////////////////////////////////////
ISceneDataProvider &GetSceneDataProvider();

/////////////////////////////////////////////////
/// @brief Get the asset data provider instance.
/////////////////////////////////////////////////
IAssetDataProvider &GetAssetDataProvider();

/////////////////////////////////////////////////
/// @brief Get the fragment data provider instance.
/////////////////////////////////////////////////
IFragmentDataProvider &GetFragmentDataProvider();

/////////////////////////////////////////////////
/// @brief Get the game config provider instance.
///
/// This provider returns FlatBuffers types for complex configuration
/// (subscriptions, event buses, scene manager) that hasn't yet been
/// converted to native C++ structs. Temporary until Phase 4.
/////////////////////////////////////////////////
IGameConfigProvider &GetGameConfigProvider();

} // namespace steamrot
