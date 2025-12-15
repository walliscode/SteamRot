# Scene.h Interface Dependency Solution

## Problem Statement

`ISceneConfigurator` interface (in `src/interfaces/`) includes `Scene.h` (in `src/scenes/`), creating a dependency from Layer 1 (interfaces) to Layer 2 (implementations). This violates the layered architecture principle where interfaces should have zero dependencies on implementation packages.

**Current Issue**:
```
src/interfaces/ISceneConfigurator.h
  ↓ includes
src/scenes/Scene.h  ← Layer 1 depends on Layer 2 (VIOLATION)
```

## Root Cause Analysis

`ISceneConfigurator` needs `Scene&` parameter in its virtual methods:
```cpp
virtual std::expected<std::monostate, FailInfo>
ConfigureSceneInfo(Scene &scene, const SceneData *scene_data) = 0;
```

The interface needs to reference `Scene` class, but `Scene.h` is in the implementation layer.

## Solution: Forward Declaration Pattern

**✅ RECOMMENDED APPROACH**: Use forward declaration instead of including `Scene.h`

### Why This Works

1. **Forward declarations are sufficient for pointers and references**
   - `ISceneConfigurator` only uses `Scene&` (reference)
   - It doesn't need the full definition of `Scene`
   - Only needs to know that `Scene` exists as a type

2. **Keeps interfaces truly abstract**
   - Layer 1 (`interfaces`) has zero dependencies on Layer 2
   - Implementations in Layer 2 include full definitions

3. **Maintains existing API**
   - No changes to method signatures
   - No changes to calling code
   - Binary compatible

### Implementation Steps

#### Step 1: Update ISceneConfigurator.h

**Current (src/interfaces/ISceneConfigurator.h)**:
```cpp
#pragma once

#include "FailInfo.h"
#include "Scene.h"          // ← REMOVE THIS
#include "SceneData.h"
#include <expected>
#include <variant>

namespace steamrot {

class ISceneConfigurator {
  // ...
};

} // namespace steamrot
```

**Updated (src/interfaces/ISceneConfigurator.h)**:
```cpp
#pragma once

#include "FailInfo.h"
#include "SceneData.h"
#include <expected>
#include <variant>

namespace steamrot {

// Forward declaration
class Scene;                // ← ADD THIS

class ISceneConfigurator {
  // ... rest unchanged
};

} // namespace steamrot
```

#### Step 2: Move Implementation Methods to .cpp

The non-virtual methods `ConfigureScene()` and `ConfigureLogicMap()` are **defined inline** in the header. These need the full `Scene` definition.

**Problem**: These methods call `scene.GetSceneContext()`, `scene.GetSceneInfo()`, and access `scene.m_scene_resources`.

**Solution**: Keep declarations in header, move definitions to .cpp file.

**In ISceneConfigurator.h** (keep only declarations):
```cpp
class ISceneConfigurator {
public:
  ISceneConfigurator() {};
  virtual ~ISceneConfigurator() {};

  // Non-virtual wrapper - DECLARATION ONLY
  std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneData *scene_data);

  // Virtual methods - pure virtual, no definition needed
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(Scene &scene, const SceneData *scene_data) = 0;

  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene &scene, const SceneData *scene_data) = 0;

  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneConfig(Scene &scene, const SceneData *scene_data) = 0;

  // Non-virtual method - DECLARATION ONLY
  std::expected<std::monostate, FailInfo>
  ConfigureLogicMap(Scene &scene);
};
```

**In src/scenes/ISceneConfigurator.cpp** (move definitions here):
```cpp
#include "ISceneConfigurator.h"
#include "Scene.h"              // ← NOW include Scene.h in .cpp
#include "LogicFactory.h"
#include "SceneContext.h"

namespace steamrot {

std::expected<std::monostate, FailInfo>
ISceneConfigurator::ConfigureScene(Scene &scene, const SceneData *scene_data) {
  
  // Configure SceneInfo
  auto info_result = ConfigureSceneInfo(scene, scene_data);
  if (!info_result.has_value())
    return std::unexpected(info_result.error());

  // Configure SceneResources
  auto resources_result = ConfigureSceneResources(scene, scene_data);
  if (!resources_result.has_value())
    return std::unexpected(resources_result.error());

  // Configure SceneConfig
  auto config_result = ConfigureSceneConfig(scene, scene_data);
  if (!config_result.has_value())
    return std::unexpected(config_result.error());

  // Configure LogicMap
  auto logic_result = ConfigureLogicMap(scene);
  if (!logic_result.has_value())
    return std::unexpected(logic_result.error());

  return std::monostate();
}

std::expected<std::monostate, FailInfo>
ISceneConfigurator::ConfigureLogicMap(Scene &scene) {

  // Get SceneContext from the scene
  SceneContext scene_context = scene.GetSceneContext();

  // Get scene type from scene info
  SceneType scene_type = scene.GetSceneInfo().type;

  // Create LogicFactory
  LogicFactory logic_factory(scene_type, scene_context);

  // Create logic map
  auto logic_map_result = logic_factory.CreateLogicMap();
  if (!logic_map_result.has_value())
    return std::unexpected(logic_map_result.error());

  // Store logic map in scene resources
  scene.m_scene_resources.logic_map = std::move(logic_map_result.value());

  return std::monostate();
}

} // namespace steamrot
```

#### Step 3: Verify CMakeLists.txt

Ensure `src/scenes/CMakeLists.txt` still compiles `ISceneConfigurator.cpp`:

```cmake
add_library(scenes STATIC
  ISceneConfigurator.cpp    # ← Should be here
  Scene.cpp
  SceneFactory.cpp
  # ... other files
)

target_link_libraries(scenes
  PUBLIC
    interfaces              # ← Scenes depends on interfaces
    types
  PRIVATE
    # ... other dependencies
)
```

## Verification Steps

1. **Check interfaces library has no Scene.h dependency**:
   ```bash
   grep -r "Scene\.h" src/interfaces/
   # Should return nothing (or only forward declarations)
   ```

2. **Rebuild project**:
   ```bash
   cmake --build --preset Debug
   ```

3. **Verify no circular dependencies**:
   - `interfaces` → only `types` (Layer 1 → Layer 1 ✅)
   - `scenes` → `interfaces`, `types` (Layer 2 → Layer 1 ✅)
   - No circular deps ✅

## Alternative Considered (Not Recommended)

**❌ Option: Pass only structs to configurators**

```cpp
// Instead of Scene&, pass individual structs
virtual std::expected<std::monostate, FailInfo>
ConfigureSceneInfo(SceneInfo &info, const SceneData *data) = 0;
```

**Why not recommended**:
- Requires changing all configurator implementations
- Breaks encapsulation (exposes internal Scene structure)
- More invasive change
- Doesn't solve the problem for `ConfigureLogicMap` which needs full Scene access

## Benefits of Forward Declaration Solution

✅ **Minimal changes**: Only modify ISceneConfigurator.h and .cpp  
✅ **Maintains API**: No changes to calling code  
✅ **Breaks dependency**: interfaces layer truly independent  
✅ **Best practice**: Standard C++ pattern for interface libraries  
✅ **Fast to implement**: 15-20 minutes  

## Summary

**Problem**: `ISceneConfigurator.h` includes `Scene.h`, creating Layer 1 → Layer 2 dependency

**Solution**: 
1. Replace `#include "Scene.h"` with `class Scene;` forward declaration in header
2. Move inline method implementations from .h to .cpp file
3. Include `Scene.h` in .cpp file instead

**Result**: Clean layer separation, no circular dependencies, interfaces library has zero implementation dependencies

**Time Estimate**: 15-20 minutes

## Next Steps

After implementing this fix:
1. Move `SceneInfoProvider.h` to `src/interfaces/` (see CURRENT_STATE_CMAKE_ANALYSIS.md)
2. Continue Phase 2 of migration (see MIGRATION_PLAN.md)
3. Complete save infrastructure (Phase 3)
