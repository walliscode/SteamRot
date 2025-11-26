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
