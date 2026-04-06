/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the LogicFactory class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "LogicFactory.h"
#include "FailInfo.h"
#include "GrimoireMachinaActionLogic.h"
#include "GrimoireMachinaCollisionLogic.h"
#include "GrimoireMachinaPositioningLogic.h"
#include "GrimoireMachinaRenderLogic.h"
#include "LogicType.h"
#include "UIActionLogic.h"
#include "UICollisionLogic.h"
#include "UIRenderLogic.h"
#include "UIStateLogic.h"
#include <array>
#include <expected>
#include <memory>

namespace steamrot::logic {

/////////////////////////////////////////////////
LogicFactory::LogicFactory(const SceneContext &scene_context)
    : m_scene_context(scene_context) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
LogicFactory::AddLogicsToCollection(LogicCollection &logic_collection,
                                    LogicGrouping grouping,
                                    const std::vector<LogicType> &logic_types) {
  LogicVector &logics = logic_collection[grouping];
  for (const LogicType &logic_type : logic_types) {
    // create the Logic object
    auto logic_result = CreateLogicObject(logic_type);
    if (!logic_result) {
      return std::unexpected(logic_result.error());
    }

    // configure the Logic object
    auto configure_result = ConfigureLogicObject(*logic_result.value());
    if (!configure_result) {
      return std::unexpected(configure_result.error());
    }

    // add the configured Logic object to the appropriate vector in the
    // LogicCollection
    logics.push_back(std::move(logic_result.value()));
  }
  return std::monostate();
}

/////////////////////////////////////////////////
std::expected<LogicCollection, FailInfo>
LogicFactory::ProvideLogicCollection(SceneType scene_type) {

  // create an empty LogicCollection to modify
  LogicCollection logic_collection = CreateEmptyLogicCollection();

  // configure the LogicCollection based on the SceneType
  switch (scene_type) {
  case SceneType::TITLE: {
    auto result = ConfigureTitleLogics(logic_collection);
    if (!result) {
      return std::unexpected(result.error());
    }
    break;
  }
  case SceneType::CRAFTING: {
    auto result = ConfigureCraftingLogics(logic_collection);
    if (!result) {
      return std::unexpected(result.error());
    }
    break;
  }
  case SceneType::TEST: {
    auto result = ConfigureTestLogics(logic_collection);
    if (!result) {
      return std::unexpected(result.error());
    }
    break;
  }
  case SceneType::UI_EXPLORER: {
    // UIExplorerScene handles all logic directly via its sRender/sCollision/
    // sAction overrides.  No Logic objects are needed.
    break;
  }

  // other SceneTypes can be added here
  default:
    return std::unexpected(
        FailInfo(FailMode::SceneTypeNotFound,
                 "LogicFactory::ProvideLogicCollection: unknown SceneType"));
  }
  return logic_collection;
}

/////////////////////////////////////////////////
LogicCollection LogicFactory::CreateEmptyLogicCollection() {

  LogicCollection collection;
  // emplace constructs the vector in-place (no copying of unique_ptrs)
  collection.emplace(LogicGrouping::Collision, LogicVector{});
  collection.emplace(LogicGrouping::Action, LogicVector{});
  collection.emplace(LogicGrouping::Render, LogicVector{});
  collection.emplace(LogicGrouping::Movement, LogicVector{});

  return collection;
};

/////////////////////////////////////////////////
std::expected<std::unique_ptr<Logic>, FailInfo>
LogicFactory::CreateLogicObject(LogicType logic_type) {

  // create base pointer
  std::unique_ptr<Logic> logic_ptr;
  switch (logic_type) {
  case LogicType::UIRender:
    logic_ptr = std::make_unique<UIRenderLogic>(m_scene_context);
    break;
  case LogicType::UIState:
    logic_ptr = std::make_unique<UIStateLogic>(m_scene_context);
    break;
  case LogicType::UIAction:
    logic_ptr = std::make_unique<UIActionLogic>(m_scene_context);
    break;
  case LogicType::UICollision:
    logic_ptr = std::make_unique<UICollisionLogic>(m_scene_context);
    break;
  case LogicType::GrimoireMachinaAction:
    logic_ptr =
        std::make_unique<logic::GrimoireMachinaActionLogic>(m_scene_context);
    break;
  case LogicType::GrimoireMachinaPositioning:
    logic_ptr = std::make_unique<logic::GrimoireMachinaPositioningLogic>(
        m_scene_context);
    break;
  case LogicType::GrimoireMachinaRender:
    logic_ptr =
        std::make_unique<logic::GrimoireMachinaRenderLogic>(m_scene_context);
    break;
  case LogicType::GrimoireMachinaCollision:
    logic_ptr =
        std::make_unique<logic::GrimoireMachinaCollisionLogic>(m_scene_context);
    break;
  default:
    return std::unexpected(
        FailInfo(FailMode::EnumValueNotHandled,
                 "LogicFactory::CreateLogicObject: unknown LogicType"));
  }

  return logic_ptr;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
LogicFactory::ConfigureLogicObject(Logic &logic_object) {

  // get the LogicConfigCollection provider from the data access factory
  auto provider_result =
      m_scene_context.data_access_factory.GetLogicConfigCollectionProvider();
  if (!provider_result) {
    return std::unexpected(provider_result.error());
  }
  ILogicConfigCollectionProvider &provider = *provider_result.value();

  // get the LogicConfigCollection from the provider
  auto collection_result = provider.CreateLogicConfigCollection();
  if (!collection_result) {
    return std::unexpected(collection_result.error());
  }
  const LogicConfigCollection &logic_config_collection =
      collection_result.value();

  // check if LogicType has been set on the derived Logic object
  if (logic_object.GetLogicType() == LogicType::None) {
    return std::unexpected(FailInfo(FailMode::NotImplemented,
                                    "LogicFactory::ConfigureLogicObject: "
                                    "LogicType not set on Logic object"));
  }

  // find the LogicConfig for this Logic's LogicType and configure the Logic
  // object accordingly
  auto config_it = logic_config_collection.find(logic_object.GetLogicType());
  if (config_it == logic_config_collection.end()) {
    // if not found, return early. Does not need to have a config, so not an
    // error.
    return std::monostate{};
  }

  const LogicConfig &config = config_it->second;

  // set Subscribers
  for (const auto &subscriber : config.m_subscribers) {

    // add to logc object subscribers vector
    logic_object.AddSubscriber(subscriber);
    // add to EventHandler
    auto register_result =
        m_scene_context.event_handler.RegisterSubscriber(subscriber);
    if (!register_result.has_value()) {
      return std::unexpected(register_result.error());
    }
  }
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
LogicFactory::ConfigureTitleLogics(LogicCollection &logic_collection) {

  ////// THE ORDER OF THE LOGIC CLASSES IS VERY IMPORTANT //////
  ////// DO NOT CHANGE UNLESS YOU KNOW WHAT YOU ARE DOING //////

  // Define the Logic types for each grouping in the order they should execute
  // These are compile-time constants that define the scene's Logic
  // configuration
  static constexpr std::array collision_logic_types = {LogicType::UICollision};
  static constexpr std::array action_logic_types = {LogicType::UIAction,
                                                    LogicType::UIState};
  static constexpr std::array render_logic_types = {LogicType::UIRender};

  // Add Logics to collection using the helper function
  auto collision_result = AddLogicsToCollection(
      logic_collection, LogicGrouping::Collision,
      std::vector<LogicType>(collision_logic_types.begin(),
                             collision_logic_types.end()));
  if (!collision_result) {
    return std::unexpected(collision_result.error());
  }

  auto action_result =
      AddLogicsToCollection(logic_collection, LogicGrouping::Action,
                            std::vector<LogicType>(action_logic_types.begin(),
                                                   action_logic_types.end()));
  if (!action_result) {
    return std::unexpected(action_result.error());
  }

  auto render_result =
      AddLogicsToCollection(logic_collection, LogicGrouping::Render,
                            std::vector<LogicType>(render_logic_types.begin(),
                                                   render_logic_types.end()));
  if (!render_result) {
    return std::unexpected(render_result.error());
  }

  return std::monostate();
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
LogicFactory::ConfigureCraftingLogics(LogicCollection &logic_collection) {
  ////// THE ORDER OF THE LOGIC CLASSES IS VERY IMPORTANT //////
  ////// DO NOT CHANGE UNLESS YOU KNOW WHAT YOU ARE DOING //////

  // Define the Logic types for each grouping in the order they should execute
  // These are compile-time constants that define the scene's Logic
  // configuration
  static constexpr std::array collision_logic_types = {
      LogicType::UICollision, LogicType::GrimoireMachinaCollision};

  static constexpr std::array action_logic_types = {
      LogicType::UIAction, LogicType::UIState,
      LogicType::GrimoireMachinaAction};

  static constexpr std::array render_logic_types = {
      LogicType::UIRender, LogicType::GrimoireMachinaRender};

  static constexpr std::array movement_logic_types = {
      LogicType::GrimoireMachinaPositioning};

  // Add Logics to collection using the helper function
  auto collision_result = AddLogicsToCollection(
      logic_collection, LogicGrouping::Collision,
      std::vector<LogicType>(collision_logic_types.begin(),
                             collision_logic_types.end()));
  if (!collision_result) {
    return std::unexpected(collision_result.error());
  }

  auto action_result =
      AddLogicsToCollection(logic_collection, LogicGrouping::Action,
                            std::vector<LogicType>(action_logic_types.begin(),
                                                   action_logic_types.end()));
  if (!action_result) {
    return std::unexpected(action_result.error());
  }

  auto render_result =
      AddLogicsToCollection(logic_collection, LogicGrouping::Render,
                            std::vector<LogicType>(render_logic_types.begin(),
                                                   render_logic_types.end()));
  if (!render_result) {
    return std::unexpected(render_result.error());
  }

  auto movement_result =
      AddLogicsToCollection(logic_collection, LogicGrouping::Movement,
                            std::vector<LogicType>(movement_logic_types.begin(),
                                                   movement_logic_types.end()));
  if (!movement_result) {
    return std::unexpected(movement_result.error());
  }

  return std::monostate();
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
LogicFactory::ConfigureTestLogics(LogicCollection &logic_collection) {
  ////// THE ORDER OF THE LOGIC CLASSES IS VERY IMPORTANT //////
  ////// DO NOT CHANGE UNLESS YOU KNOW WHAT YOU ARE DOING //////

  // Define the Logic types for each grouping in the order they should execute
  // These are compile-time constants that define the scene's Logic
  // configuration
  static constexpr std::array collision_logic_types = {LogicType::UICollision};
  static constexpr std::array action_logic_types = {LogicType::UIAction};
  static constexpr std::array render_logic_types = {LogicType::UIRender};

  // Add Logics to collection using the helper function
  auto collision_result = AddLogicsToCollection(
      logic_collection, LogicGrouping::Collision,
      std::vector<LogicType>(collision_logic_types.begin(),
                             collision_logic_types.end()));
  if (!collision_result) {
    return std::unexpected(collision_result.error());
  }

  auto action_result =
      AddLogicsToCollection(logic_collection, LogicGrouping::Action,
                            std::vector<LogicType>(action_logic_types.begin(),
                                                   action_logic_types.end()));
  if (!action_result) {
    return std::unexpected(action_result.error());
  }

  auto render_result =
      AddLogicsToCollection(logic_collection, LogicGrouping::Render,
                            std::vector<LogicType>(render_logic_types.begin(),
                                                   render_logic_types.end()));
  if (!render_result) {
    return std::unexpected(render_result.error());
  }

  return std::monostate();
}
} // namespace steamrot::logic
