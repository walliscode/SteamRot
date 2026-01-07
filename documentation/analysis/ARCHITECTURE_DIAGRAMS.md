# SteamRot Architecture Diagrams

This document provides visual representations of the current and proposed architecture.

---

## Current Architecture

### Current Library Dependency Graph

```
┌─────────────────────────────────────────────────────────────────┐
│                         LAYER 6: ENGINE                         │
│                                                                 │
│  ┌──────────┐                                                   │
│  │  engine  │────────────────────────────────────┐              │
│  └──────────┘                                    │              │
└──────┬───────────────────────────────────────────┼──────────────┘
       │                                           │
┌──────┴───────────────────────────────────────────┴──────────────┐
│                   LAYER 5: SCENES & DISPLAY                     │
│                                                                 │
│  ┌─────────┐  ┌─────────┐  ┌────────────┐                      │
│  │ scenes  │  │ display │  │ error_loop │                      │
│  └─────────┘  └─────────┘  └────────────┘                      │
└──────┬───────────────────────────────────────────────────────┬──┘
       │                                                       │
┌──────┴───────────────────────────────────────────────────────┴──┐
│                   LAYER 4: GAME SYSTEMS                         │
│                                                                 │
│  ┌─────────┐  ┌───────┐                                         │
│  │ context │  │ logic │                                         │
│  └─────────┘  └───────┘                                         │
└──────┬───────────────────────────────────────────────────────┬──┘
       │                                                       │
┌──────┴───────────────────────────────────────────────────────┴──┐
│              LAYER 3: ENTITY & DATA MANAGEMENT                  │
│                                                                 │
│  ┌────────┐  ┌────────────────┐                                │
│  │ entity │  │ data_providers │                                │
│  └────────┘  └────────────────┘                                │
└──────┬───────────────────────────────────────────────────────┬──┘
       │                                                       │
┌──────┴───────────────────────────────────────────────────────┴──┐
│           LAYER 2: CONFIGURATION & RESOURCES                    │
│                                                                 │
│  ┌────────────────┐  ┌───────────┐  ┌────────┐  ┌───────────┐  │
│  │ user_interface │  │   assets  │  │ config │  │ resources │  │
│  └────────────────┘  └───────────┘  └────────┘  └───────────┘  │
│         ⇅                   ⇅                                   │
│    CIRCULAR DEPENDENCY!                                         │
└──────┬───────────────────────────────────────────────────────┬──┘
       │                                                       │
┌──────┴───────────────────────────────────────────────────────┴──┐
│               LAYER 1: CORE DATA & EVENTS                       │
│                                                                 │
│  ┌────────────┐  ┌────────┐                                     │
│  │ components │  │ events │                                     │
│  └────────────┘  └────────┘                                     │
└──────┬───────────────────────────────────────────────────────┬──┘
       │                                                       │
┌──────┴───────────────────────────────────────────────────────┴──┐
│                 LAYER 0: FOUNDATION                             │
│                                                                 │
│  ┌───────────┐  ┌────────┐  ┌────────┐                          │
│  │   types   │⇄⇄│ inter- │  │ logger │                          │
│  │(INTERFACE)│⇄⇄│ faces  │  │        │                          │
│  └───────────┘  └────────┘  └────────┘                          │
│         ⇅                                                       │
│    CIRCULAR DEPENDENCY!                                         │
└─────────────────────────────────────────────────────────────────┘

Legend:
  │  = depends on (downward)
  ⇅  = circular dependency (problem!)
  INTERFACE = header-only library
```

### Current Issues Highlighted

```
ISSUE 1: Components Library
┌──────────────────────────────┐
│  components (NORMAL library) │
│                              │
│  Component.cpp               │  ❌ Has .cpp files for
│  CMeta.cpp                   │     simple functions
│  CUserInterface.cpp          │
│  CMachinaForm.cpp            │  ❌ Built as library
│  CGrimoireMachina.cpp        │     despite being POD
│  CUIState.cpp                │
│                              │  ❌ Adds compilation
│  Dependencies: types         │     overhead
└──────────────────────────────┘

Should be:
┌──────────────────────────────┐
│ types/components/ (headers)  │
│                              │
│  Component.h (inline)        │  ✅ Header-only
│  CMeta.h (constexpr)         │
│  CUserInterface.h            │  ✅ No compilation
│  CMachinaForm.h              │     overhead
│  CGrimoireMachina.h          │
│  CUIState.h                  │  ✅ Faster builds
└──────────────────────────────┘
```

```
ISSUE 2: Data Structures in interfaces/
┌─────────────────────────────────┐
│  interfaces/ directory          │
│                                 │
│  IEngineDataProvider.h      ✅  │ Interface - correct!
│  IEntityConfigurator.h      ✅  │
│  IEntityImporter.h          ✅  │
│  ISceneConfigurator.h       ✅  │
│  ...                            │
│                                 │
│  SceneLoadData.h            ❌  │ Data struct - wrong place!
│  SceneInfoProvider.h        ❌  │ Data struct - wrong place!
└─────────────────────────────────┘

Should be:
┌─────────────────────────────────┐
│  types/interfaces/ directory    │
│                                 │
│  IEngineDataProvider.h      ✅  │
│  IEntityConfigurator.h      ✅  │
│  ...all interfaces...           │
└─────────────────────────────────┘
┌─────────────────────────────────┐
│  types/core/ directory          │
│                                 │
│  SceneLoadData.h            ✅  │
│  SceneInfoProvider.h        ✅  │
│  ...all data structs...         │
└─────────────────────────────────┘
```

```
ISSUE 3: Circular Dependencies
┌──────────┐         ┌────────────┐
│  types   │────────▶│ interfaces │
│          │         │            │
│          │◀────────│            │
└──────────┘         └────────────┘
     Both are INTERFACE libraries
     Both depend on each other
     Creates confusion about layers

┌────────────────┐         ┌──────────────────┐
│ user_interface │────────▶│      assets      │
│                │         │                  │
│                │◀────────│                  │
└────────────────┘         └──────────────────┘
     Why does asset loading need UI types?
     Why does UI need asset loading?
```

---

## Proposed Architecture

### Proposed Library Dependency Graph

```
┌─────────────────────────────────────────────────────────────────┐
│                         LAYER 4: ENGINE                         │
│                                                                 │
│  ┌──────────┐                                                   │
│  │  engine  │                                                   │
│  └──────────┘                                                   │
└──────┬──────────────────────────────────────────────────────────┘
       │
┌──────┴──────────────────────────────────────────────────────────┐
│                   LAYER 3: SCENES & DISPLAY                     │
│                                                                 │
│  ┌─────────┐  ┌─────────┐  ┌────────────┐                      │
│  │ scenes  │  │ display │  │ error_loop │                      │
│  └─────────┘  └─────────┘  └────────────┘                      │
└──────┬──────────────────────────────────────────────────────────┘
       │
┌──────┴──────────────────────────────────────────────────────────┐
│                   LAYER 2: GAME SYSTEMS                         │
│                                                                 │
│  ┌─────────┐  ┌───────┐                                         │
│  │ context │  │ logic │                                         │
│  └─────────┘  └───────┘                                         │
└──────┬──────────────────────────────────────────────────────────┘
       │
┌──────┴──────────────────────────────────────────────────────────┐
│              LAYER 1: BUSINESS LOGIC                            │
│                                                                 │
│  ┌────────┐  ┌────────────────┐  ┌────────────────┐  ┌────────┐│
│  │ entity │  │ data_providers │  │ user_interface │  │ assets ││
│  └────────┘  └────────────────┘  └────────────────┘  └────────┘│
│       ↓              ↓                    ↓              ↓      │
│  ┌────────┐  ┌──────┐                                           │
│  │ events │  │logger│                                           │
│  └────────┘  └──────┘                                           │
└──────┬──────────────────────────────────────────────────────────┘
       │
┌──────┴──────────────────────────────────────────────────────────┐
│                 LAYER 0: FOUNDATION (types)                     │
│                           INTERFACE LIBRARY                     │
│                                                                 │
│  types/                                                         │
│  ├── core/              # Core data types                       │
│  │   ├── FailInfo.h                                             │
│  │   ├── SceneData.h                                            │
│  │   ├── SceneLoadData.h      ← moved from interfaces/         │
│  │   ├── EngineResources.h    ← moved from resources/          │
│  │   └── ...                                                    │
│  ├── components/        # Component definitions (header-only)  │
│  │   ├── Component.h                                            │
│  │   ├── CMeta.h              ← moved from components/          │
│  │   ├── CUserInterface.h     ← moved from components/          │
│  │   └── ...                                                    │
│  ├── interfaces/        # Abstract interfaces                   │
│  │   ├── IEntityConfigurator.h  ← moved from interfaces/       │
│  │   ├── ISceneConfigurator.h                                   │
│  │   └── ...                                                    │
│  ├── events/            # Event types                           │
│  ├── assets/            # Asset types                           │
│  ├── user_interface/    # UI element types                      │
│  └── flatbuffers/       # Generated headers                     │
│                                                                 │
│  External deps: SFML, stduuid, flatbuffers                      │
└─────────────────────────────────────────────────────────────────┘

Legend:
  │  = depends on (downward, unidirectional)
  ↓  = depends on (clean dependency)
  No circular dependencies!
```

### Proposed Changes Summary

```
CHANGE 1: Merge Foundation Layer
┌─────────────────┐  ┌────────────┐  ┌───────────────┐
│ types (before)  │  │ interfaces │  │ configuration │
│   (INTERFACE)   │  │ (INTERFACE)│  │  (INTERFACE)  │
└─────────────────┘  └────────────┘  └───────────────┘
         ↓                  ↓                 ↓
┌────────────────────────────────────────────────────┐
│            types (after) - INTERFACE               │
│                                                    │
│  Organized subdirectories:                         │
│  - core/           (data structures)               │
│  - interfaces/     (abstract classes)              │
│  - components/     (ECS components)                │
│  - events/         (event types)                   │
│  - assets/         (asset configs)                 │
│  - user_interface/ (UI element types)              │
│  - flatbuffers/    (generated)                     │
└────────────────────────────────────────────────────┘
```

```
CHANGE 2: Component Library Transformation
BEFORE:                          AFTER:
┌──────────────────────┐        ┌──────────────────────┐
│ components/ (library)│        │ types/components/    │
│                      │        │     (headers only)   │
│  6 .cpp files        │   →    │                      │
│  6 .h files          │        │  6 .h files          │
│                      │        │  (with inline impls) │
│  Compile & link      │        │  Header-only         │
└──────────────────────┘        └──────────────────────┘

Benefits:
  ✅ No compilation overhead
  ✅ Faster builds
  ✅ No library to link
  ✅ Clearer that components are data-only
```

```
CHANGE 3: Break Circular Dependencies
BEFORE:
  assets ←→ user_interface    (circular!)
  types ←→ interfaces         (circular!)

AFTER:
  assets → types
  user_interface → types
  (all dependencies point down to types)
```

---

## Dependency Flow Comparison

### Before: Complex Web

```
                    engine
                      ↓
          ┌───────────┴───────────┐
          ↓                       ↓
       scenes                  display
          ↓
    ┌─────┴─────┐
    ↓           ↓
  logic      context
    ↓           ↓
    └─────┬─────┘
          ↓
    ┌─────┴─────────────┐
    ↓                   ↓
  entity          data_providers
    ↓                   ↓
    └─────┬─────────────┘
          ↓
    ┌─────┴─────────────┬─────────────┐
    ↓                   ↓             ↓
user_interface ←→ assets     components ← events
    ↓           ↓             ↓         ↓
    └─────┬─────┴─────────────┴─────────┘
          ↓
    ┌─────┴─────┬─────────┐
    ↓           ↓         ↓
  types ←→ interfaces  logger
          (circular!)

Problems:
- 2 circular dependencies
- assets and user_interface tightly coupled
- Unclear layer boundaries
```

### After: Clean Hierarchy

```
                    engine
                      ↓
          ┌───────────┴───────────┐
          ↓                       ↓
       scenes                  display
          ↓
    ┌─────┴─────┐
    ↓           ↓
  logic      context
    ↓           ↓
    └─────┬─────┘
          ↓
    ┌─────┴─────────────┐
    ↓                   ↓
  entity          data_providers
    ↓                   ↓
    └─────┬─────────────┘
          ↓
    ┌─────┴────┬─────────────┬─────┐
    ↓          ↓             ↓     ↓
user_interface assets    events  logger
    ↓          ↓             ↓     ↓
    └──────────┴─────────────┴─────┘
                     ↓
                   types
              (INTERFACE)
           All type definitions
         Components, interfaces,
           core types, events

Benefits:
✅ No circular dependencies
✅ Clear layer hierarchy
✅ Each library depends only on lower layers
✅ types is the single foundation
```

---

## Component System Transformation

### Current Component Implementation

```cpp
// Current: Component.h
struct Component {
  virtual ~Component();  // Needs .cpp file
  virtual size_t GetComponentRegisterIndex() const = 0;
  bool m_active{false};
};

// Current: CMeta.h
struct CMeta : public Component {
  CMeta() = default;
  bool m_entity_alive = false;
  size_t GetComponentRegisterIndex() const override;  // Needs .cpp file
};

// Current: CMeta.cpp (entire file for one line!)
#include "CMeta.h"
#include "containers.h"

size_t CMeta::GetComponentRegisterIndex() const {
  return TupleTypeIndex<CMeta, ComponentRegister>;
}

// Must compile and link components library
```

### Proposed Component Implementation

```cpp
// Proposed: types/components/Component.h
struct Component {
  virtual ~Component() = default;  // Inline destructor
  virtual constexpr size_t GetComponentRegisterIndex() const = 0;
  bool m_active{false};
};

// Proposed: types/components/CMeta.h
#include "Component.h"
#include "containers.h"

struct CMeta : public Component {
  CMeta() = default;
  bool m_entity_alive = false;
  
  // Inline, constexpr implementation
  constexpr size_t GetComponentRegisterIndex() const override {
    return TupleTypeIndex<CMeta, ComponentRegister>;
  }
};

// No .cpp file needed!
// No library to compile!
// Just include the header!
```

### Build Process Comparison

```
BEFORE:                           AFTER:

Source files:                     Source files:
  Component.cpp                     (none - headers only)
  CMeta.cpp
  CUserInterface.cpp               
  CMachinaForm.cpp                 
  CGrimoireMachina.cpp
  CUIState.cpp

Build steps:                      Build steps:
  1. Compile 6 .cpp files           (none - headers only)
  2. Create components.a/.lib
  3. Link into dependent libs
  
Every change to a component:      Every change to a component:
  → Recompile .cpp                  → No recompilation!
  → Relink components library       → Just recompile users
  → Relink dependent libraries      → Faster builds!
```

---

## Directory Structure Comparison

### Current Structure

```
src/
├── types/                    # INTERFACE library
│   ├── core/                 # Data types
│   ├── events/
│   ├── assets/
│   ├── user_interface/
│   └── flatbuffers/
├── interfaces/               # INTERFACE library
│   ├── I*.h                  # Abstract interfaces ✓
│   ├── SceneLoadData.h       # Data struct ✗ (wrong place!)
│   └── SceneInfoProvider.h   # Data struct ✗ (wrong place!)
├── configuration/            # INTERFACE library (thin wrapper)
│   ├── IUIElementConfigurator.h
│   └── IUIStyleConfigurator.h
├── resources/                # INTERFACE library (thin wrapper)
│   └── EngineResources.h
├── components/               # NORMAL library (but should be headers!)
│   ├── *.cpp                 # Implementation files ✗
│   └── *.h
├── events/                   # NORMAL library
├── user_interface/           # NORMAL library
├── assets/                   # NORMAL library
├── entity/                   # NORMAL library
├── data_providers/           # NORMAL library
├── context/                  # NORMAL library
├── logic/                    # NORMAL library
├── scenes/                   # NORMAL library
├── display/                  # NORMAL library
├── engine/                   # NORMAL library
├── error_loop/               # NORMAL library
└── logger/                   # NORMAL library
```

### Proposed Structure

```
src/
├── types/                    # INTERFACE library (merged foundation)
│   ├── core/                 # Core data structures
│   │   ├── FailInfo.h
│   │   ├── SceneData.h
│   │   ├── SceneLoadData.h       ← moved from interfaces/
│   │   ├── SceneInfoProvider.h   ← moved from interfaces/
│   │   ├── EngineResources.h     ← moved from resources/
│   │   └── ...
│   ├── components/           # Component definitions (header-only)
│   │   ├── Component.h           ← moved from components/
│   │   ├── CMeta.h               ← moved from components/
│   │   ├── CUserInterface.h      ← moved from components/
│   │   └── ...
│   ├── interfaces/           # Abstract interfaces
│   │   ├── IEntityConfigurator.h     ← moved from interfaces/
│   │   ├── ISceneConfigurator.h
│   │   ├── IUIElementConfigurator.h  ← moved from configuration/
│   │   ├── IUIStyleConfigurator.h    ← moved from configuration/
│   │   └── ...
│   ├── events/               # Event types
│   ├── assets/               # Asset types
│   ├── user_interface/       # UI element types
│   └── flatbuffers/          # Generated FlatBuffers headers
├── logger/                   # NORMAL library
├── events/                   # NORMAL library
├── user_interface/           # NORMAL library
├── assets/                   # NORMAL library
├── entity/                   # NORMAL library
├── data_providers/           # NORMAL library
├── context/                  # NORMAL library
├── logic/                    # NORMAL library
├── scenes/                   # NORMAL library
├── display/                  # NORMAL library
├── engine/                   # NORMAL library
└── error_loop/               # NORMAL library

Removed:
  ✗ interfaces/ (merged into types/interfaces/)
  ✗ configuration/ (merged into types/interfaces/)
  ✗ resources/ (moved to types/core/)
  ✗ components/ (merged into types/components/)
```

---

## Migration Strategy

### Phase-by-Phase Visualization

```
Phase 1: Low-Risk File Moves
────────────────────────────────
interfaces/SceneLoadData.h
    ↓
types/core/SceneLoadData.h

interfaces/SceneInfoProvider.h
    ↓
types/core/SceneInfoProvider.h

Risk: LOW (just file moves + include updates)
Benefit: Better organization
Time: 1-2 hours

Phase 2: Component Refactor
────────────────────────────────
components/ (NORMAL library)
  ├── Component.cpp
  ├── CMeta.cpp
  ├── *.cpp (6 files)
  └── *.h (6 files)
    ↓
types/components/ (header-only)
  ├── Component.h (inline destructor)
  ├── CMeta.h (constexpr GetComponentRegisterIndex)
  └── *.h (all inline)

Risk: MEDIUM (code refactoring required)
Benefit: Faster builds, cleaner architecture
Time: 4-6 hours

Phase 3: Merge Foundation
────────────────────────────────
types/ + interfaces/ + configuration/
    ↓
types/ (single INTERFACE library)
  ├── core/
  ├── components/
  ├── interfaces/
  └── ...

Risk: MEDIUM (many CMakeLists.txt changes)
Benefit: Simpler architecture, no circular deps
Time: 2-4 hours

Phase 4: Break Circular Dependencies
────────────────────────────────────
assets ←→ user_interface
    ↓
assets → types
user_interface → types

Risk: HIGH (code refactoring)
Benefit: Cleaner architecture, better modularity
Time: 6-8 hours

Total Estimated Time: 13-20 hours
```

---

## Benefits Summary

### Quantitative Benefits

| Metric | Before | After | Improvement |
|--------|--------|-------|-------------|
| Foundation libraries | 3 (types, interfaces, configuration) | 1 (types) | **67% reduction** |
| Circular dependencies | 2 | 0 | **100% elimination** |
| Component .cpp files | 6 | 0 | **100% reduction** |
| Compile steps for component change | ~10 libraries | 0 | **100% reduction** |
| INTERFACE libraries | 3 | 1 | **67% reduction** |

### Qualitative Benefits

**Code Organization:**
- ✅ All type definitions in one place
- ✅ Clear separation: types → business logic → systems → engine
- ✅ No circular dependencies

**Build Performance:**
- ✅ Header-only components = no compilation
- ✅ Fewer libraries to link
- ✅ Faster incremental builds

**Maintainability:**
- ✅ Easier to find types (all in types/)
- ✅ Easier to add components (just add .h file)
- ✅ Clearer dependency graph

**Extensibility:**
- ✅ Plugin architecture becomes feasible
- ✅ Hot reloading possible
- ✅ Easier to add new systems

---

## Conclusion

The proposed architecture:
1. **Eliminates complexity** - Fewer libraries, clearer structure
2. **Improves performance** - Header-only components, faster builds
3. **Enhances maintainability** - Clear organization, no circular deps
4. **Enables extensibility** - Clean layers support plugins and modularity

The migration can be done **incrementally** with low risk, and each phase delivers immediate benefits.
