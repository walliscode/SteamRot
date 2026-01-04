# SaveData and TestData Integration Guide

## Overview

This document explains how to align TestData with SaveData so that loading engine state for simulation closely mimics save/load functionality. This alignment provides several benefits:

1. **Unified Serialization**: Both systems use similar data structures (EntityCollectionFbs)
2. **Test Realism**: Integration tests can use save-like data structures
3. **Reduced Duplication**: Shared FlatBuffers schemas between SaveData and TestData
4. **Migration Path**: Test data can be converted to/from save data for debugging

## Current State

### TestData Schema (Current)
```fbs
// src/types/flatbuffers/testing/test_data.fbs
table TestDataFbs {
  meta_data: TestMetadataFbs (required);
  simulation_data: SimulationDataFbs;
  num_ticks: uint32;
}
```

### SaveData Schema (Proposed)
```fbs
// src/types/flatbuffers/configuration/save_data.fbs
table SaveData {
  metadata: SaveMetadata (required);
  game_progression: GameProgressionData (required);
  saved_scenes: [SavedSceneData] (required);
  hybrid_resources: HybridResourcesData;
}

table SavedSceneData {
  scene_type: SceneType (required);
  entity_collection: EntityCollectionFbs (required);
  scene_flags: [string];
  scene_data: [KeyValuePair];
}
```

## Integration Strategy

### 1. Add Entity Collections to TestData

Update `test_data.fbs` to include entity collections that mirror SaveData structure:

```fbs
include "../entities/entities.fbs";
include "../core/scene_types.fbs";
include "simulation_data.fbs";
include "input_test_data.fbs";
include "../events/event_packet_data.fbs";
include "../events/event_bus_data.fbs";
include "../engine/engine_state.fbs";

namespace steamrot;

////////////////////////////////////////////////////////////
/// @brief Test metadata for data-driven testing
////////////////////////////////////////////////////////////
table TestMetadataFbs {
  test_name: string (required);
  test_description: string;
  tags: [string];
  will_pass: bool = true;
  version: uint32;
}

////////////////////////////////////////////////////////////
/// @brief Scene state for testing
/// 
/// This mirrors SavedSceneData structure to allow tests
/// to initialize engine state similarly to save loading.
////////////////////////////////////////////////////////////
table TestSceneData {
  /// @brief Scene type to test
  scene_type: SceneType (required);
  
  /// @brief Initial entity collection for the scene
  entity_collection: EntityCollectionFbs;
  
  /// @brief Scene-specific test flags
  scene_flags: [string];
}

////////////////////////////////////////////////////////////
/// @brief Root table for test data configuration
////////////////////////////////////////////////////////////
table TestDataFbs {
  /// @brief Metadata about this test case
  meta_data: TestMetadataFbs (required);
  
  /// @brief Initial scene state (mirrors SaveData structure)
  /// Similar to SaveData::saved_scenes but for testing
  initial_scene_state: TestSceneData;
  
  /// @brief Expected scene state after test execution
  /// Similar to final SaveData state after gameplay
  expected_scene_state: TestSceneData;
  
  /// @brief Simulation data for executing logic steps during tests
  simulation_data: SimulationDataFbs;
  
  /// @brief Number of ticks to run the test for
  num_ticks: uint32;
}

root_type TestDataFbs;
```

### 2. Unified Entity Collection Structure

Both SaveData and TestData use the same `EntityCollectionFbs`:

```fbs
// src/types/flatbuffers/entities/entities.fbs
table EntityDataFbs {
  index: uint32;
  c_user_interface: UserInterfaceFbs;
  c_grimoire_machina: GrimoireMachinaData;
  c_ui_state: UIStateCollectionData;
}

table EntityCollectionFbs {
  entities: [EntityDataFbs] (required);
  entity_memory_pool_size: int;
}
```

**Key Point**: This is the SAME structure used in both systems, ensuring consistency.

### 3. Example Test Data Mimicking SaveData

```json
// tests/integration/scene_simulation/crafting_scene.test_data.json
{
  "meta_data": {
    "test_name": "crafting_scene_simulation",
    "test_description": "Simulate crafting scene behavior with save-like initial state",
    "tags": ["integration", "scene", "crafting"],
    "will_pass": true,
    "version": 1
  },
  "initial_scene_state": {
    "scene_type": "SceneType_Crafting",
    "entity_collection": {
      "entity_memory_pool_size": 100,
      "entities": [
        {
          "index": 0,
          "c_grimoire_machina": {
            "all_fragments": {
              "fragment_1": {
                "fragment_id": "fire_fragment",
                "element_type": "Fire"
              }
            },
            "all_joints": {},
            "machina_forms": {}
          }
        },
        {
          "index": 1,
          "c_user_interface": {
            "ui_name": "crafting_panel",
            "start_visible": true
          }
        }
      ]
    },
    "scene_flags": ["grimoire_initialized", "ui_loaded"]
  },
  "expected_scene_state": {
    "scene_type": "SceneType_Crafting",
    "entity_collection": {
      "entity_memory_pool_size": 100,
      "entities": [
        {
          "index": 0,
          "c_grimoire_machina": {
            "all_fragments": {
              "fragment_1": {
                "fragment_id": "fire_fragment",
                "element_type": "Fire"
              }
            },
            "holding_form": {
              "form_name": "new_machina"
            }
          }
        }
      ]
    },
    "scene_flags": ["grimoire_initialized", "ui_loaded", "form_created"]
  },
  "simulation_data": {
    "steps": [
      {"logic_class_type": "UIActionLogic"},
      {"function_type": "ProcessButtonElementActions"}
    ]
  },
  "num_ticks": 5
}
```

## Data Flow Comparison

### SaveData Loading (Production)

```
1. User selects "Load Game" → slot index
   ↓
2. SaveDataManager::LoadGame(slot)
   ↓
3. Read SaveData from file (save_slot_1.bin)
   ↓
4. Extract SavedSceneData for each scene
   ↓
5. SceneManager::RestoreFromSaveData(saved_scenes)
   ↓
6. For each scene:
   - Create Scene with scene_type
   - FlatbuffersEntityConfigurator::ConfigureEntitiesFromSaveData()
   - Populate EntityMemoryPool from entity_collection
   - Restore scene_flags
   ↓
7. Engine continues with restored state
```

### TestData Loading (Testing)

```
1. Test framework loads test_data.json
   ↓
2. Parse TestDataFbs
   ↓
3. Extract initial_scene_state
   ↓
4. TestEngine::InitializeFromTestData(test_data)
   ↓
5. For test scene:
   - Create Scene with scene_type
   - FlatbuffersEntityConfigurator::ConfigureEntitiesFromTestData()
   - Populate EntityMemoryPool from entity_collection
   - Set scene_flags
   ↓
6. Run simulation steps
   ↓
7. Compare final state with expected_scene_state
```

**Key Similarity**: Steps 5-6 use the SAME configurator logic, just different data sources!

## Implementation Approach

### Phase 1: Shared Configurator Infrastructure

Create a unified configurator that works with both SaveData and TestData:

```cpp
// src/entity/FlatbuffersEntityConfigurator.h

class FlatbuffersEntityConfigurator {
public:
  // Existing method for default scene data
  std::expected<std::monostate, FailInfo>
  ConfigureEntitiesFromDefaultData(const EntityCollectionFbs* entity_collection);
  
  // NEW: Method for SaveData (will be added in save system implementation)
  std::expected<std::monostate, FailInfo>
  ConfigureEntitiesFromSaveData(const EntityCollectionFbs* entity_collection);
  
  // NEW: Method for TestData (uses same logic as SaveData)
  std::expected<std::monostate, FailInfo>
  ConfigureEntitiesFromTestData(const EntityCollectionFbs* entity_collection);
  
private:
  // Shared internal method
  std::expected<std::monostate, FailInfo>
  ConfigureEntitiesFromCollection(const EntityCollectionFbs* entity_collection);
};
```

**Implementation Detail**: All three public methods can delegate to the shared private method since they all work with `EntityCollectionFbs`.

### Phase 2: TestEngine Initialization

Add TestEngine initialization from TestData that mirrors SaveData loading:

```cpp
// src/engine/TestEngine.h

class TestEngine : public Engine {
public:
  // Initialize engine with test data (similar to loading a save)
  std::expected<std::monostate, FailInfo>
  InitializeFromTestData(const TestDataFbs* test_data);
  
private:
  // Create scene from test scene data (mirrors creating from SavedSceneData)
  std::expected<std::monostate, FailInfo>
  CreateSceneFromTestData(const TestSceneData* test_scene_data);
};
```

### Phase 3: Test Harness Integration

```cpp
// tests/harness/test_data_harness.h

namespace steamrot::tests {

// Load and execute a test that uses scene state
std::expected<std::monostate, FailInfo>
run_scene_state_test(const TestDataFbs* config) {
  // 1. Create TestEngine
  TestEngine engine;
  
  // 2. Initialize from test data (mimics loading save)
  auto init_result = engine.InitializeFromTestData(config);
  if (!init_result.has_value()) {
    return std::unexpected(init_result.error());
  }
  
  // 3. Run simulation steps
  if (config->simulation_data()) {
    auto sim_result = run_simulation(engine, config->simulation_data());
    if (!sim_result.has_value()) {
      return std::unexpected(sim_result.error());
    }
  }
  
  // 4. Compare final state with expected_scene_state
  if (config->expected_scene_state()) {
    auto compare_result = compare_scene_state(
        engine.GetSceneState(),
        config->expected_scene_state()
    );
    if (!compare_result.has_value()) {
      return std::unexpected(compare_result.error());
    }
  }
  
  return std::monostate{};
}

} // namespace steamrot::tests
```

## Benefits of This Approach

### 1. Test-Driven Save System Development

You can write integration tests for save/load BEFORE implementing the production SaveDataManager:

```cpp
TEST_CASE("Save and restore grimoire state", "[integration][save]") {
  // Load test data that mimics a save file
  auto test_configs = steamrot::tests::load_test_data_configs("save_load");
  REQUIRE(test_configs.has_value());
  
  const auto* config = test_configs.value()[0];
  
  // This tests the save/load pipeline using test data
  auto result = steamrot::tests::run_scene_state_test(config);
  REQUIRE(result.has_value());
}
```

### 2. Debugging Production Saves

Convert a production save to test data for debugging:

```bash
# Hypothetical utility
./tools/save_to_test_data data/user/saves/slot_1.bin > tests/debug/slot_1.test_data.json

# Now you can debug the save with test infrastructure
```

### 3. Save Format Validation

Test that save data can be loaded correctly:

```json
{
  "meta_data": {
    "test_name": "validate_save_v1_format",
    "test_description": "Ensure v1 save format loads correctly"
  },
  "initial_scene_state": {
    "scene_type": "SceneType_Title",
    "entity_collection": {
      // Copied from actual save file
    }
  }
}
```

### 4. Schema Evolution Testing

Test backward compatibility when adding new fields:

```json
{
  "meta_data": {
    "test_name": "load_old_save_format",
    "test_description": "Load save without new tile_display_config field"
  },
  "initial_scene_state": {
    "scene_type": "SceneType_Crafting",
    "entity_collection": { /* old format */ }
    // Note: tile_display_config is missing (old save)
  },
  "expected_scene_state": {
    "scene_type": "SceneType_Crafting",
    "entity_collection": { /* expected state */ }
    // Should initialize tile_display_config with defaults
  }
}
```

## Migration Path

### Step 1: Update test_data.fbs
Add `initial_scene_state` and `expected_scene_state` fields to TestDataFbs.

### Step 2: Extend FlatbuffersEntityConfigurator
Add `ConfigureEntitiesFromTestData()` method (can reuse internal logic).

### Step 3: Update TestEngine
Add `InitializeFromTestData()` method to create scenes from test data.

### Step 4: Update Test Harness
Add `run_scene_state_test()` function for scene state tests.

### Step 5: Write Integration Tests
Create test data files that mimic save structures.

### Step 6: Implement SaveData System
When implementing SaveDataManager, reuse the same configurator infrastructure.

## Example Use Cases

### Use Case 1: Test Grimoire Persistence

```json
{
  "meta_data": {
    "test_name": "grimoire_fragments_persist",
    "test_description": "Verify grimoire fragments are maintained through simulation"
  },
  "initial_scene_state": {
    "scene_type": "SceneType_Crafting",
    "entity_collection": {
      "entity_memory_pool_size": 50,
      "entities": [
        {
          "index": 0,
          "c_grimoire_machina": {
            "all_fragments": {
              "fire_fragment": {
                "fragment_id": "fire_fragment",
                "element_type": "Fire",
                "power_level": 5
              }
            }
          }
        }
      ]
    }
  },
  "simulation_data": {
    "steps": [
      {"logic_class_type": "UIActionLogic"},
      {"logic_class_type": "CraftingRenderLogic"}
    ]
  },
  "expected_scene_state": {
    "entity_collection": {
      "entities": [
        {
          "index": 0,
          "c_grimoire_machina": {
            "all_fragments": {
              "fire_fragment": {
                "fragment_id": "fire_fragment",
                "element_type": "Fire",
                "power_level": 5
              }
            }
          }
        }
      ]
    }
  },
  "num_ticks": 10
}
```

### Use Case 2: Test UI State Restoration

```json
{
  "meta_data": {
    "test_name": "ui_state_restoration",
    "test_description": "Verify UI visibility states are restored from save"
  },
  "initial_scene_state": {
    "scene_type": "SceneType_Title",
    "entity_collection": {
      "entities": [
        {
          "index": 0,
          "c_user_interface": {
            "ui_name": "main_menu",
            "start_visible": false
          },
          "c_ui_state": {
            "is_visible": true,
            "is_hovered": false
          }
        }
      ]
    },
    "scene_flags": ["menu_initialized"]
  },
  "expected_scene_state": {
    "entity_collection": {
      "entities": [
        {
          "index": 0,
          "c_ui_state": {
            "is_visible": true
          }
        }
      ]
    }
  }
}
```

## Key Design Principles

1. **Shared Schema**: Use same `EntityCollectionFbs` in both systems
2. **Parallel Structure**: TestSceneData mirrors SavedSceneData
3. **Unified Configuration**: Same configurator logic for both systems
4. **Test as Documentation**: Test data files serve as save format examples
5. **Early Validation**: Test save/load patterns before production implementation

## Conclusion

By aligning TestData with SaveData structure:

- **TestData.initial_scene_state** ≈ **SaveData.saved_scenes** (source state)
- **TestData.expected_scene_state** ≈ **SaveData.saved_scenes** (after gameplay)
- **TestData simulation** ≈ **Actual gameplay between saves**

This creates a unified approach where:
1. Tests validate save/load patterns early
2. Production save system reuses tested configuration code
3. Debugging can use test infrastructure with save data
4. Schema evolution is validated through tests

The test system becomes a safe playground for developing and validating the save system before production deployment.
