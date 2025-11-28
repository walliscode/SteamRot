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

### Solution 7: TestEngine Execution Granularity

The TestEngine uses the **SAME architecture as GameEngine** (SceneManager, DisplayManager, etc.) and supports testing at different tick execution levels:

#### Four Tick Execution Levels

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                       TESTENGINE TICK LEVELS                                 │
│     (Uses same architecture as GameEngine - SceneManager, DisplayManager)    │
└─────────────────────────────────────────────────────────────────────────────┘

TickLevel::FullEngine (Mirrors GameEngine::UpdateSystems EXACTLY):
─────────────────────────────────────────────────────────────────
├─ UpdateGameResources → PreloadEvents → ProcessWaitingRoom
├─ UpdateSubscribers → ProcessSubscriptions → UpdateSceneManager
├─ CallRenderCycle → TickGlobalEventBus
├─ Use case: Full integration tests identical to game behavior
├─ API: engine.UseTickLevel(TickLevel::FullEngine)
├─ API: engine.UseFullScene(SceneType::Title) (convenience)

TickLevel::SceneManager (Mirrors SceneManager::UpdateSceneManager):
────────────────────────────────────────────────────────────────
├─ Events → UpdateScenes → ProcessSubscriptions
├─ Use case: Testing scene management without rendering
├─ API: engine.UseTickLevel(TickLevel::SceneManager)

TickLevel::SceneLogic (Runs sAction, sCollision, sRender):
───────────────────────────────────────────────────────────
├─ Events → sAction → sCollision → sRender (current scene)
├─ Use case: Testing scene logic in isolation
├─ API: engine.UseTickLevel(TickLevel::SceneLogic)

TickLevel::Custom (Mix and match - flexible):
────────────────────────────────────────────────
├─ Entity Only: Just load entities, no logic execution
├─ Mixed Steps: Combine Logic classes + free functions in any order
├─ Use case: Unit tests, building up logic incrementally
├─ API: engine.AddLogic<T>(), engine.AddFunction(f)
```

#### Why Same Architecture as GameEngine?

```cpp
// TestEngine has SAME members as GameEngine
class GameEngine {
  GameResources m_game_resources;
  GameContext m_game_context;
  SceneManager m_scene_manager;    // ← SAME
  DisplayManager m_display_manager; // ← SAME
};

class TestEngine : public Engine {
  SceneManager m_scene_manager;    // ← SAME as GameEngine
  DisplayManager m_display_manager; // ← SAME as GameEngine
  
  void TickFullEngine() {
    // Mirrors GameEngine::UpdateSystems() EXACTLY
    m_game_resources.mouse_position = ...;
    m_game_resources.event_handler.PreloadEvents(...);
    m_game_resources.event_handler.ProcessWaitingRoomEventBus();
    m_game_resources.event_handler.UpateSubscribersFromGlobalEventBus();
    m_scene_manager.UpdateSceneManager();
    m_display_manager.CallRenderCycle();
    m_game_resources.event_handler.TickGlobalEventBus();
  }
  
  void TickSceneManagerLevel() {
    // Mirrors SceneManager::UpdateSceneManager()
    events::ProcessEventBusCycle(...);
    m_scene_manager.UpdateSceneManager();
  }
  
  void TickSceneLogicLevel() {
    // Just runs scene logic
    events::ProcessEventBusCycle(...);
    m_scene_manager.UpdateScenes();
  }
};
```

#### Unified FlatBuffers Structure for All Levels

All test levels use the **same FlatBuffers structures** for consistent data loading:

```fbs
// test_data.fbs - Unified structure for all test levels
table TestDataConfig {
  metadata: TestMetadata (required);
  
  // Level 1+: Entity data (shared with game SceneData)
  start_data_collection: DataCollection;
  expected_data_collection: DataCollection;
  
  // Level 2+: Simulation configuration
  simulation_data: SimulationData;
  
  // Level 3-5: Scene and logic configuration
  scene_type: SceneType;
  logic_config: LogicConfig;
  
  // Event/input sequences (all levels)
  event_sequence: [EventPacketData];
  input_sequence: [InputEventData];
  tick_snapshots: [TickSnapshot];
}

// LogicConfig - specifies which logic to run
table LogicConfig {
  // Level 3: Single logic class
  logic_class_name: string;      // e.g., "UICollisionLogic"
  
  // Level 4: Logic collection
  logic_types: [LogicType];      // e.g., [Action, Collision]
  
  // Level 5: Full scene - use SceneType above
}

// DataCollection - identical to game's SceneData entity structure
table DataCollection {
  entity_collection: EntityCollection;   // Same as SceneData
  event_bus: EventBusData;
  waiting_room: EventBusData;
}
```

#### TestEngine Design

The TestEngine allows two modes of logic execution:
1. **Custom Mode (Levels 1-3)**: Mix and match Logic classes and free functions for incremental testing
2. **Standard Mode (Levels 4-5)**: Use the **same execution path as GameEngine** - hard-baked into the abstract Engine base class

```cpp
/////////////////////////////////////////////////
/// @class Engine
/// @brief Abstract base class with standard execution methods
///
/// Standard execution (LogicCollection, FullScene) is defined here
/// so both GameEngine and TestEngine use identical paths.
/////////////////////////////////////////////////
class Engine {
protected:
  GameResources m_game_resources;
  GameContext m_game_context;
  LogicCollection m_logic_collection;
  
  /////////////////////////////////////////////////
  /// @brief Standard logic execution - SAME for game and test
  /// @note This is on the base class so it's identical for both
  /////////////////////////////////////////////////
  void ExecuteStandardLogicTick() {
    // Event handling (standard - always same order)
    m_game_resources.event_handler.ProcessWaitingRoomEventBus();
    m_game_resources.event_handler.ClearSubscribers();
    m_game_resources.event_handler.UpdateSubscribersFromGlobalEventBus();
    m_game_resources.event_handler.TickGlobalEventBus();
    
    // Logic execution (standard - Action → Collision → Render)
    ExecuteLogicOfType(LogicType::Action);
    ExecuteLogicOfType(LogicType::Collision);
    ExecuteLogicOfType(LogicType::Render);
  }
  
  /////////////////////////////////////////////////
  /// @brief Execute all Logic of a specific type
  /////////////////////////////////////////////////
  void ExecuteLogicOfType(LogicType type) {
    if (m_logic_collection.find(type) != m_logic_collection.end()) {
      for (auto& logic : m_logic_collection[type]) {
        logic->RunLogic();
      }
    }
  }
  
  virtual void ExecuteTick() = 0;
  
public:
  void Run(size_t num_ticks = 0);
};

/////////////////////////////////////////////////
/// @class GameEngine
/// @brief Production game engine - uses standard execution
/////////////////////////////////////////////////
class GameEngine : public Engine {
protected:
  void ExecuteTick() override {
    // Capture real SFML input...
    UpdateGameResources(m_game_resources);
    m_game_resources.event_handler.PreloadEvents(m_game_resources.game_window);
    
    // Use STANDARD execution from base class
    ExecuteStandardLogicTick();
    
    // Render
    m_display_manager.CallRenderCycle();
  }
};

/////////////////////////////////////////////////
/// @class TestEngine
/// @brief Test engine with custom and standard execution modes
///
/// Custom Mode: Mix and match Logic classes + free functions
/// Standard Mode: Uses identical path as GameEngine
/////////////////////////////////////////////////
class TestEngine : public Engine {
public:
  /////////////////////////////////////////////////
  /// @brief Execution level for this test run
  /////////////////////////////////////////////////
  enum class ExecutionLevel {
    EntityOnly,      // Level 1: Just load entities (no logic)
    Custom,          // Level 2-3: Mix Logic classes + free functions
    LogicCollection, // Level 4: Uses STANDARD path from Engine base
    FullScene        // Level 5: Uses STANDARD path from Engine base
  };

private:
  const TestDataConfig *m_test_config;
  ExecutionLevel m_execution_level = ExecutionLevel::FullScene;
  
  /////////////////////////////////////////////////
  /// @brief Custom execution steps - mix Logic classes and free functions
  ///
  /// For incremental testing, you can add any combination:
  /// - Logic class instances
  /// - Free functions
  /// - Lambdas
  /// Steps execute in order added.
  /////////////////////////////////////////////////
  std::vector<std::variant<
    std::unique_ptr<Logic>,
    std::function<void(SceneContext&)>
  >> m_custom_steps;
  
public:
  /////////////////////////////////////////////////
  /// @brief Add a Logic class to custom execution
  /////////////////////////////////////////////////
  template<typename TLogic>
  TestEngine& AddLogic() {
    m_execution_level = ExecutionLevel::Custom;
    m_custom_steps.push_back(std::make_unique<TLogic>(GetLogicContext()));
    return *this;
  }
  
  /////////////////////////////////////////////////
  /// @brief Add a free function to custom execution
  /////////////////////////////////////////////////
  TestEngine& AddFunction(std::function<void(SceneContext&)> func) {
    m_execution_level = ExecutionLevel::Custom;
    m_custom_steps.push_back(std::move(func));
    return *this;
  }
  
  /////////////////////////////////////////////////
  /// @brief Set to standard LogicCollection mode (uses Engine base path)
  /////////////////////////////////////////////////
  void UseLogicCollection(std::vector<LogicType> types) {
    m_execution_level = ExecutionLevel::LogicCollection;
    // Configure m_logic_collection from scene...
  }
  
  /////////////////////////////////////////////////
  /// @brief Set to full scene mode (uses Engine base path)
  /////////////////////////////////////////////////
  void UseFullScene(SceneType scene_type) {
    m_execution_level = ExecutionLevel::FullScene;
    // Configure m_logic_collection from scene...
  }
  
protected:
  void ExecuteTick() override {
    switch (m_execution_level) {
      case ExecutionLevel::EntityOnly:
        // No logic execution - just event handling
        m_game_resources.event_handler.ProcessWaitingRoomEventBus();
        m_game_resources.event_handler.ClearSubscribers();
        m_game_resources.event_handler.UpdateSubscribersFromGlobalEventBus();
        m_game_resources.event_handler.TickGlobalEventBus();
        break;
        
      case ExecutionLevel::Custom:
        // Event handling
        m_game_resources.event_handler.ProcessWaitingRoomEventBus();
        m_game_resources.event_handler.ClearSubscribers();
        m_game_resources.event_handler.UpdateSubscribersFromGlobalEventBus();
        m_game_resources.event_handler.TickGlobalEventBus();
        
        // Execute custom steps (mixed Logic + functions)
        for (auto& step : m_custom_steps) {
          std::visit([this](auto& s) {
            using T = std::decay_t<decltype(s)>;
            if constexpr (std::is_same_v<T, std::unique_ptr<Logic>>) {
              s->RunLogic();
            } else {
              s(m_scene_context);
            }
          }, step);
        }
        break;
        
      case ExecutionLevel::LogicCollection:
      case ExecutionLevel::FullScene:
        // Use STANDARD execution from Engine base class
        // Identical to GameEngine path!
        ExecuteStandardLogicTick();
        break;
    }
  }
};
```

#### Usage Examples

**Level 1: Entity-Only Test (Just Load Data)**
```cpp
TEST_CASE("Entity configuration loads correctly", "[unit][entity]") {
  auto configs = load_test_data_configs();
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  TestEngine engine(config);
  // Default is EntityOnly when no logic is configured
  engine.ConfigureFromData();
  
  // Run 0 ticks - just inspect loaded entities
  engine.Run(0);
  
  // Assert entity state
  auto& pool = engine.GetEntityManager().GetEntityMemoryPool();
  REQUIRE(pool.size() == 10);
}
```

**Custom Mode: Mix Logic Classes and Free Functions (Incremental Testing)**
```cpp
TEST_CASE("Building up logic incrementally", "[unit][custom]") {
  TestEngine engine(config);
  
  // Mix and match - fluent API allows chaining
  engine
    .AddLogic<UICollisionLogic>()           // Logic class
    .AddFunction([](SceneContext& ctx) {    // Free function
      // Custom validation between logics
      auto& pool = ctx.entity_pool;
      // ... assertions ...
    })
    .AddLogic<UIActionLogic>()              // Another Logic class
    .AddFunction(ValidateActionResults);    // Named function
    
  engine.ConfigureFromData();
  engine.Run(1);  // All steps execute in order
}
```

**Custom Mode: Single Logic Class Test**
```cpp
TEST_CASE("UICollisionLogic detects overlap", "[unit][logic]") {
  TestEngine engine(config);
  engine.AddLogic<UICollisionLogic>();  // Single logic - still custom mode
  
  engine.ConfigureFromData();
  engine.Run(1);
  
  // Assert collision results
  auto& pool = engine.GetEntityManager().GetEntityMemoryPool();
  auto& ui = emp_helpers::GetComponent<CUserInterface>(0, pool);
  REQUIRE(ui.m_collision_detected == true);
}
```

**Standard Mode: Logic Collection (Uses Engine Base Class Path)**
```cpp
TEST_CASE("Action logics process input correctly", "[integration][logic]") {
  TestEngine engine(config);
  // UseLogicCollection switches to STANDARD mode
  // This uses ExecuteStandardLogicTick() from Engine base class
  engine.UseLogicCollection({LogicType::Action});
  
  engine.ConfigureFromData();
  engine.Run(3);  // 3 ticks - IDENTICAL to GameEngine path
  
  // Assert cumulative state
}
```

**Standard Mode: Full Scene (Uses Engine Base Class Path)**
```cpp
TEST_CASE("Title scene workflow", "[integration][scene]") {
  TestEngine engine(config);
  // UseFullScene switches to STANDARD mode
  // This uses ExecuteStandardLogicTick() from Engine base class
  engine.UseFullScene(SceneType::SceneType_TITLE);
  
  engine.ConfigureFromData();
  engine.Run(10);  // IDENTICAL execution path to GameEngine
  
  // Assert final scene state
}
```

#### Key Design Principles

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                  EXECUTION MODES: CUSTOM vs STANDARD                         │
└─────────────────────────────────────────────────────────────────────────────┘

CUSTOM MODE (TestEngine only - for incremental testing):
┌───────────────────────────────────────────────────────────────────────────┐
│ engine.AddLogic<A>().AddFunction(f).AddLogic<B>()                         │
│                                                                            │
│   Execution order: A → f → B (in order added)                              │
│   Flexibility: Mix Logic classes, free functions, lambdas                  │
│   Use case: Building up tests incrementally                                │
└───────────────────────────────────────────────────────────────────────────┘

STANDARD MODE (Uses Engine base class - IDENTICAL to GameEngine):
┌───────────────────────────────────────────────────────────────────────────┐
│ engine.UseLogicCollection({Action, Collision})                            │
│ engine.UseFullScene(SceneType::Title)                                     │
│                                                                            │
│   Calls: Engine::ExecuteStandardLogicTick()                                │
│   Execution order: Action → Collision → Render (always)                    │
│   Use case: Integration tests that MUST match game behavior                │
│                                                                            │
│   ┌────────────────────────────────────────────────────────────────────┐  │
│   │ GameEngine::ExecuteTick() and TestEngine::ExecuteTick() both call  │  │
│   │ Engine::ExecuteStandardLogicTick() - SAME CODE PATH!               │  │
│   └────────────────────────────────────────────────────────────────────┘  │
└───────────────────────────────────────────────────────────────────────────┘
```

#### Data Reuse Across Levels

The key benefit is that **all levels use identical FlatBuffers structures**:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                     FLATBUFFERS STRUCTURE REUSE                              │
└─────────────────────────────────────────────────────────────────────────────┘

Game Loading:
  SceneData (from data/scene/{scene}.bin)
    └─► entity_collection: EntityCollection
        └─► entities: [EntityData]
            └─► c_user_interface: UserInterfaceData
            └─► c_grimoire_machina: GrimoireMachinaData
            
Test Loading (ALL LEVELS):
  TestDataConfig (from tests/*/data/*.test_data.bin)
    └─► start_data_collection: DataCollection
        └─► entity_collection: EntityCollection   ◄── SAME STRUCTURE!
            └─► entities: [EntityData]
                └─► c_user_interface: UserInterfaceData
                └─► c_grimoire_machina: GrimoireMachinaData
                
Shared Loading Path:
  ┌──────────────────────────┐
  │    FlatbuffersConfigurator    │
  ├──────────────────────────┤
  │ ConfigureEntitiesFromCollection(pool, collection)  │
  │   └─ Works identically for game and test data!     │
  └──────────────────────────┘
```

**Benefits:**
1. **Consistent data format**: Game and test JSON/binary use identical schemas
2. **Single configuration path**: `FlatbuffersConfigurator` handles both
3. **Easy test data creation**: Copy game data, modify for test scenario
4. **Validation at all levels**: Same data → same loading → predictable behavior

---

## Implementation Plan

### Overview: Three-Stage Approach

The implementation follows a three-stage approach to minimize disruption:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    STAGED IMPLEMENTATION APPROACH                            │
└─────────────────────────────────────────────────────────────────────────────┘

STAGE 1: NON-DISRUPTIVE ADDITIONS
─────────────────────────────────
│ Add new code that doesn't touch existing implementations
│ - New interfaces (IEntityDataSource, ILogicStep)
│ - New helper functions
│ - New FlatBuffers fields (additive)
│ - Documentation and tests for new code
│
│ ✓ Existing code compiles and works unchanged
│ ✓ New code can be developed and tested in isolation
│ ✓ Zero risk to production

STAGE 2: SIDE-BY-SIDE IMPLEMENTATION
───────────────────────────────────
│ Add new Engine hierarchy alongside existing GameEngine
│ - Engine base class (new)
│ - TestEngine (new, uses Engine)
│ - GameEngine updated to derive from Engine (minimal changes)
│ - New tick infrastructure exists alongside old test harness
│
│ ✓ Both old and new approaches work simultaneously
│ ✓ Can A/B test behavior
│ ✓ Gradual migration possible

STAGE 3: MIGRATION
──────────────────
│ Remove old implementations, keep only new approach
│ - Remove TestFixture, tick_executor, simulation_runner
│ - Remove duplicate code paths
│ - Consolidate to single implementation
│ - Update all documentation
│
│ ✓ Clean, unified codebase
│ ✓ Single source of truth
```

---

### Stage 1: Non-Disruptive Additions (4-5 tasks)

These changes can be made without affecting any existing code. They introduce new concepts that will be used later.

#### Task 1.1: Create IEntityDataSource Interface

**Files to create:**
- `src/data_handlers/IEntityDataSource.h`

**Purpose:** Define interface for entity data loading that both game and test can use.

```cpp
// IEntityDataSource.h
class IEntityDataSource {
public:
  virtual ~IEntityDataSource() = default;
  virtual const EntityCollection* GetEntityCollection() const = 0;
  virtual size_t GetEntityCount() const = 0;
};
```

**Why non-disruptive:** 
- New file, doesn't modify anything
- Pure interface with no implementation

**Dependencies:** None

**Estimated effort:** 1-2 hours

---

#### Task 1.2: Create ILogicStep Interface

**Files to create:**
- `src/logic/ILogicStep.h`

**Purpose:** Define interface for execution steps (Logic classes, free functions, lambdas).

```cpp
// ILogicStep.h
class ILogicStep {
public:
  virtual ~ILogicStep() = default;
  virtual void Execute(SceneContext& ctx) = 0;
  virtual std::string GetName() const = 0;
};

// Implementations
template<typename TLogic>
class LogicClassStep : public ILogicStep { ... };

class FunctionStep : public ILogicStep { ... };
```

**Why non-disruptive:**
- New files only
- Doesn't touch existing Logic class hierarchy

**Dependencies:** None

**Estimated effort:** 2-3 hours

---

#### Task 1.3: Add EntityCollection to Game Data Schema (Additive)

**Files to modify:**
- `src/flatbuffers_headers/scene_data.fbs` - Add EntityCollection field if not present

**Purpose:** Ensure game scene data uses same EntityCollection structure as test data.

```fbs
// scene_data.fbs - Additive change only
table SceneData {
  scene_type: SceneType;
  entities: [EntityData];  // Existing
  entity_collection: EntityCollection;  // New (optional, for unified loading)
}
```

**Why non-disruptive:**
- Additive FlatBuffers change (optional field)
- Existing code ignores new field
- New code can use either path

**Dependencies:** None

**Estimated effort:** 1 hour

---

#### Task 1.4: Create EventHandlerTick Free Function

**Files to create:**
- `src/events/event_handler_tick.h`
- `src/events/event_handler_tick.cpp`

**Purpose:** Extract consolidated event handling into a reusable free function.

```cpp
// event_handler_tick.h
namespace steamrot::events {

void ProcessEventBusCycle(EventHandler& handler);
// Calls in order:
//   ProcessWaitingRoomEventBus()
//   ClearSubscribers()
//   UpdateSubscribersFromGlobalEventBus()
//   TickGlobalEventBus()

}
```

**Why non-disruptive:**
- New files only
- Wraps existing EventHandler methods
- Existing code doesn't need to use this yet

**Dependencies:** None

**Estimated effort:** 1-2 hours

---

#### Task 1.5: Create Test Data Loading Helpers

**Files to create:**
- `tests/harness/test_data_loader.h`
- `tests/harness/test_data_loader.cpp`

**Purpose:** Provide path-based test data loading for CMake template approach.

```cpp
// test_data_loader.h
namespace steamrot::tests {

std::expected<std::vector<const TestDataConfig*>, FailInfo>
LoadTestDataConfigsFromPath(const std::filesystem::path& data_dir);

}
```

**Why non-disruptive:**
- New files only
- Existing test_data_harness.cpp unchanged
- Tests can use either old or new loading approach

**Dependencies:** None

**Estimated effort:** 2-3 hours

---

### Stage 2: Side-by-Side Implementation (5-6 tasks)

These changes add new implementations alongside existing code. Both old and new approaches work during this stage.

#### Task 2.1: Create Abstract Engine Base Class

**Files to create:**
- `src/systems/Engine.h`
- `src/systems/Engine.cpp`

**Purpose:** Abstract base class for unified resource management.

```cpp
// Engine.h
class Engine {
protected:
  GameResources m_game_resources;
  GameContext m_game_context;
  
  virtual std::expected<std::monostate, FailInfo> ConfigureFromData() = 0;
  virtual void ExecuteTick() = 0;
  
  // Standard logic tick - shared by GameEngine and TestEngine (Standard Mode)
  void ExecuteStandardLogicTick();
  
public:
  virtual ~Engine() = default;
  std::expected<std::monostate, FailInfo> StartUp();
  void Run(size_t num_ticks = 0);
  
  GameResources& GetGameResources() { return m_game_resources; }
  GameContext& GetGameContext() { return m_game_context; }
};
```

**Why side-by-side:**
- New files, doesn't modify GameEngine yet
- Can be developed and tested in isolation

**Dependencies:** Task 1.4 (EventHandlerTick)

**Estimated effort:** 4-6 hours

---

#### Task 2.2: Modify GameEngine to Derive from Engine

**Files to modify:**
- `src/systems/GameEngine.h`
- `src/systems/GameEngine.cpp`

**Purpose:** GameEngine becomes a derived class while maintaining all existing behavior.

```cpp
// GameEngine.h - Before
class GameEngine {
  GameResources m_game_resources;
  GameContext m_game_context;
  SceneManager m_scene_manager;
  // ...
};

// GameEngine.h - After
class GameEngine : public Engine {
  SceneManager m_scene_manager;
  DisplayManager m_display_manager;
protected:
  void ExecuteTick() override;
  std::expected<std::monostate, FailInfo> ConfigureFromData() override;
};
```

**Why side-by-side:**
- Minimal changes to GameEngine (just inheritance + remove duplicated members)
- Behavior unchanged - same methods called in same order
- All existing game code continues to work

**Key principle:** Move members to Engine base, keep behavior identical.

**Dependencies:** Task 2.1 (Engine base class)

**Estimated effort:** 3-4 hours

---

#### Task 2.3: Create TestEngine Derived Class

**Files to create:**
- `tests/harness/TestEngine.h`
- `tests/harness/TestEngine.cpp`

**Purpose:** New test engine that derives from Engine base class.

```cpp
// TestEngine.h
class TestEngine : public Engine {
public:
  enum class Mode { Custom, Standard };
  
  // Custom mode: mix and match
  TestEngine& AddLogic(std::unique_ptr<ILogicStep> step);
  template<typename TLogic> TestEngine& AddLogic();
  TestEngine& AddFunction(std::function<void(SceneContext&)> func);
  
  // Standard mode: uses Engine base
  void UseLogicCollection(std::vector<LogicType> types);
  void UseFullScene(SceneType scene_type);
  
  // Results access
  EntityMemoryPool& GetEntityPool();
  
protected:
  void ExecuteTick() override;
  std::expected<std::monostate, FailInfo> ConfigureFromData() override;

private:
  const TestDataConfig* m_test_config;
  std::vector<std::unique_ptr<ILogicStep>> m_custom_steps;
  Mode m_mode = Mode::Custom;
};
```

**Why side-by-side:**
- New files only
- TestFixture continues to work with old test harness
- New tests can use TestEngine, old tests use TestFixture

**Dependencies:** Task 2.1, Task 1.2, Task 1.1

**Estimated effort:** 6-8 hours

---

#### Task 2.4: Create Data Source Implementations

**Files to create:**
- `src/data_handlers/DefaultSceneDataSource.h`
- `src/data_handlers/DefaultSceneDataSource.cpp`
- `tests/harness/TestDataSource.h`

**Purpose:** Implement IEntityDataSource for game and test contexts.

```cpp
// DefaultSceneDataSource.h
class DefaultSceneDataSource : public IEntityDataSource {
public:
  explicit DefaultSceneDataSource(SceneType scene_type);
  const EntityCollection* GetEntityCollection() const override;
  size_t GetEntityCount() const override;
private:
  FlatbuffersDataLoader m_loader;
  SceneType m_scene_type;
};

// TestDataSource.h
class TestDataSource : public IEntityDataSource {
public:
  explicit TestDataSource(const TestDataConfig* config);
  const EntityCollection* GetEntityCollection() const override;
  size_t GetEntityCount() const override;
private:
  const TestDataConfig* m_config;
};
```

**Why side-by-side:**
- New implementations of IEntityDataSource
- FlatbuffersConfigurator can accept either implementation
- Existing loading code unchanged

**Dependencies:** Task 1.1 (IEntityDataSource), Task 1.3 (EntityCollection schema)

**Estimated effort:** 3-4 hours

---

#### Task 2.5: Add CMake Template for Data-Driven Tests

**Files to create:**
- `tests/cmake/data_driven_test.cpp.in`
- `tests/cmake/DataDrivenTests.cmake`

**Purpose:** CMake infrastructure for auto-generated test runners.

```cmake
# DataDrivenTests.cmake
function(add_data_driven_test)
  cmake_parse_arguments(ARG "" "NAME;DATA_DIR;TAG" "" ${ARGN})
  configure_file(
    ${CMAKE_SOURCE_DIR}/tests/cmake/data_driven_test.cpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/${ARG_NAME}_test.cpp
    @ONLY
  )
  add_executable(test_${ARG_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${ARG_NAME}_test.cpp)
  # ...
endfunction()
```

**Why side-by-side:**
- New CMake infrastructure
- Existing tests unchanged
- New tests can use add_data_driven_test()

**Dependencies:** Task 1.5 (test_data_loader)

**Estimated effort:** 2-3 hours

---

#### Task 2.6: Write Migration Tests

**Files to create:**
- `tests/migration/engine_comparison.test.cpp`
- `tests/migration/data_loading_comparison.test.cpp`

**Purpose:** Tests that verify old and new approaches produce identical results.

```cpp
// engine_comparison.test.cpp
TEST_CASE("GameEngine via Engine base matches direct GameEngine", "[migration]") {
  // Run same scenario through both paths
  // Assert identical results
}

TEST_CASE("TestEngine matches TestFixture results", "[migration]") {
  // Run same test data through both
  // Assert entity pool states match
}
```

**Why side-by-side:**
- Proves behavioral equivalence before migration
- Catches regressions early
- Documents expected behavior

**Dependencies:** Task 2.2, Task 2.3

**Estimated effort:** 4-6 hours

---

### Stage 3: Migration (5-6 tasks)

These changes remove old implementations and consolidate to the new approach.

#### Task 3.1: Migrate Existing Tests to TestEngine

**Files to modify:**
- All files in `tests/` that use TestFixture

**Purpose:** Update existing tests to use TestEngine instead of TestFixture.

```cpp
// Before
TestFixture fixture(config);
fixture.Run();
auto result = fixture.GetResult();

// After
TestEngine engine(config);
engine.ConfigureFromData();
engine.Run(1);
auto& pool = engine.GetEntityPool();
```

**Migration strategy:**
1. Start with simple tests (entity-only)
2. Progress to logic tests
3. End with complex integration tests

**Dependencies:** Stage 2 complete, Task 2.6 passing

**Estimated effort:** 8-12 hours (depends on test count)

---

#### Task 3.2: Remove TestFixture and Old Test Harness

**Files to delete:**
- `tests/harness/TestFixture.h`
- `tests/harness/TestFixture.cpp`
- `tests/harness/tick_executor.h`
- `tests/harness/tick_executor.cpp`
- `tests/harness/simulation_runner.h`
- `tests/harness/simulation_runner.cpp`

**Purpose:** Remove deprecated test infrastructure.

**Dependencies:** Task 3.1 complete (all tests migrated)

**Estimated effort:** 1-2 hours

---

#### Task 3.3: Consolidate FlatbuffersConfigurator

**Files to modify:**
- `src/entity/FlatbuffersConfigurator.h`
- `src/entity/FlatbuffersConfigurator.cpp`

**Purpose:** Remove duplicate loading paths, use only IEntityDataSource.

```cpp
// Before: Multiple loading methods
ConfigureEntitiesFromDefaultData(pool);
ConfigureEntitiesFromTestData(pool, config);

// After: Single unified method
ConfigureEntities(pool, IEntityDataSource& source);
```

**Dependencies:** Task 3.1 (all code using new data sources)

**Estimated effort:** 2-3 hours

---

#### Task 3.4: Rename TestContext to TestMetadataContext

**Files to modify:**
- `tests/harness/test_context.h` (rename)
- All files referencing TestContext

**Purpose:** Eliminate naming confusion with GameContext/SceneContext.

**Migration strategy:**
1. Create TestMetadataContext as alias
2. Mark TestContext as deprecated
3. Update all references
4. Remove alias

**Dependencies:** None (can be done anytime in Stage 3)

**Estimated effort:** 1-2 hours

---

#### Task 3.5: Update Documentation

**Files to modify:**
- `documentation/testing/TEST_DATA_CONFIGURATION.md`
- `documentation/architecture/GAME_LOOP.md`
- `documentation/testing/TESTING_HARNESS_LOOP.md`
- `.github/copilot-instructions.md`
- `README.md`

**Purpose:** Document new unified architecture.

**Key updates:**
- Remove TestFixture documentation
- Add TestEngine documentation
- Update architecture diagrams
- Add migration guide for existing tests

**Dependencies:** Stage 3 changes complete

**Estimated effort:** 3-4 hours

---

#### Task 3.6: Final Cleanup

**Purpose:** Remove any remaining deprecated code, update comments, clean up.

**Tasks:**
- Remove TODO comments referencing old approach
- Clean up unused includes
- Update CMakeLists.txt files
- Run full test suite
- Update copilot-instructions.md workflows

**Dependencies:** All other Stage 3 tasks

**Estimated effort:** 2-3 hours

---

### Implementation Timeline

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         IMPLEMENTATION TIMELINE                              │
└─────────────────────────────────────────────────────────────────────────────┘

Week 1-2: Stage 1 (Non-Disruptive Additions)
──────────────────────────────────────────────
├─ Task 1.1: IEntityDataSource interface
├─ Task 1.2: ILogicStep interface
├─ Task 1.3: EntityCollection schema (additive)
├─ Task 1.4: EventHandlerTick free function
└─ Task 1.5: Test data loading helpers

Week 3-4: Stage 2 (Side-by-Side Implementation)
──────────────────────────────────────────────────
├─ Task 2.1: Engine base class
├─ Task 2.2: GameEngine derives from Engine
├─ Task 2.3: TestEngine derived class
├─ Task 2.4: Data source implementations
├─ Task 2.5: CMake template for tests
└─ Task 2.6: Migration tests (prove equivalence)

Week 5-6: Stage 3 (Migration)
─────────────────────────────
├─ Task 3.1: Migrate tests to TestEngine
├─ Task 3.2: Remove TestFixture and old harness
├─ Task 3.3: Consolidate FlatbuffersConfigurator
├─ Task 3.4: Rename TestContext
├─ Task 3.5: Update documentation
└─ Task 3.6: Final cleanup

Total estimated effort: ~50-70 hours over 6 weeks
```

---

### Rollback Strategy

If issues are discovered at any stage:

**Stage 1:** Simply delete new files - no existing code affected.

**Stage 2:** GameEngine continues to work. TestFixture continues to work. New code can be disabled or removed.

**Stage 3:** Rollback by reverting commits. Migration tests in Task 2.6 ensure we catch issues before removal.

---

## Benefits

1. **Consistency**: Same event ordering in game and tests reduces behavioral differences
2. **Maintainability**: Common code reduces duplication and potential for drift
3. **Flexibility**: Logic provider interface allows custom logic injection
4. **Clarity**: Renamed TestMetadataContext removes confusion
5. **Testability**: Tests can more accurately simulate game behavior
6. **Granular Testing**: TestEngine supports 5 levels from entity-only to full scene
7. **Data Reuse**: Identical FlatBuffers structures for game and all test levels

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
