# Native Bridging Structures: Solution Summary

## Quick Answer to the Problem

**Your Question:** "How can I make IEntityImporter work bidirectionally so it can take a reference to the EMP and use it externally (for testing or serialization)?"

**Recommended Solution:** Don't modify IEntityImporter. Instead, use the **Dual-Mode Interface Pattern**:

1. Keep IEntityImporter for import (External → EMP)
2. Keep IEntityExporter for export (EMP → External)
3. Add new implementations that work with native structures
4. Add optional EntityMemoryPool storage to SceneData

## The Core Issue

Your current IEntityImporter is one-way:
```cpp
virtual std::expected<std::monostate, FailInfo>
ImportEntities(EntityMemoryPool &emp) = 0;
```

- Takes a mutable EMP reference
- Returns monostate (nothing)
- **Problem**: No way to get the EMP back out for external use

## Solution: Native Implementations

### 1. NativeEntityImporter (Native → EMP)

```cpp
class NativeEntityImporter : public IEntityImporter {
private:
  EventHandler &m_event_handler;
  const EntityMemoryPool &m_source_emp;  // Holds reference to source

public:
  NativeEntityImporter(EventHandler &event_handler,
                      const EntityMemoryPool &source_emp);

  std::expected<std::monostate, FailInfo>
  ImportEntities(EntityMemoryPool &target_emp) override {
    // Copy from m_source_emp to target_emp
  }
};
```

**Usage for testing:**
```cpp
// Create test data
EntityMemoryPool test_emp = CreateTestEntities();

// Import into engine
NativeEntityImporter importer(event_handler, test_emp);
importer.ImportEntities(scene.GetEntityMemoryPool());
```

### 2. NativeEntityExporter (EMP → Native)

```cpp
class NativeEntityExporter : public IEntityExporter {
public:
  // Existing interface (binary export)
  std::expected<std::unique_ptr<uint8_t[]>, FailInfo>
  ExportEntities(const EntityMemoryPool &emp, size_t &out_size) override;
  
  // NEW: Export to native structure
  std::expected<EntityMemoryPool, FailInfo>
  ExportToNativeStructure(const EntityMemoryPool &source_emp) {
    // Copy source_emp to new EMP
    return emp_copy;
  }
};
```

**Usage for serialization:**
```cpp
// Export engine state
NativeEntityExporter exporter;
auto emp_copy = exporter.ExportToNativeStructure(scene.GetEntityMemoryPool());

// Store in snapshot
snapshot.scene_collection_data[0].entity_data = emp_copy.value();
```

### 3. Enhanced SceneData

```cpp
struct SceneData {
  SceneInfo scene_info;
  SceneResourcesConfig scene_resources_config;
  AssetConfig scene_asset_config;
  
  // For import (format-agnostic)
  std::unique_ptr<IEntityImporter> entity_importer{nullptr};
  
  // NEW: For native workflows (testing, snapshots)
  std::optional<EntityMemoryPool> entity_data;
};
```

Now you can:
- **Import from FlatBuffers**: Use `entity_importer`
- **Import from native**: Set `entity_data`, create NativeEntityImporter
- **Export to native**: Use NativeEntityExporter, store in `entity_data`

## Complete Workflow Examples

### Testing: Create → Import → Run → Export → Validate

```cpp
// 1. Create test entities programmatically
EntityMemoryPool test_entities = CreateTestEntities();

// 2. Store in TestData
TestData test_data;
test_data.starting_engine_snapshot.scene_collection_data[0].entity_data = test_entities;

// 3. Import into engine
NativeEntityImporter importer(
    event_handler, 
    *test_data.starting_engine_snapshot.scene_collection_data[0].entity_data);
importer.ImportEntities(test_engine.GetScene().GetEntityMemoryPool());

// 4. Run test
test_engine.Run(10); // 10 ticks

// 5. Export result
NativeEntityExporter exporter;
auto result = exporter.ExportToNativeStructure(
    test_engine.GetScene().GetEntityMemoryPool());

// 6. Validate
REQUIRE(result.value() == test_data.expected_engine_snapshots[10].entity_data);
```

### Serialization: Engine → SaveData → File

```cpp
// 1. Export engine state
NativeEntityExporter exporter;
SaveData save_data;
save_data.meta_data = CreateMetaData();

// 2. Export each scene
for (auto& scene : engine.GetScenes()) {
  SceneData scene_data;
  auto emp = exporter.ExportToNativeStructure(scene.GetEntityMemoryPool());
  scene_data.entity_data = emp.value();
  save_data.engine_snapshot.scene_collection_data.push_back(scene_data);
}

// 3. Serialize to FlatBuffers
FlatbuffersSaveDataExporter fb_exporter;
auto binary = fb_exporter.ExportToFlatBuffers(save_data);

// 4. Write to file
WriteToFile("save.dat", binary);
```

## Why This Solution is Better

### ❌ Alternative: Modify IEntityImporter
```cpp
class IEntityImporter {
  virtual EntityMemoryPool& GetSourceData() = 0; // BAD
};
```

**Problems:**
- FlatbuffersEntityImporter doesn't have an EMP source
- Violates interface segregation (not all importers need this)
- Confusing semantics (why does an "Importer" have a getter?)

### ✅ Recommended: Separate Native Implementations
```cpp
class NativeEntityImporter : public IEntityImporter {
  const EntityMemoryPool &m_source_emp; // GOOD
};
```

**Benefits:**
- Clear separation: Import vs Export
- Works with existing FlatBuffers implementations
- Easy to test (just C++ structs)
- Format-agnostic (FlatBuffers, JSON, Binary, etc.)

## Key Insights

### 1. Two-Way Flow = Two Interfaces
- **Import** (External → Internal): IEntityImporter
- **Export** (Internal → External): IEntityExporter

### 2. Native Implementations Bridge the Gap
- **NativeEntityImporter**: Works with C++ structs
- **NativeEntityExporter**: Works with C++ structs
- **Format Importers/Exporters**: Work with FlatBuffers/JSON/etc.

### 3. SceneData Becomes Flexible
```cpp
// Option A: Import from FlatBuffers
scene_data.entity_importer = std::make_unique<FlatbuffersEntityImporter>(...);

// Option B: Import from native
scene_data.entity_data = CreateTestEntities();
auto importer = std::make_unique<NativeEntityImporter>(event_handler, *scene_data.entity_data);
```

### 4. Snapshots Work Naturally
```cpp
// Capture state
EngineSnapshot snapshot;
for (auto& scene : engine.GetScenes()) {
  NativeEntityExporter exporter;
  auto emp = exporter.ExportToNativeStructure(scene.GetEntityMemoryPool());
  snapshot.scene_collection_data.push_back({...});
  snapshot.scene_collection_data.back().entity_data = emp.value();
}

// Restore state
for (size_t i = 0; i < snapshot.scene_collection_data.size(); i++) {
  NativeEntityImporter importer(
      event_handler, 
      *snapshot.scene_collection_data[i].entity_data);
  importer.ImportEntities(engine.GetScene(i).GetEntityMemoryPool());
}
```

## Implementation Checklist

- [ ] Create `NativeEntityImporter` class (src/entity/)
- [ ] Create `NativeEntityExporter` class (src/entity/)
- [ ] Add `std::optional<EntityMemoryPool> entity_data` to SceneData
- [ ] Create helper utilities in namespace `steamrot::snapshot`
  - [ ] `CaptureEngineState()`
  - [ ] `RestoreEngineState()`
  - [ ] `CreateImporterFromSnapshot()`
- [ ] Write unit tests for NativeEntityImporter
- [ ] Write unit tests for NativeEntityExporter
- [ ] Write integration tests for snapshot workflows
- [ ] Update TestEngine to use native snapshots
- [ ] Update documentation and examples

## Next Steps

1. **Implement NativeEntityImporter** - Start with copying EMP component-by-component
2. **Implement NativeEntityExporter** - Mirror of importer
3. **Add entity_data to SceneData** - Simple struct change
4. **Write tests** - Verify round-trip (export → import → compare)
5. **Create utilities** - Helper functions for common operations

## Questions?

**Q: Do I need to modify IEntityImporter?**  
**A:** No. Keep it as-is. Add new implementations instead.

**Q: How do I access the EMP for testing?**  
**A:** Use NativeEntityExporter to copy it out, or store it in SceneData.entity_data.

**Q: Can I still use FlatBuffers?**  
**A:** Yes! FlatbuffersEntityImporter remains unchanged. Native implementations are additive.

**Q: Is this a breaking change?**  
**A:** No. All existing code continues to work. This adds new capabilities.

## Summary

The solution is to **add new native implementations** that work alongside existing format-specific ones:

- **IEntityImporter** stays focused on import
- **IEntityExporter** stays focused on export  
- **NativeEntityImporter/Exporter** work with C++ structs
- **SceneData.entity_data** provides optional native storage
- **Snapshot utilities** make it easy to capture/restore state

This gives you bidirectional flow without breaking the existing abstraction.

See `NATIVE_BRIDGING_STRUCTURES.md` for full architecture details.
