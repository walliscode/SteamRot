/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersEngineDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersEngineDataProvider.h"
#include "FailInfo.h"
#include "configure_asset_config.h"
#include "configure_engine_data.h"

namespace steamrot {

/////////////////////////////////////////////////
std::expected<EngineData, FailInfo>
FlatbuffersEngineDataProvider::CreateEngineData() const {

  // create return object
  EngineData data;

  // configure engine data
  auto configure_result = ConfigureEngineData(data);
  if (!configure_result.has_value()) {
    return std::unexpected(configure_result.error());
  }

  return data;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersEngineDataProvider::ConfigureEngineData(
    EngineData &engine_data) const {

  // use flatbuffers data loader to EngineDataFbs
  auto load_engine_data_reult = m_loader.ProvideEngineDataFbs();
  if (!load_engine_data_reult.has_value()) {
    return std::unexpected(load_engine_data_reult.error());
  }
  const EngineDataFbs *fb_engine_data = load_engine_data_reult.value();

  // populate EngineResourcesConfig
  auto populate_erc_result = data::configure::ConfigureEngineResourcesConfig(
      engine_data.engine_resources_config,
      fb_engine_data->engine_resources_config());
  // check for errors
  if (!populate_erc_result.has_value()) {
    return std::unexpected(populate_erc_result.error());
  }
  // populate EngineConfig
  auto populate_ec_result = data::configure::ConfigureEngineConfig(
      engine_data.engine_config, fb_engine_data->engine_config());
  // check for errors
  if (!populate_ec_result.has_value()) {
    return std::unexpected(populate_ec_result.error());
  }
  // populate EngineState
  auto populate_es_result = data::configure::ConfigureEngineState(
      engine_data.engine_state, fb_engine_data->engine_state());
  // check for errors
  if (!populate_es_result.has_value()) {
    return std::unexpected(populate_es_result.error());
  }

  // populate Initial AssetConfig
  auto populate_iac_result = data::configure::ConfigureAssetConfig(
      engine_data.initial_asset_config, fb_engine_data->initial_asset_config());
  // check for errors
  if (!populate_iac_result.has_value()) {
    return std::unexpected(populate_iac_result.error());
  }
  return std::monostate{};
}
} // namespace steamrot
