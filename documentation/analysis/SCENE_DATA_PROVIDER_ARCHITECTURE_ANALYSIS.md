# Scene Data Provider Architecture Analysis

**Date**: December 7, 2025  
**Context**: Analysis of scene data access patterns for default vs. saved data  
**Related**: SAVE_LOAD_WORKFLOW_ANALYSIS.md, DATA_PROVIDER_SYSTEM.md, PROVIDER_VS_VIEWER_PATTERN.md

---

## Note on Provider vs. Viewer Pattern

**Q: "Why not use ISceneViewer instead of ISceneDataProvider?"**

**Short answer**: Current SceneData is small (64 bytes), so Provider pattern is adequate and simpler. Viewer pattern would be beneficial when scene data grows to include entities/assets (10KB+). See **PROVIDER_VS_VIEWER_PATTERN.md** for detailed comparison.

---

## The Question

How should the provider and viewer classes handle scene data when:
1. **Default scene data** is accessed directly (flat structure)
2. **Saved scene data** is nested within a save file (layered structure)

Without:
- Duplicating provider methods for each access pattern
- Creating complex conditional logic in providers
- Exposing implementation details to consumers

---

## Current Architecture

### Scene Data Structure

```cpp
// src/data_providers/ISceneDataProvider.h
struct SceneData {
  SceneType scene_type{SceneType::SceneType_UNKNOWN};
  std::string scene_id;
  uint32_t render_texture_width{800};
  uint32_t render_texture_height{600};
};

class ISceneDataProvider {
  virtual std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType scene_type) const = 0;
};
```

### Save Data Structure

```cpp
// src/configuration/ISaveDataProvider.h
struct SaveData {
  struct Metadata {
    std::string save_name;
    std::string created_at;
    std::string last_modified;
    std::string game_version;
    uint64_t play_time_seconds{0};
    uint32_t slot_index{0};
  } metadata;
  
  SceneType current_scene_type{SceneType::SceneType_UNKNOWN};
  uint32_t version{1};
  
  // Note: Scene state data not yet implemented
  // Future: std::vector<SceneStateData> scene_states;
};

class ISaveDataProvider {
  virtual std::expected<SaveData, FailInfo>
  LoadSave(uint32_t slot_index) const = 0;
};
```

### FlatBuffers Schema Structure

```fbs
// Default scene data (direct access)
// src/flatbuffers_headers/scenes/scene_data.fbs
table SceneDataFbs {
  scene_info: SceneInfoFbs;
  scene_resources: SceneResourcesFbs;
  assets: AssetCollection;
  entity_collection: EntityCollection;
  logic_collection_data: LogicCollectionData;
}

// Saved scene data (nested in save file)
// src/flatbuffers_headers/configuration/save_data.fbs
table SaveData {
  metadata: SaveMetadata;
  current_scene_type: SceneType;
  // Future: scene_states: [SceneStateData];
}

// Future nested structure
table SceneStateData {
  scene_id: string;
  scene_type: SceneType;
  entity_collection: EntityCollection;  // Nested!
}
```

---

## The Two Access Patterns

### Pattern 1: Direct Access (Default Data)

**Use Case**: Loading a new game or default scene configuration

**Data Flow**:
```
File: data/scenes/title.scene_data.bin
  ↓
FlatbuffersDataLoader.ProvideDefaultSceneData(SceneType)
  ↓
ISceneDataProvider.LoadSceneData(SceneType)
  ↓
SceneData struct (direct, no nesting)
```

**Code Example**:
```cpp
// Current implementation
ISceneDataProvider& provider = GetSceneDataProvider();
auto result = provider.LoadSceneData(SceneType::SceneType_TITLE);
if (result.has_value()) {
  const SceneData& data = result.value();
  // Use scene data directly
  uint32_t width = data.render_texture_width;
}
```

**Characteristics**:
- ✅ Simple, flat access
- ✅ No nested navigation required
- ✅ Single method call to get scene data
- ✅ Fast and efficient

---

### Pattern 2: Nested Access (Saved Data)

**Use Case**: Loading a saved game where scene data is part of the save file

**Data Flow**:
```
File: saves/slot_0/save_data.bin
  ↓
ISaveDataProvider.LoadSave(slot_index)
  ↓
SaveData struct
  ├─ metadata
  ├─ current_scene_type
  └─ scene_states (future) ← Array of scenes!
       └─ SceneStateData
            └─ entity_collection (nested)
```

**Code Example (Future)**:
```cpp
// Future implementation when scene states are added
ISaveDataProvider& save_provider = GetSaveDataProvider();
auto save_result = save_provider.LoadSave(0);
if (save_result.has_value()) {
  const SaveData& save = save_result.value();
  
  // Scene data is nested within save data
  for (const auto& scene_state : save.scene_states) {
    // Need to extract scene data from nested structure
    SceneData scene_data = ExtractSceneData(scene_state);
    // Use scene data
  }
}
```

**Characteristics**:
- ⚠️ Nested structure (SaveData → SceneStateData)
- ⚠️ Multiple scenes in one file
- ⚠️ Requires navigation or extraction
- ⚠️ More complex access pattern

---

## The Conceptual Challenge

### Problem: Same Data, Different Access Paths

The same logical entity (scene configuration) exists in two different structural contexts:

**Context 1: Default Data (Standalone)**
```
SceneDataFbs (root)
├─ scene_info
├─ scene_resources
├─ assets
├─ entity_collection
└─ logic_collection_data
```

**Context 2: Saved Data (Nested)**
```
SaveData (root)
├─ metadata
├─ current_scene_type
└─ scene_states[]
     └─ SceneStateData
          ├─ scene_id
          ├─ scene_type
          └─ entity_collection  ← Same data as default, but nested!
```

### Key Insight: Two Different Responsibilities

1. **ISceneDataProvider**: Provides default/template scene configuration
   - Loads from `data/scenes/*.scene_data.bin`
   - Returns fresh, default scene configuration
   - Used for new games, resetting scenes, templates

2. **ISaveDataProvider**: Provides saved game state
   - Loads from `saves/slot_X/save_data.bin`
   - Returns saved scene states (future)
   - Used for loading saved games

**These are separate concerns that should remain separate.**

---

## Solution 1: Separate Providers (Current Approach)

### Keep Providers Separate

**Recommendation**: Keep `ISceneDataProvider` and `ISaveDataProvider` as separate interfaces with different responsibilities.

```cpp
// For default scene data (new game)
ISceneDataProvider& scene_provider = GetSceneDataProvider();
auto scene_data = scene_provider.LoadSceneData(SceneType::SceneType_TITLE);

// For saved game data (load game)
ISaveDataProvider& save_provider = GetSaveDataProvider();
auto save_data = save_provider.LoadSave(0);
```

### Pros
- ✅ Clear separation of concerns
- ✅ Simple to understand and use
- ✅ Each provider has one responsibility
- ✅ No conditional logic in providers
- ✅ Easy to test

### Cons
- ⚠️ Consumers need to know which provider to use
- ⚠️ Different code paths for new game vs. load game

---

## Solution 2: Adapter/Extractor Pattern

### Create Extractors for Nested Data

When scene data is nested in saved data, use extractors to convert between formats:

```cpp
namespace steamrot {

/////////////////////////////////////////////////
/// @brief Extract scene data from save state
///
/// Converts nested SceneStateData from save files into
/// the standard SceneData format used by scene loading code.
/////////////////////////////////////////////////
class SceneDataExtractor {
public:
  /////////////////////////////////////////////////
  /// @brief Extract SceneData from SceneStateData
  ///
  /// @param scene_state The saved scene state from a save file
  /// @return SceneData in standard format
  /////////////////////////////////////////////////
  static SceneData ExtractFromSceneState(const SceneStateData& scene_state) {
    SceneData data;
    data.scene_type = scene_state.scene_type;
    data.scene_id = scene_state.scene_id;
    // Extract other fields as needed
    return data;
  }
  
  /////////////////////////////////////////////////
  /// @brief Extract all scene data from save file
  ///
  /// @param save_data The complete save data
  /// @return Vector of scene data for all saved scenes
  /////////////////////////////////////////////////
  static std::vector<SceneData> 
  ExtractAllScenesFromSave(const SaveData& save_data) {
    std::vector<SceneData> scenes;
    // Future implementation when scene_states is added
    // for (const auto& state : save_data.scene_states) {
    //   scenes.push_back(ExtractFromSceneState(state));
    // }
    return scenes;
  }
};

} // namespace steamrot
```

### Usage Example

```cpp
// Load game workflow
class GameLoader {
public:
  void LoadSavedGame(uint32_t slot_index) {
    // 1. Load save data
    ISaveDataProvider& save_provider = GetSaveDataProvider();
    auto save_result = save_provider.LoadSave(slot_index);
    if (!save_result.has_value()) {
      return;
    }
    
    const SaveData& save = save_result.value();
    
    // 2. Extract scene data from nested structure
    auto scene_data_list = SceneDataExtractor::ExtractAllScenesFromSave(save);
    
    // 3. Use extracted scene data with standard scene loading code
    for (const auto& scene_data : scene_data_list) {
      LoadScene(scene_data);  // Same code path as default data!
    }
  }
  
  void LoadNewGame() {
    // 1. Load default scene data
    ISceneDataProvider& scene_provider = GetSceneDataProvider();
    auto scene_result = scene_provider.LoadSceneData(
        SceneType::SceneType_TITLE);
    if (!scene_result.has_value()) {
      return;
    }
    
    const SceneData& scene_data = scene_result.value();
    
    // 2. Use scene data with standard scene loading code
    LoadScene(scene_data);  // Same LoadScene() method!
  }
  
private:
  void LoadScene(const SceneData& scene_data) {
    // Unified scene loading logic
    // Works for both default and saved data
  }
};
```

### Pros
- ✅ Providers stay simple and focused
- ✅ Extraction logic is explicit and testable
- ✅ Same downstream code for both paths
- ✅ Clear conversion point
- ✅ Easy to extend with more extraction methods

### Cons
- ⚠️ Additional extractor class to maintain
- ⚠️ Extraction might copy data (unless using views)

---

## Solution 3: View Pattern (Future Enhancement)

### Use Views for Zero-Copy Navigation

As detailed in SAVE_LOAD_WORKFLOW_ANALYSIS.md, use view interfaces to navigate nested structures:

```cpp
// Abstract view interface
class ISceneDataView {
public:
  virtual ~ISceneDataView() = default;
  
  virtual SceneType GetSceneType() const = 0;
  virtual std::string GetSceneID() const = 0;
  virtual uint32_t GetRenderTextureWidth() const = 0;
  virtual uint32_t GetRenderTextureHeight() const = 0;
  
  // Navigate to nested data
  virtual IEntityCollectionView GetEntityCollection() const = 0;
};

// Adapter for default scene data
class DefaultSceneDataView : public ISceneDataView {
private:
  const SceneDataFbs* m_fb_data;
public:
  DefaultSceneDataView(const SceneDataFbs* data) : m_fb_data(data) {}
  // Implement interface methods
};

// Adapter for saved scene data
class SavedSceneDataView : public ISceneDataView {
private:
  const SceneStateData* m_state_data;
public:
  SavedSceneDataView(const SceneStateData* data) : m_state_data(data) {}
  // Implement interface methods
};
```

### Usage Example

```cpp
// Unified interface for both access patterns
void ConfigureScene(const ISceneDataView& view) {
  // Works for both default and saved data
  SceneType type = view.GetSceneType();
  std::string id = view.GetSceneID();
  
  auto entity_view = view.GetEntityCollection();
  ConfigureEntities(entity_view);
}

// Load from default data
auto default_view = CreateDefaultSceneDataView(scene_type);
ConfigureScene(default_view);

// Load from saved data
auto saved_view = CreateSavedSceneDataView(save_data, scene_index);
ConfigureScene(saved_view);
```

### Pros
- ✅ Zero-copy navigation through nested data
- ✅ Unified interface for all consumers
- ✅ Format-agnostic (FlatBuffers, JSON, XML)
- ✅ Testable with mock views
- ✅ Flexible and extensible

### Cons
- ⚠️ Significant implementation effort (4-6 weeks)
- ⚠️ More interfaces to define
- ⚠️ Buffer lifetime management complexity
- ⚠️ Deferred due to current priorities

---

## Recommended Approach (Current State)

### Phase 1: Separate Providers (Current Implementation) ✅

**Status**: Already implemented

Keep providers separate with clear responsibilities:

```cpp
// For default/new game scenes
ISceneDataProvider& scene_provider = GetSceneDataProvider();
auto scene_data = scene_provider.LoadSceneData(SceneType::SceneType_TITLE);

// For saved game data
ISaveDataProvider& save_provider = GetSaveDataProvider();
auto save_data = save_provider.LoadSave(0);
```

**Rationale**:
- Simple and clear
- Already working
- Minimal complexity
- Easy to understand

---

### Phase 2: Add Extractor Pattern (When Save States Implemented)

**Status**: Future implementation

When `SaveData` is extended to include `scene_states`:

```fbs
table SaveData {
  metadata: SaveMetadata;
  current_scene_type: SceneType;
  scene_states: [SceneStateData];  // Add this
}
```

Add `SceneDataExtractor` class:

```cpp
// src/scenes/SceneDataExtractor.h
class SceneDataExtractor {
public:
  static SceneData ExtractFromSceneState(const SceneStateData& state);
  static std::vector<SceneData> ExtractAllScenesFromSave(const SaveData& save);
};
```

**Usage Pattern**:

```cpp
// High-level game loading code
class GameManager {
  void LoadGame(uint32_t slot_index) {
    auto save = m_save_provider.LoadSave(slot_index).value();
    
    // Extract scene data
    auto scenes = SceneDataExtractor::ExtractAllScenesFromSave(save);
    
    // Load each scene using standard path
    for (const auto& scene_data : scenes) {
      m_scene_manager.LoadScene(scene_data);
    }
  }
  
  void NewGame() {
    // Load default scene
    auto scene_data = m_scene_provider.LoadSceneData(
        SceneType::SceneType_TITLE).value();
    
    // Load scene using same standard path
    m_scene_manager.LoadScene(scene_data);
  }
};

// Scene manager doesn't know or care about data source
class SceneManager {
  void LoadScene(const SceneData& data) {
    // Unified scene loading logic
    // Works for both default and extracted saved data
  }
};
```

**Benefits**:
- Clear separation: providers load, extractors convert, managers use
- SceneManager has single code path for loading
- Easy to test each component independently
- Explicit conversion makes data flow clear

---

### Phase 3: Consider View Pattern (Future Enhancement)

**Status**: Deferred (see SAVE_LOAD_WORKFLOW_ANALYSIS.md)

If needed in the future for performance or architectural reasons, migrate to view pattern as detailed in SAVE_LOAD_WORKFLOW_ANALYSIS.md.

**When to Consider**:
- Save files become very large (megabytes)
- Need to support multiple data formats
- Performance becomes critical
- Architecture needs more flexibility

---

## Summary: Handling Two Access Patterns

### The Pattern Difference

| Aspect | Default Scene Data | Saved Scene Data |
|--------|-------------------|------------------|
| **Structure** | Flat, direct access | Nested in save file |
| **Provider** | ISceneDataProvider | ISaveDataProvider |
| **File Location** | `data/scenes/*.bin` | `saves/slot_X/*.bin` |
| **Use Case** | New game, templates | Load saved game |
| **Access** | Direct SceneData | Extract from SaveData |

### Current Solution (Phase 1)

**Keep providers separate:**
```cpp
// Two different providers for two different data sources
ISceneDataProvider  → LoadSceneData(SceneType)     → SceneData
ISaveDataProvider   → LoadSave(slot_index)         → SaveData
```

### Future Solution (Phase 2)

**Add extraction layer:**
```cpp
ISceneDataProvider  → LoadSceneData(SceneType)     → SceneData
                                                      ↓
                                               LoadScene(SceneData)
                                                      ↑
ISaveDataProvider   → LoadSave(slot_index)         → SaveData
                                                      ↓
                    SceneDataExtractor::Extract()  → SceneData
```

### Key Principles

1. **Separation of Concerns**: Providers load, extractors convert, managers use
2. **Single Responsibility**: Each provider handles one data source
3. **Explicit Conversion**: Use extractors to make data flow clear
4. **Unified Consumption**: Downstream code uses standard SceneData format
5. **Progressive Enhancement**: Start simple, add complexity when needed

---

## Answering the Original Question

### Q: How does this work with provider and viewer classes?

**A**: Use separate providers for separate data sources.

- `ISceneDataProvider`: Loads default scene data (flat)
- `ISaveDataProvider`: Loads saved game data (nested)
- When needed, use extractors to convert saved → default format

### Q: Sometimes I can get scene data straight away?

**A**: Yes, when loading default data.

```cpp
ISceneDataProvider& provider = GetSceneDataProvider();
auto scene_data = provider.LoadSceneData(SceneType::SceneType_TITLE);
// Use scene_data directly - no nesting
```

### Q: Sometimes it will be nested?

**A**: Yes, when loading saved data.

```cpp
ISaveDataProvider& provider = GetSaveDataProvider();
auto save_data = provider.LoadSave(0);
// Scene data is nested in save_data.scene_states (future)
// Use extractor to convert: SceneDataExtractor::Extract(save_data)
```

### Q: Default scene data is just scene data, saved is layered?

**A**: Correct. Use appropriate provider for each:

```cpp
// Default: Direct access
SceneData default_data = scene_provider.LoadSceneData(type);

// Saved: Extract from nested structure
SaveData save = save_provider.LoadSave(slot);
std::vector<SceneData> saved_scenes = 
    SceneDataExtractor::ExtractAllScenesFromSave(save);
```

---

## Implementation Checklist (Future)

When implementing scene state saving:

- [ ] Extend `SaveData` struct with `scene_states` field
- [ ] Update `save_data.fbs` schema with `SceneStateData` table
- [ ] Create `SceneDataExtractor` class
- [ ] Add `ExtractFromSceneState()` method
- [ ] Add `ExtractAllScenesFromSave()` method
- [ ] Update `GameManager` to use extractor in load path
- [ ] Write tests for extraction logic
- [ ] Document in DATA_PROVIDER_SYSTEM.md

---

## Related Documentation

- [Abstract Factory Pattern](ABSTRACT_SCENE_FACTORY_PATTERN.md) - Analysis of AbstractSceneFactory approach
- [Provider vs Viewer Pattern](PROVIDER_VS_VIEWER_PATTERN.md) - Why Provider pattern chosen over Viewer
- [Save/Load Workflow Analysis](SAVE_LOAD_WORKFLOW_ANALYSIS.md) - View pattern details
- [Data Provider System](../DATA_PROVIDER_SYSTEM.md) - Current provider architecture
- [Save Data Configuration](../configuration/SAVE_DATA.md) - Save file structure
- [Engine Data Organization](../proposals/ENGINE_DATA_ORGANIZATION.md) - Data categories

---

**Analysis Complete**: December 7, 2025
