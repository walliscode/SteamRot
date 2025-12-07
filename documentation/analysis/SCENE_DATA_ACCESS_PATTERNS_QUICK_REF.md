# Scene Data Access Patterns - Quick Reference

**Date**: December 7, 2025  
**Context**: Quick reference for handling default vs. saved scene data  
**Full Analysis**: SCENE_DATA_PROVIDER_ARCHITECTURE_ANALYSIS.md

---

## Quick Decision Guide

### When to Use Each Provider

```cpp
// ✅ Use ISceneDataProvider for:
// - New game initialization
// - Default/template scene configuration
// - Scene resets
ISceneDataProvider& provider = GetSceneDataProvider();
auto scene_data = provider.LoadSceneData(SceneType::SceneType_TITLE);

// ✅ Use ISaveDataProvider for:
// - Loading saved games
// - Save file management
// - Saved scene states (future)
ISaveDataProvider& provider = GetSaveDataProvider();
auto save_data = provider.LoadSave(slot_index);
```

---

## The Two Patterns

### Pattern 1: Direct Access (Default Data)

**Characteristics**:
- ✅ Flat, simple structure
- ✅ One method call
- ✅ Fast and efficient

**Code**:
```cpp
ISceneDataProvider& provider = GetSceneDataProvider();
auto result = provider.LoadSceneData(SceneType::SceneType_TITLE);
if (result.has_value()) {
  const SceneData& data = result.value();
  // Use directly - no extraction needed
  LoadScene(data);
}
```

### Pattern 2: Nested Access (Saved Data)

**Characteristics**:
- ⚠️ Nested in save file
- ⚠️ Requires extraction (future)
- ⚠️ Multiple scenes possible

**Code (Future)**:
```cpp
ISaveDataProvider& provider = GetSaveDataProvider();
auto result = provider.LoadSave(0);
if (result.has_value()) {
  const SaveData& save = result.value();
  
  // Extract scene data from nested structure
  auto scenes = SceneDataExtractor::ExtractAllScenesFromSave(save);
  
  // Use extracted data
  for (const auto& scene_data : scenes) {
    LoadScene(scene_data);  // Same method as Pattern 1!
  }
}
```

---

## Recommended Approach

### Current (Phase 1): Separate Providers ✅

```cpp
// Different providers for different data sources
ISceneDataProvider   → LoadSceneData(type)    → SceneData (direct)
ISaveDataProvider    → LoadSave(slot)         → SaveData (contains scenes)
```

**Why**: Simple, clear, already implemented

### Future (Phase 2): Add Extractor Pattern

```cpp
// Add extraction layer when scene states are implemented
ISceneDataProvider   → LoadSceneData(type)    → SceneData
                                                  ↓
                                            LoadScene(SceneData)
                                                  ↑
ISaveDataProvider    → LoadSave(slot)         → SaveData
                                                  ↓
                      SceneDataExtractor      → SceneData
```

**Why**: Unified downstream code, explicit conversion

---

## Common Usage Scenarios

### Scenario 1: New Game

```cpp
void GameManager::NewGame() {
  ISceneDataProvider& provider = GetSceneDataProvider();
  auto scene_data = provider.LoadSceneData(SceneType::SceneType_TITLE).value();
  m_scene_manager.LoadScene(scene_data);
}
```

### Scenario 2: Load Game (Current - No Scene States Yet)

```cpp
void GameManager::LoadGame(uint32_t slot) {
  ISaveDataProvider& provider = GetSaveDataProvider();
  auto save = provider.LoadSave(slot).value();
  
  // Currently only has current_scene_type
  auto scene_data = m_scene_provider.LoadSceneData(
      save.current_scene_type).value();
  m_scene_manager.LoadScene(scene_data);
}
```

### Scenario 3: Load Game (Future - With Scene States)

```cpp
void GameManager::LoadGame(uint32_t slot) {
  ISaveDataProvider& provider = GetSaveDataProvider();
  auto save = provider.LoadSave(slot).value();
  
  // Extract all saved scene states
  auto scenes = SceneDataExtractor::ExtractAllScenesFromSave(save);
  
  // Load each scene
  for (const auto& scene_data : scenes) {
    m_scene_manager.LoadScene(scene_data);
  }
}
```

---

## Key Principles

1. **Separate Providers** = Separate data sources (default files vs. save files)
2. **Direct Access** = Use `ISceneDataProvider` for templates/defaults
3. **Nested Access** = Use `ISaveDataProvider` + extractor for saved states
4. **Unified Consumption** = `LoadScene(SceneData)` works for both patterns
5. **Explicit Conversion** = Use extractors to make data flow clear

---

## Quick Comparison Table

| Aspect | Default Data | Saved Data |
|--------|-------------|------------|
| **Provider** | `ISceneDataProvider` | `ISaveDataProvider` |
| **Method** | `LoadSceneData(SceneType)` | `LoadSave(slot_index)` |
| **Returns** | `SceneData` (direct) | `SaveData` (contains scenes) |
| **Structure** | Flat | Nested |
| **File Location** | `data/scenes/*.bin` | `saves/slot_X/*.bin` |
| **Extraction** | None needed | Use `SceneDataExtractor` |
| **Use Case** | New game, templates | Load saved game |

---

## When to Use Extractor

### Don't Use Extractor If:
- Loading default/new game scene
- Scene data is already in `SceneData` format
- Using `ISceneDataProvider`

### Use Extractor If:
- Loading from save file
- Scene data is nested in `SaveData`
- Need to convert `SceneStateData` → `SceneData`

---

## Code Templates

### Template: Load Default Scene

```cpp
std::expected<SceneData, FailInfo> 
LoadDefaultScene(SceneType type) {
  ISceneDataProvider& provider = GetSceneDataProvider();
  return provider.LoadSceneData(type);
}
```

### Template: Extract Scene from Save (Future)

```cpp
std::expected<std::vector<SceneData>, FailInfo>
ExtractScenesFromSave(uint32_t slot_index) {
  ISaveDataProvider& provider = GetSaveDataProvider();
  auto save_result = provider.LoadSave(slot_index);
  
  if (!save_result.has_value()) {
    return std::unexpected(save_result.error());
  }
  
  // Extract scenes
  auto scenes = SceneDataExtractor::ExtractAllScenesFromSave(
      save_result.value());
  
  return scenes;
}
```

### Template: Unified Scene Loading

```cpp
class SceneManager {
public:
  void LoadScene(const SceneData& data) {
    // Works for both default and extracted saved data
    // Single code path, no conditionals
    
    // Create scene with data
    auto scene = m_factory.CreateScene(data.scene_type, data);
    
    // Configure and activate
    scene->Initialize();
    m_scenes.push_back(std::move(scene));
  }
};
```

---

## FAQ

### Q: Which provider should I use?

**A**: 
- New game / default scenes → `ISceneDataProvider`
- Saved game / load game → `ISaveDataProvider`

### Q: Do I always need an extractor?

**A**: No, only when loading saved game data with scene states.

### Q: Can I use the same loading code for both?

**A**: Yes! That's the point. Both patterns produce `SceneData`:
```cpp
// Works for both default and saved data
void LoadScene(const SceneData& data) {
  // Single unified implementation
}
```

### Q: When will the extractor be needed?

**A**: When `SaveData` is extended to include `scene_states` field.

### Q: Should providers know about each other?

**A**: No. Keep them independent. Use extractors or adapters to convert between formats.

---

## Related Documentation

- **Full Analysis**: SCENE_DATA_PROVIDER_ARCHITECTURE_ANALYSIS.md
- **View Pattern**: SAVE_LOAD_WORKFLOW_ANALYSIS.md
- **Provider System**: ../DATA_PROVIDER_SYSTEM.md
- **Save Data**: ../configuration/SAVE_DATA.md

---

**Quick Ref Created**: December 7, 2025
