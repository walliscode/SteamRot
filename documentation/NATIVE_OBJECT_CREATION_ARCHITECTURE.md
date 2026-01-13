# Native Object Creation and Configuration Architecture

## Executive Summary

This document analyzes the current object creation/configuration architecture in SteamRot and proposes a generalized pattern that:
- Eliminates tight data type coupling
- Allows easy compile-time addition of new data types
- Uses interfaces for standalone data types
- Promotes code reusability through composition
- Maintains type safety without type erasure

## Current Architecture Analysis

### Existing Patterns

The codebase currently uses several patterns for object creation and configuration:

#### 1. Interface + Concrete Implementation Pattern
- **Interfaces**: `ISceneConfigurator`, `IEntityConfigurator`, `IUIElementConfigurator`, `ISceneDataProvider`
- **Concrete Implementations**: `FlatbuffersSceneConfigurator`, `FlatbuffersEntityConfigurator`, etc.
- **Factory**: `DataAccessFactory` switches between implementations based on `DataType` enum

#### 2. Data Provider Pattern
- Interfaces provide data from external sources (files, memory)
- Separate concerns: data loading vs. object configuration
- Example: `ISceneDataProvider` → `FlatbuffersSceneDataProvider`

#### 3. Current Flow
```
DataType Enum → DataAccessFactory → Concrete Implementation
                                   ↓
                            Data Specific Type (e.g., SceneDataFbs*)
                                   ↓
                            Agnostic Type (e.g., SceneData)
```

### Current Coupling Points

1. **DataAccessFactory**: Hard-coded switch statement for each data type
2. **Concrete Implementations**: Tightly coupled to FlatBuffers types
3. **Interface Methods**: Some take agnostic types (SceneData), others take concrete types (const EntityCollectionFbs&)
4. **Data Loading**: Each provider knows its specific file format

### Current Strengths

1. ✅ Clear separation of concerns (loading vs. configuring)
2. ✅ Strategy pattern allows runtime switching (though only Flatbuffers implemented)
3. ✅ Type-safe concrete implementations
4. ✅ Consistent error handling with `std::expected`

### Current Weaknesses

1. ❌ Adding new data type requires modifying DataAccessFactory
2. ❌ Mixed abstraction levels (some interfaces use concrete types)
3. ❌ Configuration logic mixed with interface implementations
4. ❌ Not all standalone data types have interfaces

## Proposed Generalized Architecture

### Core Principles

1. **Standalone Data Types Get Interfaces**: Any data type that can exist independently (SceneData, UIStyle, EntityCollection) gets an interface
2. **Nested Data Types Use Free Functions**: Types that only exist within others (SceneInfo, SceneState) use free configuration functions
3. **Stateless Configuration**: All configuration functions are stateless and receive everything they need as parameters
4. **Composition Over Inheritance**: Configurators for composite objects call configurators for their components
5. **Compile-Time Extensibility**: New data types added at compile time via template mechanisms

### Proposed Pattern: Template-Based Factory

Instead of switch statements, use template specialization for type-safe dispatch:

```cpp
// Generic interface for object creation and configuration
template<typename DataType, typename ObjectType>
class IObjectConfigurator {
public:
  virtual ~IObjectConfigurator() = default;
  
  // Create new object from data
  virtual std::expected<ObjectType, FailInfo> 
  CreateObject(const DataType& data) = 0;
  
  // Configure existing object from data
  virtual std::expected<std::monostate, FailInfo>
  ConfigureObject(ObjectType& object, const DataType& data) = 0;
};

// Generic data provider interface
template<typename DataType>
class IDataProvider {
public:
  virtual ~IDataProvider() = default;
  
  // Load data from source
  virtual std::expected<DataType, FailInfo> 
  LoadData() = 0;
};

// Registry for concrete implementations
template<DataSourceType SourceType, typename DataType, typename ObjectType>
struct ConfiguratorRegistry {
  using ConfiguratorType = IObjectConfigurator<DataType, ObjectType>;
  
  static std::unique_ptr<ConfiguratorType> Create(/* dependencies */);
};

// Specializations for specific data sources
template<>
struct ConfiguratorRegistry<DataSourceType::Flatbuffers, SceneDataFbs, Scene> {
  using ConfiguratorType = IObjectConfigurator<SceneDataFbs, Scene>;
  
  static std::unique_ptr<ConfiguratorType> Create(EventHandler& eh) {
    return std::make_unique<FlatbuffersSceneConfigurator>(eh);
  }
};
```

### Example: Scene Configuration

```cpp
// 1. Generic interface for any scene data type
template<typename SceneDataType>
class ISceneConfigurator {
public:
  virtual ~ISceneConfigurator() = default;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene& scene, const SceneDataType& scene_data) = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(Scene& scene, const SceneDataType& scene_data) = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene& scene, const SceneDataType& scene_data) = 0;
};

// 2. Concrete implementation for FlatBuffers
class FlatbuffersSceneConfigurator 
    : public ISceneConfigurator<SceneDataFbs> {
public:
  std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene& scene, const SceneDataFbs& scene_data) override {
    // Use free functions for nested types
    auto info_result = ConfigureSceneInfoFromFlatbuffers(
        scene.GetSceneInfo(), scene_data.scene_info());
    if (!info_result) return std::unexpected(info_result.error());
    
    // Call nested configurator if SceneResources has its own interface
    auto resources_result = ConfigureSceneResourcesFromFlatbuffers(
        scene.GetResources(), scene_data.resources());
    if (!resources_result) return std::unexpected(resources_result.error());
    
    return std::monostate{};
  }
  
  // ... other methods
};

// 3. Free functions for non-standalone types
std::expected<std::monostate, FailInfo>
ConfigureSceneInfoFromFlatbuffers(SceneInfo& info, 
                                  const SceneInfoFbs* fb_info) {
  if (!fb_info) return std::unexpected(FailInfo{...});
  
  info.id = /* convert uuid */;
  info.type = fb_info->type();
  
  return std::monostate{};
}
```

### Proposed Factory Pattern

Replace switch-based DataAccessFactory with template-based registry:

```cpp
// Data source type enum
enum class DataSourceType {
  Flatbuffers,
  JSON,  // Future
  Binary // Future
};

// Generic factory that uses template specialization
template<DataSourceType SourceType>
class DataAccessFactory {
private:
  EventHandler& m_event_handler;
  
  // Cache instances
  template<typename InterfaceType>
  std::unordered_map<std::type_index, std::unique_ptr<void>> m_instances;

public:
  DataAccessFactory(EventHandler& event_handler) 
      : m_event_handler(event_handler) {}
  
  // Generic getter with automatic instantiation
  template<typename DataType, typename ObjectType>
  std::expected<IObjectConfigurator<DataType, ObjectType>*, FailInfo>
  GetConfigurator() {
    auto type_id = std::type_index(
        typeid(IObjectConfigurator<DataType, ObjectType>));
    
    auto it = m_instances.find(type_id);
    if (it != m_instances.end()) {
      return static_cast<IObjectConfigurator<DataType, ObjectType>*>(
          it->second.get());
    }
    
    // Create using registry
    auto instance = ConfiguratorRegistry<SourceType, DataType, ObjectType>
        ::Create(m_event_handler);
    
    if (!instance) {
      return std::unexpected(FailInfo{
          FailMode::NullPointer, 
          "Failed to create configurator"});
    }
    
    auto* ptr = instance.get();
    m_instances[type_id] = std::move(instance);
    return ptr;
  }
  
  // Similar for data providers
  template<typename DataType>
  std::expected<IDataProvider<DataType>*, FailInfo>
  GetDataProvider() {
    // Similar pattern
  }
};
```

### Addressing Specific Concerns

#### 1. "How do concrete implementations accept data type without type erasure?"

**Answer**: Template specialization at the interface level, not runtime polymorphism for data types.

```cpp
// Each data source gets its own template specialization
template<typename DataType>
class ISceneConfigurator {
  virtual std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene& scene, const DataType& data) = 0;
};

// FlatBuffers specialization - compile-time resolution
class FlatbuffersSceneConfigurator 
    : public ISceneConfigurator<SceneDataFbs> {
  // Knows about SceneDataFbs at compile time
};

// Future JSON specialization
class JSONSceneConfigurator 
    : public ISceneConfigurator<JSONSceneData> {
  // Knows about JSONSceneData at compile time
};
```

The factory uses templates to create the right type:
```cpp
// User code - type-safe, no type erasure
auto configurator = factory.GetConfigurator<SceneDataFbs, Scene>();
configurator->ConfigureScene(scene, flatbuffers_data);
```

#### 2. "Where is coupling allowed to happen?"

**Answer**: Coupling is confined to three specific places:

1. **ConfiguratorRegistry Specializations** (compile-time)
   ```cpp
   template<>
   struct ConfiguratorRegistry<DataSourceType::Flatbuffers, ...> {
     // Only place that knows about Flatbuffers concrete class
   };
   ```

2. **Top-Level Factory Instantiation** (application entry point)
   ```cpp
   DataAccessFactory<DataSourceType::Flatbuffers> factory(event_handler);
   ```

3. **Data File Format** (file I/O layer)
   ```cpp
   // Data loaders know their format
   class FlatbuffersDataLoader : public IDataProvider<SceneDataFbs> { ... };
   ```

#### 3. "Should top-level objects source their own data?"

**Answer**: Yes, with a unified pattern:

```cpp
// Each top-level object has a paired provider
template<typename DataType>
class IDataProvider {
  virtual std::expected<DataType, FailInfo> LoadData() = 0;
};

// Usage pattern
auto provider = factory.GetDataProvider<SceneDataFbs>();
auto data = provider->LoadData();

auto configurator = factory.GetConfigurator<SceneDataFbs, Scene>();
configurator->ConfigureScene(scene, data.value());
```

This keeps the data source decision at the factory level while allowing each object type to load its own data.

## Implementation Strategy

### Phase 1: Create Template Infrastructure

1. Define generic interfaces with templates
2. Create ConfiguratorRegistry infrastructure
3. Update DataAccessFactory to use templates

### Phase 2: Refactor Existing Configurators

1. Update ISceneConfigurator to be template-based
2. Refactor FlatbuffersSceneConfigurator to match new pattern
3. Extract free functions for nested types (SceneInfo, SceneState)

### Phase 3: Apply Pattern to Other Types

1. Update IEntityConfigurator
2. Update IUIElementConfigurator
3. Ensure consistent pattern across all configurators

### Phase 4: Documentation and Examples

1. Document the pattern for adding new data types
2. Create example showing JSON implementation
3. Update architecture documentation

## Benefits of Proposed Approach

1. ✅ **No Switch Statements**: Template specialization replaces switch-based dispatch
2. ✅ **Compile-Time Type Safety**: No type erasure, full type checking
3. ✅ **Easy Extension**: Add new data type by creating specialization
4. ✅ **Clear Coupling Points**: Explicitly defined in registry
5. ✅ **Composable**: Configurators can call other configurators
6. ✅ **Reusable**: Free functions for nested types usable anywhere
7. ✅ **Consistent**: Same pattern for all object types

## Example: Adding a New Data Type

To add JSON support:

```cpp
// 1. Define JSON data structure
struct SceneDataJSON {
  // JSON-specific structure
};

// 2. Create configurator
class JSONSceneConfigurator 
    : public ISceneConfigurator<SceneDataJSON> {
  // Implementation
};

// 3. Register in ConfiguratorRegistry
template<>
struct ConfiguratorRegistry<DataSourceType::JSON, SceneDataJSON, Scene> {
  static std::unique_ptr<ISceneConfigurator<SceneDataJSON>> 
  Create(EventHandler& eh) {
    return std::make_unique<JSONSceneConfigurator>(eh);
  }
};

// 4. Use it
DataAccessFactory<DataSourceType::JSON> factory(event_handler);
auto configurator = factory.GetConfigurator<SceneDataJSON, Scene>();
```

No existing code needs to change!

## Comparison with Current System

| Aspect | Current | Proposed |
|--------|---------|----------|
| Adding new data type | Modify DataAccessFactory switch | Add template specialization |
| Type safety | Mixed (some type erasure) | Full compile-time checking |
| Code reuse | Limited | High (free functions + composition) |
| Coupling location | Scattered | Explicit (registry only) |
| Extensibility | Manual | Template-based |
| Consistency | Varies by type | Uniform pattern |

## Migration Path

The proposed architecture can be adopted incrementally:

1. **Phase 1**: Add template infrastructure alongside existing code
2. **Phase 2**: Migrate one configurator type (e.g., Scene)
3. **Phase 3**: Migrate remaining configurators
4. **Phase 4**: Remove old DataAccessFactory switch implementation

This allows testing the new pattern without disrupting existing functionality.

## Recommendations

1. **Adopt template-based ConfiguratorRegistry** for all new configurators
2. **Extract free functions** for nested/dependent types
3. **Use composition** when object A contains object B with interface
4. **Confine coupling** to registry specializations only
5. **Document pattern** for team consistency

## Conclusion

The proposed architecture provides a general, extensible pattern for native object creation and configuration that:
- Eliminates tight coupling between data types
- Enables easy addition of new data types at compile time
- Maintains full type safety without type erasure
- Clearly defines where coupling is allowed
- Promotes code reuse through composition and free functions

This approach addresses all concerns raised in the problem statement while providing a clear, consistent pattern for the entire codebase.
