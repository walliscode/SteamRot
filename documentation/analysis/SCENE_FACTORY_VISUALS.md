# Scene Factory Architecture - Visual Diagrams

**Visual reference for Scene configuration workflow**

---

## Table of Contents

1. [System Overview](#system-overview)
2. [Component Relationships](#component-relationships)
3. [Sequence Diagrams](#sequence-diagrams)
4. [Class Diagrams](#class-diagrams)
5. [Data Flow](#data-flow)
6. [Comparison Diagrams](#comparison-diagrams)

---

## System Overview

### High-Level Architecture

```
┌────────────────────────────────────────────────────────────────┐
│                         Game Engine                            │
│                                                                │
│  ┌──────────────────────────────────────────────────────────┐ │
│  │                    SceneManager                          │ │
│  │  • Orchestrates Scene loading                           │ │
│  │  • Chooses data source (Default vs Save)                │ │
│  │  • Creates configurators                                │ │
│  │  • Manages Scene lifecycle                              │ │
│  └──────────┬──────────────────────────┬────────────────────┘ │
│             │                          │                       │
│             │ Creates                  │ Creates               │
│             ↓                          ↓                       │
│  ┌─────────────────────┐    ┌─────────────────────┐          │
│  │ Default             │    │ Saved               │          │
│  │ SceneConfigurator   │    │ SceneConfigurator   │          │
│  └──────────┬──────────┘    └──────────┬──────────┘          │
│             │                          │                       │
│             │ Passed to Factory        │                       │
│             └──────────────┬───────────┘                       │
│                            ↓                                   │
│              ┌─────────────────────────┐                       │
│              │    SceneFactory         │                       │
│              │  • Uses configurator    │                       │
│              │  • Creates Scene        │                       │
│              │  • Configures Scene     │                       │
│              └─────────────────────────┘                       │
└────────────────────────────────────────────────────────────────┘
```

### Data Layer Separation

```
┌─────────────────────────────────────────────────────────────────┐
│                        Data Sources                             │
├─────────────────────────────────────────────────────────────────┤
│  File System                                                    │
│  ├─ data/scenes/title_scene.bin      (Default scene data)      │
│  └─ saves/slot_0.save.bin            (Saved game data)         │
└─────────────┬───────────────────────────┬───────────────────────┘
              │                           │
              ↓                           ↓
┌─────────────────────────┐    ┌─────────────────────────┐
│  ISceneDataProvider     │    │  ISaveDataProvider      │
│  (Default data)         │    │  (Save data)            │
└─────────────┬───────────┘    └─────────────┬───────────┘
              │                               │
              │ Used by                       │ Used by
              ↓                               ↓
┌─────────────────────────┐    ┌─────────────────────────┐
│ DefaultScene           │    │ SavedScene              │
│ Configurator           │    │ Configurator            │
└─────────────┬───────────┘    └─────────────┬───────────┘
              │                               │
              └───────────────┬───────────────┘
                              │
                              ↓
                    ┌─────────────────────┐
                    │  ISceneConfigurator │
                    │  (Strategy)         │
                    └─────────────────────┘
                              │
                              ↓
                    ┌─────────────────────┐
                    │   SceneFactory      │
                    │   (Consumer)        │
                    └─────────────────────┘
                              │
                              ↓
                    ┌─────────────────────┐
                    │   Configured Scene  │
                    └─────────────────────┘
```

---

## Component Relationships

### Strategy Pattern Implementation

```
                    ┌──────────────────────────┐
                    │   ISceneConfigurator     │
                    │   (Strategy Interface)   │
                    ├──────────────────────────┤
                    │ + GetSceneData()         │
                    │ + CreateEntityConfig()   │
                    │ + GetSceneType()         │
                    └────────────▲─────────────┘
                                 │
                                 │ implements
                ┌────────────────┴─────────────────┐
                │                                  │
   ┌────────────────────────┐       ┌─────────────────────────┐
   │ DefaultScene           │       │ SavedScene              │
   │ Configurator           │       │ Configurator            │
   ├────────────────────────┤       ├─────────────────────────┤
   │ - scene_type           │       │ - save_slot_index       │
   │ - scene_data_provider  │       │ - save_data_provider    │
   │ - cached_scene_data    │       │ - cached_scene_data     │
   ├────────────────────────┤       ├─────────────────────────┤
   │ + GetSceneData()       │       │ + GetSceneData()        │
   │ + CreateEntityConfig() │       │ + CreateEntityConfig()  │
   │ + GetSceneType()       │       │ + GetSceneType()        │
   └────────────────────────┘       └─────────────────────────┘
                │                                  │
                │ uses                            │ uses
                ↓                                  ↓
   ┌────────────────────────┐       ┌─────────────────────────┐
   │ ISceneDataProvider     │       │ ISaveDataProvider       │
   └────────────────────────┘       └─────────────────────────┘
```

### Factory and Configurator Relationship

```
┌─────────────────────────────────────────────────────────┐
│                    ISceneFactory                        │
│                                                         │
│  ┌──────────────────────────────────────────────────┐  │
│  │  Protected Members:                              │  │
│  │  • const ISceneConfigurator &m_scene_configurator│  │
│  │  • unique_ptr<IEntityConfigurator>               │  │
│  │  • const GameContext &m_game_context             │  │
│  └──────────────────────────────────────────────────┘  │
│                                                         │
│  ┌──────────────────────────────────────────────────┐  │
│  │  Public Methods:                                 │  │
│  │  • CreateScene()                                 │  │
│  │  • ConfigureSceneInfo(Scene&)                    │  │
│  └──────────────────────────────────────────────────┘  │
│                                                         │
│  ┌──────────────────────────────────────────────────┐  │
│  │  Virtual Methods:                                │  │
│  │  • ConfigureSceneResources(Scene&)               │  │
│  │  • ConfigureSceneConfig(Scene&)                  │  │
│  └──────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
                            │
                            │ implements
                            ↓
┌─────────────────────────────────────────────────────────┐
│             FlatbuffersSceneFactory                     │
│                                                         │
│  Overrides:                                             │
│  • ConfigureSceneResources(Scene&)                      │
│    - Calls m_scene_configurator.GetSceneData()         │
│    - Uses SceneDataFbs* directly                        │
│    - Configures Scene from FlatBuffers                  │
│                                                         │
│  • ConfigureSceneConfig(Scene&)                         │
│    - Configures scene-specific settings                 │
└─────────────────────────────────────────────────────────┘
```

---

## Sequence Diagrams

### Sequence 1: Load Default Scene

```
┌───────────┐  ┌─────────┐  ┌──────────┐  ┌────────────┐  ┌──────────┐
│SceneManager│ │Provider │  │Configurator│ │SceneFactory│  │  Scene   │
└─────┬──────┘  └────┬────┘  └─────┬──────┘ └──────┬─────┘  └────┬─────┘
      │              │              │                │             │
      │ LoadScene    │              │                │             │
      │ FromDefault()│              │                │             │
      │─────┐        │              │                │             │
      │     │        │              │                │             │
      │<────┘        │              │                │             │
      │              │              │                │             │
      │ Get Provider │              │                │             │
      │─────────────>│              │                │             │
      │              │              │                │             │
      │ ISceneData   │              │                │             │
      │ Provider&    │              │                │             │
      │<─────────────│              │                │             │
      │              │              │                │             │
      │ Create Configurator         │                │             │
      │────────────────────────────>│                │             │
      │              │              │                │             │
      │ DefaultSceneConfigurator    │                │             │
      │<────────────────────────────│                │             │
      │              │              │                │             │
      │ Create Factory with Config  │                │             │
      │────────────────────────────────────────────>│             │
      │              │              │                │             │
      │ SceneFactory │              │                │             │
      │<────────────────────────────────────────────│             │
      │              │              │                │             │
      │ CreateScene()│              │                │             │
      │────────────────────────────────────────────>│             │
      │              │              │                │             │
      │              │              │ GetSceneData() │             │
      │              │              │<───────────────│             │
      │              │              │                │             │
      │              │ LoadScene    │                │             │
      │              │ Data()       │                │             │
      │              │<─────────────│                │             │
      │              │              │                │             │
      │              │ SceneDataFbs*│                │             │
      │              │─────────────>│                │             │
      │              │              │                │             │
      │              │              │ SceneDataFbs*  │             │
      │              │              │───────────────>│             │
      │              │              │                │             │
      │              │              │ CreateEntity   │             │
      │              │              │ Configurator() │             │
      │              │              │<───────────────│             │
      │              │              │                │             │
      │              │              │ IEntity        │             │
      │              │              │ Configurator*  │             │
      │              │              │───────────────>│             │
      │              │              │                │             │
      │              │              │                │ new Scene() │
      │              │              │                │────────────>│
      │              │              │                │             │
      │              │              │                │ Scene*      │
      │              │              │                │<────────────│
      │              │              │                │             │
      │              │              │  Configure     │             │
      │              │              │  Scene         │             │
      │              │              │  ──────────────────────────>│
      │              │              │                │             │
      │              │              │                │ Configured  │
      │              │              │                │<────────────│
      │              │              │                │             │
      │ unique_ptr<Scene>           │                │             │
      │<────────────────────────────────────────────│             │
      │              │              │                │             │
      │ Store Scene  │              │                │             │
      │─────┐        │              │                │             │
      │     │        │              │                │             │
      │<────┘        │              │                │             │
```

### Sequence 2: Load Saved Scene

```
┌───────────┐  ┌─────────┐  ┌──────────┐  ┌────────────┐  ┌──────────┐
│SceneManager│ │Provider │  │Configurator│ │SceneFactory│  │  Scene   │
└─────┬──────┘  └────┬────┘  └─────┬──────┘ └──────┬─────┘  └────┬─────┘
      │              │              │                │             │
      │ LoadScene    │              │                │             │
      │ FromSave()   │              │                │             │
      │─────┐        │              │                │             │
      │     │        │              │                │             │
      │<────┘        │              │                │             │
      │              │              │                │             │
      │ Get Provider │              │                │             │
      │─────────────>│              │                │             │
      │              │              │                │             │
      │ ISaveData    │              │                │             │
      │ Provider&    │              │                │             │
      │<─────────────│              │                │             │
      │              │              │                │             │
      │ Create Configurator         │                │             │
      │────────────────────────────>│                │             │
      │              │              │                │             │
      │ SavedSceneConfigurator      │                │             │
      │<────────────────────────────│                │             │
      │              │              │                │             │
      │ Create Factory with Config  │                │             │
      │────────────────────────────────────────────>│             │
      │              │              │                │             │
      │ SceneFactory │              │                │             │
      │<────────────────────────────────────────────│             │
      │              │              │                │             │
      │ CreateScene()│              │                │             │
      │────────────────────────────────────────────>│             │
      │              │              │                │             │
      │              │              │ GetSceneData() │             │
      │              │              │<───────────────│             │
      │              │              │                │             │
      │              │ LoadSave()   │                │             │
      │              │<─────────────│                │             │
      │              │              │                │             │
      │              │ SaveDataFbs* │                │             │
      │              │─────────────>│                │             │
      │              │              │                │             │
      │              │              │ Extract        │             │
      │              │              │ SceneDataFbs   │             │
      │              │              │─────┐          │             │
      │              │              │     │          │             │
      │              │              │<────┘          │             │
      │              │              │                │             │
      │              │              │ SceneDataFbs*  │             │
      │              │              │───────────────>│             │
      │              │              │                │             │
      │              │              │ CreateEntity   │             │
      │              │              │ Configurator() │             │
      │              │              │<───────────────│             │
      │              │              │                │             │
      │              │              │ IEntity        │             │
      │              │              │ Configurator*  │             │
      │              │              │───────────────>│             │
      │              │              │                │             │
      │              │              │                │ new Scene() │
      │              │              │                │────────────>│
      │              │              │                │             │
      │              │              │                │ Scene*      │
      │              │              │                │<────────────│
      │              │              │                │             │
      │              │              │  Configure     │             │
      │              │              │  Scene         │             │
      │              │              │  ──────────────────────────>│
      │              │              │                │             │
      │              │              │                │ Configured  │
      │              │              │                │<────────────│
      │              │              │                │             │
      │ unique_ptr<Scene>           │                │             │
      │<────────────────────────────────────────────│             │
      │              │              │                │             │
      │ Store Scene  │              │                │             │
      │─────┐        │              │                │             │
      │     │        │              │                │             │
      │<────┘        │              │                │             │
```

---

## Class Diagrams

### Complete Class Hierarchy

```
┌───────────────────────┐
│  ISceneConfigurator   │
│  (Interface)          │
├───────────────────────┤
│ + GetSceneData()      │
│ + CreateEntity        │
│   Configurator()      │
│ + GetSceneType()      │
└──────────▲────────────┘
           │
           │ implements
           │
    ┌──────┴──────┐
    │             │
┌───────────────────────┐  ┌───────────────────────┐
│ DefaultScene          │  │ SavedScene            │
│ Configurator          │  │ Configurator          │
├───────────────────────┤  ├───────────────────────┤
│ - m_scene_type        │  │ - m_save_slot_index   │
│ - m_scene_data_       │  │ - m_save_data_        │
│   provider            │  │   provider            │
│ - m_cached_scene_data │  │ - m_cached_scene_data │
├───────────────────────┤  ├───────────────────────┤
│ + GetSceneData()      │  │ + GetSceneData()      │
│ + CreateEntity        │  │ + CreateEntity        │
│   Configurator()      │  │   Configurator()      │
│ + GetSceneType()      │  │ + GetSceneType()      │
└───────────────────────┘  └───────────────────────┘
           │                           │
           │ uses                      │ uses
           │                           │
           ↓                           ↓
┌───────────────────────┐  ┌───────────────────────┐
│ ISceneDataProvider    │  │ ISaveDataProvider     │
├───────────────────────┤  ├───────────────────────┤
│ + LoadSceneData()     │  │ + LoadSave()          │
└───────────────────────┘  │ + GetSaveSlots()      │
                           │ + SaveGame()          │
                           │ + DeleteSave()        │
                           └───────────────────────┘
```

### SceneFactory Class Hierarchy

```
┌────────────────────────────────────────┐
│         ISceneFactory                  │
│         (Abstract)                     │
├────────────────────────────────────────┤
│ # m_scene_configurator                 │
│ # m_entity_configurator                │
│ # m_game_context                       │
│ # scene_type                           │
├────────────────────────────────────────┤
│ + CreateScene()                        │
│ + ConfigureSceneInfo()                 │
│ # CreateSceneByType()                  │
│ + ConfigureLogicMap()                  │
│                                        │
│ [virtual]                              │
│ + ConfigureSceneResources()            │
│ + ConfigureSceneConfig()               │
└────────────────────────────────────────┘
                    ▲
                    │ inherits
                    │
┌────────────────────────────────────────┐
│    FlatbuffersSceneFactory             │
├────────────────────────────────────────┤
│ [no additional members]                │
├────────────────────────────────────────┤
│ + ConfigureSceneResources() override   │
│ + ConfigureSceneConfig() override      │
│                                        │
│ Implementation uses:                   │
│ • m_scene_configurator.GetSceneData()  │
│ • m_entity_configurator methods        │
└────────────────────────────────────────┘
```

---

## Data Flow

### FlatBuffers Data Flow

```
┌──────────────────────────────────────────────────────────┐
│                   File System                            │
├──────────────────────────────────────────────────────────┤
│  data/scenes/title_scene.bin                             │
│  ┌────────────────────────────────────────────────────┐  │
│  │ SceneDataFbs {                                     │  │
│  │   scene_info: { scene_id, scene_type }            │  │
│  │   scene_resources: { render_texture_width, ... }   │  │
│  │   entity_collection: { entities[], pool_size }     │  │
│  │   logic_collection_data: { ... }                   │  │
│  │   assets: { textures[], fonts[], ... }             │  │
│  │ }                                                   │  │
│  └────────────────────────────────────────────────────┘  │
└────────────────────┬─────────────────────────────────────┘
                     │
                     ↓ Loaded by
┌────────────────────────────────────────────────────────┐
│            ISceneDataProvider                          │
│  • LoadSceneData(SceneType)                           │
│  • Returns FlatBuffers data pointer                    │
└────────────────────┬───────────────────────────────────┘
                     │
                     ↓ Used by
┌────────────────────────────────────────────────────────┐
│         DefaultSceneConfigurator                       │
│  • Caches SceneDataFbs*                               │
│  • Provides to Factory on demand                       │
└────────────────────┬───────────────────────────────────┘
                     │
                     ↓ GetSceneData()
┌────────────────────────────────────────────────────────┐
│            SceneFactory                                │
│  • Receives const SceneDataFbs*                       │
│  • Extracts data for configuration                     │
│  • No ownership of FlatBuffers data                    │
└────────────────────┬───────────────────────────────────┘
                     │
                     ↓ Configures
┌────────────────────────────────────────────────────────┐
│                Scene Object                            │
│  • m_scene_info populated                             │
│  • m_scene_resources populated                         │
│  • m_scene_config populated                            │
│  • Entities configured via EntityConfigurator          │
└────────────────────────────────────────────────────────┘
```

### SaveData to SceneData Extraction

```
┌──────────────────────────────────────────────────────────┐
│              saves/slot_0.save.bin                       │
├──────────────────────────────────────────────────────────┤
│  SaveDataFbs {                                           │
│    metadata: { save_name, created_at, ... }             │
│    current_scene_type: TITLE                             │
│    version: 1                                            │
│    ┌──────────────────────────────────────────────────┐ │
│    │ scene_data: SceneDataFbs {                       │ │
│    │   scene_info: { ... }                            │ │
│    │   scene_resources: { ... }                        │ │
│    │   entity_collection: { ... }                      │ │
│    │   logic_collection_data: { ... }                  │ │
│    │   assets: { ... }                                 │ │
│    │ }                                                 │ │
│    └──────────────────────────────────────────────────┘ │
│  }                                                       │
└────────────────────┬─────────────────────────────────────┘
                     │
                     ↓ Loaded by
┌────────────────────────────────────────────────────────┐
│            ISaveDataProvider                           │
│  • LoadSave(slot_index)                               │
│  • Returns SaveDataFbs*                                │
└────────────────────┬───────────────────────────────────┘
                     │
                     ↓ Used by
┌────────────────────────────────────────────────────────┐
│         SavedSceneConfigurator                         │
│  • Extracts scene_data field                          │
│  • Caches SceneDataFbs*                               │
│  • Provides to Factory on demand                       │
└────────────────────┬───────────────────────────────────┘
                     │
                     ↓ GetSceneData()
┌────────────────────────────────────────────────────────┐
│            SceneFactory                                │
│  • Receives const SceneDataFbs*                       │
│  • Same flow as default scene                          │
│  • No knowledge of SaveDataFbs                         │
└────────────────────┬───────────────────────────────────┘
                     │
                     ↓ Configures
┌────────────────────────────────────────────────────────┐
│           Configured Scene                             │
└────────────────────────────────────────────────────────┘
```

---

## Comparison Diagrams

### Before vs After: Factory Coupling

#### Before (❌ Coupled)

```
┌─────────────────────────────────────────┐
│     FlatbuffersSceneFactory             │
├─────────────────────────────────────────┤
│ - const SceneDataFbs *m_scene_data_fbs  │  ❌ Direct coupling
├─────────────────────────────────────────┤
│ + FlatbuffersSceneFactory(              │
│     GameContext&,                        │
│     SceneDataFbs*  ← FlatBuffers type!  │  ❌ Exposed to caller
│   )                                      │
└─────────────────────────────────────────┘
```

#### After (✅ Decoupled)

```
┌─────────────────────────────────────────┐
│     FlatbuffersSceneFactory             │
├─────────────────────────────────────────┤
│ [inherits m_scene_configurator]         │  ✅ Strategy pattern
├─────────────────────────────────────────┤
│ + FlatbuffersSceneFactory(              │
│     GameContext&,                        │
│     ISceneConfigurator&  ← Interface!   │  ✅ Abstraction
│   )                                      │
└─────────────────────────────────────────┘
```

### Before vs After: SceneManager

#### Before (❌ Coupled)

```
SceneManager::AddSceneFromDefault(SceneType type) {
  // Load FlatBuffers data directly
  auto scene_data_fbs = LoadSceneDataFbs(type);  ❌ Exposed
  
  // Create factory with FlatBuffers
  FlatbuffersSceneFactory factory(
      m_game_context, 
      scene_data_fbs);  ❌ Direct coupling
  
  auto scene = factory.CreateScene();
  // ...
}
```

#### After (✅ Decoupled)

```
SceneManager::LoadSceneFromDefault(SceneType type) {
  // Get provider (abstraction)
  ISceneDataProvider &provider = GetSceneDataProvider();  ✅
  
  // Create configurator (strategy)
  DefaultSceneConfigurator configurator(type, provider);  ✅
  
  // Create factory with configurator
  FlatbuffersSceneFactory factory(
      m_game_context,
      configurator);  ✅ Strategy pattern
  
  auto scene = factory.CreateScene();
  // ...
}
```

### Comparison: Overloaded vs Virtual Methods

#### ❌ DON'T: Overloaded Methods (Won't Work)

```cpp
class IEntityConfigurator {
public:
  // These are OVERLOADS, not overrides
  std::expected<std::monostate, FailInfo>
  ConfigureFromDefault(const SceneDataFbs *data);  ❌
  
  std::expected<std::monostate, FailInfo>
  ConfigureFromSave(const SaveDataFbs *data);  ❌
  
  // Problem: Overload resolution happens at compile-time
  // Polymorphism doesn't work with overloads!
};

// This won't work as expected:
IEntityConfigurator *config = GetConfigurator();
const SceneDataFbs *data = ...;
config->ConfigureFromDefault(data);  // ❌ Won't dispatch correctly
```

#### ✅ DO: Virtual Methods (Correct)

```cpp
class IEntityConfigurator {
public:
  // These are VIRTUAL methods (true polymorphism)
  virtual std::expected<std::monostate, FailInfo>
  ConfigureFromDefault(EntityMemoryPool &emp) = 0;  ✅
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureFromSave(EntityMemoryPool &emp) = 0;  ✅
  
  // Problem solved: Virtual methods dispatch at runtime
};

// This works correctly:
IEntityConfigurator *config = GetConfigurator();
config->ConfigureFromDefault(emp);  // ✅ Calls correct implementation
```

**OR BETTER: Strategy Pattern (No Method Duplication)**

```cpp
class IEntityConfigurator {
protected:
  const EntityCollectionFbs &m_entity_data;  // From configurator

public:
  // Single configuration method
  virtual std::expected<std::monostate, FailInfo>
  ConfigureEntityMemoryPool(EntityMemoryPool &emp) = 0;  ✅
  
  // Data source determined by which configurator created this
  // No need for separate methods!
};
```

---

## Entity Configurator Creation

### Configurator Creates Matching Entity Configurator

```
┌─────────────────────────────────────────────────────────┐
│        DefaultSceneConfigurator                         │
├─────────────────────────────────────────────────────────┤
│ CreateEntityConfigurator(EventHandler &handler) {       │
│   const SceneDataFbs *scene_data = GetSceneData();     │
│   const EntityCollectionFbs *entities =                 │
│       scene_data->entity_collection();                  │
│                                                         │
│   return std::make_unique<FlatbuffersEntityConfigurator>(│
│       handler, *entities);                              │
│ }                                                       │
└─────────────────────────────────────────────────────────┘
                        │
                        ↓ Creates
┌─────────────────────────────────────────────────────────┐
│     FlatbuffersEntityConfigurator                       │
│  • References entity_collection from scene_data         │
│  • Configures entities from FlatBuffers                 │
└─────────────────────────────────────────────────────────┘


┌─────────────────────────────────────────────────────────┐
│        SavedSceneConfigurator                           │
├─────────────────────────────────────────────────────────┤
│ CreateEntityConfigurator(EventHandler &handler) {       │
│   const SceneDataFbs *scene_data = GetSceneData();     │
│   const EntityCollectionFbs *entities =                 │
│       scene_data->entity_collection();                  │
│                                                         │
│   return std::make_unique<FlatbuffersEntityConfigurator>(│
│       handler, *entities);                              │
│ }                                                       │
└─────────────────────────────────────────────────────────┘
                        │
                        ↓ Creates
┌─────────────────────────────────────────────────────────┐
│     FlatbuffersEntityConfigurator                       │
│  • Same class, but entities from SaveDataFbs           │
│  • Transparent to Factory                               │
└─────────────────────────────────────────────────────────┘
```

### Key Insight

Both configurators create the **same type** of entity configurator (`FlatbuffersEntityConfigurator`), but with different entity data sources. The factory doesn't need to know which!

---

## Complete Workflow Comparison

### Current Workflow (❌)

```
SceneManager
    ↓
LoadSceneDataFbs() ────────────┐  ❌ Exposes FlatBuffers
    ↓                          │
SceneDataFbs* ─────────────────┤  ❌ Coupled
    ↓                          │
FlatbuffersSceneFactory ───────┘  ❌ Knows about FlatBuffers
    ↓
Configured Scene
```

### Proposed Workflow (✅)

```
SceneManager
    ↓
Get ISceneDataProvider ────────┐  ✅ Abstraction
    ↓                          │
Create Configurator ───────────┤  ✅ Strategy
    ↓                          │
Pass to Factory ───────────────┘  ✅ Decoupled
    ↓
Factory.GetSceneData() ────────┐  ✅ On-demand access
    ↓                          │
SceneDataFbs* (internal) ──────┘  ✅ Encapsulated
    ↓
Configured Scene
```

---

## Summary

### Key Visual Takeaways

1. **Strategy Pattern**: Configurators encapsulate data access
2. **No Direct Coupling**: Factory never stores FlatBuffers pointers
3. **Unified Flow**: Same factory code for both default and save data
4. **Clear Responsibilities**: Each component has single, clear purpose
5. **Type Safety**: Strong interfaces enforce correct usage

### Visual Checklist

- ✅ Configurator wraps data provider
- ✅ Factory receives configurator by reference
- ✅ Factory calls GetSceneData() on-demand
- ✅ No FlatBuffers types in Factory members
- ✅ Entity configurator created by scene configurator
- ✅ Same Scene configuration code for all sources

---

## Related Documents

- [Full Analysis](SCENE_FACTORY_AND_CONFIGURATOR_ANALYSIS.md)
- [Quick Reference](SCENE_FACTORY_QUICK_REF.md)
