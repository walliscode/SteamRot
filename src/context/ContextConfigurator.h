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
#include "GameContextBuilder.h"
#include "LogicContextBuilder.h"
#include "PathProvider.h"
#include "context_data_generated.h"
#include "scene_change_packet_generated.h"
#include <expected>
#include <string>

namespace steamrot {

/////////////////////////////////////////////////
/// @class ContextConfigurator
/// @brief Configures and creates context builders from FlatBuffers data.
///
/// Provides methods to create GameContextBuilder and LogicContextBuilder
/// instances from ContextData configuration.
/////////////////////////////////////////////////
class ContextConfigurator {
private:
  const ContextData *m_config_data{nullptr};

  /////////////////////////////////////////////////
  /// @brief Parse environment type string to EnvironmentType enum.
  ///
  /// @param type_str String representation of environment type
  /// @return Expected containing EnvironmentType or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<EnvironmentType, FailInfo>
  ParseEnvironmentType(const std::string &type_str) const;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for ContextConfigurator.
  ///
  /// @param config Pointer to ContextData configuration
  /////////////////////////////////////////////////
  explicit ContextConfigurator(const ContextData *config);

  /////////////////////////////////////////////////
  /// @brief Create GameContextBuilder from configuration data.
  ///
  /// Note: The builder still needs runtime objects set via SetWindow(),
  /// SetEventHandler(), etc. This method only configures static settings
  /// like environment type from the configuration data.
  ///
  /// @return Expected containing GameContextBuilder or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<GameContextBuilder, FailInfo> CreateGameContextBuilder() const;

  /////////////////////////////////////////////////
  /// @brief Create LogicContextBuilder from configuration for a scene type.
  ///
  /// Note: The builder still needs runtime objects set via SetSceneEntities(),
  /// SetGameWindow(), etc. This method only validates the scene configuration
  /// exists.
  ///
  /// @param scene_type Scene type to get configuration for
  /// @return Expected containing LogicContextBuilder or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<LogicContextBuilder, FailInfo>
  CreateLogicContextBuilder(const SceneType &scene_type) const;
};

} // namespace steamrot
