/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersEngineDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersEngineDataProvider.h"
#include "core_data_generated.h"

namespace steamrot {

/////////////////////////////////////////////////
std::expected<EngineCoreData, FailInfo>
FlatbuffersEngineDataProvider::LoadEngineCoreData() const {
  // Use existing loader
  auto fb_result = m_loader.ProvideEngineCoreData();
  if (!fb_result.has_value()) {
    return std::unexpected(fb_result.error());
  }

  const auto *fb_data = fb_result.value();

  // Convert FlatBuffers type to native struct
  EngineCoreData native_data;
  native_data.window_width = fb_data->window_width();
  native_data.window_height = fb_data->window_height();
  if (fb_data->window_title()) {
    native_data.window_title = fb_data->window_title()->str();
  }
  native_data.framerate_limit = fb_data->framerate_limit();

  return native_data;
}

/////////////////////////////////////////////////
std::expected<EngineData, FailInfo>
FlatbuffersEngineDataProvider::LoadEngineData() const {
  auto core_result = LoadEngineCoreData();
  if (!core_result.has_value()) {
    return std::unexpected(core_result.error());
  }

  EngineData engine_data;
  engine_data.core = core_result.value();
  // Future: Load other engine data

  return engine_data;
}

} // namespace steamrot
