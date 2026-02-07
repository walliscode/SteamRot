/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the LogicFactory class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "LogicFactory.h"
#include "CraftingRenderLogic.h"
#include "FailInfo.h"
#include "UIActionLogic.h"
#include "UICollisionLogic.h"
#include "UIRenderLogic.h"
#include "UIStateLogic.h"
#include <array>
#include <expected>
#include <memory>

namespace steamrot {

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
    auto logic_result = CreateLogicObject(logic_type);
    if (!logic_result) {
      return std::unexpected(logic_result.error());
    }
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
  case LogicType::CraftingRender:
    logic_ptr = std::make_unique<CraftingRenderLogic>(m_scene_context);
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

  //
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
LogicFactory::ConfigureTitleLogics(LogicCollection &logic_collection) {

  ////// THE ORDER OF THE LOGIC CLASSES IS VERY IMPORTANT //////
  ////// DO NOT CHANGE UNLESS YOU KNOW WHAT YOU ARE DOING //////

  // Define the Logic types for each grouping in the order they should execute
  // These are compile-time constants that define the scene's Logic configuration
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

  auto action_result = AddLogicsToCollection(
      logic_collection, LogicGrouping::Action,
      std::vector<LogicType>(action_logic_types.begin(),
                             action_logic_types.end()));
  if (!action_result) {
    return std::unexpected(action_result.error());
  }

  auto render_result = AddLogicsToCollection(
      logic_collection, LogicGrouping::Render,
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
  // These are compile-time constants that define the scene's Logic configuration
  static constexpr std::array collision_logic_types = {LogicType::UICollision};
  static constexpr std::array action_logic_types = {LogicType::UIAction,
                                                     LogicType::UIState};
  static constexpr std::array render_logic_types = {LogicType::UIRender,
                                                     LogicType::CraftingRender};

  // Add Logics to collection using the helper function
  auto collision_result = AddLogicsToCollection(
      logic_collection, LogicGrouping::Collision,
      std::vector<LogicType>(collision_logic_types.begin(),
                             collision_logic_types.end()));
  if (!collision_result) {
    return std::unexpected(collision_result.error());
  }

  auto action_result = AddLogicsToCollection(
      logic_collection, LogicGrouping::Action,
      std::vector<LogicType>(action_logic_types.begin(),
                             action_logic_types.end()));
  if (!action_result) {
    return std::unexpected(action_result.error());
  }

  auto render_result = AddLogicsToCollection(
      logic_collection, LogicGrouping::Render,
      std::vector<LogicType>(render_logic_types.begin(),
                             render_logic_types.end()));
  if (!render_result) {
    return std::unexpected(render_result.error());
  }

  return std::monostate();
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
LogicFactory::ConfigureTestLogics(LogicCollection &logic_collection) {
  ////// THE ORDER OF THE LOGIC CLASSES IS VERY IMPORTANT //////
  ////// DO NOT CHANGE UNLESS YOU KNOW WHAT YOU ARE DOING //////

  // Define the Logic types for each grouping in the order they should execute
  // These are compile-time constants that define the scene's Logic configuration
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

  auto action_result = AddLogicsToCollection(
      logic_collection, LogicGrouping::Action,
      std::vector<LogicType>(action_logic_types.begin(),
                             action_logic_types.end()));
  if (!action_result) {
    return std::unexpected(action_result.error());
  }

  auto render_result = AddLogicsToCollection(
      logic_collection, LogicGrouping::Render,
      std::vector<LogicType>(render_logic_types.begin(),
                             render_logic_types.end()));
  if (!render_result) {
    return std::unexpected(render_result.error());
  }

  return std::monostate();
}
} // namespace steamrot
