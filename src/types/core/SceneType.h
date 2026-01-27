
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
  CRAFTING = 3
};

} // namespace steamrot
