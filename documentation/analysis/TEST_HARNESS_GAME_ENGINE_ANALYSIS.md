# Test Harness and Game Engine Architecture Analysis

## Executive Summary

This document analyzes the relationship between the test harness and game engine, identifying opportunities for code reuse and proposing a unified architecture where:

1. **The Test Harness** is an application of the game engine core with the ability to:
   - Create custom logic workflows
   - Inject data at runtime
   - Run at multiple levels: Logic, Scene, SceneManager, or full GameEngine

2. **The Game Engine** is a locked-down version without runtime injection, with configuration fixed at compile time to eliminate runtime guards.

## Current Architecture Overview

### Game Engine Hierarchy

```
GameEngine
├── GameResources (owns)
│   ├── game_window: RenderWindow
│   ├── event_handler: EventHandler
│   ├── mouse_position: Vector2i
│   ├── loop_number: size_t
│   ├── asset_manager: AssetManager
│   └── env_type: EnvironmentType
│
├── GameContext (references GameResources members)
├── SceneManager
│   ├── scenes: map<uuid, Scene>
│   └── subscriptions: map<EventType, Subscriber>
│
└── DisplayManager
```

### Scene Structure

```
Scene
├── SceneInfo (id, type)
├── EntityManager
│   ├── EntityMemoryPool
│   └── ArchetypeManager
├── ActionManager
├── GameContext& (reference)
├── LogicCollection: map<LogicType, vector<Logic>>
├── SceneResources
│   └── scene_texture: RenderTexture
└── scene_event_types
```

### Current Test Harness Structure

```
TestFixture
├── path_provider: PathProvider (Test)
├── m_game_resources: GameResources (owns)
├── m_scene_resources: SceneResources (owns)
├── m_entity_manager: EntityManager (owns)
├── m_game_context: unique_ptr<GameContext> (lazy created)
├── m_scene_context: unique_ptr<SceneContext> (lazy created)
└── m_scene_type: SceneType
```

### Key Observation

The test harness (`TestFixture`) and `GameEngine` both:
1. Own `GameResources`
2. Create contexts (`GameContext`, `SceneContext`)
3. Configure resources from FlatBuffers data
4. Manage entity memory pools

**Critical Difference**: The test harness operates at the scene/logic level without the full game loop, while GameEngine runs the complete loop.

## FlatBuffers Schema Analysis

### Current Schema Structure

The FlatBuffers schemas are organized into two categories:

#### 1. Production Schemas (Game Engine)

```
src/flatbuffers_headers/
├── game_engine.fbs          # GameEngineData (subscriptions + resources)
├── scene_manager.fbs        # SceneManagerData (subscriptions)
├── scenes.fbs               # SceneData (entities + logic + assets + resources)
├── entities.fbs             # EntityData + EntityCollection
├── logic_data.fbs           # LogicData + LogicCollectionData
├── resource_data.fbs        # GameResourcesData + SceneResourcesData
├── context_data.fbs         # GameContextConfig + SceneContextConfig
├── events.fbs               # Event types and definitions
├── subscriber_data.fbs      # Subscriber configuration
└── [component schemas]      # user_interface.fbs, grimoire_machina.fbs, etc.
```

#### 2. Test Schemas (Test Harness)

```
src/flatbuffers_headers/
├── test_data.fbs            # TestDataConfig (root test configuration)
├── simulation.fbs           # SimulationData + SimulationStep
├── input_test_data.fbs      # InputSequence for input injection
├── event_test_data.fbs      # EventSequence for event injection
└── event_bus_data.fbs       # EventBusData for state comparison
```

### Schema Hierarchy

```
TestDataConfig (test_data.fbs)
├── metadata: TestMetadata (required)
├── start_data_collection: DataCollection
│   ├── entity_collection: EntityCollection ──────► entities.fbs
│   ├── event_bus: EventBusData ──────────────────► event_bus_data.fbs
│   └── waiting_room: EventBusData
├── expected_data_collection: DataCollection
├── game_resources: GameResourcesData ────────────► resource_data.fbs
├── scene_resources: SceneResourcesData
├── simulation_data: SimulationData ──────────────► simulation.fbs
│   └── steps: [SimulationStep]
│       ├── execution_mode: Function | LogicClass
│       ├── function_type: FunctionType
│       └── logic_class_type: LogicClassType
├── input_sequence: InputSequence ────────────────► input_test_data.fbs
├── event_sequence: EventSequence ────────────────► event_test_data.fbs
├── num_ticks: uint32
└── tick_snapshots: [TickSnapshot]
```

### Analysis: Schema Design for Unified Architecture

#### ✅ What Works Well

1. **Shared Resource Schemas**: `resource_data.fbs` defines `GameResourcesData` and `SceneResourcesData` used by both production (`GameEngine`, `Scene`) and test (`TestFixture`):
   ```fbs
   table GameResourcesData {
     window_width: uint32 = 800;
     window_height: uint32 = 600;
     window_title: string;
     framerate_limit: uint32 = 60;
   }
   ```

2. **Shared Entity Schemas**: `entities.fbs` is used by both `SceneData` (production) and `TestDataConfig` (test):
   ```fbs
   table EntityCollection {
     entities: [EntityData] (required);
     entity_memory_pool_size: int;
   }
   ```

3. **Simulation Schema**: `simulation.fbs` already maps to the execution levels:
   - `FunctionType` enum → Level 1 (free function execution)
   - `LogicClassType` enum → Level 1 (Logic class execution)
   - `SimulationStep` → Supports both modes

4. **DataCollection Pattern**: Groups related state for comparison:
   ```fbs
   table DataCollection {
     entity_collection: EntityCollection;
     event_bus: EventBusData;
     waiting_room: EventBusData;
   }
   ```

#### ⚠️ Potential Improvements

1. **Logic Execution Schema Gap**

   **Current**: `simulation.fbs` hardcodes specific Logic classes in `LogicClassType`:
   ```fbs
   enum LogicClassType : byte {
     None = 0,
     UIActionLogic = 1,
     UICollisionLogic = 2,
     UIRenderLogic = 3,
     // ... must add each new Logic class manually
   }
   ```

   **Issue**: Adding new Logic classes requires schema changes.

   **Recommendation**: Keep as-is. The explicit enum provides type safety and documentation. When new Logic classes are added, update the enum. This is preferable to a string-based approach that loses compile-time validation.

2. **Execution Level Schema**

   **Current**: No explicit schema for execution level configuration.

   **Proposed Addition** (`execution_config.fbs`):
   ```fbs
   namespace steamrot;
   
   enum ExecutionLevel : byte {
     Logic = 1,       // Level 1: Single logic/function
     Scene = 2,       // Level 2: Full scene tick
     SceneManager = 3,// Level 3: SceneManager update
     GameLoop = 4     // Level 4: Full game loop
   }
   
   table ExecutionConfig {
     level: ExecutionLevel = Logic;
     num_iterations: uint32 = 1;
     headless: bool = true;
   }
   ```

   **Benefit**: Test data can specify execution level explicitly, making test intent clearer.

3. **Scene-Level Configuration**

   **Current**: `SceneData` (production) and `TestDataConfig` (test) have different structures.

   **Observation**: They share `EntityCollection` and resource data but not logic configuration.

   **Recommendation**: No change needed. The separation is intentional:
   - Production (`SceneData`): Uses `LogicCollectionData` with full scene logic
   - Test (`TestDataConfig`): Uses `SimulationData` for selective execution

4. **Context Configuration Consolidation**

   **Current**: Two similar schemas exist:
   - `resource_data.fbs`: `GameResourcesData`, `SceneResourcesData`
   - `context_data.fbs`: `GameContextConfig`, `SceneContextConfig`

   **Observation**: These overlap but serve different purposes:
   - `resource_data.fbs`: Runtime resource configuration
   - `context_data.fbs`: Context builder configuration

   **Recommendation**: Keep separate. The redundancy enables different configuration paths (direct resource config vs. context builder).

### Schema Changes for Proposed Architecture

#### Required Changes: None

The current schema structure supports the proposed architecture without modification:

| Execution Level | Schema Support |
|-----------------|----------------|
| Level 1: Logic | `simulation.fbs` (FunctionType, LogicClassType) |
| Level 2: Scene | `entities.fbs` + `simulation.fbs` |
| Level 3: SceneManager | `event_test_data.fbs` provides `EventSequence` for injecting scene change events (e.g., `EVENT_CHANGE_SCENE`) that trigger SceneManager's `ProcessSubscriptions()` |
| Level 4: GameLoop | `test_data.fbs` already supports full configuration |

#### Optional Enhancements

1. **Add `execution_config.fbs`** for explicit execution level specification:
   ```fbs
   table ExecutionConfig {
     level: ExecutionLevel = Logic;
     num_iterations: uint32 = 1;
     headless: bool = true;
   }
   ```
   Would be added to `TestDataConfig` as optional field.

2. **Add scene type to SimulationData** for Scene-level testing:
   ```fbs
   table SimulationData {
     steps: [SimulationStep];
     description: string;
     scene_type: SceneType;  // NEW: Which scene to test
   }
   ```

3. **Add logic workflow schema** for custom logic sequences:
   ```fbs
   table LogicWorkflow {
     name: string;
     steps: [SimulationStep];
     description: string;
   }
   
   table TestDataConfig {
     // ... existing fields ...
     custom_workflows: [LogicWorkflow];  // NEW: Named workflows
   }
   ```

### Schema Design Principles

The current schema design follows good practices:

1. **Separation of Concerns**: Test-specific schemas (`test_data.fbs`, `simulation.fbs`) are separate from production schemas (`game_engine.fbs`, `scenes.fbs`).

2. **Composition**: Complex types are built from simpler ones (e.g., `DataCollection` composes `EntityCollection` and `EventBusData`).

3. **Extensibility**: Optional fields allow adding new capabilities without breaking existing data files.

4. **Type Safety**: Enums (`FunctionType`, `LogicClassType`, `SceneType`) provide compile-time validation.

### Conclusion: Schema Compatibility

**The current FlatBuffers schema structure is compatible with the proposed unified architecture.** No breaking changes are required. Optional enhancements (execution level config, scene type in simulation) can be added incrementally without affecting existing test data files.

The key insight is that the schemas already follow a pattern of:
- **Shared core types** (entities, resources) used by both production and test
- **Specialized types** (SimulationData, TestDataConfig) for test-specific needs
- **Enum-based dispatch** (FunctionType, LogicClassType) matching the execution model

## Proposed Architecture

### Core Principle: Composition Over Inheritance

Instead of having separate implementations, extract the core execution logic into reusable free functions that both the test harness and game engine can use.

### Execution Levels

The system should support running at different levels:

```
Level 1: Logic Execution
   └── Execute single Logic class or free function
       └── Uses: SceneContext

Level 2: Scene Tick
   └── Execute all logics for a scene (sAction, sCollision, sRender, sMovement)
       └── Uses: Scene + SceneContext

Level 3: Scene Manager Update
   └── Process subscriptions + Update all scenes
       └── Uses: SceneManager + GameContext

Level 4: Full Game Loop
   └── UpdateSystems + RunGameLoop
       └── Uses: GameEngine (full)
```

### Reusable Components

#### 1. Resource Configuration (Already Exists)

**Location**: `src/resources/resources_configuration.h`

```cpp
namespace steamrot::resources {
  // Configure GameResources from FlatBuffers
  std::expected<std::monostate, FailInfo>
  ConfigureGameResources(GameResources &resources, const GameResourcesData *game_data);

  // Configure SceneResources from FlatBuffers  
  std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(SceneResources &resources, const SceneResourcesData *scene_data);
}
```

**Status**: ✅ Already reusable - used by both TestFixture and GameEngine.

#### 2. Entity Configuration (Already Exists)

**Location**: `src/entity/FlatbuffersConfigurator.h`

```cpp
// Configure entities from EntityCollection (data-driven)
std::expected<std::monostate, FailInfo>
ConfigureEntitiesFromCollection(EntityMemoryPool &pool, const EntityCollection *collection);

// Configure entities from default scene data
std::expected<std::monostate, FailInfo>
ConfigureEntitiesFromDefaultData(EntityMemoryPool &pool, const SceneType &scene_type);
```

**Status**: ✅ Already reusable.

#### 3. Logic Execution (Proposed Extraction)

**Current Location**: Split between `simulation_runner.cpp` and `Scene` derived classes

**Proposed**: Extract scene system calls into free functions

```cpp
// src/logic/logic_execution.h
namespace steamrot::logic::execution {

  /////////////////////////////////////////////////
  /// @brief Execute all collision logics for a scene
  ///
  /// @param logic_map The scene's logic collection
  /// @param scene_context Context for logic execution
  /////////////////////////////////////////////////
  void ExecuteCollisionLogics(const LogicCollection &logic_map, 
                              SceneContext &scene_context);

  /////////////////////////////////////////////////
  /// @brief Execute all action logics for a scene
  /////////////////////////////////////////////////
  void ExecuteActionLogics(const LogicCollection &logic_map,
                           SceneContext &scene_context);

  /////////////////////////////////////////////////
  /// @brief Execute all render logics for a scene
  /////////////////////////////////////////////////
  void ExecuteRenderLogics(const LogicCollection &logic_map,
                           SceneContext &scene_context);

  /////////////////////////////////////////////////
  /// @brief Execute all movement logics for a scene
  /////////////////////////////////////////////////
  void ExecuteMovementLogics(const LogicCollection &logic_map,
                             SceneContext &scene_context);

  /////////////////////////////////////////////////
  /// @brief Execute a complete scene tick (all systems in order)
  ///
  /// Order: Action -> Movement -> Collision -> Render
  /////////////////////////////////////////////////
  void ExecuteSceneTick(const LogicCollection &logic_map,
                        SceneContext &scene_context);
}
```

#### 4. Event Processing (Proposed Extraction)

**Current Location**: `GameEngine::UpdateSystems()` and `tick_executor.cpp`

**Proposed**: Extract event processing into free functions

```cpp
// src/events/event_processing.h
namespace steamrot::events::processing {

  /////////////////////////////////////////////////
  /// @brief Process a single tick of event handling
  ///
  /// Performs: PreloadEvents -> ProcessWaitingRoom -> UpdateSubscribers
  /////////////////////////////////////////////////
  void ProcessEventTick(EventHandler &event_handler, 
                        sf::RenderWindow *window = nullptr);

  /////////////////////////////////////////////////
  /// @brief Complete an event tick (after logic execution)
  ///
  /// Performs: TickGlobalEventBus (decrement lifetimes, remove expired)
  /////////////////////////////////////////////////
  void CompleteEventTick(EventHandler &event_handler);
}
```

#### 5. Game Loop Core (Proposed Extraction)

**Current Location**: `GameEngine::UpdateSystems()` and `GameEngine::RunGameLoop()`

**Proposed**: Extract the core loop logic

```cpp
// src/systems/game_loop.h
namespace steamrot::game_loop {

  /////////////////////////////////////////////////
  /// @brief Execute a single game loop iteration
  ///
  /// @param game_resources Reference to game resources
  /// @param scene_manager Reference to scene manager
  /// @param display_manager Reference to display manager (optional for headless)
  /////////////////////////////////////////////////
  void ExecuteSingleIteration(GameResources &game_resources,
                              SceneManager &scene_manager,
                              DisplayManager *display_manager = nullptr);

  /////////////////////////////////////////////////
  /// @brief Update game resources each tick
  ///
  /// Currently: Updates mouse position
  /////////////////////////////////////////////////
  void UpdateGameResources(GameResources &game_resources);
}
```

### Proposed Test Harness Structure

The test harness should mirror the execution levels:

```
tests/harness/
├── core/
│   ├── TestFixture.h              # Resource ownership (existing)
│   ├── TestFixture.cpp
│   └── execution_levels.h         # NEW: Level definitions
│
├── execution/
│   ├── logic_executor.h           # Level 1: Execute Logic
│   ├── logic_executor.cpp
│   ├── scene_executor.h           # Level 2: Execute Scene tick
│   ├── scene_executor.cpp
│   ├── scene_manager_executor.h   # Level 3: Execute SceneManager update
│   ├── scene_manager_executor.cpp
│   ├── game_loop_executor.h       # Level 4: Execute full game loop
│   └── game_loop_executor.cpp
│
├── simulation/
│   ├── simulation_runner.h        # Existing: Step-by-step execution
│   ├── simulation_runner.cpp
│   ├── tick_executor.h            # Existing: Tick-based execution
│   ├── tick_executor.cpp
│   ├── input_simulation.h         # Existing: Input injection
│   ├── input_simulation.cpp
│   ├── event_simulation.h         # Existing: Event injection
│   └── event_simulation.cpp
│
├── data/
│   ├── test_data_harness.h        # Existing: Data loading
│   └── test_data_harness.cpp
│
└── output/
    └── console_output.h           # Existing: Formatted output
```

### Execution Level APIs

#### Level 1: Logic Execution

```cpp
// tests/harness/execution/logic_executor.h
namespace steamrot::tests::execution {

  /////////////////////////////////////////////////
  /// @brief Execute a single Logic class instance
  ///
  /// @param logic Logic class to execute
  /// @param scene_context Context for execution
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ExecuteLogic(Logic &logic, SceneContext &scene_context);

  /////////////////////////////////////////////////
  /// @brief Execute a free function by type
  ///
  /// @param function_type Function to execute
  /// @param scene_context Context for execution
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ExecuteFunction(FunctionType function_type, SceneContext &scene_context);

  /////////////////////////////////////////////////
  /// @brief Execute a custom logic workflow
  ///
  /// @param steps Vector of simulation steps
  /// @param scene_context Context for execution
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ExecuteCustomWorkflow(const std::vector<SimulationStep*> &steps,
                        SceneContext &scene_context);
}
```

#### Level 2: Scene Execution

```cpp
// tests/harness/execution/scene_executor.h
namespace steamrot::tests::execution {

  /////////////////////////////////////////////////
  /// @brief Execute a complete scene tick using existing Scene
  ///
  /// @param scene Scene to execute tick for
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ExecuteSceneTick(Scene &scene);

  /////////////////////////////////////////////////
  /// @brief Execute a scene tick with custom logic map
  ///
  /// @param logic_map Custom logic collection to use
  /// @param scene_context Scene context
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ExecuteSceneTickWithCustomLogic(const LogicCollection &logic_map,
                                   SceneContext &scene_context);

  /////////////////////////////////////////////////
  /// @brief Execute scene tick with injected data
  ///
  /// Allows injection of entities and events before execution
  /////////////////////////////////////////////////
  struct SceneTickConfig {
    const EntityCollection *entity_data = nullptr;
    const EventBusData *event_data = nullptr;
    const SimulationData *simulation = nullptr;
  };

  std::expected<std::monostate, FailInfo>
  ExecuteConfiguredSceneTick(Scene &scene, const SceneTickConfig &config);
}
```

#### Level 3: SceneManager Execution

```cpp
// tests/harness/execution/scene_manager_executor.h
namespace steamrot::tests::execution {

  /////////////////////////////////////////////////
  /// @brief Execute a SceneManager update cycle
  ///
  /// @param scene_manager SceneManager to update
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ExecuteSceneManagerUpdate(SceneManager &scene_manager);

  /////////////////////////////////////////////////
  /// @brief Execute SceneManager with injected scene configuration
  /////////////////////////////////////////////////
  struct SceneManagerConfig {
    std::vector<SceneType> scenes_to_load;
    const EventSequence *event_sequence = nullptr;
    const InputSequence *input_sequence = nullptr;
  };

  std::expected<std::monostate, FailInfo>
  ExecuteConfiguredSceneManager(SceneManager &scene_manager,
                                const SceneManagerConfig &config,
                                GameResources &resources);
}
```

#### Level 4: Game Loop Execution

```cpp
// tests/harness/execution/game_loop_executor.h
namespace steamrot::tests::execution {

  /////////////////////////////////////////////////
  /// @brief Configuration for game loop execution
  /////////////////////////////////////////////////
  struct GameLoopConfig {
    size_t num_iterations = 1;
    bool headless = true;              // No window/display
    const EventSequence *events = nullptr;
    const InputSequence *inputs = nullptr;
  };

  /////////////////////////////////////////////////
  /// @brief Execute game loop iterations with configuration
  ///
  /// @param engine GameEngine to run
  /// @param config Configuration for execution
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ExecuteGameLoop(GameEngine &engine, const GameLoopConfig &config);

  /////////////////////////////////////////////////
  /// @brief Execute game loop with fixture (for testing)
  ///
  /// Uses TestFixture resources instead of full GameEngine
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ExecuteGameLoopWithFixture(TestFixture &fixture, 
                             const GameLoopConfig &config);
}
```

### Data Injection Points

The test harness should support injection at multiple points:

#### 1. Entity Data Injection (Existing)

```cpp
// Already supported via TestFixture::Initialize()
fixture.Initialize(entity_collection);
```

#### 2. Event Injection (Existing)

```cpp
// Already supported via event_simulation.h
ExecuteEventSequence(event_sequence, fixture);
```

#### 3. Input Injection (Existing)

```cpp
// Already supported via input_simulation.h
ExecuteInputSequence(input_sequence, fixture);
```

#### 4. Logic Workflow Injection (Proposed Enhancement)

```cpp
// Create custom logic workflow at runtime
std::vector<std::unique_ptr<Logic>> custom_logics;
custom_logics.push_back(std::make_unique<UICollisionLogic>(scene_context));
custom_logics.push_back(std::make_unique<CustomTestLogic>(scene_context));

// Execute custom workflow
for (auto &logic : custom_logics) {
  logic->RunLogic();
}
```

### Game Engine: Compile-Time Configuration

For the production game engine, eliminate runtime checks by using compile-time configuration:

```cpp
// src/systems/game_engine_config.h
namespace steamrot::config {

  /////////////////////////////////////////////////
  /// @brief Compile-time game engine configuration
  ///
  /// These settings are fixed at compile time for production builds.
  /// Test builds may override these via preprocessor definitions.
  /////////////////////////////////////////////////
  
#ifdef STEAMROT_TEST_MODE
  // Test configuration: enables all injection capabilities
  struct GameEngineConfig {
    static constexpr bool ENABLE_RUNTIME_LOGIC_INJECTION = true;
    static constexpr bool ENABLE_DATA_INJECTION = true;
    static constexpr bool ENABLE_HEADLESS_MODE = true;
    static constexpr EnvironmentType ENV_TYPE = EnvironmentType::Test;
  };
#else
  // Production configuration: all injection disabled
  struct GameEngineConfig {
    static constexpr bool ENABLE_RUNTIME_LOGIC_INJECTION = false;
    static constexpr bool ENABLE_DATA_INJECTION = false;
    static constexpr bool ENABLE_HEADLESS_MODE = false;
    static constexpr EnvironmentType ENV_TYPE = EnvironmentType::Production;
  };
#endif

}
```

### Benefits of This Architecture

#### 1. Code Reuse

- **Resource configuration**: Single source of truth
- **Logic execution**: Same functions used by game and tests
- **Event processing**: Consistent behavior

#### 2. Testing at Multiple Levels

```cpp
// Level 1: Test a single Logic class
TEST_CASE("UICollisionLogic detects mouse over", "[Logic]") {
  TestFixture fixture;
  fixture.Initialize(entity_collection);
  
  // Create and execute the logic
  UICollisionLogic logic(fixture.GetSceneContext());
  auto result = execution::ExecuteLogic(logic, fixture.GetSceneContext());
  REQUIRE(result.has_value());
}

// Level 2: Test a complete scene tick
TEST_CASE("Scene tick processes all systems", "[Scene]") {
  TestFixture fixture;
  fixture.Initialize();
  
  auto result = execution::ExecuteSceneTick(scene);
  REQUIRE(result.has_value());
}

// Level 3: Test scene manager updates
TEST_CASE("SceneManager handles scene changes", "[SceneManager]") {
  // Test configuration can inject events that trigger scene changes
  SceneManagerConfig config;
  config.event_sequence = scene_change_events;
  
  auto result = execution::ExecuteConfiguredSceneManager(scene_manager, config, resources);
  REQUIRE(result.has_value());
}

// Level 4: Integration test with game loop
TEST_CASE("Game loop runs correctly", "[GameLoop]") {
  GameLoopConfig config;
  config.num_iterations = 10;
  config.headless = true;
  
  auto result = execution::ExecuteGameLoop(engine, config);
  REQUIRE(result.has_value());
}
```

#### 3. Minimal Runtime Overhead in Production

- Compile-time configuration eliminates runtime checks
- No injection points in production builds
- Same core code, different configurations

#### 4. Clear Separation of Concerns

- **Test Harness**: Owns injection capabilities
- **Game Engine**: Uses same core logic, locked configuration
- **Shared Core**: Free functions for execution

### Implementation Phases

#### Phase 1: Extract Core Execution Functions

1. Create `src/logic/logic_execution.h/cpp`
2. Create `src/events/event_processing.h/cpp`
3. Create `src/systems/game_loop.h/cpp`
4. Refactor `GameEngine` to use these functions
5. Refactor `Scene` to use these functions

#### Phase 2: Restructure Test Harness

1. Create `tests/harness/execution/` directory
2. Implement level-based executors
3. Refactor existing `simulation_runner.cpp` to use new structure
4. Update `tick_executor.cpp` to use new execution functions

#### Phase 3: Add Compile-Time Configuration

1. Create `src/systems/game_engine_config.h`
2. Add `STEAMROT_TEST_MODE` preprocessor definition for tests
3. Conditionally compile injection points

#### Phase 4: Documentation and Migration

1. Update test documentation
2. Create migration guide for existing tests
3. Add examples for each execution level

### File Changes Summary

#### New Files to Create

| File | Purpose |
|------|---------|
| `src/logic/logic_execution.h` | Free functions for logic execution |
| `src/logic/logic_execution.cpp` | Implementation |
| `src/events/event_processing.h` | Free functions for event processing |
| `src/events/event_processing.cpp` | Implementation |
| `src/systems/game_loop.h` | Core game loop functions |
| `src/systems/game_loop.cpp` | Implementation |
| `src/systems/game_engine_config.h` | Compile-time configuration |
| `tests/harness/execution/logic_executor.h` | Level 1 executor |
| `tests/harness/execution/scene_executor.h` | Level 2 executor |
| `tests/harness/execution/scene_manager_executor.h` | Level 3 executor |
| `tests/harness/execution/game_loop_executor.h` | Level 4 executor |

#### Files to Modify

| File | Changes |
|------|---------|
| `src/systems/GameEngine.cpp` | Use extracted functions |
| `src/scenes/Scene.cpp` | Use logic_execution functions |
| `tests/harness/simulation_runner.cpp` | Use logic_executor |
| `tests/harness/tick_executor.cpp` | Use execution functions |

### Conclusion

This architecture enables:

1. **The Test Harness** to be a flexible application of the game engine core that can:
   - Run at any execution level (Logic, Scene, SceneManager, GameEngine)
   - Inject custom logic workflows via free functions
   - Inject data (entities, events, inputs) at runtime

2. **The Game Engine** to use the same core code but with:
   - Fixed configuration at compile time
   - No injection points in production
   - No runtime guards needed

The key insight is that both share the same execution primitives (free functions), but the game engine's configuration is locked while the test harness retains flexibility.

---

## Staged Implementation Plan

This section provides a detailed, step-by-step implementation plan organized into stages. Each stage builds upon the previous one and can be completed independently with working code at each checkpoint.

### Overview

| Stage | Focus | Duration Estimate | Dependencies |
|-------|-------|-------------------|--------------|
| 1 | Core Extraction | 2-3 days | None |
| 2 | Test Harness Restructure | 2-3 days | Stage 1 |
| 3 | Execution Level Framework | 2-3 days | Stage 2 |
| 4 | SceneManager & GameLoop Levels | 2-3 days | Stage 3 |
| 5 | Compile-Time Configuration | 1-2 days | Stage 4 |
| 6 | Documentation & Migration | 1-2 days | Stage 5 |

**Total Estimated Duration**: 10-16 days

---

### Stage 1: Core Extraction

**Goal**: Extract reusable execution logic from `GameEngine` and `Scene` into free functions.

#### Step 1.1: Create Logic Execution Functions

**Files to Create**:
- `src/logic/logic_execution.h`
- `src/logic/logic_execution.cpp`

**Implementation**:

```cpp
// src/logic/logic_execution.h
#pragma once

#include "Logic.h"
#include "SceneContext.h"
#include <unordered_map>
#include <vector>
#include <memory>

namespace steamrot::logic::execution {

  /////////////////////////////////////////////////
  /// @brief Execute all logics in a LogicVector
  /////////////////////////////////////////////////
  void ExecuteLogicVector(const std::vector<std::unique_ptr<Logic>> &logics);

  /////////////////////////////////////////////////
  /// @brief Execute all logics of a specific type from a LogicCollection
  /////////////////////////////////////////////////
  void ExecuteLogicsByType(const LogicCollection &logic_map, 
                           LogicType type);

  /////////////////////////////////////////////////
  /// @brief Execute a complete scene tick (all systems in order)
  ///
  /// Order: Action -> Movement -> Collision -> Render
  /////////////////////////////////////////////////
  void ExecuteSceneTick(const LogicCollection &logic_map);

} // namespace steamrot::logic::execution
```

**Tasks**:
1. [ ] Create header file with function declarations
2. [ ] Implement `ExecuteLogicVector` - iterates and calls `RunLogic()`
3. [ ] Implement `ExecuteLogicsByType` - finds logics by type and executes
4. [ ] Implement `ExecuteSceneTick` - calls system methods in order
5. [ ] Add to `src/logic/CMakeLists.txt`
6. [ ] Write unit tests in `tests/unit/logic/logic_execution.test.cpp`

**Verification**:
- [ ] All existing tests pass
- [ ] New unit tests pass
- [ ] No changes to existing behavior

#### Step 1.2: Create Event Processing Functions

**Files to Create**:
- `src/events/event_processing.h`
- `src/events/event_processing.cpp`

**Implementation**:

```cpp
// src/events/event_processing.h
#pragma once

#include "EventHandler.h"
#include <SFML/Graphics/RenderWindow.hpp>

namespace steamrot::events::processing {

  /////////////////////////////////////////////////
  /// @brief Process events at the start of a tick
  ///
  /// Performs: PreloadEvents -> ProcessWaitingRoom -> UpdateSubscribers
  /////////////////////////////////////////////////
  void ProcessEventTickStart(EventHandler &event_handler, 
                              sf::RenderWindow *window = nullptr);

  /////////////////////////////////////////////////
  /// @brief Complete an event tick (after logic execution)
  ///
  /// Performs: TickGlobalEventBus (decrement lifetimes, remove expired)
  /////////////////////////////////////////////////
  void ProcessEventTickEnd(EventHandler &event_handler);

  /////////////////////////////////////////////////
  /// @brief Execute a full event tick (start + end)
  /////////////////////////////////////////////////
  void ProcessFullEventTick(EventHandler &event_handler,
                            sf::RenderWindow *window = nullptr);

} // namespace steamrot::events::processing
```

**Tasks**:
1. [ ] Create header file with function declarations
2. [ ] Extract event processing logic from `GameEngine::UpdateSystems()`
3. [ ] Implement free functions
4. [ ] Add to `src/events/CMakeLists.txt`
5. [ ] Write unit tests

**Verification**:
- [ ] Event processing works identically to before
- [ ] Test harness event simulation still works

#### Step 1.3: Create Game Loop Core Functions

**Files to Create**:
- `src/systems/game_loop.h`
- `src/systems/game_loop.cpp`

**Implementation**:

```cpp
// src/systems/game_loop.h
#pragma once

#include "GameResources.h"
#include "SceneManager.h"
#include "DisplayManager.h"

namespace steamrot::game_loop {

  /////////////////////////////////////////////////
  /// @brief Update game resources each tick
  /////////////////////////////////////////////////
  void UpdateGameResources(GameResources &game_resources);

  /////////////////////////////////////////////////
  /// @brief Execute a single game loop iteration (no display)
  /////////////////////////////////////////////////
  void ExecuteHeadlessIteration(GameResources &game_resources,
                                 SceneManager &scene_manager);

  /////////////////////////////////////////////////
  /// @brief Execute a single game loop iteration (with display)
  /////////////////////////////////////////////////
  void ExecuteFullIteration(GameResources &game_resources,
                            SceneManager &scene_manager,
                            DisplayManager &display_manager);

} // namespace steamrot::game_loop
```

**Tasks**:
1. [ ] Create header file with function declarations
2. [ ] Extract loop iteration logic from `GameEngine::RunGameLoop()`
3. [ ] Implement free functions
4. [ ] Add to `src/systems/CMakeLists.txt`
5. [ ] Write unit tests

**Verification**:
- [ ] Game runs identically to before
- [ ] Headless iteration works for testing

#### Step 1.4: Refactor GameEngine to Use Extracted Functions

**Files to Modify**:
- `src/systems/GameEngine.cpp`

**Tasks**:
1. [ ] Replace inline event processing with `events::processing::ProcessEventTickStart()`
2. [ ] Replace inline loop logic with `game_loop::ExecuteFullIteration()`
3. [ ] Replace `UpdateGameResources` implementation with call to `game_loop::UpdateGameResources()`
4. [ ] Run all tests to verify no regression

**Verification**:
- [ ] Game behavior unchanged
- [ ] All existing tests pass

#### Step 1.5: Refactor Scene to Use Extracted Functions

**Files to Modify**:
- `src/scenes/Scene.cpp`
- `src/scenes/TitleScene.cpp`
- `src/scenes/CraftingScene.cpp`

**Tasks**:
1. [ ] Replace inline logic execution with `logic::execution::ExecuteLogicsByType()`
2. [ ] Ensure derived scenes still work correctly
3. [ ] Run scene-related tests

**Verification**:
- [ ] Scene updates work identically
- [ ] All scene tests pass

---

### Stage 2: Test Harness Restructure

**Goal**: Reorganize test harness to use the extracted functions and prepare for execution levels.

#### Step 2.1: Update simulation_runner.cpp

**Files to Modify**:
- `tests/harness/simulation_runner.cpp`

**Tasks**:
1. [ ] Import new `logic::execution` functions
2. [ ] Refactor `ExecuteLogicClass()` to use `logic::execution` where applicable
3. [ ] Keep `ExecuteFunction()` for free function dispatch (test-specific)
4. [ ] Update tests

**Verification**:
- [ ] All simulation tests pass
- [ ] Data-driven tests still work

#### Step 2.2: Update tick_executor.cpp

**Files to Modify**:
- `tests/harness/tick_executor.cpp`

**Tasks**:
1. [ ] Import `events::processing` functions
2. [ ] Replace inline event processing with extracted functions
3. [ ] Ensure tick execution matches game engine behavior exactly
4. [ ] Update tests

**Verification**:
- [ ] All tick-based tests pass
- [ ] Event lifetimes work correctly

#### Step 2.3: Create Execution Directory Structure

**Files to Create**:
- `tests/harness/execution/` directory
- `tests/harness/execution/CMakeLists.txt`

**Tasks**:
1. [ ] Create directory structure
2. [ ] Add CMakeLists.txt with library definition
3. [ ] Update parent `tests/harness/CMakeLists.txt` to include subdirectory

**Verification**:
- [ ] Project builds correctly

---

### Stage 3: Execution Level Framework

**Goal**: Implement Level 1 (Logic) and Level 2 (Scene) executors.

#### Step 3.1: Implement Level 1 - Logic Executor

**Files to Create**:
- `tests/harness/execution/logic_executor.h`
- `tests/harness/execution/logic_executor.cpp`

**Implementation**:

```cpp
// tests/harness/execution/logic_executor.h
#pragma once

#include "FailInfo.h"
#include "Logic.h"
#include "SceneContext.h"
#include "TestFixture.h"
#include "simulation_generated.h"
#include <expected>

namespace steamrot::tests::execution {

  /////////////////////////////////////////////////
  /// @brief Execute a single Logic class instance
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ExecuteLogic(Logic &logic);

  /////////////////////////////////////////////////
  /// @brief Execute a Logic class by type enum
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ExecuteLogicByType(LogicClassType type, SceneContext &scene_context);

  /////////////////////////////////////////////////
  /// @brief Execute a free function by type enum
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ExecuteFunction(FunctionType type, SceneContext &scene_context);

  /////////////////////////////////////////////////
  /// @brief Execute a custom workflow (sequence of steps)
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ExecuteWorkflow(const SimulationData *simulation_data,
                  SceneContext &scene_context);

} // namespace steamrot::tests::execution
```

**Tasks**:
1. [ ] Create header with declarations
2. [ ] Implement `ExecuteLogic` - wraps `logic.RunLogic()`
3. [ ] Implement `ExecuteLogicByType` - creates Logic instance and executes
4. [ ] Implement `ExecuteFunction` - dispatches to free functions
5. [ ] Implement `ExecuteWorkflow` - iterates SimulationData steps
6. [ ] Write comprehensive unit tests
7. [ ] Add to CMakeLists.txt

**Verification**:
- [ ] Can execute individual Logic classes
- [ ] Can execute free functions
- [ ] Can execute custom workflows
- [ ] All tests pass

#### Step 3.2: Implement Level 2 - Scene Executor

**Files to Create**:
- `tests/harness/execution/scene_executor.h`
- `tests/harness/execution/scene_executor.cpp`

**Implementation**:

```cpp
// tests/harness/execution/scene_executor.h
#pragma once

#include "FailInfo.h"
#include "Scene.h"
#include "TestFixture.h"
#include "entities_generated.h"
#include "simulation_generated.h"
#include <expected>

namespace steamrot::tests::execution {

  /////////////////////////////////////////////////
  /// @brief Execute a single scene tick
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ExecuteSceneTick(Scene &scene);

  /////////////////////////////////////////////////
  /// @brief Execute a scene tick using TestFixture
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ExecuteSceneTickWithFixture(TestFixture &fixture);

  /////////////////////////////////////////////////
  /// @brief Configuration for scene tick execution
  /////////////////////////////////////////////////
  struct SceneTickConfig {
    const EntityCollection *start_entities = nullptr;
    const SimulationData *simulation = nullptr;
    uint32_t num_ticks = 1;
  };

  /////////////////////////////////////////////////
  /// @brief Execute configured scene ticks
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ExecuteConfiguredSceneTicks(TestFixture &fixture,
                               const SceneTickConfig &config);

} // namespace steamrot::tests::execution
```

**Tasks**:
1. [ ] Create header with declarations
2. [ ] Implement `ExecuteSceneTick` - calls scene systems in order
3. [ ] Implement `ExecuteSceneTickWithFixture` - uses fixture's scene context
4. [ ] Implement `ExecuteConfiguredSceneTicks` - handles entity injection and multi-tick
5. [ ] Write unit tests
6. [ ] Add to CMakeLists.txt

**Verification**:
- [ ] Can execute scene ticks
- [ ] Multi-tick execution works
- [ ] Entity injection works
- [ ] All tests pass

#### Step 3.3: Update Test Harness to Use Executors

**Files to Modify**:
- `tests/harness/simulation_runner.cpp`
- `tests/harness/tick_executor.cpp`

**Tasks**:
1. [ ] Refactor `simulation_runner.cpp` to use `logic_executor`
2. [ ] Refactor `tick_executor.cpp` to use `scene_executor` where applicable
3. [ ] Ensure backward compatibility with existing test data
4. [ ] Run all harness tests

**Verification**:
- [ ] All existing tests pass
- [ ] New executors are used internally

---

### Stage 4: SceneManager & GameLoop Levels

**Goal**: Implement Level 3 (SceneManager) and Level 4 (GameLoop) executors.

#### Step 4.1: Implement Level 3 - SceneManager Executor

**Files to Create**:
- `tests/harness/execution/scene_manager_executor.h`
- `tests/harness/execution/scene_manager_executor.cpp`

**Implementation**:

```cpp
// tests/harness/execution/scene_manager_executor.h
#pragma once

#include "FailInfo.h"
#include "GameResources.h"
#include "SceneManager.h"
#include "event_test_data_generated.h"
#include "input_test_data_generated.h"
#include <expected>

namespace steamrot::tests::execution {

  /////////////////////////////////////////////////
  /// @brief Execute a SceneManager update cycle
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ExecuteSceneManagerUpdate(SceneManager &scene_manager);

  /////////////////////////////////////////////////
  /// @brief Configuration for SceneManager execution
  /////////////////////////////////////////////////
  struct SceneManagerConfig {
    SceneType initial_scene = SceneType_TITLE;
    const EventSequence *event_sequence = nullptr;
    const InputSequence *input_sequence = nullptr;
    uint32_t num_updates = 1;
  };

  /////////////////////////////////////////////////
  /// @brief Execute configured SceneManager updates
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ExecuteConfiguredSceneManager(SceneManager &scene_manager,
                                 GameResources &resources,
                                 const SceneManagerConfig &config);

} // namespace steamrot::tests::execution
```

**Tasks**:
1. [ ] Create header with declarations
2. [ ] Implement `ExecuteSceneManagerUpdate` - calls SceneManager's update cycle
3. [ ] Implement `ExecuteConfiguredSceneManager` - handles scene loading and event injection
4. [ ] Write unit tests for scene transitions
5. [ ] Add to CMakeLists.txt

**Verification**:
- [ ] Can execute SceneManager updates
- [ ] Scene transitions work
- [ ] Event injection triggers correct behavior

#### Step 4.2: Implement Level 4 - GameLoop Executor

**Files to Create**:
- `tests/harness/execution/game_loop_executor.h`
- `tests/harness/execution/game_loop_executor.cpp`

**Implementation**:

```cpp
// tests/harness/execution/game_loop_executor.h
#pragma once

#include "FailInfo.h"
#include "GameEngine.h"
#include "TestFixture.h"
#include <expected>

namespace steamrot::tests::execution {

  /////////////////////////////////////////////////
  /// @brief Configuration for game loop execution
  /////////////////////////////////////////////////
  struct GameLoopConfig {
    size_t num_iterations = 1;
    bool headless = true;
    const EventSequence *events = nullptr;
    const InputSequence *inputs = nullptr;
  };

  /////////////////////////////////////////////////
  /// @brief Execute game loop iterations using GameEngine
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ExecuteGameLoop(GameEngine &engine, const GameLoopConfig &config);

  /////////////////////////////////////////////////
  /// @brief Execute game loop iterations using TestFixture
  ///
  /// Creates a lightweight game loop without full GameEngine
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ExecuteGameLoopWithFixture(TestFixture &fixture,
                              const GameLoopConfig &config);

} // namespace steamrot::tests::execution
```

**Tasks**:
1. [ ] Create header with declarations
2. [ ] Implement `ExecuteGameLoop` - runs actual GameEngine iterations
3. [ ] Implement `ExecuteGameLoopWithFixture` - simulates game loop with fixture
4. [ ] Write integration tests
5. [ ] Add to CMakeLists.txt

**Verification**:
- [ ] Can run game loop iterations
- [ ] Headless mode works
- [ ] Event/input injection works at game loop level

#### Step 4.3: Create Unified Execution Entry Point

**Files to Create**:
- `tests/harness/execution/execution_runner.h`
- `tests/harness/execution/execution_runner.cpp`

**Implementation**:

```cpp
// tests/harness/execution/execution_runner.h
#pragma once

#include "FailInfo.h"
#include "TestFixture.h"
#include "test_data_generated.h"
#include <expected>

namespace steamrot::tests::execution {

  /////////////////////////////////////////////////
  /// @brief Execution level enum
  /////////////////////////////////////////////////
  enum class ExecutionLevel {
    Logic = 1,
    Scene = 2,
    SceneManager = 3,
    GameLoop = 4
  };

  /////////////////////////////////////////////////
  /// @brief Run test at specified execution level
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  RunTestAtLevel(const TestDataConfig *config,
                 ExecutionLevel level,
                 TestFixture &fixture);

  /////////////////////////////////////////////////
  /// @brief Auto-detect execution level from config
  /////////////////////////////////////////////////
  ExecutionLevel DetectExecutionLevel(const TestDataConfig *config);

} // namespace steamrot::tests::execution
```

**Tasks**:
1. [ ] Create header with declarations
2. [ ] Implement `RunTestAtLevel` - dispatches to appropriate executor
3. [ ] Implement `DetectExecutionLevel` - infers level from config
4. [ ] Write tests for level detection
5. [ ] Add to CMakeLists.txt

**Verification**:
- [ ] Can run tests at any level
- [ ] Level detection works correctly

---

### Stage 5: Compile-Time Configuration

**Goal**: Add compile-time configuration for production vs test builds.

#### Step 5.1: Create Game Engine Config Header

**Files to Create**:
- `src/systems/game_engine_config.h`

**Implementation**:

```cpp
// src/systems/game_engine_config.h
#pragma once

#include "PathProvider.h"

namespace steamrot::config {

#ifdef STEAMROT_TEST_MODE
  struct GameEngineConfig {
    static constexpr bool ENABLE_RUNTIME_LOGIC_INJECTION = true;
    static constexpr bool ENABLE_DATA_INJECTION = true;
    static constexpr bool ENABLE_HEADLESS_MODE = true;
    static constexpr EnvironmentType DEFAULT_ENV_TYPE = EnvironmentType::Test;
  };
#else
  struct GameEngineConfig {
    static constexpr bool ENABLE_RUNTIME_LOGIC_INJECTION = false;
    static constexpr bool ENABLE_DATA_INJECTION = false;
    static constexpr bool ENABLE_HEADLESS_MODE = false;
    static constexpr EnvironmentType DEFAULT_ENV_TYPE = EnvironmentType::Production;
  };
#endif

} // namespace steamrot::config
```

**Tasks**:
1. [ ] Create header file
2. [ ] Add documentation for each config option
3. [ ] No implementation file needed (header-only)

#### Step 5.2: Update CMake for Test Mode

**Files to Modify**:
- `tests/CMakeLists.txt`

**Tasks**:
1. [ ] Add `STEAMROT_TEST_MODE` definition for test targets
2. [ ] Ensure production builds don't have this definition
3. [ ] Verify builds work correctly

**Verification**:
- [ ] Test builds have `STEAMROT_TEST_MODE` defined
- [ ] Production builds do not
- [ ] Config struct has correct values in each case

#### Step 5.3: Use Config in GameEngine

**Files to Modify**:
- `src/systems/GameEngine.cpp`

**Tasks**:
1. [ ] Include `game_engine_config.h`
2. [ ] Use config values where appropriate
3. [ ] Add comments explaining compile-time behavior

**Verification**:
- [ ] GameEngine behavior unchanged
- [ ] Config is correctly applied

---

### Stage 6: Documentation & Migration ✅ COMPLETE

**Goal**: Document the new architecture and provide migration guidance.

#### Step 6.1: Update Test Harness README ✅

**Files Modified**:
- `tests/harness/README.md`

**Completed Tasks**:
1. [x] Document new execution level framework
2. [x] Add examples for each execution level
3. [x] Explain when to use each level
4. [x] Document directory structure changes

#### Step 6.2: Create Migration Guide ✅

**Files Created**:
- `documentation/workflows/TEST_HARNESS_MIGRATION.md`

**Content**:
1. [x] Overview of changes
2. [x] Step-by-step migration for existing tests
3. [x] Before/after code examples
4. [x] FAQ section (Common Migration Issues)

#### Step 6.3: Integration with test_data_harness ✅

**Files Modified**:
- `tests/harness/test_data_harness.h`
- `tests/harness/test_data_harness.cpp`

**Completed Tasks**:
1. [x] Added `RunFixtureTestAtLevel()` function for explicit level control
2. [x] Integrated execution_runner.h into test_data_harness
3. [x] Maintained backwards compatibility with existing `RunFixtureTest()`

#### Step 6.4: Documentation Complete

**Notes**:
- Example tests are documented in README.md with code snippets
- Migration guide provides comprehensive before/after examples
- COPILOT_INSTRUCTIONS.md already contains test harness documentation that references README

**Verification**:
- [x] README updated with execution level documentation
- [x] Migration guide created with examples
- [x] New function added for explicit level execution

---

### Post-Implementation Checklist

After completing all stages:

- [ ] All existing tests pass
- [ ] New tests pass
- [ ] Documentation is complete
- [ ] No regressions in game behavior
- [ ] Build times acceptable
- [ ] Test execution times acceptable
- [ ] Code review completed

---

### Risk Mitigation

| Risk | Mitigation |
|------|------------|
| Breaking existing tests | Run tests after each step; revert if needed |
| GameEngine behavior change | Careful extraction; extensive testing |
| Build complexity increase | Keep CMake changes minimal |
| Documentation gaps | Document as you go; review at end |

---

### Success Criteria

The implementation is successful when:

1. **Tests can run at any level**: Logic, Scene, SceneManager, or GameLoop
2. **Code reuse is achieved**: Same execution functions used by game and tests
3. **No runtime overhead in production**: Compile-time configuration works
4. **Existing tests unaffected**: All current tests continue to pass
5. **Clear documentation exists**: Developers can easily understand and use the framework
