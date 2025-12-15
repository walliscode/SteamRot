# SteamRot Layering Architecture Diagrams

**Date**: December 15, 2025  
**Purpose**: Visual representation of layering architecture and EngineResources solution

---

## Table of Contents

1. [Three-Layer Architecture Overview](#three-layer-architecture-overview)
2. [Current State (With Violation)](#current-state-with-violation)
3. [Proposed State (After Fix)](#proposed-state-after-fix)
4. [EngineResources Migration](#engineresources-migration)
5. [Package Dependency Graph](#package-dependency-graph)
6. [Data Flow Through Layers](#data-flow-through-layers)
7. [Construction Pattern](#construction-pattern)

---

## Three-Layer Architecture Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                                                                 │
│                      LAYER 3: ORCHESTRATION                     │
│                                                                 │
│  ┌──────────────┐  ┌─────────────┐  ┌─────────────┐           │
│  │   context/   │  │   scenes/   │  │  display/   │           │
│  │              │  │             │  │             │           │
│  │ GameContext  │  │SceneManager │  │DisplayMgr   │           │
│  │SceneContext  │  │Scene classes│  │             │           │
│  └──────────────┘  └─────────────┘  └─────────────┘           │
│                           │                                     │
│                           │  ┌──────────────┐                  │
│                           └─▶│   engine/    │                  │
│                              │              │                  │
│                              │ Engine       │                  │
│                              │ GameEngine   │                  │
│                              │ TestEngine   │                  │
│                              └──────────────┘                  │
│                                                                 │
│  Purpose: Coordinate services, manage game/test flow          │
│  Rule: Depends on Layer 1 + 2                                 │
└─────────────────────────────────────────────────────────────────┘
                              ▲
                              │ depends on
                              │
┌─────────────────────────────────────────────────────────────────┐
│                                                                 │
│                    LAYER 2: LOGIC + SERVICES                    │
│                                                                 │
│  ┌──────────────┐  ┌─────────────┐  ┌─────────────┐           │
│  │   events/    │  │   assets/   │  │  entity/    │           │
│  │              │  │             │  │             │           │
│  │EventHandler  │  │AssetManager │  │EntityMgr    │           │
│  └──────────────┘  └─────────────┘  └─────────────┘           │
│                                                                 │
│  ┌──────────────┐  ┌─────────────┐  ┌─────────────┐           │
│  │   logic/     │  │data_providers│ │ resources/  │  ← NEW    │
│  │              │  │             │  │             │           │
│  │Logic classes │  │Providers    │  │EngineRes    │           │
│  └──────────────┘  └─────────────┘  └─────────────┘           │
│                                                                 │
│  Purpose: Implement interfaces, provide services              │
│  Rule: Depends ONLY on Layer 1                                │
└─────────────────────────────────────────────────────────────────┘
                              ▲
                              │ depends on
                              │
┌─────────────────────────────────────────────────────────────────┐
│                                                                 │
│                   LAYER 1: DATA + INTERFACES                    │
│                                                                 │
│  ┌──────────────┐  ┌─────────────┐  ┌─────────────┐           │
│  │  types/core/ │  │ interfaces/ │  │ components/ │           │
│  │              │  │             │  │             │           │
│  │ Config       │  │IEventHandler│  │ CMeta       │           │
│  │ structs      │  │IAssetMgr    │  │ CUserInterface │        │
│  │ Enums        │  │IProviders   │  │             │           │
│  └──────────────┘  └─────────────┘  └─────────────┘           │
│                                                                 │
│  ┌──────────────┐  ┌─────────────┐                            │
│  │types/events/ │  │  types/     │                            │
│  │              │  │user_interface│                           │
│  │EventPacket   │  │             │                            │
│  │EventType     │  │UIElement    │                            │
│  └──────────────┘  └─────────────┘                            │
│                                                                 │
│  Purpose: Pure data structures, interface definitions         │
│  Rule: ZERO dependencies on Layer 2 or 3                      │
└─────────────────────────────────────────────────────────────────┘
```

---

## Current State (With Violation)

### The Problem: EngineResources in Wrong Layer

```
┌───────────────────────────────────────────────────────────┐
│                    LAYER 1: types/core/                   │
│                                                            │
│  ┌────────────────────────────────────────────────────┐  │
│  │         EngineResources.h  ❌ VIOLATION             │  │
│  │                                                     │  │
│  │  struct EngineResources {                          │  │
│  │    sf::RenderWindow game_window;     ✅ OK        │  │
│  │    EventHandler event_handler;       ❌ Layer 2   │  │
│  │    AssetManager asset_manager;       ❌ Layer 2   │  │
│  │    sf::Vector2i mouse_position;      ✅ OK        │  │
│  │    size_t loop_number;               ✅ OK        │  │
│  │  };                                                 │  │
│  │                                                     │  │
│  │  Includes:                                          │  │
│  │  #include "EventHandler.h"    ⚠️ from events/     │  │
│  │  #include "AssetManager.h"    ⚠️ from assets/     │  │
│  └────────────────────────────────────────────────────┘  │
│                                                            │
│  ⚠️  Layer 1 code depends on Layer 2 classes              │
│  ⚠️  Violates "zero dependencies" rule                    │
│  ⚠️  Makes Layer 1 untestable in isolation                │
└───────────────────────────────────────────────────────────┘
                            │
                            │ ❌ Wrong direction
                            ▼
┌───────────────────────────────────────────────────────────┐
│                    LAYER 2: Services                      │
│                                                            │
│  ┌─────────────────────┐  ┌─────────────────────┐        │
│  │  events/            │  │  assets/            │        │
│  │                     │  │                     │        │
│  │  EventHandler.h/cpp │  │  AssetManager.h/cpp │        │
│  │                     │  │                     │        │
│  │  Complex logic,     │  │  Complex logic,     │        │
│  │  state management   │  │  state management   │        │
│  └─────────────────────┘  └─────────────────────┘        │
└───────────────────────────────────────────────────────────┘
```

### Why This Is Wrong

1. **Layer 1 includes Layer 2 headers**: Types layer pulls in service implementations
2. **Cannot test Layer 1 in isolation**: Need to build EventHandler/AssetManager
3. **Violates architectural principle**: Data layer depends on logic layer
4. **Blocks clean builds**: Can't compile types without compiling events and assets

---

## Proposed State (After Fix)

### Solution: Move EngineResources to Resources Package

```
┌───────────────────────────────────────────────────────────┐
│                    LAYER 1: types/core/                   │
│                                                            │
│  ┌────────────────────────────────────────────────────┐  │
│  │         EngineResourcesConfig.h  ✅ STAYS HERE     │  │
│  │                                                     │  │
│  │  struct EngineResourcesConfig {                    │  │
│  │    uint32_t window_width;        ✅ POD type      │  │
│  │    uint32_t window_height;       ✅ POD type      │  │
│  │    std::string window_title;     ✅ POD type      │  │
│  │    uint32_t framerate_limit;     ✅ POD type      │  │
│  │  };                                                 │  │
│  │                                                     │  │
│  │  Pure data, no includes of Layer 2                 │  │
│  └────────────────────────────────────────────────────┘  │
│                                                            │
│  ✅ Layer 1 has zero dependencies on Layer 2              │
│  ✅ Can be compiled independently                         │
│  ✅ Can be tested with zero mocks                         │
└───────────────────────────────────────────────────────────┘
                            ▲
                            │ ✅ Correct direction
                            │
┌───────────────────────────────────────────────────────────┐
│                    LAYER 2: Services                      │
│                                                            │
│  ┌─────────────────────┐  ┌─────────────────────┐        │
│  │  events/            │  │  assets/            │        │
│  │  EventHandler       │  │  AssetManager       │        │
│  └─────────────────────┘  └─────────────────────┘        │
│                                                            │
│  ┌─────────────────────────────────────────────────────┐ │
│  │         resources/EngineResources.h  ✅ MOVES HERE  │ │
│  │                                                      │ │
│  │  struct EngineResources {                           │ │
│  │    sf::RenderWindow game_window;     ✅ OK         │ │
│  │    EventHandler event_handler;       ✅ Layer 2    │ │
│  │    AssetManager asset_manager;       ✅ Layer 2    │ │
│  │    sf::Vector2i mouse_position;      ✅ OK         │ │
│  │    size_t loop_number;               ✅ OK         │ │
│  │  };                                                  │ │
│  │                                                      │ │
│  │  Includes:                                           │ │
│  │  #include "EventHandler.h"    ✅ Same layer (2)    │ │
│  │  #include "AssetManager.h"    ✅ Same layer (2)    │ │
│  └─────────────────────────────────────────────────────┘ │
│                                                            │
│  ✅ Layer 2 can depend on other Layer 2 packages          │
│  ✅ No layering violation                                 │
│  ✅ EngineResources is a "resource owner" - fits Layer 2  │
└───────────────────────────────────────────────────────────┘
```

### Benefits of This Approach

1. **Clean layer separation**: Layer 1 has zero Layer 2 dependencies
2. **Types are pure data**: EngineResourcesConfig stays in types/
3. **Resource owners in Layer 2**: EngineResources owns complex objects
4. **No circular dependencies**: Clear dependency direction
5. **Better testability**: Layer 1 can be tested independently

---

## EngineResources Migration

### Before: Single Struct in Layer 1

```
types/core/
├── EngineResources.h        ❌ Contains runtime objects
│   ├── EventHandler         ❌ Layer 2 dependency
│   └── AssetManager         ❌ Layer 2 dependency
└── EngineResourcesConfig.h  ✅ Pure data (stays)
```

### After: Split Between Layer 1 and Layer 2

```
Layer 1: types/core/
└── EngineResourcesConfig.h      ✅ Configuration data
    ├── window_width             ✅ POD type
    ├── window_height            ✅ POD type
    └── framerate_limit          ✅ POD type

Layer 2: resources/
└── EngineResources.h            ✅ Runtime container
    ├── EventHandler             ✅ Same layer
    ├── AssetManager             ✅ Same layer
    └── RenderWindow             ✅ External lib (SFML)
```

### Migration Steps

```
┌─────────────────────────────────────────────────────────┐
│  Step 1: Create resources/ package                      │
│                                                          │
│  mkdir src/resources                                     │
│  Create CMakeLists.txt                                   │
└─────────────────────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│  Step 2: Move EngineResources.h                         │
│                                                          │
│  mv types/core/EngineResources.h                        │
│     resources/EngineResources.h                         │
└─────────────────────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│  Step 3: Update all includes                            │
│                                                          │
│  Find: #include "EngineResources.h"                     │
│  Context: Files including from types/core/              │
│  Update: Still #include "EngineResources.h"             │
│          (CMake handles new path)                       │
└─────────────────────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│  Step 4: Update CMakeLists.txt files                    │
│                                                          │
│  resources/CMakeLists.txt: Link to events, assets       │
│  context/CMakeLists.txt: Add resources dependency       │
│  engine/CMakeLists.txt: Add resources dependency        │
└─────────────────────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│  Step 5: Build and test                                 │
│                                                          │
│  cmake --build --preset Debug                           │
│  ctest --preset Debug                                   │
└─────────────────────────────────────────────────────────┘
```

---

## Package Dependency Graph

### Current Dependencies (Simplified)

```
                  ┌──────────┐
                  │  engine  │  Layer 3
                  └────┬─────┘
                       │
         ┌─────────────┼─────────────┐
         │             │             │
         ▼             ▼             ▼
    ┌────────┐    ┌────────┐    ┌─────────┐
    │display │    │context │    │ scenes  │  Layer 3
    └───┬────┘    └───┬────┘    └────┬────┘
        │             │              │
        │      ┌──────┴───────┐      │
        │      │              │      │
        ▼      ▼              ▼      ▼
    ┌───────┬────────┬──────────┬────────┐
    │ logic │ entity │  assets  │ events │  Layer 2
    └───┬───┴───┬────┴─────┬────┴───┬────┘
        │       │          │        │
        │       └──────┬───┴────────┘
        │              │
        ▼              ▼
    ┌──────────┬──────────────┬────────────┐
    │components│ interfaces   │  types     │  Layer 1
    └──────────┴──────────────┴────────────┘

Legend:
  Layer 1: Pure data, interfaces
  Layer 2: Services, logic
  Layer 3: Orchestration
```

### After Fix: Clean Layers

```
                  ┌──────────┐
                  │  engine  │  Layer 3: Orchestration
                  └────┬─────┘
                       │
         ┌─────────────┼─────────────┐
         │             │             │
         ▼             ▼             ▼
    ┌────────┐    ┌────────┐    ┌─────────┐
    │display │    │context │    │ scenes  │  Layer 3
    └───┬────┘    └───┬────┘    └────┬────┘
        │             │              │
        │      ┌──────┴───────┐      │
        │      │              │      │
        ▼      ▼              ▼      ▼
    ┌───────┬────────┬───────────┬────────┬──────────┐
    │ logic │ entity │ resources │ assets │  events  │  Layer 2: Services
    └───┬───┴───┬────┴─────┬─────┴───┬────┴────┬─────┘
        │       │          │         │         │
        │       └──────────┼─────────┴─────────┘
        │                  │
        ▼                  ▼
    ┌──────────┬──────────────┬────────────┐
    │components│ interfaces   │  types     │  Layer 1: Data
    └──────────┴──────────────┴────────────┘

Key Change:
  ✅ resources/ package added to Layer 2
  ✅ EngineResources moved from types/ to resources/
  ✅ All Layer 1 packages depend only on each other
```

---

## Data Flow Through Layers

### Configuration to Runtime Objects

```
┌────────────────────────────────────────────────────────────┐
│                    DATA LOADING (Layer 1)                  │
│                                                             │
│  ┌──────────────────────────────────────────┐             │
│  │  FlatBuffers File                        │             │
│  │  engine_resources.json                   │             │
│  │  {                                        │             │
│  │    "window_width": 1920,                 │             │
│  │    "window_height": 1080,                │             │
│  │    "window_title": "SteamRot",           │             │
│  │    "framerate_limit": 60                 │             │
│  │  }                                        │             │
│  └──────────────────────────────────────────┘             │
│                      │                                     │
│                      │ Loaded by Provider                  │
│                      ▼                                     │
│  ┌──────────────────────────────────────────┐             │
│  │  EngineResourcesConfig (Layer 1)         │             │
│  │  struct {                                 │             │
│  │    uint32_t window_width = 1920;         │             │
│  │    uint32_t window_height = 1080;        │             │
│  │    std::string window_title = "...";     │             │
│  │    uint32_t framerate_limit = 60;        │             │
│  │  }                                        │             │
│  └──────────────────────────────────────────┘             │
└────────────────────────────────────────────────────────────┘
                      │
                      │ Passed to configurator
                      ▼
┌────────────────────────────────────────────────────────────┐
│                 CONFIGURATION (Layer 2)                    │
│                                                             │
│  ┌──────────────────────────────────────────┐             │
│  │  engine::ConfigureEngineResources()      │             │
│  │                                           │             │
│  │  Takes: EngineResourcesConfig (Layer 1)  │             │
│  │  Returns: EngineResources (Layer 2)      │             │
│  └──────────────────────────────────────────┘             │
│                      │                                     │
│                      │ Creates runtime objects             │
│                      ▼                                     │
│  ┌──────────────────────────────────────────┐             │
│  │  EngineResources (Layer 2)               │             │
│  │  struct {                                 │             │
│  │    sf::RenderWindow game_window;         │ ← Created   │
│  │    EventHandler event_handler;           │ ← Created   │
│  │    AssetManager asset_manager;           │ ← Created   │
│  │    sf::Vector2i mouse_position{0,0};     │             │
│  │    size_t loop_number{1};                │             │
│  │  }                                        │             │
│  └──────────────────────────────────────────┘             │
└────────────────────────────────────────────────────────────┘
                      │
                      │ Passed by reference
                      ▼
┌────────────────────────────────────────────────────────────┐
│                  USAGE (Layer 3)                           │
│                                                             │
│  ┌──────────────────────────────────────────┐             │
│  │  GameContext (Layer 3)                   │             │
│  │  struct {                                 │             │
│  │    EngineResources &engine_resources;    │ ← Reference │
│  │    sf::RenderWindow &game_window;        │ ← Reference │
│  │    EventHandler &event_handler;          │ ← Reference │
│  │    AssetManager &asset_manager;          │ ← Reference │
│  │  }                                        │             │
│  └──────────────────────────────────────────┘             │
│                      │                                     │
│                      │ Passed to systems                   │
│                      ▼                                     │
│  ┌──────────────────────────────────────────┐             │
│  │  DisplayManager, SceneManager, etc.      │             │
│  │                                           │             │
│  │  Receive GameContext by reference        │             │
│  │  Access resources via context             │             │
│  └──────────────────────────────────────────┘             │
└────────────────────────────────────────────────────────────┘
```

---

## Construction Pattern

### Top-Down Construction (Layer 3 → Layer 2 → Layer 1)

```
┌────────────────────────────────────────────────────────────┐
│                Engine::StartUp() (Layer 3)                 │
│                                                             │
│  Step 1: Load config (Layer 1)                             │
│  ┌────────────────────────────────────────┐               │
│  │ auto config =                          │               │
│  │   provider.ProvideEngineResourcesConfig(); │           │
│  │                                         │               │
│  │ Returns: EngineResourcesConfig         │               │
│  │          (Layer 1, pure data)          │               │
│  └────────────────────────────────────────┘               │
│                   │                                        │
│                   ▼                                        │
│  Step 2: Create resources (Layer 2)                       │
│  ┌────────────────────────────────────────┐               │
│  │ EngineResources m_engine_resources;    │               │
│  │                                         │               │
│  │ Members constructed:                   │               │
│  │ - EventHandler() default constructor   │               │
│  │ - AssetManager() default constructor   │               │
│  │ - sf::RenderWindow() default           │               │
│  └────────────────────────────────────────┘               │
│                   │                                        │
│                   ▼                                        │
│  Step 3: Configure resources                              │
│  ┌────────────────────────────────────────┐               │
│  │ engine::ConfigureEngineResources(      │               │
│  │   m_engine_resources,                  │               │
│  │   config                               │               │
│  │ );                                      │               │
│  │                                         │               │
│  │ Configures window, framerate, etc.     │               │
│  └────────────────────────────────────────┘               │
│                   │                                        │
│                   ▼                                        │
│  Step 4: Create context (Layer 3)                         │
│  ┌────────────────────────────────────────┐               │
│  │ GameContext game_context(              │               │
│  │   m_engine_resources                   │               │
│  │ );                                      │               │
│  │                                         │               │
│  │ Creates reference wrapper               │               │
│  └────────────────────────────────────────┘               │
│                   │                                        │
│                   ▼                                        │
│  Step 5: Pass to systems                                  │
│  ┌────────────────────────────────────────┐               │
│  │ DisplayManager display(game_context);  │               │
│  │ SceneManager scenes(game_context);     │               │
│  │                                         │               │
│  │ Systems receive lightweight context    │               │
│  └────────────────────────────────────────┘               │
└────────────────────────────────────────────────────────────┘
```

### Ownership vs Reference Flow

```
┌──────────────────────────────────────────────────────────┐
│                    OWNERSHIP                             │
│                                                           │
│  Engine (Layer 3)                                        │
│  ├─── OWNS EngineResources (Layer 2)                    │
│  │    ├─── OWNS EventHandler                            │
│  │    ├─── OWNS AssetManager                            │
│  │    └─── OWNS sf::RenderWindow                        │
│  │                                                       │
│  ├─── OWNS DisplayManager                               │
│  └─── OWNS SceneManager                                 │
│                                                           │
└──────────────────────────────────────────────────────────┘
                     │
                     │ Creates references
                     ▼
┌──────────────────────────────────────────────────────────┐
│                   REFERENCES                             │
│                                                           │
│  GameContext (Layer 3)                                   │
│  ├─── REFERENCES EngineResources                        │
│  │    ├─── REFERENCES EventHandler                      │
│  │    ├─── REFERENCES AssetManager                      │
│  │    └─── REFERENCES sf::RenderWindow                  │
│  │                                                       │
│  Passed to:                                              │
│  ├─── DisplayManager (stores context reference)         │
│  ├─── SceneManager (stores context reference)           │
│  └─── Logic classes (receive context each call)         │
│                                                           │
└──────────────────────────────────────────────────────────┘
```

---

## Summary

### Key Takeaways

1. **Three clear layers**: Data → Services → Orchestration
2. **EngineResources moves**: From types/ (Layer 1) to resources/ (Layer 2)
3. **Config stays**: EngineResourcesConfig remains in types/core/
4. **Clean dependencies**: No Layer 1 → Layer 2 violations
5. **Ownership in Engine**: Top-level constructs and owns everything
6. **References in contexts**: Lightweight wrappers for access

### Migration is Straightforward

| Step | Complexity | Time |
|------|-----------|------|
| Create resources/ package | LOW | 30 min |
| Move EngineResources.h | LOW | 15 min |
| Update includes | LOW | 30 min |
| Update CMakeLists | LOW | 30 min |
| Build & test | LOW | 30 min |
| **Total** | **LOW** | **~2-3 hours** |

---

**Document Version**: 1.0  
**Last Updated**: December 15, 2025
