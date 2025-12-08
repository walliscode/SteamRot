# Scene Factory Architecture - Visual Diagrams

**Last Updated**: December 8, 2025

---

## Table of Contents

1. [Current Architecture (Before)](#current-architecture-before)
2. [Proposed Architecture (After)](#proposed-architecture-after)
3. [Data Flow Diagrams](#data-flow-diagrams)
4. [Sequence Diagrams](#sequence-diagrams)
5. [Class Relationships](#class-relationships)
6. [Pattern Comparisons](#pattern-comparisons)

---

## Current Architecture (Before)

### Component Diagram - Current State

```
┌────────────────────────────────────────────────────────────────┐
│                         SceneManager                           │
│                                                                │
│  AddSceneFromDefault(SceneType)                                │
│    ↓                                                           │
│    Creates FlatbuffersSceneFactory directly                    │
└────────────────────────────────────────────────────────────────┘
                            ↓
                            ↓ creates with SceneDataFbs*
                            ↓
┌────────────────────────────────────────────────────────────────┐
│                  FlatbuffersSceneFactory                       │
│                                                                │
│  - m_scene_data_fbs: const SceneDataFbs*                       │
│  - m_entity_configurator: unique_ptr<IEntityConfigurator>      │
│                                                                │
│  Constructor:                                                  │
│    FlatbuffersSceneFactory(GameContext, SceneDataFbs*)         │
│    {                                                           │
│      m_entity_configurator =                                   │
│        make_unique<FlatbuffersEntityConfigurator>(             │
│          event_handler,                                        │
│          *m_scene_data_fbs->entity_collection()                │
│        );                                                      │
│    }                                                           │
│                                                                │
│  ConfigureSceneResources(Scene&)                               │
│  ConfigureSceneConfig(Scene&)                                  │
└────────────────────────────────────────────────────────────────┘
                            ↓
                            ↓ uses
                            ↓
┌────────────────────────────────────────────────────────────────┐
│              FlatbuffersEntityConfigurator                     │
│                                                                │
│  - m_entity_collection_data: const EntityCollectionFbs&        │
│                                                                │
│  ConfigureEntityMemoryPool(EntityMemoryPool&)                  │
│  ConfigureFirstLayerComponents(EntityMemoryPool&)              │
│  ConfigureSecondLayerComponents(EntityMemoryPool&)             │
└────────────────────────────────────────────────────────────────┘
```

### Current Data Flow

```
FlatbuffersDataLoader
        ↓
        ↓ ProvideDefaultSceneData(SceneType)
        ↓ returns: const SceneDataFbs*
        ↓
        ↓ passed to constructor
        ↓
FlatbuffersSceneFactory
        ↓
        ↓ extracts entity_collection()
        ↓ passes EntityCollectionFbs& to
        ↓
FlatbuffersEntityConfigurator
        ↓
        ↓ ConfigureEntityMemoryPool()
        ↓
   EntityMemoryPool (Scene)
```

### Problems with Current Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                         PROBLEM 1                               │
│                                                                 │
│  SceneFactory only supports ONE data source:                    │
│    ✗ Hardcoded to SceneDataFbs*                                 │
│    ✗ Cannot support ISaveDataProvider                           │
│    ✗ Cannot support test fixtures                               │
│    ✗ Not extensible                                             │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│                         PROBLEM 2                               │
│                                                                 │
│  To add save support, would need:                               │
│    ✗ Dual constructor with nullable providers                   │
│    ✗ Null-check conditionals everywhere                         │
│    ✗ Complex error handling                                     │
│    ✗ Violates Single Responsibility Principle                   │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│                         PROBLEM 3                               │
│                                                                 │
│  SceneManager knows too much:                                   │
│    ✗ Must know about FlatBuffers directly                       │
│    ✗ Must know how to extract data from providers               │
│    ✗ Tight coupling to implementation details                   │
└─────────────────────────────────────────────────────────────────┘
```

---

## Proposed Architecture (After)

### Component Diagram - With Strategy Pattern

```
┌────────────────────────────────────────────────────────────────────┐
│                         SceneManager                               │
│                                                                    │
│  AddSceneFromDefault(SceneType):                                   │
│    1. Get ISceneDataProvider                                       │
│    2. Create DefaultSceneConfigurator(provider, scene_type)        │
│    3. Create FlatbuffersSceneFactory(context, configurator)        │
│    4. Call factory.CreateScene()                                   │
│                                                                    │
│  AddSceneFromSave(slot_index):                                     │
│    1. Get ISaveDataProvider                                        │
│    2. Create SavedSceneConfigurator(provider, slot_index)          │
│    3. Create FlatbuffersSceneFactory(context, configurator)        │
│    4. Call factory.CreateScene()                                   │
└────────────────────────────────────────────────────────────────────┘
              ↓                                    ↓
              ↓ creates                            ↓ creates
              ↓ DefaultSceneConfigurator          ↓ SavedSceneConfigurator
              ↓                                    ↓
┌─────────────────────────────┐    ┌─────────────────────────────────┐
│  DefaultSceneConfigurator   │    │  SavedSceneConfigurator         │
│  : ISceneConfigurator       │    │  : ISceneConfigurator           │
│                             │    │                                 │
│  - m_scene_data_provider    │    │  - m_save_data_provider         │
│  - m_scene_type             │    │  - m_slot_index                 │
│  - m_cached_scene_data      │    │  - m_cached_scene_data          │
│                             │    │                                 │
│  GetSceneData()             │    │  GetSceneData()                 │
│  CreateEntityConfigurator() │    │  CreateEntityConfigurator()     │
│  GetSceneType()             │    │  GetSceneType()                 │
└─────────────────────────────┘    └─────────────────────────────────┘
              │                                    │
              └────────────────┬───────────────────┘
                               ↓
                        implements interface
                               ↓
                    ┌───────────────────────┐
                    │  ISceneConfigurator   │
                    │     (interface)       │
                    │                       │
                    │  GetSceneData()       │
                    │  CreateEntity-        │
                    │    Configurator()     │
                    │  GetSceneType()       │
                    └───────────────────────┘
                               ↑
                               ↑ passed to
                               ↑
┌────────────────────────────────────────────────────────────────────┐
│                  FlatbuffersSceneFactory                           │
│                     : ISceneFactory                                │
│                                                                    │
│  - m_scene_configurator: unique_ptr<ISceneConfigurator>            │
│  - m_entity_configurator: unique_ptr<IEntityConfigurator>          │
│                                                                    │
│  Constructor:                                                      │
│    FlatbuffersSceneFactory(                                        │
│      GameContext,                                                  │
│      unique_ptr<ISceneConfigurator>)                               │
│    {                                                               │
│      scene_type = m_scene_configurator->GetSceneType();            │
│      m_entity_configurator =                                       │
│        m_scene_configurator->CreateEntityConfigurator();           │
│    }                                                               │
│                                                                    │
│  ConfigureSceneResources(Scene&):                                  │
│    {                                                               │
│      auto data = m_scene_configurator->GetSceneData();             │
│      // Use data...                                                │
│    }                                                               │
│                                                                    │
│  ConfigureSceneConfig(Scene&):                                     │
│    {                                                               │
│      auto data = m_scene_configurator->GetSceneData();             │
│      // Use data...                                                │
│    }                                                               │
└────────────────────────────────────────────────────────────────────┘
                               ↓
                               ↓ uses
                               ↓
┌────────────────────────────────────────────────────────────────────┐
│              FlatbuffersEntityConfigurator                         │
│                   : IEntityConfigurator                            │
│                                                                    │
│  - m_entity_collection_data: const EntityCollectionFbs&            │
│                                                                    │
│  ConfigureEntityMemoryPool(EntityMemoryPool&)                      │
│  ConfigureFirstLayerComponents(EntityMemoryPool&)                  │
│  ConfigureSecondLayerComponents(EntityMemoryPool&)                 │
└────────────────────────────────────────────────────────────────────┘
```

### New Data Flow - Default Scene

```
┌──────────────────────┐
│   SceneManager       │
│                      │
│ AddSceneFromDefault()│
└──────────────────────┘
           ↓
           ↓ 1. Get provider
           ↓
┌──────────────────────┐
│ ISceneDataProvider   │
│  (FlatBuffers impl)  │
└──────────────────────┘
           ↓
           ↓ 2. Create configurator with provider
           ↓
┌──────────────────────────┐
│ DefaultSceneConfigurator │
│                          │
│ Wraps provider,          │
│ caches data              │
└──────────────────────────┘
           ↓
           ↓ 3. Pass to factory
           ↓
┌─────────────────────────────┐
│  FlatbuffersSceneFactory    │
│                             │
│  Calls:                     │
│   - configurator.GetData()  │
│   - configurator.Create-    │
│       EntityConfigurator()  │
└─────────────────────────────┘
           ↓
           ↓ 4. Configure scene
           ↓
     ┌──────────┐
     │  Scene   │
     └──────────┘
```

### New Data Flow - Saved Scene

```
┌──────────────────────┐
│   SceneManager       │
│                      │
│ AddSceneFromSave()   │
└──────────────────────┘
           ↓
           ↓ 1. Get provider
           ↓
┌──────────────────────┐
│  ISaveDataProvider   │
│  (FlatBuffers impl)  │
└──────────────────────┘
           ↓
           ↓ 2. Create configurator with provider
           ↓
┌─────────────────────────┐
│ SavedSceneConfigurator  │
│                         │
│ Wraps provider,         │
│ extracts scene data,    │
│ caches data             │
└─────────────────────────┘
           ↓
           ↓ 3. Pass to factory
           ↓
┌─────────────────────────────┐
│  FlatbuffersSceneFactory    │
│                             │
│  Calls:                     │
│   - configurator.GetData()  │
│   - configurator.Create-    │
│       EntityConfigurator()  │
└─────────────────────────────┘
           ↓
           ↓ 4. Configure scene
           ↓
     ┌──────────┐
     │  Scene   │
     └──────────┘
```

### Benefits of New Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                         BENEFIT 1                               │
│                   Clean Separation of Concerns                  │
│                                                                 │
│  ✓ SceneManager:     Orchestrates (decides which source)        │
│  ✓ Configurator:     Encapsulates data sourcing                 │
│  ✓ Factory:          Configures scene (source-agnostic)         │
│  ✓ Entity Config:    Configures entities (source-agnostic)      │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│                         BENEFIT 2                               │
│                      Zero Conditionals                          │
│                                                                 │
│  ✓ No null-check if statements                                  │
│  ✓ No "if default else if save" logic                           │
│  ✓ Type-safe by design                                          │
│  ✓ Compiler catches mistakes                                    │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│                         BENEFIT 3                               │
│                        Extensibility                            │
│                                                                 │
│  ✓ New source = new configurator                                │
│  ✓ Zero changes to Factory                                      │
│  ✓ Zero changes to Entity Configurator                          │
│  ✓ Easy to add: Network, Procedural, Test sources               │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│                         BENEFIT 4                               │
│                         Testability                             │
│                                                                 │
│  ✓ Mock configurator for Factory tests                          │
│  ✓ Mock provider for Configurator tests                         │
│  ✓ Independent unit tests for each component                    │
│  ✓ Integration tests verify full flow                           │
└─────────────────────────────────────────────────────────────────┘
```

---

## Sequence Diagrams

### Sequence: Loading Default Scene

```
SceneManager  SceneDataProv  DefaultConfig  Factory  EntityConfig  Scene
     │               │              │          │          │         │
     │──Get()───────>│              │          │          │         │
     │<──provider────│              │          │          │         │
     │                              │          │          │         │
     │──New(provider, type)────────>│          │          │         │
     │<──configurator───────────────│          │          │         │
     │                              │          │          │         │
     │──New(context, configurator)──────────>│          │         │
     │                              │          │          │         │
     │                              │<─GetType()│          │         │
     │                              │──type────>│          │         │
     │                              │          │          │         │
     │                              │<─CreateEntityConfig()│         │
     │                              │          │          │         │
     │                              │──LoadData()>         │         │
     │               │<─LoadSceneData(type)────│          │         │
     │               │──SceneDataFbs*──────────>│          │         │
     │                              │<─cache────│          │         │
     │                              │          │          │         │
     │                              │──New(handler, entity_data)────>│
     │                              │<─entity_config───────│         │
     │                              │──entity_config──────>│         │
     │<──factory────────────────────────────────│          │         │
     │                              │          │          │         │
     │──CreateScene()──────────────────────────>│          │         │
     │                              │          │          │         │
     │                              │          │──GetSceneData()───>│
     │                              │<─SceneDataFbs*───────│         │
     │                              │          │          │         │
     │                              │          │──ConfigureResources(scene)──>│
     │                              │          │<─success──│         │
     │                              │          │          │         │
     │                              │          │──ConfigureConfig(scene)────>│
     │                              │          │<─success──│         │
     │<──scene──────────────────────────────────│          │         │
     │                              │          │          │         │
```

### Sequence: Loading Saved Scene

```
SceneManager  SaveDataProv  SavedConfig  Factory  EntityConfig  Scene
     │               │            │         │          │         │
     │──Get()───────>│            │         │          │         │
     │<──provider────│            │         │          │         │
     │                            │         │          │         │
     │──New(provider, slot)──────>│         │          │         │
     │<──configurator─────────────│         │          │         │
     │                            │         │          │         │
     │──New(context, configurator)────────>│          │         │
     │                            │         │          │         │
     │                            │<─GetType()         │         │
     │                            │         │          │         │
     │                            │──LoadSave(slot)───>│         │
     │               │<─LoadSave(slot)─────│          │         │
     │               │──SaveData───────────>│          │         │
     │                            │<─cache──│          │         │
     │                            │         │          │         │
     │                            │──ExtractSceneData()>         │
     │                            │<─SceneDataFbs*─────│         │
     │                            │──type──>│          │         │
     │                            │         │          │         │
     │                            │<─CreateEntityConfig()        │
     │                            │──New(handler, entity_data)──>│
     │                            │<─entity_config─────│         │
     │                            │──entity_config────>│         │
     │<──factory──────────────────────────────│         │         │
     │                            │         │          │         │
     │──CreateScene()────────────────────────>│         │         │
     │                            │         │          │         │
     │                            │         │──GetSceneData()───>│
     │                            │<─SceneDataFbs*─────│         │
     │                            │         │          │         │
     │                            │         │──ConfigureResources(scene)──>│
     │                            │         │<─success──│         │
     │<──scene────────────────────────────────│         │         │
     │                            │         │          │         │
```

---

## Class Relationships

### Inheritance Hierarchy

```
                    ISceneConfigurator
                    (abstract interface)
                            │
                ┌───────────┴───────────┐
                │                       │
    DefaultSceneConfigurator   SavedSceneConfigurator
         (concrete)                 (concrete)
                │                       │
                └───────────┬───────────┘
                            │
                    used by (composition)
                            │
                            ↓
                  FlatbuffersSceneFactory
                    : ISceneFactory
                    (concrete)
                            │
                     has-a (composition)
                            │
                            ↓
                   IEntityConfigurator
                    (abstract interface)
                            │
                            │
              FlatbuffersEntityConfigurator
                      (concrete)
```

### Dependency Graph

```
┌─────────────────┐
│  SceneManager   │
└────────┬────────┘
         │ creates
         ├──────────────────────┬─────────────────────────┐
         ↓                      ↓                         ↓
┌──────────────────┐  ┌──────────────────┐    ┌──────────────────┐
│ISceneDataProvider│  │ISaveDataProvider │    │ GameContext      │
└────────┬─────────┘  └────────┬─────────┘    └────────┬─────────┘
         │                     │                        │
         │ passed to           │ passed to              │ passed to
         │                     │                        │
         ↓                     ↓                        ↓
┌────────────────────┐  ┌───────────────────┐   ┌──────────────────┐
│DefaultSceneConfig  │  │SavedSceneConfig   │   │                  │
└────────┬───────────┘  └────────┬──────────┘   │                  │
         │                       │               │                  │
         │ passed to             │ passed to     │                  │
         └───────────┬───────────┘               │                  │
                     ↓                           ↓                  │
            ┌───────────────────────────────────────────────┐      │
            │      FlatbuffersSceneFactory                  │      │
            └──────────────────┬────────────────────────────┘      │
                               │ creates                            │
                               ↓                                    │
            ┌────────────────────────────────────┐                 │
            │  FlatbuffersEntityConfigurator     │<────────────────┘
            └────────────────────────────────────┘   receives
                               │                      EventHandler
                               │ configures
                               ↓
                      ┌─────────────────┐
                      │ EntityMemoryPool│
                      └─────────────────┘
```

---

## Pattern Comparisons

### Comparison: Null-Check vs Strategy Pattern

#### Option A: Null-Check Pattern (NOT RECOMMENDED)

```
┌────────────────────────────────────────────────────────────┐
│              FlatbuffersSceneFactory                       │
│                                                            │
│  Constructor(context, default_prov*, save_prov*):          │
│    ┌──────────────────────────────────────────────┐       │
│    │ if (default_prov && !save_prov)              │       │
│    │   // Use default                             │       │
│    │ else if (save_prov && !default_prov)         │       │
│    │   // Use save                                │       │
│    │ else if (both)                               │       │
│    │   // ERROR: Invalid state                    │       │
│    │ else                                         │       │
│    │   // ERROR: Both null                        │       │
│    └──────────────────────────────────────────────┘       │
│                                                            │
│  ConfigureSceneResources(scene):                           │
│    ┌──────────────────────────────────────────────┐       │
│    │ if (m_default_provider)                      │       │
│    │   data = m_default_provider->Load()          │       │
│    │ else if (m_save_provider)                    │       │
│    │   data = m_save_provider->Load()             │       │
│    └──────────────────────────────────────────────┘       │
│                                                            │
│  ConfigureSceneConfig(scene):                              │
│    ┌──────────────────────────────────────────────┐       │
│    │ if (m_default_provider)                      │       │
│    │   data = m_default_provider->Load()          │       │
│    │ else if (m_save_provider)                    │       │
│    │   data = m_save_provider->Load()             │       │
│    └──────────────────────────────────────────────┘       │
└────────────────────────────────────────────────────────────┘

PROBLEMS:
  ✗ Conditionals in multiple places
  ✗ Easy to make mistakes
  ✗ Complex error cases
  ✗ Not extensible
  ✗ Hard to test (many branches)
```

#### Option B: Strategy Pattern (RECOMMENDED)

```
┌────────────────────────────────────────────────────────────┐
│              FlatbuffersSceneFactory                       │
│                                                            │
│  Constructor(context, unique_ptr<ISceneConfigurator>):     │
│    ┌──────────────────────────────────────────────┐       │
│    │ // No conditionals!                          │       │
│    │ m_configurator = move(configurator)          │       │
│    │ scene_type = m_configurator->GetSceneType()  │       │
│    │ m_entity_config = m_configurator->Create...()│       │
│    └──────────────────────────────────────────────┘       │
│                                                            │
│  ConfigureSceneResources(scene):                           │
│    ┌──────────────────────────────────────────────┐       │
│    │ // No conditionals!                          │       │
│    │ data = m_configurator->GetSceneData()        │       │
│    │ // Use data...                               │       │
│    └──────────────────────────────────────────────┘       │
│                                                            │
│  ConfigureSceneConfig(scene):                              │
│    ┌──────────────────────────────────────────────┐       │
│    │ // No conditionals!                          │       │
│    │ data = m_configurator->GetSceneData()        │       │
│    │ // Use data...                               │       │
│    └──────────────────────────────────────────────┘       │
└────────────────────────────────────────────────────────────┘
         ↑
         │ Configurator handles all source logic
         │
┌────────┴──────────────────────────────────────────────────┐
│  ISceneConfigurator (interface)                           │
│    - GetSceneData()                                       │
│    - CreateEntityConfigurator()                           │
│    - GetSceneType()                                       │
└───────────────────────────────────────────────────────────┘
         ↑                               ↑
         │                               │
┌────────┴────────────┐     ┌────────────┴───────────┐
│DefaultSceneConfig   │     │SavedSceneConfig        │
│                     │     │                        │
│Wraps ISceneData-    │     │Wraps ISaveData-        │
│Provider             │     │Provider                │
└─────────────────────┘     └────────────────────────┘

BENEFITS:
  ✓ Zero conditionals in Factory
  ✓ Type-safe by design
  ✓ Easy to extend (new configurator = new source)
  ✓ Easy to test (mock configurator)
  ✓ Clean separation of concerns
```

### Comparison: Overloaded vs Virtual Methods

#### Overloaded Methods (DON'T USE)

```
class IEntityConfigurator {
public:
    // These are OVERLOADS, not overrides
    // Resolved at COMPILE-TIME
    virtual void Configure(const EntityCollectionFbs& default_data) = 0;
    virtual void Configure(const SavedEntityData& save_data) = 0;
};

class FlatbuffersEntityConfigurator : public IEntityConfigurator {
    void Configure(const EntityCollectionFbs& data) override {
        // Implementation
    }
    void Configure(const SavedEntityData& data) override {
        // Implementation
    }
};

// PROBLEM: Polymorphism doesn't work
IEntityConfigurator* config = new FlatbuffersEntityConfigurator();
EntityCollectionFbs data;
config->Configure(data);  // Which one is called?
                           // Depends on pointer type, not object type!
                           // This defeats polymorphism!
```

#### Virtual Methods (RECOMMENDED)

```
class IEntityConfigurator {
public:
    // Named virtual methods
    // True polymorphism at RUNTIME
    virtual void ConfigureFromDefault(const EntityCollectionFbs& data) = 0;
    virtual void ConfigureFromSave(const EntityCollectionFbs& data) = 0;
};

class FlatbuffersEntityConfigurator : public IEntityConfigurator {
    void ConfigureFromDefault(const EntityCollectionFbs& data) override {
        // Implementation for default data
    }
    void ConfigureFromSave(const EntityCollectionFbs& data) override {
        // Implementation for save data
        // (might adapt SavedEntityData -> EntityCollectionFbs first)
    }
};

// CORRECT: Polymorphism works
IEntityConfigurator* config = new FlatbuffersEntityConfigurator();
config->ConfigureFromDefault(data);  // Calls derived class method
config->ConfigureFromSave(data);     // Calls derived class method
```

---

## Implementation Phases Diagram

```
Phase 1: Create Interfaces
┌──────────────────────────┐
│ Create ISceneConfigurator│
│ Create DefaultSceneConfig│
│ Create SavedSceneConfig  │
│ Write unit tests         │
└──────────────────────────┘
            ↓
            
Phase 2: Update Factory
┌──────────────────────────┐
│ Modify Factory header    │
│ Modify Factory impl      │
│ Update Factory tests     │
└──────────────────────────┘
            ↓
            
Phase 3: Update SceneManager
┌──────────────────────────┐
│ Update AddSceneFrom...() │
│ Update SceneManager tests│
└──────────────────────────┘
            ↓
            
Phase 4: Integration
┌──────────────────────────┐
│ Write integration tests  │
│ Test default flow        │
│ Test save flow           │
│ Verify no regressions    │
└──────────────────────────┘
```

---

## Summary Diagram

```
╔═══════════════════════════════════════════════════════════════╗
║            SCENE FACTORY DATA SOURCING ARCHITECTURE           ║
╠═══════════════════════════════════════════════════════════════╣
║                                                               ║
║  PATTERN:  Strategy Pattern + Abstract Factory Pattern       ║
║                                                               ║
║  KEY IDEA: Configurator encapsulates data source logic       ║
║                                                               ║
║  ┌────────────┐                                              ║
║  │SceneManager│──decides source──>┌────────────────┐         ║
║  └────────────┘                   │ ISceneConfig   │         ║
║                                   │  (strategy)    │         ║
║                                   └────────────────┘         ║
║                                          │                   ║
║                         ┌────────────────┴────────────────┐  ║
║                         │                                 │  ║
║                ┌────────▼──────┐              ┌──────────▼┐  ║
║                │ DefaultScene  │              │SavedScene │  ║
║                │ Configurator  │              │Config     │  ║
║                └───────────────┘              └───────────┘  ║
║                         │                                 │  ║
║                         └────────────────┬────────────────┘  ║
║                                          │                   ║
║                                  passed to                   ║
║                                          │                   ║
║                            ┌─────────────▼────────────┐      ║
║                            │FlatbuffersSceneFactory   │      ║
║                            │  (source-agnostic!)      │      ║
║                            └──────────────────────────┘      ║
║                                                               ║
║  BENEFITS:                                                    ║
║    ✓ Zero conditionals in Factory                            ║
║    ✓ Clean separation of concerns                            ║
║    ✓ Easy to test                                            ║
║    ✓ Extensible (new source = new configurator)              ║
║    ✓ Type-safe                                               ║
║    ✓ SOLID principles                                        ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝
```

---

**End of Visual Diagrams**
