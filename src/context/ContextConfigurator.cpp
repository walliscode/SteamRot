/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ContextConfigurator class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ContextConfigurator.h"

namespace steamrot {

/////////////////////////////////////////////////
ContextConfigurator::ContextConfigurator(const ContextData *config)
    : m_config_data(config) {}

/////////////////////////////////////////////////
std::expected<EnvironmentType, FailInfo>
ContextConfigurator::ParseEnvironmentType(const std::string &type_str) const {
  if (type_str == "None") {
    return EnvironmentType::None;
  } else if (type_str == "Test") {
    return EnvironmentType::Test;
  } else if (type_str == "Production") {
    return EnvironmentType::Production;
  } else {
    return std::unexpected(
        FailInfo{FailMode::NonExistentEnumValue,
                 "Unknown environment type: " + type_str});
  }
}

/////////////////////////////////////////////////
std::expected<GameContextBuilder, FailInfo>
ContextConfigurator::CreateGameContextBuilder() const {
  if (!m_config_data) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer, "ContextData is null"});
  }

  if (!m_config_data->game_context()) {
    return std::unexpected(FailInfo{FailMode::MissingRequiredField,
                                     "GameContextConfig is missing"});
  }

  const GameContextConfig *game_config = m_config_data->game_context();

  GameContextBuilder builder;

  // Parse and set environment type if provided
  if (game_config->environment_type()) {
    auto env_type_result =
        ParseEnvironmentType(game_config->environment_type()->str());
    if (!env_type_result.has_value()) {
      return std::unexpected(env_type_result.error());
    }
    builder.SetEnvironmentType(env_type_result.value());
  }

  // Note: Window, EventHandler, AssetManager, and LoopNumber must be set
  // by the caller with actual runtime objects after getting this builder

  return builder;
}

/////////////////////////////////////////////////
std::expected<LogicContextBuilder, FailInfo>
ContextConfigurator::CreateLogicContextBuilder(
    const SceneType &scene_type) const {
  if (!m_config_data) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer, "ContextData is null"});
  }

  if (!m_config_data->scene_contexts()) {
    return std::unexpected(FailInfo{FailMode::MissingRequiredField,
                                     "Scene contexts are missing"});
  }

  // Find the configuration for the requested scene type
  const SceneContextConfig *scene_config = nullptr;
  for (const auto *config : *m_config_data->scene_contexts()) {
    if (config && config->scene_type() == scene_type) {
      scene_config = config;
      break;
    }
  }

  if (!scene_config) {
    return std::unexpected(
        FailInfo{FailMode::SceneTypeNotFound,
                 "Scene configuration not found for requested scene type"});
  }

  // Create builder (runtime objects must be set by caller)
  LogicContextBuilder builder;

  // Note: All runtime objects (scene_entities, archetypes, scene_texture,
  // game_window, asset_manager, event_handler, mouse_position) must be set
  // by the caller after getting this builder

  return builder;
}

} // namespace steamrot
