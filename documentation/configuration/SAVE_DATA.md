# Save Data System

## Overview

The save data system provides persistent storage for gameplay progress,
allowing players to save and resume their game across sessions.

## Architecture

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           DATA FLOW                                          │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │                     Default Scene Data                               │    │
│  │               (scenes/*.scene_data.json)                             │    │
│  │         Initial game state - "New Game" template                     │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                   │                                          │
│                                   │  New Game                                │
│                                   ▼                                          │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │                     Runtime Game State                               │    │
│  │              (EntityMemoryPool, EventBus, etc.)                      │    │
│  │            Active game state during gameplay                         │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                    │                                    ▲                    │
│            Save    │                                    │  Load              │
│                    ▼                                    │                    │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │                       Save Data Files                                │    │
│  │               (saves/slot_X.save.bin)                                │    │
│  │          Serialized game state with metadata                         │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

## Save Data Structure

### SaveMetadata

| Field | Type | Description |
|-------|------|-------------|
| `save_name` | string | Display name for the save |
| `created_at` | string | Creation timestamp (ISO 8601) |
| `last_modified` | string | Last modification timestamp |
| `game_version` | string | Game version that created the save |
| `play_time_seconds` | uint64 | Total play time |
| `slot_index` | uint32 | Save slot identifier |

### SaveData

| Field | Type | Description |
|-------|------|-------------|
| `metadata` | SaveMetadata | Save file metadata (required) |
| `current_scene_type` | SceneType | Scene player was in when saving |
| `scene_states` | [SceneState] | Entity states per scene |
| `global_event_bus` | EventBusData | Persistent event data |
| `version` | uint32 | Save format version |

### SceneState

Minimal save data for a scene. Distinct from `SceneDataData` which includes
full scene definition (assets, logic, resources, etc.).

| Field | Type | Description |
|-------|------|-------------|
| `scene_type` | SceneType | Scene identifier |
| `entity_collection` | EntityCollection | All entity data for the scene |

## SceneState vs SceneDataData

| SceneState (save_data.fbs) | SceneDataData (scene_data.fbs) |
|----------------------------|--------------------------------|
| Minimal: scene_type + entities | Full: assets, logic, resources, entities |
| For save files | For scene definitions |
| Runtime-captured state | Default scene template |

## File Locations

- **Save directory**: `{data_dir}/saves/`
- **Save files**: `slot_{index}.save.bin` (0-9)

## Usage

### Listing Save Slots

```cpp
#include "FlatbuffersSaveDataProvider.h"

steamrot::FlatbuffersSaveDataProvider provider;

auto slots_result = provider.GetSaveSlots();
if (slots_result.has_value()) {
    for (const auto& slot : slots_result.value()) {
        if (slot.is_populated) {
            std::cout << slot.save_name << " - " 
                      << slot.play_time_seconds << " seconds\n";
        } else {
            std::cout << "Empty slot " << slot.slot_index << "\n";
        }
    }
}
```

### Loading a Save

```cpp
steamrot::FlatbuffersSaveDataProvider provider;

auto save_result = provider.LoadSave(0);  // Load slot 0
if (save_result.has_value()) {
    const auto& save = save_result.value();
    
    // Restore game state
    SetCurrentScene(save.current_scene_type);
    // Restore entity states...
}
```

### Saving the Game

```cpp
steamrot::FlatbuffersSaveDataProvider provider;

steamrot::SaveData save;
save.metadata.slot_index = 0;
save.metadata.save_name = "My Save";
save.current_scene_type = SceneType::SceneType_CRAFTING;

auto result = provider.SaveGame(save);
if (!result.has_value()) {
    // Handle error
}
```

### Deleting a Save

```cpp
steamrot::FlatbuffersSaveDataProvider provider;

auto result = provider.DeleteSave(0);
if (!result.has_value()) {
    // Handle error
}
```

## Interface Design

The system uses an interface `ISaveDataProvider` to allow for different
implementations:

```cpp
class ISaveDataProvider {
public:
    virtual std::expected<std::vector<SaveSlotInfo>, FailInfo> GetSaveSlots() const = 0;
    virtual std::expected<SaveData, FailInfo> LoadSave(uint32_t slot_index) const = 0;
    virtual std::expected<std::monostate, FailInfo> SaveGame(const SaveData&) = 0;
    virtual std::expected<std::monostate, FailInfo> DeleteSave(uint32_t slot_index) = 0;
    virtual bool HasSaveData(uint32_t slot_index) const = 0;
    virtual uint32_t GetMaxSaveSlots() const = 0;
};
```

This allows:
- `FlatbuffersSaveDataProvider` for file-based storage
- Mock implementations for testing
- Future cloud save implementations

## FlatBuffers Schema

See `src/flatbuffers_headers/save_data.fbs` for the complete schema.

## Relationship to Other Systems

### vs. Default Scene Data

| Default Scene Data | Save Data |
|-------------------|-----------|
| Template for new games | Captured game state |
| Static, read-only | Dynamic, per-slot |
| In `scenes/` directory | In `saves/` directory |

### vs. User Preferences

| User Preferences | Save Data |
|-----------------|-----------|
| Settings (audio, display) | Game progress |
| Persist across all games | Specific to save slot |
| Single file | Multiple slots |

### vs. TestEngine

**Important**: The save data system is only used by `GameEngine`. The `TestEngine`
does NOT interact with save files - it uses injected `TestDataConfig` for all
its starting state. This keeps tests isolated and reproducible.

| GameEngine | TestEngine |
|------------|------------|
| Loads/saves to files | Uses injected config |
| Supports save slots | No save functionality |
| Persists between runs | Ephemeral per test |

## Save Slot Management

- **Maximum slots**: 10 (configurable via `kMaxSaveSlots`)
- **Slot indices**: 0-9
- **Empty slots**: No file on disk
- **Slot info**: Metadata loaded without full save data

## Best Practices

1. **Quick save/load**: Load metadata only for slot listings
2. **Version migration**: Use `version` field for save format updates
3. **Atomic saves**: Write to temp file, then rename
4. **Backup old saves**: Before overwriting, keep backup
5. **Validate on load**: Check for corruption/incompatibility

## Error Handling

The system uses `std::expected` for error handling:

```cpp
auto result = provider.LoadSave(slot);
if (!result.has_value()) {
    switch (result.error().mode) {
        case FailMode::FileNotFound:
            // Slot is empty
            break;
        case FailMode::InvalidInput:
            // Invalid slot index
            break;
        default:
            // Other error
            break;
    }
}
```

## Future Enhancements

- Entity serialization (currently stub implementation)
- Autosave functionality
- Cloud save synchronization
- Save file compression
- Save thumbnails/screenshots
- Import/export saves
