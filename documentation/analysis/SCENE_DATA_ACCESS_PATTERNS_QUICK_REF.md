# Scene Data Access Patterns - Quick Reference

**Date**: December 7, 2025 (Updated)  
**Context**: Quick reference for accessing scene data from default files vs save files  
**Related**: SCENE_DATA_PROVIDER_ARCHITECTURE_ANALYSIS.md

---

## The Unified Access Pattern

**Key Insight**: SceneManager decides data source; Factory/Configurator are source-agnostic

### Overview

```
User Action (New Game OR Load Game)
        ↓
SceneManager::LoadScene(scene_type, from_save)
        ↓
    ┌───────────────┴───────────────┐
    ↓ (default)              ↓ (save)
ISceneDataProvider      ISaveDataProvider
::LoadSceneData()      ::LoadSave() + Extractor
        └───────────────┬───────────────┘
                        ↓
        SceneConfigurationData (unified)
                        ↓
        SceneFactory::CreateScene() (source-agnostic)
                        ↓
        Scene::Configure() (source-agnostic)
                        ↓
        Fully Configured Scene
```

---

### For Default Scenes (New Game)

**When**: Starting a new game, loading template scene

**Data File Location**:
```
data/scenes/title.scene_data.bin
data/scenes/crafting.scene_data.bin
```

**SceneManager Code**:
```cpp
// SceneManager - decides to use default provider
std::expected<uuids::uuid, FailInfo> LoadScene(SceneType type, bool from_save = false) {
  SceneConfigurationData config;
  
  if (!from_save) {
    // Load from default template
    ISceneDataProvider& provider = GetSceneDataProvider();
    auto result = provider.LoadSceneData(type);
    if (!result) return std::unexpected(result.error());
    config = result.value();
  }
  
  // Create and configure (source-agnostic)
  return CreateAndConfigureScene(config);
}
```

**Key Types**:
- `ISceneDataProvider` - Interface for loading default scenes
- `SceneConfigurationData` - Unified configuration struct
- `SceneManager::LoadScene()` - Single entry point
- `SceneFactory::CreateScene()` - Source-agnostic creation
- `Scene::Configure()` - Source-agnostic configuration

---

### For Saved Scenes (Load Game)

**When**: Loading a saved game, continuing from save file

**Data File Location**:
```
saves/slot_0.save.bin
saves/slot_1.save.bin
saves/slot_2.save.bin
```

**SceneManager Code**:
```cpp
// SceneManager - decides to use save provider + extractor
std::expected<uuids::uuid, FailInfo> LoadScene(SceneType type, bool from_save, 
                                                 uint32_t save_slot = 0) {
  SceneConfigurationData config;
  
  if (from_save) {
    // Load from save file
    ISaveDataProvider& save_provider = GetSaveDataProvider();
    auto save_result = save_provider.LoadSave(save_slot);
    if (!save_result) return std::unexpected(save_result.error());
    
    // Extract configuration for this scene
    SceneDataExtractor extractor;
    config = extractor.ExtractConfiguration(save_result.value(), type);
  }
  
  // Create and configure (source-agnostic - same code as default!)
  return CreateAndConfigureScene(config);
}
```

**Key Types**:
- `ISaveDataProvider` - Interface for loading save files
- `SaveData` - Complete saved game state (includes scene states)
- `SceneDataExtractor` - Converts nested save data to flat configuration
- `SceneConfigurationData` - Unified configuration struct (same as default!)
- `SceneManager::LoadScene()` - Single entry point
- `SceneFactory::CreateScene()` - Source-agnostic creation (same as default!)
- `Scene::Configure()` - Source-agnostic configuration (same as default!)

---

## Data Structure Comparison

### SceneData (Flat, for Configuration)

```cpp
struct SceneData {
  SceneType scene_type;           // Enum (TITLE, CRAFTING, etc.)
  std::string scene_id;           // Unique identifier
  uint32_t render_texture_width;  // Render dimensions
  uint32_t render_texture_height;
};
```

**Purpose**: Scene metadata for creation and render texture setup

**Used By**: 
- `SceneFactory` (scene creation)
- Both default and save paths

---

### SaveData (Nested, Complete Game State)

```cpp
struct SaveData {
  // Save file metadata
  struct Metadata {
    std::string save_name;
    std::string created_at;
    std::string last_modified;
    std::string game_version;
    uint64_t play_time_seconds;
    uint32_t slot_index;
  } metadata;
  
  // Current scene player is in
  SceneType current_scene_type;
  
  // Complete state for all scenes in save
  std::vector<SceneStateData> scene_states;
  
  // Version for migration
  uint32_t version;
};
```

**Purpose**: Complete saved game state

**Contains**: Multiple scene states, current scene, metadata

**Used By**:
- `ISaveDataProvider` (load/save operations)
- `SceneDataExtractor` (extract scene data)

---

### SceneStateData (Full Scene State)

```cpp
struct SceneStateData {
  // Scene metadata (same as SceneData)
  SceneType scene_type;
  std::string scene_id;
  uint32_t render_texture_width;
  uint32_t render_texture_height;
  
  // Complete scene state
  EntityCollectionData entity_data;  // All entity states
  // Future: Logic states, camera position, etc.
};
```

**Purpose**: Complete state for a single scene

**Used By**:
- `SceneFactory::CreateSceneFromSave()` (scene creation)
- `Scene::ConfigureFromSave()` (entity restoration)

---

## SceneDataExtractor

**Purpose**: Adapt nested SaveData to flat SceneData/SceneStateData

### Method 1: ExtractSceneMetadata

**Input**: `SaveData` (complete save file)  
**Output**: `SceneData` (flat metadata)

```cpp
SceneData SceneDataExtractor::ExtractSceneMetadata(const SaveData& save) const {
  // Get current scene from save
  const auto& scene_state = save.scene_states[0];  // Or find by current_scene_type
  
  // Extract metadata only
  SceneData data;
  data.scene_type = scene_state.scene_type;
  data.scene_id = scene_state.scene_id;
  data.render_texture_width = scene_state.render_texture_width;
  data.render_texture_height = scene_state.render_texture_height;
  
  return data;
}
```

**Usage**: Scene creation (need dimensions, type, ID only)

### Method 2: ExtractSceneState

**Input**: `SaveData` + scene index  
**Output**: `SceneStateData` (complete state)

```cpp
SceneStateData SceneDataExtractor::ExtractSceneState(
    const SaveData& save, 
    uint32_t scene_index) const {
  // Return full scene state
  return save.scene_states[scene_index];
}
```

**Usage**: Scene configuration (need full entity data, etc.)

---

## Implementation Status

| Component | Status | Notes |
|-----------|--------|-------|
| `ISceneDataProvider` | ✅ Implemented | Loads default scene metadata |
| `SceneData` | ✅ Defined | Flat metadata struct |
| `FlatbuffersSceneDataProvider` | ⚠️ Partial | Needs full implementation |
| `ISaveDataProvider` | ✅ Implemented | Interface exists |
| `SaveData` | ⚠️ Partial | Missing scene_states field |
| `SceneStateData` | ❌ Not Defined | Needs to be added to SaveData |
| `SceneDataExtractor` | ❌ Not Implemented | Future Phase 2 |
| `SceneFactory::CreateDefaultScene()` | ✅ Implemented | Works for default scenes |
| `SceneFactory::CreateSceneFromSave()` | ❌ Not Implemented | Future Phase 5 |
| `Scene::ConfigureFromDefault()` | ✅ Implemented | Loads default entities |
| `Scene::ConfigureFromSave()` | ❌ Not Implemented | Future Phase 5 |

---

## Decision Tree

### "Which provider should I use?"

```
Q: Are you loading a saved game?
├─ YES → Use ISaveDataProvider
│         └─ Load SaveData
│             └─ Extract with SceneDataExtractor
│                 └─ Create with CreateSceneFromSave()
│
└─ NO → Are you starting a new game?
         └─ YES → Use ISceneDataProvider
                   └─ Load SceneData
                       └─ Create with CreateDefaultScene()
```

### "Which configuration method should I call?"

```
Q: Did the scene come from a save file?
├─ YES → scene->ConfigureFromSave(scene_state)
│
└─ NO → scene->ConfigureFromDefault()
```

### "Do I need SceneData or SceneStateData?"

```
Q: What do you need?
├─ Only metadata (dimensions, type, ID)?
│   └─ Use SceneData
│       └─ ExtractSceneMetadata() OR LoadSceneData()
│
└─ Complete scene state (entities, logic)?
    └─ Use SceneStateData
        └─ ExtractSceneState()
```

---

## Code Examples

### Example 1: Load Title Scene (New Game)

```cpp
void GameEngine::StartNewGame() {
  // Use default scene provider
  ISceneDataProvider& provider = GetSceneDataProvider();
  
  // Load title scene
  auto uuid = m_scene_manager.LoadTitleScene();
  
  // Scene is created and configured from defaults
  // Ready to play!
}
```

### Example 2: Load Game from Save Slot

```cpp
void GameEngine::LoadGame(uint32_t slot_index) {
  // 1. Load save file
  ISaveDataProvider& save_provider = GetSaveDataProvider();
  auto save_result = save_provider.LoadSave(slot_index);
  if (!save_result) {
    ShowError("Failed to load save");
    return;
  }
  const SaveData& save = save_result.value();
  
  // 2. Clear current scenes
  m_scene_manager.ClearAllScenes();
  
  // 3. Recreate scene from save
  SceneDataExtractor extractor;
  SceneData scene_data = extractor.ExtractSceneMetadata(save);
  SceneStateData scene_state = extractor.ExtractSceneState(save, 0);
  
  // 4. Create and configure scene
  SceneFactory factory;
  auto scene = factory.CreateSceneFromSave(
      scene_data, scene_state, m_game_context);
  
  // 5. Add to manager
  m_scene_manager.AddScene(std::move(scene));
  
  // 6. Continue playing from save state!
}
```

### Example 3: Save Current Game

```cpp
void GameEngine::SaveGame(uint32_t slot_index, const std::string& save_name) {
  // 1. Capture current game state
  SaveData save;
  save.metadata.slot_index = slot_index;
  save.metadata.save_name = save_name;
  save.metadata.created_at = GetCurrentTimestamp();
  save.metadata.play_time_seconds = m_play_time_counter;
  save.version = 1;
  
  // 2. Capture current scene state
  const auto& scenes = m_scene_manager.GetScenes();
  for (const auto& [uuid, scene_ptr] : scenes) {
    SceneStateData scene_state = scene_ptr->CaptureState();
    save.scene_states.push_back(scene_state);
    
    if (scene_ptr->GetActive()) {
      save.current_scene_type = scene_ptr->GetSceneInfo().type;
    }
  }
  
  // 3. Save to file
  ISaveDataProvider& save_provider = GetSaveDataProvider();
  auto result = save_provider.SaveGame(save);
  
  if (result.has_value()) {
    ShowMessage("Game saved successfully!");
  }
}
```

---

## Anti-Patterns to Avoid

### ❌ Don't Mix Providers

```cpp
// BAD: Using both providers for the same operation
ISceneDataProvider& scene_provider = GetSceneDataProvider();
ISaveDataProvider& save_provider = GetSaveDataProvider();
auto scene_data = scene_provider.LoadSceneData(type);  // Confusing!
auto save = save_provider.LoadSave(0);                 // Which one?
```

**Fix**: Choose one path (default OR save)

### ❌ Don't Pass FlatBuffers Types

```cpp
// BAD: FlatBuffers leaking out
const SceneDataFbs* fb_data = loader.ProvideDefaultSceneData(type);
ConfigureScene(fb_data);  // ❌ FlatBuffers in game code
```

**Fix**: Convert to native struct at provider boundary

### ❌ Don't Create Intermediate Structs with FlatBuffers

```cpp
// BAD: Intermediate struct containing FlatBuffers
struct SceneConfigData {
  const SceneDataFbs* fb_scene;  // ❌ FlatBuffers pointer
  SceneType type;
};
```

**Fix**: Use native structs only (SceneData, SceneStateData)

### ❌ Don't Manually Extract Scene Data

```cpp
// BAD: Manual extraction in game code
const SaveData& save = ...;
SceneData data;
data.scene_type = save.scene_states[0].scene_type;
data.scene_id = save.scene_states[0].scene_id;
// ... manual copying
```

**Fix**: Use SceneDataExtractor for consistent extraction

---

## Provider Factory Pattern

**Centralized Access**:

```cpp
// provider_factory.h/cpp
ISceneDataProvider& GetSceneDataProvider();
ISaveDataProvider& GetSaveDataProvider();
IEngineDataProvider& GetEngineDataProvider();
// ... other providers

// Implementation
ISceneDataProvider& GetSceneDataProvider() {
  static FlatbuffersSceneDataProvider provider;
  return provider;
}

ISaveDataProvider& GetSaveDataProvider() {
  static FlatbuffersSaveDataProvider provider;
  return provider;
}
```

**Benefits**:
- ✅ Single access point
- ✅ Easy to swap implementations (change factory only)
- ✅ Singleton pattern for stateless providers
- ✅ Consistent across codebase

---

## Summary

### Default Scene Path (New Game)
```
ISceneDataProvider → SceneData → CreateDefaultScene() → ConfigureFromDefault()
```

### Saved Scene Path (Load Game)
```
ISaveDataProvider → SaveData → SceneDataExtractor → SceneData + SceneStateData 
→ CreateSceneFromSave() → ConfigureFromSave()
```

### Key Insight

> Both paths converge at Scene creation, using the **same SceneData struct** for metadata. The only difference is the **source** (default file vs save file) and the **configuration method** (default entities vs saved entities).

### Unified Architecture

- ✅ One interface per data domain
- ✅ Native structs at API boundaries
- ✅ Extractor pattern for nested data
- ✅ Polymorphism for format abstraction
- ✅ Overloads for source differentiation
- ✅ Factory pattern for provider access
- ✅ Same configuration pipeline regardless of source

---

**Document Status**: Quick Reference Complete
