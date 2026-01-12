# Native Bridging Structures: Quick Reference

## TL;DR

**Problem**: Need bidirectional flow between EntityMemoryPool and native structs (SaveData/EngineSnapshot/TestData).

**Solution**: Add NativeEntityImporter and NativeEntityExporter that work with C++ structs.

## Three-Tier Data Flow

```
┌─────────────────────────────────┐
│   External Formats              │
│   (FlatBuffers, JSON, Binary)   │
└──────────┬──────────────────────┘
           │ Format Importers/Exporters
           ↓
┌─────────────────────────────────┐
│   Native Bridging Structures    │
│   (SaveData, EngineSnapshot)    │
│   Contains: EntityMemoryPool    │
└──────────┬──────────────────────┘
           │ Native Importers/Exporters
           ↓
┌─────────────────────────────────┐
│   Runtime Engine                │
│   (Active EntityMemoryPool)     │
└─────────────────────────────────┘
```

## Classes to Implement

### NativeEntityImporter
```cpp
class NativeEntityImporter : public IEntityImporter {
private:
  EventHandler &m_event_handler;
  const EntityMemoryPool &m_source_emp;
public:
  NativeEntityImporter(EventHandler &event_handler,
                      const EntityMemoryPool &source_emp);
  std::expected<std::monostate, FailInfo>
  ImportEntities(EntityMemoryPool &target_emp) override;
};
```

### NativeEntityExporter
```cpp
class NativeEntityExporter : public IEntityExporter {
private:
  EventHandler &m_event_handler;
public:
  NativeEntityExporter(EventHandler &event_handler);
  
  std::expected<std::unique_ptr<uint8_t[]>, FailInfo>
  ExportEntities(const EntityMemoryPool &emp, size_t &out_size) override;
  
  std::expected<std::monostate, FailInfo>
  ExportToEntityMemoryPool(const EntityMemoryPool &source_emp,
                           EntityMemoryPool &target_emp);
};
```

## Updated SceneData
```cpp
struct SceneData {
  SceneInfo scene_info;
  SceneResourcesConfig scene_resources_config;
  AssetConfig scene_asset_config;
  
  // NEW: Variant for entity operation (mutually exclusive)
  std::variant<
    std::monostate,                        // Empty/uninitialized
    std::unique_ptr<IEntityImporter>,      // Format-specific importer
    std::unique_ptr<IEntityExporter>,      // Format-specific exporter
    EntityMemoryPool                       // Native data
  > entity_source;
};
```

## Common Use Cases

### 1. Testing (Native → Engine)
```cpp
EntityMemoryPool test_emp = CreateTestEntities();
NativeEntityImporter importer(event_handler, test_emp);
importer.ImportEntities(scene.GetEntityMemoryPool());
```

### 2. Snapshotting (Engine → Native)
```cpp
NativeEntityExporter exporter(event_handler);
EntityMemoryPool snapshot;
auto result = exporter.ExportToEntityMemoryPool(scene.GetEntityMemoryPool(), snapshot);
if (result.has_value()) {
  save_data.engine_snapshot.scene_collection_data[0].entity_source = std::move(snapshot);
}
```

### 3. Round-Trip (Export → Import)
```cpp
// Export
NativeEntityExporter exporter(event_handler);
EntityMemoryPool emp_copy;
exporter.ExportToEntityMemoryPool(engine_emp, emp_copy);

// Store in variant
SceneData scene_data;
scene_data.entity_source = std::move(emp_copy);

// Import (check variant type first)
if (std::holds_alternative<EntityMemoryPool>(scene_data.entity_source)) {
  NativeEntityImporter importer(
      event_handler, 
      std::get<EntityMemoryPool>(scene_data.entity_source));
  importer.ImportEntities(test_emp);
}
``` 
      std::get<EntityMemoryPool>(scene_data.entity_source));
  importer.ImportEntities(test_emp);
}
```

## Snapshot Utilities (To Implement)

```cpp
namespace steamrot::snapshot {

EngineSnapshot CaptureEngineState(const Engine& engine, size_t tick);

std::expected<std::monostate, FailInfo>
RestoreEngineState(Engine& engine, const EngineSnapshot& snapshot);

std::unique_ptr<IEntityImporter>
CreateImporterFromSnapshot(EventHandler& handler, const SceneData& data);

}
```

## File Locations

- **Interfaces**: `src/types/interfaces/IEntityImporter.h`, `IEntityExporter.h`
- **Native Implementations**: `src/entity/NativeEntityImporter.h`, `NativeEntityExporter.h`
- **Snapshot Structures**: `src/types/core/EngineSnapshot.h`, `SaveData.h`
- **Tests**: `tests/unit/entity/NativeEntityImporter.test.cpp`, etc.

## Benefits

✅ Format-independent testing  
✅ Easy state capture/restore  
✅ No FlatBuffers schema changes needed  
✅ Clean separation of concerns  
✅ Non-breaking change (additive only)

## Implementation Order

1. NativeEntityImporter class
2. NativeEntityExporter class (configurator pattern)
3. Update SceneData with entity_source variant
4. Write tests
5. Create snapshot utilities
6. Update TestEngine integration

## See Also

- `NATIVE_BRIDGING_STRUCTURES.md` - Full architecture
- `SOLUTION_SUMMARY.md` - Detailed explanation
