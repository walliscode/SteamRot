/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the LogicFactory class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "LogicFactory.h"
#include "CraftingRenderLogic.h"
#include "UIActionLogic.h"
#include "UICollisionLogic.h"
#include "UIRenderLogic.h"
#include <expected>

namespace steamrot {
/////////////////////////////////////////////////
LogicFactory::LogicFactory(const SceneType scene_type,
                           const LogicContext &logic_context)
    : m_scene_type(scene_type), m_logic_context(logic_context) {}

/////////////////////////////////////////////////
std::expected<LogicCollection, FailInfo> LogicFactory::CreateLogicMap() {

  // create return object
  LogicCollection logic_collection;

  // create collision logics
  auto collision_logics = CreateCollisionLogics();
  if (!collision_logics.has_value()) {
    return std::unexpected(collision_logics.error());
  }
  logic_collection[LogicType::Collision] = std::move(collision_logics.value());

  // create render logics
  auto render_logics = CreateRenderLogics();
  if (!render_logics.has_value()) {
    return std::unexpected(render_logics.error());
  }
  logic_collection[LogicType::Render] = std::move(render_logics.value());

  // create action logics
  auto action_logics = CreateActionLogics();
  if (!action_logics.has_value()) {
    return std::unexpected(action_logics.error());
  }
  logic_collection[LogicType::Action] = std::move(action_logics.value());

  return logic_collection;
}

/////////////////////////////////////////////////
std::expected<LogicVector, FailInfo> LogicFactory::CreateRenderLogics() {

  LogicVector render_logics;

  switch (m_scene_type) {
  case SceneType::SceneType_TITLE: {
    render_logics.push_back(std::make_unique<UIRenderLogic>(m_logic_context));
    break;
  }
  case SceneType::SceneType_CRAFTING: {
    render_logics.push_back(
        std::make_unique<CraftingRenderLogic>(m_logic_context));
    render_logics.push_back(std::make_unique<UIRenderLogic>(m_logic_context));
    break;
  }
  case SceneType::SceneType_TEST: {
    // add render logics for test purposes
    render_logics.push_back(std::make_unique<UIRenderLogic>(m_logic_context));
    break;
  }
  default:
    return std::unexpected(FailInfo{FailMode::NonExistentEnumValue,
                                    "Unsupported scene type for render logic"});
  }
  return render_logics;
}
/////////////////////////////////////////////////
std::expected<LogicVector, FailInfo> LogicFactory::CreateCollisionLogics() {

  LogicVector collision_logics;

  switch (m_scene_type) {
  case SceneType::SceneType_TITLE: {

    collision_logics.push_back(
        std::make_unique<UICollisionLogic>(m_logic_context));
    break;
  }
  case SceneType::SceneType_CRAFTING: {

    collision_logics.push_back(
        std::make_unique<UICollisionLogic>(m_logic_context));
    break;
  }
  case SceneType::SceneType_TEST: {

    // add collision logics for test purposes
    collision_logics.push_back(
        std::make_unique<UICollisionLogic>(m_logic_context));

    break;
  }
  default:
    return std::unexpected(
        FailInfo{FailMode::NonExistentEnumValue,
                 "Unsupported scene type for collision logic"});
  }
  return collision_logics;
}

/////////////////////////////////////////////////
std::expected<LogicVector, FailInfo> LogicFactory::CreateActionLogics() {

  LogicVector action_logics;

  switch (m_scene_type) {
  case SceneType::SceneType_TITLE: {

    action_logics.push_back(std::make_unique<UIActionLogic>(m_logic_context));
    break;
  }
  case SceneType::SceneType_CRAFTING: {
    action_logics.push_back(std::make_unique<UIActionLogic>(m_logic_context));
    break;
  }
  case SceneType::SceneType_TEST: {
    // add action logics for test purposes
    action_logics.push_back(std::make_unique<UIActionLogic>(m_logic_context));
    break;
  }
  default:
    return std::unexpected(FailInfo{FailMode::NonExistentEnumValue,
                                    "Unsupported scene type for action logic"});
  }
  return action_logics;
}

} // namespace steamrot
