/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the SimulationRunner class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SimulationRunner.h"
#include "UIActionLogic.h"
#include "UICollisionLogic.h"
#include "UIRenderLogic.h"
#include "UIStateLogic.h"
#include <variant>

namespace steamrot::tests {

/////////////////////////////////////////////////
SimulationRunner::SimulationRunner(const SimulationData &simulation_data,
                                   SceneContext &scene_context)
    : m_simulation_data(simulation_data), m_scene_context(scene_context) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> SimulationRunner::ExecuteSimulation() {
  // Iterate through all steps
  for (const auto &step : m_simulation_data.steps) {
    auto result = ExecuteStep(step);
    if (!result.has_value()) {
      return std::unexpected(result.error());
    }
  }
  return std::monostate{};
}
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteLogicClass(LogicClassEnum logic_class, SceneContext &context) {

  switch (logic_class) {
  case LogicClassEnum::UIActionLogic: {
    UIActionLogic logic(context);
    logic.RunLogic();
    return std::monostate{};
  }
  case LogicClassEnum::UICollisionLogic: {
    UICollisionLogic logic(context);
    logic.RunLogic();
    return std::monostate{};
  }
  case LogicClassEnum::UIRenderLogic: {
    UIRenderLogic logic(context);
    logic.RunLogic();
    return std::monostate{};
  }
  case LogicClassEnum::UIStateLogic: {
    UIStateLogic logic(context);
    logic.RunLogic();
    return std::monostate{};
  }
  case LogicClassEnum::None:
  default:
    return std::unexpected(
        FailInfo{FailMode::NonExistentEnumValue,
                 "Invalid LogicClassEnum value in simulation step"});
  }
}
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SimulationRunner::ExecuteStep(const SimulationStep &step) {
  return ExecuteLogicClass(step.element, m_scene_context);
}

} // namespace steamrot::tests
