# SaveData Implementation Analysis

## Executive Summary

This document provides a comprehensive analysis for implementing a SaveData system in the SteamRot game engine. The SaveData struct will serve as the native C++ representation for serializing and deserializing complete game state, enabling save/load functionality.

**Key Findings:**
- SaveData schema already exists but is currently empty (`save_data.fbs`)
- SaveData should be managed at the **Engine level**, not Scene level
- Architecture supports clean separation between gameplay data (SaveData) and user preferences (UserPreferencesData)
- Current EntityCollectionFbs provides a foundation for entity state serialization
- Extensibility built-in through FlatBuffers' schema evolution capabilities

**Scope:**
- **Includes**: All game state needed to restore gameplay (entities, scene state, progression)
- **Excludes**: User preferences (already handled by UserPreferencesData), gameplay-agnostic settings
- **Hybrid Data**: Collections like Grimoire blueprints that may be shared across saves

**Recommended Actions:**
1. Design SaveData FlatBuffers schema with scene collection and global game state
2. Create native SaveData C++ struct in `src/types/core/`
3. Implement SaveDataManager at Engine level for save/load operations
4. Add save/load hooks to Engine lifecycle
5. Design hybrid data handling strategy for cross-save resources

---

## Table of Contents

1. [Problem Statement Analysis](#problem-statement-analysis)
2. [Current Architecture Review](#current-architecture-review)
3. [SaveData Design](#savedata-design)
4. [Integration Level Analysis](#integration-level-analysis)
5. [Extensibility Strategy](#extensibility-strategy)
6. [Hybrid Data Handling](#hybrid-data-handling)
7. [Implementation Roadmap](#implementation-roadmap)
8. [Testing Strategy](#testing-strategy)
9. [Future Considerations](#future-considerations)

---

## Problem Statement Analysis

### Requirements Breakdown

From the problem statement:

1. **SaveData Struct**
   - Native C++ struct for serialization/deserialization
   - Contains all information needed to restore game state

2. **Integration Level**
   - Where should save/load happen? (Engine vs Scene)
   - Must be architectural decision considering game lifecycle

3. **Extensibility**
   - Future systems like "tiling display manager" linked to scenes
   - Must support adding new save-worthy state without breaking existing saves

4. **Scope Exclusions**
   - **User preferences**: Already handled by UserPreferencesData (audio, display settings)
   - **Gameplay-agnostic data**: Settings that don't affect game state

5. **Hybrid Data**
   - Example: Grimoire with blueprints and spells
   - Needed in save state BUT may carry over between saves
   - Requires special handling to avoid duplication

---

## Current Architecture Review

### Existing Data Serialization Patterns

The codebase follows a three-layer architecture:

```
┌─────────────────────────────────────────┐
│  Layer 1: Types (src/types/)            │
│  - Pure C++ data structs                │
│  - Zero dependencies on implementation  │
│  Example: EngineConfig, SceneConfig     │
└─────────────────────────────────────────┘
              ▲
              │ configured by
              │
┌─────────────────────────────────────────┐
│  Layer 2: Configuration                 │
│  - Configurators and Providers          │
│  - Converts FlatBuffers → Native types  │
│  Example: FlatbuffersDataLoader         │
└─────────────────────────────────────────┘
              ▲
              │ loads from
              │
┌─────────────────────────────────────────┐
│  Layer 3: Serialized Data               │
│  - FlatBuffers schemas (.fbs)           │
│  - Binary files (.bin) or JSON          │
│  Example: engine_data.json → .bin       │
└─────────────────────────────────────────┘
```

### Relevant Existing Components

**1. User Preferences (Already Implemented)**
- **Schema**: `src/types/flatbuffers/configuration/user_preferences.fbs`
- **Native Type**: Loaded into `EngineConfig` struct
- **Scope**: Display settings (fullscreen, vsync), audio (volume), accessibility
- **Location**: `data/user/preferences/` (user-specific overrides)

**2. Entity Data**
- **Schema**: `src/types/flatbuffers/entities/entities.fbs`
- **Tables**: `EntityDataFbs`, `EntityCollectionFbs`
- **Native Type**: Entities stored in `EntityMemoryPool`
- **Components**: `CUserInterface`, `CGrimoireMachina`, `CUIState`, `CMeta`

**3. Scene Data**
- **Schema**: `src/types/flatbuffers/scenes/scene_data.fbs`
- **Tables**: `SceneDataFbs`, `SceneInfoFbs`, `SceneResourcesFbs`
- **Native Type**: `Scene` class with `SceneConfig`, `SceneState`, `SceneResources`

**4. Engine State**
- **Schema**: `src/types/flatbuffers/engine/engine_state.fbs`
- **Native Type**: `EngineState` struct
- **Contents**: Runtime flags (running, paused), subscriptions, performance metrics

**5. Existing SaveData Schema**
```fbs
// src/types/flatbuffers/configuration/save_data.fbs
table SaveMetadata {
  save_name: string;
  created_at: string;
  last_modified: string;
  game_version: string;
  play_time_seconds: uint64 = 0;
  slot_index: uint32 = 0;
}

table SaveData {
  // Currently empty - to be designed
}
```

### Data Flow Patterns

**Loading Default Data:**
```
1. FlatbuffersDataLoader::ProvideEngineDataFbs()
   ↓
2. Engine::StartUp()
   ↓
3. EngineConfig populated from EngineCoreDataFbs
   ↓
4. SceneManager::AddSceneFromDefault(SceneType)
   ↓
5. FlatbuffersSceneConfigurator configures Scene
   ↓
6. FlatbuffersEntityConfigurator populates EntityMemoryPool
```

**This pattern should be mirrored for save/load.**

---

## SaveData Design

### Conceptual Model

SaveData must capture:

1. **Global Game State**
   - Current scene being played
   - Game progression flags
   - Global timers/counters

2. **Scene Collection State**
   - All active scenes (typically 1, but architecture supports multiple)
   - Per-scene entity collections
   - Per-scene configuration state

3. **Hybrid/Persistent Resources**
   - Grimoire data (blueprints, spells, collected fragments/joints)
   - Unlocked abilities
   - Discovered recipes

4. **Metadata**
   - Save slot info (already defined in SaveMetadata)

### Proposed FlatBuffers Schema

Update `src/types/flatbuffers/configuration/save_data.fbs`:

```fbs
include "../entities/entities.fbs";
include "../core/scene_types.fbs";
include "../events/event_bus_data.fbs";

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
/// Global game progression state
////////////////////////////////////////////////////////////
table GameProgressionData {
  /// Current active scene type
  current_scene_type: SceneType;
  
  /// Total game time elapsed (seconds)
  total_play_time_seconds: uint64 = 0;
  
  /// Game-specific progression flags
  /// Example: ["tutorial_completed", "level_1_unlocked"]
  progression_flags: [string];
  
  /// Arbitrary key-value storage for game-specific counters
  /// Example: {"enemies_defeated": "50", "items_collected": "10"}
  counters: [KeyValuePair];
}

////////////////////////////////////////////////////////////
/// Key-value pair for extensible data storage
////////////////////////////////////////////////////////////
table KeyValuePair {
  key: string (required);
  value: string (required);
}

////////////////////////////////////////////////////////////
/// Saved state for a single scene
////////////////////////////////////////////////////////////
table SavedSceneData {
  /// Type of the scene
  scene_type: SceneType (required);
  
  /// Entity collection for this scene
  entity_collection: EntityCollectionFbs (required);
  
  /// Scene-specific flags/state
  /// Example: ["boss_defeated", "puzzle_solved"]
  scene_flags: [string];
  
  /// Scene-specific extensible data
  scene_data: [KeyValuePair];
}

////////////////////////////////////////////////////////////
/// Collection of hybrid resources that persist across saves
////////////////////////////////////////////////////////////
table HybridResourcesData {
  /// Grimoire data (fragments, joints, machina forms, blueprints)
  /// This is stored here to support potential cross-save sharing
  grimoire_data: GrimoireMachinaData;
  
  /// Other persistent resources can be added here
  /// Example: unlocked_abilities: [string]
}

////////////////////////////////////////////////////////////
/// Root table containing complete save game data
////////////////////////////////////////////////////////////
table SaveData {
  /// Save file metadata
  metadata: SaveMetadata (required);
  
  /// Global game progression state
  game_progression: GameProgressionData (required);
  
  /// Collection of saved scenes
  saved_scenes: [SavedSceneData] (required);
  
  /// Hybrid resources that may be shared across saves
  hybrid_resources: HybridResourcesData;
}

root_type SaveData;
```

### Native C++ Struct Design

Create `src/types/core/SaveData.h`:

```cpp
#pragma once

#include "SceneType.h"
#include "entity_memory.h"
#include "CGrimoireMachina.h"
#include <chrono>
#include <map>
#include <string>
#include <vector>

namespace steamrot {

////////////////////////////////////////////////////////////
/// @brief Key-value pair for extensible data
////////////////////////////////////////////////////////////
struct KeyValuePair {
  std::string key;
  std::string value;
};

////////////////////////////////////////////////////////////
/// @brief Save file metadata
////////////////////////////////////////////////////////////
struct SaveMetadata {
  std::string save_name;
  std::string created_at;      // ISO 8601 format
  std::string last_modified;    // ISO 8601 format
  std::string game_version;
  uint64_t play_time_seconds{0};
  uint32_t slot_index{0};
};

////////////////////////////////////////////////////////////
/// @brief Global game progression state
////////////////////////////////////////////////////////////
struct GameProgressionData {
  SceneType current_scene_type{SceneType::SceneType_Title};
  uint64_t total_play_time_seconds{0};
  std::vector<std::string> progression_flags;
  std::map<std::string, std::string> counters;
};

////////////////////////////////////////////////////////////
/// @brief Saved state for a single scene
////////////////////////////////////////////////////////////
struct SavedSceneData {
  SceneType scene_type;
  EntityMemoryPool entity_pool;
  std::vector<std::string> scene_flags;
  std::map<std::string, std::string> scene_data;
};

////////////////////////////////////////////////////////////
/// @brief Hybrid resources that may persist across saves
////////////////////////////////////////////////////////////
struct HybridResourcesData {
  // Optional: contains Grimoire data if available
  // This allows grimoire to be shared across saves or
  // kept per-save depending on game design
  std::unique_ptr<CGrimoireMachina> grimoire_data;
};

////////////////////////////////////////////////////////////
/// @brief Complete save game data
////////////////////////////////////////////////////////////
struct SaveData {
  SaveMetadata metadata;
  GameProgressionData game_progression;
  std::vector<SavedSceneData> saved_scenes;
  HybridResourcesData hybrid_resources;
};

} // namespace steamrot
```

---

## Integration Level Analysis

### Question: Where Should Save/Load Be Managed?

**Analysis of Options:**

#### Option 1: Scene Level ❌
**Pros:**
- Each scene manages its own state
- Simpler scene-specific logic

**Cons:**
- **Cannot capture global game state** (which scene is active, progression)
- **Cannot coordinate multi-scene saves** (architecture supports multiple active scenes)
- **Scene lifecycle issues**: Scenes are created/destroyed, but saves persist
- Unclear ownership of hybrid resources

#### Option 2: SceneManager Level ⚠️
**Pros:**
- Has access to all scenes
- Manages scene lifecycle

**Cons:**
- **Cannot access Engine-level state** (play time, engine configuration)
- SceneManager is itself managed by Engine
- Still doesn't capture "which scene is active" from Engine's perspective

#### Option 3: Engine Level ✅ **RECOMMENDED**
**Pros:**
- **Has complete view of game state**: all scenes, progression, timers
- **Natural lifecycle integration**: Save on exit, load on startup
- **Consistent with other systems**: Engine manages EngineConfig, EngineState
- **Can coordinate scene serialization** through SceneManager
- **Clear ownership** of save file management

**Cons:**
- Engine must coordinate with SceneManager (but it already does)
- Slightly more complex than scene-local saves

### Recommended Architecture

```
┌─────────────────────────────────────────────┐
│  Engine                                     │
│  ┌───────────────────────────────────────┐  │
│  │ SaveDataManager                       │  │
│  │ - SaveGame(slot)                      │  │
│  │ - LoadGame(slot)                      │  │
│  │ - ListSaves()                         │  │
│  └───────────────────────────────────────┘  │
│           │                                  │
│           │ coordinates                      │
│           ▼                                  │
│  ┌───────────────────────────────────────┐  │
│  │ SceneManager                          │  │
│  │ - GetSceneSaveData() → SavedSceneData │  │
│  │ - RestoreFromSaveData(SavedSceneData) │  │
│  └───────────────────────────────────────┘  │
│           │                                  │
│           │ accesses                         │
│           ▼                                  │
│  ┌───────────────────────────────────────┐  │
│  │ Scene(s)                              │  │
│  │ - EntityMemoryPool                    │  │
│  │ - SceneState                          │  │
│  └───────────────────────────────────────┘  │
└─────────────────────────────────────────────┘
```

**Integration Points:**

1. **Engine::StartUp()**
   - Check for "continue" flag
   - Load most recent save if requested

2. **Engine Menu Actions**
   - "Save Game" → SaveDataManager::SaveGame()
   - "Load Game" → SaveDataManager::LoadGame()
   - "New Game" → LoadGame with slot=new, or LoadDefaultData()

3. **Auto-save Triggers**
   - Scene transitions
   - Periodic intervals
   - Before quit

---

## Extensibility Strategy

### FlatBuffers Schema Evolution

FlatBuffers supports **schema evolution**, allowing new fields to be added without breaking existing saves:

**Compatibility Rules:**
1. **New fields** can be added at any time
2. **Default values** ensure old saves work with new code
3. **Optional fields** (default behavior) allow forward/backward compatibility
4. **Required fields** should be avoided after initial release

**Example: Adding Tiling Display Manager**

Current schema:
```fbs
table SavedSceneData {
  scene_type: SceneType (required);
  entity_collection: EntityCollectionFbs (required);
  scene_flags: [string];
  scene_data: [KeyValuePair];
}
```

Future addition:
```fbs
table TileDisplayConfig {
  tile_width: uint32 = 32;
  tile_height: uint32 = 32;
  tilemap_data: [uint8];
}

table SavedSceneData {
  scene_type: SceneType (required);
  entity_collection: EntityCollectionFbs (required);
  scene_flags: [string];
  scene_data: [KeyValuePair];
  
  // New field - old saves will have this as null
  tile_display_config: TileDisplayConfig;
}
```

**Handling in code:**
```cpp
if (saved_scene_data->tile_display_config()) {
  // New feature available
  scene.ConfigureTileDisplay(saved_scene_data->tile_display_config());
} else {
  // Old save - use defaults or skip
  scene.InitializeDefaultTileDisplay();
}
```

### Extensible Data Patterns

**KeyValuePair Strategy:**
- Allows game-specific data without schema changes
- Useful for prototyping or game-specific features
- Trade-off: No type safety, manual parsing

**When to use:**
- Rapid prototyping of new features
- Game-specific counters/flags
- Per-scene custom data

**When to add schema fields:**
- Feature is stable and released
- Type safety is important
- Structured data (nested objects)

---

## Hybrid Data Handling

### Problem Analysis

**Example: Grimoire System**

The Grimoire contains:
- `m_all_fragments`: All fragments discovered
- `m_all_joints`: All joints discovered
- `m_machina_forms`: Blueprints for machina forms
- `m_holding_form`: Current work-in-progress form

**Use Cases:**
1. **Save-Specific Grimoire**: Each save has its own progression
2. **Shared Grimoire**: Fragments/blueprints discovered in one save are available in others
3. **Hybrid**: Some content is shared (discovered fragments), some is per-save (current holding form)

### Recommended Approach

**1. Default: Per-Save Data**
- Store Grimoire in each SavedSceneData's entity collection
- Each save maintains independent progression
- Simplest implementation, clearest ownership

**2. Extensibility: Hybrid Resources Section**
- Add `HybridResourcesData` to SaveData root
- Contains shareable resources like Grimoire blueprints
- Each save can reference or copy from hybrid resources

**3. Implementation Strategy**

```cpp
// When saving:
void SaveDataManager::SaveGame(uint32_t slot) {
  SaveData save_data;
  
  // Per-save grimoire (normal flow)
  save_data.saved_scenes = GatherSceneData(); // includes entities with CGrimoireMachina
  
  // Optional: Extract shareable parts to hybrid resources
  if (m_enable_grimoire_sharing) {
    save_data.hybrid_resources.grimoire_data = ExtractSharedGrimoireData();
  }
}

// When loading:
void SaveDataManager::LoadGame(uint32_t slot) {
  SaveData save_data = LoadFromFile(slot);
  
  // Restore per-save data
  RestoreScenes(save_data.saved_scenes);
  
  // Optional: Merge hybrid resources
  if (save_data.hybrid_resources.grimoire_data) {
    MergeSharedGrimoireData(save_data.hybrid_resources.grimoire_data);
  }
}
```

**4. Configuration Option**

Add to game configuration:
```fbs
table GameplayConfig {
  grimoire_sharing_enabled: bool = false;
  // Other gameplay options
}
```

This allows the game designer to toggle sharing behavior without code changes.

---

## Implementation Roadmap

### Phase 1: Schema and Native Types (Week 1)

**Tasks:**
1. Update `save_data.fbs` with proposed schema
2. Create `SaveData.h` native struct in `src/types/core/`
3. Update FlatBuffers generation (happens automatically on build)
4. Add unit tests for SaveData struct

**Deliverables:**
- Complete SaveData schema
- Native C++ SaveData type
- Basic tests for struct creation

### Phase 2: Save/Load Infrastructure (Week 2)

**Tasks:**
1. Create `SaveDataManager` class in `src/engine/`
   - `SaveGame(slot)` - Serialize current game state
   - `LoadGame(slot)` - Deserialize and restore state
   - `ListSaves()` - Enumerate available save files
   - `DeleteSave(slot)` - Remove a save file

2. Create `SaveDataConfigurator` in `src/configuration/`
   - Converts SaveData FlatBuffers ↔ Native SaveData struct
   - Follows existing configurator patterns

3. Add save file utilities in `src/data_providers/`
   - `WriteSaveFile(slot, SaveData)`
   - `ReadSaveFile(slot) → SaveData`
   - File path management

**Deliverables:**
- SaveDataManager class
- Save/load functionality
- File I/O utilities

### Phase 3: Engine Integration (Week 2-3)

**Tasks:**
1. Add SaveDataManager to EngineResources
2. Create Engine hooks:
   - `Engine::SaveCurrentGameState()`
   - `Engine::LoadGameFromSave()`
3. Add SceneManager support methods:
   - `SceneManager::GetSceneSaveData() → vector<SavedSceneData>`
   - `SceneManager::RestoreFromSaveData(vector<SavedSceneData>)`
4. Implement Scene serialization:
   - Extract EntityMemoryPool data
   - Capture SceneState flags

**Deliverables:**
- Integrated save/load in Engine
- Scene state serialization
- End-to-end save/load functionality

### Phase 4: Hybrid Resources (Week 3-4)

**Tasks:**
1. Implement Grimoire data extraction/merging
2. Add configuration for hybrid resource sharing
3. Handle edge cases (missing data, version mismatches)

**Deliverables:**
- Hybrid resource handling
- Configuration options
- Migration support

### Phase 5: UI and Polish (Week 4)

**Tasks:**
1. Add save/load UI elements
2. Implement save slot selection
3. Add auto-save functionality
4. Error handling and user feedback

**Deliverables:**
- Complete save/load UI
- Auto-save feature
- User-facing save system

---

## Testing Strategy

### Unit Tests

**1. SaveData Struct Tests**
- Test struct construction and defaults
- Test field assignment and access

**2. SaveDataManager Tests**
- Mock file I/O
- Test save serialization
- Test load deserialization
- Test error handling (corrupted saves, missing files)

**3. Configurator Tests**
- Test FlatBuffers → Native conversion
- Test Native → FlatBuffers conversion
- Test null handling for optional fields

### Integration Tests

**1. Round-Trip Tests**
- Save current state → Load → Verify state matches
- Test with various game states
- Test with empty/minimal saves

**2. Schema Evolution Tests**
- Load old save format with new code
- Verify defaults applied for missing fields

**3. Multi-Scene Tests**
- Save with multiple active scenes
- Load and verify all scenes restored

### System Tests

**1. Gameplay Tests**
- Start game → Play → Save → Exit
- Start game → Load → Continue → Verify state

**2. Edge Case Tests**
- Save during scene transition
- Save with corrupt entity data (error recovery)
- Load save from different game version

### Integration with TestData System

**Unified Testing Approach:**

The SaveData system should align with the TestData system to enable:
1. **Save/Load Testing**: Use TestData with scene state structures that mirror SaveData
2. **Shared Configuration**: Reuse FlatbuffersEntityConfigurator for both systems
3. **Test-Driven Development**: Write tests using TestData before implementing production SaveDataManager

**Recommended TestData Schema Extensions:**

```fbs
table TestSceneData {
  scene_type: SceneType (required);
  entity_collection: EntityCollectionFbs;  // Same as SavedSceneData
  scene_flags: [string];
}

table TestDataFbs {
  meta_data: TestMetadataFbs (required);
  initial_scene_state: TestSceneData;      // Mimics SaveData initial state
  expected_scene_state: TestSceneData;     // Mimics SaveData after gameplay
  simulation_data: SimulationDataFbs;
  num_ticks: uint32;
}
```

**Benefits:**
- Test save/load patterns before production implementation
- Validate schema evolution with test cases
- Debug production saves using test infrastructure
- Use test data as save format documentation

**See**: [SaveData and TestData Integration Guide](SAVE_DATA_TEST_DATA_INTEGRATION.md) for complete integration strategy and examples.

---

## Future Considerations

### Performance Optimization

**Current Design: Full State Serialization**
- Simple: Serialize entire game state
- May be large for complex games

**Future Optimization: Delta Saves**
- Save only changes from default/last save
- More complex but smaller file sizes

**Recommendation:** Start with full serialization, optimize later if needed.

### Cloud Saves

Future extension:
- Add cloud save provider interface
- SaveDataManager uses provider pattern
- Local files for now, cloud backend later

### Save Versioning

```fbs
table SaveData {
  metadata: SaveMetadata (required);
  save_version: uint32 = 1;  // Track schema version
  // ... other fields
}
```

Add migration logic:
```cpp
if (save_data->save_version() < CURRENT_SAVE_VERSION) {
  MigrateSave(save_data);
}
```

### Compressed Saves

FlatBuffers supports compression:
- zlib compression for smaller files
- Can be added transparently to file I/O layer

---

## Conclusion

### Key Recommendations

1. **Use Engine-Level Management**: SaveDataManager should be part of Engine, not Scene
2. **Follow Existing Patterns**: Mirror FlatBuffers schema → Native struct → Configurator pattern
3. **Design for Extensibility**: Use optional fields and KeyValuePair for future features
4. **Handle Hybrid Data Carefully**: Default to per-save data, add sharing as optional feature
5. **Separate Concerns**: Keep user preferences separate from game state (already done)

### Implementation Priority

**High Priority (MVP):**
- Basic SaveData schema (global state + scene collection)
- SaveDataManager with save/load
- Engine integration
- File I/O

**Medium Priority:**
- Hybrid resources handling
- Auto-save
- Save slot management UI

**Low Priority (Future):**
- Cloud saves
- Delta saves
- Advanced versioning/migration

### Success Criteria

SaveData implementation is successful when:
1. ✅ Player can save game at any point
2. ✅ Player can load saved game and continue from saved state
3. ✅ Multiple save slots are supported
4. ✅ Saves are backward compatible with future game versions
5. ✅ Hybrid resources (Grimoire) are handled appropriately
6. ✅ User preferences remain separate from save data

---

## References

### Existing Patterns in Codebase
- `src/types/flatbuffers/configuration/user_preferences.fbs` - Preferences separation
- `src/types/flatbuffers/entities/entities.fbs` - Entity serialization
- `src/engine/Engine.h` - Engine-level management
- `src/data_providers/FlatbuffersDataLoader.h` - Data loading patterns

### FlatBuffers Documentation
- Schema evolution: https://google.github.io/flatbuffers/flatbuffers_guide_writing_schema.html
- Binary format: https://google.github.io/flatbuffers/flatbuffers_internals.html

### SteamRot Documentation
- `.github/copilot-instructions.md` - Coding standards
- `README.md` - Architecture overview
- `SAVE_DATA_TEST_DATA_INTEGRATION.md` - Aligning SaveData with TestData for testing

### Related Analysis Documents
- **[SaveData and TestData Integration Guide](SAVE_DATA_TEST_DATA_INTEGRATION.md)** - How to align SaveData structure with TestData for unified testing and save/load validation
