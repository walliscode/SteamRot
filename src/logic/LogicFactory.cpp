
#include "LogicFactory.h"
#include "UIRenderLogic.h"
#include "logics_generated.h"
#include <unordered_map>

namespace steamrot {

std::unordered_map<LogicType, std::vector<std::unique_ptr<BaseLogic>>>
LogicFactory::CreateLogicMap(const LogicCollection &logic_collection,
                             const LogicContext &logic_context) {

  // create return object
  std::unordered_map<LogicType, std::vector<std::unique_ptr<BaseLogic>>>
      logic_map;

  // Iterate through the provided logic types and create the corresponding
  // logics
  for (const auto &logic_type : *logic_collection.types()) {
    switch (logic_type) {
    case LogicType::LogicType_Render: {
      logic_map[LogicType::LogicType_Render] =
          CreateRenderLogics(logic_context);
      break;
    }
    // Add cases for other logic types as needed
    default:
      // Handle unknown logic type if necessary
      break;
    }
  }
  // Add other logic types as needed
  return logic_map;
}

std::vector<std::unique_ptr<BaseLogic>>
LogicFactory::CreateRenderLogics(const LogicContext &logic_context) {
  // Create a vector of unique pointers to BaseLogic for rendering
  std::vector<std::unique_ptr<BaseLogic>> render_logics;

  // compile time defined order of logic types
  render_logics.push_back(std::make_unique<UIRenderLogic>(logic_context));

  return render_logics;
}
} // namespace steamrot
