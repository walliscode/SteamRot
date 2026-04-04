/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <cstdint>
#include <string>

namespace steamrot {
/////////////////////////////////////////////////
/// @brief Native C++ struct for EngineResources configuration data.
///
/// This data is used to configure the EngineResources struct
/// (window, event handler, asset manager, etc.).
/////////////////////////////////////////////////
struct EngineResourcesConfig {
  uint32_t window_width{800};
  uint32_t window_height{600};
  std::string window_title{"SteamRot"};
  uint32_t framerate_limit{60};
};
} // namespace steamrot
