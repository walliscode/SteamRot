/////////////////////////////////////////////////
/// @file
/// @brief Compile-time configuration for game engine
///
/// This header provides compile-time configuration that differs between
/// production builds and test builds. The STEAMROT_TEST_MODE preprocessor
/// define controls which configuration is active.
///
/// In test mode:
/// - Runtime logic injection is enabled
/// - Data injection is enabled
/// - Headless mode is available
/// - Default environment is Test
///
/// In production mode:
/// - All injection capabilities are disabled at compile time
/// - No runtime guards needed for these features
/// - Default environment is Production
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "PathProvider.h"

namespace steamrot::config {

#ifdef STEAMROT_TEST_MODE

/////////////////////////////////////////////////
/// @brief Game engine configuration for test builds
///
/// When STEAMROT_TEST_MODE is defined, the game engine allows:
/// - Runtime injection of custom Logic classes
/// - Data injection for entity state manipulation
/// - Headless mode for automated testing
/////////////////////////////////////////////////
struct GameEngineConfig {
  /////////////////////////////////////////////////
  /// @brief Enable runtime injection of Logic classes
  ///
  /// When true, allows tests to inject custom Logic classes
  /// into the execution pipeline at runtime.
  /////////////////////////////////////////////////
  static constexpr bool ENABLE_RUNTIME_LOGIC_INJECTION = true;

  /////////////////////////////////////////////////
  /// @brief Enable data injection
  ///
  /// When true, allows tests to inject entity data and
  /// state into the game engine at runtime.
  /////////////////////////////////////////////////
  static constexpr bool ENABLE_DATA_INJECTION = true;

  /////////////////////////////////////////////////
  /// @brief Enable headless mode
  ///
  /// When true, allows the game engine to run without
  /// creating windows or rendering graphics.
  /////////////////////////////////////////////////
  static constexpr bool ENABLE_HEADLESS_MODE = true;

  /////////////////////////////////////////////////
  /// @brief Default environment type
  ///
  /// The default environment type used when no explicit
  /// environment is specified.
  /////////////////////////////////////////////////
  static constexpr EnvironmentType DEFAULT_ENV_TYPE = EnvironmentType::Test;
};

#else

/////////////////////////////////////////////////
/// @brief Game engine configuration for production builds
///
/// When STEAMROT_TEST_MODE is not defined, all injection capabilities
/// are disabled at compile time, eliminating the need for runtime guards.
/////////////////////////////////////////////////
struct GameEngineConfig {
  /////////////////////////////////////////////////
  /// @brief Enable runtime injection of Logic classes
  ///
  /// Disabled in production - no custom Logic injection allowed.
  /////////////////////////////////////////////////
  static constexpr bool ENABLE_RUNTIME_LOGIC_INJECTION = false;

  /////////////////////////////////////////////////
  /// @brief Enable data injection
  ///
  /// Disabled in production - no data injection allowed.
  /////////////////////////////////////////////////
  static constexpr bool ENABLE_DATA_INJECTION = false;

  /////////////////////////////////////////////////
  /// @brief Enable headless mode
  ///
  /// Disabled in production - window and rendering required.
  /////////////////////////////////////////////////
  static constexpr bool ENABLE_HEADLESS_MODE = false;

  /////////////////////////////////////////////////
  /// @brief Default environment type
  ///
  /// Production environment is the default in production builds.
  /////////////////////////////////////////////////
  static constexpr EnvironmentType DEFAULT_ENV_TYPE = EnvironmentType::Production;
};

#endif

/////////////////////////////////////////////////
/// @brief Check if a configuration option is enabled
///
/// Helper template to check config options at compile time.
///
/// @tparam Option The config option to check
/// @return true if the option is enabled
/////////////////////////////////////////////////
template <bool Option>
constexpr bool IsConfigEnabled() {
  return Option;
}

} // namespace steamrot::config
