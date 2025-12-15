# CMake Dependency State Analysis - December 15, 2024

## Executive Summary

**Status**: ✅ Build is working, but **4 circular dependencies remain**

**Good News**:
- ✅ Phase 1 complete: `interfaces` library successfully extracted
- ✅ Build compiles and links successfully
- ✅ `scenes ↔ data_providers` circular dependency resolved via interfaces

**Issues Remaining**:
- ⚠️ **4 circular dependencies** still exist (scenes ↔ display ↔ engine)
- ⚠️ **Layer violation**: ISceneConfigurator.h includes Scene.h (Layer 1 → Layer 2)
- ⚠️ SceneInfoProvider.h still in `src/scenes/` instead of `src/interfaces/`

**Impact**: While the build works, these circular dependencies:
- Increase build times (everything rebuilds when display/scenes/engine changes)
- Make incremental compilation inefficient
- Violate clean architecture principles
- Will cause issues as codebase grows

---

## Current Dependency Graph

### Complete Package Dependencies

```
Layer 1 (Foundation):
  ✅ logger → (none)
  ✅ types → (none)
  ✅ interfaces → types

Layer 2 (Should depend only on Layer 1):
  ✅ components → logger, types
  ⚠️  events → logger, types (OK)
  ⚠️  user_interface → types, events (OK)
  ⚠️  data_providers → events, interfaces, types (OK)
  
Layer 2+ (Depend on other Layer 2):
  ⚠️  logic → components, entity, user_interface
  ⚠️  entity → components, types, events, data_providers, user_interface
  ⚠️  assets → types, interfaces, user_interface, data_providers
  ⚠️  context → types, assets

Layer 3 (Orchestration - CIRCULAR!):
  🔴 scenes → display, entity, logger, context, logic, types, data_providers, interfaces
  🔴 display → scenes, engine, logic
  🔴 engine → display, entity, events, logger, context, logic, assets, data_providers, types
```

### Circular Dependencies Detected

**Found 4 circular dependencies:**

1. **scenes → display → scenes** (PRIMARY)
   - scenes links display
   - display links scenes
   - Forms 2-way cycle

2. **display → engine → display**
   - display links engine
   - engine links display
   - Forms 2-way cycle

3. **display ↔ scenes** (duplicate of #1)
   
4. **engine ↔ display** (duplicate of #2)

**Visualization**:
```
    ┌─────────┐
    │  scenes │
    └────┬────┘
         │ depends on
         ↓
    ┌────────┐
    │ display│←──┐
    └────┬───┘   │
         │       │ depends on
         │       │
         ↓       │
    ┌────────┐  │
    │ engine │──┘
    └────────┘
```

This creates a 3-way circular dependency: **scenes → display → engine → display**

---

## Root Causes

### 1. Display → Scenes Dependency

**File**: `src/display/CMakeLists.txt` line 17
```cmake
target_link_libraries(display PUBLIC
    scenes    # ← This creates the circular dependency
    engine
    logic
)
```

**Why it exists**: Display needs Scene types/interfaces

**Likely culprits**:
- DisplayManager.cpp includes Scene.h or SceneManager.h
- Display needs scene types to render

### 2. Scenes → Display Dependency

**File**: `src/scenes/CMakeLists.txt` line 26
```cmake
target_link_libraries(scenes PUBLIC
    display   # ← This creates the circular dependency
    entity
    logger
    ...
)
```

**Why it exists**: Scenes need display for rendering

**Likely culprits**:
- Scene.cpp uses DisplayManager or RenderTexture from display
- Scenes configure display settings

### 3. Display ↔ Engine Cycle

**Files**: 
- `src/display/CMakeLists.txt` line 19: display links engine
- `src/engine/CMakeLists.txt` line 16: engine links display

**Why it exists**: Bidirectional communication between engine and display

### 4. ISceneConfigurator.h → Scene.h (Layer Violation)

**File**: `src/interfaces/ISceneConfigurator.h` line 12
```cpp
#include "Scene.h"  // ← Layer 1 (interfaces) includes Layer 2 (scenes)
```

**Problem**:
- `interfaces` is Layer 1 (foundation)
- `Scene.h` is in `src/scenes/` (Layer 2/3)
- Creates upward dependency

**Why it exists**:
- `ConfigureScene()` method (lines 43-66) is inline in header
- `ConfigureLogicMap()` method (lines 97-117) is inline in header
- Both methods call Scene member functions and access Scene members

---

## Recommended Fixes (Priority Order)

### Priority 1: Fix ISceneConfigurator Layer Violation (15-20 minutes)

**Issue**: interfaces → scenes dependency via Scene.h include

**Solution**: Use forward declaration pattern

**Steps**:

1. **Edit `src/interfaces/ISceneConfigurator.h`**:
   ```cpp
   // Remove line 12:
   // #include "Scene.h"
   
   // Add after line 11:
   class Scene;  // Forward declaration
   ```

2. **Move inline implementations to .cpp**:
   
   Remove lines 43-66 (ConfigureScene body) from .h, replace with:
   ```cpp
   std::expected<std::monostate, FailInfo>
   ConfigureScene(Scene &scene, const SceneData *scene_data);
   ```

   Remove lines 97-117 (ConfigureLogicMap body) from .h, replace with:
   ```cpp
   std::expected<std::monostate, FailInfo>
   ConfigureLogicMap(Scene &scene);
   ```

3. **Edit `src/scenes/ISceneConfigurator.cpp`**:
   
   Add the method bodies that were removed from .h:
   ```cpp
   #include "ISceneConfigurator.h"
   #include "Scene.h"        // Now we can include Scene.h here
   #include "LogicFactory.h"
   
   namespace steamrot {
   
   std::expected<std::monostate, FailInfo>
   ISceneConfigurator::ConfigureScene(Scene &scene, const SceneData *scene_data) {
     // [Move body from .h file here]
   }
   
   std::expected<std::monostate, FailInfo>
   ISceneConfigurator::ConfigureLogicMap(Scene &scene) {
     // [Move body from .h file here]
   }
   
   } // namespace steamrot
   ```

4. **Rebuild and verify**

**Result**: Breaks interfaces → scenes dependency

**See**: `SCENE_INTERFACE_DEPENDENCY_SOLUTION.md` for complete details

---

### Priority 2: Move SceneInfoProvider to interfaces (30 minutes)

**Issue**: SceneInfoProvider.h is in scenes, causing display → scenes dependency

**File to move**: `src/scenes/SceneInfoProvider.h` → `src/interfaces/SceneInfoProvider.h`

**Steps**:

1. **Move the file**:
   ```bash
   mv src/scenes/SceneInfoProvider.h src/interfaces/SceneInfoProvider.h
   ```

2. **Update includes in affected files**:
   ```bash
   # Find all files that include SceneInfoProvider.h
   grep -r "SceneInfoProvider.h" src/ --include="*.cpp" --include="*.h"
   
   # Update each to use from interfaces
   ```

3. **Update `src/display/CMakeLists.txt`**:
   
   Remove line 17 (scenes dependency):
   ```cmake
   target_link_libraries(display PUBLIC
       SFML::Graphics
       # scenes    # ← Remove this
       magic_enum
       engine
       stduuid
       logic
       interfaces  # ← Add this if not present
   )
   ```

4. **Update `src/scenes/CMakeLists.txt`**:
   
   Remove display from line 26:
   ```cmake
   target_link_libraries(scenes PUBLIC
   SFML::Graphics
   # display   # ← Remove this
   entity
   logger
   ...
   ```

5. **Rebuild and verify**

**Result**: Breaks scenes ↔ display circular dependency

---

### Priority 3: Break Display ↔ Engine Cycle (1-2 hours)

**Issue**: Bidirectional dependency between display and engine

**Analysis needed**: Investigate why these depend on each other

**Likely solutions**:
1. **Extract display interface**: Move display abstractions to interfaces
2. **Use dependency injection**: Engine receives display interface, doesn't link library
3. **Event-based communication**: Use event system instead of direct calls

**Investigation steps**:
```bash
# Find display includes in engine
grep -n "DisplayManager.h\|RenderTexture.h" src/engine/*.cpp src/engine/*.h

# Find engine includes in display
grep -n "Engine.h\|GameEngine.h" src/display/*.cpp src/display/*.h
```

**Defer**: This is more complex, do after Priority 1 & 2

---

## Progress Tracking

### Phase 1: Extract Interfaces ✅ COMPLETE

- ✅ Created `src/interfaces/` directory
- ✅ Extracted ISceneDataProvider
- ✅ Extracted ISceneConfigurator
- ✅ Extracted IAssetDataProvider
- ✅ Extracted IEngineDataProvider
- ✅ Extracted ISceneManagerDataProvider
- ✅ Extracted ISubscriberViewer
- ✅ Created interfaces CMake library
- ⚠️ **Issue**: ISceneConfigurator still includes Scene.h (needs fix)
- ⚠️ **Issue**: SceneInfoProvider not moved yet

### Phase 2: Refactor Configurators 🚧 PARTIAL

- ✅ ISceneConfigurator takes SceneData* parameter
- ⚠️ **Issue**: Inline methods in header create Layer 1 → Layer 2 dependency
- ⚠️ **TODO**: Move inline implementations to .cpp

### Circular Dependencies Status

| Dependency Cycle | Status | Priority | Estimated Time |
|------------------|--------|----------|----------------|
| scenes ↔ data_providers | ✅ RESOLVED | N/A | N/A |
| interfaces → scenes (via Scene.h) | ⚠️ EXISTS | HIGH | 15-20 min |
| scenes ↔ display | ⚠️ EXISTS | HIGH | 30 min |
| display ↔ engine | ⚠️ EXISTS | MEDIUM | 1-2 hours |

---

## Build Status

✅ **Build compiles successfully**
✅ **Links successfully**
⚠️ **Circular dependencies present** (4 cycles)
⚠️ **Layer violations present** (1 violation)

**Build Impact**:
- Changes to display trigger rebuild of scenes, engine
- Changes to scenes trigger rebuild of display
- Changes to engine trigger rebuild of display
- Slow incremental builds

**Recommended immediate action**: Fix Priority 1 and Priority 2 (total ~50 minutes) to eliminate most critical issues.

---

## Next Steps

### Immediate (Today - 50 minutes)

1. ✅ **Fix ISceneConfigurator layer violation** (15-20 min)
   - Move inline methods to .cpp
   - Use forward declaration
   - See SCENE_INTERFACE_DEPENDENCY_SOLUTION.md

2. ✅ **Move SceneInfoProvider to interfaces** (30 min)
   - Move file
   - Update includes
   - Update CMakeLists.txt

### Short-term (This Week - 1-2 hours)

3. 🔍 **Investigate display ↔ engine cycle**
   - Analyze why bidirectional dependency exists
   - Design solution (extract interface, use events, DI)

4. 🔧 **Implement display/engine fix**

### Medium-term (Follow MIGRATION_PLAN.md)

5. Continue Phase 2: Complete configurator refactoring
6. Phase 3: Implement save infrastructure
7. Phase 4: Enforce layer rules with CMake checks
8. Phase 5: Final package reorganization

---

## Validation Commands

### Check for circular dependencies
```bash
cd /home/runner/work/SteamRot/SteamRot
python3 <<'EOF'
import re

deps = {}
packages = ['interfaces', 'types', 'components', 'events', 'logger',
            'assets', 'context', 'entity', 'user_interface', 'logic',
            'data_providers', 'scenes', 'display', 'engine']

for pkg in packages:
    with open(f'src/{pkg}/CMakeLists.txt', 'r') as f:
        content = f.read()
        match = re.search(rf'target_link_libraries\s*\(\s*{pkg}\s+(?:PUBLIC|PRIVATE|INTERFACE)?\s*([^)]+)\)', content, re.DOTALL)
        if match:
            links = match.group(1).strip().split()
            deps[pkg] = [l for l in links if l in packages]
        else:
            deps[pkg] = []

# Find cycles
def find_path(start, end, visited, path):
    visited.add(start)
    path.append(start)
    if start == end and len(path) > 1:
        return [path[:]]
    paths = []
    for dep in deps.get(start, []):
        if dep not in visited or dep == end:
            paths.extend(find_path(dep, end, visited, path))
    path.pop()
    visited.discard(start)
    return paths

cycles = []
for pkg in packages:
    paths = find_path(pkg, pkg, set(), [])
    for path in paths:
        cycle_key = tuple(sorted(path))
        if cycle_key not in [tuple(sorted(c)) for c in cycles]:
            cycles.append(path)

if cycles:
    print(f"⚠️  Found {len(cycles)} circular dependencies:")
    for cycle in cycles:
        print(f"  {' → '.join(cycle)}")
else:
    print("✅ No circular dependencies!")
EOF
```

### Verify layer violations
```bash
# Check if interfaces includes any Layer 2+ files
cd /home/runner/work/SteamRot/SteamRot
echo "Checking interfaces/ for Layer 2+ includes:"
grep -rn "#include.*Scene.h\|#include.*Display\|#include.*Engine" src/interfaces/ || echo "✅ No layer violations"
```

---

## Summary

**Current State**: Build works but has architectural debt

**Critical Issues**: 2 (ISceneConfigurator layer violation, SceneInfoProvider location)

**Time to Fix Critical Issues**: ~50 minutes

**Benefit**: Eliminates 3 of 4 circular dependencies, proper layer separation

**Next Steps**: Follow Priority 1 & 2 fixes above, then investigate display ↔ engine

**Documentation References**:
- `SCENE_INTERFACE_DEPENDENCY_SOLUTION.md` - Forward declaration pattern
- `CURRENT_STATE_CMAKE_ANALYSIS.md` - Previous analysis (SceneInfoProvider fix)
- `MIGRATION_PLAN.md` - Complete migration roadmap
