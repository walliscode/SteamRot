/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the ContextConfigurator class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "GameResources.h"
#include "ResourceConfigurator.h"
#include "SceneResources.h"
#include "context_data_generated.h"
#include "scene_change_packet_generated.h"
#include <expected>
#include <string>

namespace steamrot {

/////////////////////////////////////////////////
/// @class ContextConfigurator
/// @brief Configures resources from FlatBuffers data.
///
/// Provides a simplified interface that wraps ResourceConfigurator
/// for configuring GameResources and SceneResources.
/////////////////////////////////////////////////
class ContextConfigurator {
private:
  ResourceConfigurator m_resource_configurator;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for ContextConfigurator.
  ///
  /// @param config Pointer to ContextData configuration
  /////////////////////////////////////////////////
  explicit ContextConfigurator(const ContextData *config);

  /////////////////////////////////////////////////
  /// @brief Configure GameResources from FlatBuffers data.
  ///
  /// Configures window settings, environment type, and other game-level
  /// resources from the configuration data.
  ///
  /// @param resources GameResources instance to configure
  /// @return Expected containing monostate or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureGameResources(GameResources &resources) const;

  /////////////////////////////////////////////////
  /// @brief Configure SceneResources from FlatBuffers data.
  ///
  /// Configures scene-level resources (render texture) for the specified
  /// scene type from the configuration data.
  ///
  /// @param resources SceneResources instance to configure
  /// @param scene_type Scene type to get configuration for
  /// @return Expected containing monostate or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(SceneResources &resources,
                          const SceneType &scene_type) const;
};

} // namespace steamrot
