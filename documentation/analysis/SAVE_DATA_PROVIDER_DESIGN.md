# SaveData Provider System Design

## Overview

This document describes the architecture for loading game state from save data files. The SaveData provider system builds upon the existing ISceneDataProvider architecture to enable loading game configuration from both default data and saved game files.

## Problem Statement

Currently, the game can only load default scene data via `ISceneDataProvider::ProvideDefaultSceneData()`. We need a system to:

1. Load complete game state from save data files
2. Use save data to configure scenes, engine state, and game context
3. Enable test-driven development with save data fixtures
4. Maintain consistency with existing provider patterns

## Architecture

### Core Components

#### 1. Native SaveData Struct (`src/types/core/SaveData.h`)

A native C++ struct that contains pointers to polymorphic types (like `SceneData`) and directly stores cheap copyable data.

```cpp
namespace steamrot {

struct SaveData {
  /////////////////////////////////////////////////
  /// @brief Metadata about the save file
  /////////////////////////////////////////////////
  SaveMetadata metadata;
  
  /////////////////////////////////////////////////
  /// @brief Scene data (polymorphic - FbsSceneData for FlatBuffers)
  /////////////////////////////////////////////////
  std::unique_ptr<SceneData> scene_data;
  
  /////////////////////////////////////////////////
  /// @brief Engine state configuration
  /////////////////////////////////////////////////
  EngineState engine_state;
  
  /////////////////////////////////////////////////
  /// @brief Scene manager state
  /////////////////////////////////////////////////
  SceneManagerState scene_manager_state;
  
  /////////////////////////////////////////////////
  /// @brief Scene state for current scene
  /////////////////////////////////////////////////
  SceneState scene_state;
  
  // Future extensions:
  // - Player inventory
  // - Quest progress
  // - Game flags/switches
};

struct SaveMetadata {
  std::string save_name;
  std::string created_at;
  std::string last_modified;
  std::string game_version;
  uint64_t play_time_seconds{0};
  uint32_t slot_index{0};
};

} // namespace steamrot
```

**Key Design Decisions:**
- **Polymorphic SceneData**: Uses `std::unique_ptr<SceneData>` to support different scene data implementations (FlatBuffers, JSON, etc.)
- **Value Types**: Simple structs like `EngineState`, `SceneManagerState` are stored by value for efficiency
- **Extensibility**: Easy to add new fields without breaking existing code

#### 2. ISaveDataProvider Interface (`src/interfaces/ISaveDataProvider.h`)

Abstract interface for loading save data from various sources.

```cpp
namespace steamrot {

class ISaveDataProvider {
public:
  virtual ~ISaveDataProvider() = default;
  
  /////////////////////////////////////////////////
  /// @brief Load save data from a specific save slot
  ///
  /// @param slot_index The save slot to load (0-based)
  /// @return SaveData or failure information
  /////////////////////////////////////////////////
  virtual std::expected<SaveData, FailInfo>
  LoadSaveData(uint32_t slot_index) const = 0;
  
  /////////////////////////////////////////////////
  /// @brief Check if a save slot contains data
  ///
  /// @param slot_index The save slot to check (0-based)
  /// @return true if save exists, false otherwise
  /////////////////////////////////////////////////
  virtual bool SaveExists(uint32_t slot_index) const = 0;
  
  /////////////////////////////////////////////////
  /// @brief Get metadata for a save slot without loading full data
  ///
  /// @param slot_index The save slot to query (0-based)
  /// @return SaveMetadata or failure information
  /////////////////////////////////////////////////
  virtual std::expected<SaveMetadata, FailInfo>
  GetSaveMetadata(uint32_t slot_index) const = 0;
};

} // namespace steamrot
```

#### 3. FlatbuffersSaveDataProvider (`src/data_providers/FlatbuffersSaveDataProvider.h`)

Concrete implementation that loads SaveData from FlatBuffers files.

```cpp
namespace steamrot {

class FlatbuffersSaveDataProvider : public ISaveDataProvider {
public:
  std::expected<SaveData, FailInfo>
  LoadSaveData(uint32_t slot_index) const override;
  
  bool SaveExists(uint32_t slot_index) const override;
  
  std::expected<SaveMetadata, FailInfo>
  GetSaveMetadata(uint32_t slot_index) const override;
};

} // namespace steamrot
```

**Implementation Details:**
- Loads `.save` files from `data/saves/` directory
- Uses `FlatbuffersDataLoader` to read binary data
- Converts FlatBuffers data to native SaveData struct
- Handles missing files gracefully

### Integration with ISceneDataProvider

The `ISceneDataProvider` interface will be extended to accept SaveData:

```cpp
class ISceneDataProvider {
public:
  virtual ~ISceneDataProvider() = default;
  
  /////////////////////////////////////////////////
  /// @brief Provides default scene data
  /////////////////////////////////////////////////
  virtual std::expected<std::unique_ptr<SceneData>, FailInfo>
  ProvideDefaultSceneData(const SceneType scene_type) const = 0;
  
  /////////////////////////////////////////////////
  /// @brief Provides scene data from save data
  ///
  /// @param save_data SaveData containing scene configuration
  /// @return SceneData extracted from save data
  /////////////////////////////////////////////////
  virtual std::expected<std::unique_ptr<SceneData>, FailInfo>
  ProvideSceneDataFromSave(const SaveData& save_data) const = 0;
};
```

**Key Points:**
- `ProvideDefaultSceneData()` remains unchanged for default scene loading
- New `ProvideSceneDataFromSave()` extracts scene data from SaveData
- `FlatbuffersSceneDataProvider` implements both methods

### FlatBuffers Schema Extensions

#### save_data.fbs

```fbs
include "../entities/entities.fbs";
include "../core/scene_types.fbs";
include "../events/event_bus_data.fbs";
include "../engine/engine_state.fbs";
include "../scenes/scene_data.fbs";

namespace steamrot;

////////////////////////////////////////////////////////////
/// Metadata for save file management
////////////////////////////////////////////////////////////
table SaveMetadata {
  save_name: string;
  created_at: string;
  last_modified: string;
  game_version: string;
  play_time_seconds: uint64 = 0;
  slot_index: uint32 = 0;
}

////////////////////////////////////////////////////////////
/// Scene manager runtime state
////////////////////////////////////////////////////////////
table SceneManagerStateFbs {
  subscriptions: [SubscriberFbs];
}

////////////////////////////////////////////////////////////
/// Scene runtime state
////////////////////////////////////////////////////////////
table SceneStateFbs {
  active: bool = true;
}

////////////////////////////////////////////////////////////
/// Root table containing complete save game data
////////////////////////////////////////////////////////////
table SaveData {
  /// Metadata about this save file
  metadata: SaveMetadata (required);
  
  /// Scene data for the saved scene
  scene_data: SceneDataFbs (required);
  
  /// Engine state at time of save
  engine_state: EngineStateFbs;
  
  /// Scene manager state
  scene_manager_state: SceneManagerStateFbs;
  
  /// Scene state
  scene_state: SceneStateFbs;
}

root_type SaveData;
```

### Integration with TestData System

The `TestDataConfig` schema will be extended to include SaveData:

```fbs
table TestDataConfig {
  metadata: TestMetadata (required);
  
  /// Starting engine state (existing)
  starting_engine_state: EngineStateFbs;
  
  /// NEW: Optional save data for configuring test starting state
  /// When present, overrides starting_engine_state and provides
  /// complete scene configuration
  save_data: SaveData;
  
  simulation_data: SimulationData;
  num_ticks: uint32;
  tick_snapshots: [TickSnapshot];
}
```

**Usage Pattern:**
```cpp
// In test harness
if (config->save_data()) {
  // Load complete state from save data
  SaveData save_data = ConvertToNativeSaveData(config->save_data());
  
  // Configure engine from save data
  engine.LoadFromSaveData(save_data);
} else if (config->starting_engine_state()) {
  // Use existing pattern for engine state only
  engine.LoadEngineState(config->starting_engine_state());
}
```

## Data Flow

### Loading from Save File

```
User selects save slot
    ↓
FlatbuffersSaveDataProvider::LoadSaveData(slot_index)
    ↓
Read save_slot_N.save file
    ↓
Parse FlatBuffers SaveData
    ↓
Convert to native SaveData struct
    ↓
Extract SceneData via ProvideSceneDataFromSave()
    ↓
Configure Engine, SceneManager, Scene
```

### Test Data Flow

```
Load test_data.bin file
    ↓
Check for save_data field
    ↓
If present: Use SaveData to configure test starting state
    ↓
TestEngine loads SaveData
    ↓
Scene configured with saved entity pool
    ↓
Execute test simulation
```

## Implementation Plan

### Phase 1: Core Types and Interface
1. Create `SaveData.h` and `SaveMetadata.h` native structs
2. Create `ISaveDataProvider.h` interface
3. Extend `save_data.fbs` FlatBuffers schema
4. Update `ISceneDataProvider` with `ProvideSceneDataFromSave()`

### Phase 2: FlatBuffers Implementation
1. Create `FlatbuffersSaveDataProvider` class
2. Implement `LoadSaveData()`, `SaveExists()`, `GetSaveMetadata()`
3. Update `FlatbuffersSceneDataProvider::ProvideSceneDataFromSave()`
4. Add to `provider_factory.h/cpp`

### Phase 3: Test Integration
1. Extend `test_data.fbs` with `save_data` field
2. Update `TestEngine` to handle SaveData
3. Create test utilities for SaveData fixtures
4. Add unit tests for all components

### Phase 4: Documentation and Examples
1. Create workflow guide for SaveData usage
2. Add example test data files with SaveData
3. Document save file format and structure
4. Update architecture diagrams

## Benefits

### For Game Development
- **Save/Load System**: Foundation for player save games
- **Quick Start**: Load directly into specific game states
- **Debug Support**: Create save files for specific scenarios

### For Testing
- **Data-Driven Tests**: Configure complex starting states in JSON
- **Reproducibility**: Exact game state snapshots
- **Integration Tests**: Test save/load functionality

### For Architecture
- **Separation of Concerns**: Clear provider responsibilities
- **Extensibility**: Easy to add new data types
- **Consistency**: Follows existing patterns
- **Type Safety**: Native C++ structs with compile-time checking

## Future Extensions

### Additional Save Data
- Player inventory
- Quest/mission progress
- Game flags and switches
- Audio/video settings
- Key bindings

### Advanced Features
- Auto-save functionality
- Cloud save synchronization
- Save file versioning/migration
- Compressed save files
- Encrypted save data

### Testing Features
- SaveData generators for random test states
- SaveData comparison utilities
- SaveData mutation for fuzzing tests
- SaveData visualization tools

## References

- [ISceneDataProvider Pattern](../../src/interfaces/ISceneDataProvider.h)
- [Test Data Configuration](../workflows/FILLING_TEST_DATA.md)
- [Context Configuration](../configuration/CONTEXT_CONFIGURATION.md)
- [Provider Factory Pattern](../../src/data_providers/provider_factory.h)
