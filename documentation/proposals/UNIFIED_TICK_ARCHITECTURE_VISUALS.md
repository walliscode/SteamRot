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

### Abstract Engine Base Class

The chosen approach uses an abstract `Engine` base class with derived `GameEngine` and `TestEngine` classes. This keeps resource management identical while allowing different tick execution behaviors.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    ABSTRACT ENGINE ARCHITECTURE                              │
└─────────────────────────────────────────────────────────────────────────────┘

                              ┌─────────────────────────────────────┐
                              │             Engine                   │
                              │         (Abstract Base)              │
                              ├─────────────────────────────────────┤
                              │ # m_game_resources: GameResources   │
                              │ # m_game_context: GameContext       │
                              ├─────────────────────────────────────┤
                              │ + StartUp() → shared                │
                              │ + Run(num_ticks) → shared loop      │
                              │ + GetLoopNumber() → shared          │
                              │ + GetGameResources() → shared       │
                              ├─────────────────────────────────────┤
                              │ # ConfigureFromData() = 0           │ ◄── Override
                              │ # ExecuteTick() = 0                 │ ◄── Override
                              └───────────────┬─────────────────────┘
                                              │
                      ┌───────────────────────┴───────────────────────┐
                      │                                               │
                      ▼                                               ▼
┌─────────────────────────────────────┐   ┌─────────────────────────────────────┐
│           GameEngine                │   │           TestEngine                │
│    (Production Implementation)      │   │    (Testing Implementation)         │
├─────────────────────────────────────┤   ├─────────────────────────────────────┤
│ - m_scene_manager                   │   │ - m_test_config                     │
│ - m_display_manager                 │   │ - m_entity_manager                  │
├─────────────────────────────────────┤   │ - m_scene_context                   │
│ # ConfigureFromData() override      │   │ - m_current_tick                    │
│   └─ Load from game data files      │   ├─────────────────────────────────────┤
│                                     │   │ # ConfigureFromData() override      │
│ # ExecuteTick() override            │   │   └─ Load from TestDataConfig       │
│   ├─ UpdateGameResources()          │   │                                     │
│   ├─ PreloadEvents()                │   │ # ExecuteTick() override            │
│   │                                 │   │   ├─ ExecuteInputEventsForTick()    │
│   ├─ ProcessWaitingRoom() ◄─────────┼───┼───┤─ ExecuteEventsForTick()         │
│   ├─ ClearSubscribers()   ◄─────────┼───┼───┤                                 │
│   ├─ UpdateSubscribers()  ◄─SHARED──┼───┼───┤─ ProcessWaitingRoom() ◄─SHARED  │
│   ├─ TickGlobalEventBus() ◄─────────┼───┼───┤─ ClearSubscribers()   ◄─SHARED  │
│   │  (all event handling together)  │   │   ├─ UpdateSubscribers()  ◄─SHARED  │
│   │                                 │   │   ├─ TickGlobalEventBus() ◄─SHARED  │
│   ├─ ProcessSubscriptions()         │   │   │  (all event handling together)  │
│   ├─ UpdateSceneManager()           │   │   │                                 │
│   └─ CallRenderCycle()              │   │   ├─ ExecuteSimulationSteps()       │
└─────────────────────────────────────┘   │   └─ CompareTickSnapshot()          │
                                          ├─────────────────────────────────────┤
                                          │ + GetEntityManager()                │
                                          └─────────────────────────────────────┘

Legend:
◄─SHARED = Identical implementation in both derived classes
           (could be extracted to shared free functions)
```

### Engine::Run() - Shared Loop Structure

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    Engine::Run(size_t num_ticks)                            │
│                    (Shared by GameEngine and TestEngine)                    │
└─────────────────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
        ┌───────────────────────────────────────────────────────┐
        │ 1. StartUp()                                          │
        │    ├─ Configure resources from data                   │
        │    └─ Load initial scene/entities                     │
        └───────────────────────────────────────────────────────┘
                                    │
                                    ▼
        ┌───────────────────────────────────────────────────────┐
        │ 2. Determine loop condition                           │
        │    ├─ num_ticks == 0 → while(window.isOpen())        │
        │    └─ num_ticks > 0  → for(i=0; i<num_ticks; i++)    │
        └───────────────────────────────────────────────────────┘
                                    │
                                    ▼
        ┌───────────────────────────────────────────────────────┐
        │ 3. Loop body (each iteration):                        │
        │    ├─ ExecuteTick()  ◄── Virtual - different per type │
        │    └─ m_game_resources.loop_number++                  │
        └───────────────────────────────────────────────────────┘
```

### ExecuteTick() Comparison

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    ExecuteTick() Implementations                            │
└─────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────┐   ┌─────────────────────────────────────┐
│     GameEngine::ExecuteTick()       │   │     TestEngine::ExecuteTick()       │
├─────────────────────────────────────┤   ├─────────────────────────────────────┤
│                                     │   │                                     │
│ 1. UpdateGameResources()            │   │ 1. ++m_current_tick                 │
│    └─ Update mouse position         │   │                                     │
│                                     │   │ 2. ExecuteInputEventsForTick()      │
│ 2. PreloadEvents()                  │   │    └─ Inject simulated inputs       │
│    └─ Capture SFML window events    │   │                                     │
│                                     │   │ 3. ExecuteEventsForTick()           │
│                                     │   │    └─ Inject simulated events       │
├─────────────────────────────────────┼───┼─────────────────────────────────────┤
│ EVENT HANDLING BLOCK (consolidated) │   │ EVENT HANDLING BLOCK (consolidated) │
├─────────────────────────────────────┼───┼─────────────────────────────────────┤
│ 3. ProcessWaitingRoomEventBus()     │ = │ 4. ProcessWaitingRoomEventBus()     │
│    └─ Move to global bus            │   │    └─ Move to global bus            │
│                                     │   │                                     │
│ 4. ClearSubscribers()               │ = │ 5. ClearSubscribers()               │
│    └─ Clear old subscriber state    │   │    └─ Clear old subscriber state    │
│                                     │   │                                     │
│ 5. UpdateSubscribersFromGlobalBus() │ = │ 6. UpdateSubscribersFromGlobalBus() │
│    └─ Notify all subscribers        │   │    └─ Notify all subscribers        │
│                                     │   │                                     │
│ 6. TickGlobalEventBus()             │ = │ 7. TickGlobalEventBus()             │
│    └─ Decrement event lifetimes     │   │    └─ Decrement event lifetimes     │
├─────────────────────────────────────┼───┼─────────────────────────────────────┤
│ LOGIC EXECUTION (after events)      │   │ LOGIC EXECUTION (after events)      │
├─────────────────────────────────────┼───┼─────────────────────────────────────┤
│ 7. ProcessSubscriptions()           │   │ (No subscription processing)        │
│    └─ GameEngine level              │   │                                     │
│                                     │   │ 8. ExecuteSimulationSteps()         │
│ 8. UpdateSceneManager()             │   │    └─ Run SimulationData steps      │
│    └─ sAction→sCollision→sRender    │   │                                     │
│                                     │   │ 9. CompareTickSnapshot()            │
│ 9. CallRenderCycle()                │   │    └─ Validate entity state         │
│    └─ Render to window              │   │                                     │
└─────────────────────────────────────┘   └─────────────────────────────────────┘

Legend: = indicates identical operations (shared event handling block)
```

### Benefits of Abstract Engine Approach

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         BENEFITS                                             │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ✓ UNIFIED RESOURCE MANAGEMENT                                              │
│    ┌─────────────────────────────────────────────────────────────────────┐  │
│    │ GameResources, GameContext handled identically in Engine base      │  │
│    │ No duplication between GameEngine and TestEngine                   │  │
│    └─────────────────────────────────────────────────────────────────────┘  │
│                                                                             │
│  ✓ CONSISTENT LOOP STRUCTURE                                                │
│    ┌─────────────────────────────────────────────────────────────────────┐  │
│    │ Engine::Run() shared by both derived classes                       │  │
│    │ Loop number management in one place                                │  │
│    └─────────────────────────────────────────────────────────────────────┘  │
│                                                                             │
│  ✓ CLEAR EXTENSION POINTS                                                   │
│    ┌─────────────────────────────────────────────────────────────────────┐  │
│    │ Only ConfigureFromData() and ExecuteTick() differ                  │  │
│    │ Easy to see what's different between game and test                 │  │
│    └─────────────────────────────────────────────────────────────────────┘  │
│                                                                             │
│  ✓ TYPE-SAFE                                                                │
│    ┌─────────────────────────────────────────────────────────────────────┐  │
│    │ Can't accidentally use GameEngine behaviors in TestEngine          │  │
│    │ Compile-time checking via class hierarchy                          │  │
│    └─────────────────────────────────────────────────────────────────────┘  │
│                                                                             │
│  ✓ TESTABLE                                                                 │
│    ┌─────────────────────────────────────────────────────────────────────┐  │
│    │ TestEngine can be instantiated directly in unit tests              │  │
│    │ No need for separate test harness infrastructure                   │  │
│    └─────────────────────────────────────────────────────────────────────┘  │
│                                                                             │
│  ✓ NO COMPILE-TIME CONDITIONALS                                             │
│    ┌─────────────────────────────────────────────────────────────────────┐  │
│    │ No #ifdef STEAMROT_TEST_BUILD needed                               │  │
│    │ Polymorphism handles the differences cleanly                       │  │
│    └─────────────────────────────────────────────────────────────────────┘  │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

### TickContext (Supplementary Approach)

The `TickContext` approach remains available for cases where you want to share individual tick execution steps as free functions:

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
│           UNIFIED TICK ARCHITECTURE OVERVIEW (Abstract Engine)              │
└─────────────────────────────────────────────────────────────────────────────┘

                              ┌─────────────────────────┐
                              │         Engine          │
                              │    (Abstract Base)      │
                              ├─────────────────────────┤
                              │ # m_game_resources      │
                              │ # m_game_context        │
                              ├─────────────────────────┤
                              │ + StartUp()             │
                              │ + Run(num_ticks)        │
                              │ # ConfigureFromData()   │ ◄── virtual
                              │ # ExecuteTick()         │ ◄── virtual
                              └───────────┬─────────────┘
                                          │
                     ┌────────────────────┴────────────────────┐
                     │                                         │
                     ▼                                         ▼
          ┌─────────────────────┐                 ┌─────────────────────┐
          │     GameEngine      │                 │     TestEngine      │
          ├─────────────────────┤                 ├─────────────────────┤
          │ - m_scene_manager   │                 │ - m_test_config     │
          │ - m_display_manager │                 │ - m_entity_manager  │
          ├─────────────────────┤                 │ - m_current_tick    │
          │ ExecuteTick():      │                 ├─────────────────────┤
          │  • Real SFML input  │                 │ ExecuteTick():      │
          │  • Scene logic      │                 │  • Simulated input  │
          │  • Render to window │                 │  • Simulation steps │
          └─────────────────────┘                 │  • State validation │
                                                  └─────────────────────┘


Benefits:
✓ Unified resource management in Engine base class
✓ Consistent loop structure via Engine::Run()
✓ Clear extension points (ConfigureFromData, ExecuteTick)
✓ Type-safe through polymorphism
✓ TestEngine usable directly in unit tests
✓ No compile-time conditionals needed
```
