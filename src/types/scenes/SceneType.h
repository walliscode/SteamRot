
/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the native SceneType enum.
///
/// This is the native C++ enum for scene types, separate from the
/// FlatBuffers SceneTypeFbs enum. This enum should be used throughout
/// the main codebase. FlatBuffers enums should only be used at the
/// boundary when loading/saving data.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include <string>
namespace steamrot {

/////////////////////////////////////////////////
/// @enum SceneType
/// @brief Native C++ enum representing different scene types
///
/// This enum should be used throughout the codebase for scene type
/// identification. The FlatBuffers SceneTypeFbs enum should only be
/// used when loading/saving data and converted to this enum.
/////////////////////////////////////////////////
enum class SceneType {
  UNKNOWN = 0,
  TEST = 1,
  TITLE = 2,
  CRAFTING = 3,
  UI_EXPLORER = 4,
  SPATIAL_ANALYSIS = 5
};

/////////////////////////////////////////////////
/// @brief Converts a SceneType enum value to its string representation.
///
/// @param scene_type SceneType enum value to convert.
/////////////////////////////////////////////////
inline std::string EnumNameSceneType(SceneType scene_type) {
  switch (scene_type) {
  case SceneType::UNKNOWN:
    return "UNKNOWN";
  case SceneType::TEST:
    return "TEST";
  case SceneType::TITLE:
    return "TITLE";
  case SceneType::CRAFTING:
    return "CRAFTING";
  case SceneType::UI_EXPLORER:
    return "UI_EXPLORER";
  case SceneType::SPATIAL_ANALYSIS:
    return "SpatialAnalysis";
  default:
    return "INVALID_SCENE_TYPE";
  }
}
} // namespace steamrot
