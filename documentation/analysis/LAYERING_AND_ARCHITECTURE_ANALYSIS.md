# SteamRot Layering and Architecture Analysis

**Date**: December 15, 2025  
**Type**: Architectural Analysis  
**Purpose**: Document current codebase structure, layering architecture, and address EngineResources construction issue

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Current Layer Architecture](#current-layer-architecture)
3. [Package Dependency Analysis](#package-dependency-analysis)
4. [EngineResources Construction Problem](#engineresources-construction-problem)
5. [Proposed Layering Model](#proposed-layering-model)
6. [Solutions for EngineResources](#solutions-for-engineresources)
7. [Migration Path](#migration-path)
8. [Architectural Principles](#architectural-principles)

---

## Executive Summary

### Key Findings

1. **Mixed Layering**: SteamRot has partial layering but lacks clear architectural boundaries
2. **EngineResources Problem**: Lives in data layer but requires Level 2 classes (EventHandler, AssetManager) for construction
3. **Circular Dependencies**: Several packages have bidirectional dependencies
4. **Context Pattern Success**: GameContext/SceneContext provide good reference-based access

### Current State

- **Types Package**: Intended as Layer 1 (zero-dependency data), but violated by EngineResources
- **Logic/Entity Packages**: Mixed Layer 2 responsibilities
- **Engine Package**: Layer 3 orchestration, depends on all layers

### Recommendations

1. **Extract Interfaces**: Create pure interface package for EventHandler, AssetManager
2. **Split EngineResources**: Separate into EngineResourcesConfig (data) and EngineResources (runtime)
3. **Enforce Layer Rules**: Types → Interfaces → Logic → Systems → Orchestration
4. **Use Dependency Injection**: Pass constructed objects rather than constructing internally

---

## Current Layer Architecture

### Intended Design (from Documentation)

```
Layer 1: Data/Types (zero dependencies)
  ├── types/
  ├── interfaces/
  └── components/

Layer 2: Logic/Providers (depends on Layer 1 only)
  ├── logic/
  ├── data_providers/
  ├── entity/
  └── events/

Layer 3: Orchestration (depends on Layer 1+2)
  ├── scenes/
  ├── display/
  └── engine/
```

### Actual Implementation

**Layer 1 (Data/Types)**
- ✅ `types/` - Mostly pure data (BUT: includes EngineResources)
- ✅ `interfaces/` - Pure interfaces (depends only on types)
- ✅ `components/` - Pure data containers

**Layer 2 (Logic)**
- ⚠️ `events/` - EventHandler has complex logic
- ⚠️ `assets/` - AssetManager has complex logic
- ⚠️ `logic/` - Depends on entity, context (Layer 2.5?)
- ⚠️ `data_providers/` - Clean provider pattern
- ⚠️ `entity/` - Depends on data_providers, user_interface

**Layer 3 (Systems)**
- ⚠️ `context/` - Depends on Layer 2 (assets, events, entity)
- ⚠️ `scenes/` - Depends on everything
- ⚠️ `display/` - Depends on scenes, logic
- ⚠️ `engine/` - Top-level orchestration

### Problems Identified

#### Problem 1: EngineResources in Types Layer

**Location**: `src/types/core/EngineResources.h`

**Issue**: Contains concrete Level 2 objects:
```cpp
struct EngineResources {
  sf::RenderWindow game_window;      // OK - SFML type
  EventHandler event_handler;        // ❌ Level 2 class
  AssetManager asset_manager;        // ❌ Level 2 class
  sf::Vector2i mouse_position;       // OK - POD type
  size_t loop_number;                // OK - POD type
};
```

**Why This Violates Layering**:
- Types layer (Layer 1) should have zero dependencies on logic layer (Layer 2)
- EventHandler lives in `src/events/` (Layer 2)
- AssetManager lives in `src/assets/` (Layer 2)
- To construct EngineResources, you need to include Level 2 headers

#### Problem 2: Context Depends on Level 2

**Location**: `src/context/`

**Issue**: GameContext/SceneContext need EngineResources, but EngineResources includes Level 2 classes:
```cpp
// context/CMakeLists.txt
target_link_libraries(context
  PRIVATE
  types      # Includes EngineResources with EventHandler/AssetManager
  assets     # Also links directly
  events     # Also links directly
  entity
  components
)
```

#### Problem 3: Circular Dependencies

Several packages have circular or bidirectional dependencies:

**assets ↔ data_providers**:
- assets depends on data_providers
- This creates tight coupling

**entity ↔ data_providers**:
- entity depends on data_providers
- Could be unidirectional (entity uses providers)

**logic ↔ context**:
- logic depends on context
- context depends on entity (which logic also depends on)

---

## Package Dependency Analysis

### Complete Dependency Graph

```
Internal Package Dependencies:
============================================================
Layer 1 (Data/Interfaces):
  types                -> (none)
  interfaces           -> types
  components           -> types

Layer 2 (Logic/Services):
  events               -> types
  data_providers       -> events, interfaces, types
  
Layer 2.5 (Resources/Entity):
  assets               -> types, interfaces, user_interface, data_providers
  entity               -> components, types, interfaces, events, data_providers, user_interface
  
Layer 2.7 (Context - should be lower):
  context              -> types, assets, events, entity, components

Layer 3 (Systems):
  logic                -> components, entity, user_interface, types, context
  scenes               -> entity, context, logic, types, data_providers, interfaces
  display              -> scenes, logic
  engine               -> display, entity, events, context, logic, assets, data_providers, types
```

### Dependency Violations

| From Package | To Package | Issue | Severity |
|-------------|------------|-------|----------|
| types | events (via EngineResources) | Layer 1 → Layer 2 | HIGH |
| types | assets (via EngineResources) | Layer 1 → Layer 2 | HIGH |
| context | assets | Layer 2 → Layer 2 (OK but tight) | MEDIUM |
| context | events | Layer 2 → Layer 2 (OK but tight) | MEDIUM |
| assets | data_providers | Bidirectional potential | LOW |
| logic | context | Layer 2 → Layer 2 (OK) | LOW |

### What Should Depend On What

**Ideal Layering**:
```
Layer 1: Pure Data (Zero Dependencies)
  types/core/          # POD structs, enums
  interfaces/          # Pure abstract interfaces
  
Layer 2: Services (Depends on Layer 1)
  events/              # EventHandler implementation
  assets/              # AssetManager implementation
  data_providers/      # Data loading
  
Layer 3: Domain Logic (Depends on Layer 1+2)
  components/          # Component data
  entity/              # Entity management
  logic/               # Game logic
  
Layer 4: Context (Depends on Layer 1+2+3)
  context/             # Reference wrappers
  
Layer 5: Orchestration (Depends on Layer 1+2+3+4)
  scenes/              # Scene management
  display/             # Display management
  engine/              # Top-level orchestration
```

---

## EngineResources Construction Problem

### The Problem Statement

**Question**: "How do we deal with EngineResources which needs knowledge of level 2 classes to construct?"

**Current Situation**:

1. **EngineResources lives in `types/core/`** (Layer 1)
2. **EngineResources contains Level 2 objects**:
   - `EventHandler event_handler;` (from `src/events/`)
   - `AssetManager asset_manager;` (from `src/assets/`)
3. **To construct EngineResources, you must**:
   - Include `EventHandler.h` and `AssetManager.h`
   - This pulls in Level 2 dependencies into Layer 1
4. **This creates a layering violation**

### Why This Happened

**Historical Context**:
- EngineResources was created to **own** engine-level resources
- It replaced the old "Core" classes (GameCore)
- The goal was to have a single struct that owns all engine resources
- It was placed in `types/core/` because it's a "resource struct"

**Design Intent**:
- EngineResources should be the **ownership container**
- GameContext/SceneContext provide **reference access**
- This separation allows contexts to be lightweight

**The Conflict**:
- To own EventHandler and AssetManager, EngineResources must include their headers
- But EventHandler and AssetManager are complex Level 2 classes
- This violates the "types layer has zero dependencies" principle

### Current Usage Pattern

```cpp
// In Engine::StartUp()
EngineResources m_engine_resources;  // Contains EventHandler, AssetManager

// Configure from data
engine::ConfigureEngineResources(m_engine_resources, config_data);

// Create lightweight context
GameContext game_context(m_engine_resources);

// Pass context to systems
DisplayManager display_manager(game_context);
SceneManager scene_manager(game_context, scene_data);
```

### Why Not Just Move EngineResources?

**Option Considered**: Move EngineResources out of `types/core/`

**Problems**:
1. Where does it go? Engine package? But it's used by context...
2. GameContext/SceneContext are in `src/context/`, need EngineResources
3. Creates more circular dependencies
4. Doesn't solve the fundamental issue

**The Real Issue**: EngineResources has two responsibilities:
1. **Data structure** (config, layout) - belongs in types
2. **Ownership container** (runtime objects) - belongs in systems

---

## Proposed Layering Model

### Three-Layer Architecture (Recommended)

```
┌─────────────────────────────────────────────────────────────┐
│                     Layer 1: Data + Interfaces              │
│                                                             │
│  Purpose: Pure data structures and interface definitions   │
│  Rule: ZERO dependencies on Layer 2 or 3                   │
│                                                             │
│  Packages:                                                  │
│  ├── types/core/         # POD structs (NO complex objects) │
│  │   ├── EngineResourcesConfig  # Config data only        │
│  │   ├── SceneConfig                                       │
│  │   ├── FailInfo                                          │
│  │   └── (All config/data structs)                         │
│  │                                                          │
│  ├── types/events/       # Event data structures           │
│  │                                                          │
│  ├── types/user_interface/ # UIElement data                │
│  │                                                          │
│  ├── interfaces/         # Abstract interfaces             │
│  │   ├── IEventHandler  # NEW - interface for EventHandler│
│  │   ├── IAssetManager  # NEW - interface for AssetManager│
│  │   ├── IEngineDataProvider                               │
│  │   └── (All provider interfaces)                         │
│  │                                                          │
│  └── components/         # Pure data components            │
│      ├── CMeta, CUserInterface, etc.                       │
│      └── (No logic, only data + GetComponentRegisterIndex)│
└─────────────────────────────────────────────────────────────┘
                            │
                            │ Can depend on
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                     Layer 2: Logic + Services               │
│                                                             │
│  Purpose: Implementations of interfaces and game logic     │
│  Rule: Depends ONLY on Layer 1                             │
│                                                             │
│  Packages:                                                  │
│  ├── events/             # EventHandler implementation      │
│  │   └── EventHandler : public IEventHandler               │
│  │                                                          │
│  ├── assets/             # AssetManager implementation      │
│  │   └── AssetManager : public IAssetManager               │
│  │                                                          │
│  ├── data_providers/     # Data loading implementations    │
│  │   ├── FlatbuffersEngineDataProvider                     │
│  │   └── (All provider implementations)                    │
│  │                                                          │
│  ├── entity/             # Entity management               │
│  │   ├── EntityManager                                     │
│  │   └── ArchetypeManager                                  │
│  │                                                          │
│  ├── logic/              # Game logic classes              │
│  │   ├── UIActionLogic, UIRenderLogic, etc.               │
│  │   └── (Logic implementations)                           │
│  │                                                          │
│  └── resources/          # NEW - Resource ownership        │
│      ├── EngineResources # NOW contains concrete objects   │
│      └── SceneResources  # Owns scene-level resources      │
└─────────────────────────────────────────────────────────────┘
                            │
                            │ Can depend on
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                  Layer 3: Systems + Orchestration           │
│                                                             │
│  Purpose: Coordinate Layer 2 components, high-level flow   │
│  Rule: Depends on Layer 1 and 2                            │
│                                                             │
│  Packages:                                                  │
│  ├── context/            # Reference wrappers              │
│  │   ├── GameContext    # References EngineResources       │
│  │   └── SceneContext   # References scene+engine resources│
│  │                                                          │
│  ├── scenes/             # Scene management                │
│  │   ├── SceneManager                                      │
│  │   └── Scene implementations                             │
│  │                                                          │
│  ├── display/            # Display management              │
│  │   └── DisplayManager                                    │
│  │                                                          │
│  └── engine/             # Top-level orchestration         │
│      ├── Engine (abstract)                                 │
│      ├── GameEngine                                        │
│      └── TestEngine                                        │
└─────────────────────────────────────────────────────────────┘
```

### Key Principles

1. **Layer 1 has ZERO runtime dependencies**
   - Only data structures, enums, interfaces
   - No `#include` of Layer 2 or 3 headers
   - Can be compiled independently

2. **Layer 2 implements interfaces from Layer 1**
   - EventHandler implements IEventHandler
   - AssetManager implements IAssetManager
   - Providers implement I*Provider interfaces

3. **Layer 3 orchestrates Layer 2**
   - Creates instances of Layer 2 classes
   - Passes them as interface references
   - Manages lifetime and dependencies

4. **Dependency Injection throughout**
   - Constructors take interface references
   - No internal object construction in low layers
   - Top layer (Engine) constructs everything

---

## Solutions for EngineResources

### Solution 1: Split EngineResources (RECOMMENDED)

**Approach**: Separate configuration data from runtime objects

#### Step 1: Keep Config in Layer 1

**File**: `src/types/core/EngineResourcesConfig.h`

```cpp
namespace steamrot {

// Pure data struct - remains in types/core/
struct EngineResourcesConfig {
  uint32_t window_width{800};
  uint32_t window_height{600};
  std::string window_title{"SteamRot"};
  uint32_t framerate_limit{60};
};

} // namespace steamrot
```

**Status**: ✅ Already exists, already in Layer 1, no changes needed

#### Step 2: Move EngineResources to New Package

**Create**: `src/resources/` package (Layer 2)

**File**: `src/resources/EngineResources.h`

```cpp
#pragma once

#include "EventHandler.h"   // Layer 2 - OK now
#include "AssetManager.h"   // Layer 2 - OK now
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Runtime container for engine-level resources.
///
/// Owns all engine-level objects (window, event handler, asset manager).
/// Lives in Layer 2 (resources package), can depend on other Layer 2 classes.
///
/// Constructed via dependency injection - receives already-constructed
/// EventHandler and AssetManager instances, or constructs them internally.
/////////////////////////////////////////////////
struct EngineResources {
  
  /////////////////////////////////////////////////
  /// @brief Default constructor - creates default instances
  /////////////////////////////////////////////////
  EngineResources() = default;

  /////////////////////////////////////////////////
  /// @brief The game window.
  /////////////////////////////////////////////////
  sf::RenderWindow game_window;

  /////////////////////////////////////////////////
  /// @brief Global event handler.
  /////////////////////////////////////////////////
  EventHandler event_handler;

  /////////////////////////////////////////////////
  /// @brief Current mouse position.
  /////////////////////////////////////////////////
  sf::Vector2i mouse_position{0, 0};

  /////////////////////////////////////////////////
  /// @brief Loop number.
  /////////////////////////////////////////////////
  size_t loop_number{1};

  /////////////////////////////////////////////////
  /// @brief Asset manager.
  /////////////////////////////////////////////////
  AssetManager asset_manager;
};

} // namespace steamrot
```

**Why This Works**:
- EngineResources now lives in Layer 2 (resources package)
- Can freely include EventHandler.h and AssetManager.h (also Layer 2)
- No layering violation

#### Step 3: Update Context Package

**File**: `src/context/GameContext.h`

```cpp
#pragma once

#include "EngineResources.h"  // Now from resources/ not types/
#include <SFML/Graphics/RenderWindow.hpp>

namespace steamrot {

struct GameContext {
  GameContext() = delete;
  GameContext(EngineResources &engine_resources);

  EngineResources &engine_resources;
  sf::RenderWindow &game_window;
  EventHandler &event_handler;
  AssetManager &asset_manager;
  sf::Vector2i &mouse_position;
  size_t &loop_number;
};

} // namespace steamrot
```

**Changes**:
- Include path changes from `types/core/EngineResources.h` to `resources/EngineResources.h`
- No other code changes needed
- Context still provides reference access

#### Step 4: Update CMakeLists

**File**: `src/resources/CMakeLists.txt` (NEW)

```cmake
add_library(resources
  INTERFACE
)

target_include_directories(resources
  INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(resources
  INTERFACE
  types          # For config structs
  events         # For EventHandler
  assets         # For AssetManager
)
```

**File**: `src/context/CMakeLists.txt` (UPDATED)

```cmake
target_link_libraries(context
  PRIVATE
  resources      # NEW - for EngineResources
  types
  entity
  components
)
```

**File**: `src/engine/CMakeLists.txt` (UPDATED)

```cmake
target_link_libraries(engine PUBLIC
  resources      # NEW - for EngineResources
  # ... rest unchanged
)
```

### Solution 2: Interface-Based EngineResources (Alternative)

**Approach**: EngineResources holds interfaces, not concrete types

#### Overview

```cpp
// types/core/EngineResources.h (Layer 1)
struct EngineResources {
  sf::RenderWindow game_window;
  IEventHandler &event_handler;    // Interface reference
  IAssetManager &asset_manager;    // Interface reference
  sf::Vector2i mouse_position{0, 0};
  size_t loop_number{1};
};
```

**Pros**:
- EngineResources can stay in types layer
- Only depends on interfaces (Layer 1)
- More flexible (can swap implementations)

**Cons**:
- References require careful lifetime management
- Must construct EventHandler/AssetManager elsewhere
- More complex construction logic
- Breaks current pattern (contexts take EngineResources by reference)

**Why Not Recommended**:
- Complicates ownership model
- EngineResources meant to **own** resources
- References defeat the purpose of an ownership container

### Solution 3: Keep Status Quo, Document Exception (NOT Recommended)

**Approach**: Accept that EngineResources is a "special case"

**Rationale**:
- EngineResources is the **only** struct in types that needs Level 2
- It's a core "resource container" with specific purpose
- Document it as architectural exception

**Why Not Recommended**:
- Violates clean architecture principles
- Makes automated dependency checking impossible
- Sets bad precedent for future code
- Confuses developers about layering rules

---

## Migration Path

### Phase 1: Create Resources Package (Week 1)

**Tasks**:
1. Create `src/resources/` directory
2. Create `src/resources/CMakeLists.txt`
3. Move `EngineResources.h` from `types/core/` to `resources/`
4. Update include paths in all files (search/replace)
5. Update CMakeLists.txt link dependencies
6. Build and test

**Files to Update**:
```bash
# Find all files including EngineResources
grep -r "EngineResources.h" src/

# Update include paths
# Old: #include "EngineResources.h"
# New: #include "EngineResources.h"  (from resources/ not types/core/)
```

**Risk**: LOW - Mechanical refactor, no logic changes

### Phase 2: Update Documentation (Week 1)

**Tasks**:
1. Update architecture diagrams
2. Update layer descriptions
3. Update README.md
4. Create quick reference guide

**Files to Update**:
- `documentation/analysis/ARCHITECTURE_DIAGRAMS.md`
- `documentation/configuration/RESOURCE_CONTEXT_ARCHITECTURE.md`
- `README.md`
- `documentation/analysis/LAYERING_QUICK_REFERENCE.md` (NEW)

### Phase 3: Create Interface Abstractions (Week 2-3)

**Optional Enhancement**: Extract interfaces for better testability

**Tasks**:
1. Create `IEventHandler` interface in `interfaces/`
2. Create `IAssetManager` interface in `interfaces/`
3. Make EventHandler/AssetManager inherit from interfaces
4. Update test mocks to use interfaces

**Benefits**:
- Easier mocking in tests
- Clearer contracts
- Future-proof for alternative implementations

**Risk**: MEDIUM - More invasive, requires careful implementation

### Phase 4: Enforce Layering Rules (Week 4)

**Tasks**:
1. Create dependency checking script
2. Add to CI/CD pipeline
3. Document layer rules in CONTRIBUTING.md
4. Add layer rules to Copilot instructions

**Script Example**:
```python
# Check that types/ package only depends on external libs
# Check that Layer 2 only depends on Layer 1
# etc.
```

---

## Architectural Principles

### Principle 1: Dependency Direction

**Rule**: Dependencies flow downward only

```
Engine (Layer 3)
  ↓ depends on
Context (Layer 2.5)
  ↓ depends on
Logic/Services (Layer 2)
  ↓ depends on
Data/Interfaces (Layer 1)
```

**Never**:
- Layer 1 depends on Layer 2
- Layer 2 depends on Layer 3

### Principle 2: Data vs. Runtime Separation

**Data Structs** (Layer 1):
- Configuration data (EngineResourcesConfig, SceneConfig)
- POD types (ints, strings, enums)
- Interface definitions (abstract classes with no implementation)

**Runtime Objects** (Layer 2+):
- Complex classes with behavior (EventHandler, AssetManager)
- Stateful objects that own resources
- Implementations of interfaces

### Principle 3: Ownership vs. Reference

**Ownership Containers**:
- Own the actual objects (EventHandler, AssetManager)
- Live in Layer 2+ (resources, engine)
- Example: EngineResources

**Reference Wrappers**:
- Hold references to owned objects
- Lightweight, cheap to copy
- Example: GameContext, SceneContext

### Principle 4: Construction Flows Downward

**Top Layer (Engine)** constructs everything:
```cpp
// Engine owns and constructs
EngineResources engine_resources;
ConfigureEngineResources(engine_resources, config);

// Pass by reference to lower layers
GameContext game_context(engine_resources);
SceneManager scene_manager(game_context);
```

**Lower layers** receive via constructor injection:
```cpp
class SceneManager {
public:
  SceneManager(const GameContext &context);  // Receives reference
private:
  const GameContext &m_context;
};
```

### Principle 5: Test Independence

**Layer 1** can be tested with zero mocking:
- Pure data structures
- Interfaces compile independently

**Layer 2** can be tested with interface mocks:
- EventHandler can be mocked via IEventHandler
- AssetManager can be mocked via IAssetManager

**Layer 3** can be tested with Layer 2 mocks:
- Engine can be tested with mock EventHandler/AssetManager
- No need for full system

---

## Summary

### Current State

- **Partial layering** with violations
- **EngineResources in wrong layer** (types vs resources)
- **Dependency graph shows circular patterns**

### Recommended Solution

1. **Create resources/ package** in Layer 2
2. **Move EngineResources** from types/core/ to resources/
3. **Update all include paths** (mechanical refactor)
4. **Document layering rules** clearly
5. **Optional: Extract interfaces** for EventHandler/AssetManager

### Expected Benefits

- ✅ Clean three-layer architecture
- ✅ No layering violations
- ✅ Clear dependency direction
- ✅ Easier testing (can mock Layer 2)
- ✅ Better onboarding (clear rules)

### Risk Assessment

| Phase | Risk Level | Mitigation |
|-------|-----------|------------|
| Create resources/ package | LOW | Additive change |
| Move EngineResources | MEDIUM | Careful include path updates |
| Update documentation | LOW | Documentation only |
| Extract interfaces | MEDIUM | Requires careful design |

---

## Next Steps

1. **Approve approach**: Review and approve this analysis
2. **Create resources package**: Start with Phase 1 (1-2 days)
3. **Move EngineResources**: Mechanical refactor (1 day)
4. **Update docs**: Keep documentation current (1 day)
5. **Test thoroughly**: Ensure no regressions (1 day)

**Total Estimated Time**: 1 week for core migration

---

**Document Version**: 1.0  
**Last Updated**: December 15, 2025
