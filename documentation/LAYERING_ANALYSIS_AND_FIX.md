# Layering Analysis and Resolution Plan

## Current Problem

The introduction of `IEntityImporter`/`IEntityExporter` and `SceneLoadData` has created **circular dependencies** in the library layering architecture.

## Current Layering Issues

### Issue 1: SceneLoadData Location

**Current State**:
```
SceneLoadData (in interfaces/)
  ├─ includes SceneData (from types)
  └─ contains unique_ptr<IEntityImporter> (forward declared, but from entity)
```

**Problem**:
- `interfaces` library links to `types` only
- `ISceneDataProvider` (in interfaces) uses `SceneLoadData`
- But `SceneLoadData` needs `IEntityImporter` which is in `entity`
- `entity` depends on `interfaces` → **CIRCULAR DEPENDENCY**

```
interfaces → entity (needs IEntityImporter for SceneLoadData)
entity → interfaces (needs ISceneDataProvider)
```

### Issue 2: Entity Library Over-Dependencies

**Current State**:
```
entity depends on:
  - components
  - types
  - interfaces
  - events
  - data_providers  ← PROBLEM!
  - user_interface
  - configuration
```

**Problem**:
- `entity` should be a low-level library (entity management logic)
- `data_providers` is high-level (loads data from files)
- `entity` depending on `data_providers` is backwards
- Creates: `data_providers` → `interfaces` → (needs) → `entity` → `data_providers`

### Issue 3: IEntityImporter/IEntityExporter Location

**Current State**:
- `IEntityImporter.h` in `src/entity/`
- `IEntityExporter.h` in `src/entity/`

**Problem**:
- These are **interfaces**, not implementations
- Interfaces should be in `src/interfaces/` for proper layering
- Having them in `entity` forces higher layers to depend on `entity` just for the interface

## Proper Layering Architecture

### Ideal Layer Hierarchy (Bottom to Top)

```
Layer 0: types
  - Pure data structures
  - No dependencies on other project libs
  
Layer 1: interfaces
  - Interface definitions
  - Depends on: types
  
Layer 2: events, components
  - Core abstractions
  - Depends on: types, interfaces
  
Layer 3: entity
  - Entity management, memory pools
  - Depends on: types, interfaces, events, components
  
Layer 4: configuration, data_providers
  - Data loading and configuration
  - Depends on: types, interfaces, entity, events
  
Layer 5: logic, scenes, assets
  - High-level game systems
  - Depends on: lower layers
```

## Resolution Plan

### Step 1: Move Interfaces to `interfaces` Library

**Action**: Move interface files from `entity` to `interfaces`

**Files to move**:
- `src/entity/IEntityImporter.h` → `src/interfaces/IEntityImporter.h`
- `src/entity/IEntityExporter.h` → `src/interfaces/IEntityExporter.h`
- `src/entity/IEntityConfigurator.h` → `src/interfaces/IEntityConfigurator.h`

**Rationale**:
- Interfaces define contracts, not implementations
- Allows high-level code to depend on interfaces without depending on `entity`
- Follows Dependency Inversion Principle

**Update**: `src/interfaces/CMakeLists.txt`
```cmake
add_library(interfaces INTERFACE)

target_include_directories(interfaces
  INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(interfaces
  INTERFACE
  types
  SFML::Graphics
  stduuid
)
```

**Files stay header-only** (no .cpp implementations in interfaces library).

### Step 2: Keep Implementations in `entity`

**Files remaining in `entity`**:
- `FlatbuffersEntityImporter.h/cpp` (implementation)
- `FlatbuffersEntityConfigurator.h/cpp` (implementation)
- `EntityManager.h/cpp`
- `ArchetypeManager.h/cpp`
- etc.

**Update**: `src/entity/CMakeLists.txt`
```cmake
add_library(entity
    EntityManager.cpp
    FlatbuffersEntityConfigurator.cpp
    FlatbuffersEntityImporter.cpp
    ArchetypeManager.cpp
    entity_memory.cpp
)

target_include_directories(entity
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(entity
  PUBLIC
  SFML::Graphics
  components
  types
  interfaces  # OK - entity can depend on interfaces
  events
  # REMOVE: data_providers  ← Remove this!
  user_interface
  configuration
)
```

### Step 3: Fix SceneLoadData

**Current location**: `src/interfaces/SceneLoadData.h` ✓ (Correct!)

**Update**: `src/interfaces/SceneLoadData.h`
```cpp
#pragma once

#include "SceneData.h"
#include "IEntityImporter.h"  // Now in interfaces - OK!
#include <memory>

namespace steamrot {

struct SceneLoadData {
  SceneData scene_data;
  std::unique_ptr<IEntityImporter> entity_importer;
};

} // namespace steamrot
```

**Rationale**:
- `interfaces` can include other interfaces
- No circular dependency: `interfaces` → `types` (clean)

### Step 4: Fix data_providers Dependencies

**Update**: `src/data_providers/FlatbuffersSceneDataProvider.h`
```cpp
#pragma once

#include "ISceneDataProvider.h"
#include "FlatbuffersEntityImporter.h"  // Implementation from entity
// ...
```

**Update**: `src/data_providers/CMakeLists.txt`
```cmake
target_link_libraries(data_providers
  PUBLIC
  SFML::Graphics
  events
  interfaces
  types
  configuration
  entity  # Add this - data_providers can depend on entity
)
```

**Rationale**:
- `data_providers` is high-level, can depend on `entity`
- `data_providers` creates implementations (FlatbuffersEntityImporter)
- Flow: `data_providers` → `entity` → `interfaces` → `types` (no cycles!)

### Step 5: Update All Includes

**Files to update**:

1. **FlatbuffersEntityImporter.h** (in entity):
```cpp
#include "IEntityImporter.h"  // Now from ../interfaces/
```

2. **FlatbuffersEntityConfigurator.h** (in entity):
```cpp
#include "IEntityConfigurator.h"  // Now from ../interfaces/
```

3. **ISceneDataProvider.h** (in interfaces):
```cpp
#include "SceneLoadData.h"  // Same directory - OK
#include "IEntityImporter.h"  // Same directory - OK (after move)
```

4. **Any data_providers files**:
```cpp
#include "FlatbuffersEntityImporter.h"  // From entity/ - OK
#include "IEntityImporter.h"  // From interfaces/ - OK
```

### Step 6: Verify No Circular Dependencies

**Check dependency graph** after changes:

```
types (Layer 0)
  ↑
interfaces (Layer 1)
  - IEntityImporter
  - IEntityExporter
  - IEntityConfigurator
  - SceneLoadData
  - ISceneDataProvider
  ↑
entity (Layer 3)
  - FlatbuffersEntityImporter
  - FlatbuffersEntityConfigurator
  ↑
data_providers (Layer 4)
  - FlatbuffersSceneDataProvider
  ↑
scenes (Layer 5)
  - SceneFactory
```

**No cycles!** Each layer only depends on lower layers.

## Final Library Dependency Graph

```
Layer 0: types
  Dependencies: SFML, stduuid, flatbuffers

Layer 1: interfaces
  Dependencies: types

Layer 2: events, components
  events: types, logger
  components: types

Layer 3: entity
  Dependencies: types, interfaces, events, components, user_interface, configuration

Layer 4: data_providers
  Dependencies: types, interfaces, entity, events, configuration

Layer 5: logic, scenes, assets
  logic: entity, components, types, context, resources
  scenes: entity, context, logic, data_providers, interfaces, assets
  assets: types, interfaces, user_interface, data_providers
```

## Migration Checklist

- [ ] Create `src/interfaces/IEntityImporter.h` (move from entity)
- [ ] Create `src/interfaces/IEntityExporter.h` (move from entity)
- [ ] Create `src/interfaces/IEntityConfigurator.h` (move from entity)
- [ ] Update `src/entity/FlatbuffersEntityImporter.h` includes
- [ ] Update `src/entity/FlatbuffersEntityConfigurator.h` includes
- [ ] Update `src/entity/CMakeLists.txt` (remove data_providers dependency)
- [ ] Update `src/data_providers/CMakeLists.txt` (add entity dependency)
- [ ] Update `src/interfaces/SceneLoadData.h` to include IEntityImporter
- [ ] Update all files that include moved interfaces
- [ ] Build and verify no circular dependencies
- [ ] Run tests

## Key Principles

1. **Dependency Inversion**: High-level modules depend on abstractions (interfaces), not implementations
2. **Layering**: Each layer depends only on lower layers
3. **Interface Segregation**: Interfaces separate from implementations
4. **Single Responsibility**: `entity` manages entities, `data_providers` loads data

## Benefits of This Structure

1. ✅ No circular dependencies
2. ✅ Clean separation of concerns
3. ✅ `entity` library is independent of data loading
4. ✅ `data_providers` can create any entity importer implementation
5. ✅ Extensible for new importers (JSON, network, etc.)
6. ✅ Testable: can mock IEntityImporter in tests
7. ✅ Follows SOLID principles

---

**Document Version**: 1.0  
**Date**: 2026-01-07  
**Status**: Ready for implementation  
**Estimated Time**: 2-3 hours (mostly moving files and updating includes)
