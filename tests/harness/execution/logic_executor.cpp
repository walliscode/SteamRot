/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Level 1 Logic executor
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "logic_executor.h"
#include "CUIState.h"
#include "CUserInterface.h"
#include "CraftingRenderLogic.h"
#include "TestFixture.h"
#include "UIActionLogic.h"
#include "UICollisionLogic.h"
#include "UIRenderLogic.h"
#include "UIStateLogic.h"
#include "archetype_helpers.h"
#include "entity_memory.h"
#include "logic_action.h"
#include "logic_collision.h"
#include "logic_ui.h"
#include <format>
#include <iostream>

namespace steamrot::tests::execution {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ExecuteLogic(Logic &logic) {
  // Execute the logic - currently RunLogic() doesn't return errors
  // but wrapping it allows for future error handling
  logic.RunLogic();
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteLogicByType(LogicClassType type, SceneContext &scene_context) {

  switch (type) {
  case LogicClassType_UIActionLogic: {
    UIActionLogic logic(scene_context);
    return ExecuteLogic(logic);
  }

  case LogicClassType_UICollisionLogic: {
    UICollisionLogic logic(scene_context);
    return ExecuteLogic(logic);
  }

  case LogicClassType_UIRenderLogic: {
    UIRenderLogic logic(scene_context);
    return ExecuteLogic(logic);
  }

  case LogicClassType_UIStateLogic: {
    UIStateLogic logic(scene_context);
    return ExecuteLogic(logic);
  }

  case LogicClassType_CraftingRenderLogic: {
    CraftingRenderLogic logic(scene_context);
    return ExecuteLogic(logic);
  }

  case LogicClassType_None:
  default:
    std::string error_msg = std::format(
        "Unknown or unsupported LogicClassType: {}", static_cast<int>(type));
    return std::unexpected(FailInfo(FailMode::NonExistentEnumValue, error_msg));
  }
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteFunction(FunctionType type, SceneContext &scene_context) {

  switch (type) {
  case FunctionType_ProcessUIActionsAndEvents: {
    // Iterate through UI entities and process actions
    ArchetypeID ui_archetype = GenerateArchetypeIDfromTypes<CUserInterface>();
    const auto it = scene_context.archetypes.find(ui_archetype);

    if (it != scene_context.archetypes.end()) {
      const Archetype &archetype = it->second;
      for (size_t entity_id : archetype) {
        CUserInterface &ui_component =
            entity::memory::GetComponent<CUserInterface>(
                entity_id, scene_context.scene_entities);

        if (ui_component.m_root_element) {
          steamrot::logic::action::ProcessUIActionsAndEvents(
              *ui_component.m_root_element, scene_context.event_handler,
              scene_context);
        }
      }
    } else {
      std::cout << "No CUserInterface archetype found in scene." << std::endl;
    }
    return std::monostate{};
  }

  case FunctionType_ProcessNestedUIActionsAndEvents: {
    ArchetypeID ui_archetype = GenerateArchetypeIDfromTypes<CUserInterface>();
    const auto it = scene_context.archetypes.find(ui_archetype);

    if (it != scene_context.archetypes.end()) {
      const Archetype &archetype = it->second;
      for (size_t entity_id : archetype) {
        CUserInterface &ui_component =
            entity::memory::GetComponent<CUserInterface>(
                entity_id, scene_context.scene_entities);

        if (ui_component.m_root_element) {
          steamrot::logic::action::ProcessNestedUIActionsAndEvents(
              *ui_component.m_root_element, scene_context.event_handler,
              scene_context);
        }
      }
    }
    return std::monostate{};
  }

  case FunctionType_ProcessButtonElementActions: {
    // This function operates on ButtonElement instances
    // Typically called from ProcessUIActionsAndEvents
    return std::monostate{};
  }

  case FunctionType_ProcessDropDownListElementActions: {
    // This function operates on DropDownListElement instances
    // Typically called from ProcessUIActionsAndEvents
    return std::monostate{};
  }

  case FunctionType_CheckMouseOverNestedUIElement: {
    auto entity_indexes = GenerateEntityIndexesFromComponents<CUserInterface>(
        scene_context.archetypes);

    for (size_t entity_id : entity_indexes) {
      CUserInterface &ui_component =
          entity::memory::GetComponent<CUserInterface>(
              entity_id, scene_context.scene_entities);
      if (ui_component.m_root_element) {
        steamrot::logic::collision::CheckMouseOverNestedUIElement(
            scene_context.mouse_position, *ui_component.m_root_element);
      }
    }
    return std::monostate{};
  }

  case FunctionType_UpdateCUserInterfaceVisibilityFromCUIState: {
    auto entity_indexes =
        GenerateEntityIndexesFromComponents<CUIState>(scene_context.archetypes);

    for (size_t entity_id : entity_indexes) {
      CUIState &ui_state = entity::memory::GetComponent<CUIState>(
          entity_id, scene_context.scene_entities);

      steamrot::logic::ui::UpdateCUserInterfaceVisibilityFromCUIState(
          ui_state, scene_context.scene_entities);
    }
    return std::monostate{};
  }

  case FunctionType_None:
  default:
    std::string error_msg = std::format(
        "Unknown or unsupported FunctionType: {}", EnumNameFunctionType(type));
    return std::unexpected(FailInfo(FailMode::NonExistentEnumValue, error_msg));
  }
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteSimulationStep(const SimulationStep *step, SceneContext &scene_context) {

  if (!step) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "SimulationStep is null"));
  }

  switch (step->execution_mode()) {
  case ExecutionMode_Function:
    return ExecuteFunction(step->function_type(), scene_context);

  case ExecutionMode_LogicClass:
    return ExecuteLogicByType(step->logic_class_type(), scene_context);

  default:
    std::string error_msg = std::format(
        "Unknown ExecutionMode: {}", static_cast<int>(step->execution_mode()));
    return std::unexpected(FailInfo(FailMode::NonExistentEnumValue, error_msg));
  }
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteWorkflow(const SimulationData *simulation_data,
                SceneContext &scene_context) {

  if (!simulation_data) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "SimulationData is null"));
  }

  if (!simulation_data->steps()) {
    // No steps is valid - just a no-op
    return std::monostate{};
  }

  for (const auto *step : *simulation_data->steps()) {
    auto result = ExecuteSimulationStep(step, scene_context);
    if (!result.has_value()) {
      return result; // Propagate error
    }
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteWorkflowWithFixture(const SimulationData *simulation_data,
                           TestFixture &fixture) {
  SceneContext &scene_context = fixture.GetSceneContext();
  return ExecuteWorkflow(simulation_data, scene_context);
}

} // namespace steamrot::tests::execution
