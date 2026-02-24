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
#include "FlatbuffersGrimoireMachinaProvider.h"
#include "FlatbuffersInputActionConfigProvider.h"
#include "FlatbuffersLogicConfigCollectionProvider.h"
#include "FlatbuffersSceneDataProvider.h"
#include "FlatbuffersSceneManagerDataProvider.h"
#include "ILogicConfigCollectionProvider.h"
#include <expected>

namespace steamrot {
/////////////////////////////////////////////////
DataAccessFactory::DataAccessFactory(EventHandler &event_handler,
                                     const DataType data_type)
    : m_event_handler(event_handler), m_data_type(data_type) {

  auto set_providers_result = SetDataProviders();
  if (!set_providers_result) {
    throw std::runtime_error(
        "DataAccessFactory::DataAccessFactory: Failed to set data providers: " +
        set_providers_result.error().message);
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

  // set ISceneDataProvider
  m_scene_data_provider =
      std::make_unique<FlatbuffersSceneDataProvider>(m_event_handler);
  if (!m_scene_data_provider) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer,
                 "Failed to create FlatbuffersSceneDataProvider instance."});
  }

  // set IGrimoireMachinaProvider
  m_grimoire_machina_provider =
      std::make_unique<FlatbuffersGrimoireMachinaProvider>();
  if (!m_grimoire_machina_provider) {
    return std::unexpected(FailInfo{
        FailMode::NullPointer,
        "Failed to create FlatbuffersGrimoireMachinaProvider instance."});
  }

  // set ILogicConfigCollectionProvider
  m_logic_config_collection_provider =
      std::make_unique<FlatbuffersLogicConfigCollectionProvider>();
  if (!m_logic_config_collection_provider) {
    return std::unexpected(FailInfo{
        FailMode::NullPointer,
        "Failed to create FlatbuffersLogicConfigCollectionProvider instance."});
  }

  // Set IInputActionConfigProvider
  m_input_action_config_provider =
      std::make_unique<FlatbuffersInputActionConfigProvider>();
  if (!m_input_action_config_provider) {
    return std::unexpected(FailInfo{
        FailMode::NullPointer,
        "Failed to create FlatbuffersInputActionConfigProvider instance."});
  }

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

/////////////////////////////////////////////////
std::expected<ISceneDataProvider *, FailInfo>
DataAccessFactory::GetSceneDataProvider() {
  if (!m_scene_data_provider) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer, "Scene Data Provider is null"});
  }

  return m_scene_data_provider.get();
}
/////////////////////////////////////////////////
std::expected<IGrimoireMachinaProvider *, FailInfo>
DataAccessFactory::GetGrimoireMachinaProvider() {
  if (!m_grimoire_machina_provider) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer, "Grimoire Machina Provider is null"});
  }
  return m_grimoire_machina_provider.get();
}

/////////////////////////////////////////////////
std::expected<ILogicConfigCollectionProvider *, FailInfo>
DataAccessFactory::GetLogicConfigCollectionProvider() {
  if (!m_logic_config_collection_provider) {
    return std::unexpected(FailInfo{
        FailMode::NullPointer, "Logic Config Collection Provider is null"});
  }
  return m_logic_config_collection_provider.get();
}

/////////////////////////////////////////////////
std::expected<IInputActionConfigProvider *, FailInfo>
DataAccessFactory::GetInputActionConfigProvider() {
  if (!m_input_action_config_provider) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer,
                 "Input Action Config Provider is null"});
  }
  return m_input_action_config_provider.get();
}
} // namespace steamrot
