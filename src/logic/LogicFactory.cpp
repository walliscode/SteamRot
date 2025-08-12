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
#include "log_handler.h"
#include "logics_generated.h"
#include "spdlog/common.h"
#include <iostream>
#include <unordered_map>

namespace steamrot {

/////////////////////////////////////////////////
std::unordered_map<LogicType, std::vector<std::unique_ptr<Logic>>>
LogicFactory::CreateLogicMap(const LogicCollection &logic_collection,
                             const LogicContext logic_context) {

  // create return object
  std::unordered_map<LogicType, std::vector<std::unique_ptr<Logic>>> logic_map;

  // guard against empty logic collection
  if (logic_collection.types()->empty()) {
    log_handler::ProcessLog(spdlog::level::err, log_handler::LogCode::kNoCode,
                            "Logic collection is empty, no logics to create.");
  }
  // Iterate through the provided logic types and create the corresponding
  // logics
  std::cout << "Creating logic map with " << logic_collection.types()->size()
            << " logic types." << std::endl;
  for (const auto &logic_type : *logic_collection.types()) {
    switch (logic_type) {

    // Render Logic
    case LogicType::LogicType_Render: {
      std::cout << "Creating Render logics." << std::endl;
      logic_map[LogicType::LogicType_Render] =
          CreateRenderLogics(logic_context);
      std::cout << "Render logics created." << std::endl;
      break;
    }

    // Collision Logic
    case LogicType::LogicType_Collision: {
      std::cout << "Creating Collision logics." << std::endl;
      logic_map[LogicType::LogicType_Collision] =
          CreateCollisionLogics(logic_context);
      std::cout << "Collision logics created." << std::endl;
      break;
    }

      // Event Logic
    case LogicType::LogicType_Action: {
      std::cout << "Creating Event logics." << std::endl;
      logic_map[LogicType::LogicType_Action] =
          CreateActionLogics(logic_context);
      std::cout << "Event logics created." << std::endl;
      break;
    }

    // Add cases for other logic types as needed
    default:
      // Handle unknown logic type if necessary
      break;
    }
  }
  std::cout << "Logic map created with " << logic_map.size() << " logic types."
            << std::endl;
  return logic_map;
}

/////////////////////////////////////////////////
std::vector<std::unique_ptr<Logic>>
LogicFactory::CreateRenderLogics(const LogicContext logic_context) {
  // Create a vector of unique pointers to Logic for rendering
  std::vector<std::unique_ptr<Logic>> render_logics;

  // compile time defined order of logic types
  render_logics.push_back(std::make_unique<CraftingRenderLogic>(logic_context));
  render_logics.push_back(std::make_unique<UIRenderLogic>(logic_context));

  return render_logics;
}

/////////////////////////////////////////////////
std::vector<std::unique_ptr<Logic>>
LogicFactory::CreateCollisionLogics(const LogicContext logic_context) {

  // Create a vector of unique pointers to Logic for collision
  std::vector<std::unique_ptr<Logic>> collision_logics;
  std::cout << "Creating a vector of collision logics." << std::endl;

  // compile time defined order of logic types
  collision_logics.push_back(std::make_unique<UICollisionLogic>(logic_context));
  std::cout << "Collision logics created with size: " << collision_logics.size()
            << std::endl;

  return collision_logics;
}

/////////////////////////////////////////////////
std::vector<std::unique_ptr<Logic>>
LogicFactory::CreateActionLogics(const LogicContext logic_context) {

  // Create a vector of unique pointers to Logic for events
  std::vector<std::unique_ptr<Logic>> event_logics;
  std::cout << "Creating a vector of event logics." << std::endl;

  // compile time defined order of logic types
  event_logics.push_back(std::make_unique<UIActionLogic>(logic_context));
  std::cout << "Event logics created with size: " << event_logics.size()
            << std::endl;
  return event_logics;
}
} // namespace steamrot
