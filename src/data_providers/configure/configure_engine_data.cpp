/////////////////////////////////////////////////
/// @file
/// @brief Implementation of functions to configure engine data.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_engine_data.h"
#include "configure_subscribers.h"

namespace steamrot::data::configure {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConfigureEngineResourcesConfig(
    EngineResourcesConfig &engine_resources_config,
    const EngineResourcesConfigFbs *engine_resources_config_data) {

  // check for null data
  if (!engine_resources_config_data) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "EngineResourcesConfigFbs data is null, cannot populate "
                 "EngineResourcesConfig"});
  }

  // populate fields from flatbuffers data
  if (!engine_resources_config_data->window_width())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "EngineResourcesConfigFbs missing window_width field"});
  engine_resources_config.window_width =
      engine_resources_config_data->window_width();

  if (!engine_resources_config_data->window_height())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "EngineResourcesConfigFbs missing window_height field"});
  engine_resources_config.window_height =
      engine_resources_config_data->window_height();

  if (engine_resources_config_data->window_title()) {
    engine_resources_config.window_title =
        engine_resources_config_data->window_title()->str();
  }

  if (!engine_resources_config_data->framerate_limit())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "EngineResourcesConfigFbs missing framerate_limit field"});
  engine_resources_config.framerate_limit =
      engine_resources_config_data->framerate_limit();

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureEngineConfig(EngineConfig &engine_config,
                      const EngineConfigFbs *engine_config_data) {
  // check for null data
  if (!engine_config_data) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "EngineConfigFbs data is null, cannot populate EngineConfig"});
  }
  // populate fields from flatbuffers data

  engine_config.display.window_title =
      engine_config_data->display()->window_title()->str();
  engine_config.display.framerate_limit =
      engine_config_data->display()->framerate_limit();

  // Set defaults for user preferences (can be loaded from separate file later)
  engine_config.user_preferences.master_volume = 1.0f;
  engine_config.user_preferences.show_fps = false;
  engine_config.user_preferences.preferred_language = "en";

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureEngineState(EngineState &engine_state,
                     const EngineStateFbs *engine_state_data) {
  // check for null data
  if (!engine_state_data) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "EngineStateFbs data is null, cannot populate EngineState"});
  }
  // populate fields from flatbuffers data
  engine_state.running = engine_state_data->running();
  engine_state.paused = engine_state_data->paused();
  engine_state.quit_requested = engine_state_data->quit_requested();

  //  populate subscriptions
  for (const SubscriberFbs *subscriber_fbs :
       *engine_state_data->subscriptions()) {
    auto create_result = data::configure::CreateSubscriber(subscriber_fbs);
    if (!create_result.has_value()) {
      return std::unexpected(create_result.error());
    }
    engine_state.subscriptions.push_back(
        std::make_shared<Subscriber>(create_result.value()));
  }
  return std::monostate{};
}

} // namespace steamrot::data::configure
