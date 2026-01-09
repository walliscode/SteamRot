# Architecture Diagram: TestData Input vs EngineSnapshot Output

```
┌─────────────────────────────────────────────────────────────────────────┐
│                           TEST DATA STRUCTURE                            │
└─────────────────────────────────────────────────────────────────────────┘

┌───────────────────────────────────────────────────────────────┐
│  TestData                                                     │
│                                                               │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │ TEST METADATA (Meta information)                        │ │
│  │  • test_name: string                                    │ │
│  │  • test_description: string                             │ │
│  │  • will_pass: bool                                      │ │
│  └─────────────────────────────────────────────────────────┘ │
│                                                               │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │ INPUT: What to simulate                                 │ │
│  │ ─────────────────────────────────────────────────────── │ │
│  │                                                          │ │
│  │  SimulationData simulation_data                         │ │
│  │    └─> List of logic steps to execute                  │ │
│  │                                                          │ │
│  │  uint32_t number_of_ticks                               │ │
│  │    └─> How many ticks to run                           │ │
│  │                                                          │ │
│  │  SceneCollectionData starting_scene_collection_data ✨  │ │
│  │    └─> Initial scenes to load                          │ │
│  │        • vector<SceneLoadData>                          │ │
│  │        • Each contains:                                 │ │
│  │          - SceneData (info, config, assets)             │ │
│  │          - IEntityImporter (entity data wrapper)        │ │
│  └─────────────────────────────────────────────────────────┘ │
│                                                               │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │ OUTPUT: What to expect                                  │ │
│  │ ─────────────────────────────────────────────────────── │ │
│  │                                                          │ │
│  │  map<size_t, EngineSnapshot> expected_engine_snapshots  │ │
│  │    └─> Expected state at each tick                     │ │
│  │        • Key: tick number                               │ │
│  │        • Value: EngineSnapshot with expected data       │ │
│  └─────────────────────────────────────────────────────────┘ │
└───────────────────────────────────────────────────────────────┘


┌─────────────────────────────────────────────────────────────────────────┐
│                       ENGINE SNAPSHOT STRUCTURE                          │
└─────────────────────────────────────────────────────────────────────────┘

┌───────────────────────────────────────────────────────────────┐
│  EngineSnapshot                                               │
│                                                               │
│  PURPOSE: Capture engine state AFTER tick execution          │
│  USE CASE: Validation and comparison                         │
│                                                               │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │ optional<size_t> tick_number                            │ │
│  │   └─> Context: which tick was this captured            │ │
│  └─────────────────────────────────────────────────────────┘ │
│                                                               │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │ optional<map<uuid, EntityMemoryPool>> scene_snapshots   │ │
│  │   └─> Deep copy of each active scene's entity state    │ │
│  │       Key: Runtime-generated scene UUID                 │ │
│  │       Value: EntityMemoryPool (direct entity data)      │ │
│  └─────────────────────────────────────────────────────────┘ │
│                                                               │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │ optional<EventBus> global_event_bus                     │ │
│  │   └─> Snapshot of all events at this tick              │ │
│  └─────────────────────────────────────────────────────────┘ │
└───────────────────────────────────────────────────────────────┘


┌─────────────────────────────────────────────────────────────────────────┐
│                        WORKFLOW COMPARISON                               │
└─────────────────────────────────────────────────────────────────────────┘

INPUT (Loading Scenes):
═══════════════════════════════════════════════════════════════

  test_data.fbs (JSON)
        ↓
  TestDataFbs (binary)
        ↓
  FlatbuffersTestDataProvider::CreateTestData()
        ↓
  TestData.starting_scene_collection_data
        ↓
  TestEngine::StartUp()
        ↓
  SceneManager::AddScenesFromSceneCollectionData()
        ↓
  Scenes loaded with:
    • Scene configuration (type, texture size, etc.)
    • Entity data via IEntityImporter
    • Proper scene factory construction


OUTPUT (Capturing State):
═══════════════════════════════════════════════════════════════

  TestEngine::RunGameLoop()
        ↓
  ExecuteTick()  (Run simulation)
        ↓
  TestEngine::CaptureSnapShot(tick_number)
        ↓
  For each active scene:
    • Get EntityMemoryPool
    • Deep copy to EngineSnapshot
    • Store with runtime UUID as key
        ↓
  m_data_bank[tick] = EngineSnapshot


VALIDATION (Comparing Results):
═══════════════════════════════════════════════════════════════

  expected = test_data.expected_engine_snapshots[tick]
  actual   = test_engine.GetDataBank()[tick]
        ↓
  Compare EngineSnapshots:
    • Scene entity states
    • Event bus states
    • Other captured data
        ↓
  REQUIRE(actual == expected)


┌─────────────────────────────────────────────────────────────────────────┐
│                     KEY DESIGN DECISIONS                                 │
└─────────────────────────────────────────────────────────────────────────┘

✅ CORRECT APPROACH:
─────────────────────────────────────────────────────────────────
  INPUT                          OUTPUT
  ─────────                      ──────
  TestData                       EngineSnapshot
    ├─ starting_scene_...    →   Engine    →   expected_engine_...
    ├─ simulation_data                         (for validation)
    └─ number_of_ticks

  Clear separation of concerns:
    • SceneCollectionData for configuration
    • EngineSnapshot for validation


❌ PROBLEMATIC APPROACH:
─────────────────────────────────────────────────────────────────
  EngineSnapshot used for BOTH input and output

  Problems:
    • UUID key mismatch (runtime vs test-defined)
    • Data format mismatch (EntityMemoryPool vs SceneLoadData)
    • Semantic confusion (snapshot vs configuration)
    • Violates single responsibility


┌─────────────────────────────────────────────────────────────────────────┐
│                     IMPLEMENTATION SUMMARY                               │
└─────────────────────────────────────────────────────────────────────────┘

Changes Needed:                              Status:
────────────────────────────────────────────────────────────────

1. Update test_data.fbs                      📋 TODO
   └─ Add starting_scene_collection_data

2. Update FlatbuffersTestDataProvider        📋 TODO
   └─ Add ConfigureSceneCollectionData()

3. Update TestEngine::StartUp()              📋 TODO
   └─ Load scenes from starting_scene_...

4. Add integration test                      📋 TODO
   └─ Demonstrate complete workflow


No Changes Needed:                           Status:
────────────────────────────────────────────────────────────────

✓ SceneManager::AddScenesFromSceneCollectionData()  ✅ READY
✓ FlatbuffersSceneLoadDataProvider                  ✅ READY
✓ SceneFactory::CreateSceneFromSceneLoadData()      ✅ READY
✓ EngineSnapshot structure                          ✅ READY
✓ TestEngine::CaptureSnapShot()                     ✅ READY


Estimated Implementation Time: 1-2 hours
