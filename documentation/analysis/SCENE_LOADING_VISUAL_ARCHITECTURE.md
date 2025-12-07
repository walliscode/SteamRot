# Scene Data Loading - Visual Architecture

**Date**: December 7, 2025  
**Context**: Visual diagrams for scene data loading architecture  
**Related**: SCENE_DATA_PROVIDER_ARCHITECTURE_ANALYSIS.md

---

## Table of Contents

1. [System Overview](#system-overview)
2. [The Two Access Patterns](#the-two-access-patterns)
3. [Data Flow Diagrams](#data-flow-diagrams)
4. [Provider Architecture](#provider-architecture)
5. [Complete End-to-End Flow](#complete-end-to-end-flow)

---

## System Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                         SteamRot Engine                         │
│                                                                  │
│  ┌────────────────────────────────────────────────────────┐   │
│  │                    Data Layer                          │   │
│  │                                                         │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌──────────────┐  │   │
│  │  │ data/scenes/│  │ saves/      │  │ data/engine/ │  │   │
│  │  │ *.bin       │  │ slot_*.bin  │  │ *.bin        │  │   │
│  │  └─────────────┘  └─────────────┘  └──────────────┘  │   │
│  └────────────────────────────────────────────────────────┘   │
│           ↓                 ↓                ↓                  │
│  ┌────────────────────────────────────────────────────────┐   │
│  │              Provider Interface Layer                  │   │
│  │                                                         │   │
│  │  ┌──────────────┐  ┌──────────────┐  ┌─────────────┐ │   │
│  │  │ISceneData    │  │ISaveData     │  │IEngineData  │ │   │
│  │  │Provider      │  │Provider      │  │Provider     │ │   │
│  │  └──────────────┘  └──────────────┘  └─────────────┘ │   │
│  └────────────────────────────────────────────────────────┘   │
│           ↓                 ↓                ↓                  │
│  ┌────────────────────────────────────────────────────────┐   │
│  │           Native Struct Layer                          │   │
│  │                                                         │   │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐            │   │
│  │  │SceneData │  │SaveData  │  │EngineData│            │   │
│  │  └──────────┘  └──────────┘  └──────────┘            │   │
│  └────────────────────────────────────────────────────────┘   │
│           ↓                 ↓                ↓                  │
│  ┌────────────────────────────────────────────────────────┐   │
│  │              Factory/Configurator Layer                │   │
│  │                                                         │   │
│  │  ┌──────────────┐  ┌──────────────┐                   │   │
│  │  │SceneFactory  │  │Entity        │                   │   │
│  │  │              │  │Configurator  │                   │   │
│  │  └──────────────┘  └──────────────┘                   │   │
│  └────────────────────────────────────────────────────────┘   │
│           ↓                                                     │
│  ┌────────────────────────────────────────────────────────┐   │
│  │               Game Objects Layer                       │   │
│  │                                                         │   │
│  │  ┌─────────┐  ┌───────────────┐  ┌──────────────┐    │   │
│  │  │Scene    │  │EntityMemory   │  │Logic         │    │   │
│  │  │         │  │Pool           │  │Collection    │    │   │
│  │  └─────────┘  └───────────────┘  └──────────────┘    │   │
│  └────────────────────────────────────────────────────────┘   │
│           ↓                                                     │
│  ┌────────────────────────────────────────────────────────┐   │
│  │                    Game Loop                           │   │
│  └────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
```

---

## The Two Access Patterns

### Pattern 1: Default Scene (New Game)

```
┌──────────────────────────────────────────────────────────────┐
│                     New Game Flow                            │
└──────────────────────────────────────────────────────────────┘

User Action: "Start New Game"
        │
        ↓
┌───────────────────────┐
│ ISceneDataProvider    │
│ GetSceneDataProvider()│
└───────────────────────┘
        │
        │ LoadSceneData(SceneType::TITLE)
        ↓
┌───────────────────────────────────┐
│ FlatbuffersSceneDataProvider      │
│                                   │
│ 1. Load: title.scene_data.bin    │
│ 2. Parse FlatBuffers              │
│ 3. Convert to native struct       │
└───────────────────────────────────┘
        │
        │ Returns SceneData
        ↓
┌───────────────────────┐
│ SceneData             │
│ ├─ scene_type         │
│ ├─ scene_id           │
│ ├─ texture_width      │
│ └─ texture_height     │
└───────────────────────┘
        │
        │ Pass to factory
        ↓
┌───────────────────────┐
│ SceneFactory          │
│ CreateDefaultScene()  │
└───────────────────────┘
        │
        │ Create Scene
        ↓
┌───────────────────────┐
│ Scene                 │
│ (created, empty)      │
└───────────────────────┘
        │
        │ ConfigureFromDefault()
        ↓
┌───────────────────────┐
│ IEntityDataProvider   │
│ LoadDefaultEntities() │
└───────────────────────┘
        │
        │ EntityCollectionData
        ↓
┌───────────────────────┐
│ EntityConfigurator    │
│ ConfigureEntities()   │
└───────────────────────┘
        │
        │ Populate
        ↓
┌───────────────────────┐
│ EntityMemoryPool      │
│ (fully configured)    │
└───────────────────────┘
        │
        │ Generate
        ↓
┌───────────────────────┐
│ Archetypes            │
└───────────────────────┘
        │
        │ Create
        ↓
┌───────────────────────┐
│ LogicCollection       │
└───────────────────────┘
        │
        ↓
Scene Ready for Game Loop
```

### Pattern 2: Saved Scene (Load Game)

```
┌──────────────────────────────────────────────────────────────┐
│                    Load Game Flow                            │
└──────────────────────────────────────────────────────────────┘

User Action: "Load Game (Slot 1)"
        │
        ↓
┌───────────────────────┐
│ ISaveDataProvider     │
│ GetSaveDataProvider() │
└───────────────────────┘
        │
        │ LoadSave(1)
        ↓
┌───────────────────────────────────┐
│ FlatbuffersSaveDataProvider       │
│                                   │
│ 1. Load: slot_1.save.bin         │
│ 2. Parse FlatBuffers              │
│ 3. Convert to native struct       │
└───────────────────────────────────┘
        │
        │ Returns SaveData
        ↓
┌───────────────────────┐
│ SaveData              │
│ ├─ metadata           │
│ ├─ current_scene_type │
│ └─ scene_states[]     │
│    ├─ SceneStateData  │
│    │  ├─ scene_type   │
│    │  ├─ scene_id     │
│    │  ├─ dimensions   │
│    │  └─ entity_data  │
│    └─ ...             │
└───────────────────────┘
        │
        │ Extract scene data
        ↓
┌───────────────────────┐
│ SceneDataExtractor    │
│ ExtractSceneMetadata()│
└───────────────────────┘
        │
        │ Returns flat SceneData
        ↓
┌───────────────────────┐
│ SceneData             │
│ (metadata only)       │
└───────────────────────┘
        │
        │ Extract scene state
        ↓
┌───────────────────────┐
│ SceneDataExtractor    │
│ ExtractSceneState(0)  │
└───────────────────────┘
        │
        │ Returns SceneStateData
        ↓
┌───────────────────────┐
│ SceneStateData        │
│ ├─ scene metadata     │
│ └─ entity_data        │
└───────────────────────┘
        │
        │ Pass both to factory
        ↓
┌───────────────────────┐
│ SceneFactory          │
│ CreateSceneFromSave() │
└───────────────────────┘
        │
        │ Create Scene
        ↓
┌───────────────────────┐
│ Scene                 │
│ (created, empty)      │
└───────────────────────┘
        │
        │ ConfigureFromSave(scene_state)
        ↓
┌───────────────────────┐
│ EntityConfigurator    │
│ ConfigureEntities()   │
│ (from saved data)     │
└───────────────────────┘
        │
        │ Populate with saved state
        ↓
┌───────────────────────┐
│ EntityMemoryPool      │
│ (restored from save)  │
└───────────────────────┘
        │
        │ Generate
        ↓
┌───────────────────────┐
│ Archetypes            │
└───────────────────────┘
        │
        │ Create
        ↓
┌───────────────────────┐
│ LogicCollection       │
└───────────────────────┘
        │
        ↓
Scene Ready (Restored State)
```

---

## Data Flow Diagrams

### Default Scene Data Flow

```
┌──────────────────────────────────────────────────────────────┐
│                  FlatBuffers File                            │
│  data/scenes/title.scene_data.bin                           │
│                                                              │
│  ┌────────────────────────────────────────────────┐         │
│  │ SceneDataFbs {                                 │         │
│  │   scene_type: SceneType_TITLE,                │         │
│  │   scene_id: "title-001",                      │         │
│  │   render_texture_width: 1920,                 │         │
│  │   render_texture_height: 1080,                │         │
│  │   entity_collection: EntityCollection { ... } │         │
│  │ }                                              │         │
│  └────────────────────────────────────────────────┘         │
└──────────────────────────────────────────────────────────────┘
                        │
                        │ FlatbuffersSceneDataProvider
                        │ reads and converts
                        ↓
┌──────────────────────────────────────────────────────────────┐
│                  Native C++ Struct                           │
│  SceneData                                                   │
│                                                              │
│  ┌────────────────────────────────────────────────┐         │
│  │ SceneData {                                    │         │
│  │   scene_type = SceneType::SceneType_TITLE,    │         │
│  │   scene_id = "title-001",                     │         │
│  │   render_texture_width = 1920,                │         │
│  │   render_texture_height = 1080                │         │
│  │ }                                              │         │
│  └────────────────────────────────────────────────┘         │
└──────────────────────────────────────────────────────────────┘
                        │
                        │ Used by SceneFactory
                        │ and Scene
                        ↓
┌──────────────────────────────────────────────────────────────┐
│                  Configured Scene                            │
│  Scene object with render texture, entities, logic          │
└──────────────────────────────────────────────────────────────┘
```

### Save File Data Flow

```
┌──────────────────────────────────────────────────────────────┐
│                  Save File (Nested)                          │
│  saves/slot_1.save.bin                                      │
│                                                              │
│  ┌────────────────────────────────────────────────┐         │
│  │ SaveDataFbs {                                  │         │
│  │   metadata: { ... },                           │         │
│  │   current_scene_type: SceneType_CRAFTING,     │         │
│  │   scene_states: [                              │         │
│  │     SceneStateFbs {                            │         │
│  │       scene_type: SceneType_CRAFTING,         │         │
│  │       scene_id: "crafting-001",               │         │
│  │       render_texture_width: 1920,             │         │
│  │       render_texture_height: 1080,            │         │
│  │       entity_collection: EntityCollection {   │         │
│  │         entities: [ ... ]                     │         │
│  │       }                                        │         │
│  │     }                                          │         │
│  │   ]                                            │         │
│  │ }                                              │         │
│  └────────────────────────────────────────────────┘         │
└──────────────────────────────────────────────────────────────┘
                        │
                        │ FlatbuffersSaveDataProvider
                        │ reads and converts
                        ↓
┌──────────────────────────────────────────────────────────────┐
│             Native C++ Struct (Nested)                       │
│  SaveData                                                    │
│                                                              │
│  ┌────────────────────────────────────────────────┐         │
│  │ SaveData {                                     │         │
│  │   metadata = { ... },                          │         │
│  │   current_scene_type = SceneType_CRAFTING,    │         │
│  │   scene_states = [                             │         │
│  │     SceneStateData {                           │         │
│  │       scene_type = SceneType_CRAFTING,        │         │
│  │       scene_id = "crafting-001",              │         │
│  │       render_texture_width = 1920,            │         │
│  │       render_texture_height = 1080,           │         │
│  │       entity_data = EntityCollectionData {    │         │
│  │         entities = [ ... ]                    │         │
│  │       }                                        │         │
│  │     }                                          │         │
│  │   ]                                            │         │
│  │ }                                              │         │
│  └────────────────────────────────────────────────┘         │
└──────────────────────────────────────────────────────────────┘
                        │
                        │ SceneDataExtractor
                        │ extracts specific data
                        ↓
┌──────────────────────────────────────────────────────────────┐
│          Native C++ Struct (Flat - Metadata)                 │
│  SceneData                                                   │
│                                                              │
│  ┌────────────────────────────────────────────────┐         │
│  │ SceneData {                                    │         │
│  │   scene_type = SceneType_CRAFTING,            │         │
│  │   scene_id = "crafting-001",                  │         │
│  │   render_texture_width = 1920,                │         │
│  │   render_texture_height = 1080                │         │
│  │ }                                              │         │
│  └────────────────────────────────────────────────┘         │
└──────────────────────────────────────────────────────────────┘
                        │
                        │ Used by SceneFactory
                        │
                        ↓
┌──────────────────────────────────────────────────────────────┐
│          Native C++ Struct (Complete State)                  │
│  SceneStateData                                              │
│                                                              │
│  ┌────────────────────────────────────────────────┐         │
│  │ SceneStateData {                               │         │
│  │   scene_type = SceneType_CRAFTING,            │         │
│  │   scene_id = "crafting-001",                  │         │
│  │   render_texture_width = 1920,                │         │
│  │   render_texture_height = 1080,               │         │
│  │   entity_data = EntityCollectionData { ... }  │         │
│  │ }                                              │         │
│  └────────────────────────────────────────────────┘         │
└──────────────────────────────────────────────────────────────┘
                        │
                        │ Used by EntityConfigurator
                        │
                        ↓
┌──────────────────────────────────────────────────────────────┐
│                  Configured Scene                            │
│  Scene object with restored state from save file            │
└──────────────────────────────────────────────────────────────┘
```

---

## Provider Architecture

### Interface Hierarchy

```
┌────────────────────────────────────────────────────────────┐
│               Provider Interfaces (Abstract)               │
│                                                            │
│  ┌──────────────────────┐  ┌──────────────────────┐      │
│  │ ISceneDataProvider   │  │ ISaveDataProvider    │      │
│  │                      │  │                      │      │
│  │ virtual:             │  │ virtual:             │      │
│  │ LoadSceneData()      │  │ LoadSave()           │      │
│  │                      │  │ SaveGame()           │      │
│  │                      │  │ GetSaveSlots()       │      │
│  │                      │  │ DeleteSave()         │      │
│  └──────────────────────┘  └──────────────────────┘      │
│           ↑                          ↑                     │
│           │                          │                     │
└───────────┼──────────────────────────┼─────────────────────┘
            │                          │
            │ implements               │ implements
            │                          │
┌───────────┼──────────────────────────┼─────────────────────┐
│           │                          │                     │
│  ┌────────┴────────────┐   ┌─────────┴──────────┐        │
│  │ FlatBuffers         │   │ FlatBuffers        │        │
│  │ SceneDataProvider   │   │ SaveDataProvider   │        │
│  │                     │   │                    │        │
│  │ - m_loader          │   │ - kMaxSaveSlots    │        │
│  │                     │   │                    │        │
│  │ LoadSceneData():    │   │ LoadSave():        │        │
│  │ 1. Load .bin file   │   │ 1. Load save file  │        │
│  │ 2. Parse FlatBuffers│   │ 2. Parse FlatBuf   │        │
│  │ 3. Convert→native   │   │ 3. Convert→native  │        │
│  │ 4. Return SceneData │   │ 4. Return SaveData │        │
│  └─────────────────────┘   └────────────────────┘        │
│                                                            │
│           FlatBuffers Implementations (Concrete)          │
└────────────────────────────────────────────────────────────┘
            │                          │
            │ future                   │ future
            │                          │
┌───────────┼──────────────────────────┼─────────────────────┐
│           │                          │                     │
│  ┌────────┴────────────┐   ┌─────────┴──────────┐        │
│  │ Json                │   │ Json               │        │
│  │ SceneDataProvider   │   │ SaveDataProvider   │        │
│  │                     │   │                    │        │
│  │ LoadSceneData():    │   │ LoadSave():        │        │
│  │ 1. Load .json file  │   │ 1. Load .json      │        │
│  │ 2. Parse JSON       │   │ 2. Parse JSON      │        │
│  │ 3. Convert→native   │   │ 3. Convert→native  │        │
│  │ 4. Return SceneData │   │ 4. Return SaveData │        │
│  └─────────────────────┘   └────────────────────┘        │
│                                                            │
│              JSON Implementations (Future)                │
└────────────────────────────────────────────────────────────┘
```

### Provider Factory

```
┌────────────────────────────────────────────────────────────┐
│                    Provider Factory                        │
│  provider_factory.h/cpp                                   │
│                                                            │
│  ┌────────────────────────────────────────────┐           │
│  │ ISceneDataProvider& GetSceneDataProvider() │           │
│  │ ISaveDataProvider& GetSaveDataProvider()   │           │
│  │ IEngineDataProvider& GetEngineDataProvider()│          │
│  │ IEntityDataProvider& GetEntityDataProvider()│          │
│  │ IAssetDataProvider& GetAssetDataProvider() │           │
│  └────────────────────────────────────────────┘           │
│                       │                                    │
│                       │ returns singleton                  │
│                       ↓                                    │
│  ┌────────────────────────────────────────────┐           │
│  │ static FlatbuffersSceneDataProvider        │           │
│  │ static FlatbuffersSaveDataProvider         │           │
│  │ static FlatbuffersEngineDataProvider       │           │
│  │ ...                                        │           │
│  └────────────────────────────────────────────┘           │
│                                                            │
│  Benefits:                                                 │
│  ✅ Single access point                                   │
│  ✅ Easy to swap implementations                          │
│  ✅ Consistent across codebase                            │
│  ✅ Singleton pattern for stateless providers             │
└────────────────────────────────────────────────────────────┘
```

---

## Complete End-to-End Flow

### Full Engine Startup to Running Scene

```
┌─────────────────────────────────────────────────────────────────┐
│ 1. Engine Initialization                                        │
│                                                                  │
│  main.cpp                                                        │
│    ↓                                                             │
│  GameEngine engine;                                              │
│  engine.RunGame();                                               │
│    ↓                                                             │
│  Engine::RunGame()                                               │
│    ├─ StartUp();          ← Configure engine resources          │
│    └─ RunGameLoop();      ← Main game loop                      │
└─────────────────────────────────────────────────────────────────┘
                        │
                        ↓
┌─────────────────────────────────────────────────────────────────┐
│ 2. Engine StartUp                                               │
│                                                                  │
│  IEngineDataProvider& provider = GetEngineDataProvider();       │
│    ↓                                                             │
│  LoadEngineResourcesConfig()                                    │
│  LoadEngineConfig()                                              │
│  LoadEngineState()                                               │
│    ↓                                                             │
│  EngineResources configured ✅                                  │
│  EngineConfig configured ✅                                     │
│  EngineState configured ✅                                      │
└─────────────────────────────────────────────────────────────────┘
                        │
                        ↓
┌─────────────────────────────────────────────────────────────────┐
│ 3. Initial Scene Load                                           │
│                                                                  │
│  SceneManager::LoadTitleScene()                                 │
│    ↓                                                             │
│  AddSceneFromDefault(SceneType::SceneType_TITLE)                │
│    ↓                                                             │
│  SceneFactory::CreateDefaultScene()                             │
└─────────────────────────────────────────────────────────────────┘
                        │
                        ↓
┌─────────────────────────────────────────────────────────────────┐
│ 4. Scene Metadata Loading                                       │
│                                                                  │
│  ISceneDataProvider& provider = GetSceneDataProvider();         │
│    ↓                                                             │
│  provider.LoadSceneData(SceneType::SceneType_TITLE)             │
│    ↓                                                             │
│  FlatbuffersSceneDataProvider:                                  │
│    ├─ Load: data/scenes/title.scene_data.bin                   │
│    ├─ Parse FlatBuffers                                         │
│    ├─ Convert to SceneData struct                               │
│    └─ Return SceneData                                          │
│    ↓                                                             │
│  SceneData { type, id, width, height } ✅                      │
└─────────────────────────────────────────────────────────────────┘
                        │
                        ↓
┌─────────────────────────────────────────────────────────────────┐
│ 5. Scene Creation                                               │
│                                                                  │
│  SceneFactory:                                                   │
│    ├─ Create Scene object (TitleScene)                          │
│    ├─ Configure render texture dimensions                       │
│    └─ Return scene_ptr                                          │
│    ↓                                                             │
│  Scene object created ✅                                        │
└─────────────────────────────────────────────────────────────────┘
                        │
                        ↓
┌─────────────────────────────────────────────────────────────────┐
│ 6. Entity Configuration [⚠️ NEEDS IMPROVEMENT]                  │
│                                                                  │
│  scene->ConfigureFromDefault()                                  │
│    ↓                                                             │
│  EntityManager::ConfigureEntitiesFromDefaultData()              │
│    ↓                                                             │
│  FlatbuffersConfigurator::ConfigureEntities()                   │
│    ├─ [Currently uses FlatBuffers directly ❌]                 │
│    ├─ [FUTURE: IEntityDataProvider interface ✅]               │
│    ↓                                                             │
│  EntityMemoryPool configured ✅                                 │
└─────────────────────────────────────────────────────────────────┘
                        │
                        ↓
┌─────────────────────────────────────────────────────────────────┐
│ 7. Archetype Generation                                         │
│                                                                  │
│  ArchetypeManager::GenerateAllArchetypes()                      │
│    ↓                                                             │
│  Iterate through EntityMemoryPool                               │
│  Group entities by component signatures                         │
│    ↓                                                             │
│  Archetypes generated ✅                                        │
└─────────────────────────────────────────────────────────────────┘
                        │
                        ↓
┌─────────────────────────────────────────────────────────────────┐
│ 8. Logic System Creation                                        │
│                                                                  │
│  LogicFactory::CreateLogicMap(scene_type, scene_context)        │
│    ↓                                                             │
│  CreateCollisionLogics()                                        │
│  CreateRenderLogics()                                           │
│  CreateActionLogics()                                           │
│  CreateMovementLogics()                                         │
│    ↓                                                             │
│  LogicCollection configured ✅                                  │
└─────────────────────────────────────────────────────────────────┘
                        │
                        ↓
┌─────────────────────────────────────────────────────────────────┐
│ 9. Scene Ready                                                  │
│                                                                  │
│  Scene fully configured:                                         │
│    ✅ Metadata (type, id, dimensions)                           │
│    ✅ Render texture                                             │
│    ✅ Entity memory pool                                         │
│    ✅ Archetypes                                                 │
│    ✅ Logic systems                                              │
│    ↓                                                             │
│  SceneManager stores scene                                       │
│    ↓                                                             │
│  m_scenes[uuid] = std::move(scene_ptr)                          │
└─────────────────────────────────────────────────────────────────┘
                        │
                        ↓
┌─────────────────────────────────────────────────────────────────┐
│ 10. Game Loop Running                                           │
│                                                                  │
│  Engine::RunGameLoop()                                           │
│    ↓                                                             │
│  while (window.isOpen()) {                                       │
│    ExecuteTick();                                                │
│      ├─ TickEvents()          ← Process input                   │
│      ├─ TickEngineLogic()     ← Engine subscriptions            │
│      ├─ TickSceneManager()    ← Scene manager logic             │
│      ├─ TickSceneLogic()      ← Update scenes                   │
│      └─ TickRendering()       ← Render to display               │
│  }                                                               │
└─────────────────────────────────────────────────────────────────┘
```

---

## Key Architectural Concepts

### Data Transformation Layers

```
FlatBuffers Binary File (Serialized)
            ↓ [Provider reads and parses]
FlatBuffers Object (In-Memory, Format-Specific)
            ↓ [Provider converts to native]
Native C++ Struct (Format-Agnostic)
            ↓ [Factory uses for creation]
Game Object (Configured and Ready)
            ↓ [Used by game loop]
Running Game State
```

### Separation of Concerns

```
┌────────────────┐
│ Data Layer     │  Files, Serialization
└────────────────┘
        ↓
┌────────────────┐
│ Provider Layer │  Loading, Format Conversion
└────────────────┘
        ↓
┌────────────────┐
│ Struct Layer   │  Native C++, Format-Agnostic
└────────────────┘
        ↓
┌────────────────┐
│ Factory Layer  │  Object Creation
└────────────────┘
        ↓
┌────────────────┐
│ Configurator   │  Object Configuration
│ Layer          │
└────────────────┘
        ↓
┌────────────────┐
│ Object Layer   │  Game Objects (Scene, Entities)
└────────────────┘
        ↓
┌────────────────┐
│ Game Loop      │  Running Game
└────────────────┘
```

---

**Document Status**: Visual Architecture Complete
