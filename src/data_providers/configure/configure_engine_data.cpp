/////////////////////////////////////////////////
/// @file
/// @brief Implementation of functions to configure engine data.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_engine_data.h"

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
} // namespace steamrot::data::configure
