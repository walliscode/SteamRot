/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersLogicConfigCollectionProvider class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersLogicConfigCollectionProvider.h"
#include "FlatbuffersDataLoader.h"
#include "configure_logic_config.h"

namespace steamrot {
/////////////////////////////////////////////////
std::expected<LogicConfigCollection, FailInfo>
FlatbuffersLogicConfigCollectionProvider::CreateLogicConfigCollection() const {
  LogicConfigCollection logic_config_collection;
  auto result = ConfigureLogicConfigCollection(logic_config_collection);
  if (!result) {
    return std::unexpected(result.error());
  }
  return logic_config_collection;
}

std::expected<std::monostate, FailInfo>
FlatbuffersLogicConfigCollectionProvider::ConfigureLogicConfigCollection(
    LogicConfigCollection &logic_config_collection) const {

  // initiate data loader and get logic config data
  FlatbuffersDataLoader data_loader;
  auto result = data_loader.ProvideLogicConfigCollectionFbs();
  if (!result) {
    return std::unexpected(result.error());
  }
  const LogicConfigCollectionFbs *collection = result.value();

  // check for each LogicConfigFbs manually
  // Enter more as needed
  if (collection->test_logic()) {
    // create a LogicConfig object and configure it using the data from the
    // Flatbuffers
    LogicConfig logic_config;
    auto configure_result = data::configure::ConfigureLogicConfig(
        logic_config, collection->test_logic());
    if (!configure_result) {
      return std::unexpected(configure_result.error());
    }
    logic_config_collection[steamrot::LogicType::Test] = logic_config;
  }
  if (collection->grimoire_machina_action_logic()) {
    LogicConfig logic_config;
    auto configure_result = data::configure::ConfigureLogicConfig(
        logic_config, collection->grimoire_machina_action_logic());
    if (!configure_result) {
      return std::unexpected(configure_result.error());
    }
    logic_config_collection[steamrot::LogicType::GrimoireMachinaAction] =
        logic_config;
  }
  if (collection->ghost_action_logic()) {
    LogicConfig logic_config;
    auto configure_result = data::configure::ConfigureLogicConfig(
        logic_config, collection->ghost_action_logic());
    if (!configure_result) {
      return std::unexpected(configure_result.error());
    }
    logic_config_collection[steamrot::LogicType::GhostAction] = logic_config;
  }
  if (collection->camera_action_logic()) {
    LogicConfig logic_config;
    auto configure_result = data::configure::ConfigureLogicConfig(
        logic_config, collection->camera_action_logic());
    if (!configure_result) {
      return std::unexpected(configure_result.error());
    }
    logic_config_collection[steamrot::LogicType::Camera] = logic_config;
  }

  return std::monostate{};
}
} // namespace steamrot
