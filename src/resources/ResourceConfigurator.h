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
#include "resource_data_generated.h"
#include "scene_change_packet_generated.h"
#include <expected>
#include <string>

namespace steamrot {

////////////////////////////////////////////////////////////
/// @class ResourceConfigurator
/// @brief Configures resource structs from FlatBuffers data.
///
/// Provides methods to configure GameResources and SceneResources
/// instances from resource-specific FlatBuffers data (not ContextData).
/// This allows resource configuration to be independent of context
/// configuration.
////////////////////////////////////////////////////////////
class ResourceConfigurator {
private:
  const GameResourcesData *m_game_resources_data{nullptr};

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
  /// @param game_data Pointer to GameResourcesData configuration
  ////////////////////////////////////////////////////////////
  explicit ResourceConfigurator(const GameResourcesData *game_data);

  ////////////////////////////////////////////////////////////
  /// @brief Configure GameResources from FlatBuffers data.
  ///
  /// Configures window settings (size, title, framerate) and environment
  /// type from the configuration data. The window is created directly
  /// during configuration.
  ///
  /// @param resources GameResources instance to configure
  /// @return Expected containing monostate or FailInfo on error
  ////////////////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureGameResources(GameResources &resources) const;

  ////////////////////////////////////////////////////////////
  /// @brief Configure SceneResources from FlatBuffers data.
  ///
  /// Configures render texture dimensions from the scene data.
  /// If scene_data is null, uses default dimensions (800x600).
  /// The render texture is created directly during configuration.
  ///
  /// @param resources SceneResources instance to configure
  /// @param scene_data Scene resource data (can be null for defaults)
  /// @return Expected containing monostate or FailInfo on error
  ////////////////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(SceneResources &resources,
                          const SceneResourcesData *scene_data) const;
};

} // namespace steamrot
