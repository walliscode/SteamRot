/////////////////////////////////////////////////
/// @file
/// @brief FlatBuffers implementation of IEngineDataProvider.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersDataLoader.h"
#include "IEngineDataProvider.h"
#include "ISubscriberViewer.h"
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

public:
  FlatbuffersEngineDataProvider() = default;

  std::expected<EngineResourcesConfig, FailInfo>
  LoadEngineResourcesConfig() const override;

  std::expected<EngineConfig, FailInfo> LoadEngineConfig() const override;

  std::expected<EngineState, FailInfo> LoadEngineState() const override;

  std::expected<std::unique_ptr<ISubscriberViewer>, FailInfo>
  GetSubscriberViewer() const override;
};

} // namespace steamrot
