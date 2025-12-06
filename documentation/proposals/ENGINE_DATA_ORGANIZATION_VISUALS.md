# Engine Data Organization - Visual Guide

**Companion Document to**: [ENGINE_DATA_ORGANIZATION.md](ENGINE_DATA_ORGANIZATION.md)

This document provides visual diagrams and flowcharts to illustrate the proposed Engine data organization strategy.

---

## Table of Contents

1. [Category Overview](#category-overview)
2. [Current State Diagram](#current-state-diagram)
3. [Proposed State Diagram](#proposed-state-diagram)
4. [Decision Trees](#decision-trees)
5. [Access Pattern Diagrams](#access-pattern-diagrams)
6. [Migration Flow](#migration-flow)
7. [Example Scenarios](#example-scenarios)

---

## Category Overview

### The Three Categories

```
┌─────────────────────────────────────────────────────────────────┐
│                         ENGINE DATA                              │
└────────────────┬────────────────┬───────────────┬────────────────┘
                 │                │               │
        ┌────────▼────────┐ ┌────▼─────┐ ┌──────▼──────┐
        │  Core Resources │ │  Config  │ │ Engine State│
        └────────┬────────┘ └────┬─────┘ └──────┬──────┘
                 │                │               │
        ┌────────▼────────────────▼───────────────▼────────┐
        │         Distinct Responsibilities                 │
        │  • Lifetime     • Source      • Mutability      │
        │  • Access       • Naming      • Purpose         │
        └──────────────────────────────────────────────────┘
```

### Category Characteristics Matrix

```
┌─────────────────┬───────────────┬───────────────┬──────────────┐
│ Characteristic  │ Core Resources│ Configuration │ Engine State │
├─────────────────┼───────────────┼───────────────┼──────────────┤
│ Lifetime        │ Engine        │ Session       │ Runtime      │
│                 │ lifetime      │ lifetime      │ (transient)  │
├─────────────────┼───────────────┼───────────────┼──────────────┤
│ Creation        │ Construction  │ Startup       │ During       │
│                 │               │               │ execution    │
├─────────────────┼───────────────┼───────────────┼──────────────┤
│ Source          │ Code          │ Files/User    │ Generated    │
│                 │               │               │ at runtime   │
├─────────────────┼───────────────┼───────────────┼──────────────┤
│ Mutability      │ Resources     │ Mostly        │ Highly       │
│                 │ can change    │ read-only     │ mutable      │
├─────────────────┼───────────────┼───────────────┼──────────────┤
│ Persistence     │ No            │ Yes (saved)   │ No (unless   │
│                 │               │               │ explicit)    │
├─────────────────┼───────────────┼───────────────┼──────────────┤
│ Scope           │ Global        │ Global        │ Engine-level │
├─────────────────┼───────────────┼───────────────┼──────────────┤
│ Examples        │ RenderWindow  │ window_width  │ running flag │
│                 │ EventHandler  │ framerate     │ subscriptions│
│                 │ AssetManager  │ vsync         │ quit request │
└─────────────────┴───────────────┴───────────────┴──────────────┘
```

---

## Current State Diagram

### Current Engine Data Layout

```
┌───────────────────────────────────────────────────────────────┐
│                          Engine                                │
├───────────────────────────────────────────────────────────────┤
│                                                                │
│  ┌──────────────────────┐                                     │
│  │     GameCore         │  ← Resource container               │
│  │  - game_window       │                                     │
│  │  - event_handler     │                                     │
│  │  - mouse_position    │  ⚠️ Mixed: resources + runtime state│
│  │  - loop_number       │                                     │
│  │  - asset_manager     │                                     │
│  └──────────────────────┘                                     │
│                                                                │
│  ┌──────────────────────┐                                     │
│  │    GameContext       │  ← References to GameCore           │
│  │  (references only)   │                                     │
│  └──────────────────────┘                                     │
│                                                                │
│  ┌──────────────────────┐                                     │
│  │   UserPreferences    │  ← Configuration                    │
│  │  (some config data)  │                                     │
│  └──────────────────────┘  ⚠️ But not all config here        │
│                                                                │
│  bool m_running;          ← Engine state (scattered)          │
│                              ⚠️ Not grouped                    │
│  vector<Subscriber>       ← More engine state                 │
│  m_subscriptions;            ⚠️ Purpose unclear                │
│                                                                │
│  SceneManager            ← Subsystem (correct)                │
│  m_scene_manager;                                              │
│                                                                │
└───────────────────────────────────────────────────────────────┘

Problems:
⚠️  No clear categorization
⚠️  Mixed purposes in single structs
⚠️  Hard to find where data belongs
⚠️  Inconsistent naming
```

---

## Proposed State Diagram

### Proposed Engine Data Layout

```
┌────────────────────────────────────────────────────────────────┐
│                          Engine                                 │
├────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌──────────────────────────────────────────────────┐          │
│  │              CORE RESOURCES (GameCore)           │          │
│  ├──────────────────────────────────────────────────┤          │
│  │  // Rendering & Display                          │          │
│  │  sf::RenderWindow game_window;                   │          │
│  │                                                   │          │
│  │  // Event System                                 │          │
│  │  EventHandler event_handler;                     │          │
│  │  sf::Vector2i mouse_position;                    │          │
│  │                                                   │          │
│  │  // Asset Management                             │          │
│  │  AssetManager asset_manager;                     │          │
│  │                                                   │          │
│  │  // Timing                                       │          │
│  │  size_t loop_number;                             │          │
│  └──────────────────────────────────────────────────┘          │
│  ✓ Clear: Long-lived global resources                          │
│                                                                 │
│  ┌──────────────────────────────────────────────────┐          │
│  │         CONFIGURATION (EngineConfig)             │          │
│  ├──────────────────────────────────────────────────┤          │
│  │  DisplayConfig display {                         │          │
│  │    window_width, window_height,                  │          │
│  │    window_title, framerate_limit,                │          │
│  │    fullscreen, vsync                             │          │
│  │  }                                               │          │
│  │                                                   │          │
│  │  InputConfig input { ... }     // Future         │          │
│  │  AudioConfig audio { ... }     // Future         │          │
│  └──────────────────────────────────────────────────┘          │
│  ✓ Clear: Loaded settings                                      │
│                                                                 │
│  ┌──────────────────────────────────────────────────┐          │
│  │       ENGINE STATE (EngineState)                 │          │
│  ├──────────────────────────────────────────────────┤          │
│  │  // Execution control                            │          │
│  │  bool running;                                   │          │
│  │  bool paused;                                    │          │
│  │                                                   │          │
│  │  // Subscription management                      │          │
│  │  vector<shared_ptr<Subscriber>> subscriptions;   │          │
│  │                                                   │          │
│  │  // Requests                                     │          │
│  │  bool quit_requested;                            │          │
│  │  optional<SceneType> requested_scene;            │          │
│  │                                                   │          │
│  │  // Performance tracking                         │          │
│  │  PerformanceMetrics performance;                 │          │
│  └──────────────────────────────────────────────────┘          │
│  ✓ Clear: Runtime operational state                            │
│                                                                 │
│  ┌──────────────────────────────────────────────────┐          │
│  │           SUBSYSTEMS (Managers)                  │          │
│  ├──────────────────────────────────────────────────┤          │
│  │  SceneManager m_scene_manager;                   │          │
│  │  DisplayManager m_display_manager;  // GameEngine│          │
│  └──────────────────────────────────────────────────┘          │
│  ✓ Clear: Complex subsystems with logic                        │
│                                                                 │
└────────────────────────────────────────────────────────────────┘

Benefits:
✓  Clear categorization by purpose
✓  Intuitive naming reflects category
✓  Easy to find where data belongs
✓  Consistent organization
```

---

## Decision Trees

### Where Should New Data Go?

```
                    ┌────────────────────────────┐
                    │   New Data to Add?         │
                    └────────────┬───────────────┘
                                 │
                    ┌────────────▼───────────────┐
                    │ Is it a long-lived,        │
                    │ global infrastructure      │
                    │ component?                 │
                    └────┬──────────────┬────────┘
                         │ YES          │ NO
                         ▼              ▼
                  ┌─────────────┐  ┌──────────────────────┐
                  │  GameCore   │  │ Is it loaded from    │
                  │             │  │ files and configures │
                  │ Add to      │  │ behavior?            │
                  │ GameCore    │  └────┬───────────┬─────┘
                  │ struct      │       │ YES       │ NO
                  └─────────────┘       ▼           ▼
                                 ┌─────────────┐  ┌──────────────┐
                                 │EngineConfig │  │ Is it runtime│
                                 │             │  │ state that   │
                                 │ Add config  │  │ changes?     │
                                 │ subcategory │  └───┬──────────┘
                                 └─────────────┘      │ YES
                                                      ▼
                                              ┌──────────────┐
                                              │ EngineState  │
                                              │              │
                                              │ Add to state │
                                              │ struct       │
                                              └──────────────┘
```

### Is It a Resource or a Subsystem?

```
                    ┌────────────────────────────┐
                    │   Adding to Core?          │
                    └────────────┬───────────────┘
                                 │
                    ┌────────────▼───────────────┐
                    │ Does it have complex       │
                    │ internal logic/state?      │
                    └────┬──────────────┬────────┘
                         │ YES          │ NO
                         ▼              ▼
                  ┌─────────────┐  ┌──────────────────┐
                  │  Subsystem  │  │  Simple Resource │
                  │  Manager    │  │                  │
                  │             │  │  Add to GameCore │
                  │ Create      │  │  struct          │
                  │ separate    │  │                  │
                  │ *Manager    │  │  Example:        │
                  │ class       │  │  - RenderWindow  │
                  │             │  │  - sf::Font      │
                  │ Examples:   │  │  - Vector2i      │
                  │ - Scene     │  └──────────────────┘
                  │   Manager   │
                  │ - Display   │
                  │   Manager   │
                  │ - Audio     │
                  │   Manager   │
                  └─────────────┘
```

### Configuration vs. User Preferences?

```
                    ┌────────────────────────────┐
                    │   Adding Configuration?    │
                    └────────────┬───────────────┘
                                 │
                    ┌────────────▼───────────────┐
                    │ Can user modify this       │
                    │ through settings menu?     │
                    └────┬──────────────┬────────┘
                         │ YES          │ NO
                         ▼              ▼
                  ┌──────────────┐  ┌─────────────────┐
                  │     User     │  │  Engine Config  │
                  │ Preferences  │  │                 │
                  │              │  │  Developer/     │
                  │ Examples:    │  │  System config  │
                  │ - Volume     │  │                 │
                  │ - Language   │  │  Examples:      │
                  │ - Keybinds   │  │  - Window size  │
                  │ - Difficulty │  │  - Framerate    │
                  └──────────────┘  │  - Vsync        │
                                    └─────────────────┘
                    
                    Both go in EngineConfig
                    (different subcategories)
```

---

## Access Pattern Diagrams

### Current Access Pattern (Unclear)

```
┌────────────┐        ┌──────────────┐
│ Subsystem  │───────▶│    Engine    │
│ (Scene,    │        │              │
│  Logic)    │        │  ??? What    │
└────────────┘        │  members?    │
                      │              │
                      │  m_game_core?│
                      │  m_running?  │
                      │  m_user_pref?│
                      └──────────────┘
                      
Problem: No clear interface, must know internal structure
```

### Proposed Access Pattern (Clear)

```
┌────────────┐        ┌────────────────────────────────┐
│ Subsystem  │───────▶│        GameContext             │
│            │        │  (Reference Wrapper)           │
│            │        │                                │
│            │        │  // Core Resources             │
│            │        │  GameCore& game_core;          │
│            │        │  RenderWindow& game_window;    │
│            │        │  EventHandler& event_handler;  │
│            │        │  AssetManager& asset_manager;  │
│            │        │                                │
│            │        │  // Engine State (if needed)   │
│            │        │  EngineState& engine_state;    │
└────────────┘        └────────────────────────────────┘
                                    │
                                    ▼
                      ┌────────────────────────┐
                      │       Engine           │
                      │                        │
                      │  m_game_core           │
                      │  m_engine_config       │
                      │  m_engine_state        │
                      └────────────────────────┘

Benefit: Clear, organized access through well-defined interface
```

### Configuration Access Pattern

```
Startup Flow:
┌──────────┐      ┌───────────┐      ┌──────────────┐
│  Files   │─────▶│  Load     │─────▶│ EngineConfig │
│          │      │  Config   │      │              │
│ .json    │      │           │      │ - display    │
│ .bin     │      │           │      │ - audio      │
└──────────┘      └───────────┘      │ - input      │
                                     └──────┬───────┘
                                            │
                                            │ Configure
                                            ▼
                                     ┌──────────────┐
                                     │  GameCore    │
                                     │              │
                                     │ Setup window │
                                     │ Set framerate│
                                     └──────────────┘

Runtime Access (Read-Only):
┌──────────┐      ┌───────────┐
│  Logic   │─────▶│ m_engine_ │
│  System  │      │ config    │
│          │      │           │
│          │      │ .display  │
│          │      │ .vsync    │
└──────────┘      └───────────┘
```

---

## Migration Flow

### Gradual Migration Strategy

```
Phase 1: Add New Structs (Non-Breaking)
┌────────────────────────────────────────┐
│             Engine                      │
├────────────────────────────────────────┤
│  // OLD (keep)                          │
│  GameCore m_game_core;                  │
│  bool m_running;                        │
│  vector<Subscriber> m_subscriptions;    │
│                                         │
│  // NEW (add)                           │
│  EngineConfig m_engine_config;   ←──────┼─ New, initially empty
│  EngineState m_engine_state;     ←──────┼─ New, initially empty
└────────────────────────────────────────┘

Phase 2: Migrate One Member (Dual Access)
┌────────────────────────────────────────┐
│             Engine                      │
├────────────────────────────────────────┤
│  // OLD (keep temporarily)              │
│  bool m_running;              ←─────────┼─ Still accessible
│                                         │
│  // NEW (populate)                      │
│  EngineState m_engine_state {           │
│    running: false         ←─────────────┼─ New location
│  };                                     │
│                                         │
│  // COMPATIBILITY                       │
│  [[deprecated]]                         │
│  bool& GetRunning() {      ←────────────┼─ Delegate to new
│    return m_engine_state.running;       │
│  }                                      │
└────────────────────────────────────────┘

Phase 3: Update Usage Sites
┌────────────────┐         ┌────────────────┐
│  Usage Site 1  │         │  Usage Site 2  │
└────────┬───────┘         └────────┬───────┘
         │                          │
         │ OLD:                     │ NEW:
         │ engine.m_running         │ engine.m_engine_state.running
         │                          │
         └─────────┬────────────────┘
                   │
                   ▼
         ┌─────────────────┐
         │  All sites      │
         │  updated        │
         └─────────────────┘

Phase 4: Remove Old Member
┌────────────────────────────────────────┐
│             Engine                      │
├────────────────────────────────────────┤
│  // OLD (removed)                       │
│  // bool m_running;  ✗ DELETED          │
│                                         │
│  // NEW (only location)                 │
│  EngineState m_engine_state {           │
│    running: false                       │
│  };                                     │
└────────────────────────────────────────┘
```

---

## Example Scenarios

### Scenario 1: Adding Audio Manager

```
Question: Where should AudioManager go?

Analysis:
┌────────────────────────────────────┐
│ AudioManager Characteristics:      │
├────────────────────────────────────┤
│ - Long-lived? YES                  │
│ - Global scope? YES                │
│ - Complex logic? YES               │
│ - Has internal state? YES          │
└────────────────────────────────────┘
            │
            ▼
    Decision: Subsystem Manager
    (Like SceneManager, DisplayManager)

Implementation:
┌──────────────────────────────────────┐
│           Engine                      │
├──────────────────────────────────────┤
│  GameCore m_game_core;                │
│  EngineConfig m_engine_config;        │
│  EngineState m_engine_state;          │
│                                       │
│  // Subsystems                        │
│  SceneManager m_scene_manager;        │
│  AudioManager m_audio_manager;  ←─────┼── Add here
└──────────────────────────────────────┘

Access:
┌──────────────────────────────────────┐
│         GameContext                   │
├──────────────────────────────────────┤
│  GameCore& game_core;                 │
│  AudioManager& audio_manager;  ←──────┼── Add reference
└──────────────────────────────────────┘
```

### Scenario 2: Adding Input Configuration

```
Question: Where should key bindings go?

Analysis:
┌────────────────────────────────────┐
│ Key Bindings Characteristics:      │
├────────────────────────────────────┤
│ - Loaded from files? YES           │
│ - User-configurable? YES           │
│ - Runtime state? NO                │
│ - Changes per frame? NO            │
└────────────────────────────────────┘
            │
            ▼
    Decision: Configuration

Implementation:
┌──────────────────────────────────────┐
│        EngineConfig                   │
├──────────────────────────────────────┤
│  DisplayConfig display;               │
│  InputConfig input;            ←──────┼── Add new category
└──────────────────────────────────────┘
            │
            ▼
┌──────────────────────────────────────┐
│         InputConfig                   │
├──────────────────────────────────────┤
│  map<string, Key> key_bindings {      │
│    {"jump", Keyboard::Space},         │
│    {"attack", Keyboard::X}            │
│  };                                   │
└──────────────────────────────────────┘

Usage:
void ProcessInput() {
  auto jump_key = m_engine_config.input.key_bindings["jump"];
  if (keyboard.isPressed(jump_key)) {
    // Handle jump
  }
}
```

### Scenario 3: Adding Performance Tracking

```
Question: Where should FPS counter go?

Analysis:
┌────────────────────────────────────┐
│ FPS Counter Characteristics:       │
├────────────────────────────────────┤
│ - Changes per frame? YES           │
│ - Runtime state? YES               │
│ - Configuration? NO                │
│ - Core resource? NO                │
└────────────────────────────────────┘
            │
            ▼
    Decision: Engine State

Implementation:
┌──────────────────────────────────────┐
│        EngineState                    │
├──────────────────────────────────────┤
│  bool running;                        │
│  vector<Subscriber> subscriptions;    │
│  PerformanceMetrics performance; ←────┼── Add metrics
└──────────────────────────────────────┘
            │
            ▼
┌──────────────────────────────────────┐
│    PerformanceMetrics                 │
├──────────────────────────────────────┤
│  float frame_time_ms;                 │
│  float fps;                           │
│  size_t total_frames;                 │
│  time_point last_frame_time;          │
└──────────────────────────────────────┘

Usage:
void Engine::OnTickEnd() {
  auto now = steady_clock::now();
  auto duration = now - m_engine_state.performance.last_frame_time;
  m_engine_state.performance.frame_time_ms = duration.count();
  m_engine_state.performance.fps = 1000.0f / duration.count();
}
```

---

## Summary Diagram

### Complete Organization Structure

```
┌───────────────────────────────────────────────────────────────────┐
│                             Engine                                 │
├───────────────────────────────────────────────────────────────────┤
│                                                                    │
│  ╔═══════════════════════════════════════════════════════════╗   │
│  ║               CATEGORY 1: CORE RESOURCES                   ║   │
│  ╠═══════════════════════════════════════════════════════════╣   │
│  ║  GameCore m_game_core {                                    ║   │
│  ║    - RenderWindow game_window                              ║   │
│  ║    - EventHandler event_handler                            ║   │
│  ║    - AssetManager asset_manager                            ║   │
│  ║    - Vector2i mouse_position                               ║   │
│  ║    - size_t loop_number                                    ║   │
│  ║  }                                                          ║   │
│  ║                                                             ║   │
│  ║  Purpose: Long-lived, global infrastructure                ║   │
│  ║  Access: Via GameContext references                        ║   │
│  ╚═══════════════════════════════════════════════════════════╝   │
│                                                                    │
│  ┌───────────────────────────────────────────────────────────┐   │
│  │         CATEGORY 2: CONFIGURATION                          │   │
│  ├───────────────────────────────────────────────────────────┤   │
│  │  EngineConfig m_engine_config {                            │   │
│  │    DisplayConfig display {                                 │   │
│  │      window_width, window_height, window_title,            │   │
│  │      framerate_limit, fullscreen, vsync                    │   │
│  │    }                                                        │   │
│  │    InputConfig input { /* future */ }                      │   │
│  │    AudioConfig audio { /* future */ }                      │   │
│  │  }                                                          │   │
│  │                                                             │   │
│  │  UserPreferences m_user_preferences {                      │   │
│  │    /* user settings */                                     │   │
│  │  }                                                          │   │
│  │                                                             │   │
│  │  Purpose: Loaded settings, mostly read-only                │   │
│  │  Access: Direct member access                              │   │
│  └───────────────────────────────────────────────────────────┘   │
│                                                                    │
│  ┌───────────────────────────────────────────────────────────┐   │
│  │          CATEGORY 3: ENGINE STATE                          │   │
│  ├───────────────────────────────────────────────────────────┤   │
│  │  EngineState m_engine_state {                              │   │
│  │    bool running, paused                                    │   │
│  │    vector<Subscriber> subscriptions                        │   │
│  │    bool quit_requested                                     │   │
│  │    optional<SceneType> requested_scene                     │   │
│  │    PerformanceMetrics performance                          │   │
│  │  }                                                          │   │
│  │                                                             │   │
│  │  Purpose: Runtime operational state                        │   │
│  │  Access: Direct member access                              │   │
│  └───────────────────────────────────────────────────────────┘   │
│                                                                    │
│  ┌───────────────────────────────────────────────────────────┐   │
│  │            SUBSYSTEMS (Managers)                           │   │
│  ├───────────────────────────────────────────────────────────┤   │
│  │  SceneManager m_scene_manager                              │   │
│  │  DisplayManager m_display_manager  // GameEngine only      │   │
│  │                                                             │   │
│  │  Purpose: Complex subsystems with logic                    │   │
│  │  Access: Direct member access or via interfaces            │   │
│  └───────────────────────────────────────────────────────────┘   │
│                                                                    │
└───────────────────────────────────────────────────────────────────┘

Key Principles:
✓ Clear categorization by purpose and lifetime
✓ Intuitive naming reflects category
✓ Consistent access patterns per category
✓ Easy to determine where new data belongs
```

---

## Navigation

- **Main Document**: [ENGINE_DATA_ORGANIZATION.md](ENGINE_DATA_ORGANIZATION.md)
- **Related**: [ENGINE_ARCHITECTURE_IMPROVEMENTS.md](ENGINE_ARCHITECTURE_IMPROVEMENTS.md)
- **Proposals Index**: [README.md](README.md)
