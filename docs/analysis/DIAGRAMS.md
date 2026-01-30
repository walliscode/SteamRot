# Entity Transport Type Mismatch - Visual Diagrams

## Current Problem Flow

```
┌─────────────────────────────────────────────────────────────────┐
│                    TEST DATA LOADING PATH                        │
│                    (Expected State)                              │
└─────────────────────────────────────────────────────────────────┘

test_data.json
     │
     ├─ FlatBuffers Compilation
     ↓
test_data.bin (TestDataFbs)
     │
     ├─ FlatbuffersTestDataLoader
     ↓
TestDataFbs* 
     │
     ├─ FlatbuffersTestDataProvider::CreateTestData
     ↓
TestData {
  starting_engine_snapshot: EngineSnapshot
  expected_engine_snapshots: map<tick, EngineSnapshot>
}
     │
     ├─ ConfigureEngineSnapshot
     ↓
EngineSnapshot {
  scene_collection_data: SceneCollectionData
}
     │
     ├─ FlatbuffersSceneDataProvider::ConfigureSceneData
     ↓
SceneData {
  entity_transport: EntityTransportVariant
}
     │
     └─ [Line 94: Direct Assignment]
     
entity_transport = const EntityCollectionFbs*  ◄─── VARIANT INDEX 3
                   ^^^^^^^^^^^^^^^^^^^^^^^^
                   FlatBuffers Pointer
                   (Stays as pointer)


┌─────────────────────────────────────────────────────────────────┐
│                    TESTENGINE RUNTIME PATH                       │
│                    (Actual State)                                │
└─────────────────────────────────────────────────────────────────┘

TestEngine::RunGameLoop()
     │
     ├─ Each tick, run scene logic
     ↓
Scene modifies entities (EntityMemoryPool)
     │
     ├─ TestEngine::StoreEngineSnapShot()
     ↓
EngineSnapshot {
  scene_collection_data: SceneCollectionData
}
     │
     └─ Copy from active scene
     
entity_transport = EntityMemoryPool  ◄───────── VARIANT INDEX 1
                   ^^^^^^^^^^^^^^^^
                   C++ Container
                   (Active entities)


┌─────────────────────────────────────────────────────────────────┐
│                    COMPARISON (FAILS!)                           │
└─────────────────────────────────────────────────────────────────┘

EntityTransportEqualsMatcher::match()
     │
     ├─ Compare variant indices
     ↓
if (actual.index() != expected.index()) {
   ❌ MISMATCH!
   actual.index() = 1 (EntityMemoryPool)
   expected.index() = 3 (EntityCollectionFbs*)
   
   return false;  // Cannot compare different types
}
```

---

## Solution Flow (Option 1: Convert at Load)

```
┌─────────────────────────────────────────────────────────────────┐
│                 IMPROVED TEST DATA LOADING PATH                  │
│                 (With Conversion)                                │
└─────────────────────────────────────────────────────────────────┘

test_data.json
     │
     ├─ FlatBuffers Compilation
     ↓
test_data.bin (TestDataFbs)
     │
     ├─ FlatbuffersTestDataLoader
     ↓
TestDataFbs*
     │
     ├─ ConfigureEngineSnapshot
     ↓
SceneData {
  entity_transport: const EntityCollectionFbs*  ◄─── STARTS HERE
  entity_configurator: unique_ptr
}
     │
     ├─ ✨ NEW: Import & Convert
     │   entity_configurator->ImportEntities(fbs_ptr)
     ↓
EntityMemoryPool imported_pool;  ◄─── CONVERSION HAPPENS
     │
     ├─ Replace in variant
     ↓
entity_transport = std::move(imported_pool);
                   ^^^^^^^^^^^^^^^^^^^^^^^
                   Now it's EntityMemoryPool! ✅


┌─────────────────────────────────────────────────────────────────┐
│                    COMPARISON (NOW WORKS!)                       │
└─────────────────────────────────────────────────────────────────┘

EntityTransportEqualsMatcher::match()
     │
     ├─ Compare variant indices
     ↓
if (actual.index() != expected.index()) {
   ✅ MATCH!
   actual.index() = 1 (EntityMemoryPool)
   expected.index() = 1 (EntityMemoryPool)  ← SAME NOW!
   
   // Proceed to deep comparison
}
     │
     ├─ Both are EntityMemoryPool
     ↓
EntityMemoryPoolEqualsMatcher::match()
     │
     ├─ Compare pool sizes
     ├─ Compare component vectors
     ├─ Compare component data
     ↓
✅ Full entity state comparison works!
```

---

## EntityTransportVariant Structure

```
┌──────────────────────────────────────────────────────────────┐
│  std::variant<                                                │
│    std::monostate,                   // Index 0: Empty       │
│    EntityMemoryPool,                 // Index 1: Value ✅     │
│    std::shared_ptr<EntityMemoryPool>,// Index 2: Shared ptr  │
│    const EntityCollectionFbs *       // Index 3: FlatBuffers │
│  >                                                            │
└──────────────────────────────────────────────────────────────┘

Current State:
┌─────────────┐                    ┌─────────────┐
│  Expected   │                    │   Actual    │
│  (Test Data)│                    │ (Runtime)   │
├─────────────┤                    ├─────────────┤
│ Index: 3    │  ❌ MISMATCH ❌   │ Index: 1    │
│ Type: Fbs*  │                    │ Type: Pool  │
└─────────────┘                    └─────────────┘

After Fix:
┌─────────────┐                    ┌─────────────┐
│  Expected   │                    │   Actual    │
│  (Test Data)│                    │ (Runtime)   │
├─────────────┤                    ├─────────────┤
│ Index: 1    │  ✅ MATCH ✅      │ Index: 1    │
│ Type: Pool  │                    │ Type: Pool  │
└─────────────┘                    └─────────────┘
```

---

## Code Change Location

```cpp
File: src/data_providers/configure/configure_engine_snapshot.cpp
Around line 93-98

BEFORE:
────────────────────────────────────────────────────────
SceneData scene_data;
auto configure_result = scene_provider.ConfigureSceneData(scene_data);
if (!configure_result.has_value()) {
  return std::unexpected(configure_result.error());
}

// Add to collection
snapshot.scene_collection_data.push_back(std::move(scene_data));


AFTER (with fix):
────────────────────────────────────────────────────────
SceneData scene_data;
auto configure_result = scene_provider.ConfigureSceneData(scene_data);
if (!configure_result.has_value()) {
  return std::unexpected(configure_result.error());
}

// ✨ NEW: Convert FlatBuffers to EntityMemoryPool
if (std::holds_alternative<const EntityCollectionFbs*>(
        scene_data.entity_transport)) {
  const auto* fbs_ptr = 
      std::get<const EntityCollectionFbs*>(scene_data.entity_transport);
  
  auto pool_result = scene_data.entity_configurator->ImportEntities(fbs_ptr);
  if (!pool_result.has_value()) {
    return std::unexpected(pool_result.error());
  }
  
  scene_data.entity_transport = std::move(pool_result.value());
}

// Add to collection (now with EntityMemoryPool)
snapshot.scene_collection_data.push_back(std::move(scene_data));
```

---

## Benefits of Solution

```
┌─────────────────────────────────────────────────────────────┐
│                   UNIFIED DATA FORMAT                        │
└─────────────────────────────────────────────────────────────┘

Test Data Load → EntityMemoryPool ──┐
                                     ├─→ Matcher Comparison ✅
Engine Runtime  → EntityMemoryPool ──┘

• Simple comparison logic
• Consistent data representation
• Validates entity import early
• No matcher changes needed
• Leverages existing infrastructure
```

---

## Timeline

```
Phase 1: Implementation (2 hours)
│
├─ Add conversion code (30 min)
├─ Handle error cases (30 min)
├─ Add debug logging (30 min)
└─ Code review (30 min)

Phase 2: Testing (1 hour)
│
├─ Run existing tests (30 min)
└─ Verify conversions (30 min)

Phase 3: Validation (1 hour)
│
├─ Multi-scene tests (30 min)
└─ Edge cases (30 min)

TOTAL: 4 hours ⏱️
```
