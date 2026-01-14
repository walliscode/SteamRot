# General Data Type Switching Mechanism

This document explains how to create a general-purpose data type switching mechanism that can be used throughout the codebase, similar to the current `DataAccessFactory` but using the proposed template-based pattern.

## Problem Clarification

**User's Need**: A general approach where:
1. Data source type is configured in **one central place** (like DataAccessFactory)
2. All code uses that configuration when loading/configuring data
3. No need for type-specific facades (like a scene-only facade)
4. The registry/factory/template infrastructure handles the switching

**Goal**: Replace the current `DataAccessFactory` switch-based approach with the template-based pattern while maintaining a single configuration point.

## Solution: Unified Data Source Registry

### Architecture Overview

```
Application Startup
    ↓
Set DataSourceType (Flatbuffers, JSON, etc.)
    ↓
Create UnifiedConfiguratorFactory with that type
    ↓
Pass to GameContext
    ↓
All code uses the factory without knowing concrete type
    ↓
Factory returns configurators based on registry
```

## Implementation

### Step 1: Unified Configurator Factory (Non-Template)

Create a **non-template** wrapper that internally holds the template-based factory:

```cpp
/////////////////////////////////////////////////
/// @file UnifiedConfiguratorFactory.h
/// @brief General-purpose configurator factory with runtime data source selection
/////////////////////////////////////////////////

#pragma once

#include "DataSourceType.h"
#include "EventHandler.h"
#include "FailInfo.h"
#include <expected>
#include <memory>

namespace steamrot {

// Forward declarations for all possible configurator return types
template<typename DataType, typename ObjectType>
class IConfigurator;

template<typename DataType>
class IDataProvider;

/////////////////////////////////////////////////
/// @brief Unified configurator factory with runtime data source selection
///
/// This class provides a general-purpose interface for obtaining configurators
/// and data providers. The data source type (FlatBuffers, JSON, etc.) is 
/// selected once at construction and used for all subsequent requests.
///
/// This is the modern replacement for DataAccessFactory, using the template
/// registry pattern internally while presenting a unified interface externally.
///
/// **Usage Pattern**:
/// 1. Application startup: Create with desired DataSourceType
/// 2. Pass to GameContext
/// 3. All code requests configurators through generic template methods
/// 4. Factory dispatches to appropriate implementation via registry
/////////////////////////////////////////////////
class UnifiedConfiguratorFactory {
private:
  /////////////////////////////////////////////////
  /// @brief Type-erased holder for the actual template factory
  /////////////////////////////////////////////////
  struct FactoryHolder {
    virtual ~FactoryHolder() = default;
    
    // Template methods that derived class will implement
    virtual void* GetConfiguratorImpl(
        std::type_index data_type, 
        std::type_index object_type) = 0;
    
    virtual void* GetDataProviderImpl(
        std::type_index data_type) = 0;
  };
  
  /////////////////////////////////////////////////
  /// @brief Template implementation holder
  /////////////////////////////////////////////////
  template<DataSourceType SourceType>
  class FactoryHolderImpl : public FactoryHolder {
  private:
    EventHandler& m_event_handler;
    ConfiguratorFactory<SourceType> m_factory;
    
  public:
    FactoryHolderImpl(EventHandler& event_handler)
        : m_event_handler(event_handler),
          m_factory(event_handler) {}
    
    void* GetConfiguratorImpl(
        std::type_index data_type,
        std::type_index object_type) override {
      // This will be specialized per data/object type pair
      return nullptr;
    }
    
    void* GetDataProviderImpl(
        std::type_index data_type) override {
      // This will be specialized per data type
      return nullptr;
    }
    
    // Expose the typed factory for use in Get methods
    ConfiguratorFactory<SourceType>& GetFactory() { return m_factory; }
  };
  
  EventHandler& m_event_handler;
  DataSourceType m_data_source_type;
  std::unique_ptr<FactoryHolder> m_factory_holder;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor
  ///
  /// @param data_source_type The data source type to use (FlatBuffers, JSON, etc.)
  /// @param event_handler Reference to event handler
  /////////////////////////////////////////////////
  UnifiedConfiguratorFactory(DataSourceType data_source_type,
                            EventHandler& event_handler);

  /////////////////////////////////////////////////
  /// @brief Get configurator for specific data and object types
  ///
  /// This is a template method that works with any data/object type pair.
  /// The actual configurator returned depends on the DataSourceType set
  /// at construction.
  ///
  /// @tparam DataType The concrete data type (e.g., SceneDataFbs, JSONSceneData)
  /// @tparam ObjectType The native object type (e.g., Scene, EngineSnapshot)
  /// @return Pointer to configurator or FailInfo on error
  ///
  /// @example
  /// // Get Scene configurator - works regardless of data source
  /// auto config = factory.GetConfigurator<SceneDataFbs, Scene>();
  /////////////////////////////////////////////////
  template<typename DataType, typename ObjectType>
  std::expected<IConfigurator<DataType, ObjectType>*, FailInfo>
  GetConfigurator() {
    // Implementation dispatches based on m_data_source_type
    // See implementation section below
  }

  /////////////////////////////////////////////////
  /// @brief Get data provider for specific data type
  ///
  /// @tparam DataType The data type to provide (e.g., SceneDataFbs)
  /// @return Pointer to data provider or FailInfo on error
  /////////////////////////////////////////////////
  template<typename DataType>
  std::expected<IDataProvider<DataType>*, FailInfo>
  GetDataProvider() {
    // Implementation dispatches based on m_data_source_type
  }
  
  /////////////////////////////////////////////////
  /// @brief Get the configured data source type
  /////////////////////////////////////////////////
  DataSourceType GetDataSourceType() const { return m_data_source_type; }
};

} // namespace steamrot
```

### Step 2: Implementation with Runtime Dispatch

```cpp
/////////////////////////////////////////////////
/// @file UnifiedConfiguratorFactory.cpp
/// @brief Implementation of UnifiedConfiguratorFactory
/////////////////////////////////////////////////

#include "UnifiedConfiguratorFactory.h"
#include "ConfiguratorFactory.h"
#include "ConfiguratorRegistry.h"

namespace steamrot {

/////////////////////////////////////////////////
UnifiedConfiguratorFactory::UnifiedConfiguratorFactory(
    DataSourceType data_source_type,
    EventHandler& event_handler)
    : m_data_source_type(data_source_type),
      m_event_handler(event_handler) {
  
  // Create the appropriate template factory based on data source type
  // This is the ONLY switch statement - happens once at construction
  switch (data_source_type) {
  case DataSourceType::Flatbuffers:
    m_factory_holder = std::make_unique<
        FactoryHolderImpl<DataSourceType::Flatbuffers>>(event_handler);
    break;
  
  case DataSourceType::JSON:
    m_factory_holder = std::make_unique<
        FactoryHolderImpl<DataSourceType::JSON>>(event_handler);
    break;
  
  default:
    throw std::runtime_error(
        "Unsupported data source type in UnifiedConfiguratorFactory");
  }
}

/////////////////////////////////////////////////
// Template method implementations
/////////////////////////////////////////////////

template<typename DataType, typename ObjectType>
std::expected<IConfigurator<DataType, ObjectType>*, FailInfo>
UnifiedConfiguratorFactory::GetConfigurator() {
  
  // Dispatch based on configured data source type
  switch (m_data_source_type) {
  case DataSourceType::Flatbuffers: {
    auto* holder = static_cast<
        FactoryHolderImpl<DataSourceType::Flatbuffers>*>(
            m_factory_holder.get());
    return holder->GetFactory().template GetConfigurator<DataType, ObjectType>();
  }
  
  case DataSourceType::JSON: {
    auto* holder = static_cast<
        FactoryHolderImpl<DataSourceType::JSON>*>(
            m_factory_holder.get());
    return holder->GetFactory().template GetConfigurator<DataType, ObjectType>();
  }
  
  default:
    return std::unexpected(FailInfo{
        FailMode::EnumValueNotHandled,
        "Unsupported data source type in GetConfigurator"});
  }
}

template<typename DataType>
std::expected<IDataProvider<DataType>*, FailInfo>
UnifiedConfiguratorFactory::GetDataProvider() {
  
  // Similar dispatch for data providers
  switch (m_data_source_type) {
  case DataSourceType::Flatbuffers: {
    auto* holder = static_cast<
        FactoryHolderImpl<DataSourceType::Flatbuffers>*>(
            m_factory_holder.get());
    return holder->GetFactory().template GetDataProvider<DataType>();
  }
  
  case DataSourceType::JSON: {
    auto* holder = static_cast<
        FactoryHolderImpl<DataSourceType::JSON>*>(
            m_factory_holder.get());
    return holder->GetFactory().template GetDataProvider<DataType>();
  }
  
  default:
    return std::unexpected(FailInfo{
        FailMode::EnumValueNotHandled,
        "Unsupported data source type in GetDataProvider"});
  }
}

// Explicit template instantiations for commonly used types
template std::expected<IConfigurator<SceneDataFbs, Scene>*, FailInfo>
UnifiedConfiguratorFactory::GetConfigurator<SceneDataFbs, Scene>();

template std::expected<IConfigurator<EngineSnapshotFbs, EngineSnapshot>*, FailInfo>
UnifiedConfiguratorFactory::GetConfigurator<EngineSnapshotFbs, EngineSnapshot>();

// Add instantiations for other commonly used types...

} // namespace steamrot
```

### Step 3: Update GameContext

```cpp
/////////////////////////////////////////////////
/// @file GameContext.h
/// @brief Updated GameContext with unified factory
/////////////////////////////////////////////////

#pragma once

#include "EventHandler.h"
#include "UnifiedConfiguratorFactory.h"
// ... other includes

namespace steamrot {

struct GameContext {
  EventHandler& event_handler;
  
  /////////////////////////////////////////////////
  /// @brief Unified configurator factory
  ///
  /// All code uses this to get configurators and data providers.
  /// Data source type is configured once at application startup.
  /////////////////////////////////////////////////
  UnifiedConfiguratorFactory& configurator_factory;
  
  // ... other context members
};

} // namespace steamrot
```

### Step 4: Application Startup

```cpp
/////////////////////////////////////////////////
/// @file main.cpp or Engine.cpp
/// @brief Application entry - configure data source once
/////////////////////////////////////////////////

#include "UnifiedConfiguratorFactory.h"
#include "GameContext.h"
#include "Engine.h"

int main() {
  using namespace steamrot;

  EventHandler event_handler;

  // *** SINGLE PLACE TO CONFIGURE DATA SOURCE TYPE ***
  DataSourceType data_source = DataSourceType::Flatbuffers;
  
  // For testing with JSON:
  // DataSourceType data_source = DataSourceType::JSON;
  
  // For production:
  // DataSourceType data_source = DataSourceType::Flatbuffers;
  
  // Create unified factory with selected data source
  UnifiedConfiguratorFactory configurator_factory(data_source, event_handler);

  // Create game context
  GameContext game_context{
      .event_handler = event_handler,
      .configurator_factory = configurator_factory
      // ... other members
  };

  // Run engine
  Engine engine(game_context);
  engine.Run();

  return 0;
}
```

### Step 5: Using the Factory Throughout the Codebase

#### In SceneFactory

```cpp
/////////////////////////////////////////////////
/// @file SceneFactory.cpp
/// @brief SceneFactory using unified factory
/////////////////////////////////////////////////

#include "SceneFactory.h"

namespace steamrot {

std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateSceneFromDefault(SceneType scene_type) {

  // Step 1: Get data provider from unified factory
  // Works regardless of data source type (FlatBuffers, JSON, etc.)
  auto provider_result = 
      m_game_context.configurator_factory.GetDataProvider<SceneDataFbs>();
  
  if (!provider_result)
    return std::unexpected(provider_result.error());

  auto* provider = provider_result.value();

  // Step 2: Load data
  auto data_result = provider->LoadData(scene_type);
  if (!data_result)
    return std::unexpected(data_result.error());

  // Step 3: Create empty scene
  auto scene_result = CreateEmptyScene(scene_type);
  if (!scene_result)
    return std::unexpected(scene_result.error());

  auto scene = std::move(scene_result.value());

  // Step 4: Get configurator from unified factory
  auto configurator_result = 
      m_game_context.configurator_factory.GetConfigurator<SceneDataFbs, Scene>();
  
  if (!configurator_result)
    return std::unexpected(configurator_result.error());

  // Step 5: Configure scene
  auto config_result = configurator_result.value()->Configure(
      *scene, data_result.value());
  
  if (!config_result)
    return std::unexpected(config_result.error());

  return scene;
}

} // namespace steamrot
```

#### In TestEngine (Loading EngineSnapshot)

```cpp
/////////////////////////////////////////////////
/// @file TestEngine.cpp
/// @brief TestEngine using unified factory for EngineSnapshot
/////////////////////////////////////////////////

#include "TestEngine.h"

namespace steamrot {

std::expected<std::monostate, FailInfo>
TestEngine::LoadEngineSnapshot(const EngineSnapshot& snapshot_data) {

  // Get EngineSnapshot configurator from unified factory
  // Works regardless of whether data came from FlatBuffers or JSON
  auto configurator_result = 
      m_game_context.configurator_factory.GetConfigurator<
          EngineSnapshotFbs, EngineSnapshot>();
  
  if (!configurator_result)
    return std::unexpected(configurator_result.error());

  // Configure current engine state from snapshot
  auto config_result = configurator_result.value()->Configure(
      m_current_engine_state, snapshot_data);
  
  if (!config_result)
    return std::unexpected(config_result.error());

  return std::monostate{};
}

} // namespace steamrot
```

#### In Any Other Code

```cpp
/////////////////////////////////////////////////
/// @brief Generic pattern for using the unified factory
/////////////////////////////////////////////////

// Get configurator for any type
auto config = game_context.configurator_factory.GetConfigurator<
    SomeDataType, SomeObjectType>();

// Use configurator
config.value()->Configure(object, data);

// Get data provider for any type
auto provider = game_context.configurator_factory.GetDataProvider<
    SomeDataType>();

// Use provider
auto data = provider.value()->LoadData(...);
```

## Complete Architecture Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                    Application Startup                       │
│                                                              │
│  DataSourceType source = Flatbuffers;  ◄─── ONLY PLACE     │
│                                             TO CONFIGURE     │
│  UnifiedConfiguratorFactory factory(source, event_handler); │
│                                                              │
│  GameContext ctx{                                           │
│    .configurator_factory = factory                          │
│  };                                                          │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│              UnifiedConfiguratorFactory                      │
│           (Holds DataSourceType internally)                  │
│                                                              │
│  switch (m_data_source_type) {                              │
│    case Flatbuffers:                                         │
│      return flatbuffers_factory.GetConfigurator<D, O>();    │
│    case JSON:                                                │
│      return json_factory.GetConfigurator<D, O>();           │
│  }                                                           │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│          ConfiguratorFactory<DataSourceType>                 │
│               (Template-based factory)                       │
│                                                              │
│  template<typename DataType, typename ObjectType>           │
│  GetConfigurator() {                                         │
│    return ConfiguratorRegistry<SourceType, DataType,        │
│                                ObjectType>::Create();        │
│  }                                                           │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│        ConfiguratorRegistry Specializations                  │
│              (Compile-time dispatch)                         │
│                                                              │
│  template<>                                                  │
│  struct ConfiguratorRegistry<Flatbuffers, SceneDataFbs,     │
│                             Scene> {                         │
│    static auto Create() {                                    │
│      return make_unique<FlatbuffersSceneConfigurator>();    │
│    }                                                         │
│  };                                                          │
└─────────────────────────────────────────────────────────────┘
```

## Usage Throughout Codebase

### Pattern for All Code

```cpp
// 1. SceneFactory
auto config = m_game_context.configurator_factory
    .GetConfigurator<SceneDataFbs, Scene>();

// 2. SceneManager
auto provider = m_game_context.configurator_factory
    .GetDataProvider<SceneManagerDataFbs>();

// 3. TestEngine
auto config = m_game_context.configurator_factory
    .GetConfigurator<EngineSnapshotFbs, EngineSnapshot>();

// 4. SaveSystem
auto config = m_game_context.configurator_factory
    .GetConfigurator<SaveDataFbs, SaveData>();

// 5. Any other code
auto config = m_game_context.configurator_factory
    .GetConfigurator<AnyDataType, AnyObjectType>();
```

**Key Point**: All code uses the same factory, all code calls the same methods. The data source type is configured once and used everywhere.

## Comparison with Current DataAccessFactory

### Current DataAccessFactory

```cpp
class DataAccessFactory {
  DataType m_data_type;
  
  // Switch for each provider type
  ISceneDataProvider* GetSceneDataProvider() {
    switch (m_data_type) {
      case Flatbuffers: return new FlatbuffersSceneDataProvider();
      case JSON: return new JSONSceneDataProvider();
    }
  }
  
  // Another switch for configurators
  ISceneConfigurator* GetSceneConfigurator() {
    switch (m_data_type) {
      case Flatbuffers: return new FlatbuffersSceneConfigurator();
      case JSON: return new JSONSceneConfigurator();
    }
  }
  
  // Separate method for each type of provider/configurator
  IEngineDataProvider* GetEngineDataProvider() { /* switch */ }
  IEntityConfigurator* GetEntityConfigurator() { /* switch */ }
  // ... many more methods
};
```

**Problems**:
- Switch statement in every getter method
- Need new method for each object type
- Hard to add new data types (modify many methods)

### Proposed UnifiedConfiguratorFactory

```cpp
class UnifiedConfiguratorFactory {
  DataSourceType m_data_source_type;
  
  // Single template method for all configurators
  template<typename DataType, typename ObjectType>
  IConfigurator<DataType, ObjectType>* GetConfigurator() {
    switch (m_data_source_type) {  // Only switch is here
      case Flatbuffers: return flatbuffers_factory.GetConfigurator<D, O>();
      case JSON: return json_factory.GetConfigurator<D, O>();
    }
  }
  
  // Single template method for all providers
  template<typename DataType>
  IDataProvider<DataType>* GetDataProvider() {
    switch (m_data_source_type) {  // Only switch is here
      case Flatbuffers: return flatbuffers_factory.GetDataProvider<D>();
      case JSON: return json_factory.GetDataProvider<D>();
    }
  }
  
  // That's it - works for all types
};
```

**Benefits**:
- Switch statement in **two** methods only (GetConfigurator, GetDataProvider)
- **No new methods** needed for new object types
- Easy to add new data types (modify two methods)
- Template parameters handle all type combinations

## Adding a New Data Type (JSON Example)

### Step 1: Add to DataSourceType Enum

```cpp
enum class DataSourceType {
  Flatbuffers,
  JSON  // Add this
};
```

### Step 2: Update UnifiedConfiguratorFactory Constructor

```cpp
UnifiedConfiguratorFactory::UnifiedConfiguratorFactory(...) {
  switch (data_source_type) {
  case DataSourceType::Flatbuffers:
    m_factory_holder = std::make_unique<
        FactoryHolderImpl<DataSourceType::Flatbuffers>>(event_handler);
    break;
  
  case DataSourceType::JSON:  // Add this case
    m_factory_holder = std::make_unique<
        FactoryHolderImpl<DataSourceType::JSON>>(event_handler);
    break;
  }
}
```

### Step 3: Update GetConfigurator and GetDataProvider

```cpp
template<typename DataType, typename ObjectType>
auto UnifiedConfiguratorFactory::GetConfigurator() {
  switch (m_data_source_type) {
  case DataSourceType::Flatbuffers:
    return holder->GetFactory().GetConfigurator<DataType, ObjectType>();
  
  case DataSourceType::JSON:  // Add this case
    return holder->GetFactory().GetConfigurator<DataType, ObjectType>();
  }
}
```

### Step 4: Create Registry Specializations for JSON

```cpp
// For each object type, create JSON registry specialization
template<>
struct ConfiguratorRegistry<DataSourceType::JSON, 
                           JSONSceneData, Scene> {
  static auto Create(...) {
    return std::make_unique<JSONSceneConfigurator>(...);
  }
};

template<>
struct ConfiguratorRegistry<DataSourceType::JSON,
                           JSONEngineSnapshotData, EngineSnapshot> {
  static auto Create(...) {
    return std::make_unique<JSONEngineSnapshotConfigurator>(...);
  }
};

// etc.
```

### Step 5: Use JSON

```cpp
// main.cpp - change one line
DataSourceType data_source = DataSourceType::JSON;

// Everything else works unchanged!
```

## Key Benefits

### 1. Single Configuration Point

```cpp
// main.cpp - ONLY place to set data source
DataSourceType data_source = DataSourceType::Flatbuffers;
UnifiedConfiguratorFactory factory(data_source, event_handler);
```

### 2. Unified Interface Throughout Codebase

```cpp
// All code uses same pattern
auto config = game_context.configurator_factory
    .GetConfigurator<DataType, ObjectType>();
```

### 3. Two Switch Statements (Acceptable)

```cpp
// Constructor - switch to create holder
switch (data_source_type) { /* create holder */ }

// GetConfigurator - switch to dispatch
switch (m_data_source_type) { /* dispatch to typed factory */ }

// GetDataProvider - switch to dispatch
switch (m_data_source_type) { /* dispatch to typed factory */ }
```

**Why acceptable**:
- Only **3 switches total** (constructor + 2 template methods)
- All in **one class**
- Runs at specific times (construction + per-request dispatch)
- No switches scattered throughout codebase

### 4. Template Registry Handles Types

All actual type-specific logic in registry specializations:

```cpp
template<>
struct ConfiguratorRegistry<Flatbuffers, SceneDataFbs, Scene> { /* ... */ };

template<>
struct ConfiguratorRegistry<Flatbuffers, EngineSnapshotFbs, EngineSnapshot> { /* ... */ };

template<>
struct ConfiguratorRegistry<JSON, JSONSceneData, Scene> { /* ... */ };
```

### 5. Easy to Add New Object Types

No changes to UnifiedConfiguratorFactory:

```cpp
// Add new object type support - just add registry specialization
template<>
struct ConfiguratorRegistry<Flatbuffers, NewDataType, NewObject> {
  static auto Create(...) { return std::make_unique<NewConfigurator>(...); }
};

// Use immediately
auto config = factory.GetConfigurator<NewDataType, NewObject>();
```

## Summary

**Question**: How to have a general approach for switching data types, similar to DataAccessFactory, that all code can use?

**Answer**: Create **UnifiedConfiguratorFactory** that:

1. **Accepts DataSourceType at construction** (like DataAccessFactory)
2. **Provides template methods** for getting configurators/providers
3. **Internally dispatches** to the correct template factory based on configured type
4. **Uses registry pattern** for actual type resolution

**Key Differences from DataAccessFactory**:
- **DataAccessFactory**: N methods with switches (one per object type)
- **UnifiedConfiguratorFactory**: 2 template methods with switches (covers all types)

**Usage Pattern**:
```cpp
// Configure once
UnifiedConfiguratorFactory factory(DataSourceType::Flatbuffers, eh);

// Use everywhere
auto config = factory.GetConfigurator<AnyData, AnyObject>();
auto provider = factory.GetDataProvider<AnyData>();
```

**Result**:
- ✅ One place to configure data source type
- ✅ All code uses the same factory
- ✅ Two acceptable switches (hidden in factory)
- ✅ Registry handles type-specific logic
- ✅ Easy to add new data types
- ✅ Easy to add new object types
- ✅ Type-safe throughout
