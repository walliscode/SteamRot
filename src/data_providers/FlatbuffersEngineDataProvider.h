/////////////////////////////////////////////////
/// @file
/// @brief FlatBuffers implementation of IEngineDataProvider.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "IEngineDataProvider.h"
#include "ISubscriberDataViewer.h"
#include "FlatbuffersDataLoader.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @class FlatbuffersEngineDataProvider
/// @brief FlatBuffers implementation of IEngineDataProvider.
///
/// Loads engine data from FlatBuffers binary files and converts
/// to native C++ structs for use by game code. Also implements
/// ISubscriberDataViewer to provide access to subscriber configurations.
/////////////////////////////////////////////////
class FlatbuffersEngineDataProvider : public IEngineDataProvider,
                                      public ISubscriberDataViewer {
private:
  FlatbuffersDataLoader m_loader;

public:
  FlatbuffersEngineDataProvider() = default;

  std::expected<EngineResourcesConfigData, FailInfo>
  LoadEngineResourcesConfig() const override;

  std::expected<EngineConfig, FailInfo>
  LoadEngineConfig() const override;

  std::expected<EngineState, FailInfo>
  LoadEngineState() const override;

  /////////////////////////////////////////////////
  /// @brief Get subscriber configurations from EngineState data.
  ///
  /// @return Vector of SubscriberConfig objects or failure information
  /////////////////////////////////////////////////
  std::expected<std::vector<SubscriberConfig>, FailInfo>
  GetSubscriberConfigs() const override;
};

} // namespace steamrot
