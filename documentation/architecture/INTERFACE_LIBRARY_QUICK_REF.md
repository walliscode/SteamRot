# Interface Library Quick Reference

## TL;DR - What Can Be Moved?

### ✅ YES - Move to Interface Libraries (13 structures)

**interface_core** (8 structures, zero external deps):
1. `FailInfo` + `FailMode` enum (logger/FailInfo.h)
2. `DisplayConfig` (engine/EngineConfig.h)
3. `UserPreferencesConfig` (engine/EngineConfig.h)
4. `EngineConfig` (engine/EngineConfig.h)
5. `SceneConfig` (scenes/SceneConfig.h)
6. `SceneState` (scenes/SceneState.h)
7. `SceneManagerConfig` (scenes/SceneManagerConfig.h)
8. `SceneManagerState` (scenes/SceneManagerState.h)

**interface_data_providers** (5 interfaces):
1. `IAssetDataProvider` (data_providers/IAssetDataProvider.h)
2. `IFragmentDataProvider` (data_providers/IFragmentDataProvider.h)
3. `ISceneManagerDataProvider` (data_providers/ISceneManagerDataProvider.h)
4. `IEngineDataProvider` (data_providers/IEngineDataProvider.h)
5. `ISubscriberViewer` (data_providers/ISubscriberViewer.h)

---

### ❌ NO - Cannot Be Moved (39+ structures)

**Reason: SFML dependencies (25+)**
- EventPacket, UserInputBitset, EventHandler
- EngineResources, GameContext, SceneContext, SceneResources
- UIElement, all UI style structs
- Display/rendering classes
- Logic, Scene, Engine implementation classes

**Reason: FlatBuffers dependencies (10+)**
- All `Flatbuffers*` implementation classes
- AssetManager, UIElement, Fragment
- Generated headers (*_generated.h)

**Reason: UUID dependencies (5)**
- SceneInfo, EventPacket
- Scene, SceneManager, SceneFactory

**Reason: Complex implementation (14)**
- EntityManager, ArchetypeManager
- Logic, Scene, Engine classes
- Various manager/factory classes

---

## Why This Matters

### Problem: Circular Dependencies in CMake
```
components → logger → config
events → logger
entity → components, events
logic → entity, engine
engine → logic  ← CIRCULAR!
```

### Solution: Interface Libraries Break Cycles
```
interface_core (no deps)
    ↓
interface_data_providers (depends on interface_core)
    ↓
Implementation libraries (depend on interfaces)
```

**Result:** One-way dependency flow, no circles.

---

## Quick Decision Tree

```
Can this struct/class be moved to interface library?
    │
    ├─ Does it depend on SFML?
    │   └─ YES → ❌ NO (SFML is framework, keep it)
    │
    ├─ Does it depend on FlatBuffers (*_generated.h)?
    │   └─ YES → ❌ NO (format-specific, use I*Provider pattern)
    │
    ├─ Does it depend on UUID?
    │   └─ YES → ⚠️ MAYBE (consider replacing with uint64_t)
    │
    ├─ Is it a complex implementation class?
    │   └─ YES → ❌ NO (keep in implementation library)
    │
    ├─ Only depends on std library?
    │   └─ YES → ✅ interface_core
    │
    └─ Only depends on std + interface_core?
        └─ YES → ✅ interface_data_providers
```

---

## Implementation Checklist

### Phase 1: Create interface_core

**Time:** 2-4 hours  
**Risk:** Very low  
**Benefit:** Breaks logger circular dependencies

**Steps:**
1. [ ] Create `src/interface_core/` directory
2. [ ] Create `src/interface_core/CMakeLists.txt`:
   ```cmake
   add_library(interface_core INTERFACE)
   target_include_directories(interface_core INTERFACE
     ${CMAKE_CURRENT_SOURCE_DIR}
   )
   ```
3. [ ] Copy 8 headers to `src/interface_core/`
4. [ ] Update `src/CMakeLists.txt` to add `add_subdirectory(interface_core)` FIRST
5. [ ] Update #include paths:
   - Option A: Update all includes to `#include "interface_core/FailInfo.h"`
   - Option B: Create forward headers in original locations
6. [ ] Update CMakeLists.txt in consuming libraries:
   ```cmake
   target_link_libraries(my_library PUBLIC
     interface_core  # Instead of logger, etc.
     # ...
   )
   ```
7. [ ] Build and test

---

### Phase 2: Create interface_data_providers

**Time:** 4-6 hours  
**Risk:** Low  
**Benefit:** Clean data loading abstraction

**Steps:**
1. [ ] Create `src/interface_data_providers/` directory
2. [ ] Create `src/interface_data_providers/CMakeLists.txt`:
   ```cmake
   add_library(interface_data_providers INTERFACE)
   target_include_directories(interface_data_providers INTERFACE
     ${CMAKE_CURRENT_SOURCE_DIR}
   )
   target_link_libraries(interface_data_providers INTERFACE
     interface_core
   )
   ```
3. [ ] Copy 5 interface headers to `src/interface_data_providers/`
4. [ ] Update `src/CMakeLists.txt` to add `add_subdirectory(interface_data_providers)`
5. [ ] Update #include paths
6. [ ] Update CMakeLists.txt in data_handlers and data_providers
7. [ ] Build and test

---

## File Mappings

### interface_core (Header-only)

| Original Location | New Location | Dependencies |
|------------------|--------------|--------------|
| `logger/FailInfo.h` | `interface_core/FailInfo.h` | std::string only |
| `engine/EngineConfig.h` | `interface_core/EngineConfig.h` | std only |
| `scenes/SceneConfig.h` | `interface_core/SceneConfig.h` | std only |
| `scenes/SceneState.h` | `interface_core/SceneState.h` | std only |
| `scenes/SceneManagerConfig.h` | `interface_core/SceneManagerConfig.h` | std only |
| `scenes/SceneManagerState.h` | `interface_core/SceneManagerState.h` | std only |

---

### interface_data_providers (Header-only)

| Original Location | New Location | Dependencies |
|------------------|--------------|--------------|
| `data_providers/IAssetDataProvider.h` | `interface_data_providers/IAssetDataProvider.h` | interface_core |
| `data_providers/IFragmentDataProvider.h` | `interface_data_providers/IFragmentDataProvider.h` | interface_core |
| `data_providers/ISceneManagerDataProvider.h` | `interface_data_providers/ISceneManagerDataProvider.h` | interface_core |
| `data_providers/IEngineDataProvider.h` | `interface_data_providers/IEngineDataProvider.h` | interface_core |
| `data_providers/ISubscriberViewer.h` | `interface_data_providers/ISubscriberViewer.h` | interface_core |

---

## Common Questions

### Q: Why not abstract SFML types?

**A:** SFML IS the rendering framework. Abstracting it means:
- Creating wrapper types for Vector2, Color, RenderWindow, etc.
- Double conversion: game code → wrapper → SFML → GPU
- Added complexity with no benefit
- Performance cost

**Recommendation:** Accept SFML as foundational. It's not circular, it's directional (everyone depends on SFML).

---

### Q: What about EventPacket/Subscriber?

**A:** Currently blocked by:
- SFML dependencies (keyboard/mouse types)
- FlatBuffers dependencies (EventType enum)
- UUID dependencies

**Options:**
1. Accept current architecture (it works!)
2. Refactor to remove external deps (2-3 days work)
   - Replace UUID with uint64_t
   - Replace FlatBuffers enums with native enums
   - Abstract SFML input types

**Recommendation:** Defer unless event system interface extraction is high priority.

---

### Q: What about Components?

**A:** Per requirements, components are excluded. Don't touch them.

---

### Q: Should I move SceneInfo?

**A:** No - it depends on UUID and FlatBuffers. Consider refactoring UUID → uint64_t if needed, but not critical.

---

## Benefits Summary

### Before (Circular Dependencies)
```
components → logger → config
    ↓           ↓
  entity ←── events
    ↓           ↓
  logic ←── engine
    ↑           ↓
    └───── CIRCULAR!
```

**Problems:**
- CMake link order matters
- Difficult to build independently
- Changes cascade through dependencies

### After (Layered Dependencies)
```
interface_core (std only)
    ↓
interface_data_providers
    ↓
Implementation libraries
    ↓
One-way flow!
```

**Benefits:**
- ✅ No circular dependencies
- ✅ Clear dependency direction
- ✅ Interface libraries build independently
- ✅ Easy to understand dependency flow
- ✅ Changes isolated to layer

---

## What NOT To Do

### ❌ Don't Abstract SFML
Reason: It's the framework, not an implementation detail

### ❌ Don't Extract Complex Classes
Reason: Logic, Scene, Engine are implementations, not interfaces

### ❌ Don't Move FlatBuffers Code
Reason: Already abstracted with I*Provider pattern

### ❌ Don't Touch Components
Reason: Per requirements

### ❌ Don't Create Massive Refactors
Reason: Interface extraction should be incremental and low-risk

---

## Success Criteria

### Phase 1 Complete When:
- [ ] `interface_core` library builds successfully
- [ ] 8 headers moved to `interface_core/`
- [ ] All consuming libraries updated to link `interface_core`
- [ ] All tests pass
- [ ] No circular dependency warnings from CMake

### Phase 2 Complete When:
- [ ] `interface_data_providers` library builds successfully
- [ ] 5 interfaces moved to `interface_data_providers/`
- [ ] data_handlers and data_providers updated
- [ ] All tests pass
- [ ] Clean separation between interface and implementation

---

## References

- **Full Analysis:** [INTERFACE_LIBRARY_ANALYSIS.md](INTERFACE_LIBRARY_ANALYSIS.md)
- **Dependency Diagrams:** [INTERFACE_LIBRARY_DEPENDENCY_DIAGRAMS.md](INTERFACE_LIBRARY_DEPENDENCY_DIAGRAMS.md)
- **Repository Copilot Instructions:** [GitHub Copilot Instructions](../../README.md)

---

## Version History

**Version 1.0** - December 7, 2025
- Initial analysis complete
- 52 structures analyzed
- 13 candidates identified for interface libraries
- 39 structures documented as non-extractable with reasons

