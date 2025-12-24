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
  /////////////////////////////////////////////////
  /// @brief Instance of FlatbuffersDataLoader for loading data.
  /////////////////////////////////////////////////
  FlatbuffersDataLoader m_loader;

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor.
  /////////////////////////////////////////////////
  FlatbuffersEngineDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Provide the EngineResources configuration data.
  /////////////////////////////////////////////////
  std::expected<EngineResourcesConfig, FailInfo>
  LoadEngineResourcesConfig() const override;

  /////////////////////////////////////////////////
  /// @brief Provide the Engine configuration data.
  /////////////////////////////////////////////////
  std::expected<EngineConfig, FailInfo> LoadEngineConfig() const override;

  /////////////////////////////////////////////////
  /// @brief Provide the Engine state data.
  /////////////////////////////////////////////////
  std::expected<EngineState, FailInfo> LoadEngineState() const override;

  /////////////////////////////////////////////////
  /// @brief Return a viewer for subscriber data. In particular, the FlatBuffers
  /// implementation
  /////////////////////////////////////////////////
  std::expected<std::unique_ptr<ISubscriberViewer>, FailInfo>
  GetSubscriberViewer() const override;

  /////////////////////////////////////////////////
  /// @brief Return a viewer for asset configuration data. In particular, the
  /// FlatBuffers implementation
  /////////////////////////////////////////////////
  std::expected<std::unique_ptr<IAssetConfigViewer>, FailInfo>
  GetAssetConfigViewer() const override;
};

} // namespace steamrot
