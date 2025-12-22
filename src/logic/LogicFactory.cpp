/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the LogicFactory class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "LogicFactory.h"
#include "FailInfo.h"
#include "UIActionLogic.h"
#include "UICollisionLogic.h"
#include "UIRenderLogic.h"
#include "UIStateLogic.h"
#include <expected>
#include <memory>

namespace steamrot {

/////////////////////////////////////////////////
LogicFactory::LogicFactory(const SceneContext &scene_context)
    : m_scene_context(scene_context) {}

/////////////////////////////////////////////////
std::expected<LogicCollection, FailInfo>
LogicFactory::ProvideLogicCollection(SceneType scene_type) {

  // create an empty LogicCollection to modify
  LogicCollection logic_collection = CreateEmptyLogicCollection();

  // configure the LogicCollection based on the SceneType
  switch (scene_type) {
  case SceneType::SceneType_TITLE: {
    auto result = ConfigureTitleLogics(logic_collection);
    if (!result) {
      return std::unexpected(result.error());
    }
    break;
  }
  case SceneType::SceneType_CRAFTING: {
    auto result = ConfigureTitleLogics(logic_collection);
    if (!result) {
      return std::unexpected(result.error());
    }
    break;
  }
  case SceneType::SceneType_TEST: {
    auto result = ConfigureTestLogics(logic_collection);
    if (!result) {
      return std::unexpected(result.error());
    }
    break;
  }

  // other SceneTypes can be added here
  default:
    return std::unexpected(
        FailInfo(FailMode::SceneTypeNotFound,
                 "LogicFactory::ProvideLogicCollection, unknown SceneType"));
  }
  return logic_collection;
}
/////////////////////////////////////////////////
LogicCollection LogicFactory::CreateEmptyLogicCollection() {

  LogicCollection collection;
  // emplace constructs the vector in-place (no copying of unique_ptrs)
  collection.emplace(LogicType::Collision, LogicVector{});
  collection.emplace(LogicType::Action, LogicVector{});
  collection.emplace(LogicType::Render, LogicVector{});

  return collection;
};

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
LogicFactory::ConfigureTitleLogics(LogicCollection &logic_collection) {

  ////// THE ORDER OF THE LOGIC CLASSES IS VERY IMPORTANT //////
  ////// DO NOT CHANGE UNLESS YOU KNOW WHAT YOU ARE DOING //////

  /////// ADD COLLISION LOGICS /////
  LogicVector &collision_logics = logic_collection[LogicType::Collision];
  collision_logics.push_back(
      std::make_unique<UICollisionLogic>(m_scene_context));

  /////// ADD ACTION LOGICS /////
  LogicVector &action_logics = logic_collection[LogicType::Action];
  action_logics.push_back(
      std::make_unique<steamrot::UIActionLogic>(m_scene_context));
  action_logics.push_back(
      std::make_unique<steamrot::UIStateLogic>(m_scene_context));

  ////// ADD RENDER LOGICS /////
  LogicVector &render_logics = logic_collection[LogicType::Render];
  render_logics.push_back(
      std::make_unique<steamrot::UIRenderLogic>(m_scene_context));
  return std::monostate();
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
LogicFactory::ConfigureCraftingLogics(LogicCollection &logic_collection) {
  ////// THE ORDER OF THE LOGIC CLASSES IS VERY IMPORTANT //////
  ////// DO NOT CHANGE UNLESS YOU KNOW WHAT YOU ARE DOING //////

  /////// ADD COLLISION LOGICS /////
  LogicVector &collision_logics = logic_collection[LogicType::Collision];
  collision_logics.push_back(
      std::make_unique<UICollisionLogic>(m_scene_context));

  /////// ADD ACTION LOGICS /////
  LogicVector &action_logics = logic_collection[LogicType::Action];
  action_logics.push_back(
      std::make_unique<steamrot::UIActionLogic>(m_scene_context));
  action_logics.push_back(
      std::make_unique<steamrot::UIStateLogic>(m_scene_context));

  ////// ADD RENDER LOGICS /////
  LogicVector &render_logics = logic_collection[LogicType::Render];
  render_logics.push_back(
      std::make_unique<steamrot::UIRenderLogic>(m_scene_context));
  return std::monostate();
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
LogicFactory::ConfigureTestLogics(LogicCollection &logic_collection) {
  ////// THE ORDER OF THE LOGIC CLASSES IS VERY IMPORTANT //////
  ////// DO NOT CHANGE UNLESS YOU KNOW WHAT YOU ARE DOING //////
  /////// ADD COLLISION LOGICS /////
  LogicVector &collision_logics = logic_collection[LogicType::Collision];
  collision_logics.push_back(
      std::make_unique<UICollisionLogic>(m_scene_context));
  /////// ADD ACTION LOGICS /////
  LogicVector &action_logics = logic_collection[LogicType::Action];
  action_logics.push_back(
      std::make_unique<steamrot::UIActionLogic>(m_scene_context));
  ////// ADD RENDER LOGICS /////
  LogicVector &render_logics = logic_collection[LogicType::Render];
  render_logics.push_back(
      std::make_unique<steamrot::UIRenderLogic>(m_scene_context));
  return std::monostate();
}
} // namespace steamrot
