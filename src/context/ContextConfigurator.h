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
#include "ResourceConfigurator.h"
#include "scene_change_packet_generated.h"
#include <expected>
#include <string>

namespace steamrot {

/////////////////////////////////////////////////
/// @class ContextConfigurator
/// @brief Configures resources from FlatBuffers data (wrapper around ResourceConfigurator)
///
/// This class provides a higher-level interface for configuring game and scene
/// resources from FlatBuffers configuration data. It wraps ResourceConfigurator.
/////////////////////////////////////////////////
class ContextConfigurator {
private:
  const ContextData *m_config_data{nullptr};

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for ContextConfigurator
  ///
  /// @param config Pointer to ContextData configuration
  /////////////////////////////////////////////////
  explicit ContextConfigurator(const ContextData *config);

  /////////////////////////////////////////////////
  /// @brief Configure GameResources from FlatBuffers data
  ///
  /// @param resources Reference to GameResources to configure
  /// @return Expected containing monostate or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureGameResources(GameResources &resources) const;

  /////////////////////////////////////////////////
  /// @brief Configure SceneResources from FlatBuffers data
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
