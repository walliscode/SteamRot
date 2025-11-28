# Unified Tick Architecture Proposal

[← Back to Proposals](README.md) | [Testing Harness Loop](../testing/TESTING_HARNESS_LOOP.md) | [Game Loop](../architecture/GAME_LOOP.md)

**Status**: Proposal  
**Date**: 2025-11-27  
**Type**: Architecture Unification

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Problem Statement](#problem-statement)
3. [Current Architecture Analysis](#current-architecture-analysis)
4. [Proposed Solutions](#proposed-solutions)
5. [Implementation Plan](#implementation-plan)
6. [Benefits](#benefits)
7. [Risks](#risks)
8. [Migration Path](#migration-path)
9. [Future Considerations](#future-considerations)

---

## Executive Summary

This proposal addresses the architectural divergence between the SteamRot game engine's main loop and the test harness execution model. By creating a unified tick execution abstraction, we can:

1. Eliminate code duplication between game and test execution paths
2. Ensure consistent event ordering across both contexts
3. Allow flexible logic configuration injection
4. Unify data loading patterns
5. Simplify the testing infrastructure

The goal is to create a common foundation that both the game engine and test harness can use, while still allowing each to customize behavior as needed.

---

## Problem Statement

The following points have been identified as areas requiring improvement:

### Point 1: Event Ordering Divergence

**Problem**: The test harness and game engine have different event execution orders, leading to potential behavioral differences.

| Step | Game Loop (UpdateSystems) | Test Harness (ExecuteSingleTick) |
|------|---------------------------|----------------------------------|
| 1 | UpdateGameResources | ExecuteInputEventsForTick |
| 2 | PreloadEvents | ExecuteEventsForTick |
| 3 | ProcessWaitingRoom | ProcessWaitingRoom |
| 4 | UpdateSubscribers | UpdateSubscribers |
| 5 | ProcessSubscriptions | ExecuteSimulation |
| 6 | UpdateSceneManager | CompareTickSnapshot |
| 7 | CallRenderCycle | TickGlobalEventBus |
| 8 | TickGlobalEventBus | - |

### Point 2: Logic Configuration Injection

**Problem**: The game uses `SceneManager::UpdateScenes()` which calls the scene's built-in logic map, while the test harness uses `simulation_data` to manually specify which Logic classes/functions to execute. There's no way to inject custom logic configuration into the game loop.

### Point 3: Data Loading Pattern Divergence

**Problem**: The game loads entities from default scene data, while tests load from `TestDataConfig.start_data_collection`. The loading patterns are similar but not unified.

### Point 4: Redundant Expected Data

**Problem**: With tick-based snapshots (`tick_snapshots`), the separate `expected_data_collection` at the test level may be redundant.

### Point 5: TestContext Clunkiness

**Problem**: `TestContext` serves a different purpose than `GameContext`/`SceneContext`, but the naming similarity causes confusion. The relationships between these context types could be clearer.

---

## Current Architecture Analysis

### Game Engine Flow

```
GameEngine::RunGameLoop()
    │
    ├─→ UpdateSystems()  [per loop iteration]
    │       │
    │       ├─ UpdateGameResources(mouse_position)
    │       ├─ EventHandler::PreloadEvents(window)
    │       ├─ EventHandler::ProcessWaitingRoomEventBus()
    │       ├─ EventHandler::UpdateSubscribersFromGlobalEventBus()
    │       ├─ GameEngine::ProcessSubscriptions()
    │       ├─ SceneManager::UpdateSceneManager()
    │       │       ├─ ProcessSubscriptions()
    │       │       └─ UpdateScenes()
    │       │               └─ for each scene: sAction() → sCollision() → sRender()
    │       ├─ DisplayManager::CallRenderCycle()
    │       └─ EventHandler::TickGlobalEventBus()
    │
    └─→ loop_number++
```

### Test Harness Flow

```
ExecuteTickBasedTest(config, fixture)
    │
    ├─→ DetermineNumTicks()
    │
    └─→ for tick in 1..num_ticks:
            │
            └─→ ExecuteSingleTick(tick, config, fixture)
                    │
                    ├─ ExecuteInputEventsForTick()
                    ├─ ExecuteEventsForTick()
                    ├─ ProcessWaitingRoomEventBus()
                    ├─ UpdateSubscribersFromGlobalEventBus()
                    ├─ for each SimulationStep: ExecuteSimulationStep()
                    ├─ CompareTickSnapshot()
                    └─ TickGlobalEventBus()
```

### Key Observations

1. **Scene Logic**: Game uses `m_logic_map[LogicType::*]` organized by scene, while tests use `simulation_data.steps` organized by step type.

2. **Input Sources**: Game uses real SFML events, tests use simulated input from JSON.

3. **Resource Ownership**: Game owns resources in `GameEngine`, tests own resources in `TestFixture`.

4. **Context Structures**:
   - `GameContext`: References to game-level resources
   - `SceneContext`: References to scene-level + game-level resources  
   - `TestContext`: Test metadata for error messages (different purpose entirely)

---

## Proposed Solutions

### Solution 1: Hybrid Free Function Approach with Resource Classes

**Design Decision**: After discussion, a hybrid approach is preferred over pure inheritance:
- **Free functions** for tick execution steps (simpler, more flexible)
- **Compile-time conditionals** for game vs test differences
- **Resource classes** to encapsulate required resources and prevent misuse

The key insight is that misuse is prevented by requiring the appropriate resource class - you can't call a function without having the resources it needs.

#### TickContext - Resource Container

```cpp
/////////////////////////////////////////////////
/// @struct TickContext
/// @brief Container for resources needed during tick execution
///
/// Encapsulates all resources required for tick execution.
/// Functions require this context, preventing misuse.
/////////////////////////////////////////////////
struct TickContext {
  GameResources &game_resources;
  SceneContext &scene_context;
  
  // Game-specific resources (only present in game builds)
#ifndef STEAMROT_TEST_BUILD
  SceneManager *scene_manager = nullptr;
  DisplayManager *display_manager = nullptr;
#endif

  // Test-specific resources (only present in test builds)
#ifdef STEAMROT_TEST_BUILD
  const TestDataConfig *test_config = nullptr;
  uint32_t current_tick = 0;
  TestFixture *test_fixture = nullptr;
#endif
};
```

#### Free Functions for Tick Execution

```cpp
namespace tick {

/////////////////////////////////////////////////
/// @brief Execute pre-tick phase (input capture/injection)
/////////////////////////////////////////////////
void PreTick(TickContext &ctx) {
#ifdef STEAMROT_TEST_BUILD
  // Test: Inject simulated inputs and events
  ExecuteInputEventsForTick(ctx.test_config->input_sequence(), 
                            ctx.current_tick, *ctx.test_fixture);
  ExecuteEventsForTick(ctx.test_config->event_sequence(), 
                       ctx.current_tick, *ctx.test_fixture);
#else
  // Game: Capture real SFML events
  UpdateGameResources(ctx.game_resources);
  ctx.game_resources.event_handler.PreloadEvents(ctx.game_resources.game_window);
#endif
}

/////////////////////////////////////////////////
/// @brief Process event buses (shared - no conditional needed)
/////////////////////////////////////////////////
void ProcessEventBuses(TickContext &ctx) {
  ctx.game_resources.event_handler.ProcessWaitingRoomEventBus();
  ctx.game_resources.event_handler.UpdateSubscribersFromGlobalEventBus();
}

/////////////////////////////////////////////////
/// @brief Process subscriptions
/////////////////////////////////////////////////
void ProcessSubscriptions(TickContext &ctx) {
#ifndef STEAMROT_TEST_BUILD
  // Game: Process engine and scene-manager subscriptions
  ctx.scene_manager->ProcessSubscriptions();
#endif
  // Test harness typically doesn't have subscriptions to process
}

/////////////////////////////////////////////////
/// @brief Execute game logic
/////////////////////////////////////////////////
void ExecuteLogic(TickContext &ctx) {
#ifdef STEAMROT_TEST_BUILD
  // Test: Execute simulation steps from config
  if (ctx.test_config->simulation_data() && 
      ctx.test_config->simulation_data()->steps()) {
    for (const SimulationStep *step : *ctx.test_config->simulation_data()->steps()) {
      ExecuteSimulationStep(step, ctx.scene_context);
    }
  }
#else
  // Game: Execute scene logic via SceneManager
  ctx.scene_manager->UpdateScenes();
#endif
}

/////////////////////////////////////////////////
/// @brief Post-logic phase (rendering or validation)
/////////////////////////////////////////////////
void PostLogic(TickContext &ctx) {
#ifdef STEAMROT_TEST_BUILD
  // Test: Compare tick snapshot if present
  CompareTickSnapshot(ctx.current_tick, ctx.test_config, *ctx.test_fixture);
#else
  // Game: Render to window
  ctx.display_manager->CallRenderCycle();
#endif
}

/////////////////////////////////////////////////
/// @brief Tick the event bus (shared - no conditional needed)
/////////////////////////////////////////////////
void TickEventBus(TickContext &ctx) {
  ctx.game_resources.event_handler.TickGlobalEventBus();
}

/////////////////////////////////////////////////
/// @brief Execute a complete tick with guaranteed ordering
///
/// This is the main entry point that enforces execution order.
/// All the individual functions can also be called separately
/// for more granular control in tests.
/////////////////////////////////////////////////
void ExecuteTick(TickContext &ctx) {
  PreTick(ctx);
  ProcessEventBuses(ctx);
  ProcessSubscriptions(ctx);
  ExecuteLogic(ctx);
  PostLogic(ctx);
  TickEventBus(ctx);
}

} // namespace tick
```

#### Benefits of This Approach

1. **Enforced ordering**: `ExecuteTick()` guarantees the correct sequence
2. **No vtable overhead**: Free functions are simpler and faster
3. **Compile-time safety**: Wrong code paths are compiled out entirely
4. **Resource encapsulation**: `TickContext` ensures functions have what they need
5. **Flexibility**: Individual functions can be called for granular testing
6. **Simpler testing**: Individual functions easier to unit test in isolation

#### Usage Examples

**Game Engine (compiled without STEAMROT_TEST_BUILD):**
```cpp
// This code only compiles in game builds where scene_manager and 
// display_manager members exist in TickContext
void GameEngine::RunGameLoop() {
  tick::TickContext ctx{
    .game_resources = m_game_resources,
    .scene_context = GetSceneContext(),
    .scene_manager = &m_scene_manager,      // Only in game builds
    .display_manager = &m_display_manager   // Only in game builds
  };
  
  while (m_game_resources.game_window.isOpen()) {
    tick::ExecuteTick(ctx);
    m_game_resources.loop_number++;
  }
}
```

**Test Harness (compiled with STEAMROT_TEST_BUILD defined):**
```cpp
// This code only compiles in test builds where test_config, current_tick,
// and test_fixture members exist in TickContext
void ExecuteTickBasedTest(const TestDataConfig *config, TestFixture &fixture) {
  tick::TickContext ctx{
    .game_resources = fixture.GetGameResources(),
    .scene_context = fixture.GetSceneContext(),
    .test_config = config,       // Only in test builds
    .current_tick = 0,           // Only in test builds
    .test_fixture = &fixture     // Only in test builds
  };
  
  uint32_t num_ticks = DetermineNumTicks(config);
  for (uint32_t tick = 1; tick <= num_ticks; ++tick) {
    ctx.current_tick = tick;
    tick::ExecuteTick(ctx);
  }
}
```

### Solution 2: Logic Provider Interface

Create an interface for providing logic execution, allowing both fixed scene logic and custom test logic:

```cpp
/////////////////////////////////////////////////
/// @interface ILogicProvider
/// @brief Interface for providing logic execution
/////////////////////////////////////////////////
class ILogicProvider {
public:
  virtual ~ILogicProvider() = default;
  
  /////////////////////////////////////////////////
  /// @brief Execute action logic
  /////////////////////////////////////////////////
  virtual void ExecuteAction() = 0;
  
  /////////////////////////////////////////////////
  /// @brief Execute collision logic
  /////////////////////////////////////////////////
  virtual void ExecuteCollision() = 0;
  
  /////////////////////////////////////////////////
  /// @brief Execute render logic
  /////////////////////////////////////////////////
  virtual void ExecuteRender() = 0;
};

/////////////////////////////////////////////////
/// @class SceneLogicProvider
/// @brief Provides logic from scene's logic map
/////////////////////////////////////////////////
class SceneLogicProvider : public ILogicProvider {
  LogicCollection &m_logic_map;
  
public:
  void ExecuteAction() override {
    for (auto &logic : m_logic_map[LogicType::Action]) {
      logic->RunLogic();
    }
  }
  
  void ExecuteCollision() override {
    for (auto &logic : m_logic_map[LogicType::Collision]) {
      logic->RunLogic();
    }
  }
  
  void ExecuteRender() override {
    for (auto &logic : m_logic_map[LogicType::Render]) {
      logic->RunLogic();
    }
  }
};

/////////////////////////////////////////////////
/// @class SimulationLogicProvider
/// @brief Provides logic from simulation data configuration
/////////////////////////////////////////////////
class SimulationLogicProvider : public ILogicProvider {
  const SimulationData *m_simulation_data;
  SceneContext &m_scene_context;
  
public:
  void ExecuteAction() override {
    ExecuteStepsOfType(SimulationType::Action);
  }
  
  void ExecuteCollision() override {
    ExecuteStepsOfType(SimulationType::Collision);
  }
  
  void ExecuteRender() override {
    ExecuteStepsOfType(SimulationType::Render);
  }

private:
  void ExecuteStepsOfType(SimulationType type) {
    if (!m_simulation_data || !m_simulation_data->steps()) return;
    
    for (const SimulationStep *step : *m_simulation_data->steps()) {
      if (step && step->simulation_type() == type) {
        ExecuteSimulationStep(step, m_scene_context);
      }
    }
  }
};
```

### Solution 3: Unified Data Loading

Create a common data loading abstraction:

```cpp
/////////////////////////////////////////////////
/// @interface IEntityDataSource
/// @brief Interface for entity data sources
/////////////////////////////////////////////////
class IEntityDataSource {
public:
  virtual ~IEntityDataSource() = default;
  
  /////////////////////////////////////////////////
  /// @brief Get entity collection data
  /////////////////////////////////////////////////
  virtual const EntityCollection* GetEntityCollection() const = 0;
};

/////////////////////////////////////////////////
/// @class DefaultSceneDataSource
/// @brief Loads entities from default scene data files
/////////////////////////////////////////////////
class DefaultSceneDataSource : public IEntityDataSource {
  SceneType m_scene_type;
  const EntityCollection *m_cached_data = nullptr;
  
public:
  explicit DefaultSceneDataSource(SceneType scene_type) 
    : m_scene_type(scene_type) {
    // Load from FlatbuffersDataLoader
  }
  
  const EntityCollection* GetEntityCollection() const override {
    return m_cached_data;
  }
};

/////////////////////////////////////////////////
/// @class TestDataSource
/// @brief Provides entities from test data configuration
/////////////////////////////////////////////////
class TestDataSource : public IEntityDataSource {
  const TestDataConfig *m_config;
  
public:
  explicit TestDataSource(const TestDataConfig *config) 
    : m_config(config) {}
  
  const EntityCollection* GetEntityCollection() const override {
    if (m_config && m_config->start_data_collection()) {
      return m_config->start_data_collection()->entity_collection();
    }
    return nullptr;
  }
};
```

### Solution 4: Remove expected_data_collection

Since `tick_snapshots` provides per-tick validation, we can:

1. **Deprecate `expected_data_collection`** in favor of using a final tick snapshot
2. **Or keep both** where `expected_data_collection` is a convenience for single-tick tests

Recommendation: Keep both for flexibility but document that `tick_snapshots` at the final tick serves the same purpose.

### Solution 5: Rename TestContext to TestMetadataContext

To reduce confusion with `GameContext` and `SceneContext`:

```cpp
/////////////////////////////////////////////////
/// @brief Test metadata for error messages and reporting
///
/// Note: This is NOT related to GameContext or SceneContext.
/// It holds metadata about the test case, not game resources.
/////////////////////////////////////////////////
struct TestMetadataContext {
  std::string test_name;
  std::optional<std::string> description;
  std::optional<uint32_t> current_tick;
  std::optional<uint32_t> total_ticks;
  
  std::string FormatTestName() const;
  std::string FormatTickInfo() const;
};
```

---

## Implementation Plan

### Phase 1: Create Hybrid Tick Execution System (Point 1)

**Files to create:**
- `src/systems/tick_execution.h` - TickContext struct and free function declarations
- `src/systems/tick_execution.cpp` - Free function implementations

**Files to modify:**
- `src/systems/GameEngine.cpp` - Refactor to use tick::ExecuteTick()
- `tests/harness/tick_executor.cpp` - Refactor to use tick::ExecuteTick()
- `CMakeLists.txt` - Add `STEAMROT_TEST_BUILD` compile definition for test targets

**Compile-time configuration:**
```cmake
# In test CMakeLists.txt
target_compile_definitions(test_target PRIVATE STEAMROT_TEST_BUILD)
```

**Estimated effort:** Medium

### Phase 2: Create Logic Provider Interface (Point 2)

**Files to create:**
- `src/logic/ILogicProvider.h` - Interface
- `src/logic/SceneLogicProvider.h/cpp` - Scene-based implementation
- `tests/harness/SimulationLogicProvider.h` - Test-based implementation

**Files to modify:**
- `src/scenes/Scene.h/cpp` - Use ILogicProvider

**Estimated effort:** Medium

### Phase 3: Unify Data Loading (Point 3)

**Files to create:**
- `src/data_handlers/IEntityDataSource.h` - Interface
- `src/data_handlers/DefaultSceneDataSource.h/cpp` - Default data source
- `tests/harness/TestDataSource.h` - Test data source

**Files to modify:**
- `src/entity/EntityManager.h/cpp` - Accept IEntityDataSource
- `tests/harness/TestFixture.cpp` - Use TestDataSource

**Estimated effort:** Low-Medium

### Phase 4: Document expected_data_collection Usage (Point 4)

**Files to modify:**
- `documentation/testing/TEST_DATA_CONFIGURATION.md` - Clarify usage
- `src/flatbuffers_headers/test_data.fbs` - Add deprecation comment if needed

**Estimated effort:** Low

### Phase 5: Rename TestContext (Point 5)

**Files to modify:**
- `tests/harness/test_context.h` - Rename struct
- All files using TestContext - Update references

**Estimated effort:** Low

---

## Benefits

1. **Consistency**: Same event ordering in game and tests reduces behavioral differences
2. **Maintainability**: Common code reduces duplication and potential for drift
3. **Flexibility**: Logic provider interface allows custom logic injection
4. **Clarity**: Renamed TestMetadataContext removes confusion
5. **Testability**: Tests can more accurately simulate game behavior

---

## Risks

1. **Breaking Changes**: Refactoring core loop may introduce bugs
2. **Complexity**: New abstractions add indirection
3. **Migration Effort**: Existing code must be updated

### Mitigations

1. Implement incrementally with comprehensive tests
2. Keep abstractions minimal and well-documented
3. Provide migration guides and deprecation warnings

---

## Migration Path

1. **Phase 1**: Create new abstractions alongside existing code
2. **Phase 2**: Migrate game engine to new abstractions
3. **Phase 3**: Migrate test harness to new abstractions
4. **Phase 4**: Remove deprecated code
5. **Phase 5**: Update documentation

---

## Future Considerations

1. **Record/Replay**: Unified tick executor could support recording and replaying game sessions
2. **Deterministic Testing**: Consistent ordering enables deterministic test replay
3. **Performance Profiling**: Common tick executor could include instrumentation hooks
4. **Headless Mode**: Easier to run game logic without rendering for automated testing

---

## Alternative Approaches Considered

### Alternative A: Pure Inheritance (Template Method Pattern)

**Description**: Abstract `TickExecutor` base class with virtual hook methods.

```cpp
class TickExecutor {
protected:
  virtual void OnPreTick() = 0;
  virtual void OnExecuteLogic() = 0;
  // ...
public:
  void ExecuteTick() { /* calls virtuals in order */ }
};
```

**Pros**: 
- Guarantees execution order via template method
- Natural state encapsulation in derived classes

**Cons**: 
- Vtable overhead
- Tighter coupling
- More boilerplate

**Decision**: Not chosen - hybrid approach with free functions and compile-time conditionals preferred.

### Alternative B: Keep Separate Implementations

**Pros**: Less refactoring, simpler implementation
**Cons**: Continued code duplication, potential for drift

### Alternative C: Generate Test Harness from Game Code

**Pros**: Single source of truth
**Cons**: Complex code generation, less flexibility for tests

### Alternative D: Full Runtime Dependency Injection

**Pros**: Maximum flexibility
**Cons**: Over-engineering, runtime overhead, complexity

---

## Decision

**Chosen Approach**: Hybrid free function approach with compile-time conditionals (Solution 1 - Updated)

**Rationale**:
1. Free functions are simpler and have no vtable overhead
2. Compile-time conditionals ensure wrong code paths are never compiled
3. `TickContext` resource class prevents misuse - you can't call a function without the required resources
4. `tick::ExecuteTick()` still enforces the correct execution order
5. Individual functions remain testable in isolation

---

## Appendix: Detailed File Changes

### New Files

| File | Purpose |
|------|---------|
| `src/systems/tick_execution.h` | TickContext struct and free function declarations |
| `src/systems/tick_execution.cpp` | Free function implementations with compile-time conditionals |
| `src/logic/ILogicProvider.h` | Logic provider interface |
| `src/logic/SceneLogicProvider.h/cpp` | Scene-based logic provider |
| `src/data_handlers/IEntityDataSource.h` | Entity data source interface |

### Modified Files

| File | Changes |
|------|---------|
| `src/systems/GameEngine.cpp` | Use tick::ExecuteTick() with TickContext |
| `src/scenes/Scene.h/cpp` | Use ILogicProvider |
| `tests/harness/tick_executor.cpp` | Use tick::ExecuteTick() with TickContext |
| `tests/harness/test_context.h` | Rename to TestMetadataContext |
| `CMakeLists.txt` | Add STEAMROT_TEST_BUILD definition for test targets |

### Documentation Updates

| File | Changes |
|------|---------|
| `documentation/architecture/GAME_LOOP.md` | Add tick execution section |
| `documentation/testing/TESTING_HARNESS_LOOP.md` | Update architecture |
| `documentation/testing/TEST_DATA_CONFIGURATION.md` | Clarify expected_data usage |
