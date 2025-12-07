# Scene Data Loading - Analysis Summary

**Date**: December 7, 2025  
**Status**: Analysis Complete - Implementation Ready  
**Related Documents**: 
- SCENE_DATA_PROVIDER_ARCHITECTURE_ANALYSIS.md (detailed analysis)
- SCENE_DATA_ACCESS_PATTERNS_QUICK_REF.md (quick reference)

---

## Executive Summary

This analysis provides a comprehensive, end-to-end architecture for scene data loading in the SteamRot game engine, from Engine startup to fully configured Scene ready to run the game loop.

### The Problem

- Need to load scenes from **default data** (new game) and **save data** (loaded game)
- Must abstract FlatBuffers away from game code (interface pattern)
- Handle nested data structures without coupling or wasteful copying
- Maintain consistency with existing patterns (IEngineDataProvider, etc.)
- Support future extensibility (JSON, XML, Lua formats)

### The Solution

**Two-Provider Strategy with Extractor Pattern**:
- `ISceneDataProvider` → Loads default/template scene data
- `ISaveDataProvider` → Loads saved game data (including scene states)
- `SceneDataExtractor` → Converts nested save data to flat scene data
- Unified configuration pipeline regardless of source

### Key Insight

> "The interface pattern you've already established with `ISaveDataProvider` and `ISceneDataProvider` is **exactly the right approach**. We don't need to mix systems - we extend the existing pattern consistently."

---

## Architectural Decisions

### 1. Use Separate Providers (Not One)

**Decision**: Keep `ISceneDataProvider` and `ISaveDataProvider` separate

**Rationale**:
- Different responsibilities (templates vs save files)
- Different error handling (missing defaults vs file corruption)
- Different lifecycle (static vs dynamic)
- Single Responsibility Principle

**Files**:
- Default scenes: `data/scenes/*.scene_data.bin`
- Save files: `saves/slot_X.save.bin`

### 2. Use Polymorphism for Format Abstraction

**Decision**: Virtual functions in interfaces for swappable implementations

```cpp
class ISceneDataProvider {
  virtual std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType scene_type) const = 0;
};

class FlatbuffersSceneDataProvider : public ISceneDataProvider { /*...*/ };
class JsonSceneDataProvider : public ISceneDataProvider { /*...*/ };  // Future
```

**Rationale**: Allows format changes without touching game code

### 3. Use Function Overloads for Source Differentiation

**Decision**: Different methods for default vs save

```cpp
class SceneFactory {
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateDefaultScene(const SceneType& scene_type, 
                    const GameContext& game_context);
  
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateSceneFromSave(const SceneData& scene_metadata,
                     const SceneStateData& scene_state,
                     const GameContext& game_context);
};
```

**Rationale**: Clear intent at call site, different parameter types

### 4. Use Extractor Pattern for Nested Data

**Decision**: SceneDataExtractor converts SaveData → SceneData

```cpp
class SceneDataExtractor {
  SceneData ExtractSceneMetadata(const SaveData& save) const;
  SceneStateData ExtractSceneState(const SaveData& save, uint32_t index) const;
};
```

**Rationale**: No wasteful copying, single source of truth, clean transformation

### 5. Native Structs at API Boundaries

**Decision**: Interfaces return native C++ structs, not FlatBuffers types

```cpp
// ✅ GOOD
struct SceneData {
  SceneType scene_type;
  std::string scene_id;
  uint32_t render_texture_width;
  uint32_t render_texture_height;
};

// ❌ BAD
const SceneDataFbs* LoadSceneData();
```

**Rationale**: No format coupling in game code

---

## The Two Access Patterns

### Pattern 1: Default Scene (New Game)

```
User Starts New Game
        ↓
ISceneDataProvider::LoadSceneData(SceneType)
        ↓
SceneData (metadata)
        ↓
SceneFactory::CreateDefaultScene()
        ↓
Scene::ConfigureFromDefault()
        ↓
Fully Configured Scene
```

**Code**:
```cpp
ISceneDataProvider& provider = GetSceneDataProvider();
auto scene_data = provider.LoadSceneData(SceneType::SceneType_TITLE);
SceneFactory factory;
auto scene = factory.CreateDefaultScene(scene_type, game_context);
scene->ConfigureFromDefault();
```

### Pattern 2: Saved Scene (Load Game)

```
User Loads Save File
        ↓
ISaveDataProvider::LoadSave(slot_index)
        ↓
SaveData (nested, complete state)
        ↓
SceneDataExtractor::ExtractSceneData()
        ↓
SceneData + SceneStateData
        ↓
SceneFactory::CreateSceneFromSave()
        ↓
Scene::ConfigureFromSave()
        ↓
Fully Configured Scene
```

**Code**:
```cpp
ISaveDataProvider& save_provider = GetSaveDataProvider();
auto save_data = save_provider.LoadSave(slot_index);
SceneDataExtractor extractor;
SceneData scene_data = extractor.ExtractSceneMetadata(save_data);
SceneStateData scene_state = extractor.ExtractSceneState(save_data, 0);
SceneFactory factory;
auto scene = factory.CreateSceneFromSave(scene_data, scene_state, game_context);
scene->ConfigureFromSave(scene_state);
```

---

## End-to-End Flow

### Complete Pipeline (Default Scene)

```
1. main.cpp
   └─ GameEngine engine;
   └─ engine.RunGame();

2. Engine::RunGame()
   └─ StartUp();        // ← Configure engine resources
   └─ RunGameLoop();

3. Engine::StartUp() [✅ WORKING]
   └─ IEngineDataProvider::LoadEngineResourcesConfig()
   └─ IEngineDataProvider::LoadEngineConfig()
   └─ IEngineDataProvider::LoadEngineState()

4. SceneManager::LoadTitleScene()
   └─ AddSceneFromDefault(SceneType::SceneType_TITLE)

5. SceneFactory::CreateDefaultScene() [✅ WORKING]
   └─ Create Scene object
   └─ ISceneDataProvider::LoadSceneData()
   └─ Configure render texture dimensions
   └─ scene->ConfigureFromDefault()

6. Scene::ConfigureFromDefault() [⚠️ NEEDS IMPROVEMENT]
   └─ EntityManager::ConfigureEntitiesFromDefaultData()
   └─ [Currently uses FlatBuffers directly - needs interface]

7. ArchetypeManager::GenerateAllArchetypes() [✅ WORKING]
   └─ Generate archetypes from configured components

8. LogicFactory::CreateLogicMap() [✅ WORKING]
   └─ Create logic systems for scene

9. Scene ready → Game loop runs [✅ WORKING]
```

---

## Implementation Roadmap

### Current State

| Component | Status | Notes |
|-----------|--------|-------|
| Engine data loading | ✅ Complete | Via IEngineDataProvider |
| Scene metadata loading | ✅ Complete | Via ISceneDataProvider |
| Entity configuration | ⚠️ Partial | Uses FlatBuffers directly |
| Save data interface | ⚠️ Partial | Missing scene_states |
| Scene data extractor | ❌ Not implemented | Future Phase 2 |
| Save scene loading | ❌ Not implemented | Future Phase 5 |

### Implementation Phases

| Phase | Goal | Risk | Effort | Priority |
|-------|------|------|--------|----------|
| **Phase 1** | Complete SceneData provider | ⬜ Low | 1-2 days | High |
| **Phase 2** | Add SceneDataExtractor | ⬜ Low | 2-3 days | High |
| **Phase 3** | Create IEntityDataProvider | 🟡 Medium | 1-2 weeks | Medium |
| **Phase 4** | Implement SaveData scene states | 🔴 High | 2-3 weeks | Low |
| **Phase 5** | Add SceneFactory overloads | ⬜ Low | 3-5 days | Medium |

**Total Estimated Effort**: 4-6 weeks (Phases 1-5)

---

## Code Patterns

### Provider Access (Factory Pattern)

```cpp
// Centralized provider access
ISceneDataProvider& GetSceneDataProvider();
ISaveDataProvider& GetSaveDataProvider();
IEngineDataProvider& GetEngineDataProvider();

// Implementation
ISceneDataProvider& GetSceneDataProvider() {
  static FlatbuffersSceneDataProvider provider;
  return provider;
}
```

### Configuration Pattern

```cpp
// Step 1: Load data via provider (interface)
ISceneDataProvider& provider = GetSceneDataProvider();
auto data_result = provider.LoadSceneData(scene_type);

// Step 2: Convert to native struct (at provider boundary)
const SceneData& scene_data = data_result.value();

// Step 3: Use native struct in game code (no FlatBuffers!)
scene->Configure(scene_data);
```

### Extractor Pattern

```cpp
// Step 1: Load complete save file
ISaveDataProvider& provider = GetSaveDataProvider();
const SaveData& save = provider.LoadSave(slot).value();

// Step 2: Extract specific data as needed
SceneDataExtractor extractor;
SceneData metadata = extractor.ExtractSceneMetadata(save);
SceneStateData state = extractor.ExtractSceneState(save, 0);

// Step 3: Use extracted data (flat, easy to consume)
scene->Configure(metadata, state);
```

---

## Key Takeaways

### What Makes This Architecture Cohesive

1. **Consistent Pattern**: Same approach for Engine, Scene, Entity, Asset data
2. **Clear Boundaries**: Interfaces return native structs, never FlatBuffers
3. **Separation of Concerns**: One interface per data domain
4. **Unified Pipeline**: Same flow for default and save scenes
5. **Extractor Pattern**: Clean handling of nested data
6. **Format Abstraction**: Polymorphism for FlatBuffers/JSON/XML
7. **Source Clarity**: Overloads for default vs save

### No Mixed Systems

- ✅ All data loading uses interface pattern
- ✅ All data boundaries use native structs
- ✅ All nested data uses extractor pattern
- ✅ All format variations use polymorphism
- ✅ All source variations use overloads

### Future Extensibility

**Adding JSON Support**:
```cpp
class JsonSceneDataProvider : public ISceneDataProvider {
  std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType scene_type) const override {
    // Load from JSON file
    // Parse to SceneData
    // Return native struct
  }
};

// Game code unchanged!
ISceneDataProvider& provider = GetSceneDataProvider();
auto scene_data = provider.LoadSceneData(scene_type);
```

**Adding New Scene Types**:
```cpp
// 1. Add to enum
enum class SceneType { 
  SceneType_TITLE, 
  SceneType_CRAFTING,
  SceneType_SHOP  // New!
};

// 2. Create scene class
class ShopScene : public Scene { /*...*/ };

// 3. Add to SceneFactory switch
// 4. Add data file: data/scenes/shop.scene_data.bin
// 5. Provider automatically supports it!
```

---

## Anti-Patterns to Avoid

### ❌ Don't Mix Providers

```cpp
// BAD: Using both for same operation
ISceneDataProvider& scene_provider = GetSceneDataProvider();
ISaveDataProvider& save_provider = GetSaveDataProvider();
```

**Fix**: Choose one path (default OR save)

### ❌ Don't Pass FlatBuffers Types

```cpp
// BAD: FlatBuffers in game code
const SceneDataFbs* fb_data = loader.ProvideSceneData(type);
scene->Configure(fb_data);
```

**Fix**: Convert to native struct at provider boundary

### ❌ Don't Create Intermediate Structs with FlatBuffers

```cpp
// BAD: Intermediate containing FlatBuffers
struct SceneConfigData {
  const SceneDataFbs* fb_scene;  // ❌
  SceneType type;
};
```

**Fix**: Use native structs only

---

## Decision Trees

### Which Provider?

```
Q: Loading a saved game?
├─ YES → ISaveDataProvider
└─ NO → ISceneDataProvider
```

### Which Factory Method?

```
Q: Scene from save file?
├─ YES → CreateSceneFromSave()
└─ NO → CreateDefaultScene()
```

### Which Configuration Method?

```
Q: Scene from save file?
├─ YES → ConfigureFromSave()
└─ NO → ConfigureFromDefault()
```

### Which Data Structure?

```
Q: What do you need?
├─ Only metadata? → SceneData
└─ Complete state? → SceneStateData
```

---

## Related Documentation

### Detailed Analysis
- `SCENE_DATA_PROVIDER_ARCHITECTURE_ANALYSIS.md` (36KB)
  - Complete end-to-end flow
  - Current state assessment
  - Detailed implementation roadmap
  - Polymorphism vs overloads strategy
  - Future extensibility

### Quick Reference
- `SCENE_DATA_ACCESS_PATTERNS_QUICK_REF.md` (12KB)
  - The two access patterns
  - Code examples
  - Decision trees
  - Anti-patterns

### Existing Documentation
- `DATA_LOADING_INTERFACE_EXECUTIVE_SUMMARY.md`
  - Interface pattern overview
  - Nomenclature system
  - Migration strategy

### Related Analysis
- `SAVE_LOAD_WORKFLOW_ANALYSIS.md`
  - View pattern for nested data
  - Anti-patterns to avoid

---

## Next Steps

### For User Review

1. ✅ **Review Analysis Documents**
   - Read executive summary (this document)
   - Read detailed analysis if needed
   - Verify architectural decisions align with vision

2. ✅ **Validate Approach**
   - Two-provider strategy (ISceneDataProvider + ISaveDataProvider)
   - Extractor pattern for nested data
   - Polymorphism for formats, overloads for sources

3. ✅ **Prioritize Implementation**
   - Phase 1: Complete SceneData provider (high priority)
   - Phase 2: Add SceneDataExtractor (high priority)
   - Phase 3-5: Entity data provider and save states (medium/low)

### For Implementation

1. **Phase 1: Complete SceneData Provider** (1-2 days)
   - Finish FlatbuffersSceneDataProvider implementation
   - Add comprehensive tests

2. **Phase 2: Add Extractor Pattern** (2-3 days)
   - Create SceneDataExtractor class
   - Define SceneStateData struct
   - Implement extraction methods

3. **Phase 3: Create IEntityDataProvider** (1-2 weeks)
   - Define EntityCollectionData struct
   - Create interface and implementation
   - Remove DataType enum
   - Update EntityConfigurator

---

## Summary

### The Vision (Achieved in Analysis)

✅ Cohesive architecture for scene data loading  
✅ Abstracts FlatBuffers from game code  
✅ Supports default and save scenes  
✅ Handles nested data efficiently  
✅ Uses polymorphism appropriately (format abstraction)  
✅ Uses overloads appropriately (source differentiation)  
✅ Consistent with existing patterns  
✅ Extensible to new formats and scenes  

### The Architecture

```
Data Files (FlatBuffers/JSON/XML)
        ↓
Provider Interfaces (ISceneDataProvider, ISaveDataProvider)
        ↓
Native Structs (SceneData, SceneStateData, SaveData)
        ↓
Extractors (SceneDataExtractor)
        ↓
Factories (SceneFactory)
        ↓
Configurators (EntityConfigurator)
        ↓
Configured Objects (Scene, EntityMemoryPool)
        ↓
Game Loop
```

### The Outcome

A **clean, cohesive, extensible system** that:
- Separates concerns appropriately
- Uses consistent patterns throughout
- Abstracts implementation details
- Supports future growth
- Maintains existing code stability

---

**Document Status**: Analysis Complete - Ready for Implementation
**Implementation Status**: 40% Complete (Phases 1-2), 60% Remaining (Phases 3-5)
**Estimated Completion**: 4-6 weeks for full implementation
