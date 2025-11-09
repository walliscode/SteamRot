/////////////////////////////////////////////////
/// @file
/// @brief Implementation of simulation runner for data-driven logic testing
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "simulation_runner.h"
#include "CUserInterface.h"
#include "CraftingRenderLogic.h"
#include "UIActionLogic.h"
#include "UICollisionLogic.h"
#include "UIRenderLogic.h"
#include "UIStateLogic.h"
#include "archetype_helpers.h"
#include "entity_memory.h"
#include <format>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Execute a free function based on FunctionType enum
/////////////////////////////////////////////////
static std::expected<std::monostate, FailInfo>
execute_function(const FunctionType function_type,
                 SceneContext &scene_context) {

  switch (function_type) {
  case FunctionType_ProcessUIActionsAndEvents: {
    // This function needs a UIElement - we need to iterate through
    // UI entities in the scene
    ArchetypeID ui_archetype = GenerateArchetypeIDfromTypes<CUserInterface>();
    const auto it = scene_context.archetypes.find(ui_archetype);

    if (it != scene_context.archetypes.end()) {
      const Archetype &archetype = it->second;
      for (size_t entity_id : archetype) {
        CUserInterface &ui_component =
            entity::memory::GetComponent<CUserInterface>(
                entity_id, scene_context.scene_entities);

        if (ui_component.m_root_element) {
          ProcessUIActionsAndEvents(*ui_component.m_root_element,
                                    scene_context.event_handler, scene_context);
        }
      }
    }
    return std::monostate{};
  }

  case FunctionType_ProcessNestedUIActionsAndEvents: {
    // Similar to above but processes nested elements
    ArchetypeID ui_archetype = GenerateArchetypeIDfromTypes<CUserInterface>();
    const auto it = scene_context.archetypes.find(ui_archetype);

    if (it != scene_context.archetypes.end()) {
      const Archetype &archetype = it->second;
      for (size_t entity_id : archetype) {
        CUserInterface &ui_component =
            entity::memory::GetComponent<CUserInterface>(
                entity_id, scene_context.scene_entities);

        if (ui_component.m_root_element) {
          ProcessNestedUIActionsAndEvents(*ui_component.m_root_element,
                                          scene_context.event_handler,
                                          scene_context);
        }
      }
    }
    return std::monostate{};
  }

  case FunctionType_ProcessButtonElementActions: {
    // This function operates on ButtonElement instances
    // For now, we'll skip this as it requires specific element type
    // It's typically called from ProcessUIActionsAndEvents
    return std::monostate{};
  }

  case FunctionType_ProcessDropDownListElementActions: {
    // This function operates on DropDownListElement instances
    // For now, we'll skip this as it requires specific element type
    // It's typically called from ProcessUIActionsAndEvents
    return std::monostate{};
  }

  case FunctionType_None:
  default:
    std::string error_msg =
        std::format("Unknown or unsupported FunctionType: {}",
                    static_cast<int>(function_type));
    return std::unexpected(FailInfo(FailMode::NonExistentEnumValue, error_msg));
  }
}

/////////////////////////////////////////////////
/// @brief Execute a Logic class based on LogicClassType enum
/////////////////////////////////////////////////
static std::expected<std::monostate, FailInfo>
execute_logic_class(const LogicClassType logic_class_type,
                    SceneContext &scene_context) {

  switch (logic_class_type) {
  case LogicClassType_UIActionLogic: {
    UIActionLogic logic(scene_context);
    logic.RunLogic();
    return std::monostate{};
  }

  case LogicClassType_UICollisionLogic: {
    UICollisionLogic logic(scene_context);
    logic.RunLogic();
    return std::monostate{};
  }

  case LogicClassType_UIRenderLogic: {
    UIRenderLogic logic(scene_context);
    logic.RunLogic();
    return std::monostate{};
  }

  case LogicClassType_UIStateLogic: {
    UIStateLogic logic(scene_context);
    logic.RunLogic();
    return std::monostate{};
  }

  case LogicClassType_CraftingRenderLogic: {
    CraftingRenderLogic logic(scene_context);
    logic.RunLogic();
    return std::monostate{};
  }

  case LogicClassType_None:
  default:
    std::string error_msg =
        std::format("Unknown or unsupported LogicClassType: {}",
                    static_cast<int>(logic_class_type));
    return std::unexpected(FailInfo(FailMode::NonExistentEnumValue, error_msg));
  }
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_simulation_step(const SimulationStep *step,
                        SceneContext &scene_context) {

  // Validate step
  if (!step) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "SimulationStep is null"));
  }

  // Dispatch based on execution mode
  switch (step->execution_mode()) {
  case ExecutionMode_Function:
    return execute_function(step->function_type(), scene_context);

  case ExecutionMode_LogicClass:
    return execute_logic_class(step->logic_class_type(), scene_context);

  default:
    std::string error_msg = std::format(
        "Unknown ExecutionMode: {}", static_cast<int>(step->execution_mode()));
    return std::unexpected(FailInfo(FailMode::NonExistentEnumValue, error_msg));
  }
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_simulation(const SimulationData *simulation_data,
                   SceneContext &scene_context) {

  // Validate simulation data
  if (!simulation_data) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "SimulationData is null"));
  }

  // Check if steps are provided
  if (!simulation_data->steps()) {
    // No steps is valid - just a no-op simulation
    return std::monostate{};
  }

  // Execute each step in order
  for (const auto *step : *simulation_data->steps()) {
    auto result = execute_simulation_step(step, scene_context);
    if (!result.has_value()) {
      return result; // Propagate error
    }
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_simulation_with_fixture(const SimulationData *simulation_data,
                                TestFixture &fixture) {

  // Get the SceneContext from the fixture
  SceneContext &scene_context = fixture.GetSceneContext();

  // Execute the simulation
  return execute_simulation(simulation_data, scene_context);
}

} // namespace steamrot::tests
