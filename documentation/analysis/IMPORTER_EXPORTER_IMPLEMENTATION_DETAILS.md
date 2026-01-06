# IEntityImporter/Exporter Pattern: Implementation Details

This document addresses specific questions about implementing the IEntityImporter/Exporter pattern, including how it relates to Configurators and how data flows through the SceneFactory.

## Questions Addressed

1. **Would IEntityImporter/Exporter replace the Configurator?**
2. **How would we move EntityCollectionFbs data around?**
3. **How do we pass the data to the importer in SceneFactory?**

## Answer 1: Relationship with Configurator

### Short Answer

**No, it would not replace the entire Configurator.** The Configurator would still exist but with a simplified role. The Importer/Exporter would handle **entity-specific** data transformation, while the Configurator would handle **scene-level** configuration.

### Current Responsibilities

**FlatbuffersSceneConfigurator** currently does:
1. ✅ Configure SceneInfo (UUID, type)
2. ✅ Configure SceneResources (texture size)
3. ✅ Configure SceneConfig (future: game rules, etc.)
4. ⚠️ Configure Entities (creates EntityConfigurator internally)
5. ✅ Configure LogicMap
6. ✅ Pass AssetConfig to AssetManager

**Problem**: Step 4 requires downcasting to access FbsSceneData's entity_collection

### Proposed Split of Responsibilities

After implementing IEntityImporter/Exporter pattern:

**ISceneConfigurator** (keeps most duties):
```cpp
class ISceneConfigurator {
  // KEEPS: Scene-level configuration
  virtual ConfigureSceneInfo(Scene&, const SceneData*) = 0;
  virtual ConfigureSceneResources(Scene&, const SceneData*) = 0;
  virtual ConfigureSceneConfig(Scene&, const SceneData*) = 0;
  virtual ConfigureLogicMap(Scene&) = 0;
  virtual PassAssetConfig(Scene&, const SceneData*) = 0;
  
  // REMOVES: Entity configuration
  // (no more ConfigureEntities method)
};
```

**IEntityImporter** (handles entity data):
```cpp
class IEntityImporter {
  // NEW: Entity-specific data transformation
  virtual ImportEntities(EntityMemoryPool&) = 0;
};
```

### Architectural Diagram

```
Current Pattern:
┌─────────────────────────────────────────────┐
│      FlatbuffersSceneConfigurator           │
│  ┌────────────────────────────────────────┐ │
│  │ ConfigureSceneInfo()                   │ │
│  │ ConfigureSceneResources()              │ │
│  │ ConfigureSceneConfig()                 │ │
│  │ ConfigureEntities() ← REQUIRES DOWNCAST│ │
│  │ ConfigureLogicMap()                    │ │
│  │ PassAssetConfig()                      │ │
│  └────────────────────────────────────────┘ │
└─────────────────────────────────────────────┘

Proposed Pattern:
┌─────────────────────────────────────────────┐
│      FlatbuffersSceneConfigurator           │
│  ┌────────────────────────────────────────┐ │
│  │ ConfigureSceneInfo()                   │ │
│  │ ConfigureSceneResources()              │ │
│  │ ConfigureSceneConfig()                 │ │
│  │ ConfigureLogicMap()                    │ │
│  │ PassAssetConfig()                      │ │
│  └────────────────────────────────────────┘ │
└─────────────────────────────────────────────┘
                    +
┌─────────────────────────────────────────────┐
│      FlatbuffersEntityImporter              │
│  ┌────────────────────────────────────────┐ │
│  │ ImportEntities(emp)                    │ │
│  │   (stores EntityCollectionFbs ref)     │ │
│  └────────────────────────────────────────┘ │
└─────────────────────────────────────────────┘
```

## Answer 2: How EntityCollectionFbs Moves Around

### Current Flow (With Downcasting)

```cpp
// Step 1: FlatbuffersDataLoader loads binary file
const SceneDataFbs *fb_root = /* loaded from file */;

// Step 2: FlatbuffersSceneDataProvider creates FbsSceneData
FbsSceneData scene_data;
scene_data.entity_collection = fb_root->entity_collection();  // Attach pointer
return make_unique<FbsSceneData>(scene_data);

// Step 3: SceneFactory receives as base pointer
unique_ptr<SceneData> data = provider.ProvideDefaultSceneData(type);

// Step 4: Configurator downcasts to access entity_collection
FbsSceneData *fbs_data = dynamic_cast<FbsSceneData*>(scene_data);
const EntityCollectionFbs *entities = fbs_data->entity_collection;

// Step 5: Create EntityConfigurator with entities
FlatbuffersEntityConfigurator configurator(event_handler, *entities);
```

**Problem**: Step 4 requires unsafe downcast + const_cast

### Proposed Flow (With Importer)

```cpp
// Step 1: FlatbuffersDataLoader loads binary file
const SceneDataFbs *fb_root = /* loaded from file */;

// Step 2: FlatbuffersSceneDataProvider creates lightweight SceneData
SceneData scene_data;  // No derived type, no entity pointer
scene_data.scene_info = /* ... */;
scene_data.scene_resources_config = /* ... */;
scene_data.scene_asset_config = /* ... */;
// NOTE: entity_collection NOT stored in SceneData

// Step 3: Create Importer separately with entity data
unique_ptr<IEntityImporter> importer = 
    make_unique<FlatbuffersEntityImporter>(
        event_handler,
        *fb_root->entity_collection());  // Direct reference

// Step 4: SceneFactory receives BOTH SceneData and Importer
// (see detailed implementation below)

// Step 5: Use importer directly (no downcasting)
auto result = importer->ImportEntities(scene.GetEntityMemoryPool());
```

**Key Difference**: EntityCollectionFbs is passed directly to Importer constructor, not smuggled through SceneData inheritance.

## Answer 3: SceneFactory Implementation with Importer

### Current SceneFactory::CreateSceneFromDefault

```cpp
// src/scenes/SceneFactory.cpp (current)
std::expected<unique_ptr<Scene>, FailInfo>
SceneFactory::CreateSceneFromDefault(SceneType type) {
  
  // Get provider
  ISceneDataProvider &provider = 
      *m_game_context.data_access_factory.GetSceneDataProvider().value();
  
  // Provider loads SceneData (with FbsSceneData containing entity pointer)
  unique_ptr<SceneData> data = provider.ProvideDefaultSceneData(type).value();
  
  // Create scene from data (configurator handles entities internally)
  return CreateSceneFromData(data.get());
}
```

### Proposed SceneFactory with Importer - Option A (Provider Creates Both)

**Change Provider Interface** to return both SceneData and Importer:

```cpp
// src/interfaces/ISceneDataProvider.h (updated)
struct SceneLoadData {
  unique_ptr<SceneData> scene_data;
  unique_ptr<IEntityImporter> entity_importer;
};

class ISceneDataProvider {
  // Updated method signature
  virtual expected<SceneLoadData, FailInfo>
  ProvideDefaultSceneLoadData(SceneType scene_type) const = 0;
};
```

**Implementation in FlatbuffersSceneDataProvider**:

```cpp
// src/data_providers/FlatbuffersSceneDataProvider.cpp (updated)
expected<SceneLoadData, FailInfo>
FlatbuffersSceneDataProvider::ProvideDefaultSceneLoadData(
    SceneType scene_type) const {
  
  // Load FlatBuffers data
  FlatbuffersDataLoader data_loader;
  auto load_result = data_loader.ProvideDefaultSceneData(scene_type);
  if (!load_result)
    return unexpected(load_result.error());
  
  const SceneDataFbs &fb_data = *load_result.value();
  
  // Create lightweight SceneData (no entity pointer)
  auto scene_data = make_unique<SceneData>();
  
  // Configure SceneInfo
  auto info_result = ConfigureSceneInfo(
      scene_data->scene_info, 
      fb_data.scene_info());
  if (!info_result)
    return unexpected(info_result.error());
  
  // Configure SceneResourcesConfig
  auto resources_result = ConfigureSceneResourcesConfig(
      scene_data->scene_resources_config,
      fb_data.scene_resources_config());
  if (!resources_result)
    return unexpected(resources_result.error());
  
  // Configure AssetConfig
  auto asset_result = ConfigureAssetConfig(
      scene_data->scene_asset_config,
      fb_data.asset_config());
  if (!asset_result)
    return unexpected(asset_result.error());
  
  // Create Importer with entity data (SEPARATE from SceneData)
  auto entity_importer = make_unique<FlatbuffersEntityImporter>(
      m_event_handler,  // Need to pass this
      *fb_data.entity_collection());
  
  // Return both
  return SceneLoadData{
    .scene_data = move(scene_data),
    .entity_importer = move(entity_importer)
  };
}
```

**Updated SceneFactory**:

```cpp
// src/scenes/SceneFactory.cpp (updated)
expected<unique_ptr<Scene>, FailInfo>
SceneFactory::CreateSceneFromDefault(SceneType type) {
  
  // Get provider
  ISceneDataProvider &provider = 
      *m_game_context.data_access_factory.GetSceneDataProvider().value();
  
  // Provider loads BOTH SceneData and Importer
  auto load_result = provider.ProvideDefaultSceneLoadData(type);
  if (!load_result)
    return unexpected(load_result.error());
  
  SceneLoadData &load_data = load_result.value();
  
  // Create empty scene
  auto scene_result = CreateEmptyScene(load_data.scene_data->scene_info.type);
  if (!scene_result)
    return unexpected(scene_result.error());
  
  unique_ptr<Scene> scene = move(scene_result.value());
  
  // Configure scene-level settings
  ISceneConfigurator &configurator = GetSceneConfigurator();
  auto config_result = configurator.ConfigureScene(
      *scene, 
      load_data.scene_data.get());
  if (!config_result)
    return unexpected(config_result.error());
  
  // Import entities (NO DOWNCASTING)
  auto import_result = load_data.entity_importer->ImportEntities(
      scene->GetSceneContext().scene_entities);
  if (!import_result)
    return unexpected(import_result.error());
  
  return move(scene);
}
```

### Proposed SceneFactory with Importer - Option B (Factory Creates Importer)

**Keep Provider simpler**, Factory creates Importer itself:

```cpp
// src/scenes/SceneFactory.cpp (alternative approach)
expected<unique_ptr<Scene>, FailInfo>
SceneFactory::CreateSceneFromDefault(SceneType type) {
  
  // Step 1: Load FlatBuffers data directly
  FlatbuffersDataLoader data_loader;
  auto fb_result = data_loader.ProvideDefaultSceneData(type);
  if (!fb_result)
    return unexpected(fb_result.error());
  
  const SceneDataFbs &fb_data = *fb_result.value();
  
  // Step 2: Get provider to create SceneData (lightweight)
  ISceneDataProvider &provider = 
      *m_game_context.data_access_factory.GetSceneDataProvider().value();
  
  auto scene_data_result = provider.ProvideSceneDataFromData(&fb_data);
  if (!scene_data_result)
    return unexpected(scene_data_result.error());
  
  unique_ptr<SceneData> scene_data = move(scene_data_result.value());
  
  // Step 3: Create empty scene
  auto scene_result = CreateEmptyScene(scene_data->scene_info.type);
  if (!scene_result)
    return unexpected(scene_result.error());
  
  unique_ptr<Scene> scene = move(scene_result.value());
  
  // Step 4: Configure scene-level settings
  ISceneConfigurator &configurator = GetSceneConfigurator();
  auto config_result = configurator.ConfigureScene(*scene, scene_data.get());
  if (!config_result)
    return unexpected(config_result.error());
  
  // Step 5: Create Importer with entity data (Factory does this)
  auto entity_importer = make_unique<FlatbuffersEntityImporter>(
      scene->GetSceneContext().event_handler,
      *fb_data.entity_collection());
  
  // Step 6: Import entities
  auto import_result = entity_importer->ImportEntities(
      scene->GetSceneContext().scene_entities);
  if (!import_result)
    return unexpected(import_result.error());
  
  return move(scene);
}
```

### Option Comparison

| Aspect | Option A (Provider Creates Both) | Option B (Factory Creates Importer) |
|--------|----------------------------------|-------------------------------------|
| **Provider responsibility** | Returns SceneData + Importer | Returns SceneData only |
| **Factory responsibility** | Receives both, uses both | Creates Importer itself |
| **Data lifetime** | Managed by Provider | FlatBuffers data must stay valid |
| **Flexibility** | Provider controls importer type | Factory controls importer type |
| **Complexity** | New `SceneLoadData` struct | Uses existing patterns |

**Recommendation**: **Option A** is cleaner because:
- Provider manages FlatBuffers data lifetime
- Factory doesn't need to know about FlatBuffers
- More encapsulation

## Data Flow Diagrams

### Current Flow (With Downcasting)

```
┌──────────────────┐
│ SceneFactory     │
│ .CreateFrom      │
│  Default()       │
└────────┬─────────┘
         │
         │ 1. Get provider
         ▼
┌─────────────────────────────────┐
│ FlatbuffersSceneDataProvider    │
│  .ProvideDefaultSceneData()     │
│                                 │
│  ┌──────────────────────────┐   │
│  │ Load FlatBuffers binary  │   │
│  └──────────┬───────────────┘   │
│             │                   │
│             ▼                   │
│  ┌──────────────────────────┐   │
│  │ Create FbsSceneData:     │   │
│  │   - scene_info           │   │
│  │   - scene_resources      │   │
│  │   - entity_collection*───┼───┼──> EntityCollectionFbs
│  └──────────────────────────┘   │      (FlatBuffers data)
│                                 │
│  return unique_ptr<SceneData>   │
└────────┬────────────────────────┘
         │
         │ 2. SceneData (actually FbsSceneData)
         ▼
┌──────────────────┐
│ SceneFactory     │
│ .CreateScene     │
│  FromData()      │
└────────┬─────────┘
         │
         │ 3. Pass SceneData*
         ▼
┌─────────────────────────────────┐
│ FlatbuffersSceneConfigurator    │
│  .ConfigureEntities()           │
│                                 │
│  ┌──────────────────────────┐   │
│  │ const SceneData* data    │   │
│  │         ↓                │   │
│  │ dynamic_cast + const_cast│   │ ⚠️ UNSAFE
│  │         ↓                │   │
│  │ FbsSceneData* fbs_data   │   │
│  │         ↓                │   │
│  │ fbs_data->entity_        │   │
│  │         collection*──────┼───┼──> Access FlatBuffers
│  └──────────────────────────┘   │
└─────────────────────────────────┘
```

### Proposed Flow (With Importer - Option A)

```
┌──────────────────┐
│ SceneFactory     │
│ .CreateFrom      │
│  Default()       │
└────────┬─────────┘
         │
         │ 1. Get provider
         ▼
┌───────────────────────────────────────────┐
│ FlatbuffersSceneDataProvider              │
│  .ProvideDefaultSceneLoadData()           │
│                                           │
│  ┌────────────────────────────────────┐   │
│  │ Load FlatBuffers binary            │   │
│  └──────────┬─────────────────────────┘   │
│             │                             │
│             ▼                             │
│  ┌─────────────────────────────────────┐  │
│  │ SceneData (lightweight):            │  │
│  │   - scene_info                      │  │
│  │   - scene_resources                 │  │
│  │   - asset_config                    │  │
│  │   (NO entity pointer)               │  │
│  └─────────────────────────────────────┘  │
│             +                             │
│  ┌─────────────────────────────────────┐  │
│  │ FlatbuffersEntityImporter:          │  │
│  │   (event_handler,                   │  │
│  │    entity_collection) ──────────────┼──┼──> EntityCollectionFbs
│  └─────────────────────────────────────┘  │      (FlatBuffers data)
│                                           │
│  return SceneLoadData {                   │
│    scene_data, entity_importer }          │
└────────┬──────────────────────────────────┘
         │
         │ 2. SceneLoadData
         ▼
┌──────────────────┐
│ SceneFactory     │
│                  │
│  ┌────────────────────────────────────┐
│  │ Scene-level configuration:         │
│  │   ConfigureScene(scene, scene_data)│
│  └────────────────────────────────────┘
│             +
│  ┌────────────────────────────────────┐
│  │ Entity import:                     │
│  │   importer->ImportEntities(emp)    │  ✓ NO DOWNCAST
│  └────────────────────────────────────┘
└─────────────────────────────────────────┘
```

## Key Benefits of Importer Pattern

1. **No Downcasting**: EntityCollectionFbs passed directly to importer
2. **Type Safety**: Compile-time correctness, no `dynamic_cast`
3. **Clear Separation**: Scene config vs. entity import are distinct operations
4. **Extensibility**: Easy to add JsonEntityImporter, NetworkEntityImporter, etc.
5. **Testability**: Can mock IEntityImporter independently
6. **Symmetry**: Export uses IEntityExporter with same pattern

## Migration Strategy

### Phase 1: Add Importer Alongside Current Code

1. Create `IEntityImporter` interface
2. Implement `FlatbuffersEntityImporter` (copy logic from current EntityConfigurator)
3. Update Provider to return `SceneLoadData` struct
4. Update SceneFactory to use importer
5. Keep `FbsSceneData` temporarily for other uses

### Phase 2: Remove Old Pattern

1. Remove `ConfigureEntities` from `ISceneConfigurator`
2. Remove `FbsSceneData` (no longer needed)
3. Update all configurator implementations
4. Update tests

## Summary Table

| Question | Answer |
|----------|--------|
| **Replace Configurator?** | No. Configurator keeps scene-level config. Importer handles entity-specific import. |
| **Move EntityCollectionFbs?** | Provider creates Importer with EntityCollectionFbs reference. Factory receives both SceneData and Importer. |
| **Pass to Importer?** | Option A: Provider returns `SceneLoadData{scene_data, importer}`. Factory uses both. Option B: Factory loads FlatBuffers, creates Importer itself. |

---

**Document Version**: 1.0  
**Date**: 2026-01-06  
**Related**: SCENEDATA_ARCHITECTURE_ANALYSIS.md, SCENEDATA_CODE_EXAMPLES.md  
**Addresses**: Comment #3714460348 from @walliscode
