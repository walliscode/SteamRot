# Implementation Guide: IEntityImporter/Exporter Pattern

## Overview

This guide provides step-by-step instructions for implementing the IEntityImporter/Exporter pattern to eliminate the current SceneData inheritance pattern that uses downcasting.

**Goal**: Replace the current architecture where FbsSceneData uses inheritance to smuggle FlatBuffers pointers with a cleaner pattern that separates data (SceneData) from operations (Importer/Exporter).

**Benefits**:
- No dynamic_cast or const_cast needed
- SceneData becomes a simple struct (no polymorphism)
- Symmetric import/export architecture
- Type-safe at compile time
- Extensible for new data formats

## Prerequisites

Before starting:
1. Understand the current architecture (see analysis documents in `documentation/analysis/`)
2. Ensure all tests pass before making changes
3. Work in small increments, testing after each step
4. Keep a backup or work on a feature branch

## Implementation Phases

### Phase 1: Create New Interfaces (Non-Breaking)
### Phase 2: Implement FlatBuffers Importer
### Phase 3: Update SceneData and Remove Polymorphism
### Phase 4: Update SceneFactory and Provider
### Phase 5: Remove Old Code
### Phase 6: Update Tests

---

## Phase 1: Create New Interfaces

### Step 1.1: Create IEntityImporter Interface

**File**: `src/entity/IEntityImporter.h`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Declaration of IEntityImporter interface
/////////////////////////////////////////////////

#pragma once

#include "FailInfo.h"
#include "containers.h"
#include <expected>

namespace steamrot {

/////////////////////////////////////////////////
/// @class IEntityImporter
/// @brief Interface for importing entity data into EntityMemoryPool.
///
/// Implementations read from various sources and configure runtime entities.
/// The importer wraps the data source and provides a format-agnostic
/// interface for entity import operations.
/////////////////////////////////////////////////
class IEntityImporter {
public:
  /////////////////////////////////////////////////
  /// @brief Virtual destructor
  /////////////////////////////////////////////////
  virtual ~IEntityImporter() = default;

  /////////////////////////////////////////////////
  /// @brief Import entities into the given EntityMemoryPool.
  ///
  /// @param emp EntityMemoryPool to populate with entity data
  /// @return std::monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ImportEntities(EntityMemoryPool &emp) = 0;
};

} // namespace steamrot
```

**Test**: No tests needed yet (interface only).

### Step 1.2: Create IEntityExporter Interface

**File**: `src/entity/IEntityExporter.h`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Declaration of IEntityExporter interface
/////////////////////////////////////////////////

#pragma once

#include "FailInfo.h"
#include "containers.h"
#include <expected>
#include <memory>
#include <cstdint>

namespace steamrot {

/////////////////////////////////////////////////
/// @class IEntityExporter
/// @brief Interface for exporting EntityMemoryPool to serialized format.
///
/// Implementations convert runtime entity data to specific formats
/// (FlatBuffers, JSON, etc.) for saving or transmission.
/////////////////////////////////////////////////
class IEntityExporter {
public:
  /////////////////////////////////////////////////
  /// @brief Virtual destructor
  /////////////////////////////////////////////////
  virtual ~IEntityExporter() = default;

  /////////////////////////////////////////////////
  /// @brief Export entities to serialized binary format.
  ///
  /// @param emp EntityMemoryPool containing entities to export
  /// @param out_size Output parameter for binary data size
  /// @return Unique pointer to binary data or FailInfo on error
  /////////////////////////////////////////////////
  virtual std::expected<std::unique_ptr<uint8_t[]>, FailInfo>
  ExportEntities(const EntityMemoryPool &emp, size_t &out_size) = 0;
};

} // namespace steamrot
```

**Test**: No tests needed yet (interface only).

### Step 1.3: Update CMakeLists.txt

Add the new interface files to the entity library:

**File**: `src/entity/CMakeLists.txt`

```cmake
# Add to existing source files
target_sources(entity PRIVATE
  # ... existing files ...
  IEntityImporter.h
  IEntityExporter.h
)
```

**Build**: `cmake --build --preset Debug`

**Expected**: Should compile without errors.

---

## Phase 2: Implement FlatBuffers Importer

### Step 2.1: Create FlatbuffersEntityImporter

**File**: `src/entity/FlatbuffersEntityImporter.h`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Declaration of FlatbuffersEntityImporter class
/////////////////////////////////////////////////

#pragma once

#include "IEntityImporter.h"
#include "IEntityConfigurator.h"
#include "entities_generated.h"
#include "EventHandler.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @class FlatbuffersEntityImporter
/// @brief Imports entities from FlatBuffers EntityCollectionFbs data.
///
/// This class wraps the EntityCollectionFbs reference and provides
/// the IEntityImporter interface. Internally, it uses
/// FlatbuffersEntityConfigurator to perform the actual import.
/////////////////////////////////////////////////
class FlatbuffersEntityImporter : public IEntityImporter {
private:
  /////////////////////////////////////////////////
  /// @brief Reference to EventHandler for creating subscribers
  /////////////////////////////////////////////////
  EventHandler &m_event_handler;

  /////////////////////////////////////////////////
  /// @brief Reference to the EntityCollectionFbs data
  /////////////////////////////////////////////////
  const EntityCollectionFbs &m_entity_collection;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor
  ///
  /// @param event_handler Reference to event handler for UI events
  /// @param entity_collection FlatBuffers entity data
  /////////////////////////////////////////////////
  FlatbuffersEntityImporter(EventHandler &event_handler,
                            const EntityCollectionFbs &entity_collection);

  /////////////////////////////////////////////////
  /// @brief Import entities from FlatBuffers into EntityMemoryPool
  ///
  /// Creates a FlatbuffersEntityConfigurator and delegates to it.
  ///
  /// @param emp EntityMemoryPool to populate
  /// @return std::monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ImportEntities(EntityMemoryPool &emp) override;
};

} // namespace steamrot
```

**File**: `src/entity/FlatbuffersEntityImporter.cpp`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersEntityImporter class
/////////////////////////////////////////////////

#include "FlatbuffersEntityImporter.h"
#include "FlatbuffersEntityConfigurator.h"

namespace steamrot {

/////////////////////////////////////////////////
FlatbuffersEntityImporter::FlatbuffersEntityImporter(
    EventHandler &event_handler,
    const EntityCollectionFbs &entity_collection)
    : m_event_handler(event_handler),
      m_entity_collection(entity_collection) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersEntityImporter::ImportEntities(EntityMemoryPool &emp) {
  
  // Create configurator with the wrapped data
  FlatbuffersEntityConfigurator configurator(m_event_handler,
                                             m_entity_collection);
  
  // Delegate to configurator to do the actual work
  return configurator.ConfigureEntityMemoryPool(emp);
}

} // namespace steamrot
```

### Step 2.2: Update CMakeLists.txt

**File**: `src/entity/CMakeLists.txt`

```cmake
target_sources(entity PRIVATE
  # ... existing files ...
  IEntityImporter.h
  IEntityExporter.h
  FlatbuffersEntityImporter.h
  FlatbuffersEntityImporter.cpp
)
```

**Build**: `cmake --build --preset Debug`

**Expected**: Should compile without errors.

### Step 2.3: Write Tests for FlatbuffersEntityImporter

**File**: `tests/unit/entity/FlatbuffersEntityImporter.test.cpp`

```cpp
#include "FlatbuffersEntityImporter.h"
#include "TestContext.h"
#include "entity_memory.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FlatbuffersEntityImporter constructor", 
          "[unit][FlatbuffersEntityImporter]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;
  
  auto logic_context = test_context.GetLogicContextForTestScene();
  
  // Load test scene data
  steamrot::FlatbuffersDataLoader loader;
  auto scene_data_result = loader.ProvideDefaultSceneData(
      steamrot::SceneType::SceneType_TEST);
  REQUIRE(scene_data_result.has_value());
  
  const auto *entity_collection = 
      scene_data_result.value()->entity_collection();
  REQUIRE(entity_collection != nullptr);
  
  // Create importer
  steamrot::FlatbuffersEntityImporter importer(
      logic_context.event_handler,
      *entity_collection);
  
  SUCCEED("FlatbuffersEntityImporter created successfully");
}

TEST_CASE("FlatbuffersEntityImporter imports entities", 
          "[unit][FlatbuffersEntityImporter]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;
  
  auto logic_context = test_context.GetLogicContextForTestScene();
  
  // Load test scene data
  steamrot::FlatbuffersDataLoader loader;
  auto scene_data_result = loader.ProvideDefaultSceneData(
      steamrot::SceneType::SceneType_TEST);
  REQUIRE(scene_data_result.has_value());
  
  const auto *entity_collection = 
      scene_data_result.value()->entity_collection();
  REQUIRE(entity_collection != nullptr);
  
  // Create importer
  steamrot::FlatbuffersEntityImporter importer(
      logic_context.event_handler,
      *entity_collection);
  
  // Create empty EntityMemoryPool
  steamrot::EntityMemoryPool emp;
  
  // Import entities
  auto result = importer.ImportEntities(emp);
  REQUIRE(result.has_value());
  
  // Verify entities were imported
  size_t pool_size = steamrot::entity::memory::GetMemoryPoolSize(emp);
  REQUIRE(pool_size > 0);
}
```

**Add to**: `tests/unit/entity/CMakeLists.txt`

```cmake
add_executable(test_entity
  # ... existing tests ...
  FlatbuffersEntityImporter.test.cpp
)
```

**Test**: `ctest --preset Debug -R FlatbuffersEntityImporter`

**Expected**: Tests should pass (you'll run this locally).

---

## Phase 3: Update SceneData and Remove Polymorphism

### Step 3.1: Remove Virtual Destructor from SceneData

**File**: `src/types/core/SceneData.h`

```cpp
// BEFORE:
struct SceneData {
  virtual ~SceneData() = default;  // ← REMOVE THIS
  
  SceneInfo scene_info;
  SceneResourcesConfig scene_resources_config;
  AssetConfig scene_asset_config;
};

// AFTER:
struct SceneData {
  // No virtual destructor - plain struct
  
  SceneInfo scene_info;
  SceneResourcesConfig scene_resources_config;
  AssetConfig scene_asset_config;
};
```

**Build**: `cmake --build --preset Debug`

**Expected**: May see warnings about derived classes, but should compile.

### Step 3.2: Create SceneLoadData Struct

**File**: `src/types/core/SceneLoadData.h`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Declaration of SceneLoadData struct
/////////////////////////////////////////////////

#pragma once

#include "SceneData.h"
#include "IEntityImporter.h"
#include <memory>

namespace steamrot {

/////////////////////////////////////////////////
/// @struct SceneLoadData
/// @brief Contains both scene configuration and entity importer.
///
/// This struct is returned by data providers and contains:
/// - SceneData: Lightweight configuration (scene info, resources, assets)
/// - IEntityImporter: Wraps heavy entity data for import
/////////////////////////////////////////////////
struct SceneLoadData {
  /////////////////////////////////////////////////
  /// @brief Scene configuration data
  /////////////////////////////////////////////////
  SceneData scene_data;

  /////////////////////////////////////////////////
  /// @brief Entity importer (wraps entity data source)
  /////////////////////////////////////////////////
  std::unique_ptr<IEntityImporter> entity_importer;
};

} // namespace steamrot
```

**Build**: `cmake --build --preset Debug`

**Expected**: Should compile.

---

## Phase 4: Update SceneFactory and Provider

### Step 4.1: Update ISceneDataProvider Interface

**File**: `src/interfaces/ISceneDataProvider.h`

```cpp
// Add include
#include "SceneLoadData.h"

class ISceneDataProvider {
public:
  virtual ~ISceneDataProvider() = default;

  // KEEP OLD METHOD (for backward compatibility during migration)
  virtual std::expected<std::unique_ptr<SceneData>, FailInfo>
  ProvideDefaultSceneData(const SceneType scene_type) const = 0;

  // ADD NEW METHOD
  /////////////////////////////////////////////////
  /// @brief Provides scene configuration and entity importer together.
  ///
  /// @param scene_type Type of scene to load
  /// @return SceneLoadData containing both config and importer
  /////////////////////////////////////////////////
  virtual std::expected<SceneLoadData, FailInfo>
  ProvideSceneLoadData(const SceneType scene_type) const = 0;

  virtual std::expected<std::unique_ptr<SceneData>, FailInfo>
  ProvideSceneDataFromData(const SceneDataFbs *scene_data_fbs) const = 0;
};
```

### Step 4.2: Implement New Method in FlatbuffersSceneDataProvider

**File**: `src/data_providers/FlatbuffersSceneDataProvider.h`

Add method declaration:

```cpp
std::expected<SceneLoadData, FailInfo>
ProvideSceneLoadData(const SceneType scene_type) const override;
```

**File**: `src/data_providers/FlatbuffersSceneDataProvider.cpp`

Add implementation:

```cpp
#include "FlatbuffersEntityImporter.h"
#include "SceneLoadData.h"

/////////////////////////////////////////////////
std::expected<SceneLoadData, FailInfo>
FlatbuffersSceneDataProvider::ProvideSceneLoadData(
    const SceneType scene_type) const {
  
  // Load FlatBuffers data
  FlatbuffersDataLoader data_loader;
  auto load_result = data_loader.ProvideDefaultSceneData(scene_type);
  if (!load_result)
    return std::unexpected(load_result.error());
  
  const SceneDataFbs &fb_data = *load_result.value();
  
  // Create SceneData (lightweight, no entity pointer)
  SceneData scene_data;
  
  // Configure SceneInfo
  auto info_result = ConfigureSceneInfo(scene_data.scene_info,
                                        fb_data.scene_info());
  if (!info_result)
    return std::unexpected(info_result.error());
  
  // Configure SceneResourcesConfig
  if (fb_data.scene_resources_config()) {
    auto resources_result = ConfigureSceneResourcesConfig(
        scene_data.scene_resources_config,
        fb_data.scene_resources_config());
    if (!resources_result)
      return std::unexpected(resources_result.error());
  }
  
  // Configure AssetConfig
  if (fb_data.asset_config()) {
    auto asset_result = ConfigureAssetConfig(
        scene_data.scene_asset_config,
        fb_data.asset_config());
    if (!asset_result)
      return std::unexpected(asset_result.error());
  }
  
  // Create EntityImporter with FlatBuffers entity collection
  if (!fb_data.entity_collection()) {
    return std::unexpected(FailInfo{
        FailMode::FlatbuffersDataNotFound,
        "entity_collection not found in SceneDataFbs"});
  }
  
  // NOTE: We need EventHandler reference here
  // This is a challenge - we'll need to pass it or store it
  // For now, we'll need to restructure how provider gets EventHandler
  
  // TEMPORARY: Return error indicating we need refactoring
  return std::unexpected(FailInfo{
      FailMode::NotImplemented,
      "ProvideSceneLoadData needs EventHandler access - requires refactoring"});
}
```

**Note**: This reveals a design issue - the Provider needs access to EventHandler to create the Importer. See Step 4.3 for solution.

### Step 4.3: Refactor Provider to Accept EventHandler

**Option A**: Pass EventHandler to ProvideSceneLoadData

Update interface:

```cpp
virtual std::expected<SceneLoadData, FailInfo>
ProvideSceneLoadData(const SceneType scene_type, 
                     EventHandler &event_handler) const = 0;
```

**Option B**: Store EventHandler reference in Provider

Update FlatbuffersSceneDataProvider constructor:

```cpp
// In header
class FlatbuffersSceneDataProvider : public ISceneDataProvider {
private:
  EventHandler &m_event_handler;

public:
  FlatbuffersSceneDataProvider(EventHandler &event_handler);
  // ...
};

// In implementation
FlatbuffersSceneDataProvider::FlatbuffersSceneDataProvider(
    EventHandler &event_handler)
    : m_event_handler(event_handler) {}
```

**Recommended**: Option B (store reference) - cleaner interface.

**Implementation** (Option B):

Update `FlatbuffersSceneDataProvider.cpp`:

```cpp
std::expected<SceneLoadData, FailInfo>
FlatbuffersSceneDataProvider::ProvideSceneLoadData(
    const SceneType scene_type) const {
  
  // ... (previous code for loading and configuring SceneData) ...
  
  // Create EntityImporter with FlatBuffers entity collection
  auto entity_importer = std::make_unique<FlatbuffersEntityImporter>(
      m_event_handler,
      *fb_data.entity_collection());
  
  // Return both
  return SceneLoadData{
    .scene_data = std::move(scene_data),
    .entity_importer = std::move(entity_importer)
  };
}
```

### Step 4.4: Update SceneFactory to Use New Pattern

**File**: `src/scenes/SceneFactory.cpp`

Add new method that accepts SceneLoadData directly:

```cpp
/////////////////////////////////////////////////
std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateSceneFromSceneLoadData(SceneLoadData &load_data,
                                           const SceneType scene_type) {
  
  // Create empty scene
  auto scene_result = CreateEmptyScene(scene_type);
  if (!scene_result)
    return std::unexpected(scene_result.error());
  
  std::unique_ptr<Scene> scene = std::move(scene_result.value());
  
  // Configure scene-level settings (no entities)
  ISceneConfigurator &configurator = GetSceneConfigurator();
  auto config_result = configurator.ConfigureScene(*scene, 
                                                   load_data.scene_data);
  if (!config_result)
    return std::unexpected(config_result.error());
  
  // Import entities using the importer
  auto import_result = load_data.entity_importer->ImportEntities(
      scene->GetSceneContext().scene_entities);
  if (!import_result)
    return std::unexpected(import_result.error());
  
  return std::move(scene);
}
```

**File**: `src/scenes/SceneFactory.h`

Add method declaration:

```cpp
/////////////////////////////////////////////////
/// @brief Create a scene from SceneLoadData.
///
/// @param load_data SceneLoadData containing SceneData and IEntityImporter
/// @param scene_type The type of scene to create
/// @return Unique pointer to created Scene, or FailInfo on error
/////////////////////////////////////////////////
std::expected<std::unique_ptr<Scene>, FailInfo>
CreateSceneFromSceneLoadData(SceneLoadData &load_data,
                              const SceneType scene_type);
```

### Step 4.5: Update ISceneConfigurator Interface

Change from pointer to reference:

**File**: `src/interfaces/ISceneConfigurator.h`

```cpp
// BEFORE:
std::expected<std::monostate, FailInfo>
ConfigureScene(Scene &scene, const SceneData *scene_data);

virtual std::expected<std::monostate, FailInfo>
ConfigureSceneInfo(Scene &scene, const SceneData *scene_data) = 0;

// AFTER:
std::expected<std::monostate, FailInfo>
ConfigureScene(Scene &scene, const SceneData &scene_data);

virtual std::expected<std::monostate, FailInfo>
ConfigureSceneInfo(Scene &scene, const SceneData &scene_data) = 0;
```

Update all method signatures to use `const SceneData&` instead of `const SceneData*`.

**File**: `src/scenes/ISceneConfigurator.cpp`

Update implementation:

```cpp
std::expected<std::monostate, FailInfo>
ISceneConfigurator::ConfigureScene(Scene &scene, const SceneData &scene_data) {
  // No null check needed (references can't be null)
  
  auto config_info_result = ConfigureSceneInfo(scene, scene_data);
  if (!config_info_result)
    return std::unexpected(config_info_result.error());

  auto config_resources_result = ConfigureSceneResources(scene, scene_data);
  if (!config_resources_result)
    return std::unexpected(config_resources_result.error());

  auto config_config_result = ConfigureSceneConfig(scene, scene_data);
  if (!config_config_result)
    return std::unexpected(config_config_result.error());

  // NOTE: ConfigureEntities is REMOVED - done by Importer now

  auto config_logic_result = ConfigureLogicMap(scene);
  if (!config_logic_result)
    return std::unexpected(config_logic_result.error());

  auto pass_asset_result = PassAssetConfig(scene, scene_data);
  if (!pass_asset_result)
    return std::unexpected(pass_asset_result.error());

  return std::monostate{};
}
```

### Step 4.6: Update FlatbuffersSceneConfigurator

**File**: `src/scenes/FlatbuffersSceneConfigurator.h`

Update method signatures:

```cpp
std::expected<std::monostate, FailInfo>
ConfigureSceneInfo(Scene &scene, const SceneData &scene_data) override;

std::expected<std::monostate, FailInfo>
ConfigureSceneResources(Scene &scene, const SceneData &scene_data) override;

std::expected<std::monostate, FailInfo>
ConfigureSceneConfig(Scene &scene, const SceneData &scene_data) override;

// REMOVE ConfigureEntities method entirely
```

**File**: `src/scenes/FlatbuffersSceneConfigurator.cpp`

Update implementations:

```cpp
std::expected<std::monostate, FailInfo>
FlatbuffersSceneConfigurator::ConfigureSceneInfo(
    Scene &scene,
    const SceneData &scene_data) {
  
  // No null check or casting needed
  
  if (scene_data.scene_info.id.is_nil()) {
    scene.GetSceneInfo().id = uuids::uuid_system_generator{}();
  } else {
    scene.GetSceneInfo().id = scene_data.scene_info.id;
  }
  
  scene.GetSceneInfo().type = scene_data.scene_info.type;
  return std::monostate{};
}

// Similar updates for other methods - remove null checks and casting
```

**Build**: `cmake --build --preset Debug`

**Expected**: Should compile (may have some errors to fix).

---

## Phase 5: Remove Old Code

### Step 5.1: Delete FbsSceneData

**File**: `src/types/core/FbsSceneData.h`

**Action**: DELETE this entire file.

```bash
rm src/types/core/FbsSceneData.h
```

### Step 5.2: Remove FbsSceneData from Includes

Search and remove all:

```cpp
#include "FbsSceneData.h"
```

Files to update:
- `src/data_providers/FlatbuffersSceneDataProvider.cpp`
- `src/scenes/FlatbuffersSceneConfigurator.cpp`

### Step 5.3: Update CreateSceneFromDefault Method

Update `CreateSceneFromDefault` to be a wrapper that loads data and calls `CreateSceneFromSceneLoadData`:

**File**: `src/scenes/SceneFactory.cpp`

```cpp
std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateSceneFromDefault(SceneType type) {
  
  // Get provider
  auto get_provider_result =
      m_game_context.data_access_factory.GetSceneDataProvider();
  if (!get_provider_result)
    return std::unexpected(get_provider_result.error());
  
  ISceneDataProvider &provider = *get_provider_result.value();
  
  // Get SceneLoadData (contains SceneData + Importer)
  auto load_result = provider.ProvideSceneLoadData(type);
  if (!load_result)
    return std::unexpected(load_result.error());
  
  SceneLoadData &load_data = load_result.value();
  
  // Call the core implementation with loaded data
  return CreateSceneFromSceneLoadData(load_data, type);
}
```

**Design**: 
- `CreateSceneFromSceneLoadData` is the core method that accepts pre-loaded data
- `CreateSceneFromDefault` is a convenience wrapper that loads data from provider first
- This allows direct scene creation from SceneLoadData when needed (testing, custom loading, etc.)

**Build**: `cmake --build --preset Debug`

**Expected**: Should compile cleanly now.

---

## Phase 6: Update Tests

### Step 6.1: Update Configurator Tests

**File**: `tests/unit/scenes/FlatbuffersSceneConfigurator.test.cpp`

Update to use references instead of pointers:

```cpp
// BEFORE:
const SceneData *data = /* ... */;
auto result = configurator.ConfigureScene(scene, data);

// AFTER:
const SceneData &data = load_data.scene_data;
auto result = configurator.ConfigureScene(scene, data);
```

### Step 6.2: Update SceneFactory Tests

Update tests to use new `CreateSceneFromSceneLoadData` method or verify that `CreateSceneFromDefault` still works.

### Step 6.3: Run All Tests

```bash
ctest --preset Debug
```

**Expected**: All tests should pass.

**If tests fail**: Debug and fix issues. Common problems:
- Null pointer dereferences (should be caught at compile time now)
- Incorrect reference/pointer usage
- Missing EventHandler references

---

## Verification Checklist

After completing all phases, verify:

- [ ] Code compiles without errors
- [ ] All tests pass
- [ ] No `dynamic_cast` calls involving SceneData
- [ ] No `const_cast` calls involving SceneData
- [ ] `FbsSceneData.h` is deleted
- [ ] `SceneData` has no virtual destructor
- [ ] All `ISceneConfigurator` methods use `const SceneData&`
- [ ] `FlatbuffersEntityImporter` tests pass
- [ ] SceneFactory successfully creates scenes
- [ ] Memory usage is reasonable (no leaks)

---

## Common Issues and Solutions

### Issue 1: EventHandler Access in Provider

**Problem**: Provider needs EventHandler to create Importer.

**Solution**: Store EventHandler reference in Provider (see Step 4.3 Option B).

### Issue 2: FlatBuffers Data Lifetime

**Problem**: EntityCollectionFbs pointer in Importer becomes invalid.

**Solution**: Ensure FlatBuffers data remains valid for Importer lifetime. Either:
- Provider keeps FlatBuffers data alive
- Importer makes a copy (expensive)
- Use shared_ptr for FlatBuffers data

### Issue 3: Test Data Access

**Problem**: Tests need access to EntityCollectionFbs.

**Solution**: Use TestContext or create test helper to provide SceneLoadData.

### Issue 4: Compilation Errors After Removing FbsSceneData

**Problem**: Files still include or use FbsSceneData.

**Solution**: Search entire codebase:
```bash
grep -r "FbsSceneData" src/
```

Remove all references.

---

## Performance Considerations

### Before (with downcasting):
- Virtual function call overhead
- Runtime type checking (dynamic_cast)
- Potential cache misses from vtable lookups

### After (with Importer):
- No virtual calls for SceneData
- Compile-time type safety
- Better optimization opportunities
- Cleaner, more maintainable code

---

## Next Steps

After successful implementation:

1. **Documentation**: Update copilot-instructions.md to reflect new pattern
2. **Export**: Implement IEntityExporter and FlatbuffersEntityExporter
3. **Other Formats**: Add JsonEntityImporter, NetworkEntityImporter, etc.
4. **Save/Load**: Use Importer/Exporter for game save functionality
5. **Refactoring**: Apply similar patterns to other polymorphic data structures

---

## Support

If you encounter issues:

1. Check the analysis documents in `documentation/analysis/` for detailed explanations
2. Review the code examples in SCENEDATA_CODE_EXAMPLES.md
3. Consult the architecture diagrams in SCENEDATA_VISUAL_DIAGRAMS.md
4. Ask for help with specific compilation or runtime errors

---

**Guide Version**: 1.0  
**Date**: 2026-01-06  
**Based On**: Analysis documents in documentation/analysis/  
**Target**: Phase 2 (Importer/Exporter Pattern) implementation
