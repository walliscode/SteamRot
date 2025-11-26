/////////////////////////////////////////////////
/// @file
/// @brief Implementation of logic execution free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "logic_execution.h"

namespace steamrot::logic::execution {

/////////////////////////////////////////////////
void ExecuteLogicVector(const LogicVector &logics) {
  for (const auto &logic : logics) {
    if (logic) {
      logic->RunLogic();
    }
  }
}

/////////////////////////////////////////////////
void ExecuteLogicsByType(const LogicCollection &logic_map, LogicType type) {
  auto it = logic_map.find(type);
  if (it != logic_map.end()) {
    ExecuteLogicVector(it->second);
  }
}

/////////////////////////////////////////////////
void ExecuteSceneTick(const LogicCollection &logic_map) {
  // Execute in standard scene update order:
  // Action -> Movement -> Collision -> Render
  ExecuteActionLogics(logic_map);
  ExecuteMovementLogics(logic_map);
  ExecuteCollisionLogics(logic_map);
  ExecuteRenderLogics(logic_map);
}

/////////////////////////////////////////////////
void ExecuteActionLogics(const LogicCollection &logic_map) {
  ExecuteLogicsByType(logic_map, LogicType::Action);
}

/////////////////////////////////////////////////
void ExecuteMovementLogics(const LogicCollection &logic_map) {
  ExecuteLogicsByType(logic_map, LogicType::Movement);
}

/////////////////////////////////////////////////
void ExecuteCollisionLogics(const LogicCollection &logic_map) {
  ExecuteLogicsByType(logic_map, LogicType::Collision);
}

/////////////////////////////////////////////////
void ExecuteRenderLogics(const LogicCollection &logic_map) {
  ExecuteLogicsByType(logic_map, LogicType::Render);
}

} // namespace steamrot::logic::execution
