# Data Architecture Migration Plan - Step-by-Step Guide

**Date**: December 13, 2025  
**Purpose**: Practical guide for implementing the three-layer data architecture  
**Estimated Total Time**: 6-8 weeks (can be done incrementally)  
**Status**: ⚠️ Being updated to reflect simplified SaveData approach

---

## ⚠️ Important Note

**SaveData Structure Simplified**: SaveData is now a simple container with `std::vector<std::unique_ptr<SceneData>> scenes`. This simplifies Phase 3 significantly!

**Key Changes from Original Plan**:
- ✅ No separate SaveSceneData polymorphic type
- ✅ Reuse FlatbuffersSceneDataProvider (add ProvideSceneDataFromSave method)
- ✅ Reuse FlatbuffersSceneConfigurator (same logic for default and save)
- ✅ Phase 3 is simpler and faster (1-2 weeks instead of 2-4 weeks)

**Below references to "SaveSceneData" should be understood as "SceneData from SaveData.scenes vector".**

---

## Overview

This guide provides detailed, step-by-step instructions for migrating the SteamRot codebase to the three-layer architecture with polymorphic structs and provider-configurator separation.

**What you'll achieve**:
- ✅ Break circular dependencies (scenes ↔ data_providers)
- ✅ Clean three-layer architecture
- ✅ Complete save/load infrastructure (simplified approach)
- ✅ Extensible data source system (default, save, test)

**Prerequisites**:
- Read `documentation/analysis/DATA_ARCHITECTURE_EXECUTIVE_SUMMARY.md`
- Understanding of current codebase structure
- Basic CMake knowledge

---

## Quick Reference

| Phase | Duration | Priority | Difficulty | Dependencies |
|-------|----------|----------|------------|--------------|
| Phase 1: Extract Interfaces | Week 1 | HIGH | Low | None |
| Phase 2: Refactor Configurators | Week 2 | HIGH | Medium | Phase 1 |
| Phase 3: Save Infrastructure | Weeks 3-4 | MEDIUM | High | Phase 2 |
| Phase 4: Break Circular Deps | Weeks 5-6 | MEDIUM | Medium | Phase 1 |
| Phase 5: Reorganize Packages | Weeks 7-8 | LOW | Low | All above |

**Recommended Order**: Phases 1, 2, 4, 3, 5

---

## Phase 1: Extract Interfaces Package

**Goal**: Create `src/interfaces/` package with zero circular dependencies  
**Duration**: 1 week (8-12 hours)  
**Priority**: HIGH - Unblocks everything else

### Step 1.1: Create interfaces directory

```bash
cd /path/to/SteamRot
mkdir -p src/interfaces
```

### Step 1.2: Create CMakeLists.txt for interfaces

Create `src/interfaces/CMakeLists.txt`:

```cmake
add_library(interfaces INTERFACE)

target_include_directories(interfaces
  INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(interfaces
  INTERFACE
  types
)
```

**Why INTERFACE library?** Header-only library, no compiled code.

### Step 1.3: Extract ISceneDataProvider

**Current location**: `src/data_providers/ISceneDataProvider.h`

**Actions**:
1. Copy `src/data_providers/ISceneDataProvider.h` → `src/interfaces/ISceneDataProvider.h`
2. Update header to only include Layer 1 dependencies:
   ```cpp
   #pragma once
   
   #include "SceneData.h"
   #include "scene_types_generated.h"
   #include <memory>
   
   namespace steamrot {
   
   class ISceneDataProvider {
   public:
     virtual ~ISceneDataProvider() = default;
     
     virtual std::unique_ptr<SceneData>
     ProvideDefaultSceneData(const SceneType scene_type) const = 0;
   };
   
   } // namespace steamrot
   ```

3. Update `src/data_providers/FlatbuffersSceneDataProvider.h`:
   ```cpp
   #include "ISceneDataProvider.h"  // From interfaces/
   ```

4. Update any files that include ISceneDataProvider:
   ```bash
   # Find files that include it
   grep -r "ISceneDataProvider.h" src/
   
   # Update each file to use interfaces/
   # Change: #include "ISceneDataProvider.h"
   # To: #include "ISceneDataProvider.h"  (CMake handles path via target_link)
   ```

### Step 1.4: Extract ISceneConfigurator

**Current location**: `src/scenes/ISceneConfigurator.h`

**Actions**:
1. Copy `src/scenes/ISceneConfigurator.h` → `src/interfaces/ISceneConfigurator.h`
2. Review dependencies - ensure only Layer 1 types
3. Update `src/scenes/FlatbuffersDefaultSceneConfigurator.h` include
4. Update `src/scenes/CMakeLists.txt`:
   ```cmake
   target_link_libraries(scenes PUBLIC
     interfaces  # Add this
     # ... existing deps
   )
   ```

### Step 1.5: Extract IEntityConfigurator

**Current location**: `src/entity/IEntityConfigurator.h`

**Actions**:
1. Copy to `src/interfaces/IEntityConfigurator.h`
2. Update `src/entity/FlatbuffersEntityConfigurator.h` include
3. Update `src/entity/CMakeLists.txt` to link `interfaces`

### Step 1.6: Extract ISaveDataProvider

**Current location**: `src/configuration/ISaveDataProvider.h`

**Actions**:
1. Copy to `src/interfaces/ISaveDataProvider.h`
2. Update `src/configuration/FlatbuffersSaveDataProvider.h` include
3. Update `src/configuration/CMakeLists.txt` to link `interfaces`

### Step 1.7: Extract IAssetDataProvider

**Current location**: `src/data_providers/IAssetDataProvider.h`

**Actions**:
1. Copy to `src/interfaces/IAssetDataProvider.h`
2. Update implementations to include from `interfaces/`

### Step 1.8: Update src/CMakeLists.txt

Add interfaces to build:

```cmake
add_subdirectory(interfaces)  # Add before other subdirectories
add_subdirectory(types)
add_subdirectory(components)
# ... rest
```

### Step 1.9: Validation

```bash
# Clean build
rm -rf build
mkdir build && cd build

# Configure
cmake --preset Debug

# Build
cmake --build --preset Debug

# Verify no errors
echo $?  # Should be 0
```

**Common issues**:
- **Missing types**: Add to `types` package
- **Circular include**: Check that interfaces only depend on Layer 1
- **Link errors**: Ensure CMakeLists updated correctly

### Step 1.10: Commit Phase 1

```bash
git add src/interfaces/
git add src/*/CMakeLists.txt
git commit -m "Phase 1: Extract interfaces package

- Created src/interfaces/ with ISceneDataProvider, ISceneConfigurator, 
  IEntityConfigurator, ISaveDataProvider, IAssetDataProvider
- Updated all packages to link against interfaces
- Interfaces depend only on types (Layer 1)
- Breaks circular dependencies between scenes and data_providers"
```

---

## Phase 2: Refactor Configurators

**Goal**: Configurators receive `SceneData*`, don't load data  
**Duration**: 1 week (12-16 hours)  
**Priority**: HIGH - Enables provider/configurator separation

### Step 2.1: Update ISceneConfigurator interface

Edit `src/interfaces/ISceneConfigurator.h`:

**Before**:
```cpp
virtual std::expected<std::monostate, FailInfo>
ConfigureScene(Scene &scene, const SceneType scene_type) = 0;
```

**After**:
```cpp
virtual std::expected<std::monostate, FailInfo>
ConfigureScene(Scene &scene, const SceneData *data) = 0;

virtual std::expected<std::monostate, FailInfo>
ConfigureSceneInfo(Scene &scene, const SceneData *data) = 0;

virtual std::expected<std::monostate, FailInfo>
ConfigureSceneResources(Scene &scene, const SceneData *data) = 0;

virtual std::expected<std::monostate, FailInfo>
ConfigureSceneConfig(Scene &scene, const SceneData *data) = 0;

virtual std::expected<std::monostate, FailInfo>
ConfigureLogicMap(Scene &scene) = 0;
```

### Step 2.2: Refactor FlatbuffersDefaultSceneConfigurator

Edit `src/scenes/FlatbuffersDefaultSceneConfigurator.h`:

**Before**:
```cpp
class FlatbuffersDefaultSceneConfigurator : public ISceneConfigurator {
private:
  FlatbuffersDataLoader m_data_loader;  // REMOVE THIS

public:
  std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(Scene &scene, const SceneType scene_type) override;
  // ...
};
```

**After**:
```cpp
class FlatbuffersDefaultSceneConfigurator : public ISceneConfigurator {
public:
  std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneData *data) override;

  std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(Scene &scene, const SceneData *data) override;
  
  std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene &scene, const SceneData *data) override;
  
  std::expected<std::monostate, FailInfo>
  ConfigureSceneConfig(Scene &scene, const SceneData *data) override;
  
  std::expected<std::monostate, FailInfo>
  ConfigureLogicMap(Scene &scene) override;
};
```

### Step 2.3: Update ConfigureScene implementation

Edit `src/scenes/FlatbuffersDefaultSceneConfigurator.cpp`:

```cpp
std::expected<std::monostate, FailInfo>
FlatbuffersDefaultSceneConfigurator::ConfigureScene(
    Scene &scene, const SceneData *data) {
  
  // Downcast to FbsSceneData
  const FbsSceneData *fbs_data = dynamic_cast<const FbsSceneData*>(data);
  if (!fbs_data) {
    return std::unexpected(FailInfo{
      FailMode::InvalidData,
      "Expected FbsSceneData, got different type"
    });
  }
  
  // Configure scene components
  auto info_result = ConfigureSceneInfo(scene, data);
  if (!info_result.has_value()) {
    return std::unexpected(info_result.error());
  }
  
  auto resources_result = ConfigureSceneResources(scene, data);
  if (!resources_result.has_value()) {
    return std::unexpected(resources_result.error());
  }
  
  auto config_result = ConfigureSceneConfig(scene, data);
  if (!config_result.has_value()) {
    return std::unexpected(config_result.error());
  }
  
  auto logic_result = ConfigureLogicMap(scene);
  if (!logic_result.has_value()) {
    return std::unexpected(logic_result.error());
  }
  
  return std::monostate{};
}

std::expected<std::monostate, FailInfo>
FlatbuffersDefaultSceneConfigurator::ConfigureSceneInfo(
    Scene &scene, const SceneData *data) {
  
  const FbsSceneData *fbs_data = dynamic_cast<const FbsSceneData*>(data);
  const SceneDataFbs *fb = fbs_data->scene_data_fbs;
  
  // Configure SceneInfo from FlatBuffers
  scene.m_scene_info.type = data->scene_info.type;
  // ... rest of configuration
  
  return std::monostate{};
}

// Similar for ConfigureSceneResources, ConfigureSceneConfig
```

### Step 2.4: Update SceneFactory

Edit `src/scenes/SceneFactory.cpp`:

**Before**:
```cpp
std::unique_ptr<Scene> SceneFactory::CreateScene(SceneType type) {
  auto scene = CreateEmptyScene(type);
  
  ISceneConfigurator& configurator = GetDefaultSceneConfigurator();
  configurator.ConfigureScene(*scene, type);  // Old signature
  
  return scene;
}
```

**After**:
```cpp
std::unique_ptr<Scene> SceneFactory::CreateSceneFromDefault(SceneType type) {
  // Step 1: Get provider and configurator
  ISceneDataProvider& provider = GetFlatbuffersSceneDataProvider();
  ISceneConfigurator& configurator = GetFlatbuffersSceneConfigurator();
  
  // Step 2: Provider loads data
  std::unique_ptr<SceneData> data = provider.ProvideDefaultSceneData(type);
  if (!data) {
    // Log error
    return nullptr;
  }
  
  // Step 3: Create empty scene
  std::unique_ptr<Scene> scene = CreateEmptyScene(type);
  
  // Step 4: Configurator applies data
  auto config_result = configurator.ConfigureScene(*scene, data.get());
  if (!config_result.has_value()) {
    // Log error
    return nullptr;
  }
  
  return scene;
}
```

### Step 2.5: Update provider_factory

Edit `src/data_providers/provider_factory.h`:

Add:
```cpp
ISceneConfigurator& GetFlatbuffersSceneConfigurator();
```

Edit `src/data_providers/provider_factory.cpp`:

```cpp
ISceneConfigurator& GetFlatbuffersSceneConfigurator() {
  static FlatbuffersDefaultSceneConfigurator configurator;
  return configurator;
}
```

### Step 2.6: Validation

```bash
# Build
cmake --build --preset Debug

# Test that scenes still load
# Run game and verify title scene loads correctly
./build/Debug/steamrot

# Or run scene-specific tests
ctest --preset Debug -R scene
```

### Step 2.7: Commit Phase 2

```bash
git add src/scenes/ src/data_providers/ src/interfaces/
git commit -m "Phase 2: Refactor configurators to accept SceneData*

- Updated ISceneConfigurator::ConfigureScene() to accept SceneData*
- Removed FlatbuffersDataLoader from FlatbuffersDefaultSceneConfigurator
- Configurators now receive data, don't load it
- SceneFactory orchestrates provider + configurator
- Provider loads data, configurator applies it"
```

---

## Phase 3: Implement Save Infrastructure

**Goal**: Complete save/load workflow with SaveSceneData  
**Duration**: 2 weeks (20-30 hours)  
**Priority**: MEDIUM - Feature enablement

### Step 3.1: Update SaveData struct

Edit `src/configuration/ISaveDataProvider.h`:

**Add to SaveData struct**:
```cpp
struct SaveData {
  struct Metadata {
    std::string save_name;
    std::string created_at;
    std::string last_modified;
    std::string game_version;
    uint64_t play_time_seconds{0};
    uint32_t slot_index{0};
  } metadata;

  SceneType current_scene_type{SceneType::SceneType_UNKNOWN};
  uint32_t version{1};
  
  // NEW: Add vector to hold scene data
  std::vector<std::unique_ptr<SceneData>> scenes;  // Holds multiple scenes
};
```

**Key insight**: SaveData is just a container. No new polymorphic type needed!

### Step 3.2: Update FlatbuffersSceneDataProvider

Edit `src/data_providers/FlatbuffersSceneDataProvider.h`:

**Add method to extract from SaveData**:
```cpp
class FlatbuffersSceneDataProvider : public ISceneDataProvider {
public:
  // Existing method for default data
  std::unique_ptr<SceneData>
  ProvideDefaultSceneData(const SceneType scene_type) const override;
  
  // NEW: Extract SceneData from SaveData.scenes vector
  std::unique_ptr<SceneData>
  ProvideSceneDataFromSave(const SaveData &save_data,
                          const SceneType scene_type) const;
};

} // namespace steamrot
```

Create `src/data_providers/SaveSceneDataProvider.cpp`:

```cpp
#include "SaveSceneDataProvider.h"
#include "SaveSceneData.h"
#include "FlatbuffersDataLoader.h"

namespace steamrot {

std::unique_ptr<SceneData>
SaveSceneDataProvider::ProvideSceneDataFromSave(
    const SaveData &save_data,
    const SceneType scene_type) const {
  
  // Load saved scene FlatBuffers data
  FlatbuffersDataLoader loader;
  auto fb_result = loader.LoadSavedSceneData(save_data, scene_type);
  if (!fb_result.has_value()) {
    return nullptr;
  }
  
  // Create SaveSceneData (polymorphic)
  auto save_scene_data = std::make_unique<SaveSceneData>();
  save_scene_data->scene_info.type = scene_type;
  // TODO: Get UUID from save data
  save_scene_data->saved_scene_data_fbs = fb_result.value();
  save_scene_data->play_time_seconds = save_data.metadata.play_time_seconds;
  save_scene_data->last_modified = save_data.metadata.last_modified;
  
  return save_scene_data;
}

} // namespace steamrot
```

Edit `src/data_providers/FlatbuffersSceneDataProvider.cpp`:

```cpp
std::unique_ptr<SceneData>
FlatbuffersSceneDataProvider::ProvideSceneDataFromSave(
    const SaveData &save_data,
    const SceneType scene_type) const {
  
  // SaveData.scenes already contains SceneData objects
  // Find the matching scene in the vector
  for (const auto& scene : save_data.scenes) {
    if (scene->scene_info.type == scene_type) {
      // Return a copy (or move if possible)
      // The scene data is already in the correct format (likely FbsSceneData)
      return std::make_unique<SceneData>(*scene);
    }
  }
  
  return nullptr;  // Scene not found in save
}
```

**Key insight**: SaveData.scenes already contains SceneData objects (loaded from FlatBuffers). We just extract the one we need!

### Step 3.3: No separate configurator needed!

**Important**: Reuse `FlatbuffersSceneConfigurator` for save data!

The SceneData in SaveData.scenes is in FlatBuffers format (FbsSceneData), so the same configurator that handles default data will work for save data. No new configurator needed!

### Step 3.4: Implement EntitySerializer

Create `src/entity/EntitySerializer.h`:

```cpp
#pragma once

#include "EntityMemoryPool.h"
#include "entities_generated.h"
#include <flatbuffers/flatbuffers.h>

namespace steamrot {

class EntitySerializer {
public:
  /////////////////////////////////////////////////
  /// @brief Serialize EntityMemoryPool to FlatBuffers
  ///
  /// @param emp EntityMemoryPool to serialize
  /// @param builder FlatBuffers builder
  /// @return Offset to EntityCollection
  /////////////////////////////////////////////////
  static flatbuffers::Offset<EntityCollectionFbs>
  SerializeEntityPool(const EntityMemoryPool &emp,
                     flatbuffers::FlatBufferBuilder &builder);
  
private:
  static flatbuffers::Offset<EntityDataFbs>
  SerializeEntity(const EntityMemoryPool &emp,
                 size_t entity_index,
                 flatbuffers::FlatBufferBuilder &builder);
};

} // namespace steamrot
```

Implementation left as TODO - iterate through active entities and serialize components.

### Step 3.6: Update SceneFactory for save loading

Edit `src/scenes/SceneFactory.cpp`:

Add:
```cpp
std::unique_ptr<Scene> SceneFactory::CreateSceneFromSave(uint32_t save_slot) {
  // Step 1: Load SaveData
  ISaveDataProvider& save_provider = GetSaveDataProvider();
  auto save_result = save_provider.LoadSave(save_slot);
  if (!save_result.has_value()) {
    return nullptr;
  }
  
  const SaveData& save = save_result.value();
  
  // Step 2: Extract SceneData from SaveData
  ISceneDataProvider& scene_provider = GetSaveSceneDataProvider();
  std::unique_ptr<SceneData> scene_data = 
      scene_provider.ProvideSceneDataFromSave(save, save.current_scene_type);
  
  if (!scene_data) {
    return nullptr;
  }
  
  // Step 3: Configure scene
  ISceneConfigurator& configurator = GetSaveSceneConfigurator();
  std::unique_ptr<Scene> scene = CreateEmptyScene(save.current_scene_type);
  
  auto config_result = configurator.ConfigureScene(*scene, scene_data.get());
  if (!config_result.has_value()) {
    return nullptr;
  }
  
  return scene;
}
```

### Step 3.7: Validation

```bash
# Build
cmake --build --preset Debug

# Test save/load
# 1. Start game
# 2. Make some changes
# 3. Save game
# 4. Load game
# 5. Verify state restored correctly
```

### Step 3.8: Commit Phase 3

```bash
git add src/types/core/SaveSceneData.h
git add src/data_providers/SaveSceneDataProvider.*
git add src/scenes/SaveSceneConfigurator.*
git add src/entity/EntitySerializer.*
git commit -m "Phase 3: Implement save infrastructure

- Created SaveSceneData polymorphic struct
- Implemented SaveSceneDataProvider (extracts from SaveData)
- Implemented SaveSceneConfigurator (restores scene state)
- Added EntitySerializer for serializing EntityMemoryPool
- SceneFactory::CreateSceneFromSave() orchestrates three steps
- Save/load workflow now complete"
```

---

## Phase 4: Break Circular Dependencies

**Goal**: Eliminate scenes ↔ data_providers circular dependency  
**Duration**: 2 weeks (16-20 hours)  
**Priority**: MEDIUM - Architecture cleanup

### Step 4.1: Move SceneType to types

Currently `SceneType` enum is in `scene_types_generated.h` (FlatBuffers).

**Option A**: Keep in FlatBuffers (simplest)
- Leave as-is
- Both scenes and providers can include it

**Option B**: Create native enum (more work)
- Create `src/types/core/SceneType.h`
- Sync with FlatBuffers enum
- Use conversion functions

**Recommendation**: Option A for now.

### Step 4.2: Audit data_providers dependencies

```bash
# Check what data_providers depends on
grep -r "^#include" src/data_providers/*.h | grep -v "SFML\|std::\|expected"

# Look for any scenes/ includes
grep -r "#include.*scenes" src/data_providers/
```

If data_providers includes anything from scenes, extract it to types or interfaces.

### Step 4.3: Update CMakeLists.txt

Edit `src/data_providers/CMakeLists.txt`:

**Before**:
```cmake
target_link_libraries(data_providers PUBLIC
  types
  SFML::Graphics
  events
  scenes  # REMOVE THIS
)
```

**After**:
```cmake
target_link_libraries(data_providers PUBLIC
  types
  interfaces  # Only Layer 1
  SFML::Graphics
  events
)
```

### Step 4.4: Enable strict linker checks

Edit root `CMakeLists.txt`:

```cmake
# After project() declaration
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  # Detect circular dependencies at link time
  add_link_options(-Wl,--no-undefined)
endif()
```

### Step 4.5: Validation

```bash
# Clean rebuild
rm -rf build
mkdir build && cd build
cmake --preset Debug
cmake --build --preset Debug

# If build fails, you have circular deps to fix
# Error will show which symbols are undefined
```

### Step 4.6: Commit Phase 4

```bash
git add src/data_providers/CMakeLists.txt
git add CMakeLists.txt
git commit -m "Phase 4: Break circular dependencies

- Removed scenes dependency from data_providers
- data_providers now depends only on types and interfaces (Layer 1)
- Enabled -Wl,--no-undefined to catch future violations
- Clean three-layer architecture enforced"
```

---

## Phase 5: Reorganize Packages (Optional)

**Goal**: Rename packages to match architecture  
**Duration**: 2 weeks (12-16 hours)  
**Priority**: LOW - Nice to have

### Step 5.1: Rename data_providers → providers

```bash
git mv src/data_providers src/providers
```

Update all references:
```bash
# Find all includes
grep -r "data_providers" src/

# Update each file
# Change: #include "data_providers/..."
# To: #include "providers/..."
```

### Step 5.2: Create configurators package

```bash
mkdir src/configurators

# Move configurators from scenes/
git mv src/scenes/FlatbuffersDefaultSceneConfigurator.* src/configurators/FlatbuffersSceneConfigurator.*
git mv src/scenes/SaveSceneConfigurator.* src/configurators/

# Move configurators from entity/
git mv src/entity/FlatbuffersEntityConfigurator.* src/configurators/
```

Create `src/configurators/CMakeLists.txt`:

```cmake
add_library(configurators
  FlatbuffersSceneConfigurator.cpp
  SaveSceneConfigurator.cpp
  FlatbuffersEntityConfigurator.cpp
)

target_include_directories(configurators PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(configurators PUBLIC
  interfaces
  types
  providers
)
```

### Step 5.3: Rename scenes → scene_management

```bash
git mv src/scenes src/scene_management
```

Update all references to `scenes` → `scene_management`.

### Step 5.4: Commit Phase 5

```bash
git add -A
git commit -m "Phase 5: Reorganize packages

- Renamed data_providers → providers
- Created configurators package (extracted from scenes/entity)
- Renamed scenes → scene_management
- Clean three-layer architecture fully implemented"
```

---

## Troubleshooting

### Build errors after Phase 1

**Error**: "Cannot find ISceneDataProvider.h"

**Solution**: 
1. Check CMakeLists.txt links `interfaces`
2. Verify include path: `#include "ISceneDataProvider.h"` (not `#include "interfaces/..."`)
3. CMake handles path via `target_include_directories`

### Circular dependency detected

**Error**: "undefined reference to vtable"

**Solution**:
1. Check which package is missing a symbol
2. Ensure interfaces package has the declaration
3. Ensure implementation package links interfaces

### Tests failing after Phase 2

**Error**: "Scene failed to configure"

**Solution**:
1. Check that provider returns non-null SceneData
2. Verify dynamic_cast in configurator succeeds
3. Add logging to see which step fails

---

## Verification Checklist

After each phase:

### Build Check
- [ ] Clean build succeeds: `cmake --build --preset Debug`
- [ ] No warnings
- [ ] All tests pass: `ctest --preset Debug`

### Dependency Check
- [ ] No circular dependencies (use `--no-undefined` linker flag)
- [ ] Layer 1 has zero deps on other layers
- [ ] Layer 2 depends only on Layer 1
- [ ] Layer 3 depends on Layer 1 + Layer 2

### Functionality Check
- [ ] Game launches
- [ ] Title scene loads
- [ ] Can navigate between scenes
- [ ] (Phase 3+) Can save and load game

---

## Time Estimates

| Task | Time (hours) |
|------|--------------|
| Phase 1.1-1.10 | 8-12 |
| Phase 2.1-2.7 | 12-16 |
| Phase 3.1-3.8 | 20-30 |
| Phase 4.1-4.6 | 16-20 |
| Phase 5.1-5.4 | 12-16 |
| **Total** | **68-94 hours** |

**Realistic timeline**: 8 weeks at 10 hours/week

---

## Getting Help

If you encounter issues:

1. **Check documentation**:
   - `DATA_ARCHITECTURE_ANALYSIS.md` - Technical details
   - `DATA_ARCHITECTURE_DIAGRAMS.md` - Visual reference
   - `DATA_ARCHITECTURE_QUICK_REF.md` - Code snippets

2. **Common patterns**:
   - Provider loads data → returns `std::unique_ptr<SceneData>`
   - Configurator receives `SceneData*` → uses `dynamic_cast`
   - SceneFactory orchestrates provider + configurator

3. **Ask questions**: Open GitHub issue with "[Migration]" prefix

---

## Success Criteria

After completing all phases:

✅ **Architecture**:
- Zero circular dependencies
- Clean three-layer structure
- Interfaces separated from implementations

✅ **Functionality**:
- Game runs correctly
- All tests pass
- Save/load works

✅ **Code Quality**:
- Clean compile (no warnings)
- Passes linker checks (`--no-undefined`)
- Documentation updated

---

## Related Documents

- [DATA_ARCHITECTURE_EXECUTIVE_SUMMARY.md](analysis/DATA_ARCHITECTURE_EXECUTIVE_SUMMARY.md) - Overview
- [DATA_ARCHITECTURE_ANALYSIS.md](analysis/DATA_ARCHITECTURE_ANALYSIS.md) - Complete technical analysis
- [DATA_ARCHITECTURE_DIAGRAMS.md](analysis/DATA_ARCHITECTURE_DIAGRAMS.md) - Visual diagrams
- [DATA_ARCHITECTURE_QUICK_REF.md](analysis/DATA_ARCHITECTURE_QUICK_REF.md) - Quick reference

---

**Migration Plan Status**: ✅ Ready to use  
**Last Updated**: December 13, 2025  
**Version**: 1.0
