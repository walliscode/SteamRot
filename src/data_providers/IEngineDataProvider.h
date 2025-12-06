/////////////////////////////////////////////////
/// @file
/// @brief Interface for loading engine data.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EngineConfig.h"
#include "EngineState.h"
#include "FailInfo.h"
#include <cstdint>
#include <expected>
#include <string>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Native C++ struct for EngineResources configuration data.
///
/// This data is used to configure the EngineResources struct
/// (window, event handler, asset manager, etc.).
/////////////////////////////////////////////////
struct EngineResourcesConfigData {
  uint32_t window_width{800};
  uint32_t window_height{600};
  std::string window_title{"SteamRot"};
  uint32_t framerate_limit{60};
};

/////////////////////////////////////////////////
/// @class IEngineDataProvider
/// @brief Interface for loading all engine data types.
///
/// Provides separate methods for loading each of the three engine
/// data categories: EngineResources configuration, EngineConfig,
/// and EngineState. Implementations handle the actual data source
/// (files, network, etc.) and format (FlatBuffers, JSON, XML, etc.).
///
/// Usage:
/// ```cpp
/// IEngineDataProvider& provider = GetEngineDataProvider();
/// 
/// auto resources_config = provider.LoadEngineResourcesConfig();
/// auto engine_config = provider.LoadEngineConfig();
/// auto engine_state = provider.LoadEngineState();
/// ```
/////////////////////////////////////////////////
class IEngineDataProvider {
public:
  virtual ~IEngineDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Load EngineResources configuration data.
  ///
  /// Returns data needed to configure global resources like
  /// the game window, event system, etc.
  ///
  /// @return EngineResources config data or failure information
  /////////////////////////////////////////////////
  virtual std::expected<EngineResourcesConfigData, FailInfo>
  LoadEngineResourcesConfig() const = 0;

  /////////////////////////////////////////////////
  /// @brief Load EngineConfig data.
  ///
  /// Returns complete engine configuration including display
  /// settings and user preferences.
  ///
  /// @return EngineConfig or failure information
  /////////////////////////////////////////////////
  virtual std::expected<EngineConfig, FailInfo>
  LoadEngineConfig() const = 0;

  /////////////////////////////////////////////////
  /// @brief Load EngineState data.
  ///
  /// Returns initial engine state including subscriptions
  /// and runtime flags.
  ///
  /// @return EngineState or failure information
  /////////////////////////////////////////////////
  virtual std::expected<EngineState, FailInfo>
  LoadEngineState() const = 0;
};

} // namespace steamrot
