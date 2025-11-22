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
#include <expected>

namespace steamrot {
/////////////////////////////////////////////////
LogicFactory::LogicFactory(const SceneType scene_type,
                           const SceneContext &scene_context)
    : m_scene_type(scene_type), m_scene_context(scene_context),
      m_logic_collection_data(nullptr) {}

/////////////////////////////////////////////////
std::expected<LogicCollection, FailInfo>
LogicFactory::CreateLogicMap(const LogicCollectionData &logic_collection_data) {

  // Store the logic collection data
  m_logic_collection_data = &logic_collection_data;

  // create return object
  LogicCollection logic_collection;

  // create collision logics
  auto collision_logics =
      CreateCollisionLogics(logic_collection_data.collision_logic_data());
  if (!collision_logics.has_value()) {
    return std::unexpected(collision_logics.error());
  }
  logic_collection[LogicType::Collision] = std::move(collision_logics.value());

  // create render logics
  auto render_logics =
      CreateRenderLogics(logic_collection_data.render_logic_data());
  if (!render_logics.has_value()) {
    return std::unexpected(render_logics.error());
  }
  logic_collection[LogicType::Render] = std::move(render_logics.value());

  // create action logics
  auto action_logics =
      CreateActionLogics(logic_collection_data.action_logic_data());
  if (!action_logics.has_value()) {
    return std::unexpected(action_logics.error());
  }
  logic_collection[LogicType::Action] = std::move(action_logics.value());

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
  if (!logic_data->subscriber_data()) {
    // No subscribers to attach, return success
    return std::monostate{};
  }

  // Create a SubscriberFactory to create and register subscribers
  SubscriberFactory subscriber_factory(m_scene_context.event_handler);

  // Iterate through subscriber data and create subscribers
  for (const auto *subscriber_data : *logic_data->subscriber_data()) {
    if (!subscriber_data) {
      continue; // Skip null subscriber data
    }

    // Create and register subscriber
    auto subscriber_result =
        subscriber_factory.CreateAndRegisterSubscriber(*subscriber_data);
    if (!subscriber_result.has_value()) {
      return std::unexpected(subscriber_result.error());
    }

    // Add subscriber to the logic instance
    logic.AddSubscriber(subscriber_result.value());
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<LogicVector, FailInfo> LogicFactory::CreateRenderLogics(
    const flatbuffers::Vector<flatbuffers::Offset<LogicData>>
        *render_logic_data) {

  LogicVector render_logics;

  switch (m_scene_type) {
  case SceneType::SceneType_TITLE: {
    auto logic = std::make_unique<UIRenderLogic>(m_scene_context);

    // Find and attach subscribers from logic data
    if (render_logic_data) {
      for (const auto *logic_data : *render_logic_data) {
        if (logic_data && logic_data->name() &&
            logic_data->name()->str() == "UIRenderLogic") {
          auto attach_result = AttachSubscribers(*logic, logic_data);
          if (!attach_result.has_value()) {
            return std::unexpected(attach_result.error());
          }
          break;
        }
      }
    }

    render_logics.push_back(std::move(logic));
    break;
  }
  case SceneType::SceneType_CRAFTING: {
    // CraftingRenderLogic
    auto crafting_logic =
        std::make_unique<CraftingRenderLogic>(m_scene_context);
    if (render_logic_data) {
      for (const auto *logic_data : *render_logic_data) {
        if (logic_data && logic_data->name() &&
            logic_data->name()->str() == "CraftingRenderLogic") {
          auto attach_result = AttachSubscribers(*crafting_logic, logic_data);
          if (!attach_result.has_value()) {
            return std::unexpected(attach_result.error());
          }
          break;
        }
      }
    }
    render_logics.push_back(std::move(crafting_logic));

    // UIRenderLogic
    auto ui_logic = std::make_unique<UIRenderLogic>(m_scene_context);
    if (render_logic_data) {
      for (const auto *logic_data : *render_logic_data) {
        if (logic_data && logic_data->name() &&
            logic_data->name()->str() == "UIRenderLogic") {
          auto attach_result = AttachSubscribers(*ui_logic, logic_data);
          if (!attach_result.has_value()) {
            return std::unexpected(attach_result.error());
          }
          break;
        }
      }
    }
    render_logics.push_back(std::move(ui_logic));
    break;
  }
  case SceneType::SceneType_TEST: {
    // add render logics for test purposes
    auto logic = std::make_unique<UIRenderLogic>(m_scene_context);
    if (render_logic_data) {
      for (const auto *logic_data : *render_logic_data) {
        if (logic_data && logic_data->name() &&
            logic_data->name()->str() == "UIRenderLogic") {
          auto attach_result = AttachSubscribers(*logic, logic_data);
          if (!attach_result.has_value()) {
            return std::unexpected(attach_result.error());
          }
          break;
        }
      }
    }
    render_logics.push_back(std::move(logic));
    break;
  }
  default:
    return std::unexpected(FailInfo{FailMode::NonExistentEnumValue,
                                    "Unsupported scene type for render logic"});
  }
  return render_logics;
}
/////////////////////////////////////////////////
std::expected<LogicVector, FailInfo> LogicFactory::CreateCollisionLogics(
    const flatbuffers::Vector<flatbuffers::Offset<LogicData>>
        *collision_logic_data) {

  LogicVector collision_logics;

  switch (m_scene_type) {
  case SceneType::SceneType_TITLE: {
    auto logic = std::make_unique<UICollisionLogic>(m_scene_context);

    // Find and attach subscribers from logic data
    if (collision_logic_data) {
      for (const auto *logic_data : *collision_logic_data) {
        if (logic_data && logic_data->name() &&
            logic_data->name()->str() == "UICollisionLogic") {
          auto attach_result = AttachSubscribers(*logic, logic_data);
          if (!attach_result.has_value()) {
            return std::unexpected(attach_result.error());
          }
          break;
        }
      }
    }

    collision_logics.push_back(std::move(logic));
    break;
  }
  case SceneType::SceneType_CRAFTING: {
    auto logic = std::make_unique<UICollisionLogic>(m_scene_context);

    if (collision_logic_data) {
      for (const auto *logic_data : *collision_logic_data) {
        if (logic_data && logic_data->name() &&
            logic_data->name()->str() == "UICollisionLogic") {
          auto attach_result = AttachSubscribers(*logic, logic_data);
          if (!attach_result.has_value()) {
            return std::unexpected(attach_result.error());
          }
          break;
        }
      }
    }

    collision_logics.push_back(std::move(logic));
    break;
  }
  case SceneType::SceneType_TEST: {
    // add collision logics for test purposes
    auto logic = std::make_unique<UICollisionLogic>(m_scene_context);

    if (collision_logic_data) {
      for (const auto *logic_data : *collision_logic_data) {
        if (logic_data && logic_data->name() &&
            logic_data->name()->str() == "UICollisionLogic") {
          auto attach_result = AttachSubscribers(*logic, logic_data);
          if (!attach_result.has_value()) {
            return std::unexpected(attach_result.error());
          }
          break;
        }
      }
    }

    collision_logics.push_back(std::move(logic));

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
std::expected<LogicVector, FailInfo> LogicFactory::CreateActionLogics(
    const flatbuffers::Vector<flatbuffers::Offset<LogicData>>
        *action_logic_data) {

  LogicVector action_logics;

  switch (m_scene_type) {
  case SceneType::SceneType_TITLE: {
    // UIStateLogic
    auto state_logic = std::make_unique<UIStateLogic>(m_scene_context);
    if (action_logic_data) {
      for (const auto *logic_data : *action_logic_data) {
        if (logic_data && logic_data->name() &&
            logic_data->name()->str() == "UIStateLogic") {
          auto attach_result = AttachSubscribers(*state_logic, logic_data);
          if (!attach_result.has_value()) {
            return std::unexpected(attach_result.error());
          }
          break;
        }
      }
    }
    action_logics.push_back(std::move(state_logic));

    // UIActionLogic
    auto action_logic = std::make_unique<UIActionLogic>(m_scene_context);
    if (action_logic_data) {
      for (const auto *logic_data : *action_logic_data) {
        if (logic_data && logic_data->name() &&
            logic_data->name()->str() == "UIActionLogic") {
          auto attach_result = AttachSubscribers(*action_logic, logic_data);
          if (!attach_result.has_value()) {
            return std::unexpected(attach_result.error());
          }
          break;
        }
      }
    }
    action_logics.push_back(std::move(action_logic));
    break;
  }
  case SceneType::SceneType_CRAFTING: {
    // UIStateLogic
    auto state_logic = std::make_unique<UIStateLogic>(m_scene_context);
    if (action_logic_data) {
      for (const auto *logic_data : *action_logic_data) {
        if (logic_data && logic_data->name() &&
            logic_data->name()->str() == "UIStateLogic") {
          auto attach_result = AttachSubscribers(*state_logic, logic_data);
          if (!attach_result.has_value()) {
            return std::unexpected(attach_result.error());
          }
          break;
        }
      }
    }
    action_logics.push_back(std::move(state_logic));

    // UIActionLogic
    auto action_logic = std::make_unique<UIActionLogic>(m_scene_context);
    if (action_logic_data) {
      for (const auto *logic_data : *action_logic_data) {
        if (logic_data && logic_data->name() &&
            logic_data->name()->str() == "UIActionLogic") {
          auto attach_result = AttachSubscribers(*action_logic, logic_data);
          if (!attach_result.has_value()) {
            return std::unexpected(attach_result.error());
          }
          break;
        }
      }
    }
    action_logics.push_back(std::move(action_logic));
    break;
  }
  case SceneType::SceneType_TEST: {
    // add action logics for test purposes
    // UIStateLogic
    auto state_logic = std::make_unique<UIStateLogic>(m_scene_context);
    if (action_logic_data) {
      for (const auto *logic_data : *action_logic_data) {
        if (logic_data && logic_data->name() &&
            logic_data->name()->str() == "UIStateLogic") {
          auto attach_result = AttachSubscribers(*state_logic, logic_data);
          if (!attach_result.has_value()) {
            return std::unexpected(attach_result.error());
          }
          break;
        }
      }
    }
    action_logics.push_back(std::move(state_logic));

    // UIActionLogic
    auto action_logic = std::make_unique<UIActionLogic>(m_scene_context);
    if (action_logic_data) {
      for (const auto *logic_data : *action_logic_data) {
        if (logic_data && logic_data->name() &&
            logic_data->name()->str() == "UIActionLogic") {
          auto attach_result = AttachSubscribers(*action_logic, logic_data);
          if (!attach_result.has_value()) {
            return std::unexpected(attach_result.error());
          }
          break;
        }
      }
    }
    action_logics.push_back(std::move(action_logic));
    break;
  }
  default:
    return std::unexpected(FailInfo{FailMode::NonExistentEnumValue,
                                    "Unsupported scene type for action logic"});
  }
  return action_logics;
}

} // namespace steamrot
