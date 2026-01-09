# Quick Reference: SceneCollectionData Loading Analysis

**Status**: ✅ Analysis Complete  
**Date**: 2026-01-09  
**Main Document**: [SCENE_COLLECTION_DATA_LOADING_ANALYSIS.md](SCENE_COLLECTION_DATA_LOADING_ANALYSIS.md)  
**Diagrams**: [ARCHITECTURE_DIAGRAM.md](ARCHITECTURE_DIAGRAM.md)

---

## TL;DR - Executive Summary

### The Questions

1. **Can we load SceneCollectionData from TestData?**  
   → ✅ **YES** - Field exists, needs FlatBuffers support

2. **Is EngineSnapshot a good container for simulation input?**  
   → ❌ **NO** - It's for output capture only

3. **Should we use a different container?**  
   → ✅ **YES** - Use `TestData.starting_scene_collection_data`

4. **Is SceneManager ready?**  
   → ✅ **YES** - Fully implemented and tested

### The Recommendation

```cpp
struct TestData {
  // INPUT
  SceneCollectionData starting_scene_collection_data;  // ✨ Use this
  
  // OUTPUT
  map<size_t, EngineSnapshot> expected_engine_snapshots;
};
```

**Why?** Clear separation, matches SaveData pattern, minimal changes needed.

---

## What Needs to Change

### Required Changes (1-2 hours total)

**1. Update FlatBuffers Schema** ⚠️ High Priority
```fbs
// src/types/flatbuffers/testing/test_data.fbs
table TestDataFbs {
  meta_data: TestMetadataFbs (required);
  simulation_data: SimulationDataFbs;
  num_ticks: uint32;
  starting_scene_collection_data: SceneCollectionDataFbs;  // ADD THIS
}
```

**2. Update Provider** ⚠️ High Priority
```cpp
// tests/harness/FlatbuffersTestDataProvider.cpp
// Add ConfigureSceneCollectionData() method
```

**3. Update TestEngine** ⚡ Medium Priority
```cpp
// tests/harness/TestEngine.cpp::StartUp()
// Load scenes from starting_scene_collection_data
```

### No Changes Needed ✅

- SceneManager (already has `AddScenesFromSceneCollectionData()`)
- SceneFactory
- FlatbuffersSceneLoadDataProvider
- EngineSnapshot

---

## Example Usage

### JSON Test Data
```json
{
  "meta_data": {
    "test_name": "scene_state_transition"
  },
  "num_ticks": 5,
  "starting_scene_collection_data": {
    "scene_data": [
      {
        "scene_info": {"scene_type": "TITLE"},
        "scene_resources_config": {
          "texture_width": 800,
          "texture_height": 600
        },
        "entity_collection": {
          "entity_memory_pool_size": 30,
          "entities": [/* ... */]
        }
      }
    ]
  },
  "expected_engine_snapshots": {
    "5": {/* Expected state after 5 ticks */}
  }
}
```

### Test Code
```cpp
TEST_CASE("Scene transition", "[integration]") {
  const TestData &test_data = LoadTestData();
  TestEngine engine(test_data);
  
  engine.StartUp();  // Loads starting_scene_collection_data
  engine.Run();      // Executes simulation
  
  // Validate against expected_engine_snapshots
  REQUIRE(engine.GetDataBank()[5] == test_data.expected_engine_snapshots[5]);
}
```

---

## Design Rationale

### Why NOT EngineSnapshot for Input?

| Problem | Detail |
|---------|--------|
| **Semantic mismatch** | Built for capturing OUTPUT, not providing INPUT |
| **UUID mismatch** | Uses runtime UUIDs, not scene type enums |
| **Data format** | Has EntityMemoryPool, needs SceneLoadData |
| **Confusion** | Mixing input/output reduces clarity |

### Why TestData.starting_scene_collection_data?

| Benefit | Detail |
|---------|--------|
| **Already exists** | Field is in TestData struct |
| **Correct type** | Already `SceneCollectionData` |
| **Clear semantics** | `starting_` = input, `expected_` = output |
| **Consistency** | Matches SaveData pattern |
| **Minimal change** | Just needs FlatBuffers support |

---

## Workflow Overview

```
Input Pipeline:
  JSON → FlatBuffers → Provider → TestData → TestEngine → SceneManager
  
Output Pipeline:
  TestEngine → CaptureSnapShot → EngineSnapshot → Validation
```

---

## For More Details

- **Full Analysis**: [SCENE_COLLECTION_DATA_LOADING_ANALYSIS.md](SCENE_COLLECTION_DATA_LOADING_ANALYSIS.md) (599 lines)
- **Visual Diagrams**: [ARCHITECTURE_DIAGRAM.md](ARCHITECTURE_DIAGRAM.md) (192 lines)
- **Index**: [README.md](README.md)

---

## Status: Ready for Implementation

All questions answered. Clear implementation path identified. Estimated effort: **1-2 hours**.

The architecture is solid and ready. Only minor FlatBuffers updates needed to enable the feature.
