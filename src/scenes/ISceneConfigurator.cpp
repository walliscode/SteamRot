/////////////////////////////////////////////////
/// @file
/// @brief Implements a scene configurator that uses default settings for Scenes
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ISceneConfigurator.h"
#include "scene_types_generated.h"
#include <expected>
#include <variant>

namespace steamrot {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ISceneConfigurator::ConfigureScene(Scene &scene, const SceneType scene_type) {

  return std::monostate();
}

} // namespace steamrot
