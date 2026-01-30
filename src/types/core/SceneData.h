/////////////////////////////////////////////////
/// @file
/// @brief Declaration of abstract SceneData struct
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AssetConfig.h"
#include "EntityTransportVariant.h"
#include "IEntityConfigurator.h"
#include "SceneInfo.h"
#include "SceneResourcesConfig.h"
#include <memory>

namespace steamrot {

/////////////////////////////////////////////////
/// @class SceneData
/// @brief Complete configuration package for scene creation
///
/// This struct contains both data and strategy (configurator) for scene setup.
/// It follows the Data Transfer Object (DTO) pattern and includes the Strategy
/// pattern for entity configuration.
///
/// **Design Rationale**:
/// - The configurator is embedded because it's tightly coupled with the data
///   format in entity_transport (e.g., FlatbuffersEntityConfigurator for
///   FlatBuffers data, TestEntityConfigurator for test data).
/// - This design keeps the SceneFactory API simple and enables the strategy
///   pattern for different data sources.
/// - The configurator requires EventHandler at construction because entity
///   configuration includes creating and registering event subscribers
///   (UI elements, state transitions, etc.). This ensures atomic configuration
///   where entities are fully set up in a single operation.
///
/// See documentation/architecture/ENTITY_CONFIGURATOR_DESIGN_ANALYSIS.md for
/// detailed analysis of this design.
/////////////////////////////////////////////////
struct SceneData {

  /////////////////////////////////////////////////
  /// @brief Scene information data.
  /////////////////////////////////////////////////
  SceneInfo scene_info;

  /////////////////////////////////////////////////
  /// @brief SceneResources  configuration data.
  /////////////////////////////////////////////////
  SceneResourcesConfig scene_resources_config;

  /////////////////////////////////////////////////
  /// @brief Contains data needed to load all assets for this Scene
  /////////////////////////////////////////////////
  AssetConfig scene_asset_config;

  /////////////////////////////////////////////////
  /// @brief Entity data wrapped in a variant for transport.
  ///
  /// The variant type supports multiple data formats with compile-time type
  /// safety. See EntityTransportVariant.h for details.
  /////////////////////////////////////////////////
  EntityTransportVariant entity_transport;

  /////////////////////////////////////////////////
  /// @brief Entity configurator to set up entities after import.
  ///
  /// The configurator is part of the configuration package because it knows
  /// how to interpret the entity_transport data format. Different data
  /// sources use different configurators (Strategy pattern):
  /// - FlatbuffersEntityConfigurator for FlatBuffers data
  /// - TestEntityConfigurator for in-memory test data
  ///
  /// The configurator is constructed with an EventHandler reference because
  /// entity configuration includes event subscriber registration, which must
  /// happen atomically during setup.
  /////////////////////////////////////////////////
  std::unique_ptr<IEntityConfigurator> entity_configurator;
};

using SceneCollectionData = std::vector<SceneData>;

} // namespace steamrot
