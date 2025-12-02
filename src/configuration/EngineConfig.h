/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the EngineConfig struct.
///
/// EngineConfig contains user-configurable engine settings that
/// are loaded separately during engine startup. This supports the
/// cascading defaults/user configuration system.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <cstdint>
#include <string>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Engine configuration settings struct.
///
/// Contains user-configurable settings loaded from FlatBuffers.
/// Supports cascading from defaults to user overrides.
/////////////////////////////////////////////////
struct EngineConfig {
  /// Window width in pixels
  uint32_t window_width{800};

  /// Window height in pixels
  uint32_t window_height{600};

  /// Window title
  std::string window_title{"SteamRot"};

  /// Target framerate limit
  uint32_t framerate_limit{60};

  /// Fullscreen mode enabled
  bool fullscreen{false};

  /// VSync enabled
  bool vsync{true};
};

} // namespace steamrot
