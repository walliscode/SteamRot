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
public:
  std::expected<std::unique_ptr<uint8_t[]>, FailInfo>
  ExportEntities(const EntityMemoryPool &emp, size_t &out_size) override;
  
  std::expected<EntityMemoryPool, FailInfo>
  ExportToNativeStructure(const EntityMemoryPool &source_emp);
};
```

## Updated SceneData
```cpp
struct SceneData {
  SceneInfo scene_info;
  SceneResourcesConfig scene_resources_config;
  AssetConfig scene_asset_config;
  
  // NEW: Variant for entity source (mutually exclusive)
  std::variant<
    std::monostate,                        // Empty/uninitialized
    std::unique_ptr<IEntityImporter>,      // Format-specific importer
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
NativeEntityExporter exporter;
auto snapshot = exporter.ExportToNativeStructure(scene.GetEntityMemoryPool());
save_data.engine_snapshot.scene_collection_data[0].entity_data = snapshot.value();
```

### 3. Round-Trip (Export → Import)
```cpp
// Export
NativeEntityExporter exporter;
auto emp_copy = exporter.ExportToNativeStructure(engine_emp);

// Store in variant
SceneData scene_data;
scene_data.entity_source = emp_copy.value();

// Import (check variant type first)
if (std::holds_alternative<EntityMemoryPool>(scene_data.entity_source)) {
  NativeEntityImporter importer(
      event_handler, 
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
2. NativeEntityExporter class  
3. Add entity_data to SceneData
4. Write tests
5. Create snapshot utilities
6. Update TestEngine integration

## See Also

- `NATIVE_BRIDGING_STRUCTURES.md` - Full architecture
- `SOLUTION_SUMMARY.md` - Detailed explanation
