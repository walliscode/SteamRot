# Build Dependency Analysis and Stratification Strategy

**Date:** December 11, 2025  
**Purpose:** Document current build dependencies and propose stratification to reduce build times  
**Status:** Analysis Complete - Implementation Pending

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Current Architecture](#current-architecture)
3. [Circular Dependencies](#circular-dependencies)
4. [Data Layer Architecture](#data-layer-architecture)
5. [Proposed Stratification Strategy](#proposed-stratification-strategy)
6. [Interface/Implementation Separation](#interfaceimplementation-separation)
7. [Component Library Strategy](#component-library-strategy)
8. [Implementation Roadmap](#implementation-roadmap)
9. [Expected Benefits](#expected-benefits)

---

## Executive Summary

### Problem Statement

The SteamRot codebase currently has significant build dependency issues that increase compilation times and make code changes propagate unnecessarily through the system. Key issues include:

1. **Circular Dependencies:** At least 10 circular dependency chains exist between internal libraries
2. **Monolithic Libraries:** Some libraries (engine, display, scenes) have too many responsibilities
3. **Incomplete Data Abstraction:** FlatBuffers-generated headers leak into many compilation units
4. **Interface/Implementation Mixing:** Headers contain implementation details that force recompilation

### Proposed Solution

A **three-layer stratification strategy** to create clear dependency boundaries:

```
Layer 1: Data & Interfaces (foundation)
  ├── Serialization layer (FlatBuffers schemas → binary)
  ├── Native data structures (pure C++ structs)
  └── Abstract interfaces (no implementation)

Layer 2: Implementation (business logic)
  ├── Concrete implementations of interfaces
  ├── System logic (rendering, collision, events)
  └── Data transformation (FlatBuffers → Native structs)

Layer 3: Orchestration (high-level)
  ├── Factories and builders
  ├── Scene and engine management
  └── Application entry point
```

### Key Metrics

| Metric | Current | Target | Impact |
|--------|---------|--------|--------|
| Circular dependencies | 10+ | 0 | Build stability |
| Internal libraries | 16 | 20-25 | Better separation |
| Components depending on FlatBuffers | 14 | 3-5 | Reduced recompilation |
| Average build time (clean) | Baseline | -30-40% | Developer productivity |

---

## Current Architecture

### Library Inventory

The SteamRot codebase consists of 16 internal libraries:

```
src/
├── config              # Configuration paths (1 file)
├── logger              # Logging infrastructure (2 files)
├── components          # Pure data components (8 files)
├── flatbuffers_headers # Generated FlatBuffers code (32 schemas → 32 headers)
├── data_handlers       # File I/O and FlatBuffers loading (4 files)
├── data_providers      # Provider pattern for data access (14 files)
├── configuration       # Configuration providers (4 files)
├── entity              # Entity-Component System (4 files)
├── events              # Event system (7 files)
├── user_interface      # UI elements (2 files)
├── ui_styles           # UI styling (1 file in separate library)
├── context             # Context objects (2 files)
├── assets              # Asset management (1 file)
├── logic               # Game logic (21 files)
├── scenes              # Scene management (8 files)
├── display             # Display management (3 files)
└── engine              # Main engine (3 files)
```

### Current Dependency Graph

```
┌─────────────────────────────────────────────────────────────┐
│                         engine                               │
│  (depends on: display, entity, events, config, logger,      │
│   context, logic, assets, configuration, data_providers)    │
└─────────────────────────────────────────────────────────────┘
        │
        ├─────────────────────┬──────────────────────┬─────────
        │                     │                      │
        ▼                     ▼                      ▼
┌────────────┐        ┌──────────────┐      ┌──────────────┐
│  display   │ ◄────► │    scenes    │      │    logic     │
│            │        │              │      │              │
└────────────┘        └──────────────┘      └──────────────┘
        │                     │                      │
        ▼                     ▼                      ▼
┌─────────────────────────────────────────────────────────┐
│              Components & Data Layers                    │
│  (components, entity, data_providers, data_handlers,    │
│   events, user_interface, assets, configuration)        │
└─────────────────────────────────────────────────────────┘
        │
        ▼
┌─────────────────────────────────────────────────────────┐
│              Foundation Libraries                        │
│  (logger, config, flatbuffers_headers)                  │
└─────────────────────────────────────────────────────────┘
```

### Dependency Analysis by Library

#### Layer Classification (Current State)

**Foundation (should have minimal dependencies):**
- `config` ✅ - 0 internal dependencies
- `logger` ✅ - 0 internal dependencies  
- `flatbuffers_headers` ✅ - 0 internal dependencies (generated code)

**Data Layer (should depend only on foundation):**
- `components` ⚠️ - depends on logger, user_interface (PROBLEM: should not depend on user_interface)
- `data_handlers` ⚠️ - depends on components, logger (PROBLEM: should not depend on components)
- `configuration` ✅ - depends on data_handlers only

**Interface Layer (should depend on data layer):**
- `data_providers` ⚠️ - depends on data_handlers, components, events (PROBLEM: events dependency)
- `entity` ✅ - depends on data_handlers, components, logger
- `events` ✅ - depends on logger only
- `assets` ✅ - depends on data_handlers, logger

**Implementation Layer (can depend on interfaces):**
- `user_interface` ⚠️ - depends on engine, events (CIRCULAR: engine→user_interface→engine)
- `ui_styles` ⚠️ - depends on logger, data_handlers (reasonable, but could be improved)
- `logic` ⚠️ - depends on components, entity, engine, ui_styles, user_interface (CIRCULAR with engine)
- `context` ⚠️ - depends on engine (CIRCULAR: engine→context→engine)

**Orchestration Layer (should depend on everything below):**
- `scenes` ⚠️ - depends on display, entity, config, logger, context, logic (CIRCULAR with display)
- `display` ⚠️ - depends on scenes, engine, logic (CIRCULAR with scenes and engine)
- `engine` ⚠️ - depends on everything (appropriate for top level, but creates many circular deps)

---

## Circular Dependencies

### Identified Circular Dependency Chains

```
1. display ↔ scenes
   display → scenes → display

2. engine ↔ context
   engine → context → engine

3. engine ↔ display
   engine → display → engine

4. components → ... → components (long chain)
   components → user_interface → engine → display → scenes → entity → data_handlers → components

5. logic ↔ engine (through components)
   logic → components → user_interface → engine → display → logic

6. Multiple variations through engine as central hub
```

### Root Causes

1. **Forward References Not Used:** Header files include full definitions when forward declarations would suffice
2. **Concrete Types in Interfaces:** Interfaces use concrete types instead of abstract interfaces
3. **God Objects:** `engine` library is too large and central
4. **Mixed Responsibilities:** Some libraries have both interface and implementation concerns
5. **Context Objects Depend on Engine:** Context should be data-only, not depend on engine headers

### Critical Path Analysis

The most problematic dependency is **components → user_interface**:

```
components/CUserInterface.h includes:
  → UIElement.h (from user_interface)
    → engine headers (SceneContext, etc.)
      → circular dependencies propagate
```

**Impact:** Any change to components forces recompilation of nearly entire codebase.

---

## Data Layer Architecture

### Current FlatBuffers Architecture

```
┌──────────────────────────────────────────────────────────┐
│                FlatBuffers Layer                         │
│                                                          │
│  32 .fbs schemas → flatc compiler → 32 *_generated.h    │
│                                                          │
│  Categories:                                             │
│  - events/ (5 schemas)                                  │
│  - logic/ (1 schema)                                    │
│  - testing/ (4 schemas)                                 │
│  - configuration/ (3 schemas)                           │
│  - assets/ (1 schema)                                   │
│  - engine/ (4 schemas)                                  │
│  - core/ (1 schema)                                     │
│  - entities/ (8 schemas)                                │
└──────────────────────────────────────────────────────────┘
           │
           │ Direct inclusion in 14 libraries
           ▼
┌──────────────────────────────────────────────────────────┐
│         Application Code (Implementation)                │
│                                                          │
│  Problem: FlatBuffers headers included everywhere       │
│  Impact: Changes to .fbs force wide recompilation       │
└──────────────────────────────────────────────────────────┘
```

### Provider Pattern (Partial Implementation)

The codebase has **partially implemented** a provider pattern to abstract FlatBuffers:

**Implemented Providers (7):**
1. `IEngineDataProvider` / `FlatbuffersEngineDataProvider`
2. `ISceneDataProvider` / `FlatbuffersSceneDataProvider`
3. `IAssetDataProvider` / `FlatbuffersAssetDataProvider`
4. `IFragmentDataProvider` / `FlatbuffersFragmentDataProvider`
5. `ISceneManagerDataProvider` / `FlatbuffersSceneManagerDataProvider`
6. `IUserPreferencesProvider` / `FlatbuffersUserPreferencesProvider`
7. `ISaveDataProvider` / `FlatbuffersSaveDataProvider`

**Pattern Structure:**
```cpp
// Interface (header-only, native C++ types)
class IEngineDataProvider {
public:
  virtual std::expected<EngineResourcesConfigData, FailInfo>
    LoadEngineResourcesConfig() const = 0;
  // Uses native C++ struct, NOT FlatBuffers types
};

// Implementation (in .cpp file, hides FlatBuffers)
class FlatbuffersEngineDataProvider : public IEngineDataProvider {
  // FlatBuffers headers only in .cpp file
  std::expected<EngineResourcesConfigData, FailInfo>
    LoadEngineResourcesConfig() const override {
    // Load FlatBuffers data
    // Convert to native struct
    // Return native struct
  }
};
```

**Coverage:** ~75% complete
- ✅ Engine data (config, resources, state)
- ✅ Scene data
- ✅ Asset data
- ✅ Fragment/Joint data
- ✅ Save data
- ✅ User preferences
- ✅ Scene manager data
- ❌ Context data (still uses FlatBuffers directly)
- ❌ UI style data (still uses FlatBuffers directly)
- ❌ Logic data (still uses FlatBuffers directly)

### Memory Ownership in Provider Pattern

The repository memories indicate that **FlatbuffersDataLoader** is becoming a "god object" with 13+ methods. The provider pattern is the correct solution (75% complete), but needs to be extended to the remaining data types.

---

## Proposed Stratification Strategy

### Three-Layer Architecture

#### Layer 1: Data & Interfaces (Foundation)

**Goal:** Minimal, stable foundation that rarely changes

**Libraries:**
```
foundation/
├── types                    # Core types, enums (NEW)
│   ├── FailInfo.h
│   ├── SceneType.h
│   ├── Actions.h
│   └── Common types used everywhere
│
├── data_schemas             # FlatBuffers schemas only (RENAMED)
│   ├── *.fbs files
│   └── Binary compilation via CMake
│
├── data_structures          # Native C++ data structs (NEW)
│   ├── EngineConfigData.h
│   ├── SceneData.h
│   ├── ComponentData.h
│   └── Pure data, no methods, no dependencies
│
├── interfaces               # Abstract interfaces (NEW)
│   ├── IEngineDataProvider.h
│   ├── ISceneDataProvider.h
│   ├── IEntityConfigurator.h
│   ├── ILogicStep.h
│   └── Pure virtual, uses data_structures types
│
├── config                   # Build config (KEEP AS IS)
├── logger                   # Logging (KEEP AS IS)
└── components               # Pure data components (REFACTOR)
    └── Remove user_interface dependency
```

**Key Principles:**
- No circular dependencies allowed within this layer
- No implementation code in headers
- Changes here should be rare
- Everything above depends on this

#### Layer 2: Implementation (Business Logic)

**Goal:** Implementations that can change without affecting interfaces

**Libraries:**
```
implementation/
├── data_providers           # Provider implementations (KEEP, EXPAND)
│   ├── FlatbuffersEngineDataProvider.cpp
│   ├── FlatbuffersSceneDataProvider.cpp
│   └── Hide FlatBuffers in .cpp files
│
├── data_handlers            # File I/O (REFACTOR)
│   └── Remove components dependency, keep FlatBuffers loading
│
├── entity_system            # ECS implementation (RENAME entity)
│   ├── EntityManager
│   ├── ArchetypeManager
│   └── EntityConfigurators
│
├── event_system             # Event system (RENAME events)
│
├── logic_system             # Game logic (RENAME logic)
│   ├── Logic base class
│   ├── LogicFactory
│   └── Specific logic implementations
│
├── ui_system                # UI elements & styles (MERGE user_interface + ui_styles)
│   ├── UIElement hierarchy
│   ├── UIElementFactory
│   └── StylesConfigurator
│
├── asset_system             # Assets (RENAME assets)
│
├── context_impl             # Context implementations (NEW)
│   ├── GameContextBuilder
│   └── SceneContextBuilder
│
└── configuration            # Configuration loading (KEEP)
```

**Key Principles:**
- Depends on Layer 1 only
- No circular dependencies within layer
- FlatBuffers headers ONLY in .cpp files
- Each library has single responsibility

#### Layer 3: Orchestration (High-Level)

**Goal:** Coordinate lower layers, entry points

**Libraries:**
```
orchestration/
├── scene_management         # Scene logic (REFACTOR scenes)
│   ├── SceneManager
│   ├── SceneFactory
│   ├── Scene base class
│   └── Specific scene implementations
│
├── display_management       # Display (REFACTOR display)
│   ├── DisplayManager
│   ├── Session
│   └── Rendering coordination
│
└── engine_core              # Engine (REFACTOR engine)
    ├── Engine base class
    ├── GameEngine
    └── TestEngine
```

**Key Principles:**
- Depends on Layer 1 and Layer 2
- No business logic here, only coordination
- Acceptable to depend on many lower-level libraries
- Changes here don't force recompilation of lower layers

### Dependency Flow (Allowed)

```
Layer 3 (Orchestration)
    │
    │ depends on
    ▼
Layer 2 (Implementation)
    │
    │ depends on
    ▼
Layer 1 (Data & Interfaces)

❌ FORBIDDEN: Layer 1 depends on Layer 2 or 3
❌ FORBIDDEN: Layer 2 depends on Layer 3
✅ ALLOWED: Layer 3 depends on Layer 2 depends on Layer 1
```

---

## Interface/Implementation Separation

### Current Problems

Many libraries mix interface and implementation in headers:

**Problem Example: Context**
```cpp
// context/GameContext.h (current)
#include "Engine.h"  // Pulls in many headers
#include "AssetManager.h"
#include "EventHandler.h"

class GameContext {
  sf::RenderWindow& game_window;     // Concrete type
  AssetManager& asset_manager;        // Concrete type
  EventHandler& event_handler;        // Concrete type
  // ... methods with implementations in header
};
```

**Impact:** Any change to Engine, AssetManager, or EventHandler forces recompilation of everything that includes GameContext.h.

### Proposed Solution: Interface Headers

**Step 1: Extract Interfaces**

```cpp
// interfaces/IAssetProvider.h (NEW)
#pragma once
#include "AssetData.h"  // From data_structures

class IAssetProvider {
public:
  virtual ~IAssetProvider() = default;
  virtual const sf::Texture* GetTexture(const std::string& name) const = 0;
  virtual const sf::Font* GetFont(const std::string& name) const = 0;
};

// implementation/asset_system/AssetManager.h
#include "IAssetProvider.h"

class AssetManager : public IAssetProvider {
  // Implementation details
};
```

**Step 2: Use Forward Declarations**

```cpp
// interfaces/IGameContext.h (NEW)
#pragma once

// Forward declarations
namespace sf {
  class RenderWindow;
}

class IAssetProvider;
class IEventHandler;

class IGameContext {
public:
  virtual ~IGameContext() = default;
  virtual sf::RenderWindow& GetWindow() = 0;
  virtual IAssetProvider& GetAssets() = 0;
  virtual IEventHandler& GetEvents() = 0;
};
```

**Step 3: Implementation in .cpp**

```cpp
// context_impl/GameContext.cpp
#include "GameContext.h"
#include "AssetManager.h"  // Full definition only in .cpp
#include "EventHandler.h"   // Full definition only in .cpp

GameContext::GameContext(/* ... */) {
  // Implementation
}
```

### Benefits

1. **Reduced Recompilation:** Changes to AssetManager don't require recompiling GameContext users
2. **Faster Compilation:** Headers are smaller and include less
3. **Better Encapsulation:** Implementation details hidden
4. **Easier Testing:** Can mock interfaces without complex dependencies

### Application to All Systems

| System | Current State | Proposed Interface | Implementation |
|--------|---------------|-------------------|----------------|
| Assets | AssetManager (concrete) | IAssetProvider | AssetManager : IAssetProvider |
| Events | EventHandler (concrete) | IEventHandler | EventHandler : IEventHandler |
| Entity | EntityManager (concrete) | IEntityManager | EntityManager : IEntityManager |
| Data Loading | FlatbuffersDataLoader (concrete) | I*DataProvider (7 interfaces exist) | Flatbuffers*Provider (7 exist) |
| Logic | Logic (partially abstract) | ILogicStep | Logic implementations |
| Context | GameContext (concrete) | IGameContext | GameContext : IGameContext |

---

## Component Library Strategy

### Current State of Components

**Location:** `src/components/`

**Files:**
- Component.h/cpp (base struct)
- CMeta.h/cpp
- CUserInterface.h/cpp
- CMachinaForm.h/cpp
- CGrimoireMachina.h/cpp
- CUIState.h/cpp
- Fragment.h/cpp
- Joint.h/cpp
- containers.h (ComponentRegister tuple)

**Dependencies (PROBLEM):**
```
components
  → logger (OK - for logging)
  → user_interface (PROBLEM - creates circular dependency)
  → flatbuffers (OK - for data types)
  → flatbuffers_headers (PROBLEM - should be in .cpp only)
  → SFML::Graphics (OK - for sf::Vector2f, sf::Color, etc.)
```

**The Problem:**

`CUserInterface.h` includes `UIElement.h` from `user_interface` library, which includes:
- `SceneContext.h` → `engine` headers
- `EventHandler.h` → `events` headers
- Creates massive dependency chain

### Why Components Must Remain Separate

Components are the **core data structures** of the Entity-Component System. They must be:

1. **Pure Data:** No behavior, only state
2. **Widely Used:** Referenced by entity system, logic, scenes, UI, etc.
3. **Stable:** Changes should be rare
4. **Fast to Compile:** Included in many compilation units
5. **Testable:** Easy to create and compare in tests

### Proposed Solution: Break CUserInterface Dependency

**Problem:**
```cpp
// components/CUserInterface.h (current)
#include "UIElement.h"  // From user_interface library

struct CUserInterface : public Component {
  std::unique_ptr<UIElement> root_ui_element;  // Concrete type
  // ...
};
```

**Solution 1: Forward Declaration + Pointer (RECOMMENDED)**

```cpp
// components/CUserInterface.h (proposed)
#include "Component.h"
#include <memory>

// Forward declaration
class UIElement;

struct CUserInterface : public Component {
  std::unique_ptr<UIElement> root_ui_element;  // Pointer to incomplete type
  // ...
  
  // Destructor defined in .cpp where UIElement is complete
  ~CUserInterface();
};

// components/CUserInterface.cpp
#include "CUserInterface.h"
#include "UIElement.h"  // Full definition in .cpp

CUserInterface::~CUserInterface() = default;
```

**Benefits:**
- Breaks circular dependency
- CUserInterface.h no longer pulls in user_interface headers
- Components library becomes truly independent
- No functional changes, only header organization

**Solution 2: Abstract UIElement Interface (MORE COMPLEX)**

```cpp
// interfaces/IUIElement.h
class IUIElement {
public:
  virtual ~IUIElement() = default;
  virtual void Draw(sf::RenderTarget& target) const = 0;
  // ... other pure virtual methods
};

// components/CUserInterface.h
#include "IUIElement.h"

struct CUserInterface : public Component {
  std::unique_ptr<IUIElement> root_ui_element;
  // ...
};
```

**Trade-offs:**
- More work to implement
- More abstraction layers
- Better long-term design
- Allows alternative UI implementations

**Recommendation:** Start with Solution 1 (forward declaration), evolve to Solution 2 if needed.

### Additional Component Refactoring

**Remove FlatBuffers from Headers:**

```cpp
// components/CMeta.h (current)
#include "meta_generated.h"  // FlatBuffers header

// components/CMeta.h (proposed)
// No FlatBuffers includes in header
struct CMeta : public Component {
  std::string name;
  uint32_t entity_id;
  // ...
};

// entity/FlatbuffersEntityConfigurator.cpp
#include "CMeta.h"
#include "meta_generated.h"  // Only in configurator
```

**Benefits:**
- Component headers faster to compile
- Changes to FlatBuffers schemas don't force component recompilation
- Clear separation: components are runtime, FlatBuffers are serialization

### Target Component Library Structure

```
components/ (Layer 1 library)
├── Component.h/cpp          # Base struct
├── CMeta.h/cpp              # No FlatBuffers headers
├── CUserInterface.h/cpp     # Forward declare UIElement
├── CMachinaForm.h/cpp       # No FlatBuffers headers
├── CGrimoireMachina.h/cpp   # No FlatBuffers headers
├── CUIState.h/cpp           # No FlatBuffers headers
├── Fragment.h/cpp           # No FlatBuffers headers
├── Joint.h/cpp              # No FlatBuffers headers
└── containers.h             # ComponentRegister tuple

Dependencies:
  ✅ logger (minimal, for error messages)
  ✅ SFML::Graphics (for types like sf::Vector2f)
  ❌ user_interface (REMOVED)
  ❌ flatbuffers_headers (MOVED to .cpp files)
```

### Implementation Steps for Component Independence

1. **Phase 1: Forward Declarations**
   - Add forward declaration of UIElement in CUserInterface.h
   - Move UIElement.h include to CUserInterface.cpp
   - Define CUserInterface destructor in .cpp
   - Test compilation

2. **Phase 2: Remove FlatBuffers from Headers**
   - Remove all #include "*_generated.h" from component headers
   - Move FlatBuffers conversions to configurator .cpp files
   - Verify components compile without flatbuffers_headers

3. **Phase 3: Test Independence**
   - Create test that includes only Component.h
   - Verify no transitive includes of user_interface or flatbuffers
   - Document new component independence

4. **Phase 4: Update CMakeLists**
   - Remove user_interface from components target_link_libraries
   - Remove flatbuffers_headers from PUBLIC (keep PRIVATE if needed)
   - Verify build still works

---

## Implementation Roadmap

### Phase 1: Foundation Cleanup (Weeks 1-2)

**Goal:** Establish Layer 1 without circular dependencies

**Tasks:**

1. **Create New Foundation Libraries**
   ```
   - Create src/types/ for common types
   - Create src/data_structures/ for native C++ structs
   - Create src/interfaces/ for abstract interfaces
   ```

2. **Fix Component Library**
   - Remove user_interface dependency (forward declaration)
   - Remove FlatBuffers from headers (move to .cpp)
   - Update CMakeLists.txt
   - Test independence

3. **Extract Data Structures**
   - Move native structs from provider headers to data_structures/
   - Ensure no implementation in data structure files
   - Document ownership and lifecycle

4. **Consolidate Interfaces**
   - Move existing I*Provider interfaces to interfaces/
   - Create missing interfaces (IGameContext, IEventHandler, etc.)
   - Document interface contracts

**Deliverables:**
- 4 new CMake libraries (types, data_structures, interfaces, plus refactored components)
- 0 circular dependencies in Layer 1
- Documentation of Layer 1 structure

**Success Criteria:**
- Can compile Layer 1 independently
- Layer 1 compilation time < 10 seconds
- No transitive FlatBuffers dependencies in Layer 1 headers

### Phase 2: Provider Pattern Completion (Weeks 3-4)

**Goal:** Complete provider pattern for all data types

**Tasks:**

1. **Create Missing Providers**
   - IContextDataProvider / FlatbuffersContextDataProvider
   - IUIStyleProvider / FlatbuffersUIStyleProvider  
   - ILogicDataProvider / FlatbuffersLogicDataProvider

2. **Refactor FlatbuffersDataLoader**
   - Remove methods covered by providers
   - Keep only provider factory functionality
   - Document migration path

3. **Update Consumer Code**
   - Update code to use providers instead of FlatbuffersDataLoader
   - Ensure FlatBuffers headers only in provider .cpp files
   - Add tests for new providers

**Deliverables:**
- 3 new provider interfaces + implementations
- Refactored FlatbuffersDataLoader
- Updated consumer code
- Provider pattern at 100% coverage

**Success Criteria:**
- FlatBuffers headers only in 3-5 .cpp files (providers + tests)
- Zero direct FlatbuffersDataLoader usage outside providers
- All data access through interfaces

### Phase 3: Break Circular Dependencies (Weeks 5-7)

**Goal:** Eliminate all circular dependencies

**Tasks:**

1. **Break engine ↔ context**
   - Create IGameContext interface
   - Make context depend only on interfaces
   - Update engine to use IGameContext

2. **Break display ↔ scenes**
   - Extract ISceneManager interface
   - Make display depend on ISceneManager
   - Update scenes to implement interface

3. **Break engine ↔ display**
   - Extract IDisplayManager interface
   - Reduce engine dependency on display
   - Update display coordination

4. **Break logic ↔ engine (through components)**
   - Already fixed by Phase 1 component work
   - Verify no residual dependencies
   - Update logic to use interfaces

5. **Refactor Context Objects**
   - Make contexts pure data (structs with references)
   - Remove all method implementations from context headers
   - Move builders to separate library

**Deliverables:**
- 4 new interface headers
- Refactored context implementation
- Updated CMakeLists.txt dependency chains
- Dependency graph showing 0 cycles

**Success Criteria:**
- Zero circular dependencies (verified with tools)
- Each library compilable independently
- Clear layer boundaries enforced

### Phase 4: Library Restructuring (Weeks 8-10)

**Goal:** Reorganize libraries into clean three-layer structure

**Tasks:**

1. **Rename and Move Libraries**
   - events → event_system
   - entity → entity_system
   - assets → asset_system
   - logic → logic_system
   - Merge user_interface + ui_styles → ui_system

2. **Create Layer Directories**
   ```
   src/
   ├── layer1_foundation/
   │   ├── types/
   │   ├── data_structures/
   │   ├── interfaces/
   │   ├── components/
   │   ├── config/
   │   └── logger/
   ├── layer2_implementation/
   │   ├── data_providers/
   │   ├── data_handlers/
   │   ├── entity_system/
   │   ├── event_system/
   │   ├── logic_system/
   │   ├── ui_system/
   │   ├── asset_system/
   │   └── context_impl/
   └── layer3_orchestration/
       ├── scene_management/
       ├── display_management/
       └── engine_core/
   ```

3. **Update Build System**
   - Reorganize src/CMakeLists.txt to reflect layers
   - Add layer-specific CMake include guards
   - Enforce layer dependencies via CMake

4. **Update Include Paths**
   - Update #include statements to reflect new structure
   - Use consistent include style (<layer/library/header.h>)
   - Update tests to match new structure

**Deliverables:**
- Reorganized directory structure
- Updated CMake build system
- Updated include statements throughout codebase
- Migration guide for developers

**Success Criteria:**
- Clear layer separation in directory structure
- CMake enforces layer dependencies
- All tests pass after reorganization
- Documentation updated

### Phase 5: Build Optimization (Weeks 11-12)

**Goal:** Optimize compilation speed

**Tasks:**

1. **Implement Unity Builds**
   - Group .cpp files for each library
   - Configure CMake unity builds
   - Measure compilation time improvement

2. **Precompiled Headers**
   - Identify most-included stable headers
   - Create PCH for each layer
   - Configure CMake to use PCH

3. **Forward Declaration Audit**
   - Scan all headers for unnecessary includes
   - Replace with forward declarations where possible
   - Document forward declaration guidelines

4. **Header Organization**
   - Split large headers into smaller units
   - Separate interface from implementation
   - Use PIMPL idiom where beneficial

**Deliverables:**
- Unity build configuration
- Precompiled headers for each layer
- Reduced header dependencies
- Compilation time report

**Success Criteria:**
- Clean build time reduced by 30-40%
- Incremental build time reduced by 50-60%
- Developer productivity improved

### Phase 6: Validation and Documentation (Week 13)

**Goal:** Verify improvements and document new architecture

**Tasks:**

1. **Dependency Verification**
   - Generate dependency graphs
   - Verify no circular dependencies
   - Verify layer boundaries
   - Measure build times

2. **Create Architecture Documentation**
   - Layer architecture guide
   - Library responsibility matrix
   - Dependency flow diagrams
   - Migration guide from old structure

3. **Update Developer Guidelines**
   - How to add new libraries
   - Layer rules and constraints
   - Provider pattern usage
   - Forward declaration best practices

4. **Create Enforcement Tools**
   - CMake dependency checker
   - Pre-commit hook for layer violations
   - Documentation generator

**Deliverables:**
- Architecture documentation
- Developer guidelines
- Enforcement tools
- Build metrics report

**Success Criteria:**
- All documentation complete and reviewed
- Tools prevent layer violations
- Team trained on new architecture
- Metrics show expected improvements

---

## Expected Benefits

### Build Time Improvements

**Conservative Estimates:**

| Build Type | Current | Target | Improvement |
|------------|---------|--------|-------------|
| Clean build (full) | 100% | 60-70% | 30-40% faster |
| Incremental (1 file) | 100% | 40-50% | 50-60% faster |
| Layer 1 only | 100% | 20-30% | 70-80% faster |

**Factors:**
- Fewer header dependencies reduces parsing
- FlatBuffers isolation prevents cascade recompilation
- Unity builds and PCH provide additional gains
- Layer isolation enables parallel compilation

### Development Workflow Improvements

1. **Faster Iteration**
   - Change component → rebuild only component users (not entire codebase)
   - Change FlatBuffers schema → rebuild only providers
   - Change logic → rebuild logic + orchestration (not foundation)

2. **Easier Testing**
   - Layer 1 libraries testable in isolation
   - Mock interfaces for testing Layer 2
   - Integration tests at Layer 3

3. **Better Code Organization**
   - Clear responsibility boundaries
   - Easier to find code
   - Simpler onboarding for new developers

4. **Reduced Coupling**
   - Changes don't propagate unnecessarily
   - Refactoring is safer
   - Can replace implementations without breaking consumers

### Maintenance Benefits

1. **Dependency Management**
   - Easy to see what depends on what
   - Clear impact analysis for changes
   - Can upgrade libraries independently

2. **Code Quality**
   - Forced separation of concerns
   - Interface-based design encourages SOLID principles
   - Easier to reason about code

3. **Long-term Sustainability**
   - Architecture scales as codebase grows
   - Can add new layers if needed
   - Clear patterns for new code

---

## Appendices

### Appendix A: Dependency Graph Generation

Use this Python script to generate dependency graphs:

```python
# tools/generate_dependency_graph.py
import re
import os
from graphviz import Digraph

def parse_cmake_dependencies(src_dir):
    # Implementation from earlier analysis
    pass

def generate_graph(dependencies, output_file):
    dot = Digraph(comment='Library Dependencies')
    
    # Add nodes
    for lib in dependencies.keys():
        dot.node(lib, lib)
    
    # Add edges
    for lib, deps in dependencies.items():
        for dep in deps:
            if dep in dependencies:  # Internal dependency
                dot.edge(lib, dep)
    
    dot.render(output_file, format='png')

# Usage
deps = parse_cmake_dependencies('src/')
generate_graph(deps, 'docs/dependency_graph')
```

### Appendix B: Layer Verification Script

CMake script to enforce layer dependencies:

```cmake
# cmake/VerifyLayerDependencies.cmake

set(LAYER1_LIBS types data_structures interfaces components config logger)
set(LAYER2_LIBS data_providers data_handlers entity_system event_system 
                logic_system ui_system asset_system context_impl)
set(LAYER3_LIBS scene_management display_management engine_core)

function(verify_layer_dependency target)
  get_target_property(deps ${target} LINK_LIBRARIES)
  
  # Determine layer of target
  if(${target} IN_LIST LAYER1_LIBS)
    # Layer 1 can't depend on Layer 2 or 3
    foreach(dep ${deps})
      if(${dep} IN_LIST LAYER2_LIBS OR ${dep} IN_LIST LAYER3_LIBS)
        message(FATAL_ERROR 
          "Layer violation: ${target} (Layer 1) depends on ${dep} (Layer 2/3)")
      endif()
    endforeach()
  elseif(${target} IN_LIST LAYER2_LIBS)
    # Layer 2 can't depend on Layer 3
    foreach(dep ${deps})
      if(${dep} IN_LIST LAYER3_LIBS)
        message(FATAL_ERROR 
          "Layer violation: ${target} (Layer 2) depends on ${dep} (Layer 3)")
      endif()
    endforeach()
  endif()
endfunction()
```

### Appendix C: Forward Declaration Guidelines

**When to Use Forward Declarations:**

✅ **Use forward declaration when:**
- You only use pointers or references to the type
- The type appears in function parameters or return types
- The type is a member pointer
- You don't need the size or methods of the type

❌ **Cannot use forward declaration when:**
- You need the size of the type (member by value)
- You call methods on the type
- You inherit from the type
- You instantiate the type with new/make_unique in the header

**Example:**
```cpp
// Good: Forward declaration
class UIElement;

class CUserInterface {
  std::unique_ptr<UIElement> root;  // Pointer - OK
  UIElement* GetRoot();              // Pointer return - OK
};

// Bad: Needs full definition
class CUserInterface {
  UIElement root;  // By value - needs full definition
};
```

### Appendix D: CMake Unity Build Configuration

```cmake
# Enable unity builds for faster compilation
set(CMAKE_UNITY_BUILD ON)
set(CMAKE_UNITY_BUILD_BATCH_SIZE 8)

# Per-target unity build configuration
add_library(components ...)
set_target_properties(components PROPERTIES
  UNITY_BUILD ON
  UNITY_BUILD_BATCH_SIZE 4  # Smaller for frequently changed libraries
)
```

### Appendix E: Build Time Measurement

```bash
#!/bin/bash
# tools/measure_build_time.sh

# Clean build
rm -rf build/
time cmake --preset Debug
time cmake --build --preset Debug

# Incremental build (touch one file)
touch src/components/Component.cpp
time cmake --build --preset Debug

# Layer 1 only
time cmake --build --preset Debug --target components
```

---

## Conclusion

This analysis identifies a clear path to reducing build dependencies and improving compilation times in the SteamRot codebase. The proposed three-layer architecture, combined with interface/implementation separation and completion of the provider pattern, will:

1. **Eliminate circular dependencies** (currently 10+)
2. **Reduce header coupling** through interfaces and forward declarations  
3. **Isolate FlatBuffers changes** to a small number of provider implementations
4. **Improve build times** by an estimated 30-40% for clean builds, 50-60% for incremental
5. **Enhance maintainability** through clear layer boundaries and separation of concerns

The 13-week implementation roadmap provides a structured approach with measurable deliverables at each phase. Early phases focus on foundation cleanup and breaking circular dependencies, which will deliver immediate benefits, while later phases optimize for long-term maintainability.

**Estimated Total Effort:** 13 weeks (260 hours)  
**Risk Level:** Medium - Requires careful refactoring but no algorithmic changes  
**Impact:** High - Significantly improves developer productivity and code quality

---

## References

- [COMPREHENSIVE_CODEBASE_ANALYSIS.md](./COMPREHENSIVE_CODEBASE_ANALYSIS.md) - Current state analysis
- [Provider Pattern Documentation](../DATA_PROVIDER_SYSTEM.md) - Existing provider pattern
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) - Style guidelines
- [Professional CMake: A Practical Guide](https://crascit.com/professional-cmake/) - CMake best practices
