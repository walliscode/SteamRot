# Data Architecture Quick Reference

**Date**: December 13, 2025  
**For**: Quick lookup of data architecture patterns and decisions

---

## TL;DR

✅ **Use polymorphic structs** (SceneData → FbsSceneData, SaveSceneData)  
✅ **Provider loads data** → returns `std::unique_ptr<SceneData>`  
✅ **Configurator receives data** → `ConfigureScene(Scene&, SceneData*)`  
✅ **Three layers**: Data/Types → Logic/Providers → Orchestration  
❌ **No circular dependencies** between layers

---

## Quick Decision Tree

### "I need to load scene data..."

```
What's the data source?
├─ Default (.bin files) → Use FlatbuffersSceneDataProvider
├─ Save file → Use SaveSceneDataProvider  
└─ Test → Use TestSceneDataProvider
```

### "I need to configure a scene..."

```
What's the data type?
├─ FbsSceneData → Use FlatbuffersSceneConfigurator
├─ SaveSceneData → Use SaveSceneConfigurator
└─ TestSceneData → Use TestSceneConfigurator
```

### "I'm creating a new data source..."

```
1. Create derived SceneData struct
2. Implement ISceneDataProvider
3. Implement ISceneConfigurator
4. Register in provider factory
```

---

## Pattern Summary

### Polymorphic Struct Pattern

```cpp
// Base (abstract)
struct SceneData {
  SceneInfo scene_info;
};

// FlatBuffers implementation
struct FbsSceneData : public SceneData {
  const SceneDataFbs *scene_data_fbs;
};

// Save file implementation
struct SaveSceneData : public SceneData {
  const SavedSceneDataFbs *saved_scene_data_fbs;
  uint64_t play_time_seconds;
  std::string last_modified;
};
```

**Purpose**: Support multiple data sources with single interface

### Provider Pattern

```cpp
class ISceneDataProvider {
public:
  virtual std::unique_ptr<SceneData>
  ProvideDefaultSceneData(const SceneType) const = 0;
};
```

**Note**: Provider returns `std::unique_ptr<SceneData>` for ownership transfer. Configurator receives raw `SceneData*` pointer (ownership remains with caller).

**Responsibilities**:
- Load raw data from storage
- Convert FlatBuffers → native structs
- Return polymorphic `SceneData*`
- Own buffer lifetime

**NOT responsible for**: Configuration, business logic

### Configurator Pattern

```cpp
class ISceneConfigurator {
public:
  virtual std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneData *data) = 0;
};
```

**Responsibilities**:
- Receive polymorphic `SceneData*`
- `dynamic_cast` to concrete type
- Apply data to game objects
- Validate data

**NOT responsible for**: Loading data, file I/O

---

## Layer Rules (CRITICAL)

### Layer 1: Data, Types & Interfaces

**Packages**: `types`, `interfaces`, `data_structures`, `events`

**Can depend on**: SFML, std, external libraries  
**Cannot depend on**: ANY other SteamRot packages

**Contains**:
- SceneData, EntityData, Component structs
- ISceneDataProvider, ISceneConfigurator interfaces
- EntityMemoryPool, ArchetypeManager
- EventPacket, EventBus

### Layer 2: Business Logic

**Packages**: `providers`, `configurators`, `logic_implementations`, `assets_system`

**Can depend on**: Layer 1, SFML, std  
**Cannot depend on**: Layer 3, other Layer 2 peers

**Contains**:
- FlatbuffersSceneDataProvider
- SaveSceneDataProvider
- FlatbuffersSceneConfigurator
- SaveSceneConfigurator
- Logic classes (UIRenderLogic, etc.)

### Layer 3: Orchestration

**Packages**: `scene_management`, `engine_core`, `display_system`

**Can depend on**: Layer 1, Layer 2, SFML, std  
**Can have**: Circular deps within Layer 3 (via interfaces)

**Contains**:
- SceneManager, SceneFactory, Scene
- GameEngine, Engine, GameLoop
- DisplayManager

---

## Code Snippets

### Creating a Scene (Default Data)

```cpp
// In SceneFactory
std::unique_ptr<Scene> CreateScene(SceneType type) {
  // Get provider and configurator
  ISceneDataProvider& provider = GetFlatbuffersSceneDataProvider();
  ISceneConfigurator& configurator = GetFlatbuffersSceneConfigurator();
  
  // Load data (polymorphic)
  std::unique_ptr<SceneData> data = provider.ProvideDefaultSceneData(type);
  
  // Create empty scene
  std::unique_ptr<Scene> scene = CreateEmptyScene(type);
  
  // Configure scene
  auto result = configurator.ConfigureScene(*scene, data.get());
  
  return scene;
}
```

### Creating a Scene (Save File) - Two-Step Process

**Key Concept**: SaveData contains SceneData. You must first load SaveData, then extract SceneData from it.

```cpp
// In SceneFactory
std::unique_ptr<Scene> CreateSceneFromSave(uint32_t save_slot) {
  // STEP 1: Load SaveData (contains metadata + scene data)
  ISaveDataProvider& save_provider = GetSaveDataProvider();
  auto save_result = save_provider.LoadSave(save_slot);
  if (!save_result.has_value()) {
    return nullptr;
  }
  
  const SaveData& save = save_result.value();
  
  // STEP 2: Extract SceneData from SaveData
  ISceneDataProvider& scene_provider = GetSaveSceneDataProvider();
  std::unique_ptr<SceneData> scene_data = 
      scene_provider.ProvideSceneDataFromSave(save, save.current_scene_type);
  
  if (!scene_data) {
    return nullptr;
  }
  
  // STEP 3: Configure scene
  ISceneConfigurator& configurator = GetSaveSceneConfigurator();
  std::unique_ptr<Scene> scene = CreateEmptyScene(save.current_scene_type);
  
  auto result = configurator.ConfigureScene(*scene, scene_data.get());
  if (!result.has_value()) {
    return nullptr;
  }
  
  return scene;
}
```

**Why Two Steps?**
- SaveData = metadata (save name, time) + scene data
- ISaveDataProvider handles save file I/O
- ISceneDataProvider extracts scene-specific data
- Separation enables reuse and testing
```

### Implementing a Provider

```cpp
class FlatbuffersSceneDataProvider : public ISceneDataProvider {
public:
  std::unique_ptr<SceneData>
  ProvideDefaultSceneData(const SceneType scene_type) const override {
    // 1. Load FlatBuffers
    FlatbuffersDataLoader loader;
    auto fb_result = loader.ProvideDefaultSceneData(scene_type);
    
    // 2. Create polymorphic struct
    auto fbs_data = std::make_unique<FbsSceneData>();
    fbs_data->scene_info.type = scene_type;
    fbs_data->scene_data_fbs = fb_result.value();
    
    // 3. Return as base pointer
    return fbs_data;
  }
};
```

### Implementing a Configurator

```cpp
class FlatbuffersSceneConfigurator : public ISceneConfigurator {
public:
  std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneData *data) override {
    // 1. Downcast to concrete type
    const FbsSceneData *fbs_data = dynamic_cast<const FbsSceneData*>(data);
    if (!fbs_data) {
      return std::unexpected(FailInfo{"Expected FbsSceneData"});
    }
    
    // 2. Extract FlatBuffers pointer
    const SceneDataFbs *fb = fbs_data->scene_data_fbs;
    
    // 3. Configure scene
    scene.m_scene_info.type = fbs_data->scene_info.type;
    // ... configure other fields
    
    return std::monostate{};
  }
};
```

---

## Common Patterns

### Saving Game State

```cpp
// 1. Capture save data
SaveData save;
save.metadata.save_name = "Player Save 1";
save.current_scene_type = GetCurrentSceneType();

// 2. Serialize scene state
SceneSerializer serializer;
SavedSceneDataFbs* scene_state = serializer.CaptureSceneState(current_scene);

// 3. Save to file
ISaveDataProvider& provider = GetSaveDataProvider();
provider.SaveGame(save);
```

### Loading Game State

```cpp
// 1. Load save metadata
ISaveDataProvider& provider = GetSaveDataProvider();
auto save_result = provider.LoadSave(slot_index);
SaveData save = save_result.value();

// 2. Create scene from save
SceneFactory factory;
std::unique_ptr<Scene> scene = factory.CreateSceneFromSave(save);

// 3. Set as current scene
SceneManager::SetCurrentScene(std::move(scene));
```

### Adding New Data Source

```cpp
// 1. Create polymorphic struct
struct JsonSceneData : public SceneData {
  nlohmann::json scene_json;
};

// 2. Implement provider
class JsonSceneDataProvider : public ISceneDataProvider {
  std::unique_ptr<SceneData>
  ProvideDefaultSceneData(const SceneType type) const override {
    // Load JSON, create JsonSceneData, return as SceneData*
  }
};

// 3. Implement configurator
class JsonSceneConfigurator : public ISceneConfigurator {
  std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneData *data) override {
    // dynamic_cast to JsonSceneData*, configure from JSON
  }
};

// 4. Register in factory
ISceneDataProvider& GetSceneDataProvider(DataSource source) {
  switch (source) {
    case DataSource::FlatBuffers:
      return GetFlatbuffersSceneDataProvider();
    case DataSource::Json:
      return GetJsonSceneDataProvider();  // NEW
  }
}
```

---

## Checklist: Adding New Component

When adding a new component, update:

- [ ] `types/components/` - Component struct
- [ ] `types/flatbuffers/` - FlatBuffers schema
- [ ] `IEntityConfigurator` - Add `ConfigureCNewComponent()` method
- [ ] `FlatbuffersEntityConfigurator` - Implement for default data
- [ ] `SaveEntityConfigurator` - Implement for save data (when Phase 3 complete)
- [ ] Component register tuple

---

## Checklist: Adding New Scene Type

When adding a new scene type, update:

- [ ] `SceneType` enum in `scene_types.fbs`
- [ ] `SceneFactory::CreateEmptyScene()` - Add case
- [ ] Create `.fbs` schema file for scene data
- [ ] Create concrete Scene class (e.g., `NewScene : public Scene`)
- [ ] Build scene data `.bin` file from JSON
- [ ] Test loading via FlatbuffersSceneDataProvider

---

## Migration Checklist

### Phase 1: Extract Interfaces ✅

- [ ] Create `src/interfaces/` directory
- [ ] Move `ISceneDataProvider.h` to `interfaces/`
- [ ] Move `ISceneConfigurator.h` to `interfaces/`
- [ ] Move `IEntityConfigurator.h` to `interfaces/`
- [ ] Move `ISaveDataProvider.h` to `interfaces/`
- [ ] Create `interfaces` CMake target
- [ ] Update all includes

### Phase 2: Refactor Configurators 🔄

- [ ] Change `ISceneConfigurator::ConfigureScene()` signature
- [ ] Update `FlatbuffersDefaultSceneConfigurator` implementation
- [ ] Update `SceneFactory` to separate provider and configurator
- [ ] Test default scene loading still works

### Phase 3: Implement Save Infrastructure ⏳

- [ ] Create `SaveSceneData` struct
- [ ] Implement `SaveSceneDataProvider`
- [ ] Implement `SaveSceneConfigurator`
- [ ] Implement `EntitySerializer`
- [ ] Update `ISaveDataProvider` with scene methods
- [ ] Test save/load round-trip

### Phase 4: Break Circular Dependencies ⏳

- [ ] Move `SceneType` to `types` package
- [ ] Ensure `providers` only depends on Layer 1
- [ ] Add linker flag `-Wl,--no-undefined`
- [ ] Verify no circular dependencies

### Phase 5: Reorganize Packages ⏳

- [ ] Rename `data_providers` → `providers`
- [ ] Create `configurators` package
- [ ] Rename `scenes` → `scene_management`
- [ ] Rename `engine` → `engine_core`
- [ ] Update all CMakeLists.txt

---

## Common Mistakes to Avoid

❌ **Configurator loads data**
```cpp
// WRONG
class MyConfigurator {
  FlatbuffersDataLoader m_loader;  // Configurator shouldn't own loader
  
  void ConfigureScene(Scene& scene) {
    auto data = m_loader.Load();  // Configurator shouldn't load data
  }
};
```

✅ **Provider loads data, configurator receives**
```cpp
// CORRECT
class MyConfigurator {
  void ConfigureScene(Scene& scene, const SceneData *data) {
    // Receives data, doesn't load it
  }
};
```

---

❌ **Layer 2 depends on Layer 3**
```cmake
# WRONG - providers CMakeLists.txt
target_link_libraries(providers PUBLIC
  scenes  # Layer 2 depending on Layer 3!
)
```

✅ **Dependencies flow downward**
```cmake
# CORRECT - providers CMakeLists.txt
target_link_libraries(providers PUBLIC
  types       # Layer 1
  interfaces  # Layer 1
)
```

---

❌ **Exposing FlatBuffers types in public API**
```cpp
// WRONG
class ISceneDataProvider {
  virtual const SceneDataFbs* LoadData() = 0;  // FlatBuffers type exposed!
};
```

✅ **Return native or polymorphic structs**
```cpp
// CORRECT
class ISceneDataProvider {
  virtual std::unique_ptr<SceneData> LoadData() = 0;  // Native struct
};
```

---

## FAQ

**Q: Why polymorphic structs instead of templates?**  
A: Polymorphism allows runtime selection of data source. Templates require compile-time knowledge of type.

**Q: Why separate provider and configurator?**  
A: Single Responsibility Principle. Provider loads data, configurator applies it. Easy to swap either independently.

**Q: Can I add new data sources without changing existing code?**  
A: Yes! Create new polymorphic struct, implement provider and configurator, register in factory. No changes to Scene or SceneFactory logic.

**Q: Why three layers?**  
A: Prevents circular dependencies, enables clean architecture, makes testing easier.

**Q: What if I need Scene to access provider?**  
A: Don't! Scene should only work with configured data. Provider is for SceneFactory use only.

**Q: How do I test without real .bin files?**  
A: Create TestSceneDataProvider that returns TestSceneData with mocked values.

**Q: SaveData contains SceneData. How does SceneFactory handle this?**  
A: Two-step process:
1. Load SaveData via `ISaveDataProvider::LoadSave(slot)` - gets metadata + scene data container
2. Extract SceneData via `ISceneDataProvider::ProvideSceneDataFromSave(SaveData&, SceneType)` - extracts scene-specific data
3. Configure Scene via `ISceneConfigurator::ConfigureScene(Scene&, SceneData*)`

SceneFactory orchestrates all three steps in `CreateSceneFromSave(slot)`.

**Why not load SceneData directly?** SaveData is broader (metadata + multiple scenes in future). Separating concerns makes each component reusable and testable.

---

## File Locations

```
src/
├── types/                        # Layer 1
│   ├── core/
│   │   ├── SceneData.h          # Base struct
│   │   ├── FbsSceneData.h       # FlatBuffers impl
│   │   └── SaveSceneData.h      # Save file impl (Phase 3)
│
├── interfaces/                   # Layer 1 (Phase 1)
│   ├── ISceneDataProvider.h
│   ├── ISceneConfigurator.h
│   ├── IEntityConfigurator.h
│   └── ISaveDataProvider.h
│
├── providers/                    # Layer 2
│   ├── FlatbuffersSceneDataProvider.h
│   ├── SaveSceneDataProvider.h  # Phase 3
│   └── TestSceneDataProvider.h  # Phase 3
│
├── configurators/                # Layer 2 (Phase 5)
│   ├── FlatbuffersSceneConfigurator.h
│   ├── SaveSceneConfigurator.h  # Phase 3
│   └── FlatbuffersEntityConfigurator.h
│
├── scene_management/             # Layer 3 (Phase 5)
│   ├── SceneManager.h
│   ├── SceneFactory.h
│   └── Scene.h
│
└── engine_core/                  # Layer 3 (Phase 5)
    ├── GameEngine.h
    └── Engine.h
```

---

## Key Principles

1. **Polymorphism over conditionals** - Use virtual methods, not switch statements
2. **Interfaces over implementations** - Depend on abstractions
3. **Providers load, configurators configure** - Single Responsibility
4. **Layers flow downward** - No upward or circular dependencies
5. **Data is polymorphic** - SceneData base, multiple implementations
6. **FlatBuffers are internal** - Never expose in public APIs

---

## Related Documents

- [DATA_ARCHITECTURE_ANALYSIS.md](DATA_ARCHITECTURE_ANALYSIS.md) - Complete analysis
- [DATA_ARCHITECTURE_DIAGRAMS.md](DATA_ARCHITECTURE_DIAGRAMS.md) - Visual diagrams
- [SAVE_LOAD_WORKFLOW_ANALYSIS.md](SAVE_LOAD_WORKFLOW_ANALYSIS.md) - Save/load details
- [DATA_PROVIDER_SYSTEM.md](../DATA_PROVIDER_SYSTEM.md) - Current provider pattern

---

**Status**: ✅ Complete Quick Reference  
**Date**: December 13, 2025  
**Last Updated**: December 13, 2025
