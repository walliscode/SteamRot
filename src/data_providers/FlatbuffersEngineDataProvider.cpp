/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersEngineDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersEngineDataProvider.h"
#include "FlatbuffersSubscriberViewer.h"
#include "ISubscriberViewer.h"
#include <memory>

namespace steamrot {

/////////////////////////////////////////////////
std::expected<EngineResourcesConfigData, FailInfo>
FlatbuffersEngineDataProvider::LoadEngineResourcesConfig() const {
  // Use existing loader
  auto fb_result = m_loader.ProvideEngineResourcesConfigFbs();
  if (!fb_result.has_value()) {
    return std::unexpected(fb_result.error());
  }

  const auto *fb_data = fb_result.value();

  // Convert FlatBuffers type to native struct
  EngineResourcesConfigData config_data;
  config_data.window_width = fb_data->window_width();
  config_data.window_height = fb_data->window_height();
  if (fb_data->window_title()) {
    config_data.window_title = fb_data->window_title()->str();
  }
  config_data.framerate_limit = fb_data->framerate_limit();

  return config_data;
}

/////////////////////////////////////////////////
std::expected<EngineConfig, FailInfo>
FlatbuffersEngineDataProvider::LoadEngineConfig() const {

  // Load engine config data from FlatBuffers
  auto fb_result = m_loader.ProvideEngineConfigFbs();
  if (!fb_result.has_value()) {
    return std::unexpected(fb_result.error());
  }
  const auto *fb_data = fb_result.value();
  EngineConfig engine_config;

  // Populate display config from resources config

  engine_config.display.window_title =
      fb_data->display()->window_title()->str();
  engine_config.display.framerate_limit = fb_data->display()->framerate_limit();

  // Set defaults for user preferences (can be loaded from separate file later)
  engine_config.user_preferences.master_volume = 1.0f;
  engine_config.user_preferences.show_fps = false;
  engine_config.user_preferences.preferred_language = "en";

  return engine_config;
}

/////////////////////////////////////////////////
std::expected<EngineState, FailInfo>
FlatbuffersEngineDataProvider::LoadEngineState() const {
  // Load engine data for subscriptions
  auto fb_result = m_loader.ProvideEngineStateFbs();
  if (!fb_result.has_value()) {
    return std::unexpected(fb_result.error());
  }

  const auto *fb_data = fb_result.value();

  EngineState engine_state;

  // Initialize flags
  engine_state.running = false;
  engine_state.paused = false;
  engine_state.quit_requested = false;

  return engine_state;
}

/////////////////////////////////////////////////
std::expected<std::unique_ptr<ISubscriberViewer>, FailInfo>
FlatbuffersEngineDataProvider::GetSubscriptions() const {

  // load engine state for subscriptions
  // [TODO] Consider caching this if performance becomes an issue as we are
  // loading this twice
  auto fb_result = m_loader.ProvideEngineStateFbs();
  if (!fb_result.has_value()) {
    return std::unexpected(fb_result.error());
  }

  // pull out subscription data
  const auto *fb_data = fb_result.value();

  // create and return FlatbuffersSubscriberViewer as a unique_ptr
  return std::make_unique<FlatbuffersSubscriberViewer>(
      fb_data->subscriptions());
}

} // namespace steamrot
