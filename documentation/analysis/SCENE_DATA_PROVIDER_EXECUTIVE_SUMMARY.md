# Scene Data Provider Architecture - Executive Summary

**Date**: December 7, 2025  
**Status**: Analysis Complete  
**Issue**: How to handle scene data with two access patterns (direct vs. nested)

---

## Problem Statement

The game engine needs to load scene data from two different sources:

1. **Default scene data** - Direct, flat structure from template files
2. **Saved scene data** - Nested within save files (future)

**Question**: How should provider and viewer classes handle these two access patterns without:
- Duplicating code
- Creating complex conditional logic
- Exposing implementation details

---

## Analysis Summary

### Current Architecture ✅

**Already Implemented and Working:**

- `ISceneDataProvider` - Loads default/template scene configuration
  - Returns `SceneData` struct directly
  - Source: `data/scenes/*.scene_data.bin`
  - Use case: New game, scene templates

- `ISaveDataProvider` - Loads saved game data
  - Returns `SaveData` struct with metadata
  - Source: `saves/slot_X/save_data.bin`
  - Use case: Load saved game
  - Note: Scene states not yet implemented

### The Two Access Patterns

#### Pattern 1: Direct Access (Default Data)

```cpp
// Simple, flat access
ISceneDataProvider& provider = GetSceneDataProvider();
auto scene_data = provider.LoadSceneData(SceneType::SceneType_TITLE);
// Use scene_data directly
```

**Characteristics**:
- ✅ Flat structure
- ✅ Single method call
- ✅ Fast and efficient

#### Pattern 2: Nested Access (Saved Data - Future)

```cpp
// Nested structure requires extraction
ISaveDataProvider& provider = GetSaveDataProvider();
auto save_data = provider.LoadSave(0);

// Extract scene data from nested structure
auto scenes = SceneDataExtractor::ExtractAllScenesFromSave(save_data);

// Use extracted data (same format as Pattern 1!)
for (const auto& scene_data : scenes) {
  LoadScene(scene_data);
}
```

**Characteristics**:
- ⚠️ Nested in save file
- ⚠️ Requires extraction step
- ✅ Same downstream format

---

## Recommended Solution

### Keep Providers Separate ✅

**Current approach is correct - maintain it.**

```
ISceneDataProvider  → LoadSceneData(type)  → SceneData (direct)
ISaveDataProvider   → LoadSave(slot)       → SaveData (nested)
```

**Rationale**:
- Clear separation of concerns
- Simple to understand and use
- Each provider has one responsibility
- Already implemented and working

### Add Extractor When Needed (Future)

**When `SaveData` is extended to include `scene_states`:**

```
ISceneDataProvider  → LoadSceneData(type)  → SceneData
                                              ↓
                                        LoadScene(data)
                                              ↑
ISaveDataProvider   → LoadSave(slot)       → SaveData
                                              ↓
                     SceneDataExtractor    → SceneData
```

**Implementation checklist (future)**:
- [ ] Extend `SaveData` struct with `scene_states` field
- [ ] Update `save_data.fbs` schema
- [ ] Create `SceneDataExtractor` class
- [ ] Add extraction methods
- [ ] Update game loading workflow
- [ ] Write tests

---

## Key Principles

1. **Separate Providers** = Separate data sources
   - Don't merge `ISceneDataProvider` and `ISaveDataProvider`
   - Each has distinct responsibility

2. **Direct Access** = Use `ISceneDataProvider`
   - For new games, templates, defaults

3. **Nested Access** = Use `ISaveDataProvider` + extractor
   - For saved games, load games

4. **Unified Consumption** = Same `LoadScene(SceneData)` method
   - Both patterns produce `SceneData`
   - Single code path for scene loading

5. **Explicit Conversion** = Use extractors
   - Make data flow clear
   - Easy to test and understand

---

## Documentation

Three comprehensive documents created:

1. **SCENE_DATA_PROVIDER_ARCHITECTURE_ANALYSIS.md**
   - Full technical analysis (18KB)
   - Problem explanation
   - Solution comparison
   - Implementation details
   - Future roadmap

2. **SCENE_DATA_ACCESS_PATTERNS_QUICK_REF.md**
   - Quick reference guide (7KB)
   - Decision trees
   - Code templates
   - FAQ
   - Common scenarios

3. **SCENE_DATA_ACCESS_PATTERNS_VISUALS.md**
   - Visual diagrams (16KB)
   - Flow charts
   - Data structure comparisons
   - Memory layouts
   - Decision trees

All located in: `documentation/analysis/`

---

## Decision Matrix

| Scenario | Provider | Method | Returns | Extraction Needed? |
|----------|----------|--------|---------|-------------------|
| New Game | ISceneDataProvider | LoadSceneData() | SceneData | No |
| Default Scene | ISceneDataProvider | LoadSceneData() | SceneData | No |
| Load Game (Current) | ISaveDataProvider | LoadSave() | SaveData (minimal) | No |
| Load Game (Future) | ISaveDataProvider | LoadSave() | SaveData (with scenes) | Yes - use SceneDataExtractor |

---

## Code Examples

### Current: New Game

```cpp
void GameManager::NewGame() {
  ISceneDataProvider& provider = GetSceneDataProvider();
  auto result = provider.LoadSceneData(SceneType::SceneType_TITLE);
  
  if (result.has_value()) {
    m_scene_manager.LoadScene(result.value());
  }
}
```

### Current: Load Game (Minimal)

```cpp
void GameManager::LoadGame(uint32_t slot) {
  ISaveDataProvider& save_provider = GetSaveDataProvider();
  auto save = save_provider.LoadSave(slot).value();
  
  // Currently only has current_scene_type, load default
  ISceneDataProvider& scene_provider = GetSceneDataProvider();
  auto scene_data = scene_provider.LoadSceneData(
      save.current_scene_type).value();
  
  m_scene_manager.LoadScene(scene_data);
}
```

### Future: Load Game (With Scene States)

```cpp
void GameManager::LoadGame(uint32_t slot) {
  ISaveDataProvider& provider = GetSaveDataProvider();
  auto save = provider.LoadSave(slot).value();
  
  // Extract all saved scene states
  auto scenes = SceneDataExtractor::ExtractAllScenesFromSave(save);
  
  // Load each scene using standard path
  for (const auto& scene_data : scenes) {
    m_scene_manager.LoadScene(scene_data);  // Same as new game!
  }
}
```

---

## Benefits of This Approach

### For Current Implementation
- ✅ Simple and clear
- ✅ Already working
- ✅ Easy to test
- ✅ Maintainable

### For Future Enhancement
- ✅ Clear migration path
- ✅ Minimal code changes needed
- ✅ Unified downstream code
- ✅ Explicit data flow
- ✅ Testable components

### For Developers
- ✅ Easy to understand
- ✅ Clear which provider to use
- ✅ Good documentation
- ✅ Visual guides available
- ✅ Code templates provided

---

## Next Steps

### Immediate (Nothing Required)
- ✅ Current implementation is correct
- ✅ No changes needed now
- ✅ Analysis complete

### When Scene States Needed
1. Implement entity serialization
2. Extend `SaveData` with `scene_states`
3. Create `SceneDataExtractor` class
4. Update game loading workflow
5. Reference analysis documents

### Long-term (Optional)
- Consider view pattern if save files become very large
- See SAVE_LOAD_WORKFLOW_ANALYSIS.md for details
- Currently deferred due to sufficient current approach

---

## Conclusion

**The current architecture is sound and should be maintained.**

Key insights:
- Two providers for two data sources is correct
- No merging or conditional logic needed
- Future extraction layer will unify consumption
- Documentation provides clear guidance

**Status**: Analysis complete, no immediate action required.

---

## Related Documentation

- **This Document**: Executive summary
- **Full Analysis**: SCENE_DATA_PROVIDER_ARCHITECTURE_ANALYSIS.md
- **Quick Reference**: SCENE_DATA_ACCESS_PATTERNS_QUICK_REF.md
- **Visual Guide**: SCENE_DATA_ACCESS_PATTERNS_VISUALS.md
- **Provider vs Viewer Pattern**: PROVIDER_VS_VIEWER_PATTERN.md
- **Abstract Factory Pattern**: ABSTRACT_SCENE_FACTORY_PATTERN.md
- **View Pattern Details**: SAVE_LOAD_WORKFLOW_ANALYSIS.md
- **Provider System**: ../DATA_PROVIDER_SYSTEM.md
- **Save Data**: ../configuration/SAVE_DATA.md

---

**Analysis Completed**: December 7, 2025  
**Status**: Ready for Review
