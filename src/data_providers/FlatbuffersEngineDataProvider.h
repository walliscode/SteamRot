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

  /////////////////////////////////////////////////
  /// @brief Logic for populating EngineResourcesConfig from flatbuffers data
  /// source.
  ///
  /// @param engine_resources_config Reference to EngineResourcesConfig to
  /// populate.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> PopulateEngineResourcesConfig(
      EngineResourcesConfig &engine_resources_config,
      const EngineResourcesConfigFbs *engine_resources_config_data) const;

  /////////////////////////////////////////////////
  /// @brief Logic for populating EngineConfig from data source.
  ///
  /// @param engine_config Reference to EngineConfig to populate.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  PopulateEngineConfig(EngineConfig &engine_config,
                       const EngineConfigFbs *engine_config_data) const;

  /////////////////////////////////////////////////
  /// @brief Logic for populating EngineState from data source.
  ///
  /// @param engine_state Reference to EngineState to populate.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  PopulateEngineState(EngineState &engine_state,
                      const EngineStateFbs *engine_state_data) const;

  /////////////////////////////////////////////////
  /// @brief logic for populating initial AssetConfig from data source.
  ///
  /// @param asset_config Reference to AssetConfig to populate.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  PopulateInitialAssetConfig(AssetConfig &asset_config,
                             const AssetConfigFbs *asset_config_data) const;

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
