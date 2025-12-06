/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersEngineDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersEngineDataProvider.h"
#include "FlatbuffersSubscriberDataProvider.h"
#include "SubscriberFactory.h"

namespace steamrot {

/////////////////////////////////////////////////
std::expected<EngineResourcesConfigData, FailInfo>
FlatbuffersEngineDataProvider::LoadEngineResourcesConfig() const {
  // Use existing loader
  auto fb_result = m_loader.ProvideEngineCoreData();
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
  // Load display config from engine core data
  auto resources_config = LoadEngineResourcesConfig();
  if (!resources_config.has_value()) {
    return std::unexpected(resources_config.error());
  }

  EngineConfig engine_config;

  // Populate display config from resources config
  engine_config.display.window_width = resources_config.value().window_width;
  engine_config.display.window_height = resources_config.value().window_height;
  engine_config.display.window_title = resources_config.value().window_title;
  engine_config.display.framerate_limit =
      resources_config.value().framerate_limit;

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
  auto fb_result = m_loader.ProvideEngineData();
  if (!fb_result.has_value()) {
    return std::unexpected(fb_result.error());
  }

  const auto *fb_data = fb_result.value();

  EngineState engine_state;

  // Initialize flags
  engine_state.running = false;
  engine_state.paused = false;
  engine_state.quit_requested = false;

  // Load subscriptions
  if (fb_data->subscriptions()) {
    SubscriberFactory subscriber_factory;
    for (const auto *sub_data : *fb_data->subscriptions()) {
      auto subscriber_result = subscriber_factory.CreateSubscriber(sub_data);
      if (subscriber_result.has_value()) {
        engine_state.subscriptions.push_back(subscriber_result.value());
      }
    }
  }

  return engine_state;
}

/////////////////////////////////////////////////
std::expected<std::reference_wrapper<const SubscriberDataViewer>, FailInfo>
FlatbuffersEngineDataProvider::GetSubscriberViewer() const {
  // Lazily create the viewer if it doesn't exist
  if (!m_subscriber_viewer) {
    auto fb_result = m_loader.ProvideEngineData();
    if (!fb_result.has_value()) {
      return std::unexpected(fb_result.error());
    }

    const auto *fb_data = fb_result.value();
    m_subscriber_viewer =
        std::make_unique<SubscriberDataViewer>(fb_data->subscriptions());
  }

  return std::cref(*m_subscriber_viewer);
}

/////////////////////////////////////////////////
std::expected<std::vector<SubscriberConfig>, FailInfo>
FlatbuffersEngineDataProvider::GetSubscriberConfigs() const {
  auto viewer_result = GetSubscriberViewer();
  if (!viewer_result.has_value()) {
    return std::unexpected(viewer_result.error());
  }

  return viewer_result.value().get().GetSubscriberConfigs();
}

} // namespace steamrot
