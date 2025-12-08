# Scene Factory and Configurator Architecture - Visual Diagrams

**Date**: December 8, 2025  
**Related**: SCENE_FACTORY_AND_CONFIGURATOR_ANALYSIS.md, SCENE_FACTORY_QUICK_REF.md

---

## System Overview Diagram

```
┌───────────────────────────────────────────────────────────────────────────┐
│                          GAME ENGINE                                       │
│                                                                            │
│  ┌──────────────────────────────────────────────────────────────────┐    │
│  │                      SceneManager                                 │    │
│  │                                                                   │    │
│  │  ┌──────────────────────┐        ┌──────────────────────┐       │    │
│  │  │ ISceneDataProvider   │        │ ISaveDataProvider    │       │    │
│  │  │ (default scenes)     │        │ (saved games)        │       │    │
│  │  └──────────────────────┘        └──────────────────────┘       │    │
│  │           │                                  │                   │    │
│  │           │ LoadSceneData()                  │ LoadSave()        │    │
│  │           ▼                                  ▼                   │    │
│  │      SceneData                          SaveData                │    │
│  │           │                                  │                   │    │
│  │           │                                  │                   │    │
│  │           │                      ┌───────────▼───────────┐      │    │
│  │           │                      │ SceneDataExtractor    │      │    │
│  │           │                      │ ExtractSceneData()    │      │    │
│  │           │                      └───────────┬───────────┘      │    │
│  │           │                                  │                   │    │
│  │           │                                  ▼                   │    │
│  │           └──────────────────────────► SceneData                │    │
│  │                                              │                   │    │
│  │  ┌───────────────────────────────────────────────────────────┐  │    │
│  │  │              Create Configurators                         │  │    │
│  │  │                                                           │  │    │
│  │  │  IEntityConfigurator ──► ISceneConfigurator              │  │    │
│  │  │  (FlatBuffers)            (Default or Saved)             │  │    │
│  │  └───────────────────────────────────────────────────────────┘  │    │
│  │                                              │                   │    │
│  │                                              ▼                   │    │
│  │                                      SceneFactory                │    │
│  │                                   (with configurator)            │    │
│  │                                              │                   │    │
│  │                                              ▼                   │    │
│  │                                         Scene                    │    │
│  │                                    (fully configured)            │    │
│  │                                              │                   │    │
│  │                                              ▼                   │    │
│  │                                    m_scenes[uuid]                │    │
│  └──────────────────────────────────────────────────────────────────┘    │
│                                                                            │
└───────────────────────────────────────────────────────────────────────────┘
```

---

## Class Hierarchy Diagram

```
┌─────────────────────────────────────────────────────────────────────┐
│                    SCENE CONFIGURATION HIERARCHY                     │
└─────────────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────┐
│                  ISceneConfigurator                         │
│  (Abstract Interface)                                       │
├────────────────────────────────────────────────────────────┤
│  + ConfigureRenderTexture(Scene&, SceneData&) = 0          │
│  + ConfigureEntities(Scene&, SceneData&) = 0               │
│  + ConfigureLogic(Scene&, SceneData&) = 0                  │
└────────────────────────────────────────────────────────────┘
                          △
                          │
                          │ inherits
        ┌─────────────────┴─────────────────┐
        │                                   │
        │                                   │
┌───────▼──────────────────┐    ┌──────────▼─────────────────┐
│ DefaultSceneConfigurator │    │ SavedSceneConfigurator     │
│                          │    │                            │
├──────────────────────────┤    ├────────────────────────────┤
│ - IEntityConfigurator*   │    │ - IEntityConfigurator*     │
├──────────────────────────┤    ├────────────────────────────┤
│ + ConfigureRenderTexture │    │ + ConfigureRenderTexture   │
│ + ConfigureEntities      │    │ + ConfigureEntities        │
│ + ConfigureLogic         │    │ + ConfigureLogic           │
└──────────────────────────┘    └────────────────────────────┘
           │                                │
           │ composes                       │ composes
           ▼                                ▼
┌────────────────────────────────────────────────────────────┐
│                  IEntityConfigurator                        │
│  (Abstract Interface)                                       │
├────────────────────────────────────────────────────────────┤
│  # EventHandler& m_event_handler                           │
├────────────────────────────────────────────────────────────┤
│  + ConfigureFromDefault(EntityManager&, SceneType) = 0     │
│  + ConfigureFromSave(EntityManager&, SceneData&) = 0       │
│  + ConfigureFromTest(EntityManager&, TestConfig&) = 0      │
└────────────────────────────────────────────────────────────┘
                          △
                          │
                          │ inherits
        ┌─────────────────┴─────────────────┬────────────────┐
        │                                   │                │
┌───────▼───────────────────┐   ┌──────────▼────────┐   ┌──▼──────────────┐
│FlatbuffersEntity          │   │ JsonEntity        │   │ MockEntity      │
│Configurator               │   │ Configurator      │   │ Configurator    │
│                           │   │ (future)          │   │ (testing)       │
├───────────────────────────┤   └───────────────────┘   └─────────────────┘
│ - FlatbuffersDataLoader   │
├───────────────────────────┤
│ + ConfigureFromDefault    │
│ + ConfigureFromSave       │
│ + ConfigureFromTest       │
├───────────────────────────┤
│ # ConfigureCUserInterface │
│ # ConfigureCGrimoire...   │
│   (private helpers)       │
└───────────────────────────┘
```

---

## Sequence Diagram: Loading Default Scene

```
User          SceneManager    ISceneDataProvider    SceneFactory    ISceneConfigurator    Scene
 │                  │                 │                   │                 │              │
 │ LoadTitle()      │                 │                   │                 │              │
 ├─────────────────►│                 │                   │                 │              │
 │                  │                 │                   │                 │              │
 │                  │ LoadSceneFromDefault(TITLE)         │                 │              │
 │                  ├────────┐        │                   │                 │              │
 │                  │        │        │                   │                 │              │
 │                  │◄───────┘        │                   │                 │              │
 │                  │                 │                   │                 │              │
 │                  │ LoadSceneData(TITLE)                │                 │              │
 │                  ├────────────────►│                   │                 │              │
 │                  │                 │                   │                 │              │
 │                  │    SceneData    │                   │                 │              │
 │                  │◄────────────────┤                   │                 │              │
 │                  │                 │                   │                 │              │
 │                  │ new DefaultSceneConfigurator()      │                 │              │
 │                  ├─────────────────────────────────────┼────────────────►│              │
 │                  │                 │                   │                 │              │
 │                  │ new SceneFactory(configurator)      │                 │              │
 │                  ├────────────────────────────────────►│                 │              │
 │                  │                 │                   │                 │              │
 │                  │ CreateScene(SceneData, GameContext) │                 │              │
 │                  ├────────────────────────────────────►│                 │              │
 │                  │                 │                   │                 │              │
 │                  │                 │                   │ new Scene()     │              │
 │                  │                 │                   ├────────────────────────────────►│
 │                  │                 │                   │                 │              │
 │                  │                 │                   │ ConfigureRenderTexture(scene)  │
 │                  │                 │                   ├────────────────►│              │
 │                  │                 │                   │                 ├─────────────►│
 │                  │                 │                   │                 │              │
 │                  │                 │                   │ ConfigureEntities(scene)       │
 │                  │                 │                   ├────────────────►│              │
 │                  │                 │                   │                 ├─────────────►│
 │                  │                 │                   │                 │              │
 │                  │                 │                   │ ConfigureLogic(scene)          │
 │                  │                 │                   ├────────────────►│              │
 │                  │                 │                   │                 ├─────────────►│
 │                  │                 │                   │                 │              │
 │                  │                 │                   │    Scene        │              │
 │                  │                 │                   │◄───────────────────────────────┤
 │                  │                 │                   │                 │              │
 │                  │    Scene        │                   │                 │              │
 │                  │◄────────────────────────────────────┤                 │              │
 │                  │                 │                   │                 │              │
 │                  │ m_scenes.emplace(id, scene)         │                 │              │
 │                  ├────────┐        │                   │                 │              │
 │                  │        │        │                   │                 │              │
 │                  │◄───────┘        │                   │                 │              │
 │                  │                 │                   │                 │              │
 │    scene_id      │                 │                   │                 │              │
 │◄─────────────────┤                 │                   │                 │              │
 │                  │                 │                   │                 │              │
```

---

## Sequence Diagram: Loading Saved Scene

```
User          SceneManager    ISaveDataProvider  SceneDataExtractor  SceneFactory    Scene
 │                  │                 │                 │                 │              │
 │ LoadSave(slot)   │                 │                 │                 │              │
 ├─────────────────►│                 │                 │                 │              │
 │                  │                 │                 │                 │              │
 │                  │ LoadSceneFromSave(slot)           │                 │              │
 │                  ├────────┐        │                 │                 │              │
 │                  │        │        │                 │                 │              │
 │                  │◄───────┘        │                 │                 │              │
 │                  │                 │                 │                 │              │
 │                  │ LoadSave(slot)  │                 │                 │              │
 │                  ├────────────────►│                 │                 │              │
 │                  │                 │                 │                 │              │
 │                  │    SaveData     │                 │                 │              │
 │                  │◄────────────────┤                 │                 │              │
 │                  │                 │                 │                 │              │
 │                  │ ExtractSceneData(SaveData)        │                 │              │
 │                  ├──────────────────────────────────►│                 │              │
 │                  │                 │                 │                 │              │
 │                  │           SceneData               │                 │              │
 │                  │◄──────────────────────────────────┤                 │              │
 │                  │                 │                 │                 │              │
 │                  │ new SavedSceneConfigurator()      │                 │              │
 │                  ├───────────────────────────────────┼────────────────►│              │
 │                  │                 │                 │                 │              │
 │                  │ CreateScene(SceneData, GameContext)                 │              │
 │                  ├────────────────────────────────────────────────────►│              │
 │                  │                 │                 │                 │              │
 │                  │                 │         (Configure scene)         │              │
 │                  │                 │                 │                 ├─────────────►│
 │                  │                 │                 │                 │              │
 │                  │                 │                 │    Scene        │              │
 │                  │◄────────────────────────────────────────────────────┤              │
 │                  │                 │                 │                 │              │
 │    scene_id      │                 │                 │                 │              │
 │◄─────────────────┤                 │                 │                 │              │
 │                  │                 │                 │                 │              │
```

---

## Data Flow Diagram: Default Scene

```
┌──────────────────┐
│  User Action     │ (New Game / Scene Transition)
│  "Load Title"    │
└────────┬─────────┘
         │
         ▼
┌────────────────────────────────────────────────────────────────┐
│              SceneManager::LoadSceneFromDefault()              │
└────────────────────────────────────────────────────────────────┘
         │
         ├──► Step 1: Get SceneData
         │    ┌────────────────────────────────────┐
         │    │  ISceneDataProvider                │
         │    │  LoadSceneData(SceneType_TITLE)    │
         │    └────────────────────────────────────┘
         │                │
         │                ▼
         │    ┌────────────────────────────────────┐
         │    │  SceneData {                       │
         │    │    scene_type: TITLE               │
         │    │    render_texture_width: 800       │
         │    │    render_texture_height: 600      │
         │    │  }                                 │
         │    └────────────────────────────────────┘
         │
         ├──► Step 2: Create Configurators
         │    ┌────────────────────────────────────┐
         │    │  new FlatbuffersEntity             │
         │    │      Configurator(event_handler)   │
         │    └────────────────────────────────────┘
         │                │
         │                ▼
         │    ┌────────────────────────────────────┐
         │    │  new DefaultSceneConfigurator(     │
         │    │      entity_configurator)          │
         │    └────────────────────────────────────┘
         │
         ├──► Step 3: Create Factory
         │    ┌────────────────────────────────────┐
         │    │  new SceneFactory(                 │
         │    │      scene_configurator)           │
         │    └────────────────────────────────────┘
         │
         ├──► Step 4: Create Scene
         │    ┌────────────────────────────────────┐
         │    │  factory.CreateScene(              │
         │    │      scene_data, game_context)     │
         │    └────────────────────────────────────┘
         │                │
         │                ├─► new Scene(uuid, game_context)
         │                │
         │                ├─► ConfigureRenderTexture()
         │                │        └─► Set dimensions from SceneData
         │                │
         │                ├─► ConfigureEntities()
         │                │        └─► IEntityConfigurator::ConfigureFromDefault()
         │                │                  └─► Load FlatBuffers entity data
         │                │                  └─► Configure each component
         │                │
         │                ├─► Generate Archetypes
         │                │        └─► ArchetypeManager::GenerateAllArchetypes()
         │                │
         │                └─► ConfigureLogic()
         │                         └─► LogicFactory::CreateLogicMap()
         │                         └─► scene.SetLogicMap()
         │                │
         │                ▼
         │    ┌────────────────────────────────────┐
         │    │  Fully Configured Scene            │
         │    └────────────────────────────────────┘
         │
         └──► Step 5: Add to Map
              ┌────────────────────────────────────┐
              │  m_scenes.emplace(uuid, scene)     │
              └────────────────────────────────────┘
                             │
                             ▼
              ┌────────────────────────────────────┐
              │  return scene_id                   │
              └────────────────────────────────────┘
```

---

## Data Flow Diagram: Saved Scene

```
┌──────────────────┐
│  User Action     │ (Load Game)
│  "Load Slot 0"   │
└────────┬─────────┘
         │
         ▼
┌────────────────────────────────────────────────────────────────┐
│              SceneManager::LoadSceneFromSave(slot_index)       │
└────────────────────────────────────────────────────────────────┘
         │
         ├──► Step 1: Load SaveData
         │    ┌────────────────────────────────────┐
         │    │  ISaveDataProvider                 │
         │    │  LoadSave(slot_index)              │
         │    └────────────────────────────────────┘
         │                │
         │                ▼
         │    ┌────────────────────────────────────┐
         │    │  SaveData {                        │
         │    │    metadata: {...}                 │
         │    │    current_scene_type: TITLE       │
         │    │    // Future: scene_states[]       │
         │    │  }                                 │
         │    └────────────────────────────────────┘
         │
         ├──► Step 2: Extract SceneData
         │    ┌────────────────────────────────────┐
         │    │  SceneDataExtractor                │
         │    │  ExtractSceneData(save_data)       │
         │    └────────────────────────────────────┘
         │                │
         │                ▼
         │    ┌────────────────────────────────────┐
         │    │  SceneData {                       │
         │    │    scene_type: TITLE               │
         │    │    // Extracted from SaveData      │
         │    │  }                                 │
         │    └────────────────────────────────────┘
         │
         ├──► Step 3: Create Configurators
         │    ┌────────────────────────────────────┐
         │    │  new FlatbuffersEntity             │
         │    │      Configurator(event_handler)   │
         │    └────────────────────────────────────┘
         │                │
         │                ▼
         │    ┌────────────────────────────────────┐
         │    │  new SavedSceneConfigurator(       │
         │    │      entity_configurator)          │
         │    └────────────────────────────────────┘
         │
         ├──► Step 4: Create Factory
         │    ┌────────────────────────────────────┐
         │    │  new SceneFactory(                 │
         │    │      scene_configurator)           │
         │    └────────────────────────────────────┘
         │
         ├──► Step 5: Create Scene
         │    ┌────────────────────────────────────┐
         │    │  factory.CreateScene(              │
         │    │      scene_data, game_context)     │
         │    └────────────────────────────────────┘
         │                │
         │                ├─► new Scene(uuid, game_context)
         │                │
         │                ├─► ConfigureRenderTexture()
         │                │
         │                ├─► ConfigureEntities()
         │                │        └─► IEntityConfigurator::ConfigureFromSave()
         │                │                  └─► Restore entity state from SaveData
         │                │
         │                ├─► Generate Archetypes
         │                │
         │                └─► ConfigureLogic()
         │                         └─► Restore logic state (future)
         │                │
         │                ▼
         │    ┌────────────────────────────────────┐
         │    │  Restored Scene                    │
         │    └────────────────────────────────────┘
         │
         └──► Step 6: Add to Map
              ┌────────────────────────────────────┐
              │  m_scenes.emplace(uuid, scene)     │
              └────────────────────────────────────┘
                             │
                             ▼
              ┌────────────────────────────────────┐
              │  return scene_id                   │
              └────────────────────────────────────┘
```

---

## Component Diagram: Scene Creation Flow

```
┌──────────────────────────────────────────────────────────────────────┐
│                        SCENE CREATION FLOW                            │
└──────────────────────────────────────────────────────────────────────┘

Input: SceneData
    │
    ▼
┌───────────────────────────────────────────┐
│          SceneFactory                      │
│  (owns ISceneConfigurator)                 │
└───────────────────────────────────────────┘
    │
    ├─► CreateScene(SceneData, GameContext)
    │
    ▼
┌───────────────────────────────────────────┐
│  1. Instantiate Scene                      │
│     switch(scene_data.scene_type)         │
│       TITLE → new TitleScene()            │
│       CRAFTING → new CraftingScene()      │
└───────────────────────────────────────────┘
    │
    ▼
┌───────────────────────────────────────────┐
│  2. Configure Render Texture               │
│     m_configurator->                       │
│       ConfigureRenderTexture(scene, data) │
│                                           │
│     ┌─────────────────────────────┐      │
│     │ scene.scene_texture =       │      │
│     │   RenderTexture(            │      │
│     │     data.render_width,      │      │
│     │     data.render_height)     │      │
│     └─────────────────────────────┘      │
└───────────────────────────────────────────┘
    │
    ▼
┌───────────────────────────────────────────┐
│  3. Configure Entities                     │
│     m_configurator->                       │
│       ConfigureEntities(scene, data)      │
│                                           │
│     ┌─────────────────────────────┐      │
│     │ m_entity_configurator->     │      │
│     │   ConfigureFromDefault(     │      │
│     │     entity_manager,         │      │
│     │     scene_type)             │      │
│     │                             │      │
│     │ OR                          │      │
│     │                             │      │
│     │ m_entity_configurator->     │      │
│     │   ConfigureFromSave(        │      │
│     │     entity_manager,         │      │
│     │     scene_data)             │      │
│     └─────────────────────────────┘      │
└───────────────────────────────────────────┘
    │
    ▼
┌───────────────────────────────────────────┐
│  4. Generate Archetypes                    │
│     scene.entity_manager.                  │
│       GenerateAllArchetypes()             │
└───────────────────────────────────────────┘
    │
    ▼
┌───────────────────────────────────────────┐
│  5. Configure Logic                        │
│     m_configurator->                       │
│       ConfigureLogic(scene, data)         │
│                                           │
│     ┌─────────────────────────────┐      │
│     │ LogicFactory factory(       │      │
│     │   scene_type, context)      │      │
│     │                             │      │
│     │ logic_map = factory.        │      │
│     │   CreateLogicMap(data)      │      │
│     │                             │      │
│     │ scene.SetLogicMap(          │      │
│     │   logic_map)                │      │
│     └─────────────────────────────┘      │
└───────────────────────────────────────────┘
    │
    ▼
Output: Configured Scene
```

---

## State Diagram: Scene Lifecycle

```
┌───────────────┐
│   START       │
└───────┬───────┘
        │
        │ SceneManager::LoadSceneFromDefault()
        │ or
        │ SceneManager::LoadSceneFromSave()
        │
        ▼
┌───────────────────────────────────────────┐
│         Scene Created                      │
│  (in SceneFactory.CreateScene())          │
└───────────────────────────────────────────┘
        │
        │ Constructor called
        │
        ▼
┌───────────────────────────────────────────┐
│         Scene Instantiated                 │
│  - UUID generated                          │
│  - GameContext referenced                  │
│  - Members initialized                     │
└───────────────────────────────────────────┘
        │
        │ ISceneConfigurator::ConfigureRenderTexture()
        │
        ▼
┌───────────────────────────────────────────┐
│      Render Texture Configured             │
│  - Dimensions set from SceneData           │
└───────────────────────────────────────────┘
        │
        │ ISceneConfigurator::ConfigureEntities()
        │   └─► IEntityConfigurator::ConfigureFromDefault/Save()
        │
        ▼
┌───────────────────────────────────────────┐
│          Entities Configured               │
│  - Components loaded                       │
│  - Entity memory pool populated            │
└───────────────────────────────────────────┘
        │
        │ EntityManager::GenerateAllArchetypes()
        │
        ▼
┌───────────────────────────────────────────┐
│        Archetypes Generated                │
│  - Entities grouped by component signature│
└───────────────────────────────────────────┘
        │
        │ ISceneConfigurator::ConfigureLogic()
        │   └─► LogicFactory::CreateLogicMap()
        │
        ▼
┌───────────────────────────────────────────┐
│          Logic Configured                  │
│  - Logic instances created                 │
│  - Subscribers attached                    │
└───────────────────────────────────────────┘
        │
        │ return scene to SceneManager
        │
        ▼
┌───────────────────────────────────────────┐
│       Scene Added to Map                   │
│  - m_scenes[uuid] = scene                  │
│  - Scene is ACTIVE                         │
└───────────────────────────────────────────┘
        │
        │ Game loop
        │
        ▼
┌───────────────────────────────────────────┐
│          Scene Running                     │
│  - sMovement() called                      │
│  - sCollision() called                     │
│  - sAction() called                        │
│  - sRender() called                        │
└───────────────────────────────────────────┘
        │
        │ Scene transition or game end
        │
        ▼
┌───────────────────────────────────────────┐
│         Scene Destroyed                    │
│  - Removed from m_scenes                   │
│  - Resources cleaned up                    │
└───────────────────────────────────────────┘
        │
        ▼
┌───────────────┐
│      END      │
└───────────────┘
```

---

## Decision Flow: Configuration Strategy Selection

```
                    ┌─────────────────────┐
                    │  Scene Creation     │
                    │  Requested          │
                    └──────────┬──────────┘
                               │
                ┌──────────────▼─────────────┐
                │  What is the data source?  │
                └────────────────────────────┘
                         │          │
                         │          │
        ┌────────────────┴──┐   ┌──▼─────────────────┐
        │ Default Scene     │   │ Saved Game         │
        │ (new game)        │   │ (load game)        │
        └────────┬──────────┘   └──┬─────────────────┘
                 │                  │
                 │                  │
                 ▼                  ▼
┌──────────────────────────┐  ┌──────────────────────────┐
│ ISceneDataProvider       │  │ ISaveDataProvider        │
│ LoadSceneData(type)      │  │ LoadSave(slot)           │
└────────┬─────────────────┘  └──┬───────────────────────┘
         │                        │
         │                        │ SceneDataExtractor
         │                        │ ExtractSceneData()
         │                        │
         ▼                        ▼
    SceneData                SceneData
         │                        │
         │                        │
         ├────────────────────────┤
         │                        │
         ▼                        ▼
┌────────────────────┐   ┌────────────────────┐
│ DefaultScene       │   │ SavedScene         │
│ Configurator       │   │ Configurator       │
└────────┬───────────┘   └────┬───────────────┘
         │                     │
         │                     │
         ├─────────────────────┤
         │                     │
         ▼                     ▼
    ┌────────────────────────────┐
    │  SceneFactory              │
    │  CreateScene()             │
    └────────┬───────────────────┘
             │
             ▼
        Configured Scene
```

---

## Migration Path Diagram

```
CURRENT STATE                       PHASE 1                        PHASE 2
┌──────────────────┐        ┌──────────────────┐         ┌──────────────────┐
│ EntityConfigurator│        │ IEntity          │         │ IEntity          │
│                   │───────►│ Configurator     │────────►│ Configurator     │
│ (concrete base)   │        │ (abstract)       │         │ (abstract)       │
└──────────────────┘        └──────────────────┘         └──────────────────┘
         │                           │                            │
         │                           │                            │
         ▼                           ▼                            ▼
┌──────────────────┐        ┌──────────────────┐         ┌──────────────────┐
│ Flatbuffers      │        │ Flatbuffers      │         │ Flatbuffers      │
│ Configurator     │───────►│ Entity           │────────►│ Entity           │
│                   │        │ Configurator     │         │ Configurator     │
│ (overloaded)     │        │ (virtual methods)│         │ (virtual methods)│
└──────────────────┘        └──────────────────┘         └──────────────────┘
         │                           │                            │
         │                           │                            │
         ▼                           ▼                            ▼
┌──────────────────┐        ┌──────────────────┐         ┌──────────────────┐
│ Scene::          │        │ Scene::          │         │ (method removed) │
│ ConfigureFrom    │───────►│ ConfigureFrom    │────────►│                  │
│ Default()        │        │ Default()        │         │                  │
└──────────────────┘        └──────────────────┘         └──────────────────┘


PHASE 3                         PHASE 4                       FINAL STATE
┌──────────────────┐        ┌──────────────────┐         ┌──────────────────┐
│ IScene           │        │ IScene           │         │ IScene           │
│ Configurator     │───────►│ Configurator     │────────►│ Configurator     │
│ (abstract)       │        │ (abstract)       │         │ (abstract)       │
└──────────────────┘        └──────────────────┘         └──────────────────┘
         │                           │                            │
         ├───────────┬───────────────┼────────────┬──────────────┤
         │           │               │            │              │
         ▼           ▼               ▼            ▼              ▼
┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌──────────────────┐
│ Default     │ │ Saved       │ │ Default     │ │ Saved            │
│ Scene       │ │ Scene       │ │ Scene       │ │ Scene            │
│ Configurator│ │ Configurator│ │ Configurator│ │ Configurator     │
└─────────────┘ └─────────────┘ └─────────────┘ └──────────────────┘
         │           │               │            │              │
         │           │               │            │              │
         ▼           ▼               ▼            ▼              ▼
┌──────────────────────────────────────────────────────────────────┐
│                      SceneFactory                                 │
│  (uses ISceneConfigurator strategy)                              │
└──────────────────────────────────────────────────────────────────┘
         │                           │                            │
         │                           │                            │
         ▼                           ▼                            ▼
┌──────────────────┐        ┌──────────────────┐         ┌──────────────────┐
│ SceneManager     │        │ SceneManager     │         │ SceneManager     │
│ AddSceneFrom     │───────►│ LoadSceneFrom    │────────►│ LoadSceneFrom    │
│ Default()        │        │ Default/Save()   │         │ Default/Save()   │
└──────────────────┘        └──────────────────┘         └──────────────────┘
```

---

## Comparison: Overloaded vs Virtual Methods

```
┌────────────────────────────────────────────────────────────────────┐
│              OVERLOADED METHODS (Current - NOT Recommended)         │
└────────────────────────────────────────────────────────────────────┘

class FlatbuffersConfigurator : public EntityConfigurator {
    ConfigureComponent(Component&);                    // Overload 1
    ConfigureComponent(UserInterfaceData*, CUserInterface&);  // Overload 2
    ConfigureComponent(GrimoireMachinaData*, CGrimoireMachina&); // Overload 3
};

CALL SITE:
    FlatbuffersConfigurator config;
    config.ConfigureComponent(component);  // ← Resolved at COMPILE TIME
    
    EntityConfigurator* base = &config;
    base->ConfigureComponent(component);   // ❌ ERROR: Base doesn't have this method

PROBLEMS:
    ❌ Not polymorphic - resolved at compile time
    ❌ Can't use through base class pointer
    ❌ Can't swap implementations
    ❌ Not truly virtual

┌────────────────────────────────────────────────────────────────────┐
│              VIRTUAL METHODS (Recommended)                          │
└────────────────────────────────────────────────────────────────────┘

class IEntityConfigurator {
    virtual ConfigureFromDefault(EntityManager&, SceneType) = 0;  // Virtual
    virtual ConfigureFromSave(EntityManager&, SceneData&) = 0;    // Virtual
};

class FlatbuffersEntityConfigurator : public IEntityConfigurator {
    ConfigureFromDefault(...) override { ... }  // Override virtual
    ConfigureFromSave(...) override { ... }     // Override virtual
};

CALL SITE:
    auto config = std::make_unique<FlatbuffersEntityConfigurator>();
    config->ConfigureFromDefault(manager, type);  // ✅ Works
    
    IEntityConfigurator* base = config.get();
    base->ConfigureFromDefault(manager, type);    // ✅ Works - polymorphic!
    
    // Can swap at runtime:
    base = new JsonEntityConfigurator();
    base->ConfigureFromDefault(manager, type);    // ✅ Different implementation

BENEFITS:
    ✅ Truly polymorphic - resolved at RUNTIME
    ✅ Works through base class pointer
    ✅ Can swap implementations at runtime
    ✅ Testable with mocks
```

---

**End of Visual Diagrams**
