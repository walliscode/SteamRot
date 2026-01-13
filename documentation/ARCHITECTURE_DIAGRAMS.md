# Architecture Diagrams

This document provides visual representations of the proposed native object creation architecture.

## High-Level Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                         User Code                            │
│  (Creates factory, requests configurators, configures objects)│
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│         ConfiguratorFactory<DataSourceType>                  │
│                                                              │
│  - Caches configurator instances                            │
│  - Manages dependencies                                      │
│  - GetConfigurator<DataType, ObjectType>()                  │
│  - GetDataProvider<DataType>()                              │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│    ConfiguratorRegistry<Source, Data, Object>                │
│                                                              │
│  *** ONLY COUPLING POINT ***                                │
│                                                              │
│  Template specializations map:                              │
│  (DataSourceType + DataType + ObjectType) → Implementation  │
│                                                              │
│  Example:                                                    │
│  (Flatbuffers, SceneDataFbs, Scene)                         │
│    → FlatbuffersSceneConfigurator                           │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│       Concrete Configurator Implementation                   │
│                                                              │
│  IConfigurator<DataType, ObjectType>                        │
│    - Configure(Object&, const Data&)                        │
│    - Knows specific data format                             │
│    - No type erasure                                         │
└─────────────────────────────────────────────────────────────┘
```

## Data Flow

```
External Data File
    (scene.fbs.bin)
         │
         ▼
┌─────────────────────┐
│   DataProvider      │
│  LoadData()         │ ──────────┐
└─────────────────────┘           │
                                  │
                                  ▼
                           ┌──────────────┐
                           │  SceneDataFbs│
                           │  (FlatBuffers)│
                           └──────┬───────┘
                                  │
                                  ▼
┌─────────────────────┐    ┌─────────────────────┐
│   Scene             │◄───│  Configurator       │
│   (Native Object)   │    │  Configure()        │
└─────────────────────┘    └─────────────────────┘
```

## Composition Pattern (A contains B)

```
Scene Configuration
└─ ConfigureSceneInfo() ──────► Free Function
└─ ConfigureEntities() ────────► EntityConfigurator (via factory)
   └─ ConfigureEntity() ───────► Free Functions / Component Configs
└─ ConfigureUIStyles() ────────► UIStyleConfigurator (via factory)
   └─ ConfigureStyle() ────────► Free Functions
```

### Detailed Flow

```
┌──────────────────────────────────────────────────────────────┐
│           FlatbuffersSceneConfigurator                        │
│                                                               │
│  Configure(Scene& scene, const SceneDataFbs& data) {         │
│                                                               │
│    1. Configure scene info (free function)                   │
│       ConfigureSceneInfo(scene.GetInfo(), data.info());      │
│                                                               │
│    2. Get entity configurator from factory                   │
│       auto entity_config =                                   │
│         factory.GetConfigurator<EntityData, EntityPool>();   │
│                                                               │
│    3. Use entity configurator                                │
│       entity_config->Configure(                              │
│           scene.GetEntities(), data.entities());             │
│       ┌───────────────────────────────────────────┐         │
│       │  FlatbuffersEntityConfigurator            │         │
│       │  - Configures entities                    │         │
│       │  - Reusable by Scene, SaveFile, Level    │         │
│       └───────────────────────────────────────────┘         │
│                                                               │
│    4. Configure UI styles                                    │
│       auto ui_config =                                       │
│         factory.GetConfigurator<UIData, UIStyles>();         │
│       ui_config->Configure(scene.GetUI(), data.ui());        │
│  }                                                            │
└──────────────────────────────────────────────────────────────┘
```

## Current vs Proposed Architecture

### Current Architecture (Switch-Based)

```
┌─────────────────┐
│  DataType Enum  │
│  (Flatbuffers)  │
└────────┬────────┘
         │
         ▼
┌────────────────────────────────────────┐
│     DataAccessFactory                  │
│                                        │
│  switch (m_data_type) {                │
│    case Flatbuffers:                   │
│      provider = new Flatbuffers...();  │
│      break;                            │
│    case JSON:  ◄─── Must add case     │
│      provider = new JSON...();         │
│      break;                            │
│  }                                     │
└────────────────────────────────────────┘
         │
         ▼
┌────────────────────────┐
│  Concrete Implementation│
└────────────────────────┘

Problems:
❌ Must modify switch for new types
❌ Coupling in factory code
❌ Runtime dispatch overhead
```

### Proposed Architecture (Template-Based)

```
┌────────────────────────┐
│  DataSourceType Enum   │
│  (Flatbuffers)         │
└───────────┬────────────┘
            │
            ▼ (template parameter)
┌─────────────────────────────────────────┐
│  ConfiguratorFactory<Flatbuffers>       │
│                                         │
│  GetConfigurator<DataFbs, Scene>() {   │
│    return Registry<Flatbuffers,        │
│                    DataFbs,            │
│                    Scene>::Create();   │
│  }                                      │
│  ▲                                      │
│  └─ No switch, uses templates          │
└─────────────────────────────────────────┘
            │
            ▼
┌─────────────────────────────────────────┐
│  ConfiguratorRegistry Specialization    │
│  *** ONLY COUPLING POINT ***           │
│                                         │
│  template<>                             │
│  struct Registry<Flatbuffers,          │
│                  DataFbs,              │
│                  Scene> {              │
│    static auto Create() {              │
│      return make_unique<               │
│        FlatbuffersSceneConfig>();      │
│    }                                    │
│  };                                     │
│                                         │
│  To add JSON: ◄─── Just add            │
│  template<>        specialization      │
│  struct Registry<JSON, JSONData, Scene>│
│    { /* ... */ };                      │
└─────────────────────────────────────────┘
            │
            ▼
┌────────────────────────┐
│  Concrete Implementation│
└────────────────────────┘

Benefits:
✅ Add type via specialization
✅ Coupling explicit (registry)
✅ Compile-time dispatch
```

## Adding New Data Type (Visual Workflow)

```
Step 1: Define Data Structure
┌────────────────────────┐
│  struct JSONSceneData  │
│  {                     │
│    string scene_name;  │
│    ...                 │
│  };                    │
└────────────────────────┘

Step 2: Create Configurator
┌──────────────────────────────────────────────┐
│  class JSONSceneConfigurator                 │
│    : public IConfigurator<JSONData, Scene>   │
│  {                                           │
│    Configure(Scene& s, const JSONData& d) {  │
│      // Convert JSON to Scene               │
│    }                                         │
│  };                                          │
└──────────────────────────────────────────────┘

Step 3: Register (ONLY place knowing about all 3)
┌──────────────────────────────────────────────┐
│  template<>                                  │
│  struct ConfiguratorRegistry<               │
│      DataSourceType::JSON,  ◄── Source type │
│      JSONSceneData,         ◄── Data type   │
│      Scene>                 ◄── Object type │
│  {                                           │
│    static auto Create(...) {                │
│      return make_unique<                    │
│        JSONSceneConfigurator>(...);         │
│    }                                         │
│  };                                          │
└──────────────────────────────────────────────┘

Step 4: Use It
┌────────────────────────────────────┐
│  ConfiguratorFactory<JSON> factory;│
│                                    │
│  auto config = factory             │
│    .GetConfigurator<               │
│        JSONSceneData, Scene>();    │
│                                    │
│  config->Configure(scene, data);   │
└────────────────────────────────────┘

✅ No changes to existing code!
```

## Registry as Central Hub

```
                    ConfiguratorRegistry
                            │
                            │
         ┌──────────────────┼──────────────────┐
         │                  │                  │
         ▼                  ▼                  ▼
   Specialization     Specialization     Specialization
   <Flatbuffers,     <Flatbuffers,      <JSON,
    SceneDataFbs,     EntityDataFbs,     JSONSceneData,
    Scene>            EntityPool>        Scene>
         │                  │                  │
         ▼                  ▼                  ▼
   FlatbuffersScene   FlatbuffersEntity  JSONScene
   Configurator       Configurator       Configurator

All coupling confined to registry specializations
Everything else uses IConfigurator<Data, Object> interface
```

## Type Safety Flow

```
User Code (compile time)
    │
    │ factory.GetConfigurator<SceneDataFbs, Scene>()
    │                          ^^^^^^^^^^^  ^^^^^
    │                          Data type    Object type
    ▼
Template instantiation (compile time)
    │
    │ IConfigurator<SceneDataFbs, Scene>*
    │                ^^^^^^^^^^^  ^^^^^
    │                Known types - no erasure
    ▼
Registry lookup (compile time)
    │
    │ ConfiguratorRegistry<Flatbuffers, SceneDataFbs, Scene>
    │                                   ^^^^^^^^^^^  ^^^^^
    │                                   Exact types maintained
    ▼
Concrete implementation (runtime)
    │
    │ FlatbuffersSceneConfigurator::Configure(Scene&, SceneDataFbs&)
    │                                         ^^^^^  ^^^^^^^^^^^^
    │                                         Exact types - no casting
    ▼
Type-safe access to data
    │
    │ data.scene_info()->type()
    │      ^^^^^^^^^^^ Known to be SceneInfoFbs at compile time
    ▼
Result

✅ No type erasure at any point
✅ Full compile-time type checking
✅ No dynamic_cast or type checking needed
```

## Object Relationship Diagram

```
┌─────────────────────────────────────────────────────────┐
│                        Scene                             │
│  - SceneInfo (simple nested, free function)             │
│  - SceneState (simple nested, free function)            │
│  - EntityMemoryPool (complex, has interface) ─────┐     │
│  - UIStyleCollection (complex, has interface) ────┐│    │
└────────────────────────────────────────────────────┼┼────┘
                                                     ││
          ┌──────────────────────────────────────────┘│
          │                                            │
          ▼                                            ▼
┌──────────────────────┐                    ┌──────────────────────┐
│  EntityMemoryPool    │                    │  UIStyleCollection   │
│                      │                    │                      │
│  Has interface:      │                    │  Has interface:      │
│  IConfigurator<      │                    │  IConfigurator<      │
│    EntityDataFbs,    │                    │    UIStyleDataFbs,   │
│    EntityMemoryPool> │                    │    UIStyleCollection>│
│                      │                    │                      │
│  Reusable by:        │                    │  Reusable by:        │
│  - Scene             │                    │  - Scene             │
│  - SaveFile          │                    │  - UI Manager        │
│  - Level             │                    │  - Theme Manager     │
└──────────────────────┘                    └──────────────────────┘

Legend:
  Simple nested    → Use free functions
  Complex shared   → Use configurator interface
```

## Decision Tree: Interface vs Free Function

```
                Is object standalone?
                   (Has own file?)
                        │
            ┌───────────┴───────────┐
            │                       │
           Yes                     No
            │                       │
            ▼                       ▼
    Is object reused?        Is config complex?
    (Multiple parents)              │
            │              ┌─────────┴─────────┐
    ┌───────┴───────┐     │                   │
    │               │    Yes                  No
   Yes             No     │                   │
    │               │     ▼                   ▼
    ▼               ▼     Use Interface    Free Function
Use Interface    Is config
                 complex?
                     │
            ┌────────┴────────┐
            │                 │
           Yes               No
            │                 │
            ▼                 ▼
      Use Interface    Free Function
                      (can promote later)

Examples:
- Scene: Standalone + Complex → Interface
- EntityMemoryPool: Reused + Complex → Interface
- SceneInfo: Nested + Simple → Free Function
- SceneState: Nested + Simple → Free Function
- UIStyleCollection: Reused + Complex → Interface
```

## Complete System Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                         Application                              │
└────────────────────────────┬────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│                  ConfiguratorFactory<Flatbuffers>                │
│  ┌────────────────────────────────────────────────────────────┐ │
│  │ GetConfigurator<DataType, ObjectType>()                    │ │
│  │   ↓                                                         │ │
│  │ ConfiguratorRegistry<Flatbuffers, DataType, ObjectType>   │ │
│  │   ::CreateConfigurator(eh, factory)                        │ │
│  └────────────────────────────────────────────────────────────┘ │
└─────────────────────────────┬───────────────────────────────────┘
                              │
              ┌───────────────┼───────────────┐
              │               │               │
              ▼               ▼               ▼
    ┌─────────────┐  ┌─────────────┐  ┌─────────────┐
    │   Scene      │  │   Entity    │  │  UIStyle    │
    │ Configurator │  │Configurator │  │Configurator │
    └──────┬───────┘  └──────┬──────┘  └──────┬──────┘
           │                 │                 │
           │                 │                 │
    Calls  │          Calls  │          Calls  │
    free   │          other  │          free   │
    funcs  │          configs│          funcs  │
           │                 │                 │
           ▼                 ▼                 ▼
    ┌─────────────┐  ┌─────────────┐  ┌─────────────┐
    │   Scene     │  │  Entities   │  │  UIStyles   │
    │  (Native    │  │  (Native    │  │  (Native    │
    │   Object)   │  │   Object)   │  │   Object)   │
    └─────────────┘  └─────────────┘  └─────────────┘
```

## Summary

The architecture diagrams show:

1. **Clear separation** between user code, factory, registry, and implementations
2. **Single coupling point** in registry specializations
3. **Composition pattern** for nested objects
4. **Type safety** maintained throughout the flow
5. **Extensibility** via template specialization
6. **Decision tree** for choosing between interfaces and free functions

All diagrams are conceptual representations of the proposed architecture detailed in the documentation files.
