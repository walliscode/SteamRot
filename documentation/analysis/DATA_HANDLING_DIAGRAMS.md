# Data Handling Architecture - Visual Diagrams

## System Overview

```
┌─────────────────────────────────────────────────────────────────────┐
│                         DATA SOURCES                                 │
├─────────────────────────────────────────────────────────────────────┤
│  defaults/scenes/        user/saves/           tests/data/          │
│  ├─ title.scene_data.bin ├─ slot_0.save.bin   ├─ test_scene.bin    │
│  └─ crafting.scene_data  └─ slot_1.save.bin   └─ ...               │
└────────────────┬────────────────────┬────────────────┬──────────────┘
                 │                    │                │
                 ↓                    ↓                ↓
┌─────────────────────────────────────────────────────────────────────┐
│                    PROVIDER LAYER (Abstraction)                      │
├─────────────────────────────────────────────────────────────────────┤
│  FlatbuffersDefault    FlatbuffersSave        TestScene             │
│  SceneDataProvider     SceneDataProvider      DataProvider          │
│         ↓                      ↓                      ↓              │
│  ISceneDataProvider    ISceneDataProvider    ISceneDataProvider     │
└────────────────┬────────────────────┬────────────────┬──────────────┘
                 │                    │                │
                 └────────────────────┴────────────────┘
                                      │
                                      ↓ SceneData (native C++)
┌─────────────────────────────────────────────────────────────────────┐
│                     OBJECT CREATION LAYER                            │
├─────────────────────────────────────────────────────────────────────┤
│                        SceneFactory                                  │
│  CreateSceneByType(SceneType) → std::unique_ptr<Scene>              │
│                                      ↓                               │
│         TitleScene              CraftingScene                        │
└────────────────────────────────────┬────────────────────────────────┘
                                     │ Scene* (empty)
                                     ↓
┌─────────────────────────────────────────────────────────────────────┐
│                   CONFIGURATION LAYER                                │
├─────────────────────────────────────────────────────────────────────┤
│                   DefaultSceneConfigurator                           │
│  ConfigureScene(Scene&, SceneData&)                                 │
│    ├─ ConfigureSceneInfo()                                          │
│    ├─ ConfigureSceneResources()                                     │
│    ├─ ConfigureEntities() ────────────┐                             │
│    └─ ConfigureLogicMap()             │                             │
└───────────────────────────────────────┼─────────────────────────────┘
                                        │
                                        ↓ EntityCollection
┌─────────────────────────────────────────────────────────────────────┐
│                 ENTITY CONFIGURATION LAYER                           │
├─────────────────────────────────────────────────────────────────────┤
│              FlatbuffersEntityConfigurator                           │
│  ConfigureEntityMemoryPool(EntityMemoryPool&)                       │
│    ├─ ConfigureFirstLayerComponents()                               │
│    └─ ConfigureSecondLayerComponents()                              │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Current Architecture (Problematic)

```
┌──────────────────┐
│ SceneManager     │
└────────┬─────────┘
         │
         ├─ GetDefaultSceneConfigurator()
         │        ↓
         │  ┌───────────────────────────────────────┐
         │  │ FlatbuffersDefaultSceneConfigurator   │
         │  ├───────────────────────────────────────┤
         │  │ - FlatbuffersDataLoader m_data_loader │ ❌ Tightly coupled!
         │  │                                       │
         │  │ ConfigureSceneInfo(Scene&, SceneType) │
         │  │   └─> m_data_loader.ProvideSceneData()│ ❌ Loads internally
         │  │                                       │
         │  │ ConfigureSceneResources(...)          │
         │  │   └─> m_data_loader.ProvideSceneData()│ ❌ Redundant!
         │  └───────────────────────────────────────┘
         │
         └─ SceneFactory(game_context, configurator)
                 ├─> CreateSceneByType(type)
                 └─> configurator.ConfigureScene(scene, type)
                          └─> Loads FlatBuffers internally ❌

Problems:
1. Configurator knows about FlatBuffers
2. Cannot easily support XML/JSON/other formats
3. Redundant data loading
4. Testing requires mocking FlatbuffersDataLoader
```

---

## Recommended Architecture (Clean)

```
┌──────────────────┐
│ SceneManager     │
└────────┬─────────┘
         │
         ├─ (1) GetSceneDataProvider()
         │        ↓
         │  ┌────────────────────────────────┐
         │  │ ISceneDataProvider             │ ✅ Abstraction!
         │  ├────────────────────────────────┤
         │  │ LoadSceneData(SceneType)       │
         │  │   → SceneData                  │
         │  └────────┬───────────────────────┘
         │           │ implements
         │           ↓
         │  ┌────────────────────────────────────┐
         │  │ FlatbuffersDefaultSceneDataProvider│
         │  ├────────────────────────────────────┤
         │  │ - FlatbuffersDataLoader m_loader   │ ✅ Hidden!
         │  │                                    │
         │  │ LoadSceneData(SceneType) override  │
         │  │   ├─> m_loader.ProvideSceneData()  │
         │  │   └─> Convert FlatBuffers → native │
         │  │       └─> return SceneData         │
         │  └────────────────────────────────────┘
         │
         │  scene_data = provider.LoadSceneData(type) ✅
         │
         ├─ (2) SceneFactory(game_context)
         │        ↓
         │  ┌────────────────────────────────┐
         │  │ SceneFactory                   │ ✅ Simple!
         │  ├────────────────────────────────┤
         │  │ CreateSceneByType(SceneType)   │
         │  │   switch(type) {               │
         │  │     case TITLE:                │
         │  │       return new TitleScene(); │
         │  │     case CRAFTING:             │
         │  │       return new CraftingScene│
         │  │   }                            │
         │  └────────────────────────────────┘
         │
         │  scene = factory.CreateSceneByType(type) ✅
         │
         └─ (3) GetSceneConfigurator()
                  ↓
            ┌─────────────────────────────────┐
            │ ISceneConfigurator              │ ✅ Abstraction!
            ├─────────────────────────────────┤
            │ ConfigureScene(Scene&,          │
            │                SceneData&)      │
            └────────┬────────────────────────┘
                     │ implements
                     ↓
            ┌─────────────────────────────────┐
            │ DefaultSceneConfigurator        │ ✅ Generic!
            ├─────────────────────────────────┤
            │ ConfigureScene(Scene&,          │
            │                SceneData&)      │
            │   ├─> ConfigureInfo(data.info)  │
            │   ├─> ConfigureResources(...)   │
            │   ├─> ConfigureEntities(...)    │
            │   └─> ConfigureLogicMap()       │
            └─────────────────────────────────┘

            configurator.ConfigureScene(scene, scene_data) ✅

Benefits:
1. ✅ Configurator is data-source agnostic
2. ✅ Easy to add XML/JSON providers
3. ✅ Data loaded once, passed through
4. ✅ Easy to test with mock providers
```

---

## Data Flow: Default Scene Creation

```
┌─────────────────────────────────────────────────────────────────────┐
│ Step 1: Load Data                                                    │
└─────────────────────────────────────────────────────────────────────┘

ISceneDataProvider &provider = GetDefaultSceneDataProvider();
auto scene_data = provider.LoadSceneData(SceneType::TITLE);

         ↓
    
┌──────────────────────┐
│ .bin File on Disk    │
│ title.scene_data.bin │
└──────────┬───────────┘
           │ FlatbuffersDataLoader::Load()
           ↓
┌──────────────────────┐
│ FlatBuffers Memory   │
│ SceneDataFbs*        │
└──────────┬───────────┘
           │ Provider::Convert()
           ↓
┌──────────────────────┐
│ Native C++ Struct    │
│ SceneData            │
│  ├─ info             │
│  ├─ resources        │
│  ├─ entities         │
│  └─ logic            │
└──────────┬───────────┘
           │
           └─────────────────────────────────┐
                                             │
┌────────────────────────────────────────────┼────────────────────────┐
│ Step 2: Create Empty Scene                 │                        │
└────────────────────────────────────────────┼────────────────────────┘
                                             │
SceneFactory factory(game_context);          │
auto scene = factory.CreateSceneByType(TITLE);│
                                             │
         ↓                                   │
                                             │
┌──────────────────────┐                    │
│ TitleScene           │                    │
│ (empty, unconfigured)│                    │
└──────────┬───────────┘                    │
           │                                 │
           └────────────┬────────────────────┘
                        │
┌───────────────────────┼────────────────────────────────────────────┐
│ Step 3: Configure Scene with Data         │                        │
└───────────────────────┼────────────────────────────────────────────┘
                        │
ISceneConfigurator &configurator = GetSceneConfigurator();
configurator.ConfigureScene(*scene, scene_data);
                        │
         ┌──────────────┴──────────────┐
         │                             │
         ↓                             ↓
┌──────────────────┐         ┌──────────────────┐
│ TitleScene       │         │ SceneData        │
│ (empty)          │  +      │ (loaded)         │
└──────────────────┘         └──────────────────┘
         │                             │
         └──────────────┬──────────────┘
                        │
                        ↓
              ┌──────────────────┐
              │ TitleScene       │
              │ (fully configured)│
              │  ├─ scene_info   │
              │  ├─ resources    │
              │  ├─ entities     │
              │  └─ logic_map    │
              └──────────────────┘
```

---

## Data Flow: Save Game Scene Creation

```
Same as default, but Step 1 uses different provider!

┌─────────────────────────────────────────────────────────────────────┐
│ Step 1: Load Data (from save file)                                  │
└─────────────────────────────────────────────────────────────────────┘

ISceneDataProvider &provider = GetSaveSceneDataProvider();
auto scene_data = provider.LoadSceneData(SceneType::TITLE, save_data);

         ↓
    
┌──────────────────────┐
│ Save File on Disk    │
│ slot_0.save.bin      │
└──────────┬───────────┘
           │ FlatbuffersDataLoader::Load()
           ↓
┌──────────────────────┐
│ SaveDataFbs*         │
│  └─ scene_data       │
└──────────┬───────────┘
           │ Provider::Convert()
           ↓
┌──────────────────────┐
│ SceneData            │
│ (same native struct!)│
└──────────┬───────────┘
           │
           └─ Steps 2 & 3 are IDENTICAL to default!

The beauty: Only the provider changes. Factory and Configurator are reused!
```

---

## Provider Pattern Detail

```
┌────────────────────────────────────────────────────────────────────┐
│                    ISceneDataProvider                               │
│  (Interface - Pure abstraction)                                     │
├────────────────────────────────────────────────────────────────────┤
│  virtual SceneData LoadSceneData(SceneType) const = 0;             │
└────────────────────────────────────────────────────────────────────┘
                                  ▲
                                  │ implements
                ┌─────────────────┼─────────────────┐
                │                 │                 │
                ↓                 ↓                 ↓
┌───────────────────────┐ ┌──────────────────┐ ┌─────────────────┐
│ FlatbuffersDefault    │ │ FlatbuffersSave  │ │ TestScene       │
│ SceneDataProvider     │ │ SceneDataProvider│ │ DataProvider    │
├───────────────────────┤ ├──────────────────┤ ├─────────────────┤
│ - DataLoader m_loader │ │ - SaveData m_save│ │ - TestConfig    │
│                       │ │                  │ │   m_config      │
│ LoadSceneData()       │ │ LoadSceneData()  │ │                 │
│   ├─> Load .bin       │ │   ├─> Load save  │ │ LoadSceneData() │
│   ├─> Convert FBS     │ │   ├─> Convert FBS│ │   └─> Generate  │
│   └─> Return native   │ │   └─> Return     │ │       test data │
└───────────────────────┘ └──────────────────┘ └─────────────────┘

All return the SAME native type: SceneData

This allows:
1. Configurator doesn't care about source
2. Easy to add new sources (XML, JSON, procedural)
3. Testing with mock providers
```

---

## Configurator Reusability

```
┌───────────────────────────────────────────────────────────────┐
│              DefaultSceneConfigurator                          │
│  (One implementation, works with ALL data sources)             │
├───────────────────────────────────────────────────────────────┤
│  ConfigureScene(Scene &scene, const SceneData &data)          │
│    ├─ scene.m_scene_info = data.info;                         │
│    ├─ ConfigureResources(scene.m_resources, data.resources);  │
│    ├─ ConfigureEntities(scene.m_entity_manager, data.entities)│
│    └─ ConfigureLogic(scene.m_logic_map, data.logic);          │
└───────────────────────────────────────────────────────────────┘
                    ▲               ▲               ▲
                    │               │               │
              SceneData       SceneData       SceneData
                    │               │               │
        ┌───────────┴───────┐      │      ┌────────┴────────┐
        │                   │      │      │                 │
┌───────────────┐  ┌────────────────┐  ┌──────────────────┐
│ Default       │  │ Save           │  │ Test             │
│ Provider      │  │ Provider       │  │ Provider         │
└───────────────┘  └────────────────┘  └──────────────────┘

Same configurator, different data sources!
No need for separate configurator per source!
```

---

## Comparison: Current vs Recommended

### Current Architecture Issues

```
┌─────────────────────────────────────────────────────────────┐
│ SceneManager wants to create a scene from save data         │
└─────────────────────────────────────────────────────────────┘
                              │
                              ↓
              Need new configurator? ❌
                              │
┌─────────────────────────────────────────────────────────────┐
│ FlatbuffersSaveSceneConfigurator                            │
├─────────────────────────────────────────────────────────────┤
│ - FlatbuffersDataLoader m_loader                            │
│ - SaveData m_save_data                                      │
│                                                             │
│ ConfigureSceneInfo(Scene&, SceneType)                       │
│   └─> Load from save file                                   │
│                                                             │
│ ConfigureSceneResources(Scene&, SceneType)                  │
│   └─> Load from save file                                   │
└─────────────────────────────────────────────────────────────┘

Problems:
- Need new configurator for each data source
- Duplicate configuration logic
- More classes to maintain
```

### Recommended Architecture

```
┌─────────────────────────────────────────────────────────────┐
│ SceneManager wants to create a scene from save data         │
└─────────────────────────────────────────────────────────────┘
                              │
                              ↓
              Use different provider! ✅
                              │
┌─────────────────────────────────────────────────────────────┐
│ GetSaveSceneDataProvider()                                  │
│   └─> Returns ISceneDataProvider& that loads from save      │
└─────────────────────────────────────────────────────────────┘
                              │
                              ↓
┌─────────────────────────────────────────────────────────────┐
│ Same SceneFactory, Same Configurator!                       │
│ Just different data provider                                │
└─────────────────────────────────────────────────────────────┘

Benefits:
- ONE configurator for all sources
- Data loading abstracted
- Easy to add new sources
```

---

## Entity Configuration (Nested Data)

```
┌────────────────────────────────────────────────────────────────┐
│ SceneData (from provider)                                       │
├────────────────────────────────────────────────────────────────┤
│  SceneInfo info                                                │
│  SceneResources resources                                      │
│  EntityCollection entities ◄─── This is nested data!          │
│    ├─ entity_memory_pool_size                                  │
│    └─ vector<EntityData> entities                              │
│         ├─ EntityData[0]                                        │
│         │   ├─ index                                            │
│         │   ├─ CUserInterface component_data                    │
│         │   └─ CGrimoireMachina component_data                  │
│         └─ EntityData[1]                                        │
│             └─ ...                                              │
│  LogicCollectionData logic                                     │
└────────────────────────────────────────────────────────────────┘
                                │
                                ↓ Passed to configurator
┌────────────────────────────────────────────────────────────────┐
│ DefaultSceneConfigurator::ConfigureScene()                     │
├────────────────────────────────────────────────────────────────┤
│  ConfigureEntities(scene, data.entities)                       │
│    └─> Creates FlatbuffersEntityConfigurator                   │
│        └─> Passes data.entities (EntityCollection)             │
│            └─> Configurator configures EntityMemoryPool        │
└────────────────────────────────────────────────────────────────┘

Key insight: EntityCollection is already a native C++ struct!
No need to pass FlatBuffers pointers down the chain.
```

---

## Testing Benefits

### Current (Hard to Test)

```
┌────────────────────────────────────────────────────────┐
│ TEST: Scene configuration                              │
├────────────────────────────────────────────────────────┤
│ // Need to mock FlatbuffersDataLoader ❌               │
│ MockDataLoader mock_loader;                            │
│ FlatbuffersDefaultSceneConfigurator configurator;      │
│ // How do we inject mock_loader? ❌                    │
│ // Configurator owns FlatbuffersDataLoader internally! │
└────────────────────────────────────────────────────────┘
```

### Recommended (Easy to Test)

```
┌────────────────────────────────────────────────────────┐
│ TEST: Scene configuration                              │
├────────────────────────────────────────────────────────┤
│ // Create test data directly ✅                        │
│ SceneData test_data;                                   │
│ test_data.info.type = SceneType::TITLE;               │
│ test_data.resources.render_texture_width = 800;       │
│ // ... populate test data                             │
│                                                        │
│ // Test configurator with test data ✅                │
│ DefaultSceneConfigurator configurator;                 │
│ auto result = configurator.ConfigureScene(scene,       │
│                                          test_data);   │
│ REQUIRE(result.has_value());                          │
│ REQUIRE(scene.GetSceneInfo().type == TITLE);          │
└────────────────────────────────────────────────────────┘

Or use test provider:

┌────────────────────────────────────────────────────────┐
│ TEST: Full scene creation pipeline                     │
├────────────────────────────────────────────────────────┤
│ // Use test provider ✅                                │
│ ISceneDataProvider &provider = GetTestSceneDataProvider();│
│ auto scene_data = provider.LoadSceneData(TITLE);      │
│                                                        │
│ SceneFactory factory(test_game_context);               │
│ auto scene = factory.CreateSceneByType(TITLE);        │
│                                                        │
│ DefaultSceneConfigurator configurator;                 │
│ configurator.ConfigureScene(*scene, scene_data.value());│
└────────────────────────────────────────────────────────┘
```

---

## Summary Diagram

```
┌────────────────────────────────────────────────────────────────┐
│                     GOLDEN RULE                                 │
├────────────────────────────────────────────────────────────────┤
│  Providers provide data (native C++)                           │
│  Factories create objects (empty)                              │
│  Configurators apply data to objects                           │
│                                                                │
│  Each has ONE job. Data flows cleanly.                         │
└────────────────────────────────────────────────────────────────┘

     ┌─────────────┐
     │   Provider  │ ◄─── Knows about data formats
     └──────┬──────┘      (FlatBuffers, XML, etc.)
            │ SceneData (native)
            ↓
     ┌─────────────┐
     │   Factory   │ ◄─── Knows about scene types
     └──────┬──────┘      (Title, Crafting, etc.)
            │ Scene* (empty)
            ↓
     ┌─────────────┐
     │ Configurator│ ◄─── Knows how to configure
     └─────────────┘      (generic, reusable)

Each component is independently testable and reusable!
```
