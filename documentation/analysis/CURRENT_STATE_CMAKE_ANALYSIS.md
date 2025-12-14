# Current State: CMake Dependency Analysis

**Date**: December 14, 2024  
**Status**: Build failing due to circular dependencies

## Executive Summary

✅ **Phase 1 Complete**: `interfaces` library extracted successfully  
⚠️ **Circular Dependencies Remain**: Three major cycles identified  
❌ **Build Status**: Failing (X11 dependency issues in CI, circular deps in local builds)

## Identified Circular Dependencies

### Cycle 1: display ↔ scenes ↔ engine

```
display → scenes → display
display → engine → display
scenes → display
engine → display
```

**Dependency Chain**:
- `display` links `scenes` (line 17 of display/CMakeLists.txt)
- `display` links `engine` (line 19 of display/CMakeLists.txt)
- `scenes` links `display` (line 26 of scenes/CMakeLists.txt)
- `engine` links `display` (line 16 of engine/CMakeLists.txt)

**Root Cause**: 
- `DisplayManager` needs `SceneManager` and `Engine` 
- `SceneManager` needs `DisplayManager`
- `Engine` needs `DisplayManager`

### Cycle 2: scenes → data_providers (RESOLVED VIA INTERFACES)

```
scenes → data_providers
data_providers → (no direct scenes dependency)
```

**Status**: ✅ **RESOLVED** via `interfaces` library
- Previously: `data_providers` depended on `scenes` (for configurators)
- Now: `data_providers` depends on `interfaces` only
- `scenes` depends on both `data_providers` and `interfaces`

This was the **PRIMARY** circular dependency identified in the analysis, and it's been successfully broken.

### Cycle 3: logic → entity → data_providers → (indirect scenes via logic usage)

```
logic → entity
entity → data_providers
scenes → logic
```

**Status**: Minor concern, not currently causing build failures.

## Current CMake Dependency Graph

### Layer 1 (Data/Types/Interfaces) - ✅ CLEAN

```
interfaces → types
types → (no dependencies on SteamRot packages)
events → types
components → types
```

### Layer 2 (Logic/Providers) - ⚠️ MOSTLY CLEAN

```
data_providers → interfaces, types, events
user_interface → (checking...)
logic → components, entity, user_interface
entity → components, types, events, data_providers, user_interface
assets → types, interfaces, user_interface, data_providers
```

### Layer 3 (Orchestration) - ❌ CIRCULAR DEPENDENCIES

```
scenes → display, entity, logger, context, logic, types, data_providers, interfaces
display → scenes, engine, logic
engine → display, entity, events, logger, context, logic, assets, data_providers, types
context → types, assets
```

## Critical Issues

### Issue 1: display ↔ scenes Circular Dependency

**Files Involved**:
- `src/display/CMakeLists.txt` (links scenes)
- `src/scenes/CMakeLists.txt` (links display)
- `src/display/DisplayManager.h/cpp` (uses SceneManager)
- `src/scenes/SceneManager.h/cpp` (uses DisplayManager)

**Solution Options**:

**Option A: Extract IDisplayManager interface** (RECOMMENDED)
1. Create `src/interfaces/IDisplayManager.h` with abstract interface
2. Move `DisplayManager.h/cpp` implementation details to `src/display/`
3. `scenes` depends on `interfaces` (for IDisplayManager)
4. `display` implements `IDisplayManager`
5. Break: `scenes` no longer links `display` library

**Option B: Dependency Injection**
1. Pass `DisplayManager*` to `SceneManager` via constructor
2. Break compile-time dependency
3. Runtime dependency remains

**Option C: Event-based communication**
1. Use event system for Scene ↔ Display communication
2. Fully decoupled
3. More complex

### Issue 2: display ↔ engine Circular Dependency

**Files Involved**:
- `src/display/CMakeLists.txt` (links engine)
- `src/engine/CMakeLists.txt` (links display)
- `src/display/DisplayManager.h/cpp` (uses Engine)
- `src/engine/Engine.h/cpp` or `GameEngine.h/cpp` (uses DisplayManager)

**Solution**: Same as Issue 1 - extract interfaces

### Issue 3: scenes → display → engine creates triangle

All three packages are tightly coupled, creating a dependency triangle.

**Solution**: Extract all three interfaces:
- `ISceneManager` (already exists in interfaces via ISceneConfigurator pattern)
- `IDisplayManager` (needs to be created)
- `IEngine` (needs to be created)

## Recommended Action Plan

### Immediate Actions (Fix Build)

**Priority 1: Break display ↔ scenes cycle**

1. **Create `src/interfaces/IDisplayManager.h`**:
   ```cpp
   #pragma once
   #include <SFML/Graphics.hpp>
   
   namespace steamrot {
   
   class IDisplayManager {
   public:
     virtual ~IDisplayManager() = default;
     
     virtual void ClearCanvas() = 0;
     virtual void RenderCanvas() = 0;
     virtual sf::RenderTexture& GetRenderTexture() = 0;
     virtual sf::RenderWindow& GetWindow() = 0;
     // Add other virtual methods used by SceneManager
   };
   
   } // namespace steamrot
   ```

2. **Update `src/display/DisplayManager.h`**:
   ```cpp
   #include "IDisplayManager.h"  // From interfaces
   
   class DisplayManager : public IDisplayManager {
     // Existing implementation
   };
   ```

3. **Update `src/display/CMakeLists.txt`**:
   ```cmake
   target_link_libraries(display PUBLIC
       SFML::Graphics
       interfaces  # Add this
       # Remove: scenes, engine (for now)
       magic_enum
       stduuid
       logic
   )
   ```

4. **Update `src/scenes/CMakeLists.txt`**:
   ```cmake
   target_link_libraries(scenes PUBLIC
       SFML::Graphics
       # Remove: display
       entity
       logger
       stduuid
       context
       logic
       types
       data_providers
       interfaces
   )
   ```

5. **Update `src/scenes/SceneManager.h`**:
   ```cpp
   #include "IDisplayManager.h"  // Instead of DisplayManager.h
   
   class SceneManager {
   private:
     IDisplayManager* m_display_manager;  // Pointer to interface
   };
   ```

6. **Verify build**:
   ```bash
   cmake --preset Debug
   cmake --build --preset Debug
   ```

**Priority 2: Break display ↔ engine cycle** (Same pattern)

1. Create `src/interfaces/IEngine.h`
2. Update `src/engine/Engine.h` and `GameEngine.h` to implement IEngine
3. Update CMakeLists.txt files
4. Use `IEngine*` instead of `Engine*` in DisplayManager

### Long-term Refactoring (Phase 4-5)

From the original migration plan, continue with:

**Phase 4: Break Circular Dependencies** (Weeks 5-6)
- Move `SceneType` enum to `src/types/`
- Extract remaining interfaces (IEngine, IAssetManager, etc.)
- Enforce layer rules in CMake

**Phase 5: Reorganize Packages** (Weeks 7-8)
- Rename `data_providers` → `providers`
- Rename `scenes` → `scene_management`
- Final cleanup

## Verification Checklist

After implementing fixes:

- [ ] `cmake --preset Debug` completes without errors
- [ ] `cmake --build --preset Debug` completes without errors
- [ ] `ctest --preset Debug` passes all tests
- [ ] No circular dependencies in dependency graph
- [ ] All packages follow layer rules:
  - Layer 1 packages have NO dependencies on SteamRot packages
  - Layer 2 packages depend ONLY on Layer 1
  - Layer 3 packages depend on Layer 1 + Layer 2

## Dependency Graph Visualization

### Current State (With Cycles)

```
┌─────────┐
│ display │◄─────┐
└────┬────┘      │
     │           │
     ▼           │
┌─────────┐      │
│ scenes  │──────┘
└────┬────┘
     │
     ▼
┌─────────┐      ┌────────────────┐
│ engine  │─────►│ data_providers │
└────┬────┘      └────────────────┘
     │                    ▲
     │                    │
     └────────────────────┘
```

### Target State (No Cycles)

```
Layer 3 (Orchestration):
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   display   │    │   scenes    │    │   engine    │
└──────┬──────┘    └──────┬──────┘    └──────┬──────┘
       │                  │                   │
       └──────────────────┼───────────────────┘
                          │
                          ▼
Layer 2 (Logic/Providers):
┌────────────────┐    ┌─────────┐    ┌────────┐
│ data_providers │    │  logic  │    │ entity │
└────────┬───────┘    └────┬────┘    └───┬────┘
         │                 │              │
         └─────────────────┼──────────────┘
                           │
                           ▼
Layer 1 (Data/Types/Interfaces):
┌────────────┐    ┌─────────┐    ┌────────┐
│ interfaces │    │  types  │    │ events │
└────────────┘    └─────────┘    └────────┘
```

## Files Requiring Changes

### Immediate (Priority 1)

1. **CREATE** `src/interfaces/IDisplayManager.h`
2. **EDIT** `src/display/DisplayManager.h` (implement IDisplayManager)
3. **EDIT** `src/display/CMakeLists.txt` (remove scenes, add interfaces)
4. **EDIT** `src/scenes/CMakeLists.txt` (remove display)
5. **EDIT** `src/scenes/SceneManager.h` (use IDisplayManager*)
6. **EDIT** `src/scenes/SceneManager.cpp` (include IDisplayManager.h)

### Short-term (Priority 2)

7. **CREATE** `src/interfaces/IEngine.h`
8. **EDIT** `src/engine/Engine.h` (implement IEngine)
9. **EDIT** `src/engine/GameEngine.h` (implement IEngine)
10. **EDIT** `src/engine/CMakeLists.txt` (remove display, add interfaces)
11. **EDIT** `src/display/DisplayManager.h` (use IEngine*)

## Testing Strategy

1. **Unit tests**: Ensure existing tests still pass
2. **Integration tests**: Verify scene transitions work
3. **Build tests**: Verify clean builds on all platforms
4. **Dependency tests**: Use CMake dependency analyzer to verify no cycles

## Conclusion

**Current Status**:
- ✅ Phase 1 (Extract Interfaces) COMPLETE
- ⚠️ Phase 2 (Refactor Configurators) PARTIAL (ISceneConfigurator refactored)
- ❌ Circular dependencies blocking further progress

**Next Steps**:
1. Implement Priority 1 fixes (IDisplayManager interface)
2. Implement Priority 2 fixes (IEngine interface)
3. Verify build succeeds
4. Continue with Phase 2-3 of migration plan

**Estimated Time**: 4-6 hours for Priority 1+2 fixes

The good news: The hardest architectural decision (provider/configurator separation) is done. The remaining work is mechanical extraction of interfaces.
