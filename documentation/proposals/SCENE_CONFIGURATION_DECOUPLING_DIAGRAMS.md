# Scene Configuration Decoupling - Architecture Diagrams

**Visual representations of the proposed architecture changes.**

---

## Table of Contents

1. [Current Architecture (Before)](#current-architecture-before)
2. [Proposed Architecture (After)](#proposed-architecture-after)
3. [Data Flow Comparison](#data-flow-comparison)
4. [Class Hierarchy](#class-hierarchy)
5. [Sequence Diagrams](#sequence-diagrams)
6. [Migration Timeline](#migration-timeline)

---

## Current Architecture (Before)

### System Overview - Current State

```
┌─────────────────────────────────────────────────────────────────┐
│                         SceneManager                            │
│                                                                 │
│  - Manages scene lifecycle                                      │
│  - Calls SceneFactory::CreateDefaultScene()                    │
│  - ❌ No control over data source                              │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ├─────────────────────┐
                              ▼                     ▼
┌──────────────────────────────────────┐  ┌─────────────────────┐
│          SceneFactory                │  │  ISceneDataProvider │
│                                      │  │                     │
│  - CreateDefaultScene()              │  │  - LoadSceneData()  │
│  - ❌ Hardcoded to "default" data   │  │  ❌ Only used for   │
│  - ❌ Calls Scene::ConfigureFromDef()│  │     render texture  │
└──────────────────────────────────────┘  └─────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                            Scene                                │
│                                                                 │
│  - ConfigureFromDefault(DataType)                              │
│  - ❌ Knows about "default" data source                        │
│  - ❌ Tightly coupled to EntityManager                         │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                       EntityManager                             │
│                                                                 │
│  - ConfigureEntitiesFromDefaultData(SceneType, DataType)       │
│  - ❌ Creates FlatbuffersConfigurator internally                │
│  - ❌ Knows about "default" data                               │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                  FlatbuffersConfigurator                        │
│                                                                 │
│  - ConfigureEntitiesFromDefaultData(EMP&, SceneType)           │
│  - ❌ Uses FlatbuffersDataLoader internally                     │
│  - ❌ Fetches data based on SceneType                          │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                  FlatbuffersDataLoader                          │
│                                                                 │
│  - ProvideDefaultSceneData(SceneType)                          │
│  - Returns: const SceneDataData* (FlatBuffers type)            │
│  - ❌ FlatBuffers types exposed                                 │
└─────────────────────────────────────────────────────────────────┘
```

### Problems Highlighted

```
┌─────────────────────────────────────────────────────────────────┐
│                     PROBLEMS WITH CURRENT DESIGN                │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  1. ❌ "Default" in method names throughout stack              │
│     SceneFactory::CreateDefaultScene()                         │
│     Scene::ConfigureFromDefault()                              │
│     EntityManager::ConfigureEntitiesFromDefaultData()          │
│     FlatbuffersConfigurator::ConfigureEntitiesFromDefaultData()│
│                                                                 │
│  2. ❌ Data source decision at wrong layer                     │
│     SceneFactory decides to use "default"                      │
│     SceneManager just calls CreateDefaultScene()               │
│                                                                 │
│  3. ❌ FlatBuffers coupling                                    │
│     FlatBuffers types (SceneDataData*, EntityCollection*)      │
│     appear in call chain                                       │
│                                                                 │
│  4. ❌ No save/load support                                    │
│     No path to load from ISaveDataProvider                     │
│     Would need duplicate code for saved scenes                 │
│                                                                 │
│  5. ❌ SceneType passed down stack                             │
│     SceneType travels down to fetch data internally            │
│     Tight coupling to data loading                             │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## Proposed Architecture (After)

### System Overview - Proposed State

```
┌─────────────────────────────────────────────────────────────────┐
│                         SceneManager                            │
│                                                                 │
│  - Manages scene lifecycle                                      │
│  - ✅ Controls data source decision                            │
│  - ✅ Fetches data from ISceneDataProvider OR ISaveDataProvider│
│  - ✅ Creates appropriate IEntityConfigurator                  │
│  - ✅ Calls scene_factory::CreateScene with data               │
└─────────────────────────────────────────────────────────────────┘
          │                               │
          │ Fetches data                  │ Calls factory
          ▼                               ▼
┌────────────────────┐         ┌────────────────────────────┐
│ISceneDataProvider  │         │    scene_factory           │
│   OR               │         │                            │
│ISaveDataProvider   │         │ - CreateScene(type,        │
│                    │         │     context,               │
│ ✅ Used properly   │         │     configurator,          │
└────────────────────┘         │     data)                  │
                               │                            │
                               │ ✅ No data fetching        │
                               │ ✅ Receives configurator   │
                               └────────────────────────────┘
                                              │
                                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                            Scene                                │
│                                                                 │
│  - Configure(IEntityConfigurator&, const void*)                │
│  - ✅ Data-source agnostic                                     │
│  - ✅ No "default" knowledge                                   │
│  - ✅ Delegates to EntityManager                               │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                       EntityManager                             │
│                                                                 │
│  - Configure(IEntityConfigurator&, const void*)                │
│  - ✅ Receives configurator from outside                       │
│  - ✅ No data source knowledge                                 │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                    IEntityConfigurator                          │
│                     (Abstract Interface)                        │
│                                                                 │
│  + ConfigureEntities(EntityMemoryPool&, const void*)           │
│                                                                 │
│  ✅ Abstract interface for any data format                     │
│  ✅ Hides format details from Scene/EntityManager              │
└─────────────────────────────────────────────────────────────────┘
                              │
                 ┌────────────┴──────────────┬─────────────────┐
                 ▼                           ▼                 ▼
┌──────────────────────────┐  ┌───────────────────┐  ┌──────────────┐
│FlatbuffersConfigurator   │  │SaveDataConfigurator│  │JsonConfigurator│
│                          │  │                    │  │              │
│✅ FlatBuffers format    │  │✅ SaveData format  │  │✅ JSON format│
│✅ ConfigureEntities()   │  │✅ ConfigureEntities│  │✅ (Future)   │
└──────────────────────────┘  └───────────────────┘  └──────────────┘
```

### Benefits Highlighted

```
┌─────────────────────────────────────────────────────────────────┐
│                   BENEFITS OF PROPOSED DESIGN                   │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  1. ✅ Separation of Concerns                                  │
│     - SceneManager: Orchestrates, decides data source          │
│     - scene_factory: Creates scene objects                     │
│     - Scene: Domain model, format-agnostic                     │
│     - IEntityConfigurator: Format adapter                      │
│                                                                 │
│  2. ✅ Data source control at right layer                      │
│     SceneManager decides:                                      │
│       - New game? Use ISceneDataProvider                       │
│       - Load game? Use ISaveDataProvider                       │
│                                                                 │
│  3. ✅ FlatBuffers isolated                                    │
│     FlatBuffers types only in FlatbuffersConfigurator          │
│     Scene/SceneFactory never see FlatBuffers                   │
│                                                                 │
│  4. ✅ Save/load support enabled                               │
│     AddSceneFromDefault() uses ISceneDataProvider              │
│     AddSceneFromSave() uses ISaveDataProvider                  │
│     Both flow through same code path                           │
│                                                                 │
│  5. ✅ Format flexibility                                      │
│     Easy to add JSON, XML, Network configurators               │
│     Just implement IEntityConfigurator                         │
│                                                                 │
│  6. ✅ Direct EMP configuration                                │
│     Pass EntityMemoryPool& to configurator                     │
│     No intermediate copying                                    │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## Data Flow Comparison

### Current Data Flow (Problems)

```
AddSceneFromDefault(SceneType)
  │
  └─> CreateDefaultScene(SceneType)
       │
       ├─> new TitleScene()
       │
       ├─> ISceneDataProvider::LoadSceneData()  [Only for texture size]
       │
       └─> Scene::ConfigureFromDefault()
            │
            └─> EntityManager::ConfigureEntitiesFromDefaultData(SceneType)
                 │
                 └─> FlatbuffersConfigurator::ConfigureEntitiesFromDefaultData(EMP&, SceneType)
                      │
                      └─> FlatbuffersDataLoader::ProvideDefaultSceneData(SceneType)
                           │
                           └─> Returns SceneDataData* (FlatBuffers)
                                │
                                └─> ConfigureEntitiesFromCollection(EMP&, EntityCollection*)

Problems:
  ❌ SceneType passed down stack
  ❌ Data fetched at bottom of stack
  ❌ FlatBuffers types in call chain
  ❌ "Default" assumption throughout
```

### Proposed Data Flow (Solutions)

```
AddSceneFromDefault(SceneType)
  │
  ├─> ISceneDataProvider::LoadSceneData(SceneType)
  │    └─> Returns SceneData (native C++ struct)
  │
  ├─> FlatbuffersDataLoader::ProvideDefaultSceneData(SceneType)
  │    └─> Returns EntityCollection* (FlatBuffers)
  │
  ├─> new FlatbuffersConfigurator(event_handler)
  │
  └─> scene_factory::CreateScene(SceneType, GameContext, 
                                  configurator&, entity_data)
       │
       ├─> new TitleScene()
       │
       ├─> Configure render texture from SceneData
       │
       └─> Scene::Configure(configurator&, entity_data)
            │
            └─> EntityManager::Configure(configurator&, entity_data)
                 │
                 └─> configurator.ConfigureEntities(m_entity_memory_pool&, entity_data)
                      │
                      └─> FlatbuffersConfigurator::ConfigureEntities(EMP&, const void*)
                           │
                           ├─> Cast: const EntityCollection* = static_cast<...>
                           │
                           └─> Configure pool in-place

Solutions:
  ✅ Data fetched at top of stack
  ✅ Data flows down as parameter
  ✅ const void* hides format from Scene
  ✅ No "default" assumptions
  ✅ SceneManager controls data source
```

### Side-by-Side Call Stacks

```
CURRENT (Bad)                      │  PROPOSED (Good)
───────────────────────────────────┼──────────────────────────────────
SceneManager::AddSceneFromDefault  │  SceneManager::AddSceneFromDefault
  └─> SceneFactory::               │    ├─> ISceneDataProvider::LoadSceneData()
      CreateDefaultScene           │    ├─> FlatbuffersDataLoader::ProvideDefaultSceneData()
       └─> Scene::                 │    └─> scene_factory::CreateScene(
           ConfigureFromDefault    │         type, context, configurator&, data)
            └─> EntityManager::    │          └─> Scene::Configure(configurator&, data)
                ConfigureEntities  │               └─> EntityManager::Configure(
                FromDefaultData    │                    configurator&, data)
                 └─> Flatbuffers   │                    └─> configurator.ConfigureEntities(
                     Configurator:: │                         pool&, data)
                     Configure      │
                     EntitiesFrom   │
                     DefaultData    │
                      └─> Flatbuf  │
                          fersData │
                          Loader:: │
                          Provide  │
                          DefaultS │
                          ceneData │
                                   │
❌ 7 levels deep                   │  ✅ 5 levels deep
❌ "Default" in 4 method names     │  ✅ No "default" in names
❌ Data fetched at bottom          │  ✅ Data fetched at top
❌ SceneType passed down           │  ✅ Data passed down
❌ FlatBuffers types exposed       │  ✅ const void* hides format
```

---

## Class Hierarchy

### Current Configurator Hierarchy

```
┌──────────────────────┐
│ EntityConfigurator   │  (Concrete base class)
│                      │
│ + m_event_handler    │  ❌ No abstract interface
└──────────────────────┘  ❌ No polymorphism
          △
          │
          │ extends
          │
┌──────────────────────────────────┐
│  FlatbuffersConfigurator         │
│                                  │
│  + ConfigureEntitiesFromDefault  │  ❌ Specific method name
│    Data(EMP&, SceneType)         │  ❌ SceneType parameter
│                                  │
│  + ConfigureEntitiesFromCollect  │  ✅ Takes EMP& by reference
│    ion(EMP&, EntityCollection*)  │  ❌ FlatBuffers type exposed
└──────────────────────────────────┘
```

### Proposed Configurator Hierarchy

```
┌──────────────────────────────────┐
│   IEntityConfigurator            │  ✅ Abstract interface
│   (Abstract Interface)           │
│                                  │
│ + ConfigureEntities(             │  ✅ Pure virtual
│     EntityMemoryPool&,           │  ✅ EMP by reference
│     const void*)                 │  ✅ Opaque data pointer
│   = 0                            │
└──────────────────────────────────┘
          △
          │ implements
          │
┌──────────────────────────────────┐
│   EntityConfigurator             │  ✅ Base implementation
│   (Base Class)                   │
│                                  │
│ # m_event_handler                │  ✅ Protected for subclasses
│                                  │
│ + ConfigureEntities(EMP&,        │  ✅ Still pure virtual
│     const void*) = 0             │     (subclasses implement)
└──────────────────────────────────┘
          △
          ├─────────────┬─────────────┬──────────────┐
          │             │             │              │
┌─────────────────┐ ┌────────────┐ ┌───────────┐ ┌──────────┐
│Flatbuffers      │ │SaveData    │ │Json       │ │Network   │
│Configurator     │ │Configurator│ │Configurator│ │Configurator│
│                 │ │            │ │           │ │          │
│✅ Implements    │ │✅ Implements│ │✅ Implements│ │✅ Future │
│ConfigureEntities│ │ConfigureEnt│ │ConfigureEn│ │         │
│                 │ │ities       │ │tities     │ │         │
│Expects:         │ │            │ │           │ │         │
│EntityCollection*│ │Expects:    │ │Expects:   │ │Expects: │
│(FlatBuffers)    │ │SaveData*   │ │json*      │ │NetData* │
└─────────────────┘ └────────────┘ └───────────┘ └──────────┘
```

### Scene Class Evolution

```
BEFORE:                              AFTER:
┌──────────────────────────────┐    ┌──────────────────────────────┐
│         Scene                │    │         Scene                │
│                              │    │                              │
│ + ConfigureFromDefault(      │    │ + Configure(                │
│     DataType) ❌             │    │     IEntityConfigurator&,   │
│                              │    │     const void*) ✅         │
│   - Knows about "default"    │    │                              │
│   - Knows about DataType enum│    │   - Data-source agnostic    │
│   - Calls EntityManager::    │    │   - Format agnostic         │
│     ConfigureEntitiesFrom    │    │   - Delegates to             │
│     DefaultData              │    │     EntityManager::Configure │
└──────────────────────────────┘    └──────────────────────────────┘
```

---

## Sequence Diagrams

### Sequence 1: Current - Creating Scene from Default Data

```
SceneManager  SceneFactory  Scene  EntityManager  FlatbuffersConfig  FlatbuffersDataLoader
     │             │          │          │                │                  │
     │─────────────>          │          │                │                  │
     │AddSceneFrom │          │          │                │                  │
     │Default      │          │          │                │                  │
     │             │          │          │                │                  │
     │             │──────────>          │                │                  │
     │             │CreateDef │          │                │                  │
     │             │aultScene │          │                │                  │
     │             │          │          │                │                  │
     │             │          │──────────>                │                  │
     │             │          │Configure │                │                  │
     │             │          │FromDefault                │                  │
     │             │          │          │                │                  │
     │             │          │          │────────────────>                  │
     │             │          │          │ConfigureEntit  │                  │
     │             │          │          │iesFromDefault  │                  │
     │             │          │          │Data            │                  │
     │             │          │          │                │                  │
     │             │          │          │                │──────────────────>
     │             │          │          │                │ProvideDefaultSce │
     │             │          │          │                │neData(SceneType) │
     │             │          │          │                │                  │
     │             │          │          │                │<──────────────────
     │             │          │          │                │SceneDataData*    │
     │             │          │          │                │                  │
     │             │          │          │<────────────────                  │
     │             │          │<──────────                │                  │
     │             │<──────────          │                │                  │
     │<─────────────          │          │                │                  │

❌ Problems:
   - SceneType passed down 4 levels
   - Data fetched at deepest level
   - FlatBuffers type returned up stack
   - "Default" in 4 method names
```

### Sequence 2: Proposed - Creating Scene from Default Data

```
SceneManager  ISceneDataProv  FlatbuffersDataL  scene_factory  Scene  EntityManager  Configurator
     │              │                │               │           │          │              │
     │──────────────>               │               │           │          │              │
     │LoadSceneData │               │               │           │          │              │
     │<──────────────               │               │           │          │              │
     │SceneData     │               │               │           │          │              │
     │              │               │               │           │          │              │
     │──────────────────────────────>               │           │          │              │
     │ProvideDefaultSceneData       │               │           │          │              │
     │<──────────────────────────────               │           │          │              │
     │EntityCollection*             │               │           │          │              │
     │                              │               │           │          │              │
     │──────────────────────────────────────────────>           │          │              │
     │CreateScene(type, context, configurator, data)│           │          │              │
     │                              │               │           │          │              │
     │                              │               │───────────>          │              │
     │                              │               │Configure  │          │              │
     │                              │               │(config,   │          │              │
     │                              │               │data)      │          │              │
     │                              │               │           │          │              │
     │                              │               │           │──────────>              │
     │                              │               │           │Configure  │              │
     │                              │               │           │(config,   │              │
     │                              │               │           │data)      │              │
     │                              │               │           │          │              │
     │                              │               │           │          │──────────────>
     │                              │               │           │          │ConfigureEnti  │
     │                              │               │           │          │ties(pool&,    │
     │                              │               │           │          │data)          │
     │                              │               │           │          │              │
     │                              │               │           │          │<──────────────
     │                              │               │           │<──────────              │
     │                              │               │<───────────          │              │
     │<──────────────────────────────────────────────           │          │              │
     │Scene*                        │               │           │          │              │

✅ Solutions:
   - Data fetched at top level (SceneManager)
   - Data flows down as parameter
   - No "default" in method names
   - Format hidden by const void*
   - Configurator polymorphism
```

### Sequence 3: Proposed - Creating Scene from Save Data (Future)

```
SceneManager  ISaveDataProv  scene_factory  Scene  EntityManager  SaveDataConfigurator
     │              │              │          │          │                │
     │──────────────>              │          │          │                │
     │LoadSave(slot)│              │          │          │                │
     │<──────────────              │          │          │                │
     │SaveData      │              │          │          │                │
     │              │              │          │          │                │
     │──────────────────────────────>         │          │                │
     │CreateScene(type, context,   │         │          │                │
     │  saveDataConfig, &saveData) │         │          │                │
     │                              │         │          │                │
     │                              │─────────>          │                │
     │                              │Configure│          │                │
     │                              │(config, │          │                │
     │                              │&save)   │          │                │
     │                              │         │          │                │
     │                              │         │──────────>                │
     │                              │         │Configure  │                │
     │                              │         │(config,   │                │
     │                              │         │&save)     │                │
     │                              │         │          │                │
     │                              │         │          │────────────────>
     │                              │         │          │ConfigureEntities│
     │                              │         │          │(pool&, &save)   │
     │                              │         │          │                │
     │                              │         │          │<────────────────
     │                              │         │<──────────                │
     │                              │<─────────          │                │
     │<──────────────────────────────         │          │                │
     │Scene*                        │         │          │                │

✅ Same flow as default data!
   - Only difference: ISaveDataProvider vs ISceneDataProvider
   - Only difference: SaveDataConfigurator vs FlatbuffersConfigurator
   - Scene/EntityManager code unchanged
   - Perfect separation of concerns
```

---

## Migration Timeline

### Phase-by-Phase Visual

```
Phase 1: IEntityConfigurator Interface (1-2 days)
┌────────────────────────────────────────────────────────┐
│ Create:                                                │
│   ✅ IEntityConfigurator.h (abstract interface)       │
│   ✅ Pure virtual ConfigureEntities method             │
│                                                        │
│ Update:                                                │
│   ✅ EntityConfigurator inherits from interface        │
│                                                        │
│ Result:                                                │
│   - Abstract interface exists                          │
│   - Backward compatible                                │
│   - All tests pass                                     │
└────────────────────────────────────────────────────────┘

Phase 2: Refactor FlatbuffersConfigurator (2-3 days)
┌────────────────────────────────────────────────────────┐
│ Update:                                                │
│   ✅ FlatbuffersConfigurator::ConfigureEntities()     │
│   ⚠️  Keep ConfigureEntitiesFromDefaultData() temp    │
│                                                        │
│ Result:                                                │
│   - New method works                                   │
│   - Old method still works (backward compat)           │
│   - Tests for both                                     │
└────────────────────────────────────────────────────────┘

Phase 3: Scene Configuration (2-3 days)
┌────────────────────────────────────────────────────────┐
│ Add:                                                   │
│   ✅ Scene::Configure(config&, data)                  │
│   ✅ EntityManager::Configure(config&, data)          │
│                                                        │
│ Deprecate:                                             │
│   ⚠️  Scene::ConfigureFromDefault() (mark with comment)│
│                                                        │
│ Result:                                                │
│   - New API exists                                     │
│   - Old API still works                                │
│   - Tests updated                                      │
└────────────────────────────────────────────────────────┘

Phase 4: SceneFactory (2-3 days)
┌────────────────────────────────────────────────────────┐
│ Create:                                                │
│   ✅ scene_factory namespace                          │
│   ✅ CreateScene(type, ctx, config&, data)            │
│                                                        │
│ Keep:                                                  │
│   ⚠️  SceneFactory::CreateDefaultScene() (temp)       │
│                                                        │
│ Result:                                                │
│   - New factory function works                         │
│   - Old class still works                              │
│   - Both tested                                        │
└────────────────────────────────────────────────────────┘

Phase 5: SceneManager (3-4 days)
┌────────────────────────────────────────────────────────┐
│ Update:                                                │
│   ✅ AddSceneFromDefault() uses new flow              │
│   ✅ Fetches data from ISceneDataProvider              │
│   ✅ Creates FlatbuffersConfigurator                  │
│   ✅ Calls scene_factory::CreateScene                 │
│                                                        │
│ Add:                                                   │
│   ✅ AddSceneFromSave() stub                          │
│                                                        │
│ Result:                                                │
│   - New flow end-to-end                                │
│   - All integration points updated                     │
│   - Full test coverage                                 │
└────────────────────────────────────────────────────────┘

Phase 6: Cleanup (1-2 days)
┌────────────────────────────────────────────────────────┐
│ Remove:                                                │
│   ✅ Scene::ConfigureFromDefault()                    │
│   ✅ SceneFactory class (if using namespace)           │
│   ✅ ConfigureEntitiesFromDefaultData() methods       │
│   ✅ DataType enum (if unused)                         │
│                                                        │
│ Update:                                                │
│   ✅ Documentation                                     │
│   ✅ Architecture diagrams                             │
│                                                        │
│ Result:                                                │
│   - Clean codebase                                     │
│   - No deprecated code                                 │
│   - All tests passing                                  │
└────────────────────────────────────────────────────────┘
```

### Timeline Chart

```
Week 1          Week 2          Week 3
├───────────────┼───────────────┼───────────────┤

Day 1-2: Phase 1 (IEntityConfigurator)
├─────────┤

Day 3-5: Phase 2 (FlatbuffersConfigurator)
          ├────────────┤

Day 6-8: Phase 3 (Scene Configuration)
                      ├────────────┤

Day 9-11: Phase 4 (SceneFactory)
                                ├────────────┤

Day 12-15: Phase 5 (SceneManager)
                                            ├────────────────┤

Day 16-17: Phase 6 (Cleanup)
                                                            ├─────┤

Total: ~2-3 weeks
```

### Risk Timeline

```
Risk Level by Phase:

Phase 1: ████░░░░░░ (Low)       - Just interface creation
Phase 2: █████░░░░░ (Low)       - Isolated to one class
Phase 3: ███████░░░ (Moderate) - Scene API changes
Phase 4: ███████░░░ (Moderate) - Factory refactor
Phase 5: ████████░░ (Moderate) - Integration changes
Phase 6: ████░░░░░░ (Low)       - Cleanup only

Overall: ██████░░░░ (Moderate Risk, High Value)
```

---

## Dependency Graph

### Current Dependencies (Tight Coupling)

```
                    SceneManager
                         │
                         ▼
                   SceneFactory ─────────┐
                         │               │
                         ▼               │
                       Scene             │
                         │               │
                         ▼               │
                   EntityManager         │
                         │               │
                         ▼               │
              FlatbuffersConfigurator    │
                         │               │
                         ▼               │
              FlatbuffersDataLoader◄─────┘
                         │
                         ▼
                  [FlatBuffers Files]

❌ Problems:
   - SceneFactory depends on Scene internals
   - Scene depends on EntityManager internals  
   - EntityManager depends on FlatbuffersConfigurator
   - FlatbuffersConfigurator depends on FlatbuffersDataLoader
   - Long dependency chain
   - Tight coupling
```

### Proposed Dependencies (Loose Coupling)

```
                    SceneManager
                    ┌───┴───┐
                    │       │
                    ▼       ▼
          ISceneDataProvider  scene_factory
          ISaveDataProvider         │
                    │               │
                    │               ▼
                    │             Scene
                    │               │
                    │               ▼
                    │         EntityManager
                    │               │
                    │               ▼
                    │       IEntityConfigurator
                    │        (Abstract Interface)
                    │               │
                    │       ┌───────┴────────┬─────────┐
                    │       ▼                ▼         ▼
                    │  Flatbuffers    SaveData    Json
                    │  Configurator   Configurator Configurator
                    │       │
                    └───────┤
                            ▼
                  FlatbuffersDataLoader
                            │
                            ▼
                   [FlatBuffers Files]

✅ Solutions:
   - SceneManager controls data flow
   - Scene depends only on IEntityConfigurator (interface)
   - EntityManager depends only on IEntityConfigurator (interface)
   - Configurators isolated
   - Dependency Inversion Principle
   - Easy to add new configurators
```

---

## Summary Comparison

### Before vs After

```
┌─────────────────────────────────────────────────────────────────┐
│                    ARCHITECTURE COMPARISON                      │
├──────────────────────┬──────────────────────────────────────────┤
│     BEFORE (Bad)     │         AFTER (Good)                     │
├──────────────────────┼──────────────────────────────────────────┤
│                      │                                          │
│ SceneFactory:        │ scene_factory:                           │
│  ├─ CreateDefault    │  └─ CreateScene(type, ctx, config, data)│
│  │   Scene()         │                                          │
│  └─ Fetches data     │ SceneManager:                            │
│      internally      │  ├─ Fetches data                         │
│                      │  ├─ Creates configurator                 │
│ Scene:               │  └─ Calls scene_factory                  │
│  └─ ConfigureFrom    │                                          │
│      Default()       │ Scene:                                   │
│                      │  └─ Configure(IEntityConfigurator&,      │
│ EntityManager:       │      const void*)                        │
│  └─ ConfigureEntit   │                                          │
│      iesFromDefault  │ EntityManager:                           │
│      Data(SceneType) │  └─ Configure(IEntityConfigurator&,      │
│                      │      const void*)                        │
│ FlatbuffersConfig:   │                                          │
│  └─ ConfigureEntit   │ IEntityConfigurator:                     │
│      iesFromDefault  │  └─ ConfigureEntities(EMP&, void*) = 0   │
│      Data(EMP&,      │                                          │
│      SceneType)      │ FlatbuffersConfigurator:                 │
│                      │  └─ ConfigureEntities(EMP&, void*)       │
│                      │                                          │
├──────────────────────┼──────────────────────────────────────────┤
│                      │                                          │
│ ❌ "Default" in      │ ✅ Generic method names                 │
│    method names      │                                          │
│                      │                                          │
│ ❌ Data source       │ ✅ Data source decision                 │
│    decision in       │    in SceneManager                       │
│    SceneFactory      │                                          │
│                      │                                          │
│ ❌ FlatBuffers       │ ✅ FlatBuffers isolated                 │
│    types exposed     │    to configurator                       │
│                      │                                          │
│ ❌ No save/load      │ ✅ Save/load enabled                    │
│    support           │                                          │
│                      │                                          │
│ ❌ SceneType passed  │ ✅ Data passed down                     │
│    down stack        │                                          │
│                      │                                          │
│ ❌ Tight coupling    │ ✅ Loose coupling via                   │
│                      │    interfaces                            │
│                      │                                          │
└──────────────────────┴──────────────────────────────────────────┘
```

---

## Related Documentation

- [Full Proposal](SCENE_CONFIGURATION_DECOUPLING.md) - Complete design document
- [Quick Reference](SCENE_CONFIGURATION_DECOUPLING_QUICK_REF.md) - Implementation patterns
- [ISceneDataProvider](../../src/data_providers/ISceneDataProvider.h) - Scene data interface
- [ISaveDataProvider](../../src/configuration/ISaveDataProvider.h) - Save data interface
