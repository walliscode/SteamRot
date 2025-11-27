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

### Solution 1: Unified Tick Executor

Create a common `TickExecutor` class that both the game engine and test harness can use:

```cpp
/////////////////////////////////////////////////
/// @class TickExecutor
/// @brief Abstract base class for tick-based execution
///
/// Provides a consistent execution order for game ticks.
/// Derived classes can customize specific phases.
/////////////////////////////////////////////////
class TickExecutor {
protected:
  SceneContext &m_scene_context;
  GameResources &m_game_resources;

  /////////////////////////////////////////////////
  /// @brief Hook for pre-tick processing (input capture)
  /////////////////////////////////////////////////
  virtual void OnPreTick() = 0;

  /////////////////////////////////////////////////
  /// @brief Hook for event processing
  /////////////////////////////////////////////////
  virtual void OnProcessEvents() = 0;

  /////////////////////////////////////////////////
  /// @brief Hook for logic execution
  /////////////////////////////////////////////////
  virtual void OnExecuteLogic() = 0;

  /////////////////////////////////////////////////
  /// @brief Hook for post-logic processing (rendering, validation)
  /////////////////////////////////////////////////
  virtual void OnPostLogic() = 0;

public:
  /////////////////////////////////////////////////
  /// @brief Execute a single tick with consistent ordering
  /////////////////////////////////////////////////
  void ExecuteTick() {
    // 1. Pre-tick (input capture/injection)
    OnPreTick();
    
    // 2. Event processing (consistent order)
    m_game_resources.event_handler.ProcessWaitingRoomEventBus();
    m_game_resources.event_handler.UpdateSubscribersFromGlobalEventBus();
    
    // 3. Process events
    OnProcessEvents();
    
    // 4. Execute logic
    OnExecuteLogic();
    
    // 5. Post-logic (rendering, validation)
    OnPostLogic();
    
    // 6. Tick event bus (consistent)
    m_game_resources.event_handler.TickGlobalEventBus();
  }
};
```

#### Game Engine Implementation

```cpp
class GameTickExecutor : public TickExecutor {
  SceneManager &m_scene_manager;
  DisplayManager &m_display_manager;
  
protected:
  void OnPreTick() override {
    UpdateGameResources(m_game_resources);
    m_game_resources.event_handler.PreloadEvents(m_game_resources.game_window);
  }
  
  void OnProcessEvents() override {
    // Process game-engine level subscriptions
    ProcessSubscriptions();
    // Process scene-manager level subscriptions
    m_scene_manager.ProcessSubscriptions();
  }
  
  void OnExecuteLogic() override {
    // Execute scene logic (sAction → sCollision → sRender)
    m_scene_manager.UpdateScenes();
  }
  
  void OnPostLogic() override {
    m_display_manager.CallRenderCycle();
  }
};
```

#### Test Harness Implementation

```cpp
class TestTickExecutor : public TickExecutor {
  const TestDataConfig *m_config;
  uint32_t m_current_tick;
  TestFixture &m_fixture;
  
protected:
  void OnPreTick() override {
    // Inject simulated inputs
    ExecuteInputEventsForTick(m_config->input_sequence(), m_current_tick, m_fixture);
    // Inject simulated events  
    ExecuteEventsForTick(m_config->event_sequence(), m_current_tick, m_fixture);
  }
  
  void OnProcessEvents() override {
    // Test harness may not have scene-manager subscriptions
    // This could be empty or configurable
  }
  
  void OnExecuteLogic() override {
    // Execute simulation steps (custom logic configuration)
    if (m_config->simulation_data() && m_config->simulation_data()->steps()) {
      for (const SimulationStep *step : *m_config->simulation_data()->steps()) {
        ExecuteSimulationStep(step, m_scene_context);
      }
    }
  }
  
  void OnPostLogic() override {
    // Compare tick snapshot if present
    CompareTickSnapshot(m_current_tick, m_config, m_fixture);
  }
};
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

### Phase 1: Create Unified Tick Executor (Point 1)

**Files to create:**
- `src/systems/TickExecutor.h` - Abstract base class
- `src/systems/GameTickExecutor.h/cpp` - Game-specific implementation

**Files to modify:**
- `src/systems/GameEngine.cpp` - Refactor to use GameTickExecutor
- `tests/harness/tick_executor.h/cpp` - Refactor to use base class

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

### Alternative A: Keep Separate Implementations

**Pros**: Less refactoring, simpler implementation
**Cons**: Continued code duplication, potential for drift

### Alternative B: Generate Test Harness from Game Code

**Pros**: Single source of truth
**Cons**: Complex code generation, less flexibility for tests

### Alternative C: Full Dependency Injection

**Pros**: Maximum flexibility
**Cons**: Over-engineering, complexity overhead

---

## Decision

*To be filled in after review*

---

## Appendix: Detailed File Changes

### New Files

| File | Purpose |
|------|---------|
| `src/systems/TickExecutor.h` | Abstract tick executor base class |
| `src/systems/GameTickExecutor.h/cpp` | Game engine tick executor |
| `src/logic/ILogicProvider.h` | Logic provider interface |
| `src/logic/SceneLogicProvider.h/cpp` | Scene-based logic provider |
| `src/data_handlers/IEntityDataSource.h` | Entity data source interface |

### Modified Files

| File | Changes |
|------|---------|
| `src/systems/GameEngine.cpp` | Use GameTickExecutor |
| `src/scenes/Scene.h/cpp` | Use ILogicProvider |
| `tests/harness/tick_executor.h/cpp` | Derive from TickExecutor |
| `tests/harness/test_context.h` | Rename to TestMetadataContext |

### Documentation Updates

| File | Changes |
|------|---------|
| `documentation/architecture/GAME_LOOP.md` | Add TickExecutor section |
| `documentation/testing/TESTING_HARNESS_LOOP.md` | Update architecture |
| `documentation/testing/TEST_DATA_CONFIGURATION.md` | Clarify expected_data usage |
