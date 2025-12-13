# Data Architecture Analysis - Executive Summary

**Date**: December 13, 2025  
**Task**: Data architecture analysis of polymorphic structs for scene loading, save data, and configuration  
**Status**: ✅ Complete

---

## What Was Analyzed

Per the problem statement, conducted comprehensive data base analysis (documents only) covering:

1. ✅ **Sensible layer division** - Three-layer architecture to eliminate circular dependencies
2. ✅ **CMake package stratification** - Layer 1 (Data/Types), Layer 2 (Logic/Providers), Layer 3 (Orchestration)
3. ✅ **Abstract classes and interfacing** - ISceneDataProvider, ISceneConfigurator, IEntityConfigurator, ISaveDataProvider
4. ✅ **Concrete implementations** - FlatBuffers (default), Save (player progress), Test (unit tests)
5. ✅ **Visualizations** - Complete diagrams of architecture, data flow, and migration path

---

## Key Findings

### What's Already Good ✅

1. **Polymorphic struct pattern EXISTS**: `SceneData → FbsSceneData` implemented in codebase
2. **Provider pattern 75% complete**: 5 providers exist (IEngineDataProvider, ISceneDataProvider, etc.)
3. **Configurator pattern established**: ISceneConfigurator, IEntityConfigurator interfaces exist
4. **SaveData interface defined**: ISaveDataProvider with LoadSave/SaveGame methods

### What Needs Work ⚠️

1. **Circular dependencies**: scenes ↔ data_providers, display ↔ scenes ↔ engine
2. **Configurator loads data**: FlatbuffersDefaultSceneConfigurator has FlatbuffersDataLoader member (violates SRP)
3. **Save infrastructure incomplete**: Interface exists but entity serialization missing
4. **FlatBuffers exposed**: Some configurators still use FlatBuffers types directly

---

## Recommended Architecture

### Three-Layer Stratification

```
Layer 3: Orchestration
  ├─ scene_management (SceneManager, SceneFactory, Scene)
  ├─ engine_core (GameEngine, Engine, GameLoop)
  └─ display_system (DisplayManager)
  
  Dependencies: Layer 1 + Layer 2
  Can have: Circular deps within Layer 3 via interfaces

Layer 2: Business Logic
  ├─ providers (FlatbuffersSceneDataProvider, SaveSceneDataProvider)
  ├─ configurators (FlatbuffersSceneConfigurator, SaveSceneConfigurator)
  ├─ logic_implementations (UIRenderLogic, UICollisionLogic)
  └─ assets_system (AssetManager)
  
  Dependencies: Layer 1 ONLY
  Cannot have: Circular deps or deps on Layer 3

Layer 1: Data, Types & Interfaces
  ├─ types (SceneData, EntityData, Component structs)
  ├─ interfaces (ISceneDataProvider, ISceneConfigurator)
  ├─ data_structures (EntityMemoryPool, ArchetypeManager)
  └─ events (EventPacket, EventBus)
  
  Dependencies: SFML, std, external libs ONLY
  Cannot have: ANY deps on other SteamRot packages
```

**Result**: Zero circular dependencies, clean layer separation

### Polymorphic Struct Pattern

```cpp
// Base struct (abstract data type)
struct SceneData {
  SceneInfo scene_info;
};

// FlatBuffers implementation (default data)
struct FbsSceneData : public SceneData {
  const SceneDataFbs *scene_data_fbs;
};

// Save file implementation (player progress)
struct SaveSceneData : public SceneData {
  const SavedSceneDataFbs *saved_scene_data_fbs;
  uint64_t play_time_seconds;
  std::string last_modified;
};

// Test data implementation (unit tests)
struct TestSceneData : public SceneData {
  const TestSceneDataFbs *test_scene_data_fbs;
  std::string test_name;
};
```

**Benefits**: 
- Single SceneFactory interface for all data sources
- No combinatorial explosion (N scene types × M data sources)
- Easy to add new data sources
- Type-safe via dynamic_cast

### Provider-Configurator Separation

**Provider** (loads data):
```cpp
class ISceneDataProvider {
  virtual std::unique_ptr<SceneData>
  ProvideDefaultSceneData(const SceneType) const = 0;
  
  virtual std::unique_ptr<SceneData>
  ProvideSceneDataFromSave(const SaveData&, const SceneType) const = 0;
};
```

**Configurator** (applies data):
```cpp
class ISceneConfigurator {
  virtual std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneData *data) = 0;
};
```

**Result**: Single Responsibility Principle, easy to swap data sources

---

## Abstract Interfaces Required

### Scene Loading

1. **ISceneDataProvider** - Abstract data loading interface
   - Implementations: FlatbuffersSceneDataProvider, SaveSceneDataProvider, TestSceneDataProvider
   - Returns: std::unique_ptr<SceneData> (polymorphic)

2. **ISceneConfigurator** - Abstract scene configuration interface
   - Implementations: FlatbuffersSceneConfigurator, SaveSceneConfigurator, TestSceneConfigurator
   - Receives: Scene&, const SceneData*

### Save Data Loading

3. **ISaveDataProvider** - Abstract save data interface
   - Methods: LoadSave(), SaveGame(), GetSaveSlots()
   - Implementation: FlatbuffersSaveDataProvider

### Entity Configuration

4. **IEntityConfigurator** - Abstract entity configuration interface
   - Implementations: FlatbuffersEntityConfigurator, SaveEntityConfigurator
   - Receives: EntityMemoryPool&, const SceneData*

---

## Concrete Implementations

### Implementation Matrix

| Data Source | Provider | Configurator | Status |
|-------------|----------|--------------|--------|
| **Default (.bin)** | FlatbuffersSceneDataProvider | FlatbuffersSceneConfigurator | ✅ Exists (refactor needed) |
| **Save File** | SaveSceneDataProvider | SaveSceneConfigurator | ❌ Need to create |
| **Test Data** | TestSceneDataProvider | TestSceneConfigurator | ❌ Need to create |
| **Engine Config** | FlatbuffersEngineDataProvider | N/A | ✅ Exists |
| **Asset Data** | FlatbuffersAssetDataProvider | N/A | ✅ Exists |

### Example: Default Scene Loading

```
SceneFactory
    ↓
FlatbuffersSceneDataProvider
    ↓ (loads SceneDataFbs from .bin)
    ↓ (creates FbsSceneData)
    ↓ (returns std::unique_ptr<SceneData>)
SceneFactory
    ↓ (creates empty Scene)
    ↓
FlatbuffersSceneConfigurator
    ↓ (dynamic_cast to FbsSceneData*)
    ↓ (configures Scene from FlatBuffers)
    ↓
Scene configured ✅
```

### Example: Save File Loading

```
ISaveDataProvider.LoadSave(slot)
    ↓ (loads save_slot.save)
    ↓ (returns SaveData struct)
SceneFactory
    ↓
SaveSceneDataProvider
    ↓ (extracts saved scene from SaveData)
    ↓ (creates SaveSceneData with UUID)
    ↓ (returns std::unique_ptr<SceneData>)
SceneFactory
    ↓ (creates empty Scene)
    ↓
SaveSceneConfigurator
    ↓ (dynamic_cast to SaveSceneData*)
    ↓ (restores Scene UUID and state)
    ↓ (restores EntityMemoryPool)
    ↓
Scene restored ✅
```

---

## Visualizations Created

### Architecture Diagrams

1. **Three-Layer Architecture** - Shows clean layer separation with dependencies flowing downward
2. **Polymorphic Struct Hierarchy** - Shows SceneData inheritance tree
3. **Provider Pattern** - Shows ISceneDataProvider implementations
4. **Configurator Pattern** - Shows ISceneConfigurator implementations
5. **CMake Dependency Graph** - Shows current (circular) vs target (clean) dependencies

### Data Flow Diagrams

1. **Default Scene Loading** - Current (problematic) vs Target (clean) flows
2. **Save File Loading** - Complete flow from LoadSave() to configured Scene
3. **Save Game Flow** - Complete flow from CaptureGameState() to save file
4. **Entity Configuration** - Shows polymorphic data handling for entities
5. **Migration Path** - Visual timeline of 5 phases

---

## Migration Path

### Phase 1: Extract Interfaces (Week 1)

**Goal**: Create `interfaces` package with zero dependencies

- Create `src/interfaces/` directory
- Move interface declarations (ISceneDataProvider, ISceneConfigurator, etc.)
- Create `interfaces` CMake target (INTERFACE library)
- Update all includes

**Validation**: Builds successfully, no new dependencies

### Phase 2: Refactor Configurators (Week 2)

**Goal**: Configurators receive `SceneData*`, don't load data

- Change `ISceneConfigurator::ConfigureScene()` signature to accept `SceneData*`
- Remove `FlatbuffersDataLoader` member from configurators
- Update `SceneFactory` to get provider and configurator separately
- Provider loads, configurator receives

**Validation**: Default scene loading still works

### Phase 3: Implement Save Infrastructure (Weeks 3-4)

**Goal**: Complete save/load workflow

- Create `SaveSceneData` struct
- Implement `SaveSceneDataProvider`
- Implement `SaveSceneConfigurator`
- Implement `EntitySerializer` (serialize EntityMemoryPool to FlatBuffers)
- Update `ISaveDataProvider` with scene capture methods
- Test save/load round-trip

**Validation**: Can save and load game state

### Phase 4: Break Circular Dependencies (Weeks 5-6)

**Goal**: Eliminate scenes ↔ data_providers circular dependency

- Move `SceneType` enum to `types` package
- Ensure `data_providers` only depends on Layer 1 packages
- Move scene types to `types/scene/`
- Use linker flag `-Wl,--no-undefined` to catch violations

**Validation**: No circular dependencies, clean build

### Phase 5: Reorganize Packages (Weeks 7-8)

**Goal**: Full three-layer architecture

- Rename `data_providers` → `providers`
- Create `configurators` package (extract from scenes/entity)
- Rename `scenes` → `scene_management`
- Rename `engine` → `engine_core`
- Rename `display` → `display_system`
- Update all CMakeLists.txt

**Validation**: Clean build, all tests pass, layer rules enforced

---

## Benefits of Proposed Architecture

### For Development

✅ **No circular dependencies** - Clean build, clear dependency flow  
✅ **Easy to add data sources** - Just implement provider + configurator  
✅ **Easy to test** - Mock providers for unit tests  
✅ **Clean separation of concerns** - Provider loads, configurator applies  
✅ **Type-safe** - Compile-time interfaces, runtime polymorphism

### For Features

✅ **Save/Load support** - SaveSceneData + SaveSceneConfigurator  
✅ **Multiple save slots** - ISaveDataProvider.GetSaveSlots()  
✅ **Test data** - TestSceneData for unit tests  
✅ **Future: Network data** - NetworkSceneDataProvider for multiplayer  
✅ **Future: Procedural gen** - ProceduralSceneDataProvider

### For Maintenance

✅ **Single interface** - SceneFactory doesn't change when adding data sources  
✅ **Isolated changes** - Adding provider doesn't affect configurator  
✅ **Clear ownership** - Provider owns buffers, configurator owns configuration logic  
✅ **Reduced coupling** - FlatBuffers isolated to provider implementations

---

## Documents Created

1. **DATA_ARCHITECTURE_ANALYSIS.md** (37KB)
   - Complete technical analysis
   - Current state, dependencies, patterns
   - Abstract interfaces specifications
   - Concrete implementations design
   - Implementation checklist

2. **DATA_ARCHITECTURE_DIAGRAMS.md** (44KB)
   - Visual architecture diagrams
   - Data flow diagrams
   - CMake dependency graphs
   - Migration path visualization

3. **DATA_ARCHITECTURE_QUICK_REF.md** (14KB)
   - Quick decision trees
   - Pattern summaries
   - Code snippets
   - Checklists
   - FAQ

---

## Recommendations

### Immediate Actions

1. **Review with team** - Discuss architecture, get buy-in
2. **Approve design** - Confirm three-layer architecture acceptable
3. **Prioritize phases** - Decide which phases to implement first

### Suggested Priority

**High Priority**:
- Phase 1 (Extract Interfaces) - Breaks circular deps, enables rest
- Phase 2 (Refactor Configurators) - Fixes SRP violation, enables Phase 3

**Medium Priority**:
- Phase 3 (Save Infrastructure) - Enables save/load feature
- Phase 4 (Break Circular Deps) - Clean architecture

**Lower Priority**:
- Phase 5 (Reorganize Packages) - Nice to have, can defer

### NOT Recommended

❌ **Big bang refactor** - Too risky, hard to test  
❌ **Skip Phase 1** - Interfaces are foundation for rest  
❌ **Implement save without Phase 2** - Will multiply technical debt

---

## Success Criteria

After implementation, you should have:

✅ **Zero circular dependencies** between CMake packages  
✅ **Clean three-layer architecture** enforced by build system  
✅ **Multiple data sources** (default, save, test) working  
✅ **Save/load functionality** fully implemented  
✅ **Testable architecture** with mockable providers  
✅ **FlatBuffers isolated** to provider implementations  
✅ **Single SceneFactory** that works with all data sources

---

## Next Steps

1. **Review documents** with development team
2. **Discuss architecture** - Questions? Concerns? Modifications?
3. **Approve design** - Get sign-off on three-layer architecture
4. **Plan Phase 1** - Create implementation tickets for interface extraction
5. **Begin implementation** - Start with Phase 1 (lowest risk, highest foundation value)

---

## Related Documents

**Full Analysis**: [DATA_ARCHITECTURE_ANALYSIS.md](DATA_ARCHITECTURE_ANALYSIS.md)  
**Visual Diagrams**: [DATA_ARCHITECTURE_DIAGRAMS.md](DATA_ARCHITECTURE_DIAGRAMS.md)  
**Quick Reference**: [DATA_ARCHITECTURE_QUICK_REF.md](DATA_ARCHITECTURE_QUICK_REF.md)

**Previous Analyses**:
- [SAVE_LOAD_WORKFLOW_ANALYSIS.md](SAVE_LOAD_WORKFLOW_ANALYSIS.md) - View pattern for nested data
- [DATA_PROVIDER_SYSTEM.md](../DATA_PROVIDER_SYSTEM.md) - Current provider pattern
- [CURRENT_STATE_ANALYSIS_2025.md](CURRENT_STATE_ANALYSIS_2025.md) - Codebase state

---

## Conclusion

The SteamRot codebase already has good foundations:
- Polymorphic struct pattern established (SceneData → FbsSceneData)
- Provider pattern 75% complete
- Configurator pattern established

Main improvements needed:
- Break circular dependencies via three-layer architecture
- Separate provider (loads data) from configurator (applies data)
- Complete save infrastructure
- Enforce layer rules via build system

The proposed architecture provides:
- Clean separation of concerns
- Type-safe polymorphism
- Extensible design for future data sources
- Testable structure

Implementation is incremental, testable at each phase, and maintains backward compatibility.

**Ready to proceed with Phase 1 implementation upon team approval.**

---

**Status**: ✅ Analysis Complete  
**Date**: December 13, 2025  
**Author**: GitHub Copilot Agent  
**Review Required**: Yes - Team review and architecture approval needed
