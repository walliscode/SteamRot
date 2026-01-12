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
private:
  EventHandler &m_event_handler;

public:
  NativeEntityExporter(EventHandler &event_handler);

  // Existing interface (binary export - can delegate or leave unimplemented)
  std::expected<std::unique_ptr<uint8_t[]>, FailInfo>
  ExportEntities(const EntityMemoryPool &emp, size_t &out_size) override;
  
  // NEW: Export by configuring a target EMP (avoids type erasure)
  std::expected<std::monostate, FailInfo>
  ExportToEntityMemoryPool(const EntityMemoryPool &source_emp, 
                           EntityMemoryPool &target_emp);
};
```

**Design Rationale:**
- Uses configurator pattern (takes reference to populate) instead of returning data
- Avoids type erasure - concrete implementation knows exact types
- Consistent with existing configurator patterns in the codebase
- Target EMP is passed by reference, source EMP is const reference

**Usage for serialization:**
```cpp
// Export engine state
NativeEntityExporter exporter(event_handler);
EntityMemoryPool snapshot_emp;  // Create target to populate

auto result = exporter.ExportToEntityMemoryPool(
    scene.GetEntityMemoryPool(),  // source
    snapshot_emp);                // target to populate

// Store in snapshot
if (result.has_value()) {
  snapshot.scene_collection_data[0].entity_source = std::move(snapshot_emp);
}
```

### 3. Enhanced SceneData

```cpp
struct SceneData {
  SceneInfo scene_info;
  SceneResourcesConfig scene_resources_config;
  AssetConfig scene_asset_config;
  
  // NEW: Variant for entity source/destination (mutually exclusive)
  std::variant<
    std::monostate,                        // Empty/uninitialized
    std::unique_ptr<IEntityImporter>,      // Format-specific importer
    std::unique_ptr<IEntityExporter>,      // Format-specific exporter
    EntityMemoryPool                       // Native data (import or export)
  > entity_source;
};
```

**Why include exporter in variant?**
- **Import OR Export**: A SceneData represents either a source (import) or destination (export)
- **Symmetry**: Both importers and exporters can be stored
- **Flexibility**: Can hold format-specific exporter for serialization workflows
- **Type safety**: Compiler enforces one operation at a time

**Usage patterns:**
```cpp
// Option A: Import from format-specific source
scene_data.entity_source = std::make_unique<FlatbuffersEntityImporter>(...);

// Option B: Export to format-specific destination
scene_data.entity_source = std::make_unique<FlatbuffersEntityExporter>(...);

// Option C: Use native data directly (for both import and export)
scene_data.entity_source = CreateTestEntities();

// Check which type is active
if (std::holds_alternative<EntityMemoryPool>(scene_data.entity_source)) {
  auto& emp = std::get<EntityMemoryPool>(scene_data.entity_source);
  // Use for import or export
} else if (auto* importer = std::get_if<std::unique_ptr<IEntityImporter>>(&scene_data.entity_source)) {
  // Use importer
} else if (auto* exporter = std::get_if<std::unique_ptr<IEntityExporter>>(&scene_data.entity_source)) {
  // Use exporter
}
```

**Why a variant?**
- **Mutual exclusivity**: A SceneData can only represent one operation at a time
- **Type safety**: Compiler enforces correct usage
- **Clear intent**: Explicit whether importing, exporting, or using native data
- **Memory efficiency**: Only stores what's actually being used
- **Symmetry**: Both import and export operations supported

Now you can:
- **Import from FlatBuffers**: Set `entity_source` to a FlatbuffersEntityImporter
- **Export to FlatBuffers**: Set `entity_source` to a FlatbuffersEntityExporter
- **Use native data**: Set `entity_source` to EntityMemoryPool (for both import/export)

The variant ensures mutual exclusivity - a SceneData performs one operation at a time.

## Complete Workflow Examples

### Testing: Create → Import → Run → Export → Validate

```cpp
// 1. Create test entities programmatically
EntityMemoryPool test_entities = CreateTestEntities();

// 2. Store in TestData using variant
TestData test_data;
test_data.starting_engine_snapshot.scene_collection_data[0].entity_source = 
    std::move(test_entities);

// 3. Import into engine (check variant and create importer if needed)
auto& entity_source = test_data.starting_engine_snapshot.scene_collection_data[0].entity_source;
if (std::holds_alternative<EntityMemoryPool>(entity_source)) {
  NativeEntityImporter importer(
      event_handler, 
      std::get<EntityMemoryPool>(entity_source));
  importer.ImportEntities(test_engine.GetScene().GetEntityMemoryPool());
}

// 4. Run test
test_engine.Run(10); // 10 ticks

// 5. Export result using configurator pattern
NativeEntityExporter exporter(event_handler);
EntityMemoryPool result_emp;
auto export_result = exporter.ExportToEntityMemoryPool(
    test_engine.GetScene().GetEntityMemoryPool(),
    result_emp);

// 6. Validate
REQUIRE(export_result.has_value());
REQUIRE(result_emp == test_data.expected_engine_snapshots[10].entity_source);
```

### Serialization: Engine → SaveData → File

```cpp
// 1. Export engine state
NativeEntityExporter exporter;
SaveData save_data;
save_data.meta_data = CreateMetaData();

// 2. Export each scene using configurator pattern
for (auto& scene : engine.GetScenes()) {
  SceneData scene_data;
  EntityMemoryPool snapshot_emp;  // Create target to populate
  
  NativeEntityExporter exporter(event_handler);
  auto result = exporter.ExportToEntityMemoryPool(
      scene.GetEntityMemoryPool(),  // source
      snapshot_emp);                // target
  
  if (result.has_value()) {
    scene_data.entity_source = std::move(snapshot_emp);
    save_data.engine_snapshot.scene_collection_data.push_back(scene_data);
  }
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
scene_data.entity_source = std::make_unique<FlatbuffersEntityImporter>(...);

// Option B: Export to FlatBuffers
scene_data.entity_source = std::make_unique<FlatbuffersEntityExporter>(...);

// Option C: Use native data
scene_data.entity_source = CreateTestEntities();
```

### 4. Snapshots Work Naturally
```cpp
// Capture state
EngineSnapshot snapshot;
for (auto& scene : engine.GetScenes()) {
  NativeEntityExporter exporter(event_handler);
  EntityMemoryPool emp;
  auto result = exporter.ExportToEntityMemoryPool(scene.GetEntityMemoryPool(), emp);
  if (result.has_value()) {
    SceneData scene_data;
    scene_data.entity_source = std::move(emp);
    snapshot.scene_collection_data.push_back(scene_data);
  }
}

// Restore state
for (size_t i = 0; i < snapshot.scene_collection_data.size(); i++) {
  auto& entity_source = snapshot.scene_collection_data[i].entity_source;
  if (std::holds_alternative<EntityMemoryPool>(entity_source)) {
    NativeEntityImporter importer(
        event_handler, 
        std::get<EntityMemoryPool>(entity_source));
    importer.ImportEntities(engine.GetScene(i).GetEntityMemoryPool());
  }
}
```

## Implementation Checklist

- [ ] Create `NativeEntityImporter` class (src/entity/)
- [ ] Create `NativeEntityExporter` class (src/entity/) with configurator pattern
- [ ] Update SceneData with `entity_source` variant (including exporter option)
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

1. **Implement NativeEntityImporter** - Copy EMP component-by-component
2. **Implement NativeEntityExporter** - Use configurator pattern (takes reference)
3. **Update SceneData** - Add variant with importer/exporter/native options
4. **Write tests** - Verify round-trip (export → import → compare)
5. **Create utilities** - Helper functions for common operations

## Questions?

**Q: Do I need to modify IEntityImporter?**  
**A:** No. Keep it as-is. Add new implementations instead.

**Q: How do I access the EMP for testing?**  
**A:** Use NativeEntityExporter to populate a target EMP, or store native data in SceneData.entity_source variant.

**Q: Can I still use FlatBuffers?**  
**A:** Yes! FlatbuffersEntityImporter remains unchanged. Native implementations are additive.

**Q: Is this a breaking change?**  
**A:** No. All existing code continues to work. This adds new capabilities.

**Q: Why use configurator pattern for exporter?**  
**A:** Avoids type erasure. Concrete implementation knows exact types it's working with.

## Summary

The solution is to **add new native implementations** that work alongside existing format-specific ones:

- **IEntityImporter** stays focused on import
- **IEntityExporter** stays focused on export  
- **NativeEntityImporter/Exporter** work with C++ structs using configurator pattern
- **SceneData.entity_source** variant holds importer, exporter, or native data
- **Snapshot utilities** make it easy to capture/restore state

This gives you bidirectional flow without breaking the existing abstraction.

See `NATIVE_BRIDGING_STRUCTURES.md` for full architecture details.
