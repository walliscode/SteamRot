/////////////////////////////////////////////////
/// @file
/// @brief FlatBuffers implementation of IEngineDataProvider.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "IEngineDataProvider.h"
#include "SubscriberDataViewer.h"
#include "FlatbuffersDataLoader.h"
#include <memory>

namespace steamrot {

/////////////////////////////////////////////////
/// @class FlatbuffersEngineDataProvider
/// @brief FlatBuffers implementation of IEngineDataProvider.
///
/// Loads engine data from FlatBuffers binary files and converts
/// to native C++ structs for use by game code. Contains a
/// SubscriberDataViewer member to provide access to subscriber
/// configurations.
/////////////////////////////////////////////////
class FlatbuffersEngineDataProvider : public IEngineDataProvider {
private:
  FlatbuffersDataLoader m_loader;
  mutable std::unique_ptr<SubscriberDataViewer> m_subscriber_viewer;

public:
  FlatbuffersEngineDataProvider() = default;

  std::expected<EngineResourcesConfigData, FailInfo>
  LoadEngineResourcesConfig() const override;

  std::expected<EngineConfig, FailInfo>
  LoadEngineConfig() const override;

  std::expected<EngineState, FailInfo>
  LoadEngineState() const override;

  /////////////////////////////////////////////////
  /// @brief Get subscriber data viewer for this provider.
  ///
  /// Lazily creates and returns the subscriber data viewer.
  ///
  /// @return Reference to SubscriberDataViewer or failure information
  /////////////////////////////////////////////////
  std::expected<const SubscriberDataViewer&, FailInfo>
  GetSubscriberViewer() const override;

  /////////////////////////////////////////////////
  /// @brief Get subscriber configurations from EngineState data.
  ///
  /// Convenience method that delegates to the subscriber viewer.
  ///
  /// @return Vector of SubscriberConfig objects or failure information
  /////////////////////////////////////////////////
  std::expected<std::vector<SubscriberConfig>, FailInfo>
  GetSubscriberConfigs() const;
};

} // namespace steamrot
