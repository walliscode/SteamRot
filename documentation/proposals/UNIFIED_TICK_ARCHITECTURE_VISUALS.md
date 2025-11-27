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

### TickExecutor Class Hierarchy

```
                    ┌─────────────────────────┐
                    │      TickExecutor       │
                    │     (Abstract Base)     │
                    ├─────────────────────────┤
                    │ # m_scene_context       │
                    │ # m_game_resources      │
                    ├─────────────────────────┤
                    │ + ExecuteTick()         │◄─── Template Method Pattern
                    │ # OnPreTick()           │     Defines execution order
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
├───────────────────────────────┤   ├───────────────────────────────┤
│ - m_scene_manager             │   │ - m_config                    │
│ - m_display_manager           │   │ - m_current_tick              │
├───────────────────────────────┤   │ - m_fixture                   │
│ # OnPreTick()                 │   ├───────────────────────────────┤
│   └─ UpdateGameResources      │   │ # OnPreTick()                 │
│   └─ PreloadEvents            │   │   └─ ExecuteInputEvents       │
│                               │   │   └─ ExecuteEvents            │
│ # OnProcessEvents()           │   │                               │
│   └─ ProcessSubscriptions     │   │ # OnProcessEvents()           │
│                               │   │   └─ (optional)               │
│ # OnExecuteLogic()            │   │                               │
│   └─ UpdateScenes             │   │ # OnExecuteLogic()            │
│                               │   │   └─ ExecuteSimulation        │
│ # OnPostLogic()               │   │                               │
│   └─ CallRenderCycle          │   │ # OnPostLogic()               │
└───────────────────────────────┘   │   └─ CompareTickSnapshot      │
                                    └───────────────────────────────┘
```

### ExecuteTick() Method (Common for Both)

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                     TickExecutor::ExecuteTick()                             │
│                     (Guaranteed Execution Order)                            │
└─────────────────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
        ┌───────────────────────────────────────────────────────┐
        │ 1. OnPreTick()                              [VIRTUAL] │
        │    ┌────────────────────┐  ┌────────────────────────┐ │
        │    │ GameTickExecutor   │  │ TestTickExecutor       │ │
        │    │ • UpdateResources  │  │ • InjectInputs         │ │
        │    │ • PreloadEvents    │  │ • InjectEvents         │ │
        │    └────────────────────┘  └────────────────────────┘ │
        └───────────────────────────────────────────────────────┘
                                    │
                                    ▼
        ┌───────────────────────────────────────────────────────┐
        │ 2. ProcessWaitingRoomEventBus()           [CONCRETE]  │
        │    └─ Shared implementation in base class             │
        └───────────────────────────────────────────────────────┘
                                    │
                                    ▼
        ┌───────────────────────────────────────────────────────┐
        │ 3. UpdateSubscribersFromGlobalEventBus()  [CONCRETE]  │
        │    └─ Shared implementation in base class             │
        └───────────────────────────────────────────────────────┘
                                    │
                                    ▼
        ┌───────────────────────────────────────────────────────┐
        │ 4. OnProcessEvents()                        [VIRTUAL] │
        │    ┌────────────────────┐  ┌────────────────────────┐ │
        │    │ GameTickExecutor   │  │ TestTickExecutor       │ │
        │    │ • ProcessSubs      │  │ • (optional)           │ │
        │    │   (GameEngine)     │  │                        │ │
        │    │ • ProcessSubs      │  │                        │ │
        │    │   (SceneManager)   │  │                        │ │
        │    └────────────────────┘  └────────────────────────┘ │
        └───────────────────────────────────────────────────────┘
                                    │
                                    ▼
        ┌───────────────────────────────────────────────────────┐
        │ 5. OnExecuteLogic()                         [VIRTUAL] │
        │    ┌────────────────────┐  ┌────────────────────────┐ │
        │    │ GameTickExecutor   │  │ TestTickExecutor       │ │
        │    │ • UpdateScenes     │  │ • ExecuteSimulation    │ │
        │    │   (LogicMap)       │  │   (SimulationData)     │ │
        │    └────────────────────┘  └────────────────────────┘ │
        └───────────────────────────────────────────────────────┘
                                    │
                                    ▼
        ┌───────────────────────────────────────────────────────┐
        │ 6. OnPostLogic()                            [VIRTUAL] │
        │    ┌────────────────────┐  ┌────────────────────────┐ │
        │    │ GameTickExecutor   │  │ TestTickExecutor       │ │
        │    │ • CallRenderCycle  │  │ • CompareSnapshot      │ │
        │    └────────────────────┘  └────────────────────────┘ │
        └───────────────────────────────────────────────────────┘
                                    │
                                    ▼
        ┌───────────────────────────────────────────────────────┐
        │ 7. TickGlobalEventBus()                   [CONCRETE]  │
        │    └─ Shared implementation in base class             │
        └───────────────────────────────────────────────────────┘
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
│                    UNIFIED TICK ARCHITECTURE OVERVIEW                        │
└─────────────────────────────────────────────────────────────────────────────┘

                              ┌─────────────────┐
                              │  TickExecutor   │
                              │ (Abstract Base) │
                              └────────┬────────┘
                                       │
            ┌──────────────────────────┼──────────────────────────┐
            │                          │                          │
            ▼                          │                          ▼
  ┌─────────────────┐                  │               ┌─────────────────┐
  │ GameTickExecutor│                  │               │ TestTickExecutor│
  └────────┬────────┘                  │               └────────┬────────┘
           │                           │                        │
           │                           │                        │
           ▼                           ▼                        ▼
  ┌─────────────────┐         ┌─────────────────┐      ┌─────────────────┐
  │ SceneLogic-     │         │ ILogicProvider  │      │ SimulationLogic-│
  │ Provider        │◄────────│  (Interface)    │─────▶│ Provider        │
  └─────────────────┘         └─────────────────┘      └─────────────────┘
           │                                                    │
           │                                                    │
           ▼                                                    ▼
  ┌─────────────────┐         ┌─────────────────┐      ┌─────────────────┐
  │ DefaultScene-   │         │ IEntityData-    │      │ TestDataSource  │
  │ DataSource      │◄────────│ Source          │─────▶│                 │
  └─────────────────┘         └─────────────────┘      └─────────────────┘

Benefits:
✓ Consistent execution order
✓ Injectable logic configuration
✓ Unified data loading
✓ Clear naming conventions
✓ Reduced code duplication
```
