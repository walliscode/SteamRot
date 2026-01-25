# Simulation Runner Design for TestEngine

## Executive Summary

This document provides a comprehensive design analysis for the simulation runner component of the TestEngine. The simulation runner enables data-driven test execution by allowing tests to specify sequences of free functions and Logic class instances via JSON configuration, providing isolated testing of specific game behaviors without requiring recompilation.

## Table of Contents

- [Overview](#overview)
- [Current Architecture Analysis](#current-architecture-analysis)
- [Design Requirements](#design-requirements)
- [Proposed Architecture](#proposed-architecture)
- [Component Design](#component-design)
- [Robustness Considerations](#robustness-considerations)
- [JSON Configuration Schema](#json-configuration-schema)
- [Implementation Guidelines](#implementation-guidelines)
- [Integration with TestEngine](#integration-with-testengine)
- [Future Extensibility](#future-extensibility)

## Overview

### Purpose

The simulation runner is a critical component of the TestEngine that executes game logic in a controlled, data-driven manner. It allows test authors to:

1. **Specify logic sequences** - Define ordered execution of functions and Logic classes
2. **Isolate effects** - Test specific behaviors within the Engine context
3. **Configure via JSON** - Change test scenarios without recompiling
4. **Validate state** - Capture and compare game state at each tick

### Key Benefits

- **No recompilation** - Test scenarios defined in JSON files
- **Isolation** - Execute only the logic needed for a specific test
- **Flexibility** - Mix free functions and Logic classes in any order
- **Maintainability** - Clear separation between test configuration and implementation

## Current Architecture Analysis

### Existing Components

#### 1. TestEngine (tests/harness/TestEngine.h)

The TestEngine extends the Engine base class and provides:
- **Tick execution** - Runs the standard Engine tick pipeline
- **Data capture** - Stores EngineSnapshots at each tick
- **Simulation hook** - `TickSceneLogic()` method (currently empty)

```cpp
class TestEngine : public Engine {
  void TickSceneLogic() override;  // Currently empty - simulation entry point
  void RunGameLoop() override;     // Executes ticks and captures data
  const TestData &m_test_data;     // Configuration including SimulationData
};
```

#### 2. SimulationData (src/types/test_structs/SimulationData.h)

Defines the data structure for simulation configuration:

```cpp
struct SimulationData {
  std::string description;
  std::vector<SimulationStep> steps;
};

struct SimulationStep {
  SimulationElement element;  // variant<FunctionEnum, LogicClassEnum>
};

enum class FunctionEnum {
  None,
  ProcessUIActionsAndEvents,
  ProcessNestedUIActionsAndEvents,
  ProcessButtonElementActions,
  ProcessDropDownListElementActions,
  CheckMouseOverNestedUIElement,
  UpdateCUserInterfaceVisibilityFromCUIState
};

enum class LogicClassEnum {
  None,
  UIActionLogic,
  UICollisionLogic,
  UIRenderLogic,
  UIStateLogic,
  CraftingRenderLogic
};
```

#### 3. Logic Classes (src/logic/)

The game uses a Logic class hierarchy:
- **Base class**: `Logic` (abstract)
- **Derived classes**: `UIActionLogic`, `UICollisionLogic`, `UIRenderLogic`, etc.
- **Requirement**: All Logic classes need a `SceneContext` for construction
- **Execution**: `RunLogic()` calls protected `ProcessLogic()`

```cpp
class Logic {
protected:
  SceneContext m_scene_context;
  virtual void ProcessLogic() = 0;
public:
  Logic(const SceneContext scene_context);
  void RunLogic();  // Public interface
};
```

#### 4. SceneContext (src/context/SceneContext.h)

Provides access to scene and engine resources:

```cpp
struct SceneContext {
  EntityMemoryPool &scene_entities;
  const std::unordered_map<ArchetypeID, Archetype> &archetypes;
  sf::RenderTexture &scene_texture;
  sf::RenderWindow &game_window;
  AssetManager &asset_manager;
  EventHandler &event_handler;
  sf::Vector2i &mouse_position;
};
```

### Current Limitations

1. **No simulation execution** - `TestEngine::TickSceneLogic()` is empty
2. **No Logic instantiation** - No mechanism to create Logic instances from enum
3. **No function mapping** - No dispatch mechanism for FunctionEnum values
4. **No archetype management** - Unclear how to apply archetypes for tests
5. **No SceneContext creation** - TestEngine needs to provide context to Logic

## Design Requirements

### Functional Requirements

1. **Execute simulation steps** in the order specified in JSON
2. **Support both free functions and Logic classes** in the same simulation
3. **Create Logic instances** dynamically based on LogicClassEnum
4. **Provide SceneContext** to Logic instances
5. **Execute free functions** with appropriate parameters
6. **Handle errors gracefully** with detailed failure information
7. **Support multiple ticks** with simulation running each tick

### Non-Functional Requirements

1. **Robustness** - Handle invalid configurations without crashes
2. **Extensibility** - Easy to add new functions/Logic classes
3. **Performance** - Minimal overhead for Logic instantiation
4. **Maintainability** - Clear code structure and documentation
5. **Type safety** - Leverage C++ type system
6. **No recompilation** - All test scenarios configurable via JSON

## Proposed Architecture

### High-Level Design

```
TestEngine::TickSceneLogic()
    ↓
SimulationRunner::ExecuteSimulation(SimulationData, SceneContext)
    ↓
For each SimulationStep:
    ↓
    Is FunctionEnum? → FunctionDispatcher::Execute(function, context)
    ↓
    Is LogicClassEnum? → LogicFactory::CreateAndExecute(logic_class, context)
```

### Component Responsibilities

1. **SimulationRunner** - Orchestrates simulation execution
2. **LogicFactory** - Creates and manages Logic instances
3. **FunctionDispatcher** - Executes free functions
4. **TestEngine** - Provides entry point and context

## Component Design

### 1. SimulationRunner

**Location**: `tests/harness/SimulationRunner.h/cpp`

**Responsibility**: Orchestrate execution of simulation steps

**Interface**:

```cpp
namespace steamrot::tests {

class SimulationRunner {
private:
  const SimulationData &m_simulation_data;
  SceneContext &m_scene_context;
  
  std::expected<std::monostate, FailInfo>
  ExecuteStep(const SimulationStep &step);

public:
  SimulationRunner(const SimulationData &simulation_data,
                   SceneContext &scene_context);
  
  std::expected<std::monostate, FailInfo> ExecuteSimulation();
};

} // namespace steamrot::tests
```

**Implementation Strategy**:

```cpp
std::expected<std::monostate, FailInfo>
SimulationRunner::ExecuteSimulation() {
  for (const auto &step : m_simulation_data.steps) {
    auto result = ExecuteStep(step);
    if (!result.has_value()) {
      return std::unexpected(result.error());
    }
  }
  return std::monostate{};
}

std::expected<std::monostate, FailInfo>
SimulationRunner::ExecuteStep(const SimulationStep &step) {
  return std::visit(overloaded{
    [this](FunctionEnum func) -> std::expected<std::monostate, FailInfo> {
      return ExecuteFunction(func, m_scene_context);
    },
    [this](LogicClassEnum logic) -> std::expected<std::monostate, FailInfo> {
      return ExecuteLogicClass(logic, m_scene_context);
    }
  }, step.element);
}
```

### 2. Logic Instantiation Strategy

**Key Insight**: Logic classes should be instantiated **per simulation step**, not cached.

**Rationale**:
1. **Stateless execution** - Each step starts fresh
2. **Simple lifecycle** - No need to manage instance lifetime
3. **Isolation** - Steps don't affect each other
4. **Clear semantics** - One step = one Logic execution

**Implementation**:

```cpp
namespace steamrot::tests {

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
        "Invalid LogicClassEnum value"
      });
  }
}

} // namespace steamrot::tests
```

**Benefits**:
- No memory management complexity
- No state carried between steps
- Stack allocation (fast)
- RAII for resource cleanup

**Trade-offs**:
- Repeated construction (acceptable for tests)
- No instance reuse (not needed for isolated tests)

### 3. Free Function Execution

**Strategy**: Direct function calls with SceneContext

**Implementation**:

```cpp
namespace steamrot::tests {

std::expected<std::monostate, FailInfo>
ExecuteFunction(FunctionEnum function, SceneContext &context) {
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
        "Invalid FunctionEnum value"
      });
  }
}

} // namespace steamrot::tests
```

**Note**: This requires knowing the exact signature of each free function. The implementation should reference the actual function declarations.

### 4. Archetype Application

**Current Understanding**: Archetypes are managed by EntityManager

**Approach**: Archetypes are automatically maintained by the EntityManager based on component activation/deactivation. The simulation runner doesn't need to manage archetypes directly.

**Key Points**:
1. **Automatic management** - EntityManager handles archetype updates
2. **Query-based access** - Logic classes query archetypes via SceneContext
3. **No manual application** - Archetypes update when components change
4. **Test setup** - Initial entity/component configuration sets up archetypes

**Test Data Preparation**:
- Use `starting_engine_snapshot` in TestData to set initial state
- Entity component activation creates appropriate archetypes
- Simulation then operates on these archetypes

### 5. SceneContext Provision

**Challenge**: TestEngine needs to provide a valid SceneContext

**Solution**: TestEngine must access Scene resources

**Implementation in TestEngine**:

```cpp
void TestEngine::TickSceneLogic() {
  // Check if simulation data is available
  if (m_test_data.simulation_data.steps.empty()) {
    return;  // No simulation to run
  }
  
  // Get active scene (TestEngine typically has one test scene)
  auto &scenes = m_scene_manager.GetScenes();
  if (scenes.empty()) {
    // Log error or return - no scene to simulate
    return;
  }
  
  // Get first active scene (TestEngine convention)
  Scene *scene = scenes[0].get();
  if (!scene || !scene->GetActive()) {
    return;
  }
  
  // Create SceneContext from scene
  SceneContext scene_context = scene->GetSceneContext();
  
  // Execute simulation
  SimulationRunner runner(m_test_data.simulation_data, scene_context);
  auto result = runner.ExecuteSimulation();
  
  if (!result.has_value()) {
    // Handle error - log, throw, or store in test results
    // For now, we might want to throw to fail the test
    throw std::runtime_error(result.error().message);
  }
}
```

**Key Considerations**:
- SceneContext contains references, so must remain valid during simulation
- TestEngine must ensure scene exists and is initialized
- Error handling should be clear for test debugging

## Robustness Considerations

### 1. Error Handling

**Strategy**: Use `std::expected` throughout the chain

**Error Propagation**:
```
ExecuteSimulation() 
  → ExecuteStep() 
    → ExecuteFunction() / ExecuteLogicClass() 
      → Returns FailInfo on error
```

**Error Information**:
- Use `FailMode` enum for categorization
- Include descriptive messages with context
- Preserve error chain for debugging

### 2. Validation

**Configuration Validation**:
```cpp
std::expected<std::monostate, FailInfo>
ValidateSimulationData(const SimulationData &data) {
  // Check for empty steps
  if (data.steps.empty()) {
    return std::unexpected(FailInfo{
      FailMode::InvalidConfiguration,
      "Simulation has no steps"
    });
  }
  
  // Check each step has valid enum value
  for (const auto &step : data.steps) {
    if (std::holds_alternative<FunctionEnum>(step.element)) {
      auto func = std::get<FunctionEnum>(step.element);
      if (func == FunctionEnum::None) {
        return std::unexpected(FailInfo{
          FailMode::InvalidEnumValue,
          "Step has FunctionEnum::None"
        });
      }
    } else if (std::holds_alternative<LogicClassEnum>(step.element)) {
      auto logic = std::get<LogicClassEnum>(step.element);
      if (logic == LogicClassEnum::None) {
        return std::unexpected(FailInfo{
          FailMode::InvalidEnumValue,
          "Step has LogicClassEnum::None"
        });
      }
    }
  }
  
  return std::monostate{};
}
```

### 3. Extensibility

**Adding New Functions**:
1. Add to `FunctionEnum` in `SimulationData.h`
2. Add to `FunctionEnumFbs` in `simulation_data.fbs`
3. Add case in `ExecuteFunction()` switch
4. Add conversion in `ConvertFbsToFunctionEnum()`

**Adding New Logic Classes**:
1. Implement new Logic class (standard process)
2. Add to `LogicClassEnum` in `SimulationData.h`
3. Add to `LogicClassEnumFbs` in `simulation_data.fbs`
4. Add case in `ExecuteLogicClass()` switch
5. Add conversion in `ConvertFbsToLogicClassEnum()`

**Maintaining Synchronization**:
- Both enums (C++ and FlatBuffers) must stay in sync
- Conversion functions must handle all values
- Use `static_assert` or compile-time checks where possible
- Add unit tests for enum conversions

### 4. Performance Considerations

**Per-Step Logic Instantiation**:
- **Cost**: Constructor/destructor overhead per step
- **Benefit**: Simplicity, isolation, no state bugs
- **Mitigation**: Tests are not performance-critical
- **Alternative**: Could cache instances if needed (not recommended initially)

**Memory Allocation**:
- Stack allocation for Logic instances (fast)
- No heap allocation needed for basic use case
- SceneContext passed by reference (no copy)

### 5. Testing the Simulation Runner

**Unit Tests**:
- Test `ExecuteFunction()` with each FunctionEnum
- Test `ExecuteLogicClass()` with each LogicClassEnum
- Test `ExecuteSimulation()` with various step sequences
- Test error handling for invalid enums
- Test validation logic

**Integration Tests**:
- Test full TestEngine workflow with simulation
- Verify state changes after simulation
- Test multi-tick simulations
- Test combinations of functions and Logic classes

## JSON Configuration Schema

### FlatBuffers Schema (simulation_data.fbs)

The current schema already supports the design:

```fbs
enum FunctionEnumFbs: byte {
  None = 0,
  ProcessUIActionsAndEvents,
  ProcessNestedUIActionsAndEvents,
  ProcessButtonElementActions,
  ProcessDropDownListElementActions,
  CheckMouseOverNestedUIElement,
  UpdateCUserInterfaceVisibilityFromCUIState
}

enum LogicClassEnumFbs : byte {
  None = 0,
  UIActionLogic = 1,
  UICollisionLogic = 2,
  UIRenderLogic = 3,
  UIStateLogic = 4,
  CraftingRenderLogic = 5,
}

table SimulationStepFbs {
  function_type: FunctionEnumFbs = None;
  logic_class_type: LogicClassEnumFbs = None;
}

table SimulationDataFbs {
  steps: [SimulationStepFbs];
  description: string;
}
```

### Example JSON Configurations

#### Example 1: Simple UI Collision Test

```json
{
  "simulation_data": {
    "description": "Test UI collision detection",
    "steps": [
      {
        "logic_class_type": "UICollisionLogic"
      }
    ]
  }
}
```

#### Example 2: UI Action Workflow

```json
{
  "simulation_data": {
    "description": "Test button click processing",
    "steps": [
      {
        "logic_class_type": "UICollisionLogic"
      },
      {
        "function_type": "ProcessButtonElementActions"
      },
      {
        "function_type": "ProcessUIActionsAndEvents"
      }
    ]
  }
}
```

#### Example 3: Complex UI Interaction

```json
{
  "simulation_data": {
    "description": "Full UI interaction cycle",
    "steps": [
      {
        "function_type": "CheckMouseOverNestedUIElement"
      },
      {
        "logic_class_type": "UICollisionLogic"
      },
      {
        "function_type": "ProcessNestedUIActionsAndEvents"
      },
      {
        "logic_class_type": "UIActionLogic"
      },
      {
        "function_type": "UpdateCUserInterfaceVisibilityFromCUIState"
      }
    ]
  }
}
```

#### Example 4: Rendering Pipeline

```json
{
  "simulation_data": {
    "description": "Test rendering logic for crafting scene",
    "steps": [
      {
        "logic_class_type": "UIStateLogic"
      },
      {
        "logic_class_type": "CraftingRenderLogic"
      },
      {
        "logic_class_type": "UIRenderLogic"
      }
    ]
  }
}
```

#### Example 5: Multi-Tick Simulation

```json
{
  "meta_data": {
    "test_name": "ui_button_multi_tick",
    "test_description": "Test button state across multiple ticks"
  },
  "simulation_data": {
    "description": "Simulate button press over 3 ticks",
    "steps": [
      {
        "logic_class_type": "UICollisionLogic"
      },
      {
        "logic_class_type": "UIActionLogic"
      },
      {
        "logic_class_type": "UIStateLogic"
      }
    ]
  },
  "num_ticks": 3
}
```

**Note**: The same simulation steps run on each tick. For tick-specific behavior, use input/event sequences.

### Configuration Best Practices

1. **Use descriptive names** - Clear description field for debugging
2. **Order matters** - Steps execute in array order
3. **One element per step** - Either function_type OR logic_class_type
4. **Validation enforced** - FlatBuffers provider validates exactly one set
5. **Start simple** - Begin with single-step simulations
6. **Build complexity** - Add steps as needed for test scenarios

## Implementation Guidelines

### Step-by-Step Implementation Plan

#### Phase 1: Foundation (Core Implementation)

1. **Create SimulationRunner class** (`tests/harness/SimulationRunner.h/cpp`)
   - Implement constructor with SimulationData and SceneContext
   - Implement `ExecuteSimulation()` method
   - Implement `ExecuteStep()` with variant visitor

2. **Implement Logic execution** (in `SimulationRunner.cpp`)
   - Create `ExecuteLogicClass()` function
   - Add switch for each LogicClassEnum value
   - Instantiate and execute each Logic type

3. **Implement function execution** (in `SimulationRunner.cpp`)
   - Create `ExecuteFunction()` function
   - Add switch for each FunctionEnum value
   - Call appropriate free functions with context parameters

4. **Update TestEngine** (`tests/harness/TestEngine.cpp`)
   - Implement `TickSceneLogic()` method
   - Get SceneContext from active scene
   - Create SimulationRunner and execute

#### Phase 2: Validation and Error Handling

5. **Add validation**
   - Validate SimulationData before execution
   - Check for empty steps
   - Validate enum values

6. **Enhance error reporting**
   - Add contextual error messages
   - Include step number in errors
   - Preserve error chain

#### Phase 3: Testing

7. **Write unit tests** (`tests/harness/SimulationRunner.test.cpp`)
   - Test each FunctionEnum execution
   - Test each LogicClassEnum execution
   - Test error cases
   - Test validation logic

8. **Write integration tests** (`tests/harness/TestEngine.test.cpp`)
   - Test full TestEngine with simulation
   - Test multi-tick simulations
   - Test state capture after simulation

#### Phase 4: Documentation

9. **Update README** (`tests/harness/README.md`)
   - Document simulation runner usage
   - Add JSON configuration examples
   - Explain execution model

10. **Add inline documentation**
    - Doxygen comments for all public APIs
    - Implementation notes for complex logic
    - Examples in header comments

### Code Organization

**File Structure**:
```
tests/harness/
├── SimulationRunner.h           # New - Simulation execution engine
├── SimulationRunner.cpp         # New - Implementation
├── SimulationRunner.test.cpp    # New - Unit tests
├── TestEngine.h                 # Existing - Update TickSceneLogic()
├── TestEngine.cpp               # Existing - Implement TickSceneLogic()
├── TestEngine.test.cpp          # Existing - Add simulation tests
└── README.md                    # Existing - Update with simulation docs
```

**Dependencies**:
```
SimulationRunner
  ├─ depends on: SimulationData (types)
  ├─ depends on: SceneContext (context)
  ├─ depends on: Logic classes (logic)
  ├─ depends on: Free functions (logic/*.h)
  └─ depends on: FailInfo (types)

TestEngine
  ├─ depends on: SimulationRunner (harness)
  ├─ depends on: TestData (types)
  └─ depends on: Scene (scenes)
```

### Function Signature Reference

**Important**: Verify actual function signatures in the codebase before implementation.

Free functions are likely in:
- `src/logic/logic_action.h` - Action-related functions
- `src/logic/logic_collision.h` - Collision-related functions
- `src/logic/logic_ui.h` - UI-related functions

**Example expected signatures**:
```cpp
void ProcessUIActionsAndEvents(
    EntityMemoryPool &entities,
    const std::unordered_map<ArchetypeID, Archetype> &archetypes,
    EventHandler &event_handler);

void CheckMouseOverNestedUIElement(
    EntityMemoryPool &entities,
    const std::unordered_map<ArchetypeID, Archetype> &archetypes,
    const sf::Vector2i &mouse_position);
```

### Naming Conventions

Follow project style guide:
- **Classes**: PascalCase (e.g., `SimulationRunner`)
- **Functions**: PascalCase (e.g., `ExecuteSimulation`)
- **Variables**: snake_case with `m_` prefix for members
- **Files**: Match class name (e.g., `SimulationRunner.h/cpp`)

### Testing Strategy

**Test Pyramid**:
1. **Unit tests** (60%) - Test individual functions in isolation
2. **Integration tests** (30%) - Test component interactions
3. **End-to-end tests** (10%) - Test full workflows

**Coverage Goals**:
- All FunctionEnum values
- All LogicClassEnum values
- All error paths
- Validation logic
- Edge cases (empty steps, invalid enums)

**Test Data Files**:
Place test configurations in `tests/harness/data/`:
- `simulation_function_001.test_data.json` - Function tests
- `simulation_logic_001.test_data.json` - Logic class tests
- `simulation_mixed_001.test_data.json` - Combined tests

## Integration with TestEngine

### Execution Flow

```
Test Start
    ↓
TestEngine constructed with TestData
    ↓
TestEngine::RunGame()
    ↓
TestEngine::StartUp() - Initialize resources
    ↓
TestEngine::RunGameLoop() - Execute ticks
    ↓
For each tick (1 to m_target_ticks):
    ↓
    TestEngine::ExecuteTick()
        ↓
        TickEvents()          - Process SFML events
        TickEngineLogic()     - Engine subscriptions
        TickSceneManager()    - Scene manager logic
        TickSceneLogic()      - *** SIMULATION RUNS HERE ***
        TickRendering()       - Scene rendering (for validation)
        ↓
    TestEngine::StoreEngineSnapshot() - Capture state
    ↓
Compare snapshots with expected results
```

### Integration Points

1. **Construction** - TestData passed to TestEngine
2. **StartUp** - Engine initializes, creates scenes
3. **TickSceneLogic** - Simulation executes each tick
4. **State Capture** - Snapshots taken after simulation
5. **Validation** - Snapshots compared with expected values

### Scene Management

**TestEngine Assumptions**:
- Typically has one test scene
- Scene is active during test execution
- Scene provides SceneContext
- Scene EntityManager manages entities/archetypes

**Scene Selection**:
```cpp
// In TestEngine::TickSceneLogic()
auto &scenes = m_scene_manager.GetScenes();
if (!scenes.empty()) {
  Scene *scene = scenes[0].get();  // First scene
  if (scene && scene->GetActive()) {
    // Execute simulation on this scene
  }
}
```

### Data Flow

```
JSON Test Config
    ↓
FlatBuffers Compilation (build time)
    ↓
Binary .test_data.bin files
    ↓
FlatbuffersTestDataLoader (runtime)
    ↓
FlatbuffersTestDataProvider
    ↓
TestData struct (includes SimulationData)
    ↓
TestEngine construction
    ↓
SimulationRunner (each tick)
    ↓
Logic classes / Free functions
    ↓
EntityMemoryPool modifications
    ↓
EngineSnapshot capture
    ↓
Comparison with expected results
```

## Future Extensibility

### Potential Enhancements

#### 1. Per-Tick Simulation Control

**Need**: Different simulation steps for different ticks

**Approach**: Extend SimulationData to map tick number to steps

```cpp
struct SimulationData {
  std::string description;
  std::map<size_t, std::vector<SimulationStep>> tick_specific_steps;
  std::vector<SimulationStep> default_steps;  // Run on all ticks
};
```

**JSON Example**:
```json
{
  "simulation_data": {
    "default_steps": [
      { "logic_class_type": "UIStateLogic" }
    ],
    "tick_1_steps": [
      { "function_type": "ProcessButtonElementActions" }
    ],
    "tick_2_steps": [
      { "logic_class_type": "UIActionLogic" }
    ]
  }
}
```

#### 2. Conditional Execution

**Need**: Execute steps based on state conditions

**Approach**: Add condition field to SimulationStep

```cpp
struct SimulationCondition {
  enum class Type { Always, ComponentActive, ArchetypeExists };
  Type type;
  std::variant<std::monostate, ComponentType, ArchetypeID> value;
};

struct SimulationStep {
  SimulationElement element;
  SimulationCondition condition;  // New
};
```

#### 3. Parameterized Functions

**Need**: Pass parameters to functions

**Approach**: Add parameter support to FunctionEnum

```cpp
struct FunctionParameters {
  std::variant<
    std::monostate,
    int,
    float,
    std::string,
    std::vector<int>
  > value;
};

struct SimulationStep {
  SimulationElement element;
  FunctionParameters params;  // New
};
```

**Limitation**: Would require functions to accept generic parameters
**Alternative**: Use test data setup instead of runtime parameters

#### 4. Execution Statistics

**Need**: Track simulation performance and execution

**Approach**: Add metrics collection to SimulationRunner

```cpp
struct SimulationMetrics {
  size_t steps_executed;
  std::chrono::microseconds total_time;
  std::map<std::string, std::chrono::microseconds> step_times;
};

class SimulationRunner {
  SimulationMetrics GetMetrics() const;
};
```

#### 5. Nested Simulations

**Need**: Reusable simulation sequences

**Approach**: Allow simulation steps to reference other simulations

```cpp
enum class SimulationType { Inline, Reference };

struct SimulationStep {
  SimulationType type;
  std::variant<SimulationElement, std::string> content;
};
```

**Note**: Adds complexity, evaluate need before implementing

### Extension Checklist

When adding new simulation capabilities:

1. **Update enums** (C++ and FlatBuffers)
2. **Update conversion functions**
3. **Update execution functions**
4. **Add unit tests**
5. **Update documentation**
6. **Add JSON examples**
7. **Verify backward compatibility**

## Conclusion

### Summary

This design provides a robust, extensible simulation runner for the TestEngine that:

1. **Executes Logic classes and free functions** in configurable sequences
2. **Supports data-driven testing** via JSON configuration
3. **Maintains isolation** through per-step Logic instantiation
4. **Handles errors gracefully** with std::expected pattern
5. **Integrates cleanly** with existing TestEngine architecture
6. **Requires no recompilation** for new test scenarios

### Key Design Decisions

1. **Per-step instantiation** - Simple, isolated, no state management
2. **Direct function calls** - No complex dispatch mechanism needed
3. **Automatic archetypes** - Managed by EntityManager, not simulation
4. **SceneContext provision** - TestEngine accesses Scene to get context
5. **Validation at load time** - Catch configuration errors early

### Implementation Priority

**High Priority** (Core Functionality):
1. SimulationRunner basic implementation
2. Logic class execution
3. Free function execution
4. TestEngine integration
5. Basic error handling

**Medium Priority** (Robustness):
1. Comprehensive validation
2. Enhanced error messages
3. Unit tests
4. Integration tests

**Low Priority** (Future Enhancements):
1. Per-tick simulation control
2. Conditional execution
3. Execution statistics
4. Advanced features

### Robustness Assessment

The proposed design is robust because:

1. **Type-safe** - Uses C++ type system and std::variant
2. **Validated** - Configuration checked at load time
3. **Isolated** - Steps don't share state
4. **Testable** - Clear separation of concerns
5. **Extensible** - Easy to add new functions/Logic classes
6. **Maintainable** - Simple code structure
7. **Error-handled** - std::expected throughout

### Next Steps

For implementation:

1. Review this design document with stakeholders
2. Verify free function signatures in codebase
3. Create SimulationRunner skeleton
4. Implement core execution logic
5. Add TestEngine integration
6. Write comprehensive tests
7. Update documentation

---

**Document Version**: 1.0  
**Date**: 2026-01-25  
**Author**: GitHub Copilot (Analysis and Documentation)  
**Status**: Draft for Review
