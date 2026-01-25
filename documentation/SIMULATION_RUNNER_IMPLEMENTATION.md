# Simulation Runner Implementation Guide

## Purpose

This document provides step-by-step implementation guidance for developers implementing the Simulation Runner design for the TestEngine.

**Prerequisites**: Read SIMULATION_RUNNER_DESIGN.md first for complete design context.

## Implementation Phases

### Phase 1: Core SimulationRunner Class

#### Step 1.1: Create Header File

**File**: `tests/harness/SimulationRunner.h`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the SimulationRunner class.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "SceneContext.h"
#include "SimulationData.h"
#include <expected>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class SimulationRunner
/// @brief Executes simulation steps for TestEngine.
///
/// The SimulationRunner orchestrates execution of free functions
/// and Logic class instances as specified in SimulationData.
/// Each step is executed in order, with proper error handling.
/////////////////////////////////////////////////
class SimulationRunner {
private:
  /////////////////////////////////////////////////
  /// @brief Reference to simulation configuration
  /////////////////////////////////////////////////
  const SimulationData &m_simulation_data;

  /////////////////////////////////////////////////
  /// @brief Reference to scene context for execution
  /////////////////////////////////////////////////
  SceneContext &m_scene_context;

  /////////////////////////////////////////////////
  /// @brief Execute a single simulation step
  ///
  /// @param step The simulation step to execute
  /// @return Success or failure information
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ExecuteStep(const SimulationStep &step);

public:
  /////////////////////////////////////////////////
  /// @brief Constructor
  ///
  /// @param simulation_data Reference to simulation configuration
  /// @param scene_context Reference to scene context
  /////////////////////////////////////////////////
  SimulationRunner(const SimulationData &simulation_data,
                   SceneContext &scene_context);

  /////////////////////////////////////////////////
  /// @brief Execute all simulation steps
  ///
  /// @return Success or failure information
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> ExecuteSimulation();
};

} // namespace steamrot::tests
```

#### Step 1.2: Create Implementation Skeleton

**File**: `tests/harness/SimulationRunner.cpp`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the SimulationRunner class.
/////////////////////////////////////////////////

#include "SimulationRunner.h"
#include <variant>

namespace steamrot::tests {

/////////////////////////////////////////////////
SimulationRunner::SimulationRunner(const SimulationData &simulation_data,
                                   SceneContext &scene_context)
    : m_simulation_data(simulation_data), m_scene_context(scene_context) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SimulationRunner::ExecuteSimulation() {
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
SimulationRunner::ExecuteStep(const SimulationStep &step) {
  // TODO: Implement step execution with std::visit
  return std::monostate{};
}

} // namespace steamrot::tests
```

### Phase 2: Logic Class Execution

#### Step 2.1: Implement ExecuteLogicClass Function

Add to `SimulationRunner.cpp`:

```cpp
#include "UIActionLogic.h"
#include "UICollisionLogic.h"
#include "UIRenderLogic.h"
#include "UIStateLogic.h"
#include "CraftingRenderLogic.h"

namespace steamrot::tests {

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
    case LogicClassEnum::CraftingRenderLogic: {
      CraftingRenderLogic logic(context);
      logic.RunLogic();
      return std::monostate{};
    }
    case LogicClassEnum::None:
    default:
      return std::unexpected(FailInfo{
        FailMode::InvalidEnumValue,
        "Invalid LogicClassEnum value in simulation step"
      });
  }
}

} // namespace steamrot::tests
```

### Phase 3: Free Function Execution

#### Step 3.1: Identify Function Signatures

**Action Required**: Verify the actual function signatures in the codebase.

Check these files:
- `src/logic/logic_action.h`
- `src/logic/logic_collision.h`
- `src/logic/logic_ui.h`

Expected pattern:
```cpp
void FunctionName(EntityMemoryPool &entities,
                  const std::unordered_map<ArchetypeID, Archetype> &archetypes,
                  /* additional parameters */);
```

#### Step 3.2: Implement ExecuteFunction

Add to `SimulationRunner.cpp` (adjust parameters based on actual signatures):

```cpp
#include "logic_action.h"
#include "logic_collision.h"
#include "logic_ui.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteFunction(FunctionEnum function, SceneContext &context) {
  // NOTE: Adjust function calls based on actual signatures
  switch (function) {
    case FunctionEnum::ProcessUIActionsAndEvents:
      ProcessUIActionsAndEvents(context.scene_entities, 
                                context.archetypes,
                                context.event_handler);
      return std::monostate{};
      
    case FunctionEnum::ProcessNestedUIActionsAndEvents:
      ProcessNestedUIActionsAndEvents(context.scene_entities,
                                      context.archetypes,
                                      context.event_handler);
      return std::monostate{};
      
    case FunctionEnum::ProcessButtonElementActions:
      ProcessButtonElementActions(context.scene_entities,
                                  context.archetypes,
                                  context.event_handler);
      return std::monostate{};
      
    case FunctionEnum::ProcessDropDownListElementActions:
      ProcessDropDownListElementActions(context.scene_entities,
                                        context.archetypes,
                                        context.event_handler);
      return std::monostate{};
      
    case FunctionEnum::CheckMouseOverNestedUIElement:
      CheckMouseOverNestedUIElement(context.scene_entities,
                                    context.archetypes,
                                    context.mouse_position);
      return std::monostate{};
      
    case FunctionEnum::UpdateCUserInterfaceVisibilityFromCUIState:
      UpdateCUserInterfaceVisibilityFromCUIState(context.scene_entities,
                                                 context.archetypes);
      return std::monostate{};
      
    case FunctionEnum::None:
    default:
      return std::unexpected(FailInfo{
        FailMode::InvalidEnumValue,
        "Invalid FunctionEnum value in simulation step"
      });
  }
}

} // namespace steamrot::tests
```

### Phase 4: Variant Dispatch

#### Step 4.1: Implement ExecuteStep with std::visit

Update `SimulationRunner.cpp`:

```cpp
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SimulationRunner::ExecuteStep(const SimulationStep &step) {
  return std::visit(
      [this](auto &&element) -> std::expected<std::monostate, FailInfo> {
        using T = std::decay_t<decltype(element)>;
        if constexpr (std::is_same_v<T, FunctionEnum>) {
          return ExecuteFunction(element, m_scene_context);
        } else if constexpr (std::is_same_v<T, LogicClassEnum>) {
          return ExecuteLogicClass(element, m_scene_context);
        } else {
          return std::unexpected(FailInfo{
            FailMode::VariantTypeMismatch,
            "Unknown simulation element type"
          });
        }
      },
      step.element);
}
```

### Phase 5: TestEngine Integration

#### Step 5.1: Update TestEngine::TickSceneLogic

**File**: `tests/harness/TestEngine.cpp`

Add include:
```cpp
#include "SimulationRunner.h"
```

Implement TickSceneLogic:
```cpp
/////////////////////////////////////////////////
void TestEngine::TickSceneLogic() {
  // Check if simulation data is available
  if (m_test_data.simulation_data.steps.empty()) {
    return;  // No simulation to run
  }
  
  // Get scenes from scene manager
  auto &scenes = m_scene_manager.GetScenes();
  if (scenes.empty()) {
    // No scene available - log or handle error
    return;
  }
  
  // Get first scene (TestEngine typically has one test scene)
  Scene *scene = scenes[0].get();
  if (!scene || !scene->GetActive()) {
    // Scene not active - skip simulation
    return;
  }
  
  // Get SceneContext from scene
  SceneContext scene_context = scene->GetSceneContext();
  
  // Create and execute simulation
  SimulationRunner runner(m_test_data.simulation_data, scene_context);
  auto result = runner.ExecuteSimulation();
  
  // Handle errors
  if (!result.has_value()) {
    // Throw exception to fail the test with clear error message
    throw std::runtime_error(
        std::format("Simulation failed at tick {}: {}",
                    m_current_tick,
                    result.error().message));
  }
}
```

### Phase 6: Build Integration

#### Step 6.1: Update CMakeLists.txt

**File**: `tests/harness/CMakeLists.txt`

Add SimulationRunner to the test harness library:

```cmake
# Add to existing harness library sources
add_library(test_harness
  # ... existing files ...
  SimulationRunner.h
  SimulationRunner.cpp
)

# Ensure necessary dependencies are linked
target_link_libraries(test_harness
  # ... existing links ...
  steamrot_logic  # For Logic classes
)
```

### Phase 7: Testing

#### Step 7.1: Create Unit Tests

**File**: `tests/harness/SimulationRunner.test.cpp`

```cpp
#include "SimulationRunner.h"
#include <catch2/catch_test_macros.hpp>

// TODO: Add TestContext or mock SceneContext setup

TEST_CASE("SimulationRunner executes single Logic class", 
          "[unit][SimulationRunner]") {
  // Setup test data and context
  // Execute simulation
  // Verify results
  REQUIRE(true);  // Placeholder
}

TEST_CASE("SimulationRunner executes single function", 
          "[unit][SimulationRunner]") {
  // Setup test data and context
  // Execute simulation
  // Verify results
  REQUIRE(true);  // Placeholder
}

TEST_CASE("SimulationRunner handles invalid enum", 
          "[unit][SimulationRunner]") {
  // Test error handling for invalid enum values
  REQUIRE(true);  // Placeholder
}
```

Update `tests/harness/CMakeLists.txt`:
```cmake
add_executable(test_harness_unit
  # ... existing test files ...
  SimulationRunner.test.cpp
)
```

#### Step 7.2: Create Integration Tests

**File**: `tests/harness/TestEngine.test.cpp` (add to existing file)

```cpp
TEST_CASE("TestEngine executes simulation", 
          "[unit][TestEngine][simulation]") {
  // Create TestData with simulation_data
  // Run TestEngine
  // Verify simulation was executed
  REQUIRE(true);  // Placeholder
}
```

## Validation Checklist

Before considering implementation complete:

- [ ] SimulationRunner compiles without errors
- [ ] All Logic class enums have cases in ExecuteLogicClass
- [ ] All function enums have cases in ExecuteFunction
- [ ] Function signatures verified against actual implementations
- [ ] TestEngine::TickSceneLogic calls SimulationRunner
- [ ] Error handling returns FailInfo with descriptive messages
- [ ] Unit tests written for SimulationRunner
- [ ] Integration tests written for TestEngine
- [ ] CMakeLists.txt updated with new files
- [ ] Code follows project style guide (2-space indent, Doxygen comments)
- [ ] Visual dividers used appropriately

## Common Issues and Solutions

### Issue: Function Signature Mismatch

**Symptom**: Compiler errors when calling free functions

**Solution**: 
1. View actual function declarations in logic header files
2. Update ExecuteFunction to match exact signatures
3. Pass correct parameters from SceneContext

### Issue: SceneContext Invalid

**Symptom**: Crash or unexpected behavior during simulation

**Solution**:
1. Verify Scene exists and is active before getting context
2. Ensure SceneContext references remain valid during simulation
3. Check that EntityManager is properly initialized

### Issue: Logic Instance Not Running

**Symptom**: Simulation completes but has no effect

**Solution**:
1. Verify Logic class constructor succeeds
2. Check that Logic::RunLogic() is called (not ProcessLogic())
3. Ensure entities/archetypes exist for Logic to process

### Issue: Enum Conversion Fails

**Symptom**: "Invalid enum value" errors

**Solution**:
1. Check FlatBuffers enum names match C++ enum names
2. Verify conversion functions in FlatbuffersTestDataProvider
3. Ensure JSON uses correct enum string values

## Extension Guide

### Adding New Logic Classes

1. Implement Logic class (standard process)
2. Add to `LogicClassEnum` in `src/types/test_structs/SimulationData.h`
3. Add to `LogicClassEnumFbs` in `src/types/flatbuffers/testing/simulation_data.fbs`
4. Add case in `ExecuteLogicClass()` in `SimulationRunner.cpp`
5. Add conversion in `ConvertFbsToLogicClassEnum()` in `FlatbuffersTestDataProvider.cpp`
6. Add unit test for new Logic class execution

### Adding New Free Functions

1. Implement function in appropriate logic file
2. Add to `FunctionEnum` in `src/types/test_structs/SimulationData.h`
3. Add to `FunctionEnumFbs` in `src/types/flatbuffers/testing/simulation_data.fbs`
4. Add case in `ExecuteFunction()` in `SimulationRunner.cpp`
5. Add conversion in `ConvertFbsToFunctionEnum()` in `FlatbuffersTestDataProvider.cpp`
6. Add unit test for new function execution

## Next Steps After Implementation

1. **Test with simple scenarios** - Single step simulations
2. **Add complexity gradually** - Multi-step sequences
3. **Test error paths** - Invalid enums, missing scenes
4. **Document examples** - Add real test data files
5. **Update main README** - Document simulation feature

## Resources

- **Design Document**: `documentation/SIMULATION_RUNNER_DESIGN.md`
- **Examples**: `documentation/examples/simulation_examples.md`
- **Test Harness README**: `tests/harness/README.md`
- **Logic Classes**: `src/logic/`
- **SceneContext**: `src/context/SceneContext.h`

---

**Document Version**: 1.0  
**Date**: 2026-01-25  
**For**: Developer Implementation Reference
