# SaveData Implementation - Executive Summary

## Overview

This document provides a quick reference summary for the SaveData implementation analysis. For complete details, see [SAVE_DATA_IMPLEMENTATION_ANALYSIS.md](SAVE_DATA_IMPLEMENTATION_ANALYSIS.md).

## Key Questions Answered

### 1. What should SaveData contain?

**Includes:**
- ✅ Scene collection state (entities, scene-specific configuration)
- ✅ Global game progression (current scene, progression flags, counters)
- ✅ Hybrid resources (Grimoire blueprints, persistent collections)
- ✅ Save metadata (name, timestamps, version, play time)

**Excludes:**
- ❌ User preferences (display, audio settings) - handled separately by UserPreferencesData
- ❌ Gameplay-agnostic configuration (window size, framerate) - in EngineConfig
- ❌ Static game data (asset definitions, default entity templates)

### 2. Where should save/load be managed? (Integration Level)

**Answer: Engine Level** ✅

**Rationale:**
- Has complete view of game state (all scenes, progression, timers)
- Natural lifecycle integration (save on exit, load on startup)
- Consistent with other engine-level systems (EngineConfig, EngineState)
- Can coordinate scene serialization through SceneManager
- Clear ownership of save file management

**Architecture:**
```
Engine
  └─ SaveDataManager
       ├─ SaveGame(slot)
       ├─ LoadGame(slot)
       └─ ListSaves()
  └─ SceneManager
       ├─ GetSceneSaveData() → [SavedSceneData]
       └─ RestoreFromSaveData([SavedSceneData])
  └─ Scene(s)
       └─ EntityMemoryPool (actual game entities)
```

### 3. How to handle extensibility?

**FlatBuffers Schema Evolution:**
- New fields can be added without breaking existing saves
- Use optional fields with default values
- KeyValuePair tables for rapid prototyping

**Example - Adding Tiling Display Manager:**
```fbs
table SavedSceneData {
  scene_type: SceneType (required);
  entity_collection: EntityCollectionFbs (required);
  // ... existing fields ...
  
  // New field - old saves will have this as null
  tile_display_config: TileDisplayConfig;  // Added later
}
```

**Handling in code:**
```cpp
if (saved_scene_data->tile_display_config()) {
  scene.ConfigureTileDisplay(...);  // New feature
} else {
  scene.InitializeDefaultTileDisplay();  // Backward compatibility
}
```

### 4. How to handle hybrid data? (e.g., Grimoire)

**Hybrid Resources** = Data that players may want to persist across multiple saves (blueprints, unlocked content)

**Recommended Approach:**
1. **Default: Per-Save Storage**
   - Each save contains its own copy of hybrid data
   - Simplest implementation, clearest ownership
   - Stored in scene's entity collection (CGrimoireMachina component)

2. **Optional: Shared Resources**
   - Add HybridResourcesData section to SaveData root
   - Extract shareable parts (blueprint library) to shared section
   - Each save can reference or merge from shared pool

3. **Configuration Toggle**
   ```fbs
   table GameplayConfig {
     grimoire_sharing_enabled: bool = false;
   }
   ```
   - Designer can enable/disable sharing
   - No code changes needed for different game modes

## Proposed Schema (High-Level)

```fbs
table SaveData {
  metadata: SaveMetadata (required);
  game_progression: GameProgressionData (required);
  saved_scenes: [SavedSceneData] (required);
  hybrid_resources: HybridResourcesData;  // Optional
}

table GameProgressionData {
  current_scene_type: SceneType;
  total_play_time_seconds: uint64;
  progression_flags: [string];
  counters: [KeyValuePair];
}

table SavedSceneData {
  scene_type: SceneType (required);
  entity_collection: EntityCollectionFbs (required);
  scene_flags: [string];
  scene_data: [KeyValuePair];
}

table HybridResourcesData {
  grimoire_data: GrimoireMachinaData;
  // Future: other persistent resources
}
```

## Implementation Phases

### Phase 1: Schema and Native Types (Week 1)
- Update save_data.fbs
- Create SaveData.h native struct
- Add unit tests

### Phase 2: Save/Load Infrastructure (Week 2)
- Create SaveDataManager class
- Create SaveDataConfigurator
- Add file I/O utilities

### Phase 3: Engine Integration (Week 2-3)
- Add SaveDataManager to Engine
- Add save/load hooks
- Implement scene serialization

### Phase 4: Hybrid Resources (Week 3-4)
- Implement Grimoire data handling
- Add configuration options
- Handle edge cases

### Phase 5: UI and Polish (Week 4)
- Add save/load UI
- Implement auto-save
- Error handling

## Key Design Decisions

| Decision | Choice | Rationale |
|----------|--------|-----------|
| **Management Level** | Engine | Complete game state access, lifecycle integration |
| **Serialization Format** | FlatBuffers | Already used in codebase, supports schema evolution |
| **Hybrid Data** | Per-save default, optional sharing | Flexible, designer-configurable |
| **Extensibility** | Schema evolution + KeyValuePair | Add fields without breaking saves |
| **Scope Separation** | SaveData ≠ UserPreferences | Clear separation of concerns |

## Success Criteria

SaveData implementation is successful when:
1. ✅ Player can save game at any point
2. ✅ Player can load saved game and continue from saved state
3. ✅ Multiple save slots are supported
4. ✅ Saves are backward compatible with future game versions
5. ✅ Hybrid resources (Grimoire) are handled appropriately
6. ✅ User preferences remain separate from save data

## Next Steps

1. **Review Analysis**: Read full [SAVE_DATA_IMPLEMENTATION_ANALYSIS.md](SAVE_DATA_IMPLEMENTATION_ANALYSIS.md)
2. **Validate Design**: Discuss proposed schema with team
3. **Begin Implementation**: Start with Phase 1 (Schema and Native Types)
4. **Iterate**: Refine based on feedback and testing

## Related Documentation

- **[Full Analysis Document](SAVE_DATA_IMPLEMENTATION_ANALYSIS.md)** - Complete implementation analysis
- **[User Preferences Schema](../../src/types/flatbuffers/configuration/user_preferences.fbs)** - Separate from SaveData
- **[Entity Schemas](../../src/types/flatbuffers/entities/)** - Entity serialization patterns
- **[Copilot Instructions](../../.github/copilot-instructions.md)** - Code style and patterns
