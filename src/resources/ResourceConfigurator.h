/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the ResourceConfigurator class
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
#include "SceneResources.h"
#include "context_data_generated.h"
#include "scene_change_packet_generated.h"
#include <SFML/Graphics/VideoMode.hpp>
#include <expected>

namespace steamrot {

/////////////////////////////////////////////////
/// @class ResourceConfigurator
/// @brief Configures resources from FlatBuffers data
///
/// This class handles data-driven configuration of both GameResources
/// and SceneResources from FlatBuffers configuration data.
/////////////////////////////////////////////////
class ResourceConfigurator {
private:
  const ContextData *m_config_data{nullptr};

  /////////////////////////////////////////////////
  /// @brief Parse environment type string to EnvironmentType enum
  ///
  /// @param type_str String representation of environment type
  /// @return Expected containing EnvironmentType or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<EnvironmentType, FailInfo>
  ParseEnvironmentType(const std::string &type_str) const;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for ResourceConfigurator
  ///
  /// @param config Pointer to ContextData configuration
  /////////////////////////////////////////////////
  explicit ResourceConfigurator(const ContextData *config);

  /////////////////////////////////////////////////
  /// @brief Configure GameResources from FlatBuffers data
  ///
  /// This configures the window settings and environment type.
  /// Other resources (AssetManager, EventHandler, etc.) are default initialized.
  ///
  /// @param resources Reference to GameResources to configure
  /// @return Expected containing monostate or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureGameResources(GameResources &resources) const;

  /////////////////////////////////////////////////
  /// @brief Configure SceneResources from FlatBuffers data
  ///
  /// This configures the entity pool size and render texture dimensions.
  ///
  /// @param resources Reference to SceneResources to configure
  /// @param scene_type Scene type to get configuration for
  /// @return Expected containing monostate or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(SceneResources &resources,
                          const SceneType &scene_type) const;
};

} // namespace steamrot
