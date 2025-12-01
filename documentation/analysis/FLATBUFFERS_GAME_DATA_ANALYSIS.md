# FlatBuffers Game Data Loading Analysis

## Overview

This document analyzes the FlatBuffers schema organization and data loading patterns in SteamRot, focusing on identifying potential redundancies and ensuring the architecture properly supports both **configuration data** and **save data** use cases.

## Summary of Findings

### Key Findings

| Finding | Severity | Location | Recommendation |
|---------|----------|----------|----------------|
| **Clear separation exists for Resource vs Context data** | Good | `ResourceConfigurator`, `ContextConfigurator` | Current design is sound |
| **GameResourcesData duplicated in EngineData and ContextData** | Medium | `engine_data.fbs`, `context_data.fbs` | Consolidate or clarify purposes |
| **No distinct "Save Data" schema** | Medium | N/A | Add SaveData schema for game progress |
| **SceneResourcesData in multiple locations** | Low | `scene_data.fbs`, `context_data.fbs` | Clarify primary source of truth |
| **Test data system is well-designed and extensible** | Good | `test_data.fbs` | Use as model for save data |
| **EntityCollection supports both config and runtime** | Good | `entities.fbs` | Can be reused for save data |

---

## Current Architecture Overview

### Data Loading Layers

```
┌────────────────────────────────────────────────────────────────────────────────┐
│                          FLATBUFFERS SCHEMAS                                   │
├────────────────────────────────────────────────────────────────────────────────┤
│                                                                                │
│  ┌──────────────────────┐  ┌──────────────────────┐  ┌────────────────────┐   │
│  │   Configuration      │  │    Runtime Data      │  │    Test Data       │   │
│  │      Schemas         │  │      Schemas         │  │     Schemas        │   │
│  ├──────────────────────┤  ├──────────────────────┤  ├────────────────────┤   │
│  │ context_data.fbs     │  │ entities.fbs         │  │ test_data.fbs      │   │
│  │ resource_data.fbs    │  │ scene_data.fbs       │  │ simulation.fbs     │   │
│  │ logic_data.fbs       │  │ engine_data.fbs      │  │ event_test_data.fbs│   │
│  │ scene_manager_data.fbs│ │ event_bus_data.fbs   │  │ input_test_data.fbs│   │
│  └──────────────────────┘  └──────────────────────┘  └────────────────────┘   │
│                                                                                │
└────────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌────────────────────────────────────────────────────────────────────────────────┐
│                          DATA LOADERS                                          │
├────────────────────────────────────────────────────────────────────────────────┤
│                                                                                │
│  ┌──────────────────────────────────────────────────────────────────────────┐ │
│  │                    FlatbuffersDataLoader                                 │ │
│  │  - ProvideContextData()                                                  │ │
│  │  - ProvideEngineData()                                                   │ │
│  │  - ProvideDefaultSceneData(SceneType)                                    │ │
│  │  - ProvideGameResourcesData()                                            │ │
│  │  - ProvideSceneResourcesData(SceneType)                                  │ │
│  │  - ProvideLogicCollectionData(SceneType)                                 │ │
│  │  - ProvideAssetData()                                                    │ │
│  │  - ProvideUIStylesData()                                                 │ │
│  │  - ProvideFragment()                                                     │ │
│  │  - ProvideSceneManagerData()                                             │ │
│  └──────────────────────────────────────────────────────────────────────────┘ │
│                                                                                │
└────────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌────────────────────────────────────────────────────────────────────────────────┐
│                          CONFIGURATORS                                         │
├────────────────────────────────────────────────────────────────────────────────┤
│                                                                                │
│  ┌─────────────────────────┐  ┌──────────────────────────┐                    │
│  │  FlatbuffersConfigurator │  │  resources_configuration │                    │
│  │  (Entity Configuration)  │  │  (Resource Configuration)│                    │
│  ├─────────────────────────┤  ├──────────────────────────┤                    │
│  │ ConfigureEntities        │  │ ConfigureGameResources   │                    │
│  │   FromDefaultData()      │  │ ConfigureSceneResources  │                    │
│  │ ConfigureEntities        │  └──────────────────────────┘                    │
│  │   FromCollection()       │                                                  │
│  └─────────────────────────┘                                                   │
│                                                                                │
└────────────────────────────────────────────────────────────────────────────────┘
```

---

## Schema Categories Analysis

### 1. Configuration Data (Static at Runtime)

**Purpose:** Data that defines the initial game setup, UI layout, resource paths, and system configuration. Typically loaded once at startup and does not change during gameplay.

**Current Schemas:**

| Schema | Purpose | Location |
|--------|---------|----------|
| `context_data.fbs` | Window size, framerate, scene pool sizes | `data/context/context_data.json` |
| `resource_data.fbs` | GameResourcesData, SceneResourcesData | Embedded in `engine_data` and `scene_data` |
| `logic_data.fbs` | Logic class subscriptions per scene | Embedded in `scene_data` |
| `scene_manager_data.fbs` | Scene manager subscriptions | `data/scene_manager/` |
| `ui_style.fbs` | UI visual styling | `data/ui_styles/` |
| `assets.fbs` | Asset collection metadata | `data/asset_manager/` |

**Data Files:**
```
data/
├── context/context_data.json           # Game and scene context config
├── game_engine/engine_data.json        # Engine-level config and resources
├── scene_manager/scene_manager_data.json # Scene manager subscriptions
├── scenes/
│   ├── title.scene_data.json           # Title scene entities and config
│   └── crafting.scene_data.json        # Crafting scene entities and config
├── asset_manager/                       # Asset metadata
└── ui_styles/                          # UI styling definitions
```

---

### 2. Default Scene Data (Initial Game State)

**Purpose:** Defines the default entity configuration for each scene when the game starts fresh. This is the "new game" state.

**Current Schemas:**

| Schema | Purpose | Contents |
|--------|---------|----------|
| `scene_data.fbs` | Root table for scene data | EntityCollection, Assets, Resources, SceneType |
| `entities.fbs` | Entity definitions | EntityData array with component data |
| `user_interface.fbs` | UI component data | Panel, Button, DropDown hierarchies |
| `grimoire_machina.fbs` | Game-specific component | Fragment references |
| `ui_state.fbs` | UI state mappings | Visibility state configurations |

**Example Flow:**
```
Scene Load (New Game)
         │
         ▼
┌────────────────────────┐
│ ProvideDefaultSceneData│
│    (SceneType)         │
└────────────────────────┘
         │
         ▼
┌────────────────────────┐
│ ConfigureEntities      │
│   FromDefaultData()    │
└────────────────────────┘
         │
         ▼
┌────────────────────────┐
│ EntityMemoryPool with  │
│ configured components  │
└────────────────────────┘
```

---

### 3. Test Data (Development/Testing)

**Purpose:** Provides data-driven test scenarios with entity states, simulation steps, inputs, and expected outcomes.

**Current Schemas:**

| Schema | Purpose | Contents |
|--------|---------|----------|
| `test_data.fbs` | Test configuration root | Metadata, entity collections, simulation data |
| `simulation.fbs` | Simulation steps | Logic classes, functions, execution order |
| `event_test_data.fbs` | Event testing | Event sequences for injection |
| `input_test_data.fbs` | Input testing | Input sequences for injection |
| `event_bus_data.fbs` | Event bus state | EventPacket snapshots |
| `engine_data.fbs` | Engine state snapshot | Full engine state for testing |

**Key Design Pattern:**
```fbs
table TestDataConfig {
  metadata: TestMetadata (required);         # Test identification
  starting_engine_state: EngineData;         # Initial state
  simulation_data: SimulationData;           # Steps to execute
  num_ticks: uint32;                         # Duration
  tick_snapshots: [TickSnapshot];            # Intermediate validation
}
```

**Strengths:**
- ✅ Extensible design with optional fields
- ✅ Metadata support for categorization
- ✅ Supports both start and expected states
- ✅ Tick-based simulation modeling
- ✅ Clean separation from production data

---

## Identified Redundancies and Gaps

### Redundancy 1: GameResourcesData Duplication

**Location:**
- `engine_data.fbs` includes `game_resources: GameResourcesData`
- `context_data.fbs` has `GameContextConfig` with overlapping fields

**Current State:**
```fbs
// resource_data.fbs
table GameResourcesData {
  window_width: uint32 = 800;
  window_height: uint32 = 600;
  window_title: string;
  framerate_limit: uint32 = 60;
}

// context_data.fbs
table GameContextConfig {
  window_width: uint32 = 800;
  window_height: uint32 = 600;
  window_title: string;
  framerate_limit: uint32 = 60;
}
```

**Data Files:**
```json
// engine_data.json
{
  "game_resources": {
    "window_width": 800,
    "window_height": 600,
    "window_title": "SteamRot",
    "framerate_limit": 60
  }
}

// context_data.json
{
  "game_context": {
    "window_width": 800,
    "window_height": 600,
    "window_title": "SteamRot",
    "framerate_limit": 60
  }
}
```

**Impact:**
- Same data defined in two places
- Risk of inconsistency if one is updated but not the other
- Unclear which is the source of truth

**Recommendation:**
Choose one as the authoritative source:
- **Option A:** Keep `context_data.json` as the primary configuration source, remove `game_resources` from `engine_data.json`
- **Option B:** Keep `engine_data.json` as the primary source (since EngineData is more comprehensive), have `ContextConfigurator` read from it

---

### Redundancy 2: SceneResourcesData Multiple Sources

**Location:**
- `scene_data.fbs` → `SceneDataData.scene_resources`
- `context_data.fbs` → `SceneContextConfig` (render_texture_width/height)

**Current State:**
```json
// title.scene_data.json
{
  "scene_resources": {
    "render_texture_width": 800,
    "render_texture_height": 600
  }
}

// context_data.json
{
  "scene_contexts": [{
    "scene_type": "TITLE",
    "render_texture_width": 800,
    "render_texture_height": 600
  }]
}
```

**Impact:**
- Same render texture dimensions in two places
- `ProvideSceneResourcesData()` loads from `scene_data.json`
- But `ContextConfigurator` might use `context_data.json`

**Recommendation:**
- Document clearly which source is authoritative
- Consider removing one source to prevent inconsistency
- Suggest keeping scene-specific resources in `scene_data.json` and removing from `context_data.json`

---

### Gap 1: No Save Data Schema

**Current State:**
- No dedicated schema for saving/loading game progress
- EntityCollection can represent entity state but lacks:
  - Player progress metadata
  - Current scene identifier
  - Game time/timestamps
  - Achievement/unlock state

**Recommendation:**
Create a new `save_data.fbs` schema:

```fbs
// Proposed: save_data.fbs
namespace steamrot;

include "entities.fbs";
include "scene_types.fbs";
include "event_bus_data.fbs";

table SaveMetadata {
  save_name: string;
  created_at: string;          // ISO 8601 timestamp
  last_modified: string;
  game_version: string;
  play_time_seconds: uint64;
}

table SaveData {
  metadata: SaveMetadata (required);
  
  current_scene_type: SceneType;
  
  // Full scene states (only populated scenes)
  scene_states: [SceneStateData];
  
  // Global game state
  global_event_bus: EventBusData;
}

table SceneStateData {
  scene_type: SceneType;
  entity_collection: EntityCollection;
}

root_type SaveData;
```

**Benefits:**
- Clear separation from default/configuration data
- Reuses existing EntityCollection schema
- Supports multiple scenes' states
- Includes metadata for save management
- Follows existing test_data.fbs patterns

---

### Gap 2: No Distinction Between "Default" and "Runtime" Entity Data

**Current State:**
- `EntityCollection` used for both:
  - Default scene data (loaded from JSON)
  - Test data (starting/expected states)
- No explicit marker distinguishing "default" vs "saved" data

**Recommendation:**
Consider adding a flag or wrapper:

```fbs
// Option: Add to EntityCollection
table EntityCollection {
  entities: [EntityData] (required);
  entity_memory_pool_size: int;
  data_source: DataSourceType;  // NEW: Default, Saved, Test
}

enum DataSourceType : byte {
  Default = 0,    // From scene_data JSON
  Saved = 1,      // From save file
  Test = 2        // From test_data
}
```

This helps debugging and ensures proper handling of different data sources.

---

## Architecture Recommendations

### 1. Configuration vs Runtime Data Flow

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                        RECOMMENDED DATA FLOW                                    │
└─────────────────────────────────────────────────────────────────────────────────┘

                    ┌─────────────────────────────────────────┐
                    │           CONFIGURATION DATA            │
                    │  (Loaded once, immutable at runtime)    │
                    ├─────────────────────────────────────────┤
                    │ • context_data.json → Window, framerate │
                    │ • ui_styles/*.json → UI themes          │
                    │ • scene_manager_data.json → Subs        │
                    └─────────────────────────────────────────┘
                                        │
                                        │ Load at startup
                                        ▼
                    ┌─────────────────────────────────────────┐
                    │          DEFAULT SCENE DATA             │
                    │  (Templates for new game/scene load)    │
                    ├─────────────────────────────────────────┤
                    │ • scenes/title.scene_data.json          │
                    │ • scenes/crafting.scene_data.json       │
                    │ • Contains default EntityCollection     │
                    └─────────────────────────────────────────┘
                                        │
                      ┌─────────────────┴─────────────────┐
                      ▼                                   ▼
        ┌───────────────────────┐          ┌───────────────────────┐
        │      NEW GAME         │          │     LOAD GAME         │
        ├───────────────────────┤          ├───────────────────────┤
        │ Load default entities │          │ Load from save_data   │
        │ from scene_data.json  │          │ Restore entity states │
        └───────────────────────┘          └───────────────────────┘
                      │                                   │
                      └─────────────────┬─────────────────┘
                                        ▼
                    ┌─────────────────────────────────────────┐
                    │           RUNTIME STATE                 │
                    │  (EntityMemoryPool, EventBus, etc.)     │
                    ├─────────────────────────────────────────┤
                    │ • Entities modified during gameplay     │
                    │ • Events processed and cleared          │
                    │ • Can be saved to save_data             │
                    └─────────────────────────────────────────┘
                                        │
                                        │ Save action
                                        ▼
                    ┌─────────────────────────────────────────┐
                    │            SAVE DATA                    │
                    │  (Serialized runtime state)             │
                    ├─────────────────────────────────────────┤
                    │ • saves/slot1.save_data.json            │
                    │ • Contains modified EntityCollection    │
                    │ • Includes save metadata                │
                    └─────────────────────────────────────────┘
```

### 2. Consolidate Resource Configuration

**Recommended Source of Truth:**

| Data Type | Primary Source | Remove From |
|-----------|----------------|-------------|
| Window/Framerate | `context_data.json` | `engine_data.json.game_resources` |
| Scene Render Texture | `scenes/*.scene_data.json` | `context_data.json.scene_contexts` |
| Entity Pool Size | `context_data.json.scene_contexts` | Keep (no duplication) |

**Updated engine_data.json:**
```json
{
  "subscriptions": [{ "event_type_data": "EVENT_QUIT_GAME" }]
  // REMOVE: game_resources (now in context_data.json)
}
```

**Updated context_data.json:**
```json
{
  "game_context": {
    "window_width": 800,
    "window_height": 600,
    "window_title": "SteamRot",
    "framerate_limit": 60
  },
  "scene_contexts": [
    {
      "scene_type": "TITLE",
      "entity_pool_size": 50
      // REMOVE: render_texture dimensions (in scene_data.json)
    }
  ]
}
```

### 3. Implement Save/Load System

**Phase 1: Schema Definition**
1. Create `save_data.fbs` with SaveData table
2. Define SaveMetadata for save management
3. Reuse EntityCollection for scene states

**Phase 2: Loader Implementation**
1. Add `ProvideSaveData(slot)` to FlatbuffersDataLoader
2. Add `SaveGameData(slot)` for serialization
3. Implement SaveManager class

**Phase 3: Integration**
1. Add save/load UI in game
2. Hook into scene transitions
3. Add autosave support

---

## Schema Relationship Diagram

```
                    ┌─────────────────────────────────┐
                    │         ROOT SCHEMAS            │
                    └─────────────────────────────────┘
                                    │
        ┌───────────────────────────┼───────────────────────────┐
        │                           │                           │
        ▼                           ▼                           ▼
┌───────────────┐         ┌─────────────────┐         ┌─────────────────┐
│ engine_data   │         │  scene_data     │         │   test_data     │
│     .fbs      │         │      .fbs       │         │      .fbs       │
├───────────────┤         ├─────────────────┤         ├─────────────────┤
│ subscriptions │         │ entity_collection│        │ metadata        │
│ scene_manager │──┐      │ assets          │         │ start_entities  │
│ event_buses   │  │      │ scene_resources │         │ expected_entities│
└───────────────┘  │      │ scene_type      │         │ simulation_data │
                   │      └─────────────────┘         │ tick_snapshots  │
                   │                │                 └─────────────────┘
                   │                │                         │
                   ▼                ▼                         ▼
           ┌─────────────────────────────────────────────────────────────┐
           │                    SHARED SCHEMAS                           │
           ├─────────────────────────────────────────────────────────────┤
           │                                                             │
           │  ┌─────────────┐  ┌─────────────┐  ┌───────────────────┐   │
           │  │ entities.fbs│  │ events.fbs  │  │ subscriber_data   │   │
           │  ├─────────────┤  ├─────────────┤  │      .fbs         │   │
           │  │EntityData   │  │EventType    │  ├───────────────────┤   │
           │  │EntityCollect│  │EventDataData│  │SubscriberData     │   │
           │  └─────────────┘  └─────────────┘  └───────────────────┘   │
           │         │                │                   │             │
           │         ▼                ▼                   │             │
           │  ┌─────────────────────────────┐             │             │
           │  │     COMPONENT SCHEMAS       │◀────────────┘             │
           │  ├─────────────────────────────┤                           │
           │  │ user_interface.fbs          │                           │
           │  │ grimoire_machina.fbs        │                           │
           │  │ ui_state.fbs                │                           │
           │  └─────────────────────────────┘                           │
           │                                                             │
           └─────────────────────────────────────────────────────────────┘
```

---

## Recommendations Summary

### Immediate Actions (Low Risk)

1. **Document Source of Truth**
   - Update RESOURCE_CONFIGURATION.md to clarify primary sources
   - Add comments in JSON files indicating purpose

2. **Standardize Naming**
   - `game_context` vs `game_resources` - pick one naming convention
   - Document the distinction between "config" and "resource"

### Short-Term Actions (Medium Risk)

3. **Remove GameResourcesData from EngineData**
   - Keep window/framerate config only in `context_data.json`
   - Update `FlatbuffersDataLoader::ProvideGameResourcesData()` to use ContextData

4. **Remove render texture dimensions from SceneContextConfig**
   - Keep scene resource dimensions only in `scene_data.json`
   - Update ContextConfigurator accordingly

### Long-Term Actions (New Features)

5. **Create Save Data System**
   - Define `save_data.fbs` schema
   - Implement SaveManager class
   - Add serialization for runtime entity state

6. **Add Data Source Tracking**
   - Add `DataSourceType` enum to EntityCollection
   - Helps debugging and ensures proper data handling

---

## File Reference

### Schema Files
- `src/flatbuffers_headers/context_data.fbs`
- `src/flatbuffers_headers/resource_data.fbs`
- `src/flatbuffers_headers/engine_data.fbs`
- `src/flatbuffers_headers/scene_data.fbs`
- `src/flatbuffers_headers/entities.fbs`
- `src/flatbuffers_headers/test_data.fbs`

### Data Files
- `data/context/context_data.json`
- `data/game_engine/engine_data.json`
- `data/scenes/*.scene_data.json`

### Loader/Configurator Code
- `src/data_handlers/FlatbuffersDataLoader.h/cpp`
- `src/entity/FlatbuffersConfigurator.h/cpp`
- `src/resources/resources_configuration.h/cpp`

### Documentation
- `documentation/configuration/CONTEXT_CONFIGURATION.md`
- `documentation/configuration/RESOURCE_CONFIGURATION.md`
- `documentation/configuration/RESOURCE_CONTEXT_ARCHITECTURE.md`
- `documentation/testing/TEST_DATA_CONFIGURATION.md`

---

## Related Analysis Documents

- `EVENT_PACKET_CREATION_ANALYSIS.md` - Event system analysis
- `PATH_PROVIDER_NAMESPACE_ANALYSIS.md` - Path provider analysis
- `TEST_DATA_VISUALIZATION_REPORT.md` - Test data system analysis
