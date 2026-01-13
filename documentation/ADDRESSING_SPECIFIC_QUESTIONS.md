# Addressing Specific Questions from Problem Statement

This document directly addresses each question and concern raised in the problem statement about native object creation and configuration.

## Problem Statement Questions

### Q1: "With different data types the concrete implementations need to accept the data type (I do not want type erasure). How is this done with some kind of data type switching factory?"

**Answer**: Use **template-based interfaces** combined with **template specialization** for the factory registry.

#### Key Insight
You don't need runtime type switching at all. The data type is known at compile time, so use templates to dispatch to the correct implementation.

#### Solution Pattern

```cpp
// Generic interface - templated on data type
template<typename DataType, typename ObjectType>
class IConfigurator {
public:
  virtual std::expected<std::monostate, FailInfo>
  Configure(ObjectType& object, const DataType& data) = 0;
};

// Concrete implementation knows its data type at compile time
class FlatbuffersSceneConfigurator 
    : public IConfigurator<SceneDataFbs, Scene> {
    // ^^^^^^^^^^^^^^^^^^^
    // No type erasure - SceneDataFbs is explicit
public:
  std::expected<std::monostate, FailInfo>
  Configure(Scene& scene, const SceneDataFbs& data) override {
    // Full access to SceneDataFbs members, no casting needed
    if (data.scene_info()) {
      // Type-safe access to FlatBuffers data
    }
    return std::monostate{};
  }
};
```

#### Factory Without Switch Statements

The factory uses **template specialization** instead of switch statements:

```cpp
// Registry - only place that knows concrete types
template<DataSourceType SourceType, typename DataType, typename ObjectType>
struct ConfiguratorRegistry;  // No default implementation

// Specialization for FlatBuffers + Scene
template<>
struct ConfiguratorRegistry<DataSourceType::Flatbuffers, 
                           SceneDataFbs, Scene> {
  static std::unique_ptr<IConfigurator<SceneDataFbs, Scene>> 
  Create(EventHandler& eh) {
    return std::make_unique<FlatbuffersSceneConfigurator>(eh);
  }
};

// Factory uses template to get right implementation
template<DataSourceType SourceType>
class ConfiguratorFactory {
public:
  template<typename DataType, typename ObjectType>
  std::expected<IConfigurator<DataType, ObjectType>*, FailInfo>
  GetConfigurator() {
    // Calls the appropriate specialization at compile time
    auto instance = ConfiguratorRegistry<SourceType, DataType, ObjectType>
        ::Create(m_event_handler);
    // No switch, no runtime dispatch, no type erasure
    return instance.get();
  }
};
```

#### Usage Example

```cpp
// User code
ConfiguratorFactory<DataSourceType::Flatbuffers> factory(event_handler);

// Template parameters specify exact types - no type erasure
auto configurator = factory.GetConfigurator<SceneDataFbs, Scene>();
//                                          ^^^^^^^^^^^^  ^^^^^
//                                          Data type     Object type

// configurator has exact type: IConfigurator<SceneDataFbs, Scene>*
// Configure method receives SceneDataFbs, not void* or std::any
configurator.value()->Configure(scene, flatbuffers_scene_data);
```

**Result**: No type erasure, no switch statements, full compile-time type safety.

---

### Q2: "Do we say any top level objects (those with their type in data files) need to be able to source its data itself via the DataLoader?"

**Answer**: **Yes**, but with separation of concerns. Top-level objects have paired DataProviders, but the object itself doesn't do the loading.

#### Pattern: Paired Provider + Configurator

Each top-level data type (Scene, Entity, UIStyle) gets:
1. **Data Provider Interface** - loads/provides the data
2. **Configurator Interface** - configures objects from data
3. **Factory** - creates both based on data source type

```cpp
// 1. Data Provider for loading
template<typename DataType>
class IDataProvider {
public:
  virtual std::expected<DataType, FailInfo> LoadData() = 0;
};

// 2. Configurator for configuration
template<typename DataType, typename ObjectType>
class IConfigurator {
public:
  virtual std::expected<std::monostate, FailInfo>
  Configure(ObjectType& object, const DataType& data) = 0;
};

// 3. Registry knows both
template<DataSourceType SourceType, typename DataType, typename ObjectType>
struct Registry {
  static std::unique_ptr<IDataProvider<DataType>> CreateProvider();
  static std::unique_ptr<IConfigurator<DataType, ObjectType>> CreateConfigurator();
};
```

#### Example: Scene as Top-Level Object

```cpp
// Scene data provider - knows how to load scene data
template<>
struct Registry<DataSourceType::Flatbuffers, SceneDataFbs, Scene> {
  
  // Provider knows how to load FlatBuffers scene data
  static std::unique_ptr<IDataProvider<SceneDataFbs>> CreateProvider() {
    return std::make_unique<FlatbuffersSceneDataProvider>();
  }
  
  // Configurator knows how to configure Scene from SceneDataFbs
  static std::unique_ptr<IConfigurator<SceneDataFbs, Scene>> 
  CreateConfigurator(EventHandler& eh) {
    return std::make_unique<FlatbuffersSceneConfigurator>(eh);
  }
};
```

#### Usage Pattern

```cpp
// Get provider for scene data
auto provider = factory.GetDataProvider<SceneDataFbs>();
auto scene_data = provider->LoadData();

// Get configurator for scene
auto configurator = factory.GetConfigurator<SceneDataFbs, Scene>();
Scene scene;
configurator->Configure(scene, scene_data.value());
```

**Key Points**:
- Top-level objects CAN source their own data (through paired provider)
- Loading (Provider) and Configuration (Configurator) are separate
- Factory manages both, keeps them paired
- Objects themselves don't know about data sources

#### Where Coupling Happens

Coupling is confined to the **Registry specialization**:

```cpp
template<>
struct Registry<DataSourceType::Flatbuffers, SceneDataFbs, Scene> {
  // THIS is the only place that knows:
  // - SceneDataFbs is the FlatBuffers scene data type
  // - Scene is the object being configured
  // - FlatbuffersSceneDataProvider is the concrete provider
  // - FlatbuffersSceneConfigurator is the concrete configurator
  
  static auto CreateProvider() {
    return std::make_unique<FlatbuffersSceneDataProvider>();
  }
  
  static auto CreateConfigurator(EventHandler& eh) {
    return std::make_unique<FlatbuffersSceneConfigurator>(eh);
  }
};
```

**Everywhere else** uses interfaces:
```cpp
// User code - no coupling to FlatBuffers
IDataProvider<SceneDataFbs>* provider = ...;
IConfigurator<SceneDataFbs, Scene>* configurator = ...;
```

---

### Q3: "If object A contains object B and object B has an interface, how does interface for A call the interface for B?"

**Answer**: **Composition** - A's configurator receives a reference to the factory and requests B's configurator.

#### Pattern: Configurator Composition

```cpp
// Object A's configurator takes factory as dependency
class ConfiguratorForA : public IConfigurator<DataForA, ObjectA> {
private:
  EventHandler& m_event_handler;
  ConfiguratorFactory<DataSourceType>& m_factory;

public:
  ConfiguratorForA(EventHandler& eh, 
                  ConfiguratorFactory<DataSourceType>& factory)
      : m_event_handler(eh), m_factory(factory) {}
  
  std::expected<std::monostate, FailInfo>
  Configure(ObjectA& objA, const DataForA& data) override {
    
    // Configure A's own properties
    objA.SetProperty(data.property());
    
    // Object A contains Object B - get B's configurator
    auto configB_result = m_factory.GetConfigurator<DataForB, ObjectB>();
    if (!configB_result) 
      return std::unexpected(configB_result.error());
    
    auto* configB = configB_result.value();
    
    // Configure B using its configurator
    auto result = configB->Configure(objA.GetObjectB(), data.b_data());
    if (!result)
      return std::unexpected(result.error());
    
    return std::monostate{};
  }
};
```

#### Concrete Example: Scene Contains Entities

```cpp
class FlatbuffersSceneConfigurator 
    : public IConfigurator<SceneDataFbs, Scene> {
    
private:
  EventHandler& m_event_handler;
  ConfiguratorFactory<DataSourceType::Flatbuffers>& m_factory;

public:
  FlatbuffersSceneConfigurator(
      EventHandler& event_handler,
      ConfiguratorFactory<DataSourceType::Flatbuffers>& factory)
      : m_event_handler(event_handler), m_factory(factory) {}
  
  std::expected<std::monostate, FailInfo>
  Configure(Scene& scene, const SceneDataFbs& data) override {
    
    // 1. Configure Scene's own properties
    ConfigureSceneInfo(scene.GetSceneInfo(), data.scene_info());
    
    // 2. Scene contains EntityMemoryPool
    // Get entity configurator from factory
    if (data.entity_collection()) {
      auto entity_config = m_factory.GetConfigurator<
          EntityCollectionFbs, EntityMemoryPool>();
      
      if (!entity_config)
        return std::unexpected(entity_config.error());
      
      // 3. Use entity configurator to configure entities
      auto result = entity_config.value()->Configure(
          scene.GetEntityMemoryPool(),
          *data.entity_collection());
      
      if (!result)
        return std::unexpected(result.error());
    }
    
    // 4. Scene might also contain UIStyles
    if (data.ui_styles()) {
      auto ui_style_config = m_factory.GetConfigurator<
          UIStyleDataFbs, UIStyleCollection>();
      
      // Configure UI styles using their configurator
      ui_style_config.value()->Configure(
          scene.GetUIStyles(),
          *data.ui_styles());
    }
    
    return std::monostate{};
  }
};
```

#### Benefits of This Approach

1. **Code Reuse**: Entity configurator can be used by Scene, Level, SaveFile, etc.
2. **Single Responsibility**: Each configurator handles one object type
3. **Testability**: Can mock factory in tests to inject test configurators
4. **No Duplication**: Entity configuration logic exists in one place
5. **Type Safety**: Factory ensures correct configurator type

#### Alternative: Free Functions for Simple Cases

If B doesn't have complex configuration logic, use a free function instead:

```cpp
// Free function for simple nested types
std::expected<std::monostate, FailInfo>
ConfigureSceneInfo(SceneInfo& info, const SceneInfoFbs* data) {
  if (!data) return std::unexpected(...);
  info.type = data->type();
  return std::monostate{};
}

// Configurator calls free function
class FlatbuffersSceneConfigurator {
  std::expected<std::monostate, FailInfo>
  Configure(Scene& scene, const SceneDataFbs& data) override {
    // Call free function for simple nested type
    auto result = ConfigureSceneInfo(
        scene.GetSceneInfo(), 
        data.scene_info());
    return result;
  }
};
```

**When to use each**:
- **Free function**: Simple nested types (SceneInfo, SceneState)
- **Configurator interface**: Complex nested types that might be reused (EntityMemoryPool, UIStyles)

---

### Q4: "Objects that are shared amongst other objects need to have their own interface to promote code reusability"

**Answer**: **Yes, absolutely**. Shared objects get their own configurator interface.

#### Pattern: Shared Object Configurator

```cpp
// EntityMemoryPool is shared by Scene, SaveFile, Level
// It gets its own configurator interface

template<>
struct ConfiguratorRegistry<DataSourceType::Flatbuffers,
                           EntityCollectionFbs,
                           EntityMemoryPool> {
  static std::unique_ptr<IConfigurator<EntityCollectionFbs, EntityMemoryPool>>
  Create(EventHandler& eh) {
    return std::make_unique<FlatbuffersEntityConfigurator>(eh);
  }
};

// Now Scene, SaveFile, and Level can all reuse it
class FlatbuffersSceneConfigurator {
  std::expected<std::monostate, FailInfo>
  Configure(Scene& scene, const SceneDataFbs& data) override {
    // Get shared entity configurator
    auto entity_config = m_factory.GetConfigurator<
        EntityCollectionFbs, EntityMemoryPool>();
    
    // Use it
    entity_config.value()->Configure(
        scene.GetEntityMemoryPool(),
        *data.entity_collection());
  }
};

class FlatbuffersSaveFileConfigurator {
  std::expected<std::monostate, FailInfo>
  Configure(SaveFile& save, const SaveFileDataFbs& data) override {
    // Same entity configurator reused
    auto entity_config = m_factory.GetConfigurator<
        EntityCollectionFbs, EntityMemoryPool>();
    
    entity_config.value()->Configure(
        save.GetEntityMemoryPool(),
        *data.entity_collection());
  }
};
```

#### Example: UIStyles Shared Across Scenes

```cpp
// UIStyles configurator - shared by all scenes
template<>
struct ConfiguratorRegistry<DataSourceType::Flatbuffers,
                           UIStylesDataFbs,
                           UIStyleCollection> {
  static std::unique_ptr<IConfigurator<UIStylesDataFbs, UIStyleCollection>>
  Create(EventHandler& eh) {
    return std::make_unique<FlatbuffersUIStylesConfigurator>(eh);
  }
};

// Title scene uses it
class FlatbuffersTitleSceneConfigurator {
  std::expected<std::monostate, FailInfo>
  Configure(Scene& scene, const SceneDataFbs& data) override {
    auto ui_config = m_factory.GetConfigurator<
        UIStylesDataFbs, UIStyleCollection>();
    ui_config.value()->Configure(scene.GetUIStyles(), *data.ui_styles());
  }
};

// Game scene uses same configurator
class FlatbuffersGameSceneConfigurator {
  std::expected<std::monostate, FailInfo>
  Configure(Scene& scene, const SceneDataFbs& data) override {
    auto ui_config = m_factory.GetConfigurator<
        UIStylesDataFbs, UIStyleCollection>();
    ui_config.value()->Configure(scene.GetUIStyles(), *data.ui_styles());
  }
};
```

---

## Summary of Solutions

| Question | Solution | Key Technique |
|----------|----------|---------------|
| How to avoid type erasure? | Template-based interfaces | `IConfigurator<DataType, ObjectType>` |
| How to avoid switch statements? | Template specialization | `ConfiguratorRegistry<Source, Data, Object>` |
| Should top-level objects source data? | Yes, via paired provider | `IDataProvider<DataType>` |
| Where is coupling allowed? | Registry specializations only | Template specializations |
| How does A call B's interface? | Composition via factory | Pass factory to configurators |
| Should shared objects have interfaces? | Yes, for reusability | One configurator, many users |

## Complete Example: Putting It All Together

```cpp
// 1. Define interfaces
template<typename DataType, typename ObjectType>
class IConfigurator { /* ... */ };

template<typename DataType>
class IDataProvider { /* ... */ };

// 2. Define registry
template<DataSourceType Source, typename Data, typename Object>
struct ConfiguratorRegistry;  // No default

// 3. Specialize for Scene (top-level object)
template<>
struct ConfiguratorRegistry<DataSourceType::Flatbuffers, 
                           SceneDataFbs, Scene> {
  static auto CreateProvider() {
    return std::make_unique<FlatbuffersSceneDataProvider>();
  }
  static auto CreateConfigurator(EventHandler& eh, Factory& factory) {
    return std::make_unique<FlatbuffersSceneConfigurator>(eh, factory);
  }
};

// 4. Specialize for EntityMemoryPool (shared object)
template<>
struct ConfiguratorRegistry<DataSourceType::Flatbuffers,
                           EntityCollectionFbs,
                           EntityMemoryPool> {
  static auto CreateConfigurator(EventHandler& eh, Factory& factory) {
    return std::make_unique<FlatbuffersEntityConfigurator>(eh, factory);
  }
};

// 5. Use it
ConfiguratorFactory<DataSourceType::Flatbuffers> factory(event_handler);

// Load scene data
auto provider = factory.GetDataProvider<SceneDataFbs>();
auto scene_data = provider->LoadData();

// Configure scene (which internally configures entities)
auto configurator = factory.GetConfigurator<SceneDataFbs, Scene>();
Scene scene;
configurator->Configure(scene, scene_data.value());
```

**Result**:
- ✅ No type erasure (full compile-time types)
- ✅ No switch statements (template specialization)
- ✅ Top-level objects can source data (paired providers)
- ✅ Coupling confined to registry
- ✅ Composition for nested objects (factory-based)
- ✅ Shared objects have interfaces (reusable)

## Migration Strategy

To adopt this pattern incrementally:

1. **Phase 1**: Create template infrastructure alongside existing code
2. **Phase 2**: Migrate one top-level object (e.g., Scene)
3. **Phase 3**: Migrate shared objects (e.g., EntityMemoryPool)
4. **Phase 4**: Migrate remaining objects
5. **Phase 5**: Remove old switch-based factory

Each phase can be tested independently before moving to the next.
