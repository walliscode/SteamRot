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

### Solution 1: Abstract Engine Base Class with Unified Resource Management

**Design Decision**: After discussion, a hybrid approach combines:
- **Abstract Engine base class** for unified resource management
- **Derived GameEngine and TestEngine** for specific behaviors
- **Free functions with TickContext** for tick execution steps
- **Virtual methods** only for the divergent behavior points

The key insight is that resource management should be identical between game and test, with only the tick execution behavior differing.

#### Engine Class Hierarchy

```cpp
/////////////////////////////////////////////////
/// @class Engine
/// @brief Abstract base class for game and test engines
///
/// Provides unified resource management. Both GameEngine and
/// TestEngine derive from this, ensuring consistent resource
/// handling while allowing different tick execution behaviors.
/////////////////////////////////////////////////
class Engine {
protected:
  /////////////////////////////////////////////////
  /// @brief Game-level resources (window, event handler, assets, etc.)
  /////////////////////////////////////////////////
  GameResources m_game_resources;

  /////////////////////////////////////////////////
  /// @brief Game context providing references to resources
  /////////////////////////////////////////////////
  GameContext m_game_context;

  /////////////////////////////////////////////////
  /// @brief Constructor initializes resources
  /////////////////////////////////////////////////
  Engine(EnvironmentType env_type = EnvironmentType::None);

  /////////////////////////////////////////////////
  /// @brief Configure engine from FlatBuffers data
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureFromData() = 0;

  /////////////////////////////////////////////////
  /// @brief Run a single tick - implemented differently by derived classes
  /////////////////////////////////////////////////
  virtual void ExecuteTick() = 0;

public:
  virtual ~Engine() = default;

  /////////////////////////////////////////////////
  /// @brief Start up the engine (load resources, configure)
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> StartUp();

  /////////////////////////////////////////////////
  /// @brief Run the main loop
  /// @param num_ticks Number of ticks to run (0 = infinite for game)
  /////////////////////////////////////////////////
  void Run(size_t num_ticks = 0);

  /////////////////////////////////////////////////
  /// @brief Get current loop number
  /////////////////////////////////////////////////
  size_t GetLoopNumber() const { return m_game_resources.loop_number; }

  /////////////////////////////////////////////////
  /// @brief Access game resources
  /////////////////////////////////////////////////
  GameResources& GetGameResources() { return m_game_resources; }
  const GameResources& GetGameResources() const { return m_game_resources; }
};
```

#### GameEngine - Production Implementation

```cpp
/////////////////////////////////////////////////
/// @class GameEngine
/// @brief Production game engine with window, rendering, etc.
/////////////////////////////////////////////////
class GameEngine : public Engine {
private:
  SceneManager m_scene_manager;
  DisplayManager m_display_manager;

protected:
  std::expected<std::monostate, FailInfo> ConfigureFromData() override {
    // Load game engine data, configure scene manager, etc.
    FlatbuffersDataLoader data_loader;
    auto game_data = data_loader.ProvideGameEngineData();
    // ... configuration
    return std::monostate{};
  }

  void ExecuteTick() override {
    // Game-specific tick execution
    UpdateGameResources(m_game_resources);
    m_game_resources.event_handler.PreloadEvents(m_game_resources.game_window);
    
    // Event bus processing (consolidated - all event handling together)
    m_game_resources.event_handler.ProcessWaitingRoomEventBus();
    m_game_resources.event_handler.ClearSubscribers();
    m_game_resources.event_handler.UpdateSubscribersFromGlobalEventBus();
    m_game_resources.event_handler.TickGlobalEventBus();
    
    // Game-specific logic (after all event handling complete)
    ProcessSubscriptions();
    m_scene_manager.UpdateSceneManager();
    m_display_manager.CallRenderCycle();
  }

public:
  GameEngine(EnvironmentType env_type = EnvironmentType::Production)
    : Engine(env_type),
      m_scene_manager(m_game_context),
      m_display_manager(m_game_resources.game_window, m_scene_manager) {}
};
```

#### TestEngine - Testing Implementation

```cpp
/////////////////////////////////////////////////
/// @class TestEngine
/// @brief Test engine for data-driven testing
///
/// Uses same resource management as GameEngine but with
/// simulated inputs and validation instead of rendering.
/////////////////////////////////////////////////
class TestEngine : public Engine {
private:
  const TestDataConfig *m_test_config = nullptr;
  EntityManager m_entity_manager;
  SceneContext m_scene_context;
  uint32_t m_current_tick = 0;

protected:
  std::expected<std::monostate, FailInfo> ConfigureFromData() override {
    // Configure from test data instead of game data files
    if (!m_test_config) {
      return std::unexpected(FailInfo{FailMode::NullPointer, "TestDataConfig is null"});
    }
    
    // Configure entities from test data
    if (m_test_config->start_data_collection() && 
        m_test_config->start_data_collection()->entity_collection()) {
      FlatbuffersConfigurator configurator(m_game_resources.event_handler);
      auto result = configurator.ConfigureEntitiesFromCollection(
          m_entity_manager.GetEntityMemoryPool(),
          m_test_config->start_data_collection()->entity_collection());
      if (!result.has_value()) return std::unexpected(result.error());
    }
    
    return std::monostate{};
  }

  void ExecuteTick() override {
    ++m_current_tick;
    
    // Test-specific: Inject simulated inputs and events
    ExecuteInputEventsForTick(m_test_config->input_sequence(), 
                               m_current_tick, m_game_resources);
    ExecuteEventsForTick(m_test_config->event_sequence(), 
                         m_current_tick, m_game_resources);
    
    // Event bus processing (consolidated - all event handling together)
    m_game_resources.event_handler.ProcessWaitingRoomEventBus();
    m_game_resources.event_handler.ClearSubscribers();
    m_game_resources.event_handler.UpdateSubscribersFromGlobalEventBus();
    m_game_resources.event_handler.TickGlobalEventBus();
    
    // Test-specific: Execute simulation steps (after all event handling)
    if (m_test_config->simulation_data() && 
        m_test_config->simulation_data()->steps()) {
      for (const SimulationStep *step : *m_test_config->simulation_data()->steps()) {
        ExecuteSimulationStep(step, m_scene_context);
      }
    }
    
    // Test-specific: Validate tick snapshot
    CompareTickSnapshot(m_current_tick, m_test_config, *this);
  }

public:
  TestEngine(const TestDataConfig *config)
    : Engine(EnvironmentType::Test),
      m_test_config(config),
      m_entity_manager(m_game_resources.event_handler),
      m_scene_context(m_entity_manager.GetEntityMemoryPool(),
                      m_entity_manager.GetArchetypeManager(),
                      m_scene_resources,
                      m_game_resources) {}
      
  /////////////////////////////////////////////////
  /// @brief Access entity manager for test assertions
  /////////////////////////////////////////////////
  EntityManager& GetEntityManager() { return m_entity_manager; }
};
```

#### Shared Run Loop in Base Class

```cpp
void Engine::Run(size_t num_ticks) {
  // Start up and configure
  auto startup_result = StartUp();
  if (!startup_result.has_value()) {
    // Handle error
    return;
  }
  
  // Run the loop
  if (num_ticks == 0) {
    // Infinite loop (for game)
    while (m_game_resources.game_window.isOpen()) {
      ExecuteTick();
      m_game_resources.loop_number++;
    }
  } else {
    // Fixed number of ticks (for tests)
    for (size_t i = 0; i < num_ticks; ++i) {
      ExecuteTick();
      m_game_resources.loop_number++;
    }
  }
}
```

#### Benefits of Abstract Engine Approach

1. **Unified resource management**: `GameResources`, `GameContext` handled identically
2. **Consistent loop structure**: `Run()` method shared between game and test
3. **Clear extension points**: Only `ConfigureFromData()` and `ExecuteTick()` differ
4. **Type-safe**: Can't accidentally mix game and test behaviors
5. **Testable**: `TestEngine` can be used in unit tests directly
6. **No compile-time conditionals needed**: Polymorphism handles the differences

#### TickContext Still Available for Fine-Grained Control

The `TickContext` approach remains available for cases where you want to share individual tick execution steps:

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

The data loading architecture consists of three layers that work together:

#### Layer 1: Data Sources (IEntityDataSource)

The `IEntityDataSource` interface abstracts **where** entity data comes from. This determines data paths and provides raw entity collection data:

```cpp
/////////////////////////////////////////////////
/// @interface IEntityDataSource
/// @brief Interface for entity data sources - determines data paths
/////////////////////////////////////////////////
class IEntityDataSource {
public:
  virtual ~IEntityDataSource() = default;
  
  /////////////////////////////////////////////////
  /// @brief Get entity collection data from this source
  /////////////////////////////////////////////////
  virtual const EntityCollection* GetEntityCollection() const = 0;
  
  /////////////////////////////////////////////////
  /// @brief Get the source identifier (for logging/debugging)
  /////////////////////////////////////////////////
  virtual std::string GetSourceIdentifier() const = 0;
};

/////////////////////////////////////////////////
/// @class DefaultSceneDataSource
/// @brief Loads entities from default scene data files
///
/// Uses PathProvider and FlatbuffersDataLoader internally to
/// determine and load from the correct data paths.
/////////////////////////////////////////////////
class DefaultSceneDataSource : public IEntityDataSource {
  SceneType m_scene_type;
  FlatbuffersDataLoader m_data_loader;
  const SceneData *m_scene_data = nullptr;
  
public:
  explicit DefaultSceneDataSource(SceneType scene_type) 
    : m_scene_type(scene_type) {
    // FlatbuffersDataLoader uses PathProvider internally
    // to determine data file paths based on EnvironmentType
    auto scene_data_result = m_data_loader.ProvideSceneData(scene_type);
    if (scene_data_result.has_value()) {
      m_scene_data = scene_data_result.value();
    }
  }
  
  const EntityCollection* GetEntityCollection() const override {
    if (m_scene_data && m_scene_data->entity_collection()) {
      return m_scene_data->entity_collection();
    }
    return nullptr;
  }
  
  std::string GetSourceIdentifier() const override {
    return "DefaultSceneData[" + std::to_string(static_cast<int>(m_scene_type)) + "]";
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
    if (m_config && m_config->start_data_collection() &&
        m_config->start_data_collection()->entity_collection()) {
      return m_config->start_data_collection()->entity_collection();
    }
    return nullptr;
  }
  
  std::string GetSourceIdentifier() const override {
    if (m_config && m_config->metadata() && m_config->metadata()->test_name()) {
      return "TestData[" + m_config->metadata()->test_name()->str() + "]";
    }
    return "TestData[unknown]";
  }
};
```

#### Layer 2: Entity Configurators (EntityConfigurator hierarchy)

The `EntityConfigurator` hierarchy handles **how** entity data is interpreted and applied. The existing `FlatbuffersConfigurator` remains as a derived class for FlatBuffers data:

```cpp
/////////////////////////////////////////////////
/// @class EntityConfigurator
/// @brief Abstract base for configuring entities from data
///
/// Different data formats (FlatBuffers, JSON, etc.) can have
/// their own derived configurator classes.
/////////////////////////////////////////////////
class EntityConfigurator {
protected:
  EventHandler &m_event_handler;
  
public:
  EntityConfigurator(EventHandler &event_handler);
  virtual ~EntityConfigurator() = default;
  
  /////////////////////////////////////////////////
  /// @brief Configure entities from a data source
  ///
  /// @param entity_memory_pool Pool to configure
  /// @param data_source Source providing entity data
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureEntities(EntityMemoryPool &entity_memory_pool,
                    const IEntityDataSource &data_source) = 0;
};

/////////////////////////////////////////////////
/// @class FlatbuffersConfigurator
/// @brief Configures entities from FlatBuffers data
///
/// This is the primary configurator used for both game and tests
/// since both use FlatBuffers as the data format.
/////////////////////////////////////////////////
class FlatbuffersConfigurator : public EntityConfigurator {
  // ... existing implementation ...
  
public:
  /////////////////////////////////////////////////
  /// @brief Configure entities from any IEntityDataSource
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureEntities(EntityMemoryPool &entity_memory_pool,
                    const IEntityDataSource &data_source) override {
    const EntityCollection *collection = data_source.GetEntityCollection();
    if (!collection) {
      return std::unexpected(FailInfo{FailMode::NullPointer, 
        "No entity collection from " + data_source.GetSourceIdentifier()});
    }
    return ConfigureEntitiesFromCollection(entity_memory_pool, collection);
  }
  
  // Existing methods remain for backwards compatibility:
  // - ConfigureEntitiesFromDefaultData(pool, scene_type)
  // - ConfigureEntitiesFromCollection(pool, collection)
};

/////////////////////////////////////////////////
/// @class JsonConfigurator (future)
/// @brief Example: Could support JSON data format if needed
/////////////////////////////////////////////////
// class JsonConfigurator : public EntityConfigurator { ... };
```

#### Layer 3: Usage in Engine Classes

The `Engine` base class uses these abstractions:

```cpp
class Engine {
protected:
  GameResources m_game_resources;
  FlatbuffersConfigurator m_configurator;  // Primary configurator
  
  /////////////////////////////////////////////////
  /// @brief Configure entities from a data source
  /// 
  /// Can be overridden to use different configurators if needed.
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureEntitiesFrom(EntityMemoryPool &pool, 
                        const IEntityDataSource &source) {
    return m_configurator.ConfigureEntities(pool, source);
  }
  
public:
  Engine(EventHandler &event_handler) 
    : m_configurator(event_handler) {}
};

class GameEngine : public Engine {
  void LoadScene(SceneType scene_type) {
    DefaultSceneDataSource source(scene_type);
    auto result = ConfigureEntitiesFrom(m_entity_pool, source);
    // ...
  }
};

class TestEngine : public Engine {
  const TestDataConfig *m_test_config;
  
  std::expected<std::monostate, FailInfo> ConfigureFromData() override {
    TestDataSource source(m_test_config);
    return ConfigureEntitiesFrom(m_entity_pool, source);
  }
};
```

#### Data Path Determination

Data paths are determined by `PathProvider` (unchanged) and used internally by `FlatbuffersDataLoader`:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         DATA PATH DETERMINATION                              │
└─────────────────────────────────────────────────────────────────────────────┘

PathProvider (already exists - determines base paths):
┌─────────────────────────────────────────────────────────────────────────────┐
│ PathProvider::PathProvider(EnvironmentType env_type)                        │
│                                                                             │
│   EnvironmentType::Production → data_dir (from CMake)                       │
│   EnvironmentType::Test       → test_data_dir (from CMake)                  │
│                                                                             │
│ PathProvider::GetSceneDataPath(SceneType) → full path to scene .bin file    │
│ PathProvider::GetAssetPath(asset_name) → full path to asset file            │
└─────────────────────────────────────────────────────────────────────────────┘

FlatbuffersDataLoader uses PathProvider internally:
┌─────────────────────────────────────────────────────────────────────────────┐
│ FlatbuffersDataLoader::ProvideSceneData(SceneType scene_type)               │
│   │                                                                         │
│   └─► PathProvider::GetSceneDataPath(scene_type)                            │
│       │                                                                     │
│       └─► Load and verify FlatBuffers binary                                │
│           │                                                                 │
│           └─► Return const SceneData*                                       │
└─────────────────────────────────────────────────────────────────────────────┘
```

#### Summary: Three Layers

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         DATA LOADING ARCHITECTURE                            │
└─────────────────────────────────────────────────────────────────────────────┘

Layer 1: DATA SOURCES (IEntityDataSource)
─────────────────────────────────────────
Determines WHERE data comes from and provides raw EntityCollection.

  ┌─────────────────────────┐    ┌─────────────────────────┐
  │  DefaultSceneDataSource │    │    TestDataSource       │
  ├─────────────────────────┤    ├─────────────────────────┤
  │ • Uses PathProvider     │    │ • Uses TestDataConfig   │
  │ • Uses FlatbuffersData- │    │ • Extracts entity data  │
  │   Loader internally     │    │   from test config      │
  └─────────────────────────┘    └─────────────────────────┘
              │                              │
              └──────────┬───────────────────┘
                         ▼
Layer 2: CONFIGURATORS (EntityConfigurator hierarchy)
─────────────────────────────────────────────────────
Determines HOW data is interpreted and applied to entities.

  ┌─────────────────────────────────────────────────────────┐
  │             FlatbuffersConfigurator                      │
  │        (primary - both game and tests use this)          │
  ├─────────────────────────────────────────────────────────┤
  │ • ConfigureComponent(UserInterfaceData*, CUserInterface)│
  │ • ConfigureComponent(GrimoireMachinaData*, CGrimoire...)│
  │ • ConfigureEntitiesFromCollection(pool, collection)      │
  └─────────────────────────────────────────────────────────┘
                         │
                         ▼
Layer 3: ENGINE CLASSES
───────────────────────
Uses data sources and configurators to load entities.

  ┌─────────────────────────┐    ┌─────────────────────────┐
  │       GameEngine        │    │       TestEngine        │
  ├─────────────────────────┤    ├─────────────────────────┤
  │ DefaultSceneDataSource  │    │ TestDataSource          │
  │         +               │    │         +               │
  │ FlatbuffersConfigurator │    │ FlatbuffersConfigurator │
  └─────────────────────────┘    └─────────────────────────┘
```

#### Future Extensibility

This architecture allows for:

1. **New data sources**: Add `JsonDataSource`, `DatabaseDataSource`, etc.
2. **New configurators**: Add `JsonConfigurator` if using JSON entity format
3. **Mixing sources**: A scene could load base entities from default + override from test
4. **Caching**: Data sources can cache loaded data internally

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

### Solution 6: Test Data Loading Infrastructure

The current test harness uses `load_test_data_configs()` to discover test data from adjacent directories, combined with Catch2's `GENERATE` macro. To reduce boilerplate and enable easy test setup across directories:

#### Proposed: CMake-Configured Test Template

Create a template file that CMake configures for each test directory, eliminating repetitive test setup:

**Template file: `tests/cmake/data_driven_test.cpp.in`**

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Auto-generated data-driven test runner for @TEST_DIR_NAME@
///
/// This file is generated by CMake from data_driven_test.cpp.in
/// Do not edit directly - changes will be overwritten.
/////////////////////////////////////////////////

#include "test_data_harness.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

namespace {

/////////////////////////////////////////////////
/// @brief Path to this test's data directory (configured by CMake)
/////////////////////////////////////////////////
constexpr const char* TEST_DATA_DIR = "@TEST_DATA_DIR@";

/////////////////////////////////////////////////
/// @brief Load test configs from the configured data directory
/////////////////////////////////////////////////
auto LoadTestConfigs() {
  return steamrot::tests::LoadTestDataConfigsFromPath(TEST_DATA_DIR);
}

} // namespace

TEST_CASE("@TEST_DIR_NAME@ data-driven tests", "[unit][@TEST_TAG@]") {
  
  auto configs_result = LoadTestConfigs();
  REQUIRE(configs_result.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs_result.value()));
  
  INFO("Running: " << config->metadata()->test_name()->str());
  
  auto result = steamrot::tests::RunFixtureTest(config);
  
  if (!result.has_value()) {
    FAIL("Test failed: " << result.error().message);
  }
}
```

**CMake function to create test runners: `tests/cmake/DataDrivenTests.cmake`**

```cmake
#################################################
# Creates a data-driven test runner for a directory
#
# Usage:
#   add_data_driven_test(
#     NAME logic_collision
#     DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data
#     TAG collision
#   )
#################################################
function(add_data_driven_test)
  cmake_parse_arguments(ARG "" "NAME;DATA_DIR;TAG" "" ${ARGN})
  
  set(TEST_DIR_NAME ${ARG_NAME})
  set(TEST_DATA_DIR ${ARG_DATA_DIR})
  set(TEST_TAG ${ARG_TAG})
  
  # Configure the template
  set(OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${ARG_NAME}_data_driven.test.cpp)
  configure_file(
    ${CMAKE_SOURCE_DIR}/tests/cmake/data_driven_test.cpp.in
    ${OUTPUT_FILE}
    @ONLY
  )
  
  # Add the test executable
  add_executable(test_${ARG_NAME}_data_driven ${OUTPUT_FILE})
  target_link_libraries(test_${ARG_NAME}_data_driven 
    PRIVATE test_harness Catch2::Catch2WithMain)
  
  # Register with CTest
  add_test(NAME ${ARG_NAME}_data_driven 
           COMMAND test_${ARG_NAME}_data_driven)
endfunction()
```

**Usage in test CMakeLists.txt:**

```cmake
# tests/unit/logic/CMakeLists.txt

# Traditional tests
add_executable(test_logic LogicFactory.test.cpp ...)

# Data-driven tests from adjacent data/ directory
add_data_driven_test(
  NAME logic_collision
  DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data
  TAG collision
)

add_data_driven_test(
  NAME logic_render
  DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/render_data
  TAG render
)
```

#### Alternative: Macro-Based Approach (No CMake Generation)

For directories that need custom test logic, provide a convenience macro:

```cpp
// In test_data_harness.h

/////////////////////////////////////////////////
/// @brief Macro to define a standard data-driven test suite
///
/// This macro creates a Catch2 TEST_CASE that:
/// 1. Loads all test configs from adjacent data/ directory
/// 2. Uses GENERATE to iterate through each config
/// 3. Runs RunFixtureTest on each
///
/// @param test_name Name for the TEST_CASE
/// @param tags Catch2 tags (e.g., "[unit][collision]")
/////////////////////////////////////////////////
#define STEAMROT_DATA_DRIVEN_TESTS(test_name, tags) \
  TEST_CASE(test_name, tags) { \
    auto configs_result = steamrot::tests::load_test_data_configs(); \
    REQUIRE(configs_result.has_value()); \
    const auto *config = GENERATE_COPY(from_range(configs_result.value())); \
    INFO("Running: " << config->metadata()->test_name()->str()); \
    auto result = steamrot::tests::RunFixtureTest(config); \
    if (!result.has_value()) { \
      FAIL("Test failed: " << result.error().message); \
    } \
  }
```

**Usage:**

```cpp
// tests/unit/logic/collision_data_driven.test.cpp

#include "test_data_harness.h"

// One line creates the entire data-driven test suite
STEAMROT_DATA_DRIVEN_TESTS("Collision logic data-driven tests", "[unit][collision]")

// Can still add custom tests in same file if needed
TEST_CASE("Custom collision edge case", "[unit][collision]") {
  // custom test logic...
}
```

#### Comparison: CMake Template vs Macro

| Aspect | CMake Template | Macro Approach |
|--------|---------------|----------------|
| **Setup** | One CMake function call per directory | One line in .cpp file |
| **Generated files** | Yes - .cpp files in build dir | No |
| **Custom logic** | Need separate test file | Can mix in same file |
| **Data path** | Configured by CMake | Uses __FILE__ automatically |
| **Build time** | Slightly longer (configure step) | No additional time |
| **Flexibility** | Paths can be anywhere | Must be adjacent data/ dir |

**Recommendation**: Use both approaches:
- **CMake Template** for standardized test directories where all tests are data-driven
- **Macro** for directories mixing data-driven and custom tests

---

## Implementation Plan

### Phase 1: Create Abstract Engine Base Class (Point 1)

**Files to create:**
- `src/systems/Engine.h` - Abstract Engine base class
- `src/systems/Engine.cpp` - Base class implementation (StartUp, Run)
- `tests/harness/TestEngine.h` - TestEngine derived class
- `tests/harness/TestEngine.cpp` - TestEngine implementation

**Files to modify:**
- `src/systems/GameEngine.h` - Derive from Engine
- `src/systems/GameEngine.cpp` - Refactor to override virtual methods

**Key changes:**
```cpp
// Engine.h - Abstract base class
class Engine {
protected:
  GameResources m_game_resources;
  GameContext m_game_context;
  
  virtual std::expected<std::monostate, FailInfo> ConfigureFromData() = 0;
  virtual void ExecuteTick() = 0;
  
public:
  std::expected<std::monostate, FailInfo> StartUp();
  void Run(size_t num_ticks = 0);
};

// GameEngine.h - Production implementation
class GameEngine : public Engine {
  SceneManager m_scene_manager;
  DisplayManager m_display_manager;
protected:
  void ExecuteTick() override;
};

// TestEngine.h - Test implementation  
class TestEngine : public Engine {
  const TestDataConfig *m_test_config;
  EntityManager m_entity_manager;
protected:
  void ExecuteTick() override;
};
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
- `tests/harness/TestEngine.cpp` - Use TestDataSource

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

### Phase 6: Test Data Loading Infrastructure (Solution 6)

**Files to create:**
- `tests/cmake/data_driven_test.cpp.in` - CMake template for auto-generated test runners
- `tests/cmake/DataDrivenTests.cmake` - CMake function to configure test runners
- `tests/harness/test_data_harness.h` additions - `STEAMROT_DATA_DRIVEN_TESTS` macro

**Files to modify:**
- `tests/harness/test_data_harness.h` - Add `LoadTestDataConfigsFromPath()` function
- `tests/harness/test_data_harness.cpp` - Implement path-based loading
- Various test CMakeLists.txt - Use `add_data_driven_test()` function

**Key changes:**
```cmake
# CMake function for easy test setup
add_data_driven_test(
  NAME logic_collision
  DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data
  TAG collision
)
```

```cpp
// Macro for one-line test setup in custom files
STEAMROT_DATA_DRIVEN_TESTS("Collision tests", "[unit][collision]")
```

**Estimated effort:** Low-Medium

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

**Chosen Approach**: Abstract Engine base class with derived GameEngine and TestEngine (Solution 1 - Updated)

**Rationale**:
1. Abstract Engine provides unified resource management
2. Both GameEngine and TestEngine share the same resource handling
3. Virtual `ExecuteTick()` method allows different behaviors without compile-time conditionals
4. `Run()` method in base class ensures consistent loop structure
5. Clear separation of concerns: resource management in base, behavior in derived
6. TestEngine can be used directly in unit tests

---

## Appendix: Detailed File Changes

### New Files

| File | Purpose |
|------|---------|
| `src/systems/Engine.h` | Abstract Engine base class with unified resource management |
| `src/systems/Engine.cpp` | Base class implementation (StartUp, Run) |
| `tests/harness/TestEngine.h` | TestEngine derived class for testing |
| `tests/harness/TestEngine.cpp` | TestEngine implementation with simulated inputs |
| `src/logic/ILogicProvider.h` | Logic provider interface |
| `src/logic/SceneLogicProvider.h/cpp` | Scene-based logic provider |
| `src/data_handlers/IEntityDataSource.h` | Entity data source interface |

### Modified Files

| File | Changes |
|------|---------|
| `src/systems/GameEngine.h` | Derive from Engine, override virtual methods |
| `src/systems/GameEngine.cpp` | Refactor to use Engine base class |
| `src/scenes/Scene.h/cpp` | Use ILogicProvider |
| `tests/harness/TestFixture.cpp` | Refactor to use TestEngine internally for resource management |
| `tests/harness/test_context.h` | Rename to TestMetadataContext |

### Documentation Updates

| File | Changes |
|------|---------|
| `documentation/architecture/GAME_LOOP.md` | Add tick execution section |
| `documentation/testing/TESTING_HARNESS_LOOP.md` | Update architecture |
| `documentation/testing/TEST_DATA_CONFIGURATION.md` | Clarify expected_data usage |
