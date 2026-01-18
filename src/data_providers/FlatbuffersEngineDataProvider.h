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
#include "asset_config_generated.h"
#include "engine_resources_config_generated.h"
#include "engine_state_generated.h"
#include <expected>

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
  /// @brief Provides EngineData loaded from FlatBuffers binary files.
  /////////////////////////////////////////////////
  std::expected<EngineData, FailInfo> LoadEngineData() const override;
};

} // namespace steamrot
