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

## Solution Flow (Option 1: Convert in Harness Runner)

```
┌─────────────────────────────────────────────────────────────────┐
│                 IMPROVED TEST DATA FLOW                          │
│                 (With Post-Engine Conversion)                    │
└─────────────────────────────────────────────────────────────────┘

test_data.json
     │
     ├─ FlatBuffers Compilation
     ↓
test_data.bin (TestDataFbs)
     │
     ├─ FlatbuffersTestDataLoader
     ↓
TestData {
  expected_engine_snapshots: map<tick, EngineSnapshot>
    └─ SceneData {
         entity_transport: const EntityCollectionFbs*  ◄─── STAYS AS FBS
       }
}
     │
     ├─ TestEngine::StartUp()
     ├─ TestEngine::RunGame()  ← Engine runs normally
     ↓
TestEngine creates snapshots with EntityMemoryPool
     │
     ├─ ✨ NEW: In harness_runner.cpp AFTER engine runs
     │   Convert test_data.expected_engine_snapshots
     ↓
for each expected_snapshot:
  for each scene_data:
    if holds EntityCollectionFbs*:
      import to EntityMemoryPool ◄─── CONVERSION HERE
      
entity_transport = EntityMemoryPool  ✅
                   ^^^^^^^^^^^^^^^
                   Now matches actual!


┌─────────────────────────────────────────────────────────────────┐
│                    COMPARISON (NOW WORKS!)                       │
└─────────────────────────────────────────────────────────────────┘

EntityTransportEqualsMatcher::match()
     │
     ├─ Compare variant indices
     ↓
if (actual.index() != expected.index()) {
   ✅ MATCH!
   actual.index() = 1 (EntityMemoryPool)  ← From engine
   expected.index() = 1 (EntityMemoryPool) ← Converted in harness
   
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
File: tests/harness/harness_runner.cpp
After line 73 (after test_engine.RunGame())

BEFORE:
────────────────────────────────────────────────────────
// run the game
auto run_result = test_engine.RunGame();
if (!run_result) {
  return std::unexpected(run_result.error());
}


AFTER (with fix):
────────────────────────────────────────────────────────
// run the game
auto run_result = test_engine.RunGame();
if (!run_result) {
  return std::unexpected(run_result.error());
}

// ✨ NEW: Convert expected snapshots to EntityMemoryPool format
// Access TestEngine's EventHandler for the conversion
auto& engine_event_handler = test_engine.GetEngineResources().event_handler;

for (auto &[tick, expected_snapshot] : test_data.expected_engine_snapshots) {
  for (auto &scene_data : expected_snapshot.scene_collection_data) {
    if (std::holds_alternative<const EntityCollectionFbs*>(
            scene_data.entity_transport)) {
      const auto* fbs_ptr = 
          std::get<const EntityCollectionFbs*>(scene_data.entity_transport);
      
      // Create configurator with TestEngine's EventHandler
      FlatbuffersEntityConfigurator configurator(engine_event_handler);
      
      // Create and configure EntityMemoryPool
      EntityMemoryPool pool(scene_data.scene_info.entity_memory_pool_size);
      auto configure_result = configurator.ConfigureEntityMemoryPoolFromSource(
          pool, scene_data.entity_transport);
      if (!configure_result.has_value()) {
        return std::unexpected(configure_result.error());
      }
      
      scene_data.entity_transport = std::move(pool);
    }
  }
}

// Also convert starting_engine_snapshot
for (auto &scene_data : test_data.starting_engine_snapshot.scene_collection_data) {
  if (std::holds_alternative<const EntityCollectionFbs*>(
          scene_data.entity_transport)) {
    const auto* fbs_ptr = 
        std::get<const EntityCollectionFbs*>(scene_data.entity_transport);
    
    FlatbuffersEntityConfigurator configurator(engine_event_handler);
    
    EntityMemoryPool pool(scene_data.scene_info.entity_memory_pool_size);
    auto configure_result = configurator.ConfigureEntityMemoryPoolFromSource(
        pool, scene_data.entity_transport);
    if (!configure_result.has_value()) {
      return std::unexpected(configure_result.error());
    }
    
    scene_data.entity_transport = std::move(pool);
  }
}
```

---

## Benefits of Solution

```
┌─────────────────────────────────────────────────────────────┐
│                   NON-INVASIVE CONVERSION                    │
└─────────────────────────────────────────────────────────────┘

Test Data Load → EntityCollectionFbs* (original format)
                          ↓
Engine Runs → Creates EntityMemoryPool (normal behavior)
                          ↓
Harness Runner → Converts expected data to EntityMemoryPool
                          ↓
Comparison → Both in EntityMemoryPool format ✅

• Doesn't interfere with EngineSnapshot generation
• Isolated to test comparison step
• Preserves original snapshot behavior
• No changes to data providers or matchers
• Clean separation of concerns
```

---

## Timeline

```
Phase 1: Implementation (1.5 hours)
│
├─ Add conversion code in harness_runner.cpp (45 min)
├─ Handle error cases (30 min)
└─ Add debug logging if needed (15 min)

Phase 2: Testing (1 hour)
│
├─ Run existing tests (30 min)
└─ Verify conversions (30 min)

Phase 3: Validation (0.5 hours)
│
└─ Edge cases and verification (30 min)

TOTAL: 3 hours ⏱️
```
