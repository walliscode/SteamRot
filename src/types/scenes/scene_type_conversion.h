
/////////////////////////////////////////////////
/// @file
/// @brief Conversion utilities for SceneType
///
/// This file provides conversion functions between the FlatBuffers
/// SceneTypeFbs enum and the native SceneType enum.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "SceneType.h"
#include "scene_types_generated.h"
#include <expected>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Convert FlatBuffers SceneTypeFbs to native SceneType
///
/// @param fbs_type The FlatBuffers SceneTypeFbs enum value
/// @return Expected containing native SceneType or FailInfo on error
/////////////////////////////////////////////////
inline std::expected<SceneType, FailInfo>
ConvertSceneTypeFbsToSceneType(SceneTypeFbs fbs_type) {
  switch (fbs_type) {
  case SceneTypeFbs_UNKNOWN:
    return SceneType::UNKNOWN;
  case SceneTypeFbs_TEST:
    return SceneType::TEST;
  case SceneTypeFbs_TITLE:
    return SceneType::TITLE;
  case SceneTypeFbs_CRAFTING:
    return SceneType::CRAFTING;
  case SceneTypeFbs_SPATIAL_ANALYSIS:
    return SceneType::SPATIAL_ANALYSIS;
  default:
    return std::unexpected(
        FailInfo{FailMode::EnumValueNotHandled,
                 "Unknown SceneTypeFbs value in conversion"});
  }
}

} // namespace steamrot
