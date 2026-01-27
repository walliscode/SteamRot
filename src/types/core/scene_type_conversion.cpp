
/////////////////////////////////////////////////
/// @file
/// @brief Implementation of SceneType conversion utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "scene_type_conversion.h"

namespace steamrot {

/////////////////////////////////////////////////
std::expected<SceneType, FailInfo>
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
  default:
    return std::unexpected(
        FailInfo{FailMode::EnumValueNotHandled,
                 "Unknown SceneTypeFbs value in conversion"});
  }
}

} // namespace steamrot
