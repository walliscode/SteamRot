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
/// for configuring GameResources and SceneResources. This class
/// maintains backward compatibility while separating resource
/// configuration from context configuration.
/////////////////////////////////////////////////
class ContextConfigurator {
private:
  const ContextData *m_context_data{nullptr};

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
  /// Loads GameResourcesData and uses ResourceConfigurator to configure
  /// the GameResources instance.
  ///
  /// @param resources GameResources instance to configure
  /// @return Expected containing monostate or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureGameResources(GameResources &resources) const;

  /////////////////////////////////////////////////
  /// @brief Configure SceneResources from FlatBuffers data.
  ///
  /// Loads SceneResourcesCollection and uses ResourceConfigurator to
  /// configure the SceneResources instance.
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
