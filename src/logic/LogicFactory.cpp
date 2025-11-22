/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the LogicFactory class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "LogicFactory.h"
#include "CraftingRenderLogic.h"
#include "SubscriberFactory.h"
#include "UIActionLogic.h"
#include "UICollisionLogic.h"
#include "UIRenderLogic.h"
#include "UIStateLogic.h"
#include "logic_data_generated.h"
#include <expected>
#include <variant>

namespace steamrot {
/////////////////////////////////////////////////
LogicFactory::LogicFactory(const SceneType scene_type,
                           const SceneContext &scene_context)
    : m_scene_type(scene_type), m_scene_context(scene_context),
      m_subscriber_factory(scene_context.event_handler) {}

/////////////////////////////////////////////////
LogicCollection LogicFactory::CreateEmptyLogicCollection() {

  LogicCollection collection;
  // emplace constructs the vector in-place (no copying of unique_ptrs)
  collection.emplace(LogicType::Collision, LogicVector{});
  collection.emplace(LogicType::Action, LogicVector{});
  collection.emplace(LogicType::Movement, LogicVector{});
  collection.emplace(LogicType::Render, LogicVector{});

  return collection;
};

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
LogicFactory::TitleSceneLogicConfiguration(
    LogicCollection &logic_collection,
    const LogicCollectionData &logic_collection_data) {

  // Get Collision Logic Vector and Data
  LogicVector &collision_logics = logic_collection[LogicType::Collision];

  const LogicVectorData &collision_logic_data =
      *logic_collection_data.collision_logic_data();

  // Add CollisionLogics for Title Scene
  AddLogicToLogicVector<UICollisionLogic>(collision_logics,
                                          collision_logic_data);

  // Get Action Logic Vector and Data
  LogicVector &action_logics = logic_collection[LogicType::Action];
  const LogicVectorData &action_logic_data =
      *logic_collection_data.action_logic_data();
  // Add ActionLogics for Title Scene
  AddLogicToLogicVector<UIStateLogic>(action_logics, action_logic_data);
  AddLogicToLogicVector<UIActionLogic>(action_logics, action_logic_data);

  // Get Movement Logic Vector and Data
  LogicVector &movement_logics = logic_collection[LogicType::Movement];
  const LogicVectorData &movement_logic_data =
      *logic_collection_data.movement_logic_data();
  // Add MovementLogics for Title Scene
  //(No movement logics for title scene currently)

  // Get Render Logic Vector and Data
  LogicVector &render_logics = logic_collection[LogicType::Render];
  const LogicVectorData &render_logic_data =
      *logic_collection_data.render_logic_data();
  // Add RenderLogics for Title Scene
  AddLogicToLogicVector<UIRenderLogic>(render_logics, render_logic_data);

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
LogicFactory::CraftingSceneLogicConfiguration(
    LogicCollection &logic_collection,
    const LogicCollectionData &logic_collection_data) {

  // Get Collision Logic Vector and Data
  LogicVector &collision_logics = logic_collection[LogicType::Collision];
  const LogicVectorData &collision_logic_data =
      *logic_collection_data.collision_logic_data();

  // Add CollisionLogics for Title Scene
  AddLogicToLogicVector<UICollisionLogic>(collision_logics,
                                          collision_logic_data);

  // Get Action Logic Vector and Data
  LogicVector &action_logics = logic_collection[LogicType::Action];
  const LogicVectorData &action_logic_data =
      *logic_collection_data.action_logic_data();

  // Add ActionLogics for Title Scene
  AddLogicToLogicVector<UIStateLogic>(action_logics, action_logic_data);
  AddLogicToLogicVector<UIActionLogic>(action_logics, action_logic_data);

  // Get Movement Logic Vector and Data
  LogicVector &movement_logics = logic_collection[LogicType::Movement];
  const LogicVectorData &movement_logic_data =
      *logic_collection_data.movement_logic_data();
  // Add MovementLogics for Title Scene
  //(No movement logics for title scene currently)

  // Get Render Logic Vector and Data
  LogicVector &render_logics = logic_collection[LogicType::Render];
  const LogicVectorData &render_logic_data =
      *logic_collection_data.render_logic_data();

  // Add RenderLogics for Title Scene
  AddLogicToLogicVector<CraftingRenderLogic>(render_logics, render_logic_data);
  AddLogicToLogicVector<UIRenderLogic>(render_logics, render_logic_data);

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
LogicFactory::TestSceneLogicConfiguration(
    LogicCollection &logic_collection,
    const LogicCollectionData &logic_collection_data) {

  // Get Collision Logic Vector and Data
  LogicVector &collision_logics = logic_collection[LogicType::Collision];
  const LogicVectorData &collision_logic_data =
      *logic_collection_data.collision_logic_data();

  // Add CollisionLogics for Test Scene
  AddLogicToLogicVector<UICollisionLogic>(collision_logics,
                                          collision_logic_data);

  // Get Action Logic Vector and Data
  LogicVector &action_logics = logic_collection[LogicType::Action];
  const LogicVectorData &action_logic_data =
      *logic_collection_data.action_logic_data();

  // Add ActionLogics for Test Scene
  AddLogicToLogicVector<UIStateLogic>(action_logics, action_logic_data);
  AddLogicToLogicVector<UIActionLogic>(action_logics, action_logic_data);

  // Get Movement Logic Vector and Data
  LogicVector &movement_logics = logic_collection[LogicType::Movement];
  const LogicVectorData &movement_logic_data =
      *logic_collection_data.movement_logic_data();

  // Add MovementLogics for Test Scene
  //(No movement logics for test scene currently)

  // Get Render Logic Vector and Data
  LogicVector &render_logics = logic_collection[LogicType::Render];
  const LogicVectorData &render_logic_data =
      *logic_collection_data.render_logic_data();

  // Add RenderLogics for Test Scene
  AddLogicToLogicVector<UIRenderLogic>(render_logics, render_logic_data);

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<LogicCollection, FailInfo>
LogicFactory::CreateLogicMap(const LogicCollectionData &logic_collection_data) {

  // create empty logic collection
  LogicCollection logic_collection = CreateEmptyLogicCollection();

  // configure logic collection based on scene type
  switch (m_scene_type) {
  case SceneType::SceneType_TITLE: {

    auto configuration_result =
        TitleSceneLogicConfiguration(logic_collection, logic_collection_data);

    // UNEXPECTED PROPOGATION
    if (!configuration_result.has_value()) {
      return std::unexpected(configuration_result.error());
    }

    break;
  }
  case SceneType::SceneType_CRAFTING: {

    auto configuration_result = CraftingSceneLogicConfiguration(
        logic_collection, logic_collection_data);

    // UNEXPECTED PROPOGATION
    if (!configuration_result.has_value()) {
      return std::unexpected(configuration_result.error());
    }

    break;
  }
  case SceneType::SceneType_TEST: {
    auto configuration_result =
        TestSceneLogicConfiguration(logic_collection, logic_collection_data);
    // UNEXPECTED PROPOGATION
    if (!configuration_result.has_value()) {
      return std::unexpected(configuration_result.error());
    }
    break;
  }
  default:
    return std::unexpected(
        FailInfo{FailMode::NonExistentEnumValue,
                 "Unsupported scene type for logic configuration"});
  }
  return logic_collection;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
LogicFactory::AttachSubscribers(Logic &logic, const LogicData *logic_data) {

  // Check if logic_data is null
  if (!logic_data) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound, "LogicData is null"});
  }

  // Check if there are any subscribers to attach
  if (!logic_data->all_subscriptions()) {
    // No subscribers to attach, return success
    return std::monostate{};
  }

  // Iterate through subscriber data and create subscribers
  for (const auto *subscriber_data : *logic_data->all_subscriptions()) {
    if (!subscriber_data) {
      continue; // Skip null subscriber data
    }

    // Create and register subscriber
    auto subscriber_result =
        m_subscriber_factory.CreateAndRegisterSubscriber(*subscriber_data);
    if (!subscriber_result.has_value()) {
      return std::unexpected(subscriber_result.error());
    }

    // Add subscriber to the logic instance
    logic.AddSubscriber(subscriber_result.value());
  }

  return std::monostate{};
}

} // namespace steamrot
