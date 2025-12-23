/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the DataAccessFactory class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "DataAccessFactory.h"
#include "FailInfo.h"
#include "FlatbuffersEngineDataProvider.h"
#include "FlatbuffersSceneManagerDataProvider.h"
#include <expected>

namespace steamrot {
/////////////////////////////////////////////////
DataAccessFactory::DataAccessFactory(const DataType data_type)
    : m_data_type(data_type) {

  auto set_providers_result = SetDataProviders();
  if (!set_providers_result) {
    throw std::runtime_error(
        "DataAccessFactory::DataAccessFactory: Failed to set data providers: " +
        set_providers_result.error().message);
  }

  auto set_configurators_result = SetDataConfigurators();
  if (!set_configurators_result) {
    throw std::runtime_error(
        "DataAccessFactory::DataAccessFactory: Failed to set data "
        "configurators: " +
        set_configurators_result.error().message);
  }
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
DataAccessFactory::SetFlatbuffersDataProviders() {

  // set IEngineDataProvider
  m_engine_data_provider = std::make_unique<FlatbuffersEngineDataProvider>();
  if (!m_engine_data_provider) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer,
                 "Failed to create FlatbuffersEngineDataProvider instance."});
  }

  // set ISceneManagerDataProvider
  m_scene_manager_data_provider =
      std::make_unique<FlatbuffersSceneManagerDataProvider>();
  if (!m_scene_manager_data_provider) {
    return std::unexpected(FailInfo{
        FailMode::NullPointer,
        "Failed to create FlatbuffersSceneManagerDataProvider instance."});
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
DataAccessFactory::SetFlatbuffersDataConfigurators() {
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> DataAccessFactory::SetDataProviders() {

  // set the data providers based on the data type
  switch (m_data_type) {
  case DataType::Flatbuffers: {

    auto set_fb_providers = SetFlatbuffersDataProviders();
    if (!set_fb_providers.has_value()) {
      return std::unexpected(set_fb_providers.error());
    }

    break;
  }

  default:
    return std::unexpected(
        FailInfo{FailMode::EnumValueNotHandled,
                 "Unsupported data type in SetDataProviders."});
  }
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
DataAccessFactory::SetDataConfigurators() {
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
DataAccessFactory::SetDataType(const DataType data_type) {

  // if the data type is the same, do nothing
  if (m_data_type == data_type) {
    return std::monostate{};
  }
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<IEngineDataProvider *, FailInfo>
DataAccessFactory::GetEngineDataProvider() {

  if (!m_engine_data_provider) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer, "Engine Data Provider is null"});
  }

  return m_engine_data_provider.get();
}

/////////////////////////////////////////////////
std::expected<ISceneManagerDataProvider *, FailInfo>
DataAccessFactory::GetSceneManagerDataProvider() {
  if (!m_scene_manager_data_provider) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer, "Scene Manager Data Provider is null"});
  }

  return m_scene_manager_data_provider.get();
}
} // namespace steamrot
