# SceneData Architecture: Detailed Code Examples

This document provides detailed code examples illustrating the current SceneData architecture and proposed alternatives for handling data import/export operations.

## Current Implementation

### 1. Import Flow (Loading Scenes)

#### Step 1: Data Provider Creates FbsSceneData

```cpp
// src/data_providers/FlatbuffersSceneDataProvider.cpp

std::expected<std::unique_ptr<SceneData>, FailInfo>
FlatbuffersSceneDataProvider::ProvideDefaultSceneData(
    const SceneType scene_type) const {
  
  // Load FlatBuffers binary file
  auto loader_result = FlatbuffersDataLoader::LoadFile("scene_data.bin");
  if (!loader_result)
    return std::unexpected(loader_result.error());
  
  const auto *root = GetSceneDataFbs(loader_result.value().data());
  
  // Create FbsSceneData (derived from SceneData)
  FbsSceneData scene_data;
  
  // Populate base SceneData fields
  scene_data.scene_info.type = scene_type;
  scene_data.scene_info.id = /* UUID from data */;
  scene_data.scene_resources_config.texture_width = 1920;
  scene_data.scene_resources_config.texture_height = 1080;
  scene_data.scene_asset_config = /* assets */;
  
  // **KEY POINT**: Attach pointer to FlatBuffers data
  // This is the "heavy struct" being passed around
  scene_data.entity_collection = root->entities();
  
  // Return as base pointer (polymorphism)
  return std::make_unique<FbsSceneData>(scene_data);
}
```

#### Step 2: Scene Factory Uses SceneData

```cpp
// Hypothetical scene factory code

auto scene_data = scene_data_provider.ProvideDefaultSceneData(SceneType::TITLE);
if (!scene_data)
  return std::unexpected(scene_data.error());

// scene_data is std::unique_ptr<SceneData>
// Actual type is FbsSceneData but we only have base pointer

auto scene = scene_factory.CreateScene(scene_type);
auto config_result = configurator.ConfigureScene(*scene, scene_data->get());
```

#### Step 3: Configurator Downcasts to Access Entity Data

```cpp
// src/scenes/FlatbuffersSceneConfigurator.cpp

std::expected<std::monostate, FailInfo>
FlatbuffersSceneConfigurator::ConfigureEntities(
    Scene &scene, const SceneData *scene_data) {
  
  // Null check
  if (!scene_data)
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "SceneData pointer is null"));
  
  // **KEY POINT**: Must downcast to derived type to access entity_collection
  // This requires const_cast because we have const SceneData*
  FbsSceneData *fbs_scene_data =
      dynamic_cast<FbsSceneData *>(const_cast<SceneData *>(scene_data));
  
  // Validate cast succeeded
  if (!fbs_scene_data)
    return std::unexpected(
        FailInfo(FailMode::InvalidCast, "SceneData is not FbsSceneData"));
  
  // Validate entity collection exists
  if (!fbs_scene_data->entity_collection)
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "EntityCollectionFbs not found"));
  
  // Create entity configurator with FlatBuffers data
  FlatbuffersEntityConfigurator entity_configurator(
      scene.GetSceneContext().event_handler,
      *fbs_scene_data->entity_collection);  // Pass heavy struct by reference
  
  // Configure runtime EntityMemoryPool
  auto emp_config_result = entity_configurator.ConfigureEntityMemoryPool(
      scene.GetSceneContext().scene_entities);
      
  if (!emp_config_result)
    return std::unexpected(emp_config_result.error());
  
  return std::monostate{};
}
```

#### Step 4: Entity Configurator Processes FlatBuffers Data

```cpp
// src/entity/FlatbuffersEntityConfigurator.cpp

std::expected<std::monostate, FailInfo>
FlatbuffersEntityConfigurator::ConfigureEntityMemoryPool(
    EntityMemoryPool &emp) {
  
  // m_entity_collection is const EntityCollectionFbs& (stored in configurator)
  
  // Resize pool to match data size
  size_t entity_count = m_entity_collection.entities()->size();
  entity::memory::ResizeEntityMemoryPool(emp, entity_count);
  
  // Iterate over FlatBuffers entities
  for (size_t i = 0; i < entity_count; ++i) {
    const auto *entity_data = m_entity_collection.entities()->Get(i);
    
    // Configure each component from FlatBuffers data
    if (entity_data->c_user_interface()) {
      auto &component = entity::memory::GetComponent<CUserInterface>(i, emp);
      auto result = ConfigureComponent(entity_data->c_user_interface(), 
                                       component);
      if (!result)
        return std::unexpected(result.error());
    }
    
    // ... configure other components ...
  }
  
  return std::monostate{};
}
```

### 2. Current Architecture Issues

#### Issue 1: Required Downcasting

```cpp
// Every configurator that needs entity data must do this:

// Cast away const (unsafe)
FbsSceneData *fbs_scene_data =
    dynamic_cast<FbsSceneData *>(const_cast<SceneData *>(scene_data));

// Check if cast succeeded (runtime check)
if (!fbs_scene_data) {
  // Handle error - wrong SceneData type
}

// Now can access entity_collection
process(fbs_scene_data->entity_collection);
```

**Problems**:
- Requires `const_cast` (removes const-correctness)
- Runtime type checking (could fail)
- Tight coupling to specific derived type
- Not clear from interface what type is expected

#### Issue 2: Export Has No Equivalent

For saving game state, we'd need:

```cpp
// Hypothetical export code - NO CLEAR PATTERN

// Want to do something like:
NativeSceneData export_data;  // DOESN'T EXIST
export_data.scene_info = scene.GetSceneInfo();
export_data.entity_memory_pool = &scene.GetEntityMemoryPool();

// Pass to exporter
auto binary = exporter.ExportScene(&export_data);
```

**But**:
- `NativeSceneData` doesn't exist
- No clear interface for exporters
- Asymmetric with import path
- Would still use inheritance for pointer-passing

## Proposed Solution 1: Create NativeSceneData (Minimal Change)

### Add Export-Specific Derived Type

```cpp
// src/types/core/NativeSceneData.h

/////////////////////////////////////////////////
/// @file
/// @brief Native (runtime) scene data for export operations
/////////////////////////////////////////////////

#pragma once

#include "SceneData.h"
#include "containers.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @struct NativeSceneData
/// @brief Extends SceneData with pointer to runtime EntityMemoryPool
///        for export operations (saving game state).
///
/// This is the export equivalent of FbsSceneData (used for import).
/// 
/// Usage:
///   - Import: FbsSceneData (FlatBuffers → Runtime)
///   - Export: NativeSceneData (Runtime → Serialized format)
/////////////////////////////////////////////////
struct NativeSceneData : public SceneData {

  /////////////////////////////////////////////////
  /// @brief Pointer to runtime EntityMemoryPool for export operations.
  ///
  /// Non-owning pointer to the scene's runtime entity data.
  /// Must remain valid for the lifetime of this NativeSceneData instance.
  /////////////////////////////////////////////////
  const EntityMemoryPool *entity_memory_pool = nullptr;
};

} // namespace steamrot
```

### Create Export Interface

```cpp
// src/interfaces/IEntityExporter.h

/////////////////////////////////////////////////
/// @file
/// @brief Interface for exporting entity data to serialized format
/////////////////////////////////////////////////

#pragma once

#include "FailInfo.h"
#include "NativeSceneData.h"
#include <expected>
#include <memory>
#include <cstdint>

namespace steamrot {

/////////////////////////////////////////////////
/// @class IEntityExporter
/// @brief Interface for exporting EntityMemoryPool to serialized format.
///
/// Implementations convert runtime entity data to specific formats:
/// - FlatbuffersEntityExporter: Converts to FlatBuffers binary
/// - JsonEntityExporter: Converts to JSON (future)
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
  /// @param scene_data NativeSceneData containing EntityMemoryPool pointer
  /// @param out_size Output parameter for binary data size
  /// @return Unique pointer to binary data or FailInfo on error
  /////////////////////////////////////////////////
  virtual std::expected<std::unique_ptr<uint8_t[]>, FailInfo>
  ExportEntities(const NativeSceneData *scene_data, size_t &out_size) = 0;
};

} // namespace steamrot
```

### Implement FlatBuffers Exporter

```cpp
// src/entity/FlatbuffersEntityExporter.h

#pragma once

#include "IEntityExporter.h"

namespace steamrot {

class FlatbuffersEntityExporter : public IEntityExporter {
public:
  FlatbuffersEntityExporter() = default;

  /////////////////////////////////////////////////
  /// @brief Export entities to FlatBuffers binary format.
  /////////////////////////////////////////////////
  std::expected<std::unique_ptr<uint8_t[]>, FailInfo>
  ExportEntities(const NativeSceneData *scene_data, 
                 size_t &out_size) override;

private:
  /////////////////////////////////////////////////
  /// @brief Build FlatBuffers EntityCollectionFbs from EntityMemoryPool
  /////////////////////////////////////////////////
  flatbuffers::Offset<EntityCollectionFbs>
  BuildEntityCollection(flatbuffers::FlatBufferBuilder &builder,
                       const EntityMemoryPool &emp);
  
  /////////////////////////////////////////////////
  /// @brief Build FlatBuffers EntityDataFbs for single entity
  /////////////////////////////////////////////////
  flatbuffers::Offset<EntityDataFbs>
  BuildEntityData(flatbuffers::FlatBufferBuilder &builder,
                 const EntityMemoryPool &emp,
                 size_t entity_id);
};

} // namespace steamrot
```

```cpp
// src/entity/FlatbuffersEntityExporter.cpp

#include "FlatbuffersEntityExporter.h"
#include "entity_memory.h"
#include "CUserInterface.h"
#include "CGrimoireMachina.h"
// ... other component includes

namespace steamrot {

/////////////////////////////////////////////////
std::expected<std::unique_ptr<uint8_t[]>, FailInfo>
FlatbuffersEntityExporter::ExportEntities(
    const NativeSceneData *scene_data,
    size_t &out_size) {
  
  // Validate input
  if (!scene_data)
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "NativeSceneData is null"));
  
  if (!scene_data->entity_memory_pool)
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "EntityMemoryPool pointer is null"));
  
  // Create FlatBuffers builder
  flatbuffers::FlatBufferBuilder builder(1024);
  
  // Build entity collection from runtime data
  auto entity_collection_offset = 
      BuildEntityCollection(builder, *scene_data->entity_memory_pool);
  
  // Create root table
  auto scene_data_offset = CreateSceneDataFbs(
      builder,
      /* scene_info fields */,
      entity_collection_offset);
  
  builder.Finish(scene_data_offset);
  
  // Copy to owned buffer
  out_size = builder.GetSize();
  auto buffer = std::make_unique<uint8_t[]>(out_size);
  std::memcpy(buffer.get(), builder.GetBufferPointer(), out_size);
  
  return buffer;
}

/////////////////////////////////////////////////
flatbuffers::Offset<EntityCollectionFbs>
FlatbuffersEntityExporter::BuildEntityCollection(
    flatbuffers::FlatBufferBuilder &builder,
    const EntityMemoryPool &emp) {
  
  size_t entity_count = entity::memory::GetMemoryPoolSize(emp);
  std::vector<flatbuffers::Offset<EntityDataFbs>> entity_offsets;
  
  // Build each entity
  for (size_t i = 0; i < entity_count; ++i) {
    auto entity_offset = BuildEntityData(builder, emp, i);
    entity_offsets.push_back(entity_offset);
  }
  
  // Create EntityCollectionFbs
  auto entities_vector = builder.CreateVector(entity_offsets);
  return CreateEntityCollectionFbs(builder, 
                                   static_cast<uint32_t>(entity_count),
                                   entities_vector);
}

/////////////////////////////////////////////////
flatbuffers::Offset<EntityDataFbs>
FlatbuffersEntityExporter::BuildEntityData(
    flatbuffers::FlatBufferBuilder &builder,
    const EntityMemoryPool &emp,
    size_t entity_id) {
  
  // Get components from runtime data
  const auto &ui_comp = 
      entity::memory::GetComponent<CUserInterface>(entity_id, emp);
  const auto &grimoire_comp = 
      entity::memory::GetComponent<CGrimoireMachina>(entity_id, emp);
  
  // Build FlatBuffers component data
  flatbuffers::Offset<UserInterfaceData> ui_offset;
  if (ui_comp.m_active) {
    // Convert runtime component to FlatBuffers format
    ui_offset = CreateUserInterfaceData(
        builder,
        builder.CreateString(ui_comp.m_ui_name),
        ui_comp.m_start_visible,
        /* ... other fields ... */);
  }
  
  // Build and return EntityDataFbs
  return CreateEntityDataFbs(
      builder,
      static_cast<uint32_t>(entity_id),
      ui_offset,
      /* ... other component offsets ... */);
}

} // namespace steamrot
```

### Usage Example: Save Game

```cpp
// Hypothetical save game implementation

std::expected<std::monostate, FailInfo>
SaveGame(const Scene &scene, const std::string &save_file_path) {
  
  // Create NativeSceneData with runtime data
  NativeSceneData native_scene_data;
  native_scene_data.scene_info = scene.GetSceneInfo();
  native_scene_data.scene_resources_config = scene.GetResourcesConfig();
  native_scene_data.scene_asset_config = scene.GetAssetConfig();
  native_scene_data.entity_memory_pool = &scene.GetEntityMemoryPool();
  
  // Export to binary
  FlatbuffersEntityExporter exporter;
  size_t binary_size;
  auto binary_result = exporter.ExportEntities(&native_scene_data, binary_size);
  
  if (!binary_result)
    return std::unexpected(binary_result.error());
  
  // Write to file
  std::ofstream file(save_file_path, std::ios::binary);
  file.write(reinterpret_cast<const char*>(binary_result.value().get()), 
             binary_size);
  
  if (!file)
    return std::unexpected(
        FailInfo(FailMode::FileWriteError, "Failed to write save file"));
  
  return std::monostate{};
}
```

## Proposed Solution 2: Separate Importer/Exporter Hierarchies (Better Architecture)

### Remove Heavy Data from SceneData

```cpp
// SceneData remains lightweight - no pointers to heavy structs
struct SceneData {
  virtual ~SceneData() = default;
  
  SceneInfo scene_info;
  SceneResourcesConfig scene_resources_config;
  AssetConfig scene_asset_config;
};

// Remove FbsSceneData - no longer needed
// Heavy data passed through importer instead
```

### Create Importer Interface

```cpp
// src/entity/IEntityImporter.h

#pragma once

#include "FailInfo.h"
#include "containers.h"
#include <expected>

namespace steamrot {

/////////////////////////////////////////////////
/// @class IEntityImporter
/// @brief Interface for importing entity data into EntityMemoryPool.
///
/// Implementations read from various sources and configure runtime entities:
/// - FlatbuffersEntityImporter: Reads from FlatBuffers data
/// - JsonEntityImporter: Reads from JSON (future)
/// - NetworkEntityImporter: Receives from network (future)
/////////////////////////////////////////////////
class IEntityImporter {
public:
  virtual ~IEntityImporter() = default;

  /////////////////////////////////////////////////
  /// @brief Import entities into the given EntityMemoryPool.
  ///
  /// @param emp EntityMemoryPool to populate with entity data
  /// @return monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ImportEntities(EntityMemoryPool &emp) = 0;
};

} // namespace steamrot
```

### Refactor FlatBuffers Importer

```cpp
// src/entity/FlatbuffersEntityImporter.h

#pragma once

#include "IEntityImporter.h"
#include "entities_generated.h"
#include "EventHandler.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @class FlatbuffersEntityImporter
/// @brief Imports entities from FlatBuffers EntityCollectionFbs data.
///
/// This replaces the heavy-data-in-SceneData pattern.
/// The EntityCollectionFbs pointer is stored in the importer,
/// not smuggled through SceneData inheritance.
/////////////////////////////////////////////////
class FlatbuffersEntityImporter : public IEntityImporter {
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
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ImportEntities(EntityMemoryPool &emp) override;

private:
  EventHandler &m_event_handler;
  const EntityCollectionFbs &m_entity_collection;
  
  // Internal configuration methods (same as before)
  std::expected<std::monostate, FailInfo>
  ConfigureFirstLayerComponents(EntityMemoryPool &emp);
  
  std::expected<std::monostate, FailInfo>
  ConfigureSecondLayerComponents(EntityMemoryPool &emp);
};

} // namespace steamrot
```

### Update Scene Configurator

```cpp
// src/scenes/FlatbuffersSceneConfigurator.cpp

std::expected<std::monostate, FailInfo>
FlatbuffersSceneConfigurator::ConfigureEntities(
    Scene &scene, const SceneData *scene_data) {
  
  if (!scene_data)
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "SceneData is null"));
  
  // Load FlatBuffers data separately (not through SceneData)
  auto fb_data = m_data_loader.LoadEntityCollection(scene_data->scene_info.type);
  if (!fb_data)
    return std::unexpected(fb_data.error());
  
  // NO DOWNCASTING NEEDED
  // Create importer with FlatBuffers data
  FlatbuffersEntityImporter importer(
      scene.GetSceneContext().event_handler,
      *fb_data.value());
  
  // Import entities
  auto import_result = importer.ImportEntities(
      scene.GetSceneContext().scene_entities);
  
  if (!import_result)
    return std::unexpected(import_result.error());
  
  return std::monostate{};
}
```

### Benefits Comparison

| Aspect | Current (FbsSceneData) | Solution 1 (NativeSceneData) | Solution 2 (Importer/Exporter) |
|--------|------------------------|------------------------------|--------------------------------|
| **Downcasting** | Required | Required | Not needed |
| **Const-casting** | Required | Required | Not needed |
| **Import/Export Symmetry** | No | Yes | Yes |
| **Separation of Concerns** | Poor | Poor | Excellent |
| **Extensibility** | Medium | Medium | High |
| **Code Clarity** | Low | Medium | High |
| **Refactoring Cost** | N/A | Low | High |

## Recommendation

**For immediate needs**: Implement Solution 1 (NativeSceneData)
- Minimal code changes
- Achieves symmetry for import/export
- Documents the pattern clearly

**For long-term architecture**: Plan migration to Solution 2 (Importer/Exporter)
- Cleaner separation of concerns
- No runtime casting required
- More maintainable and extensible

## Testing Considerations

### Test Export Functionality

```cpp
// tests/unit/entity/FlatbuffersEntityExporter.test.cpp

TEST_CASE("FlatbuffersEntityExporter exports EntityMemoryPool", 
          "[unit][FlatbuffersEntityExporter]") {
  
  // Create test EntityMemoryPool with known data
  EntityMemoryPool test_pool;
  entity::memory::ResizeEntityMemoryPool(test_pool, 2);
  
  // Configure test entities
  auto &ui_comp = entity::memory::GetComponent<CUserInterface>(0, test_pool);
  ui_comp.m_active = true;
  ui_comp.m_ui_name = "test_ui";
  
  // Create NativeSceneData
  NativeSceneData scene_data;
  scene_data.entity_memory_pool = &test_pool;
  
  // Export
  FlatbuffersEntityExporter exporter;
  size_t binary_size;
  auto result = exporter.ExportEntities(&scene_data, binary_size);
  
  REQUIRE(result.has_value());
  REQUIRE(binary_size > 0);
  
  // Verify binary can be read back
  const auto *root = GetSceneDataFbs(result.value().get());
  REQUIRE(root != nullptr);
  REQUIRE(root->entities() != nullptr);
  REQUIRE(root->entities()->size() == 2);
}
```

### Test Round-Trip (Import then Export)

```cpp
TEST_CASE("Entity data survives round-trip import/export", 
          "[integration][entity]") {
  
  // Load original FlatBuffers data
  FlatbuffersDataLoader loader;
  auto original_data = loader.LoadSceneData(SceneType::TITLE);
  REQUIRE(original_data.has_value());
  
  // Import to EntityMemoryPool
  FlatbuffersEntityImporter importer(/* ... */);
  EntityMemoryPool runtime_pool;
  auto import_result = importer.ImportEntities(runtime_pool);
  REQUIRE(import_result.has_value());
  
  // Export back to binary
  NativeSceneData scene_data;
  scene_data.entity_memory_pool = &runtime_pool;
  
  FlatbuffersEntityExporter exporter;
  size_t binary_size;
  auto export_result = exporter.ExportEntities(&scene_data, binary_size);
  REQUIRE(export_result.has_value());
  
  // Verify exported data matches original
  // (Implementation depends on comparison strategy)
}
```

---

**Document Version**: 1.0
**Date**: 2026-01-06
**Related**: SCENEDATA_ARCHITECTURE_ANALYSIS.md
