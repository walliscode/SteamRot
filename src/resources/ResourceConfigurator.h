////////////////////////////////////////////////////////////
/// @file
/// @brief Declaration of the ResourceConfigurator class.
////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "FailInfo.h"
#include "GameResources.h"
#include "SceneResources.h"
#include "context_data_generated.h"
#include "scene_change_packet_generated.h"
#include <expected>
#include <string>

namespace steamrot {

////////////////////////////////////////////////////////////
/// @class ResourceConfigurator
/// @brief Configures resource structs from FlatBuffers data.
///
/// Provides methods to configure GameResources and SceneResources
/// instances from ContextData configuration.
////////////////////////////////////////////////////////////
class ResourceConfigurator {
private:
  const ContextData *m_config_data{nullptr};

  ////////////////////////////////////////////////////////////
  /// @brief Parse environment type string to EnvironmentType enum.
  ///
  /// @param type_str String representation of environment type
  /// @return Expected containing EnvironmentType or FailInfo on error
  ////////////////////////////////////////////////////////////
  std::expected<EnvironmentType, FailInfo>
  ParseEnvironmentType(const std::string &type_str) const;

public:
  ////////////////////////////////////////////////////////////
  /// @brief Constructor for ResourceConfigurator.
  ///
  /// @param config Pointer to ContextData configuration
  ////////////////////////////////////////////////////////////
  explicit ResourceConfigurator(const ContextData *config);

  ////////////////////////////////////////////////////////////
  /// @brief Configure GameResources from FlatBuffers data.
  ///
  /// Configures window settings (size, title, framerate) and environment
  /// type from the configuration data. Note that the window object must
  /// be created separately after configuration.
  ///
  /// @param resources GameResources instance to configure
  /// @return Expected containing monostate or FailInfo on error
  ////////////////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureGameResources(GameResources &resources) const;

  ////////////////////////////////////////////////////////////
  /// @brief Configure SceneResources from FlatBuffers data.
  ///
  /// Configures render texture dimensions for the specified scene type.
  /// The render texture must be created separately after configuration.
  ///
  /// @param resources SceneResources instance to configure
  /// @param scene_type Scene type to get configuration for
  /// @return Expected containing monostate or FailInfo on error
  ////////////////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(SceneResources &resources,
                          const SceneType &scene_type) const;
};

} // namespace steamrot
