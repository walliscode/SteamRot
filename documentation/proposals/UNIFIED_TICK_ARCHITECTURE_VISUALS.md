# Unified Tick Architecture - Visual Diagrams

[← Back to Main Proposal](UNIFIED_TICK_ARCHITECTURE.md)

This document provides visual diagrams for the Unified Tick Architecture proposal.

---

## Current Architecture Comparison

### Side-by-Side: Game Loop vs Test Harness

```
┌─────────────────────────────────────┐    ┌─────────────────────────────────────┐
│         GAME ENGINE LOOP            │    │         TEST HARNESS LOOP           │
│        (UpdateSystems)              │    │       (ExecuteSingleTick)           │
└─────────────────────────────────────┘    └─────────────────────────────────────┘
                 │                                          │
                 ▼                                          ▼
┌─────────────────────────────────────┐    ┌─────────────────────────────────────┐
│ 1. UpdateGameResources              │    │ 1. ExecuteInputEventsForTick        │
│    └─ Update mouse position         │    │    └─ Inject simulated inputs       │
└─────────────────────────────────────┘    └─────────────────────────────────────┘
                 │                                          │
                 ▼                                          ▼
┌─────────────────────────────────────┐    ┌─────────────────────────────────────┐
│ 2. PreloadEvents                    │    │ 2. ExecuteEventsForTick             │
│    └─ Capture SFML window events    │    │    └─ Inject simulated events       │
└─────────────────────────────────────┘    └─────────────────────────────────────┘
                 │                                          │
                 ▼                                          ▼
┌─────────────────────────────────────┐    ┌─────────────────────────────────────┐
│ 3. ProcessWaitingRoomEventBus       │ == │ 3. ProcessWaitingRoomEventBus       │
│    └─ Move to global bus            │    │    └─ Move to global bus            │
└─────────────────────────────────────┘    └─────────────────────────────────────┘
                 │                                          │
                 ▼                                          ▼
┌─────────────────────────────────────┐    ┌─────────────────────────────────────┐
│ 4. UpdateSubscribersFromGlobalBus   │ == │ 4. UpdateSubscribersFromGlobalBus   │
│    └─ Notify all subscribers        │    │    └─ Notify all subscribers        │
└─────────────────────────────────────┘    └─────────────────────────────────────┘
                 │                                          │
                 ▼                                          ▼
┌─────────────────────────────────────┐    ┌─────────────────────────────────────┐
│ 5. ProcessSubscriptions             │    │ (5. No subscription processing)     │
│    └─ GameEngine level              │    │                                     │
└─────────────────────────────────────┘    └─────────────────────────────────────┘
                 │                                          │
                 ▼                                          ▼
┌─────────────────────────────────────┐    ┌─────────────────────────────────────┐
│ 6. UpdateSceneManager               │    │ 5. ExecuteSimulation                │
│    ├─ ProcessSubscriptions          │    │    └─ Execute SimulationSteps       │
│    └─ UpdateScenes                  │    │        (Logic classes/functions)    │
│        └─ sAction→sCollision→sRender│    │                                     │
└─────────────────────────────────────┘    └─────────────────────────────────────┘
                 │                                          │
                 ▼                                          ▼
┌─────────────────────────────────────┐    ┌─────────────────────────────────────┐
│ 7. CallRenderCycle                  │    │ 6. CompareTickSnapshot              │
│    └─ Render to window              │    │    └─ Validate state                │
└─────────────────────────────────────┘    └─────────────────────────────────────┘
                 │                                          │
                 ▼                                          ▼
┌─────────────────────────────────────┐    ┌─────────────────────────────────────┐
│ 8. TickGlobalEventBus               │ == │ 7. TickGlobalEventBus               │
│    └─ Decrement event lifetimes     │    │    └─ Decrement event lifetimes     │
└─────────────────────────────────────┘    └─────────────────────────────────────┘

Legend: == indicates identical operations
```

### Key Differences Highlighted

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           DIVERGENCE POINTS                                  │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────┐         ┌─────────────────────────┐            │
│  │ INPUT SOURCE            │         │ INPUT SOURCE            │            │
│  │ ───────────────────     │         │ ───────────────────     │            │
│  │ SFML Window Events      │   vs    │ Simulated from JSON     │            │
│  │ (PreloadEvents)         │         │ (ExecuteInputEventsFor) │            │
│  └─────────────────────────┘         └─────────────────────────┘            │
│                                                                             │
│  ┌─────────────────────────┐         ┌─────────────────────────┐            │
│  │ LOGIC EXECUTION         │         │ LOGIC EXECUTION         │            │
│  │ ───────────────────     │         │ ───────────────────     │            │
│  │ Scene's LogicMap        │   vs    │ SimulationData steps    │            │
│  │ (sAction/sCollision/    │         │ (ExecuteSimulationStep) │            │
│  │  sRender)               │         │                         │            │
│  └─────────────────────────┘         └─────────────────────────┘            │
│                                                                             │
│  ┌─────────────────────────┐         ┌─────────────────────────┐            │
│  │ POST-LOGIC              │         │ POST-LOGIC              │            │
│  │ ───────────────────     │         │ ───────────────────     │            │
│  │ Render to window        │   vs    │ State validation        │            │
│  │ (CallRenderCycle)       │         │ (CompareTickSnapshot)   │            │
│  └─────────────────────────┘         └─────────────────────────┘            │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## Proposed Unified Architecture

### Hybrid Approach: Free Functions with TickContext

The chosen approach uses free functions with compile-time conditionals, encapsulated by a `TickContext` resource class.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    HYBRID FREE FUNCTION ARCHITECTURE                         │
└─────────────────────────────────────────────────────────────────────────────┘

                              ┌─────────────────────────┐
                              │      TickContext        │
                              │   (Resource Container)  │
                              ├─────────────────────────┤
                              │ • game_resources        │
                              │ • scene_context         │
                              ├─────────────────────────┤
                              │ #ifndef TEST_BUILD      │
                              │ • scene_manager         │
                              │ • display_manager       │
                              ├─────────────────────────┤
                              │ #ifdef TEST_BUILD       │
                              │ • test_config           │
                              │ • current_tick          │
                              │ • test_fixture          │
                              └───────────┬─────────────┘
                                          │
                                          │ passed to
                                          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                         namespace tick                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │  void ExecuteTick(TickContext &ctx)  ◄── Main entry point          │   │
│  │  {                                                                   │   │
│  │      PreTick(ctx);              // Input capture/injection          │   │
│  │      ProcessEventBuses(ctx);    // Shared - no conditionals         │   │
│  │      ProcessSubscriptions(ctx); // Game-only subscriptions          │   │
│  │      ExecuteLogic(ctx);         // Scene logic or simulation        │   │
│  │      PostLogic(ctx);            // Render or validation             │   │
│  │      TickEventBus(ctx);         // Shared - no conditionals         │   │
│  │  }                                                                   │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  Individual functions (all take TickContext&):                              │
│  ┌──────────────────────┐  ┌──────────────────────┐  ┌────────────────────┐│
│  │ PreTick()            │  │ ProcessEventBuses()  │  │ ProcessSubs()      ││
│  │ ─────────────────    │  │ ─────────────────    │  │ ───────────────    ││
│  │ #ifdef TEST_BUILD    │  │ (no conditionals)    │  │ #ifndef TEST_BUILD ││
│  │   InjectInputs       │  │ ProcessWaitingRoom   │  │   SceneManager     ││
│  │   InjectEvents       │  │ UpdateSubscribers    │  │   subscriptions    ││
│  │ #else                │  │                      │  │                    ││
│  │   UpdateResources    │  │                      │  │                    ││
│  │   PreloadEvents      │  │                      │  │                    ││
│  │ #endif               │  │                      │  │                    ││
│  └──────────────────────┘  └──────────────────────┘  └────────────────────┘│
│                                                                             │
│  ┌──────────────────────┐  ┌──────────────────────┐  ┌────────────────────┐│
│  │ ExecuteLogic()       │  │ PostLogic()          │  │ TickEventBus()     ││
│  │ ─────────────────    │  │ ─────────────────    │  │ ───────────────    ││
│  │ #ifdef TEST_BUILD    │  │ #ifdef TEST_BUILD    │  │ (no conditionals)  ││
│  │   SimulationSteps    │  │   CompareSnapshot    │  │ TickGlobalEventBus ││
│  │ #else                │  │ #else                │  │                    ││
│  │   UpdateScenes       │  │   CallRenderCycle    │  │                    ││
│  │ #endif               │  │ #endif               │  │                    ││
│  └──────────────────────┘  └──────────────────────┘  └────────────────────┘│
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Compile-Time Conditionals

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    COMPILE-TIME CONFIGURATION                                │
└─────────────────────────────────────────────────────────────────────────────┘

CMakeLists.txt:
┌─────────────────────────────────────────────────────────────────────────────┐
│ # Game executable - no special defines                                      │
│ add_executable(steamrot main.cpp ...)                                       │
│                                                                             │
│ # Test executables - define STEAMROT_TEST_BUILD                             │
│ add_executable(test_harness ...)                                            │
│ target_compile_definitions(test_harness PRIVATE STEAMROT_TEST_BUILD)        │
└─────────────────────────────────────────────────────────────────────────────┘

Result:
┌───────────────────────────────┐         ┌───────────────────────────────┐
│    Game Build (steamrot)      │         │   Test Build (test_harness)   │
├───────────────────────────────┤         ├───────────────────────────────┤
│                               │         │                               │
│  PreTick():                   │         │  PreTick():                   │
│    UpdateGameResources()      │         │    ExecuteInputEventsForTick()│
│    PreloadEvents()            │         │    ExecuteEventsForTick()     │
│                               │         │                               │
│  ExecuteLogic():              │         │  ExecuteLogic():              │
│    UpdateScenes()             │         │    ExecuteSimulationSteps()   │
│                               │         │                               │
│  PostLogic():                 │         │  PostLogic():                 │
│    CallRenderCycle()          │         │    CompareTickSnapshot()      │
│                               │         │                               │
│  ┌───────────────────────┐   │         │   ┌───────────────────────┐   │
│  │ scene_manager member  │   │         │   │ test_config member    │   │
│  │ display_manager member│   │         │   │ current_tick member   │   │
│  │                       │   │         │   │ test_fixture member   │   │
│  └───────────────────────┘   │         │   └───────────────────────┘   │
│                               │         │                               │
└───────────────────────────────┘         └───────────────────────────────┘

Benefits:
✓ Wrong code paths are never compiled
✓ No runtime overhead for conditionals  
✓ Compiler can optimize each build independently
✓ Type-safe: test members don't exist in game build
```

### ExecuteTick() Flow (Both Builds)

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    tick::ExecuteTick(TickContext &ctx)                       │
│                    (Guaranteed Execution Order)                              │
└─────────────────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
        ┌───────────────────────────────────────────────────────┐
        │ 1. PreTick(ctx)                         [CONDITIONAL] │
        │    ┌────────────────────┐  ┌────────────────────────┐ │
        │    │ Game Build         │  │ Test Build             │ │
        │    │ • UpdateResources  │  │ • InjectInputs         │ │
        │    │ • PreloadEvents    │  │ • InjectEvents         │ │
        │    └────────────────────┘  └────────────────────────┘ │
        └───────────────────────────────────────────────────────┘
                                    │
                                    ▼
        ┌───────────────────────────────────────────────────────┐
        │ 2. ProcessEventBuses(ctx)                   [SHARED]  │
        │    └─ ProcessWaitingRoomEventBus()                    │
        │    └─ UpdateSubscribersFromGlobalEventBus()           │
        └───────────────────────────────────────────────────────┘
                                    │
                                    ▼
        ┌───────────────────────────────────────────────────────┐
        │ 3. ProcessSubscriptions(ctx)            [CONDITIONAL] │
        │    ┌────────────────────┐  ┌────────────────────────┐ │
        │    │ Game Build         │  │ Test Build             │ │
        │    │ • SceneManager     │  │ • (no-op)              │ │
        │    │   subscriptions    │  │                        │ │
        │    └────────────────────┘  └────────────────────────┘ │
        └───────────────────────────────────────────────────────┘
                                    │
                                    ▼
        ┌───────────────────────────────────────────────────────┐
        │ 4. ExecuteLogic(ctx)                    [CONDITIONAL] │
        │    ┌────────────────────┐  ┌────────────────────────┐ │
        │    │ Game Build         │  │ Test Build             │ │
        │    │ • UpdateScenes     │  │ • ExecuteSimulation    │ │
        │    │   (LogicMap)       │  │   (SimulationData)     │ │
        │    └────────────────────┘  └────────────────────────┘ │
        └───────────────────────────────────────────────────────┘
                                    │
                                    ▼
        ┌───────────────────────────────────────────────────────┐
        │ 5. PostLogic(ctx)                       [CONDITIONAL] │
        │    ┌────────────────────┐  ┌────────────────────────┐ │
        │    │ Game Build         │  │ Test Build             │ │
        │    │ • CallRenderCycle  │  │ • CompareSnapshot      │ │
        │    └────────────────────┘  └────────────────────────┘ │
        └───────────────────────────────────────────────────────┘
                                    │
                                    ▼
        ┌───────────────────────────────────────────────────────┐
        │ 6. TickEventBus(ctx)                          [SHARED]│
        │    └─ TickGlobalEventBus()                            │
        └───────────────────────────────────────────────────────┘
```

---

### Original Inheritance Approach (Not Chosen)

For reference, the originally proposed inheritance-based approach:

```
                    ┌─────────────────────────┐
                    │      TickExecutor       │
                    │     (Abstract Base)     │
                    ├─────────────────────────┤
                    │ # m_scene_context       │
                    │ # m_game_resources      │
                    ├─────────────────────────┤
                    │ + ExecuteTick()         │◄─── Template Method Pattern
                    │ # OnPreTick()           │     (virtual methods)
                    │ # OnProcessEvents()     │
                    │ # OnExecuteLogic()      │
                    │ # OnPostLogic()         │
                    └───────────┬─────────────┘
                                │
                ┌───────────────┴───────────────┐
                │                               │
                ▼                               ▼
┌───────────────────────────────┐   ┌───────────────────────────────┐
│     GameTickExecutor          │   │     TestTickExecutor          │
│   (inherits TickExecutor)     │   │   (inherits TickExecutor)     │
└───────────────────────────────┘   └───────────────────────────────┘

Not chosen because:
- Vtable overhead
- Both game and test code compiled together
- More boilerplate
```

---

## ILogicProvider Interface

### Class Diagram

```
                    ┌─────────────────────────┐
                    │     ILogicProvider      │
                    │      (Interface)        │
                    ├─────────────────────────┤
                    │ + ExecuteAction()       │
                    │ + ExecuteCollision()    │
                    │ + ExecuteRender()       │
                    │ + ExecuteMovement()     │
                    └───────────┬─────────────┘
                                │
                ┌───────────────┴───────────────┐
                │                               │
                ▼                               ▼
┌───────────────────────────────┐   ┌───────────────────────────────┐
│    SceneLogicProvider         │   │  SimulationLogicProvider      │
├───────────────────────────────┤   ├───────────────────────────────┤
│ - m_logic_map: LogicCollection│   │ - m_simulation_data           │
├───────────────────────────────┤   │ - m_scene_context             │
│ + ExecuteAction()             │   ├───────────────────────────────┤
│   └─ Run LogicType::Action    │   │ + ExecuteAction()             │
│                               │   │   └─ Run Action type steps    │
│ + ExecuteCollision()          │   │                               │
│   └─ Run LogicType::Collision │   │ + ExecuteCollision()          │
│                               │   │   └─ Run Collision type steps │
│ + ExecuteRender()             │   │                               │
│   └─ Run LogicType::Render    │   │ + ExecuteRender()             │
│                               │   │   └─ Run Render type steps    │
│ + ExecuteMovement()           │   │                               │
│   └─ Run LogicType::Movement  │   │ + ExecuteMovement()           │
└───────────────────────────────┘   │   └─ Run Movement type steps  │
                                    └───────────────────────────────┘
```

### Usage in Scene

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           Scene with ILogicProvider                         │
└─────────────────────────────────────────────────────────────────────────────┘

Current Implementation:
┌─────────────────────────────────────────────────────────────────────────────┐
│ class Scene {                                                               │
│   LogicCollection m_logic_map;  // Fixed at scene creation                  │
│                                                                             │
│   void sAction() {                                                          │
│     for (auto &logic : m_logic_map[LogicType::Action]) {                   │
│       logic->RunLogic();                                                    │
│     }                                                                       │
│   }                                                                         │
│ };                                                                          │
└─────────────────────────────────────────────────────────────────────────────┘

Proposed Implementation:
┌─────────────────────────────────────────────────────────────────────────────┐
│ class Scene {                                                               │
│   std::unique_ptr<ILogicProvider> m_logic_provider;  // Injectable!        │
│                                                                             │
│   void SetLogicProvider(std::unique_ptr<ILogicProvider> provider) {        │
│     m_logic_provider = std::move(provider);                                 │
│   }                                                                         │
│                                                                             │
│   void sAction() {                                                          │
│     m_logic_provider->ExecuteAction();                                      │
│   }                                                                         │
│ };                                                                          │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## IEntityDataSource Interface

### Class Diagram

```
                    ┌─────────────────────────┐
                    │   IEntityDataSource     │
                    │      (Interface)        │
                    ├─────────────────────────┤
                    │ + GetEntityCollection() │
                    │   → EntityCollection*   │
                    └───────────┬─────────────┘
                                │
                ┌───────────────┴───────────────┐
                │                               │
                ▼                               ▼
┌───────────────────────────────┐   ┌───────────────────────────────┐
│   DefaultSceneDataSource      │   │      TestDataSource           │
├───────────────────────────────┤   ├───────────────────────────────┤
│ - m_scene_type                │   │ - m_config: TestDataConfig*   │
│ - m_cached_data               │   ├───────────────────────────────┤
├───────────────────────────────┤   │ + GetEntityCollection()       │
│ + GetEntityCollection()       │   │   └─ return config->          │
│   └─ Load from data file      │   │      start_data_collection    │
│   └─ Return cached data       │   │      ->entity_collection()    │
└───────────────────────────────┘   └───────────────────────────────┘
```

### Data Flow Comparison

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        CURRENT DATA LOADING                                  │
└─────────────────────────────────────────────────────────────────────────────┘

Game Engine:
┌──────────────────┐    ┌──────────────────┐    ┌──────────────────┐
│ FlatbuffersDat-  │───▶│ EntityManager::  │───▶│ EntityMemory-    │
│ aLoader          │    │ ConfigureFrom-   │    │ Pool             │
│                  │    │ DefaultData      │    │                  │
└──────────────────┘    └──────────────────┘    └──────────────────┘

Test Harness:
┌──────────────────┐    ┌──────────────────┐    ┌──────────────────┐
│ TestDataConfig   │───▶│ Flatbuffers-     │───▶│ EntityMemory-    │
│ (start_data_     │    │ Configurator::   │    │ Pool             │
│  collection)     │    │ ConfigureFrom-   │    │                  │
│                  │    │ Collection       │    │                  │
└──────────────────┘    └──────────────────┘    └──────────────────┘


┌─────────────────────────────────────────────────────────────────────────────┐
│                        PROPOSED UNIFIED LOADING                              │
└─────────────────────────────────────────────────────────────────────────────┘

                              ┌──────────────────┐
                              │ IEntityDataSource│
                              └────────┬─────────┘
                                       │
                   ┌───────────────────┴───────────────────┐
                   │                                       │
                   ▼                                       ▼
        ┌──────────────────┐                    ┌──────────────────┐
        │ DefaultSceneData-│                    │ TestDataSource   │
        │ Source           │                    │                  │
        └────────┬─────────┘                    └────────┬─────────┘
                 │                                       │
                 │                                       │
                 └───────────────┬───────────────────────┘
                                 │
                                 ▼
                      ┌──────────────────┐
                      │ EntityConfigurer │  ◄── Single configuration point
                      │ (unified)        │
                      └────────┬─────────┘
                               │
                               ▼
                      ┌──────────────────┐
                      │ EntityMemoryPool │
                      └──────────────────┘
```

---

## Context Renaming

### Current State (Confusing)

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           CONTEXT STRUCTURES                                 │
│                          (Currently Confusing)                               │
└─────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────┐    ┌─────────────────────────┐
│      GameContext        │    │     SceneContext        │
├─────────────────────────┤    ├─────────────────────────┤
│ Purpose:                │    │ Purpose:                │
│ References to game-     │    │ References to scene-    │
│ level resources         │    │ level + game resources  │
├─────────────────────────┤    ├─────────────────────────┤
│ • game_window          │    │ • scene_entities        │
│ • event_handler        │    │ • archetypes            │
│ • mouse_position       │    │ • scene_texture         │
│ • loop_number          │    │ • game_window           │
│ • asset_manager        │    │ • asset_manager         │
│ • env_type             │    │ • event_handler         │
│ • game_resources       │    │ • mouse_position        │
└─────────────────────────┘    └─────────────────────────┘

┌─────────────────────────┐
│      TestContext        │  ◄── CONFUSING NAME!
├─────────────────────────┤      Different purpose than GameContext/SceneContext
│ Purpose:                │
│ Test metadata for       │
│ error messages          │
├─────────────────────────┤
│ • test_name            │
│ • description          │
│ • current_tick         │
│ • total_ticks          │
└─────────────────────────┘
```

### Proposed State (Clear)

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           CONTEXT STRUCTURES                                 │
│                            (Renamed for Clarity)                             │
└─────────────────────────────────────────────────────────────────────────────┘

RESOURCE CONTEXTS (hold references to resources):
┌─────────────────────────┐    ┌─────────────────────────┐
│      GameContext        │    │     SceneContext        │
│   (unchanged)           │    │   (unchanged)           │
└─────────────────────────┘    └─────────────────────────┘

METADATA CONTEXT (holds test information):
┌─────────────────────────────┐
│   TestMetadataContext       │  ◄── RENAMED! Clear it's metadata, not resources
├─────────────────────────────┤
│ Purpose:                    │
│ Test metadata for           │
│ error messages              │
├─────────────────────────────┤
│ • test_name                │
│ • description              │
│ • current_tick             │
│ • total_ticks              │
├─────────────────────────────┤
│ + FormatTestName()          │
│ + FormatTickInfo()          │
└─────────────────────────────┘
```

---

## Summary Diagram

```
┌─────────────────────────────────────────────────────────────────────────────┐
│              UNIFIED TICK ARCHITECTURE OVERVIEW (Hybrid Approach)            │
└─────────────────────────────────────────────────────────────────────────────┘

                              ┌─────────────────────────┐
                              │      TickContext        │
                              │  (Resource Container)   │
                              └───────────┬─────────────┘
                                          │
                                          │ passed to
                                          ▼
                        ┌─────────────────────────────────────┐
                        │       tick::ExecuteTick(ctx)        │
                        │   (Free function with ordering)     │
                        └───────────────┬─────────────────────┘
                                        │
                                        │ calls (in order)
            ┌───────────┬───────────────┼───────────────┬───────────┐
            ▼           ▼               ▼               ▼           ▼
     ┌──────────┐ ┌──────────┐   ┌──────────┐   ┌──────────┐ ┌──────────┐
     │ PreTick  │ │ Process- │   │ Process- │   │ Execute- │ │ PostLogic│
     │          │ │ EventBus │   │ Subs     │   │ Logic    │ │          │
     └────┬─────┘ └────┬─────┘   └────┬─────┘   └────┬─────┘ └────┬─────┘
          │            │              │              │            │
          │            │              │              │            │
     [CONDITIONAL]  [SHARED]    [CONDITIONAL]  [CONDITIONAL] [CONDITIONAL]
          │            │              │              │            │
    ┌─────┴─────┐      │        ┌─────┴─────┐  ┌─────┴─────┐ ┌─────┴─────┐
    │Game│Test │      │        │Game│(none)│  │Game│Test │ │Game│Test │
    └────┴─────┘      │        └────┴──────┘  └────┴─────┘ └────┴─────┘


Benefits:
✓ Free functions - simpler, no vtable overhead
✓ Compile-time conditionals - wrong code never compiled
✓ Resource class - prevents misuse
✓ Guaranteed execution order via ExecuteTick()
✓ Individual functions testable in isolation
```
