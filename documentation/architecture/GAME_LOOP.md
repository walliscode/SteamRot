# Game Loop Architecture

[← Back to Documentation](../README.md) | [Architecture Overview](../README.md#architecture)

This document provides a comprehensive visual representation of the main game loop structure and update cycle in SteamRot, including tick/loop number updates.

**Related Documentation:**
- [Logic System](LOGIC_SYSTEM.md) - Logic processing
- [Testing Harness Loop](../testing/TESTING_HARNESS_LOOP.md) - Test execution loop
- [Workflows](../workflows/) - Implementation guides

---

## Table of Contents
- [Overview](#overview)
- [Visual Diagram](#visual-diagram)
- [Loop Initialization](#loop-initialization)
- [Main Game Loop](#main-game-loop)
- [UpdateSystems Breakdown](#updatesystems-breakdown)
- [Scene Update Cycle](#scene-update-cycle)
- [Tick Management](#tick-management)
- [Component Reference](#component-reference)

---

## Overview

The SteamRot game engine operates on a traditional game loop architecture where each iteration processes input, updates game state, and renders output. The loop continues until the game window is closed or a simulation limit is reached.

**Key Files:**
- `steamrot/main.cpp` - Entry point
- `src/systems/GameEngine.h/cpp` - Main game loop
- `src/scenes/SceneManager.h/cpp` - Scene management
- `src/scenes/Scene.h` - Abstract scene base class

---

## Visual Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                         GAME STARTUP                            │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│  main() creates GameEngine(EnvironmentType::Production)         │
│  • m_game_resources.loop_number = 1 (INITIAL VALUE)            │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│  GameEngine::RunGame(number_of_loops, simulation)               │
│  ├─→ StartUp()                                                  │
│  │   ├─ Configure GameResources from data                      │
│  │   ├─ Load default assets                                    │
│  │   ├─ Configure SceneManager                                 │
│  │   └─ LoadTitleScene()                                       │
│  └─→ RunGameLoop(number_of_loops, simulation)                  │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
╔═════════════════════════════════════════════════════════════════╗
║                     MAIN GAME LOOP                              ║
║  while (m_game_resources.game_window.isOpen())                  ║
╚═════════════════════════════════════════════════════════════════╝
                              │
    ┌─────────────────────────┴─────────────────────────┐
    │                                                     │
    ▼                                                     │
┌─────────────────────────────────────────────────────┐ │
│  UpdateSystems()                                    │ │
│  (Details in UpdateSystems Breakdown section)       │ │
└─────────────────────────────────────────────────────┘ │
    │                                                     │
    ▼                                                     │
┌─────────────────────────────────────────────────────┐ │
│  Simulation Check (if in simulation mode)           │ │
│  if (simulation && loop_number == number_of_loops)  │ │
│      break;                                         │ │
└─────────────────────────────────────────────────────┘ │
    │                                                     │
    ▼                                                     │
┌─────────────────────────────────────────────────────┐ │
│  🔢 TICK INCREMENT                                  │ │
│  m_game_resources.loop_number++                     │ │
│  (Loop counter incremented at END of iteration)     │ │
└─────────────────────────────────────────────────────┘ │
    │                                                     │
    └─────────────────────────────────────────────────────┘
    (Loop continues until window closes or simulation ends)

┌─────────────────────────────────────────────────────────────────┐
│                         GAME SHUTDOWN                           │
│  ShutDown() - Currently empty, reserved for cleanup            │
└─────────────────────────────────────────────────────────────────┘
```

---

## Loop Initialization

The loop counter is initialized when the GameEngine is constructed:

```cpp
GameEngine::GameEngine(EnvironmentType env_type)
    : m_game_context(m_game_resources), 
      m_scene_manager(m_game_context),
      m_display_manager(m_game_resources.game_window, m_scene_manager) {
  
  m_game_resources.env_type = env_type;
  m_game_resources.loop_number = 1;  // ← Starts at 1
}
```

---

## Main Game Loop

The main loop in `GameEngine::RunGameLoop()`:

```cpp
void GameEngine::RunGameLoop(size_t number_of_loops, bool simulation) {
  
  // Loop continues while window is open
  while (m_game_resources.game_window.isOpen()) {

    // 1. Update all systems
    UpdateSystems();

    // 2. Check simulation limit
    if (simulation && (number_of_loops == m_game_resources.loop_number))
      break;

    // 3. INCREMENT LOOP COUNTER (at end of iteration)
    m_game_resources.loop_number++;
  }
}
```

**Key Points:**
- Loop starts with `loop_number = 1`
- `UpdateSystems()` executes first
- Loop number increments **at the end** of each iteration
- Simulation mode allows running for a fixed number of loops

---

## UpdateSystems Breakdown

```
┌─────────────────────────────────────────────────────────────────┐
│                    UpdateSystems()                              │
│  (GameEngine method - orchestrates all system updates)          │
└─────────────────────────────────────────────────────────────────┘
         │
         ├─→ Step 1: UpdateGameResources()
         │   └─ Update mouse position from window
         │
         ├─→ Step 2: EventHandler::PreloadEvents(window)
         │   └─ Capture external events (keyboard, mouse, window)
         │
         ├─→ Step 3: EventHandler::ProcessWaitingRoomEventBus()
         │   └─ Move events from waiting room to global event bus
         │
         ├─→ Step 4: EventHandler::UpateSubscribersFromGlobalEventBus()
         │   └─ Update all subscribers with events from global bus
         │
         ├─→ Step 5: GameEngine::ProcessSubscriptions()
         │   └─ Process active GameEngine-level subscribers
         │      (e.g., EVENT_QUIT_GAME closes window)
         │
         ├─→ Step 6: SceneManager::UpdateSceneManager()
         │   │
         │   ├─→ SceneManager::ProcessSubscriptions()
         │   │   └─ Process active SceneManager-level subscribers
         │   │      (e.g., EVENT_LOAD_SCENE triggers scene changes)
         │   │
         │   └─→ SceneManager::UpdateScenes()
         │       └─ (See Scene Update Cycle below)
         │
         ├─→ Step 7: DisplayManager::CallRenderCycle()
         │   └─ Render all active scenes to window
         │
         └─→ Step 8: EventHandler::TickGlobalEventBus() ⏱️
             └─ Decrement event lifetimes and remove expired events
```

**Event Bus Flow:**

```
External Events → Waiting Room Event Bus
                       ↓ (ProcessWaitingRoomEventBus)
                  Global Event Bus → Subscribers
                       ↓ (After all updates)
                  Tick Event Bus (decrement lifetimes)
```

---

## Scene Update Cycle

Each active scene goes through the following update sequence:

```
┌─────────────────────────────────────────────────────────────────┐
│  SceneManager::UpdateScenes()                                   │
│  for each scene in m_scenes:                                    │
└─────────────────────────────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────────────────┐
│  Scene::sAction()                                               │
│  • Process user actions and input                              │
│  • Execute Action-type Logic classes in order                  │
│  • Examples: UIActionLogic, ButtonActionLogic                  │
└─────────────────────────────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────────────────┐
│  Scene::sCollision()                                            │
│  • Detect and handle collisions                                │
│  • Execute Collision-type Logic classes in order               │
│  • Examples: UICollisionLogic (mouse over detection)           │
└─────────────────────────────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────────────────┐
│  Scene::sRender()                                               │
│  • Clear scene render texture                                  │
│  • Execute Render-type Logic classes in order                  │
│  • Draw all scene entities to scene texture                    │
│  • Examples: UIRenderLogic, CraftingRenderLogic                │
└─────────────────────────────────────────────────────────────────┘
```

**Logic Execution Order:**

Each `s*()` method iterates through its logic vector:

```cpp
void TitleScene::sAction() {
  for (auto &action_logic : m_logic_map[LogicType::Action]) {
    action_logic->RunLogic();  // Execute in vector order
  }
}
```

Logic classes are added to the vector in the order specified by `LogicFactory`.

---

## Tick Management

### Loop Number Tracking

The `loop_number` is stored in `GameResources` and accessible throughout the game:

```cpp
struct GameResources {
  size_t loop_number;          // Current loop iteration (starts at 1)
  EnvironmentType env_type;
  sf::RenderWindow game_window;
  EventHandler event_handler;
  sf::Vector2i mouse_position;
  AssetManager asset_manager;
};
```

### Accessing Loop Number

```cpp
// From GameEngine
size_t GameEngine::GetLoopNumber() const {
  return m_game_resources.loop_number;
}

// From GameContext (available to scenes and logic)
const GameContext& game_context = ...;
size_t current_loop = game_context.game_resources.loop_number;
```

### Event Bus Ticking

The global event bus maintains event lifetimes:

```cpp
void EventHandler::TickGlobalEventBus() {
  // Decrement lifetimes and remove expired events
  // Called at END of UpdateSystems (after all processing)
}
```

**Event Lifetime Flow:**

1. Event added with initial lifetime
2. Event persists for multiple loops
3. Lifetime decremented each tick
4. Event removed when lifetime reaches 0

---

## Component Reference

### GameEngine
- **File:** `src/systems/GameEngine.h/cpp`
- **Responsibilities:**
  - Main game loop orchestration
  - Game-level resource management
  - Subscriber processing (quit events, etc.)
  - Loop counter management

### SceneManager
- **File:** `src/scenes/SceneManager.h/cpp`
- **Responsibilities:**
  - Scene lifecycle management
  - Scene update orchestration
  - Scene-level subscriber processing

### Scene (Abstract)
- **File:** `src/scenes/Scene.h`
- **Concrete Classes:** `TitleScene`, `CraftingScene`
- **Responsibilities:**
  - Entity management
  - Logic execution (Action → Collision → Render)
  - Scene-specific rendering

### EventHandler
- **File:** `src/events/EventHandler.h/cpp`
- **Responsibilities:**
  - Event bus management (waiting room, global)
  - Subscriber registration and notification
  - Event lifetime management

### DisplayManager
- **File:** `src/display/DisplayManager.h/cpp`
- **Responsibilities:**
  - Render cycle coordination
  - Scene texture composition
  - Window rendering

---

## Summary

The game loop follows this execution order per iteration:

1. **Update mouse position** (UpdateGameResources)
2. **Capture external events** (PreloadEvents)
3. **Process waiting room events** → Global event bus
4. **Update subscribers** from global event bus
5. **Process GameEngine subscriptions** (quit, etc.)
6. **Process SceneManager subscriptions** (scene changes)
7. **Update all scenes:**
   - sAction (input/actions)
   - sCollision (collision detection)
   - sRender (rendering)
8. **Render to window** (CallRenderCycle)
9. **Tick event bus** (decrement lifetimes)
10. **Increment loop_number** (for next iteration)

The loop continues until the window closes or simulation limit is reached.
