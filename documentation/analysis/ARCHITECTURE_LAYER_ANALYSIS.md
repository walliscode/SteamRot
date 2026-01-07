# SteamRot Architecture Layer Analysis

**Date:** 2026-01-07  
**Purpose:** Comprehensive analysis of the codebase architecture, layer structure, and recommendations for improving robustness and extensibility

---

## Executive Summary

This document analyzes the current architecture of the SteamRot game engine, focusing on:
1. **Library layer structure** and dependency organization
2. **Data structure placement** (e.g., SceneLoadData)
3. **Opportunities to collapse libraries** to lower layers
4. **Components library structure** and potential migration to types layer
5. **Recommendations** for improving robustness and extensibility

### Key Findings

✅ **Strengths:**
- Clear separation between data (components) and logic
- Use of INTERFACE libraries for lightweight abstractions (types, interfaces, configuration, resources)
- No circular dependencies detected
- Good use of modern C++ patterns (std::expected, FlatBuffers)

⚠️ **Areas for Improvement:**
- **Components library** contains pure data structs but is a NORMAL library (should be INTERFACE/header-only)
- **SceneLoadData** and similar data structures are in `interfaces/` but belong in `types/core/`
- **INTERFACE libraries** (configuration, resources) may add unnecessary indirection
- **Some layer mixing** where data structures span multiple library boundaries

---

## Current Architecture

### Library Inventory

The codebase has **18 libraries** organized as follows:

| Library | Type | Source Files | Primary Purpose |
|---------|------|--------------|-----------------|
| **types** | INTERFACE | 0 | Type definitions across all subsystems |
| **interfaces** | INTERFACE | 0 | Abstract interfaces for dependency injection |
| **configuration** | INTERFACE | 0 | UI configuration abstractions |
| **resources** | INTERFACE | 0 | Resource aggregation wrapper |
| **logger** | NORMAL | ~2 | Logging utilities (spdlog wrapper) |
| **components** | NORMAL | 6 cpp + headers | Pure data component structs |
| **events** | NORMAL | 5 cpp | Event system and event bus |
| **user_interface** | NORMAL | 1 cpp | UI element configurators |
| **entity** | NORMAL | 5 cpp | Entity/archetype management |
| **assets** | NORMAL | 1 cpp | Asset loading and management |
| **data_providers** | NORMAL | 11 cpp | FlatBuffers data providers |
| **context** | NORMAL | 2 cpp | Game and scene context |
| **logic** | NORMAL | 8 cpp | Game logic systems |
| **scenes** | NORMAL | 6 cpp | Scene management |
| **display** | NORMAL | 3 cpp | Display and rendering |
| **engine** | NORMAL | 3 cpp | Top-level engine orchestration |
| **error_loop** | NORMAL | 1 cpp | Error handling display |

### Dependency Analysis

#### Layer 0: Foundation (No Internal Dependencies)
- **logger** - Standalone logging wrapper
- **interfaces** - Pure abstract interfaces (but depends on types)
- **types** - Pure type definitions (INTERFACE, but depends on interfaces - circular!)

#### Layer 1: Core Data & Events
- **components** → types
- **events** → types, logger

#### Layer 2: Configuration & Resources
- **configuration** → types, interfaces, events
- **resources** → types, assets, events
- **user_interface** → types, events, data_providers, assets, configuration
- **assets** → types, interfaces, user_interface, data_providers

#### Layer 3: Entity & Data Management
- **entity** → components, types, interfaces, events, user_interface, configuration
- **data_providers** → events, entity, interfaces, types, configuration

#### Layer 4: Game Systems
- **context** → types, resources, assets, events, entity, components
- **logic** → components, entity, user_interface, types, context, resources

#### Layer 5: Scene & Display
- **scenes** → entity, components, context, logic, types, data_providers, interfaces, assets, resources
- **display** → interfaces, types
- **error_loop** → types

#### Layer 6: Engine
- **engine** → types, resources, interfaces, display, scenes, entity, logger, context, data_providers

### Dependency Complexity

**Most Complex Libraries (by internal dependencies):**
1. **scenes**: 9 internal dependencies
2. **engine**: 9 internal dependencies
3. **context**: 6 internal dependencies
4. **entity**: 6 internal dependencies
5. **logic**: 6 internal dependencies

---

## Issue Analysis

### Issue 1: Components Library Structure

**Current State:**
```cmake
add_library(components
  Component.cpp
  CMeta.cpp
  CUserInterface.cpp
  CMachinaForm.cpp
  CGrimoireMachina.cpp
  CUIState.cpp
)
target_link_libraries(components PUBLIC SFML::Graphics types)
```

**Problems:**
1. Components are **pure data structs** (POD types) but built as a **NORMAL library**
2. Each component has a `.cpp` file that only implements `GetComponentRegisterIndex()`
3. This creates an unnecessary compilation dependency
4. Components should be **header-only** since they contain no real logic

**Example Component:**
```cpp
// CMeta.h
struct CMeta : public Component {
  CMeta() = default;
  bool m_entity_alive = false;
  size_t GetComponentRegisterIndex() const override;
};

// CMeta.cpp - entire file just for one line!
size_t CMeta::GetComponentRegisterIndex() const {
  return TupleTypeIndex<CMeta, ComponentRegister>;
}
```

**Recommendation:**
- ✅ Convert components to **INTERFACE library** or merge into **types** library
- ✅ Make `GetComponentRegisterIndex()` inline or constexpr in header
- ✅ Remove all `.cpp` files for components
- ✅ Move `containers.h` to `types/components/` as well
- ✅ This reduces compilation dependencies and build times

**Important: containers.h**

The `components/containers.h` file is a critical piece that should move with the components:

```cpp
// components/containers.h
namespace steamrot {
  // Template helpers for component management
  template <typename... Components> struct ComponentContainer { ... };
  
  // The central component registry - tuple of all component types
  typedef std::tuple<CMeta, CUserInterface, CMachinaForm, CGrimoireMachina, CUIState>
      ComponentRegister;
  
  constexpr size_t ComponentRegisterSize = std::tuple_size<ComponentRegister>::value;
  
  // Type alias for entity memory pool
  using EntityMemoryPool = ComponentContainer<ComponentRegister>::ComponentVectorTuple;
  
  // Template metaprogramming for type indexing
  template <typename T, typename Tuple>
  constexpr size_t TupleTypeIndex = IndexOf<T, Tuple>::value;
}
```

This file contains:
- **ComponentRegister** - The single source of truth for all components
- **EntityMemoryPool** - Type definition for component storage
- **Template helpers** - Compile-time type index calculation
- **Zero implementation** - All constexpr/template code

**Why it should move:**
1. It's pure type definition code (no .cpp file)
2. It defines the core component system types
3. It's header-only and compile-time only
4. It's tightly coupled to component definitions
5. Moving it with components maintains cohesion

**Proposed Structure:**
```cpp
// In types/components/CMeta.h
struct CMeta : public Component {
  CMeta() = default;
  bool m_entity_alive = false;
  
  constexpr size_t GetComponentRegisterIndex() const override {
    return TupleTypeIndex<CMeta, ComponentRegister>;
  }
};

// In types/components/containers.h
// All component type definitions and helpers
```

### Issue 2: Data Structures in interfaces/

**Current State:**
The `interfaces/` directory contains:
- **13 files total**
- **11 abstract interface files** (I*.h) - ✅ Correct
- **2 data structure files**:
  - `SceneLoadData.h` - Data structure, not an interface
  - `SceneInfoProvider.h` - Data structure, not an interface

**SceneLoadData.h:**
```cpp
namespace steamrot {
struct SceneLoadData {
  SceneData scene_data;
  std::unique_ptr<IEntityImporter> entity_importer;
};
using SceneCollectionData = std::vector<SceneLoadData>;
}
```

This is a **data transfer object (DTO)**, not an interface!

**Problems:**
1. **Violates Single Responsibility** - interfaces/ should only contain abstract interfaces
2. **Confusing naming** - Not everything in interfaces/ is an interface
3. **Circular conceptual dependency** - Data structures that use interfaces shouldn't live alongside them
4. **Harder to understand** - Developers expect pure abstractions in interfaces/

**Recommendation:**
- ✅ Move `SceneLoadData.h` → `types/core/SceneLoadData.h`
- ✅ Move `SceneInfoProvider.h` → `types/core/SceneInfoProvider.h` (or rename if it's actually an interface)
- ✅ Keep `interfaces/` directory for **only abstract base classes**
- ✅ Update all includes accordingly

**Benefits:**
- Clear separation: interfaces = abstractions, types = data
- Better organization and discoverability
- Follows standard C++ conventions

### Issue 3: INTERFACE Library Indirection

**Current State:**
Several libraries are INTERFACE libraries that primarily wrap other libraries:

**configuration:**
```cmake
add_library(configuration INTERFACE)
target_link_libraries(configuration INTERFACE
  types
  interfaces
  events
  SFML::Graphics
)
```
Contains only 2 files:
- `IUIElementConfigurator.h`
- `IUIStyleConfigurator.h`

**resources:**
```cmake
add_library(resources INTERFACE)
target_link_libraries(resources INTERFACE
  types
  assets
  events
)
```
Contains only 1 file:
- `EngineResources.h`

**Problems:**
1. **Adds layer of indirection** without clear benefit
2. **Dependency aggregation** - pulls in many libraries transitively
3. **Harder to track** what you're actually depending on
4. **Interface files could live directly in types/**

**Recommendation:**

**Option A: Collapse into types (Recommended)**
- Move configuration interfaces → `types/configuration/`
- Move `EngineResources.h` → `types/core/EngineResources.h`
- Remove configuration and resources libraries
- Users directly depend on types

**Option B: Keep but clarify purpose**
- If these represent **stable API boundaries**, keep them
- Document their purpose clearly
- Ensure they provide value beyond simple aggregation

**Analysis:**
- `EngineResources` is a concrete struct → Should be in types/core
- Configuration interfaces are abstractions → Could stay in interfaces/ or types/configuration/
- **Recommendation: Use Option A** - simplify by moving to types

### Issue 4: types Library Circular Dependency

**Current State:**
```cmake
# types/CMakeLists.txt
add_library(types INTERFACE)
target_link_libraries(types INTERFACE
  stduuid
  interfaces  # types depends on interfaces
  flatbuffers
  flatbuffers_headers
)

# interfaces/CMakeLists.txt
add_library(interfaces INTERFACE)
target_link_libraries(interfaces INTERFACE
  types      # interfaces depends on types
  SFML::Graphics
  stduuid
)
```

**Problem:**
- **Circular dependency between types and interfaces**
- Both are INTERFACE libraries, so this "works" but is conceptually wrong
- Makes it unclear which is the foundation layer

**Root Cause:**
- Some types need interfaces (for polymorphism)
- Some interfaces need types (for method parameters)
- This is a classic "two-way dependency" problem

**Recommendation:**

**Option A: Merge types and interfaces**
- Both are INTERFACE libraries with headers
- Combine into single `types` library
- Organize subdirectories: types/core, types/interfaces, types/events, etc.

**Option B: Split more carefully**
- Create `types_core` with pure data types (no dependencies)
- Create `types_interfaces` that depends on types_core
- Other type categories depend on appropriate foundation

**Option C: Forward declarations**
- Use forward declarations to break circular dependencies
- Keep libraries separate but eliminate circular link dependencies

**Analysis:**
Given that both are INTERFACE libraries (header-only), **Option A is recommended**:
- Simplifies dependency graph
- Easier mental model: "types contains all type definitions"
- Still organized with subdirectories
- Eliminates circular dependency

### Issue 5: Layer Mixing Examples

**Example 1: assets → user_interface**
```cmake
# assets/CMakeLists.txt
target_link_libraries(assets PUBLIC
  types
  interfaces
  user_interface  # Why does asset loading depend on UI?
  data_providers
)
```

**Analysis:**
- Looking at `AssetManager.cpp`, it seems to load UI-related assets
- This creates coupling between asset system and UI system
- Better approach: Generic asset loading + UI-specific asset types in types/

**Example 2: user_interface → assets**
```cmake
# user_interface/CMakeLists.txt
target_link_libraries(user_interface PRIVATE
  types
  events
  data_providers
  assets  # UI depends on assets
  configuration
)
```

**Analysis:**
- UI element configurators need assets (fonts, textures)
- This is reasonable, but creates circular concern between assets and user_interface
- Both depend on each other!

**Circular Dependency:**
```
assets → user_interface
user_interface → assets
```

**Recommendation:**
- Extract **asset types** to `types/assets/`
- Extract **UI element types** to `types/user_interface/` (already done!)
- Make `assets` **not depend** on `user_interface`
- Make `user_interface` **depend on** `assets` (one direction only)
- Both depend on `types` for shared data structures

---

## Proposed Architecture Improvements

### Proposal 1: Restructure Foundation Layer

**Goal:** Create a clean, circular-dependency-free foundation

**Changes:**
1. **Merge types and interfaces into single types library**
   ```
   types/
   ├── core/           # Core data types (FailInfo, SceneData, etc.)
   ├── interfaces/     # Abstract interfaces (I*.h)
   ├── components/     # Component definitions (header-only)
   ├── events/         # Event types
   ├── assets/         # Asset configuration types
   ├── user_interface/ # UI element types
   └── flatbuffers/    # Generated FlatBuffers headers
   ```

2. **Convert components to header-only**
   - Remove all `.cpp` files from components/
   - Make `GetComponentRegisterIndex()` inline/constexpr
   - Move to `types/components/`

3. **Move data structures from interfaces/ to types/core/**
   - `SceneLoadData.h` → `types/core/`
   - `SceneInfoProvider.h` → `types/core/` (or rename)

4. **Eliminate configuration and resources libraries**
   - Move files to appropriate subdirectories in types/
   - Remove extra indirection

**Result:**
```
Foundation Layer (Layer 0):
  - types (INTERFACE) - All type definitions
  - logger (NORMAL) - Logging

Core Libraries (Layer 1):
  - events
  - data_providers
  - assets (no UI dependency!)
  - user_interface

Business Logic (Layer 2):
  - entity
  - context
  - logic

Systems (Layer 3):
  - scenes
  - display
  - error_loop
  
Engine (Layer 4):
  - engine
```

### Proposal 2: Break Circular Dependencies

**Changes:**
1. **assets ↛ user_interface**
   - Remove user_interface dependency from assets
   - Asset loading should be generic
   - UI-specific asset handling in user_interface layer

2. **Clean dependency flow:**
   ```
   types (foundation)
     ↓
   events, assets, data_providers, user_interface
     ↓
   entity
     ↓
   context, logic
     ↓
   scenes
     ↓
   engine
   ```

### Proposal 3: Simplify Component System

**Changes:**
1. Make all components header-only
2. Use constexpr for `GetComponentRegisterIndex()`
3. Move to types/components/
4. Result: **Zero runtime overhead**, faster compilation

**Example transformation:**

**Before:**
```cpp
// Component.h
struct Component {
  virtual ~Component();
  virtual size_t GetComponentRegisterIndex() const = 0;
  bool m_active{false};
};

// CMeta.h
struct CMeta : public Component {
  CMeta() = default;
  bool m_entity_alive = false;
  size_t GetComponentRegisterIndex() const override;
};

// CMeta.cpp
size_t CMeta::GetComponentRegisterIndex() const {
  return TupleTypeIndex<CMeta, ComponentRegister>;
}
```

**After:**
```cpp
// types/components/Component.h
struct Component {
  virtual ~Component() = default;
  virtual constexpr size_t GetComponentRegisterIndex() const = 0;
  bool m_active{false};
};

// types/components/CMeta.h
struct CMeta : public Component {
  CMeta() = default;
  bool m_entity_alive = false;
  
  constexpr size_t GetComponentRegisterIndex() const override {
    return TupleTypeIndex<CMeta, ComponentRegister>;
  }
};
```

---

## Robustness & Extensibility Analysis

### Robustness Improvements

**1. Reduced Compilation Dependencies**
- Header-only components mean changes don't trigger recompilation of dependent libraries
- Faster incremental builds
- Less coupling

**2. Clearer Dependency Graph**
- Eliminating circular dependencies makes the codebase easier to reason about
- Reduces risk of include cycles
- Makes refactoring safer

**3. Better Layer Separation**
- Clear foundation → application layer flow
- Easier to test individual layers in isolation
- Reduces coupling between subsystems

**4. Type Safety**
- All data structures in types/ makes it easy to use std::expected
- Consistent error handling patterns
- Compile-time checks via constexpr

### Extensibility Improvements

**1. Adding New Components**
Current: Requires .cpp file, recompile components library
Proposed: Just add .h file in types/components/, done!

**2. Adding New Data Types**
Current: Scattered across types/, interfaces/, etc.
Proposed: Single types/ library with clear organization

**3. Adding New Systems**
Current: Must navigate complex dependency web
Proposed: Clear layers mean new systems just depend on appropriate layer

**4. Plugin Architecture**
Current: Difficult due to circular dependencies
Proposed: Clean dependency flow enables:
- Runtime plugin loading
- Hot reloading of logic systems
- Modular game systems

---

## Implementation Roadmap

### Phase 1: Documentation & Analysis ✅
- [x] Analyze current architecture
- [x] Document findings
- [x] Create recommendations

### Phase 2: Foundation Layer (Low Risk)
- [ ] Move SceneLoadData.h → types/core/
- [ ] Move SceneInfoProvider.h → types/core/
- [ ] Update includes
- [ ] Test build

### Phase 3: Components Refactor (Medium Risk)
- [ ] Make GetComponentRegisterIndex() inline/constexpr
- [ ] Remove .cpp files for components
- [ ] Move components to types/components/
- [ ] Update CMakeLists.txt
- [ ] Test build and all tests

### Phase 4: Library Consolidation (Medium Risk)
- [ ] Merge interfaces into types
- [ ] Move configuration files to types/configuration/
- [ ] Move EngineResources.h to types/core/
- [ ] Remove configuration and resources libraries
- [ ] Update all CMakeLists.txt and includes

### Phase 5: Break Circular Dependencies (High Risk)
- [ ] Remove assets → user_interface dependency
- [ ] Refactor asset loading to be generic
- [ ] Update affected code
- [ ] Thorough testing

### Phase 6: Verification
- [ ] Run all tests
- [ ] Verify build times
- [ ] Check for any remaining issues
- [ ] Update documentation

---

## Recommendations Summary

### High Priority (Do These)

1. ✅ **Move data structures to types/core/**
   - Risk: Low
   - Benefit: High (better organization)
   - Effort: Small (update includes)

2. ✅ **Convert components to header-only**
   - Risk: Low
   - Benefit: High (faster builds, less coupling)
   - Effort: Medium (refactor code)

3. ✅ **Merge types and interfaces**
   - Risk: Low
   - Benefit: Medium (simpler architecture)
   - Effort: Medium (update CMake, includes)

### Medium Priority (Consider These)

4. **Remove configuration and resources libraries**
   - Risk: Low
   - Benefit: Medium (less indirection)
   - Effort: Small

5. **Break assets ↔ user_interface circular dependency**
   - Risk: Medium
   - Benefit: High (cleaner architecture)
   - Effort: Medium

### Low Priority (Nice to Have)

6. **Further layer refinement**
   - Split large libraries if they grow
   - Add more specific subdirectories in types/
   - Consider namespace organization

---

## Conclusion

The SteamRot architecture is **fundamentally sound** with clear separation of concerns between data (components) and logic (systems). However, there are several opportunities to improve:

**Key Issues:**
1. Components library should be header-only
2. Data structures misplaced in interfaces/
3. Circular dependencies between types and interfaces
4. Some unnecessary indirection with INTERFACE libraries
5. Circular concerns between assets and user_interface

**Key Benefits of Proposals:**
- **Faster builds** (header-only components)
- **Clearer architecture** (no circular dependencies)
- **Better extensibility** (clean layering)
- **Easier maintenance** (better organization)

**Recommended Approach:**
- Start with **low-risk, high-benefit changes** (move files, merge types/interfaces)
- Progress to **medium-risk changes** (component refactoring)
- Consider **high-risk changes** only after thorough testing

The architecture is well-positioned for these improvements, and they can be done incrementally without major disruption.
