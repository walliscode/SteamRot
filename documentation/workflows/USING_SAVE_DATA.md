# Using SaveData in SteamRot

This guide explains how to use the SaveData provider system to load game state from save files and configure tests with saved game states.

## Overview

The SaveData system provides:
- **ISaveDataProvider**: Interface for loading save files
- **Native SaveData struct**: C++ representation of complete game state
- **Integration with ISceneDataProvider**: Load scenes from save data
- **Test integration**: Configure tests with SaveData fixtures

## Basic Usage

### Loading Save Data

```cpp
#include "provider_factory.h"
#include "SaveData.h"

// Get the save data provider
ISaveDataProvider& provider = steamrot::GetSaveDataProvider();

// Check if save exists
if (provider.SaveExists(0)) {
  // Load save data
  auto result = provider.LoadSaveData(0);
  
  if (result.has_value()) {
    const steamrot::SaveData& save = result.value();
    
    // Access save components
    std::cout << "Save: " << save.metadata.save_name << std::endl;
    std::cout << "Play time: " << save.metadata.play_time_seconds << "s" << std::endl;
    
    // Use scene data
    if (save.scene_data) {
      // Configure scene with saved data
      ConfigureSceneFromSave(save.scene_data);
    }
  }
}
```

### Loading Save Metadata Only

For save slot UI, you can load just the metadata without the full save:

```cpp
// Get metadata for save slot selection
auto metadata_result = provider.GetSaveMetadata(0);

if (metadata_result.has_value()) {
  const auto& metadata = metadata_result.value();
  
  // Display in UI
  DisplaySaveSlot(metadata.save_name, 
                 metadata.play_time_seconds,
                 metadata.last_modified);
}
```

### Using SaveData with ISceneDataProvider

Extract scene data from a save file:

```cpp
#include "provider_factory.h"

// Load save data
auto save_result = steamrot::GetSaveDataProvider().LoadSaveData(slot);
if (!save_result.has_value()) {
  return std::unexpected(save_result.error());
}

steamrot::SaveData save_data = std::move(save_result.value());

// Extract scene data
auto scene_result = steamrot::GetSceneDataProvider()
                      .ProvideSceneDataFromSave(save_data);

if (scene_result.has_value()) {
  auto& scene_data = scene_result.value();
  // Use scene_data to configure Scene
}
```

## Save File Format

Save files are FlatBuffers binary files with `.save` extension, stored in:
- **Production**: `data/user/saves/save_slot_N.save`
- **Test**: `tests/data/user/saves/save_slot_N.save`

### JSON Source Format

Create save files from JSON during build:

```json
{
  "metadata": {
    "save_name": "My Game Save",
    "created_at": "2024-01-01T12:00:00Z",
    "last_modified": "2024-01-02T15:30:00Z",
    "game_version": "1.0.0",
    "play_time_seconds": 7200,
    "slot_index": 0
  },
  "scene_data": {
    "scene_type": "TITLE",
    "entity_pool_size": 50,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "main_menu",
          "start_visible": true,
          "root_ui_element": {
            "base_data": {
              "position": { "x": 100, "y": 200 },
              "size": { "x": 300, "y": 400 },
              "children_active": false,
              "children": [],
              "layout": "Vertical",
              "spacing_strategy": "None"
            }
          }
        }
      }
    ]
  },
  "engine_state": {
    "running": true,
    "paused": false,
    "subscriptions": [],
    "quit_requested": false
  },
  "scene_manager_state": {
    "subscriptions": []
  },
  "scene_state": {
    "active": true
  }
}
```

**File naming**: `save_slot_N.json` (e.g., `save_slot_0.json`, `save_slot_1.json`)

CMake automatically compiles these to `.save` files during build.

## Using SaveData in Tests

### Test Configuration with SaveData

Use SaveData in test_data.json files to set up complete game state:

```json
{
  "metadata": {
    "test_name": "load_game_from_save",
    "description": "Test loading game state from save data",
    "tags": ["integration", "save_load"],
    "expected_to_pass": true
  },
  "save_data": {
    "metadata": {
      "save_name": "Test Save",
      "created_at": "2024-01-01T00:00:00Z",
      "game_version": "1.0.0",
      "slot_index": 0
    },
    "scene_data": {
      "scene_type": "CRAFTING",
      "entity_pool_size": 20,
      "entities": [
        // Entity configurations...
      ]
    },
    "engine_state": {
      "running": true,
      "paused": false
    },
    "scene_state": {
      "active": true
    }
  },
  "num_ticks": 5,
  "tick_snapshots": []
}
```

**Note**: When `save_data` is present in TestDataConfig, it takes precedence over `starting_engine_state` for configuring the test engine.

### Example Test Using SaveData

```cpp
TEST_CASE("Load and verify game state from save", "[integration]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto* config = GENERATE_COPY(from_range(configs.value()));
  
  // Check if this test uses save_data
  if (config->save_data()) {
    // TestEngine will automatically use save_data
    steamrot::tests::TestEngine engine(config);
    
    // Run test
    auto result = engine.Run();
    REQUIRE(result.has_value());
    
    // Verify state loaded from save
    const auto& data_bank = engine.GetDataBank();
    // Assertions...
  }
}
```

## SaveData Structure

### Native C++ Struct

```cpp
struct SaveData {
  SaveMetadata metadata;                      // Display info, timestamps
  std::unique_ptr<SceneData> scene_data;      // Polymorphic scene data
  EngineState engine_state;                   // Engine flags, subscriptions
  SceneManagerState scene_manager_state;      // Scene manager subscriptions
  SceneState scene_state;                     // Scene active status
};
```

### SaveMetadata

```cpp
struct SaveMetadata {
  std::string save_name;           // Display name
  std::string created_at;          // ISO 8601 timestamp
  std::string last_modified;       // ISO 8601 timestamp
  std::string game_version;        // e.g., "1.0.0"
  uint64_t play_time_seconds;      // Total play time
  uint32_t slot_index;             // 0-based slot number
};
```

## Working with Scene Data

### SceneData Polymorphism

SaveData uses polymorphic SceneData to support different implementations:

```cpp
// SaveData contains polymorphic pointer
std::unique_ptr<SceneData> scene_data;

// For FlatBuffers implementation
FbsSceneData* fbs_scene = dynamic_cast<FbsSceneData*>(save_data.scene_data.get());
if (fbs_scene) {
  // Access FlatBuffers-specific data
  const SceneDataFbs* fb_data = fbs_scene->scene_data_fbs;
}
```

### Extracting Scene Data

Use ISceneDataProvider to extract and transfer ownership:

```cpp
// ProvideSceneDataFromSave moves scene_data out of SaveData
auto scene_result = GetSceneDataProvider().ProvideSceneDataFromSave(save_data);

// After this call, save_data.scene_data is nullptr (ownership transferred)
REQUIRE(save_data.scene_data == nullptr);
REQUIRE(scene_result.value() != nullptr);
```

## Error Handling

All SaveData operations return `std::expected`:

```cpp
auto result = provider.LoadSaveData(slot);

if (!result.has_value()) {
  const steamrot::FailInfo& error = result.error();
  
  switch (error.mode) {
    case steamrot::FailMode::FileNotFound:
      // Handle missing save file
      break;
    case steamrot::FailMode::FlatbuffersDataNotFound:
      // Handle corrupt save file
      break;
    case steamrot::FailMode::NullPointer:
      // Handle null data
      break;
    default:
      // Handle other errors
      break;
  }
}
```

## Best Practices

### Save File Management

1. **Slot-based naming**: Use `save_slot_N` pattern for consistency
2. **Metadata first**: Load metadata before full save for UI performance
3. **Version checking**: Validate `game_version` for compatibility
4. **Error handling**: Always check `std::expected` results

### Testing with SaveData

1. **Isolated fixtures**: Create separate save files for each test scenario
2. **Minimal data**: Include only necessary entities for test
3. **Clear metadata**: Use descriptive `save_name` and `description`
4. **Scene type matching**: Ensure `scene_type` matches test expectations

### Performance

1. **Lazy loading**: Use `SaveExists()` before `LoadSaveData()`
2. **Metadata caching**: Cache metadata results for save slot UI
3. **Move semantics**: Use `std::move()` when transferring SaveData
4. **Minimal copies**: SceneData uses unique_ptr to avoid copies

## Integration Points

### With Engine

```cpp
class Engine {
  void LoadFromSave(uint32_t slot_index) {
    auto result = GetSaveDataProvider().LoadSaveData(slot_index);
    if (result.has_value()) {
      ApplySaveData(result.value());
    }
  }
  
  void ApplySaveData(const SaveData& save) {
    m_engine_state = save.engine_state;
    m_scene_manager.ApplyState(save.scene_manager_state);
    // Configure current scene with save.scene_data
  }
};
```

### With TestEngine

TestEngine automatically handles SaveData in TestDataConfig:

```cpp
TestEngine::TestEngine(const TestDataConfig* config) {
  if (config->save_data()) {
    // Load complete state from SaveData
    LoadFromSaveData(config->save_data());
  } else if (config->starting_engine_state()) {
    // Use traditional engine state loading
    LoadEngineState(config->starting_engine_state());
  }
}
```

## Future Extensions

### Planned Features

- **Auto-save**: Periodic automatic save creation
- **Cloud sync**: Save synchronization across devices
- **Compression**: Compressed save files for disk space
- **Encryption**: Encrypted saves for security
- **Migration**: Automatic save format migration between versions

### Extensibility

Adding new data to SaveData is straightforward:

1. Add field to `SaveData` struct (e.g., `PlayerInventory inventory`)
2. Add corresponding table to `save_data.fbs`
3. Update `FlatbuffersSaveDataProvider::LoadSaveData()` to populate field
4. Update JSON save files to include new data

Example:

```cpp
// SaveData.h
struct SaveData {
  // ... existing fields ...
  PlayerInventory inventory;  // New field
};

// save_data.fbs
table SaveData {
  // ... existing fields ...
  inventory: InventoryData;  // New table
}
```

## Troubleshooting

### Save file not found

**Error**: `FailMode::FileNotFound`

**Solutions**:
- Verify save file exists: `data/user/saves/save_slot_N.save`
- Check slot index is correct (0-based)
- Ensure CMake has run to compile .json to .save

### Corrupt save data

**Error**: `FailMode::FlatbuffersDataNotFound`

**Solutions**:
- Verify JSON structure matches schema
- Rebuild to recompile save files
- Check FlatBuffers version compatibility

### Null scene data

**Error**: `FailMode::NullPointer` with message "SaveData contains null scene_data"

**Solutions**:
- Ensure `scene_data` field exists in JSON
- Verify `scene_data` is properly populated
- Check that SceneData wasn't already moved out

## Related Documentation

- [SaveData Provider Design](../analysis/SAVE_DATA_PROVIDER_DESIGN.md) - Architecture details
- [Test Data Configuration](FILLING_TEST_DATA.md) - Test data guide
- [ISceneDataProvider](../../src/interfaces/ISceneDataProvider.h) - Scene data interface
- [FlatBuffers Schemas](../../src/types/flatbuffers/configuration/save_data.fbs) - Schema definitions
