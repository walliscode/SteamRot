/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersEngineDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersEngineDataProvider.h"
#include "EngineState.h"
#include "FailInfo.h"
#include "asset_config_factory.h"
#include "engine_resources_config_generated.h"
#include "subscriber_factory.h"

namespace steamrot {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersEngineDataProvider::PopulateEngineResourcesConfig(
    EngineResourcesConfig &engine_resources_config,
    const EngineResourcesConfigFbs *engine_resources_config_data) const {

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
FlatbuffersEngineDataProvider::PopulateEngineConfig(
    EngineConfig &engine_config,
    const EngineConfigFbs *engine_config_data) const {
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
FlatbuffersEngineDataProvider::PopulateEngineState(
    EngineState &engine_state, const EngineStateFbs *engine_state_data) const {
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
    auto create_result = subscriber_factory::CreateSubscriber(subscriber_fbs);
    if (!create_result.has_value()) {
      return std::unexpected(create_result.error());
    }
    engine_state.subscriptions.push_back(
        std::make_shared<Subscriber>(create_result.value()));
  }
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersEngineDataProvider::PopulateInitialAssetConfig(
    AssetConfig &asset_config, const AssetConfigFbs *asset_config_data) const {

  // check for null data
  if (!asset_config_data) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "AssetConfigFbs data is null, cannot populate AssetConfig"});
  }
  // use factory function to populate AssetConfig from flatbuffers data
  auto populate_result = ConfigureAssetConfig(asset_config, asset_config_data);
  if (!populate_result.has_value()) {
    return std::unexpected(populate_result.error());
  }
  return std::monostate{};
}
/////////////////////////////////////////////////
std::expected<EngineData, FailInfo>
FlatbuffersEngineDataProvider::LoadEngineData() const {
  EngineData engine_data;

  // use flatbuffers data loader to EngineDataFbs
  auto load_engine_data_reult = m_loader.ProvideEngineDataFbs();
  if (!load_engine_data_reult.has_value()) {
    return std::unexpected(load_engine_data_reult.error());
  }
  const EngineDataFbs *fb_engine_data = load_engine_data_reult.value();

  // create return object
  EngineData data;

  // populate EngineResourcesConfig
  auto populate_erc_result = PopulateEngineResourcesConfig(
      data.engine_resources_config, fb_engine_data->engine_resources_config());
  // check for errors
  if (!populate_erc_result.has_value()) {
    return std::unexpected(populate_erc_result.error());
  }
  // populate EngineConfig
  auto populate_ec_result =
      PopulateEngineConfig(data.engine_config, fb_engine_data->engine_config());
  // check for errors
  if (!populate_ec_result.has_value()) {
    return std::unexpected(populate_ec_result.error());
  }
  // populate EngineState
  auto populate_es_result =
      PopulateEngineState(data.engine_state, fb_engine_data->engine_state());
  // check for errors
  if (!populate_es_result.has_value()) {
    return std::unexpected(populate_es_result.error());
  }

  // populate Initial AssetConfig
  auto populate_iac_result = PopulateInitialAssetConfig(
      data.initial_asset_config, fb_engine_data->initial_asset_config());
  // check for errors
  if (!populate_iac_result.has_value()) {
    return std::unexpected(populate_iac_result.error());
  }

  return data;
}

} // namespace steamrot
