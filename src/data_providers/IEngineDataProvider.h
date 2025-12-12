/////////////////////////////////////////////////
/// @file
/// @brief Interface for loading engine data.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EngineConfig.h"
#include "EngineResourcesConfig.h"
#include "EngineState.h"
#include "FailInfo.h"
#include "ISubscriberViewer.h"
#include <expected>
#include <memory>

// Forward declaration
namespace steamrot {
class FlatbuffersSubscriberViewer;
}

namespace steamrot {

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
  virtual std::expected<EngineResourcesConfig, FailInfo>
  LoadEngineResourcesConfig() const = 0;

  /////////////////////////////////////////////////
  /// @brief Load EngineConfig data.
  ///
  /// Returns complete engine configuration including display
  /// settings and user preferences.
  ///
  /// @return EngineConfig or failure information
  /////////////////////////////////////////////////
  virtual std::expected<EngineConfig, FailInfo> LoadEngineConfig() const = 0;

  /////////////////////////////////////////////////
  /// @brief Load EngineState data.
  ///
  /// Returns initial engine state including subscriptions
  /// and runtime flags.
  ///
  /// @return EngineState or failure information
  /////////////////////////////////////////////////
  virtual std::expected<EngineState, FailInfo> LoadEngineState() const = 0;

  /////////////////////////////////////////////////
  /// @brief Return subscriber data viewer for this provider.
  /////////////////////////////////////////////////
  virtual std::expected<std::unique_ptr<ISubscriberViewer>, FailInfo>
  GetSubscriberViewer() const = 0;
};

} // namespace steamrot
