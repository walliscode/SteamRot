# Data Loading and Configuration Architecture

## Overview

This document describes SteamRot's architecture for loading data from various sources and configuring native C++ objects. The system provides a flexible, extensible framework that separates data format concerns from game logic while maintaining type safety and compile-time guarantees.

## Design Philosophy

### Core Principles

1. **Separation of Concerns**: Data format details (FlatBuffers, JSON, XML) are isolated from game code
2. **Interface-Based Design**: Abstract interfaces define contracts, concrete implementations handle specifics
3. **Compile-Time Types**: All types are known at compile time; adding new types requires recompilation (acceptable tradeoff)
4. **Template-Friendly**: Templates can be used for type-safe, reusable code
5. **Avoid Data Type Coupling**: Game code works with native objects (SceneData, SaveData), not format-specific objects (SceneDataFbs, SceneDataXml)
6. **Code Reusability**: Configurators can call other configurators for nested structures
7. **Runtime Flexibility**: Factory pattern allows switching data sources at runtime

## Architecture Components

### 1. Data Providers (IDataProvider Pattern)

**Purpose**: Load and provide data from external sources, returning native objects.

**Conceptual Pattern** (for understanding):
```cpp
// Note: This is a CONCEPTUAL illustration showing the pattern.
// Actual implementation uses specific non-templated interfaces (see below).
template<typename NativeObject>
class IDataProvider {
public:
  virtual ~IDataProvider() = default;
  
  // Provide data with optional selection criteria
  virtual std::expected<NativeObject, FailInfo> 
  ProvideData(/* selection criteria */) const = 0;
};
```

**Actual Implementation** (non-templated, specific interfaces):
```cpp
// Specific interface for each native object type
class ISceneDataProvider {
public:
  virtual ~ISceneDataProvider() = default;
  
  virtual std::expected<SceneData, FailInfo>
  ProvideDefaultSceneData(const SceneType scene_type) const = 0;
  
  virtual std::expected<SceneData, FailInfo>
  ProvideSceneDataFromData(const SceneDataFbs* fb_data) const = 0;
};
```

**Why Non-Templated Interfaces?**
- **Runtime polymorphism**: Factory can switch implementations at runtime
- **Clear contracts**: Each interface defines specific methods for its data type
- **Easy to mock**: Concrete interfaces are simpler to mock for testing
- **No template coupling**: Game code depends on interfaces, not template parameters

**Current Implementations**:

- **`ISaveDataProvider`**: Provides `SaveData` objects
  - `FlatbuffersSaveDataProvider`: Loads from FlatBuffers

- **`ISceneDataProvider`**: Provides `SceneData` objects
  - `FlatbuffersSceneDataProvider`: Loads from FlatBuffers
  - Methods: `ProvideDefaultSceneData(SceneType)`, `ProvideSceneDataFromData(SceneDataFbs*)`

- **`ISceneManagerDataProvider`**: Provides `SceneManagerData` objects
  - `FlatbuffersSceneManagerDataProvider`: Loads from FlatBuffers

- **`IEngineDataProvider`**: Provides `EngineData` objects
  - `FlatbuffersEngineDataProvider`: Loads from FlatBuffers

- **`IUIStyleDataProvider`**: Provides style configuration data
  - `FlatbuffersUIStyleDataProvider`: Loads UI styles from FlatBuffers

**Key Characteristics**:
- Returns native objects, not format-specific objects
- Can take selection criteria (scene type, save file name, etc.)
- May internally use configurators to build objects
- Handles file I/O and data parsing

### 2. Configurators (IConfigurator Pattern)

**Purpose**: Configure existing native objects or create new ones from data sources.

**Conceptual Pattern** (for understanding):
```cpp
// Note: This is a CONCEPTUAL illustration showing the pattern.
// Actual implementation uses specific non-templated interfaces (see below).
template<typename NativeObject, typename DataObject>
class IConfigurator {
public:
  virtual ~IConfigurator() = default;
  
  // Configure existing object in-place
  virtual std::expected<std::monostate, FailInfo>
  Configure(NativeObject& obj, const DataObject* data) = 0;
  
  // Create new object (when applicable)
  virtual std::expected<NativeObject, FailInfo>
  Create(const DataObject* data) = 0;
};
```

**Actual Implementation** (non-templated, specific interfaces with data at construction):
```cpp
// Interface defines what needs to be configured
class IEntityConfigurator {
protected:
  EventHandler& m_event_handler;
  
public:
  IEntityConfigurator(EventHandler& event_handler)
      : m_event_handler(event_handler) {}
  
  virtual ~IEntityConfigurator() = default;
  
  // Configure methods for different components
  virtual std::expected<std::monostate, FailInfo>
  ConfigureEntityMemoryPool(EntityMemoryPool& emp) = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureComponent(Component& component) = 0;
};

// Concrete implementation takes data object at construction
// This hides format-specific logic in the concrete class
class FlatbuffersEntityConfigurator : public IEntityConfigurator {
private:
  const EntityCollectionFbs& m_entity_collection_data;  // Data stored at construction
  
public:
  // Constructor takes data object - format-specific logic is hidden here
  FlatbuffersEntityConfigurator(EventHandler& event_handler,
                               const EntityCollectionFbs& entity_collection_data)
      : IEntityConfigurator(event_handler)
      , m_entity_collection_data(entity_collection_data) {}
  
  // Configure methods implement format-specific logic
  std::expected<std::monostate, FailInfo>
  ConfigureEntityMemoryPool(EntityMemoryPool& emp) override;
};
```

**Why Non-Templated Interfaces with Data at Construction?**
- **Clear separation**: Interface defines what to configure, concrete class handles how
- **Data encapsulation**: Format-specific data objects held privately by concrete classes
- **Runtime flexibility**: Factory can instantiate different concrete configurators
- **Hidden complexity**: Format-specific logic (FlatBuffers, XML) stays in concrete classes

**Current Implementations**:

- **`IEntityConfigurator`**: Configures entity components
  - `FlatbuffersEntityConfigurator`: Configures from FlatBuffers EntityCollectionFbs
  - Methods: `ConfigureEntityMemoryPool()`, `ConfigureCUserInterface()`, etc.
  - Layered configuration: First-layer (independent) and second-layer (dependent) components

- **`ISceneConfigurator`**: Configures scene objects
  - `FlatbuffersSceneConfigurator`: Configures from FlatBuffers SceneDataFbs
  - Methods: `ConfigureSceneInfo()`, `ConfigureSceneResources()`, `ImportEntities()`, etc.

- **`IUIElementConfigurator`**: Creates and configures UI elements
  - `FlatbuffersUIElementConfigurator`: Creates from FlatBuffers UserInterfaceFbs
  - Method: `CreateRootUIElement()` returns `std::unique_ptr<UIElement>`
  - Handles nested UI hierarchies

- **`IUIStyleConfigurator`**: Configures UI styles
  - Methods: `ConfigureStyle()`, `ConfigureStyles()`
  - Uses `IFontProvider` for font resources

**Key Characteristics**:
- Can configure existing objects or create new ones
- Support nested configuration (configurator A calls configurator B)
- Accept format-specific data as input (e.g., `SceneDataFbs*`)
- Return native objects or modify them in-place
- Use `std::expected` for error handling

### 3. Importers/Exporters

**Purpose**: Specialized configurators for complex data operations.

**Pattern**:
```cpp
class IEntityImporter {
public:
  virtual ~IEntityImporter() = default;
  
  virtual std::expected<std::monostate, FailInfo>
  ImportEntities(EntityMemoryPool& emp) = 0;
};

class IEntityExporter {
public:
  virtual ~IEntityExporter() = default;
  
  virtual std::expected<std::monostate, FailInfo>
  ExportEntities(const EntityMemoryPool& emp) = 0;
};
```

**Current Implementations**:

- **`FlatbuffersEntityImporter`**: Imports entities from FlatBuffers
  - Wraps `EntityCollectionFbs` data
  - Uses `FlatbuffersEntityConfigurator` internally
  - Part of `SceneData.entity_transport` variant

**Key Characteristics**:
- Focused on specific operations (import/export)
- Can wrap data sources to provide uniform interface
- Often used in conjunction with configurators

### 4. Factory Pattern (DataAccessFactory)

**Purpose**: Centralized creation and management of data providers and configurators.

**Current Implementation**:

```cpp
class DataAccessFactory {
private:
  EventHandler& m_event_handler;
  DataType m_data_type{DataType::Flatbuffers};
  
  std::unique_ptr<IEngineDataProvider> m_engine_data_provider;
  std::unique_ptr<ISceneManagerDataProvider> m_scene_manager_data_provider;
  std::unique_ptr<ISceneDataProvider> m_scene_data_provider;
  std::unique_ptr<ISceneConfigurator> m_scene_configurator;
  
public:
  DataAccessFactory(EventHandler& event_handler, 
                   DataType data_type = DataType::Flatbuffers);
  
  std::expected<IEngineDataProvider*, FailInfo> GetEngineDataProvider();
  std::expected<ISceneManagerDataProvider*, FailInfo> GetSceneManagerDataProvider();
  std::expected<ISceneDataProvider*, FailInfo> GetSceneDataProvider();
  std::expected<ISceneConfigurator*, FailInfo> GetSceneConfigurator();
  
  std::expected<std::monostate, FailInfo> SetDataType(DataType data_type);
};
```

**Key Characteristics**:
- Single point of control for data access strategy
- Can switch data types at runtime (via `SetDataType()`)
- Manages lifetime of providers and configurators
- Returns raw pointers (factory maintains ownership)

### 5. Data Types

**Native Objects** (format-agnostic):
- `SceneData`: Scene configuration and entity data
- `SaveData`: Game save state
- `EngineData`: Engine configuration
- `SceneManagerData`: Scene manager configuration
- `UIStyle`: UI styling configuration
- `EntityMemoryPool`: Entity component data

**Format-Specific Objects** (internal to providers/configurators):
- `SceneDataFbs`, `SceneInfoFbs`, etc.: FlatBuffers types
- Should NOT be passed around in game code

## Normalized Pattern: Configure() and Create()

### Problem Statement

To maintain consistency and reduce "messiness" across the codebase, we need a normalized approach where:
1. Every provider/configurator has a **`Configure(NativeDataStruct&)`** method
2. If the class creates objects, it has a **`Create()`** method that calls `Configure()` internally
3. Concrete classes have specific implementation methods that `Configure()` delegates to

### Recommended Normalized Pattern

**For Providers** (create and return new objects):

```cpp
// Interface defines the contract
class ISaveDataProvider {
public:
  virtual ~ISaveDataProvider() = default;
  
  // Primary method: Create and return new object
  // Internally calls Configure()
  virtual std::expected<SaveData, FailInfo> ProvideSaveData() const = 0;
};

// Concrete implementation
class FlatbuffersSaveDataProvider : public ISaveDataProvider {
public:
  FlatbuffersSaveDataProvider() = default;
  
  // Public: Create and return new SaveData
  std::expected<SaveData, FailInfo> ProvideSaveData() const override {
    SaveData save_data;  // Create empty object
    
    // Load format-specific data
    auto fb_data = LoadFlatbuffersData();
    if (!fb_data) return std::unexpected(fb_data.error());
    
    // Configure it using the normalized Configure() method
    auto config_result = ConfigureSaveData(save_data, fb_data.value());
    if (!config_result) return std::unexpected(config_result.error());
    
    return save_data;  // Return configured object
  }

private:
  // Normalized Configure() method - configures existing object
  std::expected<std::monostate, FailInfo>
  ConfigureSaveData(SaveData& save_data, const SaveDataFbs* fb_data) const {
    // Delegate to specific implementation methods
    auto meta_result = ConfigureSaveMetaData(save_data.meta_data, 
                                             fb_data->meta_data());
    if (!meta_result) return std::unexpected(meta_result.error());
    
    auto snapshot_result = ConfigureEngineSnapshot(save_data.engine_snapshot,
                                                   fb_data->engine_snapshot());
    if (!snapshot_result) return std::unexpected(snapshot_result.error());
    
    return std::monostate{};
  }
  
  // Specific implementation methods (format-specific logic)
  std::expected<std::monostate, FailInfo>
  ConfigureSaveMetaData(SaveMetaData& meta_data, 
                       const SaveMetaDataFbs* fb_meta) const;
  
  std::expected<std::monostate, FailInfo>
  ConfigureEngineSnapshot(EngineSnapshot& snapshot,
                         const EngineSnapshotFbs* fb_snapshot) const;
};
```

**For Configurators** (configure existing objects):

```cpp
// Interface defines what to configure
class ISceneConfigurator {
public:
  virtual ~ISceneConfigurator() = default;
  
  // Wrapper that calls specific Configure methods
  std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene& scene, const SceneData& scene_data);
  
  // Specific configuration methods
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(Scene& scene, const SceneData& scene_data) = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene& scene, const SceneData& scene_data) = 0;
};

// Concrete implementation
class FlatbuffersSceneConfigurator : public ISceneConfigurator {
public:
  // Implement specific configuration methods
  std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(Scene& scene, const SceneData& scene_data) override {
    // Specific implementation for scene info
    scene.GetSceneInfo().type = scene_data.scene_info.type;
    scene.GetSceneInfo().id = scene_data.scene_info.id;
    return std::monostate{};
  }
  
  std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene& scene, const SceneData& scene_data) override {
    // Specific implementation for scene resources
    // Delegate to helper methods if needed
    return ConfigureRenderTexture(scene, scene_data.scene_resources_config);
  }

private:
  // Helper methods for specific sub-configurations
  std::expected<std::monostate, FailInfo>
  ConfigureRenderTexture(Scene& scene, 
                        const SceneResourcesConfig& config) const;
};
```

### Hierarchical Structure

The normalized pattern creates a clear hierarchy:

```
Public API Method (Create/Provide)
    ↓
Configure(NativeDataStruct&) - Normalized method
    ↓
Specific Implementation Methods - Format-specific logic
    ↓
Helper Methods - Reusable sub-configurations
```

**Example Flow**:
```cpp
// User calls public API
auto scene_data = provider->ProvideDefaultSceneData(scene_type);

// Internally:
// 1. Provider creates empty SceneData
SceneData scene_data;

// 2. Provider loads format-specific data
const SceneDataFbs* fb_data = LoadFlatbuffersFile(scene_type);

// 3. Provider calls normalized Configure()
ConfigureSceneData(scene_data, fb_data);

// 4. Configure() delegates to specific methods
ConfigureSceneInfo(scene_data.scene_info, fb_data->scene_info());
ConfigureSceneResourcesConfig(scene_data.scene_resources_config, 
                              fb_data->scene_resources_config());

// 5. Each specific method may call helpers
ValidateSceneInfo(scene_data.scene_info);
ParseUUID(fb_data->scene_info()->scene_id());

// 6. Return configured object
return scene_data;
```

### Pattern Application to All Native Data Structs

**Rule**: For each native data struct (`SceneData`, `SaveData`, `EngineData`, etc.), ensure:

1. **Provider Interface**: Has a method that creates and returns the object
2. **Provider Implementation**: 
   - Has a private `Configure(NativeStruct&)` method
   - Public method creates object and calls `Configure()`
3. **Specific Methods**: Format-specific logic in separate methods called by `Configure()`

**Consistency Checklist**:
- [ ] `SaveData`: `ISaveDataProvider::ProvideSaveData()` → creates → calls `ConfigureSaveData(SaveData&)`
- [ ] `SceneData`: `ISceneDataProvider::ProvideSceneData()` → creates → calls `ConfigureSceneData(SceneData&)`
- [ ] `EngineData`: `IEngineDataProvider::ProvideEngineData()` → creates → calls `ConfigureEngineData(EngineData&)`
- [ ] `SceneManagerData`: `ISceneManagerDataProvider::ProvideData()` → creates → calls `ConfigureSceneManagerData(SceneManagerData&)`

### Three Approaches Evaluated

**Approach 1: Manual Consistency (RECOMMENDED)**
- Each interface manually defines Configure/Create methods
- Return types are specific to each interface
- Pros: Clear contracts, specific method signatures, runtime polymorphism
- Cons: Requires discipline to maintain consistency
- **Verdict**: Best for this architecture - provides clarity and flexibility

**Approach 2: Templates**
```cpp
// NOT RECOMMENDED for interfaces
template<typename NativeObject, typename DataObject>
class IProvider {
  virtual std::expected<NativeObject, FailInfo> 
  Provide() const = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  Configure(NativeObject& obj, const DataObject* data) = 0;
};
```
- Pros: Automatic consistency, type-safe
- Cons: Can't switch implementations at runtime (factory pattern breaks), template instantiation complexity
- **Verdict**: Not suitable for runtime polymorphism needs

**Approach 3: CRTP (Curiously Recurring Template Pattern)**
```cpp
// Could be used for implementation sharing, not interfaces
template<typename Derived, typename NativeObject>
class ProviderBase {
protected:
  std::expected<NativeObject, FailInfo> CreateAndConfigure() {
    NativeObject obj;
    auto result = static_cast<Derived*>(this)->Configure(obj);
    if (!result) return std::unexpected(result.error());
    return obj;
  }
};
```
- Pros: Code reuse for common patterns
- Cons: More complex, still requires specific interfaces for factory
- **Verdict**: Could complement Approach 1 for implementation, but doesn't solve interface consistency

### Implementation Guidelines

**For New Providers**:
1. Define interface with `ProvideXXX()` method that returns native object
2. In concrete class, implement `ProvideXXX()` to:
   - Create empty native object
   - Call private `ConfigureXXX(NativeObject&)` method
   - Return configured object
3. Implement private `ConfigureXXX(NativeObject&)` that:
   - Takes format-specific data (stored or passed)
   - Delegates to specific implementation methods
   - Follows consistent naming: `ConfigureXXX()` for normalized method

**For New Configurators**:
1. Define interface with `ConfigureXXX()` methods for each aspect
2. In concrete class, implement each `ConfigureXXX()` to:
   - Take native object reference and data
   - Delegate to specific helper methods
   - Follow nested configuration pattern

**Naming Convention**:
- Public API: `Provide{DataType}()`, `Create{Element}()`
- Normalized method: `Configure{DataType}(DataType&, ...)`
- Specific methods: `Configure{SpecificAspect}({AspectType}&, ...)`
- Helpers: `Validate{Aspect}()`, `Parse{Value}()`, `Build{SubComponent}()`

### Benefits of Normalized Pattern

1. **Consistency**: Same structure across all providers/configurators
2. **Clarity**: Clear separation between creation, configuration, and implementation
3. **Reusability**: Specific methods can be called independently
4. **Testability**: Each level can be tested in isolation
5. **Maintainability**: Easy to understand and extend
6. **Hierarchical**: Natural layering from public API to specific implementation

### Migration Guide

To normalize existing code:

1. **Identify** native data structs without normalized Configure() methods
2. **Add** private `Configure(NativeStruct&)` method to provider
3. **Refactor** public method to call `Configure()` after creating object
4. **Extract** format-specific logic into specific implementation methods
5. **Test** that behavior is unchanged

**Example**:
```cpp
// Before:
std::expected<SaveData, FailInfo> ProvideSaveData() const override {
  SaveData save_data;
  // 50 lines of inline configuration logic
  return save_data;
}

// After:
std::expected<SaveData, FailInfo> ProvideSaveData() const override {
  SaveData save_data;
  auto result = ConfigureSaveData(save_data);
  if (!result) return std::unexpected(result.error());
  return save_data;
}

private:
std::expected<std::monostate, FailInfo>
ConfigureSaveData(SaveData& save_data) const {
  // Normalized configuration method
  auto meta_result = ConfigureSaveMetaData(save_data.meta_data);
  // ... delegate to specific methods
}
```

## Workflow Patterns

### Pattern 1: Providing Standalone Objects

**Use Case**: Loading SaveData, SceneData, or other standalone objects.

```cpp
// 1. Get provider from factory
auto save_provider_result = factory.GetSaveDataProvider();
if (!save_provider_result) {
  // handle error
}
ISaveDataProvider* provider = save_provider_result.value();

// 2. Use provider to get native object
auto save_data_result = provider->ProvideSaveData();
if (!save_data_result) {
  // handle error
}
SaveData save_data = save_data_result.value();

// 3. Use native object in game code
// No coupling to FlatBuffers or any specific format
ProcessSaveData(save_data);
```

**Providers with Selection Criteria**:
```cpp
// Scene data with scene type selection
auto scene_data_result = scene_provider->ProvideDefaultSceneData(
    SceneType::SceneType_CRAFTING);
```

### Pattern 2: Configuring Existing Objects

**Use Case**: Configuring scene objects, entity components, or other mutable structures.

```cpp
// 1. Get configurator from factory
auto scene_configurator_result = factory.GetSceneConfigurator();
if (!scene_configurator_result) {
  // handle error
}
ISceneConfigurator* configurator = scene_configurator_result.value();

// 2. Create or get the object to configure
Scene scene;

// 3. Use configurator to populate object
auto config_result = configurator->ConfigureScene(scene, scene_data);
if (!config_result) {
  // handle error
}

// 4. Scene is now configured, use it
scene.RunLogic();
```

### Pattern 3: Creating New Objects

**Use Case**: Creating UI elements, entities, or other objects from data.

```cpp
// 1. Get data for configuration
const UserInterfaceFbs& ui_data = GetUIData();

// 2. Create configurator with data source
FlatbuffersUIElementConfigurator configurator(event_handler, ui_data);

// 3. Create new object
auto ui_element_result = configurator.CreateRootUIElement();
if (!ui_element_result) {
  // handle error
}
std::unique_ptr<UIElement> ui_element = std::move(ui_element_result.value());

// 4. Use created object
RenderUIElement(*ui_element);
```

### Pattern 4: Nested Configuration (Configurator Composition)

**Use Case**: Struct A contains struct B; configurator A calls configurator B.

```cpp
class FlatbuffersSceneDataProvider : public ISceneDataProvider {
public:
  std::expected<SceneData, FailInfo> 
  ProvideSceneDataFromData(const SceneDataFbs* fb_data) const override {
    SceneData scene_data;
    
    // Configure SceneInfo (nested structure)
    auto info_result = ConfigureSceneInfo(
        scene_data.scene_info, 
        fb_data->scene_info());
    if (!info_result)
      return std::unexpected(info_result.error());
    
    // Configure SceneResourcesConfig (nested structure)
    auto resources_result = ConfigureSceneResourcesConfig(
        scene_data.scene_resources_config,
        fb_data->scene_resources_config());
    if (!resources_result)
      return std::unexpected(resources_result.error());
    
    return scene_data;
  }

private:
  // Helper configurator for nested structure
  std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(SceneInfo& info, const SceneInfoFbs* fb_info) const;
  
  std::expected<std::monostate, FailInfo>
  ConfigureSceneResourcesConfig(SceneResourcesConfig& config,
                               const SceneResourcesConfigFbs* fb_config) const;
};
```

**Benefits**:
- Code reuse: Each configurator handles one type
- Single responsibility: Each method configures one structure
- Composability: Build complex objects from simple pieces
- Testability: Test each configurator independently

### Pattern 5: Entity Configuration with Layers

**Use Case**: Components depend on other components.

```cpp
class FlatbuffersEntityConfigurator : public IEntityConfigurator {
public:
  std::expected<std::monostate, FailInfo>
  ConfigureEntityMemoryPool(EntityMemoryPool& emp) override {
    // First layer: Configure independent components
    auto first_layer_result = ConfigureFirstLayerComponents(emp);
    if (!first_layer_result)
      return std::unexpected(first_layer_result.error());
    
    // Second layer: Configure components that depend on first layer
    auto second_layer_result = ConfigureSecondLayerComponents(emp);
    if (!second_layer_result)
      return std::unexpected(second_layer_result.error());
    
    return std::monostate{};
  }
  
private:
  // First layer: No dependencies
  std::expected<std::monostate, FailInfo>
  ConfigureFirstLayerComponents(EntityMemoryPool& emp) override;
  
  // Second layer: Depends on first layer
  std::expected<std::monostate, FailInfo>
  ConfigureSecondLayerComponents(EntityMemoryPool& emp) override;
};
```

### Pattern 6: Runtime Data Type Switching

**Use Case**: Switch between FlatBuffers, JSON, XML, etc. at runtime.

```cpp
// Initialize with FlatBuffers
DataAccessFactory factory(event_handler, DataType::Flatbuffers);

// Use FlatBuffers providers
auto scene_data = factory.GetSceneDataProvider()->ProvideDefaultSceneData(scene_type);

// Switch to XML (hypothetical future implementation)
auto switch_result = factory.SetDataType(DataType::XML);
if (!switch_result) {
  // handle error
}

// Now using XML providers
auto scene_data_xml = factory.GetSceneDataProvider()->ProvideDefaultSceneData(scene_type);
```

**Note**: Currently only FlatBuffers is implemented, but the architecture supports adding new formats.

## Adding New Data Types

### Step 1: Define the Data Type Enum

```cpp
// In DataType.h
namespace steamrot {
enum class DataType : uint8_t {
  Flatbuffers = 0,
  XML = 1,          // Add new type
  JSON = 2,         // Add new type
};
}
```

### Step 2: Create Format-Specific Data Structures

For FlatBuffers: Create `.fbs` schemas
For XML: Define XML schema or structure
For JSON: Define JSON schema

### Step 3: Implement Provider Interface

```cpp
// XmlSceneDataProvider.h
class XmlSceneDataProvider : public ISceneDataProvider {
public:
  std::expected<SceneData, FailInfo>
  ProvideDefaultSceneData(const SceneType scene_type) const override;
  
  std::expected<SceneData, FailInfo>
  ProvideSceneDataFromData(const SceneDataXml* xml_data) const override;

private:
  // Helper methods for nested configuration
  std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(SceneInfo& info, const SceneInfoXml* xml_info) const;
  
  // ... other configuration methods
};
```

### Step 4: Implement Configurator Interface (if needed)

```cpp
// XmlEntityConfigurator.h
class XmlEntityConfigurator : public IEntityConfigurator {
public:
  XmlEntityConfigurator(EventHandler& event_handler,
                       const EntityCollectionXml& entity_data);
  
  std::expected<std::monostate, FailInfo>
  ConfigureEntityMemoryPool(EntityMemoryPool& emp) override;
  
  // ... implement all required methods
};
```

### Step 5: Update DataAccessFactory

```cpp
// In DataAccessFactory.cpp
std::expected<std::monostate, FailInfo> 
DataAccessFactory::SetXmlDataProviders() {
  m_engine_data_provider = std::make_unique<XmlEngineDataProvider>();
  m_scene_manager_data_provider = std::make_unique<XmlSceneManagerDataProvider>();
  m_scene_data_provider = std::make_unique<XmlSceneDataProvider>(m_event_handler);
  m_scene_configurator = std::make_unique<XmlSceneConfigurator>();
  
  return std::monostate{};
}

std::expected<std::monostate, FailInfo> 
DataAccessFactory::SetDataProviders() {
  switch (m_data_type) {
  case DataType::Flatbuffers:
    return SetFlatbuffersDataProviders();
  case DataType::XML:
    return SetXmlDataProviders();  // Add new case
  case DataType::JSON:
    return SetJsonDataProviders();  // Add new case
  default:
    return std::unexpected(FailInfo{FailMode::EnumValueNotHandled,
                                    "Unsupported data type"});
  }
}
```

### Step 6: Recompile

Since all types are compile-time, adding a new data type requires recompilation. This is an acceptable tradeoff for the type safety and performance benefits.

## Adding New Native Object Types

### Step 1: Define Native Object Struct

```cpp
// In src/types/core/NewData.h
namespace steamrot {
struct NewData {
  int m_value{0};
  std::string m_name{};
  // ... other fields
};
} // namespace steamrot
```

### Step 2: Create Provider Interface

```cpp
// In src/types/interfaces/INewDataProvider.h
namespace steamrot {
class INewDataProvider {
public:
  virtual ~INewDataProvider() = default;
  
  virtual std::expected<NewData, FailInfo> 
  ProvideNewData(/* selection criteria */) const = 0;
};
} // namespace steamrot
```

### Step 3: Create Format-Specific Implementation

```cpp
// In src/data_providers/FlatbuffersNewDataProvider.h
class FlatbuffersNewDataProvider : public INewDataProvider {
public:
  std::expected<NewData, FailInfo>
  ProvideNewData(/* selection criteria */) const override;

private:
  std::expected<std::monostate, FailInfo>
  ConfigureNewData(NewData& data, const NewDataFbs* fb_data) const;
};
```

### Step 4: Add to DataAccessFactory

```cpp
// In DataAccessFactory.h
class DataAccessFactory {
private:
  std::unique_ptr<INewDataProvider> m_new_data_provider{nullptr};

public:
  std::expected<INewDataProvider*, FailInfo> GetNewDataProvider();
};

// In DataAccessFactory.cpp
std::expected<std::monostate, FailInfo>
DataAccessFactory::SetFlatbuffersDataProviders() {
  // ... existing providers ...
  
  m_new_data_provider = std::make_unique<FlatbuffersNewDataProvider>();
  if (!m_new_data_provider) {
    return std::unexpected(FailInfo{FailMode::NullPointer,
                                    "Failed to create provider"});
  }
  
  return std::monostate{};
}

std::expected<INewDataProvider*, FailInfo>
DataAccessFactory::GetNewDataProvider() {
  if (!m_new_data_provider) {
    return std::unexpected(FailInfo{FailMode::NullPointer,
                                    "Provider is null"});
  }
  return m_new_data_provider.get();
}
```

## Design Decisions and Tradeoffs

### Compile-Time Types

**Decision**: All types are known at compile time.

**Benefits**:
- Type safety: Compiler catches errors
- Performance: No runtime type checking overhead
- Clarity: Clear contracts and interfaces
- IDE support: Better autocomplete and refactoring

**Tradeoffs**:
- Adding new types requires recompilation
- Cannot load arbitrary data types at runtime
- Larger binary size with multiple implementations

**Verdict**: Acceptable tradeoff for a game engine where stability and performance matter more than dynamic extensibility.

### Interface-Based Design

**Decision**: Use abstract **non-templated** interfaces (`ISceneDataProvider`, `IEntityConfigurator`) rather than templated interfaces.

**Why Non-Templated?**
- **Runtime polymorphism**: Factory can switch between concrete implementations at runtime
- **Clear contracts**: Each interface defines specific methods for its purpose
- **Easy mocking**: Concrete interfaces are simpler to mock for testing  
- **No template coupling**: Game code depends on interfaces, not template parameters
- **Data at construction**: Concrete classes take data objects (e.g., `EntityCollectionFbs`) at construction, hiding format-specific logic

**Pattern in Practice**:
```cpp
// Interface: Non-templated, defines what to configure
class IEntityConfigurator {
public:
  virtual std::expected<std::monostate, FailInfo>
  ConfigureEntityMemoryPool(EntityMemoryPool& emp) = 0;
};

// Concrete: Takes data at construction, hides format logic
class FlatbuffersEntityConfigurator : public IEntityConfigurator {
private:
  const EntityCollectionFbs& m_data;  // Format-specific data
public:
  FlatbuffersEntityConfigurator(EventHandler& handler,
                               const EntityCollectionFbs& data)
      : m_data(data) {}  // Data stored at construction
  
  // Implementation uses m_data internally
  std::expected<std::monostate, FailInfo>
  ConfigureEntityMemoryPool(EntityMemoryPool& emp) override;
};
```

**Alternative (NOT used): Templated Interface**
```cpp
// Why we DON'T do this:
template<typename DataObject>
class IEntityConfigurator {
  virtual std::expected<std::monostate, FailInfo>
  ConfigureEntityMemoryPool(EntityMemoryPool& emp, const DataObject& data) = 0;
};

// Problems:
// - Factory would need to know template parameters at compile time
// - Can't switch data types at runtime
// - More complex to use with factory pattern
// - Template instantiation in every translation unit
```

**Benefits of Current Approach**:
- Factory instantiates: `std::make_unique<FlatbuffersEntityConfigurator>(handler, data)`
- Interface methods called: `configurator->ConfigureEntityMemoryPool(emp)`
- Data type logic: Hidden inside `FlatbuffersEntityConfigurator` implementation
- Runtime switching: Easy - factory just creates different concrete class
- Clear separation: Interface = what to configure, concrete = how to configure

**Tradeoffs**:
- Virtual function overhead (minimal in practice)
- Separate interfaces per data type (e.g., `ISceneDataProvider`, `ISaveDataProvider`)
- Can't share interface across unrelated types (acceptable - each type has specific needs)

**Verdict**: Non-templated interfaces with data at construction is the right choice. This provides runtime flexibility, clear contracts, and hides format-specific logic in concrete classes while maintaining compile-time type safety for the native objects.

### Separation of Providers and Configurators

**Decision**: Separate data provision (loading) from object configuration.

**Benefits**:
- Single responsibility: Providers load, configurators configure
- Flexibility: Can configure from different sources
- Reusability: Configurators can be called independently
- Testing: Test loading and configuration separately

**Example**:
- `ISceneDataProvider`: Loads `SceneData` from files
- `ISceneConfigurator`: Configures `Scene` object from `SceneData`
- Separation allows configuring scene from in-memory data without file I/O

**Verdict**: Clean separation improves maintainability and testability.

### Factory Pattern for Centralized Management

**Decision**: Use `DataAccessFactory` to manage providers and configurators.

**Benefits**:
- Single point of control: One place to switch data types
- Lifetime management: Factory owns provider instances
- Consistency: All parts of engine use same data type
- Initialization: Set up complex dependencies once

**Tradeoffs**:
- Central dependency: Many systems depend on factory
- Memory overhead: All providers instantiated even if unused
- Coupling: Factory knows about all provider types

**Alternative**: Individual factories per provider type, but this adds complexity and inconsistency.

**Verdict**: Centralized factory is the right choice for engine-wide data access strategy.

### std::expected for Error Handling

**Decision**: Use `std::expected<T, FailInfo>` for operations that may fail.

**Benefits**:
- Explicit error handling: Caller must check result
- Type-safe errors: `FailInfo` provides structured error data
- No exceptions: Predictable control flow
- Composability: Can chain operations

**Pattern**:
```cpp
auto result = provider->ProvideData();
if (!result) {
  // Handle error
  FailInfo error = result.error();
  LogError(error.message);
  return std::unexpected(error);
}
T data = result.value();
```

**Verdict**: `std::expected` is ideal for data loading where errors are expected and recoverable.

## Best Practices

### 1. Avoid Data Type Coupling in Game Code

**Bad**:
```cpp
// Game code knows about FlatBuffers
void ProcessScene(const SceneDataFbs* fb_data) {
  if (fb_data->scene_info()) {
    // Tightly coupled to FlatBuffers
  }
}
```

**Good**:
```cpp
// Game code works with native objects
void ProcessScene(const SceneData& scene_data) {
  // Format-agnostic
}
```

### 2. Use Nested Configuration for Reusability

**Bad**:
```cpp
// Duplicate configuration logic
std::expected<SceneData, FailInfo>
ProvideSceneData(const SceneDataFbs* fb_data) const {
  SceneData scene_data;
  
  // Inline configuration of SceneInfo (not reusable)
  if (fb_data->scene_info()) {
    scene_data.scene_info.type = fb_data->scene_info()->scene_type();
    // ... 20 more lines ...
  }
  
  return scene_data;
}
```

**Good**:
```cpp
// Reusable configuration helpers
std::expected<SceneData, FailInfo>
ProvideSceneData(const SceneDataFbs* fb_data) const {
  SceneData scene_data;
  
  auto info_result = ConfigureSceneInfo(
      scene_data.scene_info, 
      fb_data->scene_info());
  if (!info_result)
    return std::unexpected(info_result.error());
  
  return scene_data;
}

// Separate, testable, reusable
std::expected<std::monostate, FailInfo>
ConfigureSceneInfo(SceneInfo& info, const SceneInfoFbs* fb_info) const {
  // Configuration logic here
}
```

### 3. Support Both Create() and Configure()

**Guideline**: Providers typically **create** and return new native objects. Configurators can **configure** existing objects or **create** new ones depending on use case.

**Pattern: Providers Create Objects**
```cpp
// Providers create and return new native objects
class FlatbuffersSceneDataProvider : public ISceneDataProvider {
public:
  // Creates and returns new SceneData
  std::expected<SceneData, FailInfo>
  ProvideDefaultSceneData(const SceneType scene_type) const override {
    SceneData scene_data;  // Create new object
    
    // Configure it
    auto config_result = ConfigureSceneData(scene_data, /* data */);
    if (!config_result)
      return std::unexpected(config_result.error());
    
    return scene_data;  // Return new object
  }
};
```

**Pattern: Configurators Can Do Both**
```cpp
// Some configurators configure existing objects
class FlatbuffersEntityConfigurator : public IEntityConfigurator {
public:
  // Configure existing EntityMemoryPool
  std::expected<std::monostate, FailInfo>
  ConfigureEntityMemoryPool(EntityMemoryPool& emp) override {
    // Modifies emp in place
  }
  
  // Configure existing component
  std::expected<std::monostate, FailInfo>
  ConfigureComponent(Component& component) override {
    // Modifies component in place
  }
};

// Other configurators create new objects
class FlatbuffersUIElementConfigurator : public IUIElementConfigurator {
public:
  // Creates and returns new UIElement tree
  std::expected<std::unique_ptr<UIElement>, FailInfo>
  CreateRootUIElement() override {
    auto element = std::make_unique<PanelElement>();
    
    // Configure it
    auto config_result = ConfigureElement(*element, /* data */);
    if (!config_result)
      return std::unexpected(config_result.error());
    
    return element;  // Return ownership
  }
};
```

**When to Use Each**:
- **Configure() on existing object**: When object already exists (EntityMemoryPool, Scene)
- **Create() returning new object**: When object must be allocated (polymorphic UIElement tree)
- **Providers always Create()**: They always create and return new native objects

**Key Point**: Concrete classes take **data object at construction** (e.g., `EntityCollectionFbs&`), then use that data internally in their Create()/Configure() methods. This hides format-specific logic.

### 4. Validate Format-Specific Data

**Important**: FlatBuffers and other formats may have null/missing fields.

```cpp
std::expected<std::monostate, FailInfo>
ConfigureSceneInfo(SceneInfo& info, const SceneInfoFbs* fb_info) const {
  // Validate pointer
  if (!fb_info) {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "SceneInfoFbs is null"});
  }
  
  // Validate required fields
  if (!fb_info->scene_type()) {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "scene_type is missing"});
  }
  
  info.type = fb_info->scene_type();
  
  // Optional fields: check before accessing
  if (fb_info->scene_id()) {
    info.id = fb_info->scene_id()->str();
  }
  
  return std::monostate{};
}
```

### 5. Use Factory for Consistent Data Access

**Bad**:
```cpp
// Scattered instantiation
FlatbuffersSceneDataProvider scene_provider(event_handler);
FlatbuffersEngineDataProvider engine_provider;
// What if we need to switch to XML?
```

**Good**:
```cpp
// Centralized management
DataAccessFactory factory(event_handler, DataType::Flatbuffers);

auto scene_provider = factory.GetSceneDataProvider();
auto engine_provider = factory.GetEngineDataProvider();

// Easy to switch data type
factory.SetDataType(DataType::XML);
```

### 6. Maintain Layer Separation

**Architecture Layers**:
1. **Data Layer**: FlatBuffers, JSON, XML (format-specific)
2. **Provider/Configurator Layer**: Interfaces and implementations
3. **Native Object Layer**: `SceneData`, `SaveData` (format-agnostic)
4. **Game Logic Layer**: Uses native objects

**Rule**: Higher layers should not depend on lower layers.

```
Game Logic Layer
      ↓ (uses)
Native Object Layer
      ↓ (configured by)
Provider/Configurator Layer
      ↓ (reads)
Data Layer
```

## Future Considerations

### Potential Extensions

1. **New Data Formats**:
   - XML provider and configurators
   - JSON provider and configurators
   - Binary custom format
   - Network data sources

2. **Data Caching**:
   - Cache loaded data for performance
   - Invalidate cache on file changes
   - Memory-efficient data sharing

3. **Hot Reloading**:
   - Detect file changes
   - Reload and reconfigure objects
   - Preserve runtime state where possible

4. **Data Validation**:
   - Schema validation (FlatBuffers, JSON Schema)
   - Data integrity checks
   - Version compatibility checks

5. **Asynchronous Loading**:
   - Load data on background threads
   - Progress callbacks
   - Avoid blocking game loop

6. **Data Migration**:
   - Convert between formats
   - Upgrade old save files
   - Schema versioning

### Limitations

1. **Compile-Time Types**: Adding new types requires recompilation
   - **Mitigation**: This is acceptable for a game engine; prioritizes type safety over dynamic loading

2. **Factory Centralization**: All providers managed by one factory
   - **Mitigation**: Could create specialized factories if needed (SaveDataFactory, SceneDataFactory)

3. **Runtime Type Switching**: Switching data types recreates all providers
   - **Mitigation**: Typically done once at startup or rarely changed

4. **Memory Overhead**: All providers instantiated even if unused
   - **Mitigation**: Lazy initialization could be added if memory becomes a concern

## Summary

SteamRot's data loading and configuration system provides:

✅ **Separation of Concerns**: Data formats isolated from game logic
✅ **Type Safety**: Compile-time type checking
✅ **Flexibility**: Runtime switching of data sources
✅ **Reusability**: Composable configurators
✅ **Extensibility**: Easy to add new formats and data types
✅ **Testability**: Interface-based design enables mocking
✅ **Error Handling**: `std::expected` for explicit error handling

**Key Pattern**:
```
IDataProvider → Native Object → Game Logic
IConfigurator ↗
```

**Workflow**:
1. Factory provides appropriate providers/configurators
2. Provider loads and returns native objects
3. Configurator configures objects from data
4. Game logic uses native objects (format-agnostic)

This architecture achieves the goals stated in the problem statement:
- ✅ General way to load data from different types
- ✅ Compile-time type knowledge with template support
- ✅ Native objects in game code, no format coupling
- ✅ `IConfigurator<NativeObject, DataObject>` pattern
- ✅ `IDataProvider<DataObject>` pattern
- ✅ Both Create() and Configure() support
- ✅ Factory pattern for runtime switching
- ✅ Nested configuration for code reuse
- ✅ Selection criteria support (scene type, save file, etc.)
- ✅ Easy flowchart for adding new types
- ✅ Prioritizes integration and code readability


## Related Documentation

### Architecture Documents
- **[Quick Reference Guide](DATA_LOADING_QUICK_REFERENCE.md)**: Fast lookup, code snippets, and decision flowcharts
- **[Visual Diagrams](DATA_LOADING_DIAGRAMS.md)**: Architecture diagrams and visual references
- **[XML Implementation Example](EXAMPLE_XML_IMPLEMENTATION.md)**: Complete walkthrough of adding XML support

### Other Architecture
- **[Error Handling](ERROR_HANDLING.md)**: Error patterns using `std::expected` (if exists)
- **[Game Loop](GAME_LOOP.md)**: Scene lifecycle and game loop structure (if exists)
- **[Logic System](LOGIC_SYSTEM.md)**: Logic architecture (if exists)

### Workflows
- **[Adding Components](../workflows/ADDING_COMPONENTS.md)**: Component workflow (if exists)

## External References

- **FlatBuffers Documentation**: https://google.github.io/flatbuffers/
- **std::expected**: C++23 error handling feature
- **Strategy Pattern**: Design pattern for runtime algorithm selection
- **Factory Pattern**: Design pattern for object creation
- **Interface Segregation**: SOLID principle for interface design

