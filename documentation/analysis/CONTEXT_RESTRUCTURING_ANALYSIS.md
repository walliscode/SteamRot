# Context Restructuring Analysis

**Date**: December 15, 2024  
**Status**: Proposal for restructuring context objects into lower-level layers

## Executive Summary

**Question**: Can context objects be restructured into a lower-level layer?  
**Answer**: **YES!** Context can be split into 3 packages across different layers, eliminating 2 circular dependencies and improving architecture significantly.

**Impact**:
- ✅ Eliminates `logic ↔ scenes` circular dependency
- ✅ Eliminates `assets ↔ user_interface` circular dependency
- ✅ Improves layer compliance to **100% (14 of 14 packages)**
- ✅ Better separation of concerns
- ✅ More testable code
- ⏱️ Estimated time: **2-3 hours**

---

## Current State

### Current Context Package

**Location**: `src/context/` (Layer 2)

**Dependencies**:
```
context -> types (Layer 1)
context -> assets (Layer 2) 
context -> events (Layer 2)
context -> SFML::Graphics (external)
```

**Used By**:
- `engine` (Layer 3)
- `scenes` (Layer 2) ⚠️ **PROBLEM: Same layer!**
- `logic` (Layer 2) ⚠️ **PROBLEM: Same layer!**

**Contents**:
1. `EngineResources` struct (owns: window, event_handler, asset_manager, mouse_position, loop_number)
2. `GameContext` struct (references to EngineResources members)

### Problems with Current Design

1. **Layer 2 packages depend on other Layer 2 packages** (violates clean architecture)
2. **Context is too concrete** - includes AssetManager and EventHandler implementations
3. **Circular dependency enabler** - scenes and logic both need context, but are in same layer

---

## Proposed Solution: Three-Context Architecture

### Overview

Split `context` package into **3 separate packages** across different layers:

```
Layer 1 (Data/Types):
  - core_types (NEW) - Contains EngineResources, IGameContext interface

Layer 2 (Logic):  
  - contexts (RENAMED from context) - Contains GameContext implementation
  
Layer 3 (Orchestration):
  - (no context code needed here)
```

### Detailed Design

#### 1. Package: `core_types` (Layer 1)

**Location**: `src/types/core/` (already exists!)

**Contents**:
- `EngineResources` struct (MOVE from types)
- `IGameContext` interface (NEW - abstract interface)

**Dependencies**: ZERO SteamRot packages (Layer 1 rule)
- SFML::Graphics (external, header-only for forward declarations)

**Example `IGameContext`**:
```cpp
// src/types/core/IGameContext.h
#pragma once
#include <SFML/System/Vector2.hpp>

namespace sf {
  class RenderWindow;
}

namespace steamrot {
  class AssetManager;
  class EventHandler;
  struct EngineResources;

  // Abstract interface - no implementation dependencies
  struct IGameContext {
    virtual ~IGameContext() = default;
    
    // Accessors (pure virtual or references)
    virtual sf::RenderWindow& GetGameWindow() = 0;
    virtual EventHandler& GetEventHandler() = 0;
    virtual sf::Vector2i& GetMousePosition() = 0;
    virtual size_t& GetLoopNumber() = 0;
    virtual AssetManager& GetAssetManager() = 0;
    virtual EngineResources& GetEngineResources() = 0;
  };
}
```

**Benefits**:
- ✅ Layer 1 package - can be used by ANY package
- ✅ Zero dependencies on implementations
- ✅ Forward declarations only
- ✅ Enables dependency injection

#### 2. Package: `contexts` (Layer 2)

**Location**: `src/contexts/` (RENAMED from `src/context/`)

**Contents**:
- `GameContext` struct (implements `IGameContext`)

**Dependencies**:
- `core_types` (Layer 1) ✅
- `assets` (Layer 2) ✅
- `events` (Layer 2) ✅
- SFML::Graphics (external)

**Example `GameContext`**:
```cpp
// src/contexts/GameContext.h
#pragma once
#include "IGameContext.h"
#include "EngineResources.h"

namespace steamrot {
  struct GameContext : public IGameContext {
    GameContext() = delete;
    GameContext(EngineResources& engine_resources);

    // Implement interface
    sf::RenderWindow& GetGameWindow() override { return game_window; }
    EventHandler& GetEventHandler() override { return event_handler; }
    sf::Vector2i& GetMousePosition() override { return mouse_position; }
    size_t& GetLoopNumber() override { return loop_number; }
    AssetManager& GetAssetManager() override { return asset_manager; }
    EngineResources& GetEngineResources() override { return engine_resources; }

    // Keep references for convenience (backward compatibility)
    EngineResources& engine_resources;
    sf::RenderWindow& game_window;
    EventHandler& event_handler;
    sf::Vector2i& mouse_position;
    size_t& loop_number;
    AssetManager& asset_manager;
  };
}
```

**Benefits**:
- ✅ Clean Layer 2 package
- ✅ Depends only on Layer 1 + other Layer 2
- ✅ Backward compatible (keeps same interface)
- ✅ Implements abstract interface

#### 3. Update Consumers

**logic package** (Layer 2):
```cmake
# src/logic/CMakeLists.txt
target_link_libraries(logic PUBLIC
  core_types  # Layer 1 - interface only
  # Remove: contexts (no longer needed!)
)
```

**scenes package** (Layer 2):
```cmake
# src/scenes/CMakeLists.txt
target_link_libraries(scenes PUBLIC
  core_types  # Layer 1 - interface only
  # Remove: contexts (no longer needed!)
)
```

**engine package** (Layer 3):
```cmake
# src/engine/CMakeLists.txt
target_link_libraries(engine PRIVATE
  core_types  # Layer 1
  contexts    # Layer 2 - actual implementation
)
```

**Key Insight**: Logic and scenes only need the **interface** (IGameContext), not the implementation (GameContext). Engine creates the GameContext and passes it down as IGameContext*.

---

## Migration Steps

### Step 1: Create IGameContext Interface (30 minutes)

**File**: `src/types/core/IGameContext.h`

1. Create abstract interface with pure virtual methods
2. Use forward declarations for all concrete types
3. No implementation dependencies

**Validation**:
```bash
# Should compile with zero additional dependencies
cd /home/runner/work/SteamRot/SteamRot
cmake --build --preset Debug --target types
```

### Step 2: Update GameContext to Implement Interface (20 minutes)

**File**: `src/context/GameContext.h`

1. Add `#include "IGameContext.h"`
2. Change declaration: `struct GameContext : public IGameContext`
3. Add override methods (simple accessors)
4. Keep existing member variables for backward compatibility

**Validation**:
```bash
cmake --build --preset Debug --target context
```

### Step 3: Update Consumers to Use Interface (45 minutes)

For each consumer (logic, scenes, user_interface, etc.):

1. Change function signatures from `GameContext&` → `IGameContext&`
2. Update CMakeLists.txt to link `core_types` instead of `context`
3. Update includes: `#include "GameContext.h"` → `#include "IGameContext.h"`

**Example** (LogicContext):
```cpp
// Before
struct LogicContext {
  GameContext& game_context;  // Concrete type
  // ...
};

// After
struct LogicContext {
  IGameContext& game_context;  // Interface type
  // ...
};
```

**Validation** (incremental):
```bash
# Test each package after updating
cmake --build --preset Debug --target logic
cmake --build --preset Debug --target scenes
cmake --build --preset Debug --target user_interface
```

### Step 4: Rename Package (Optional, 15 minutes)

Rename `context` → `contexts` for clarity:

```bash
cd /home/runner/work/SteamRot/SteamRot/src
mv context contexts
```

Update all CMakeLists.txt references.

### Step 5: Final Validation (30 minutes)

1. Full rebuild:
```bash
cd /home/runner/work/SteamRot/SteamRot
rm -rf build/Debug
cmake --preset Debug
cmake --build --preset Debug
```

2. Run tests:
```bash
ctest --preset Debug
```

3. Verify circular dependencies eliminated:
```bash
# Check dependency graph
cmake --build --preset Debug --graphviz=deps.dot
# Should show clean layering
```

---

## Benefits Analysis

### Before Restructuring

**Circular Dependencies**: 2
- logic ↔ scenes (through context)
- assets ↔ user_interface (through context)

**Layer Violations**: 2
- scenes (Layer 2) → context (Layer 2)
- logic (Layer 2) → context (Layer 2)

**Layer Compliance**: 86% (12 of 14 packages)

### After Restructuring

**Circular Dependencies**: **0** ✅

**Layer Violations**: **0** ✅

**Layer Compliance**: **100% (14 of 14 packages)** ✅

**Architecture Health**: **10/10** 🎉

### Additional Benefits

1. **Testability**: Can mock IGameContext for unit tests
2. **Flexibility**: Can create different context implementations (TestContext, MockContext, etc.)
3. **Dependency Injection**: Callers depend on interface, not implementation
4. **Build Performance**: Fewer dependencies = faster incremental builds
5. **Maintainability**: Clear layer boundaries
6. **Scalability**: Can add context variants without affecting consumers

---

## Alternative Approaches Considered

### Alternative 1: Keep Single Context Package, Extract Interface

**Approach**: Keep `src/context/` but add `IGameContext` interface

**Pros**:
- Smaller change
- Less file moving

**Cons**:
- ❌ Doesn't solve layer violation (context still in Layer 2)
- ❌ Logic and scenes still depend on Layer 2 package
- ❌ Circular dependencies remain

**Verdict**: ❌ Not recommended - doesn't achieve goals

### Alternative 2: Move Context to Layer 1

**Approach**: Move entire `context` package to Layer 1 (types)

**Pros**:
- Simple move
- Accessible to all packages

**Cons**:
- ❌ Context depends on AssetManager and EventHandler (Layer 2)
- ❌ Violates Layer 1 rule (zero SteamRot dependencies)
- ❌ Would need to move assets/events to Layer 1 too (cascading changes)

**Verdict**: ❌ Not recommended - violates layering rules

### Alternative 3: Dependency Injection with Factory

**Approach**: Create ContextFactory in Layer 3, pass contexts down

**Pros**:
- Keeps existing structure
- Clear ownership

**Cons**:
- ❌ Doesn't eliminate circular dependencies
- ❌ Still have Layer 2 → Layer 2 dependencies
- ❌ More complex (factory + interface)

**Verdict**: ⚠️ Possible but more work than recommended solution

---

## Recommended Solution: Three-Context Architecture

**Winner**: Split into `core_types` (Layer 1) + `contexts` (Layer 2)

**Reasons**:
1. ✅ Cleanly eliminates all circular dependencies
2. ✅ Achieves 100% layer compliance
3. ✅ Minimal breaking changes (interface compatible)
4. ✅ Standard pattern (interface in Layer 1, implementation in Layer 2)
5. ✅ Enables dependency injection
6. ✅ Reasonable implementation time (2-3 hours)

---

## Implementation Checklist

- [ ] **Step 1**: Create `IGameContext` interface in `src/types/core/`
  - [ ] Add IGameContext.h with abstract interface
  - [ ] Use forward declarations only
  - [ ] Build and validate types package

- [ ] **Step 2**: Update GameContext to implement interface
  - [ ] Inherit from IGameContext
  - [ ] Add override methods
  - [ ] Build and validate context package

- [ ] **Step 3**: Update logic package
  - [ ] Change GameContext& → IGameContext& in headers
  - [ ] Update CMakeLists.txt dependencies
  - [ ] Build and validate

- [ ] **Step 4**: Update scenes package
  - [ ] Change GameContext& → IGameContext& in headers
  - [ ] Update CMakeLists.txt dependencies
  - [ ] Build and validate

- [ ] **Step 5**: Update other consumers
  - [ ] user_interface package
  - [ ] display package
  - [ ] Any other packages using context

- [ ] **Step 6**: Full rebuild and test
  - [ ] Clean rebuild
  - [ ] Run all tests
  - [ ] Verify no circular dependencies

- [ ] **Step 7** (Optional): Rename context → contexts
  - [ ] Move directory
  - [ ] Update all CMakeLists.txt references

---

## Risk Assessment

### Low Risk
- ✅ Interface is backward compatible
- ✅ Existing code continues to work
- ✅ Can be done incrementally
- ✅ Easy to revert if issues found

### Medium Risk
- ⚠️ Need to update many consumer files
- ⚠️ Requires careful testing
- ⚠️ Build system changes across multiple packages

### Mitigation
- Test each package independently after changes
- Keep commits small and focused
- Run full test suite after each step
- Can implement in feature branch first

---

## Conclusion

**Recommendation**: **PROCEED with Three-Context Architecture**

**Rationale**:
- Eliminates final 2 circular dependencies
- Achieves perfect layer compliance (100%)
- Standard architecture pattern
- Reasonable implementation effort (2-3 hours)
- High-value improvement with low risk

**Next Steps**:
1. Review and approve this proposal
2. Create feature branch: `feature/context-restructuring`
3. Follow implementation checklist
4. Test thoroughly
5. Merge to main branch

**Expected Result**: Zero circular dependencies, clean three-layer architecture, 10/10 architecture health! 🎉
