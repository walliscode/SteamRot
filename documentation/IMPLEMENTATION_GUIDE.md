# Implementation Guide: Generalized Configuration Pattern

This guide provides step-by-step instructions for implementing the generalized configuration pattern in SteamRot.

## Overview

This guide shows how to:
1. Create the template infrastructure
2. Migrate existing configurators to the new pattern
3. Add new data types easily
4. Test the implementation

## Prerequisites

Before starting, ensure you understand:
- C++ template specialization
- `std::expected` error handling
- Current configurator architecture (ISceneConfigurator, etc.)

## Step 1: Create Core Template Infrastructure

### 1.1 Create DataSourceType Enum

Create `src/types/core/DataSourceType.h`:

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Enumeration of data source types
/////////////////////////////////////////////////

#pragma once

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Enumeration of supported data source types
///
/// Each data source type represents a different external
/// data format (FlatBuffers, JSON, etc.)
/////////////////////////////////////////////////
enum class DataSourceType {
  Flatbuffers,  // FlatBuffers binary format
  // Future data sources:
  // JSON,
  // Binary,
  // XML
};

} // namespace steamrot
```

### 1.2 Create Generic IConfigurator Interface

Create `src/types/interfaces/IConfigurator.h`:

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Generic configurator interface
/////////////////////////////////////////////////

#pragma once

#include "FailInfo.h"
#include <expected>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Generic interface for configuring objects from data
///
/// This interface is templated on both the data type and object type,
/// allowing type-safe configuration without type erasure.
///
/// @tparam DataType The external data format (e.g., SceneDataFbs, JSONSceneData)
/// @tparam ObjectType The native object to configure (e.g., Scene, EntityMemoryPool)
/////////////////////////////////////////////////
template<typename DataType, typename ObjectType>
class IConfigurator {
public:
  /////////////////////////////////////////////////
  /// @brief Virtual destructor
  /////////////////////////////////////////////////
  virtual ~IConfigurator() = default;
  
  /////////////////////////////////////////////////
  /// @brief Configure an existing object from data
  ///
  /// @param object Object to configure
  /// @param data Source data
  /// @return std::monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  Configure(ObjectType& object, const DataType& data) = 0;
};

} // namespace steamrot
```

### 1.3 Create Generic IDataProvider Interface

Create `src/types/interfaces/IDataProvider.h`:

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Generic data provider interface
/////////////////////////////////////////////////

#pragma once

#include "FailInfo.h"
#include <expected>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Generic interface for loading data
///
/// @tparam DataType The data type to load (e.g., SceneDataFbs)
/////////////////////////////////////////////////
template<typename DataType>
class IDataProvider {
public:
  /////////////////////////////////////////////////
  /// @brief Virtual destructor
  /////////////////////////////////////////////////
  virtual ~IDataProvider() = default;
  
  /////////////////////////////////////////////////
  /// @brief Load data from source
  ///
  /// @return Data on success, FailInfo on error
  /////////////////////////////////////////////////
  virtual std::expected<DataType, FailInfo> LoadData() = 0;
};

} // namespace steamrot
```

### 1.4 Create ConfiguratorRegistry

Create `src/data_providers/ConfiguratorRegistry.h`:

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Registry for configurator implementations
/////////////////////////////////////////////////

#pragma once

#include "DataSourceType.h"
#include "EventHandler.h"
#include "IConfigurator.h"
#include "IDataProvider.h"
#include <memory>

namespace steamrot {

// Forward declare factory
template<DataSourceType SourceType>
class ConfiguratorFactory;

/////////////////////////////////////////////////
/// @brief Registry for mapping data sources to implementations
///
/// Uses template specialization to register concrete implementations.
/// To add a new data type, create a specialization of this template.
///
/// @tparam SourceType The data source type (Flatbuffers, JSON, etc.)
/// @tparam DataType The external data format
/// @tparam ObjectType The native object type
/////////////////////////////////////////////////
template<DataSourceType SourceType, typename DataType, typename ObjectType>
struct ConfiguratorRegistry {
  // No default implementation - must specialize for each combination
  
  using ConfiguratorType = IConfigurator<DataType, ObjectType>;
  using ProviderType = IDataProvider<DataType>;
  
  // Must be specialized
  static std::unique_ptr<ConfiguratorType> 
  CreateConfigurator(EventHandler& event_handler,
                    ConfiguratorFactory<SourceType>& factory);
  
  // Must be specialized (optional - not all types need providers)
  static std::unique_ptr<ProviderType> 
  CreateDataProvider();
};

} // namespace steamrot
```

### 1.5 Create ConfiguratorFactory

Create `src/data_providers/ConfiguratorFactory.h`:

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Template-based factory for configurators
/////////////////////////////////////////////////

#pragma once

#include "ConfiguratorRegistry.h"
#include "EventHandler.h"
#include "FailInfo.h"
#include <expected>
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Factory for creating and caching configurators
///
/// Uses template parameters to determine which concrete
/// implementation to instantiate via the ConfiguratorRegistry.
///
/// @tparam SourceType The data source type (Flatbuffers, JSON, etc.)
/////////////////////////////////////////////////
template<DataSourceType SourceType>
class ConfiguratorFactory {
private:
  /////////////////////////////////////////////////
  /// @brief Reference to event handler
  /////////////////////////////////////////////////
  EventHandler& m_event_handler;
  
  /////////////////////////////////////////////////
  /// @brief Cache for configurator instances (keyed by type)
  /////////////////////////////////////////////////
  std::unordered_map<std::type_index, std::unique_ptr<void>> m_configurator_cache;
  
  /////////////////////////////////////////////////
  /// @brief Cache for data provider instances (keyed by type)
  /////////////////////////////////////////////////
  std::unordered_map<std::type_index, std::unique_ptr<void>> m_provider_cache;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor
  ///
  /// @param event_handler Reference to global event handler
  /////////////////////////////////////////////////
  ConfiguratorFactory(EventHandler& event_handler)
      : m_event_handler(event_handler) {}
  
  /////////////////////////////////////////////////
  /// @brief Get or create a configurator
  ///
  /// Returns a cached instance if available, otherwise creates
  /// a new one using the ConfiguratorRegistry.
  ///
  /// @tparam DataType External data format
  /// @tparam ObjectType Native object type
  /// @return Pointer to configurator or FailInfo on error
  /////////////////////////////////////////////////
  template<typename DataType, typename ObjectType>
  std::expected<IConfigurator<DataType, ObjectType>*, FailInfo>
  GetConfigurator() {
    using ConfiguratorType = IConfigurator<DataType, ObjectType>;
    auto type_id = std::type_index(typeid(ConfiguratorType));
    
    // Return cached instance if exists
    auto it = m_configurator_cache.find(type_id);
    if (it != m_configurator_cache.end()) {
      return static_cast<ConfiguratorType*>(it->second.get());
    }
    
    // Create new instance using registry
    auto instance = ConfiguratorRegistry<SourceType, DataType, ObjectType>
        ::CreateConfigurator(m_event_handler, *this);
    
    if (!instance) {
      return std::unexpected(FailInfo{
          FailMode::NullPointer,
          "Failed to create configurator from registry"});
    }
    
    auto* ptr = instance.get();
    m_configurator_cache[type_id] = std::move(instance);
    return ptr;
  }
  
  /////////////////////////////////////////////////
  /// @brief Get or create a data provider
  ///
  /// @tparam DataType Data type to load
  /// @return Pointer to provider or FailInfo on error
  /////////////////////////////////////////////////
  template<typename DataType>
  std::expected<IDataProvider<DataType>*, FailInfo>
  GetDataProvider() {
    using ProviderType = IDataProvider<DataType>;
    auto type_id = std::type_index(typeid(ProviderType));
    
    // Return cached instance if exists
    auto it = m_provider_cache.find(type_id);
    if (it != m_provider_cache.end()) {
      return static_cast<ProviderType*>(it->second.get());
    }
    
    // Create new instance using registry
    // Note: ObjectType doesn't matter for providers, use void
    auto instance = ConfiguratorRegistry<SourceType, DataType, void>
        ::CreateDataProvider();
    
    if (!instance) {
      return std::unexpected(FailInfo{
          FailMode::NullPointer,
          "Failed to create data provider from registry"});
    }
    
    auto* ptr = instance.get();
    m_provider_cache[type_id] = std::move(instance);
    return ptr;
  }
};

} // namespace steamrot
```

## Step 2: Create Registry Specializations

### 2.1 Scene Configurator Specialization

Create `src/scenes/FlatbuffersSceneConfiguratorRegistry.h`:

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Registry specialization for Scene configurators
/////////////////////////////////////////////////

#pragma once

#include "ConfiguratorRegistry.h"
#include "FlatbuffersSceneConfigurator.h"
#include "Scene.h"
#include "scene_data_generated.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Specialization for FlatBuffers Scene configurator
/////////////////////////////////////////////////
template<>
struct ConfiguratorRegistry<DataSourceType::Flatbuffers, 
                           SceneDataFbs, 
                           Scene> {
  using ConfiguratorType = IConfigurator<SceneDataFbs, Scene>;
  
  static std::unique_ptr<ConfiguratorType> 
  CreateConfigurator(EventHandler& event_handler,
                    ConfiguratorFactory<DataSourceType::Flatbuffers>& factory) {
    return std::make_unique<FlatbuffersSceneConfigurator>(event_handler, factory);
  }
};

} // namespace steamrot
```

### 2.2 Entity Configurator Specialization

Create `src/entity/FlatbuffersEntityConfiguratorRegistry.h`:

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Registry specialization for Entity configurators
/////////////////////////////////////////////////

#pragma once

#include "ConfiguratorRegistry.h"
#include "FlatbuffersEntityConfigurator.h"
#include "containers.h"
#include "entities_generated.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Specialization for FlatBuffers Entity configurator
/////////////////////////////////////////////////
template<>
struct ConfiguratorRegistry<DataSourceType::Flatbuffers,
                           EntityCollectionFbs,
                           EntityMemoryPool> {
  using ConfiguratorType = IConfigurator<EntityCollectionFbs, EntityMemoryPool>;
  
  static std::unique_ptr<ConfiguratorType>
  CreateConfigurator(EventHandler& event_handler,
                    ConfiguratorFactory<DataSourceType::Flatbuffers>& factory) {
    return std::make_unique<FlatbuffersEntityConfigurator>(event_handler, factory);
  }
};

} // namespace steamrot
```

## Step 3: Refactor Existing Configurators

### 3.1 Update FlatbuffersSceneConfigurator

Modify `src/scenes/FlatbuffersSceneConfigurator.h`:

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief FlatBuffers implementation of scene configurator
/////////////////////////////////////////////////

#pragma once

#include "ConfiguratorFactory.h"
#include "EventHandler.h"
#include "IConfigurator.h"
#include "Scene.h"
#include "scene_data_generated.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Configures Scene objects from FlatBuffers data
/////////////////////////////////////////////////
class FlatbuffersSceneConfigurator 
    : public IConfigurator<SceneDataFbs, Scene> {
    
private:
  EventHandler& m_event_handler;
  ConfiguratorFactory<DataSourceType::Flatbuffers>& m_factory;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor
  ///
  /// @param event_handler Reference to event handler
  /// @param factory Reference to configurator factory for nested objects
  /////////////////////////////////////////////////
  FlatbuffersSceneConfigurator(
      EventHandler& event_handler,
      ConfiguratorFactory<DataSourceType::Flatbuffers>& factory)
      : m_event_handler(event_handler), m_factory(factory) {}
  
  /////////////////////////////////////////////////
  /// @brief Configure Scene from FlatBuffers data
  ///
  /// @param scene Scene object to configure
  /// @param data FlatBuffers scene data
  /// @return std::monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  Configure(Scene& scene, const SceneDataFbs& data) override;

private:
  /////////////////////////////////////////////////
  /// @brief Configure SceneInfo from FlatBuffers
  ///
  /// Free function for nested type configuration
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(SceneInfo& info, const SceneInfoFbs* fb_info);
  
  // ... other configuration methods
};

} // namespace steamrot
```

Modify `src/scenes/FlatbuffersSceneConfigurator.cpp`:

```cpp
#include "FlatbuffersSceneConfigurator.h"
#include "containers.h"
#include "entities_generated.h"

namespace steamrot {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersSceneConfigurator::Configure(Scene& scene, 
                                       const SceneDataFbs& data) {
  
  // Configure scene info using free function
  if (data.scene_info()) {
    auto info_result = ConfigureSceneInfo(
        scene.GetSceneInfo(), 
        data.scene_info());
    if (!info_result) 
      return std::unexpected(info_result.error());
  }
  
  // Configure entities using entity configurator
  if (data.entity_collection()) {
    auto entity_configurator = 
        m_factory.GetConfigurator<EntityCollectionFbs, EntityMemoryPool>();
    
    if (!entity_configurator)
      return std::unexpected(entity_configurator.error());
    
    auto entities_result = entity_configurator.value()->Configure(
        scene.GetEntityMemoryPool(),
        *data.entity_collection());
    
    if (!entities_result)
      return std::unexpected(entities_result.error());
  }
  
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersSceneConfigurator::ConfigureSceneInfo(SceneInfo& info, 
                                                const SceneInfoFbs* fb_info) {
  if (!fb_info) {
    return std::unexpected(FailInfo{
        FailMode::FlatbuffersDataNotFound,
        "SceneInfo data is null"});
  }
  
  info.type = fb_info->type();
  // ... configure other fields
  
  return std::monostate{};
}

} // namespace steamrot
```

## Step 4: Update CMakeLists.txt

Add new files to `src/CMakeLists.txt`:

```cmake
# Add new infrastructure files
target_sources(steamrot_lib PRIVATE
  types/core/DataSourceType.h
  types/interfaces/IConfigurator.h
  types/interfaces/IDataProvider.h
  data_providers/ConfiguratorRegistry.h
  data_providers/ConfiguratorFactory.h
  
  # Registry specializations
  scenes/FlatbuffersSceneConfiguratorRegistry.h
  entity/FlatbuffersEntityConfiguratorRegistry.h
)
```

## Step 5: Usage Examples

### 5.1 Basic Usage

```cpp
#include "ConfiguratorFactory.h"
#include "FlatbuffersDataLoader.h"

void ConfigureSceneExample() {
  EventHandler event_handler;
  
  // Create factory
  ConfiguratorFactory<DataSourceType::Flatbuffers> factory(event_handler);
  
  // Load data
  FlatbuffersDataLoader loader;
  auto scene_data = loader.ProvideDefaultSceneData(SceneType::Title);
  if (!scene_data) {
    // Handle error
    return;
  }
  
  // Get configurator
  auto configurator = factory.GetConfigurator<SceneDataFbs, Scene>();
  if (!configurator) {
    // Handle error
    return;
  }
  
  // Configure scene
  Scene scene;
  auto result = configurator.value()->Configure(scene, *scene_data.value());
  if (!result) {
    // Handle error
    return;
  }
}
```

### 5.2 Replacing DataAccessFactory

In `src/engine/Engine.cpp`, replace old factory:

```cpp
// Old code:
// DataAccessFactory data_factory(m_event_handler, DataType::Flatbuffers);
// auto scene_configurator = data_factory.GetSceneConfigurator();

// New code:
ConfiguratorFactory<DataSourceType::Flatbuffers> data_factory(m_event_handler);
auto scene_configurator_result = 
    data_factory.GetConfigurator<SceneDataFbs, Scene>();

if (!scene_configurator_result) {
  // Handle error
}

auto* scene_configurator = scene_configurator_result.value();
```

## Step 6: Adding a New Data Type

### 6.1 Define New Data Structure

Create `src/types/core/JSONSceneData.h`:

```cpp
#pragma once

#include <string>
#include <vector>

namespace steamrot {

struct JSONSceneData {
  struct SceneInfo {
    std::string id;
    std::string type;
  } scene_info;
  
  // ... other fields
};

} // namespace steamrot
```

### 6.2 Create Configurator

Create `src/scenes/JSONSceneConfigurator.h`:

```cpp
#pragma once

#include "IConfigurator.h"
#include "JSONSceneData.h"
#include "Scene.h"

namespace steamrot {

class JSONSceneConfigurator 
    : public IConfigurator<JSONSceneData, Scene> {
public:
  JSONSceneConfigurator(EventHandler& event_handler)
      : m_event_handler(event_handler) {}
  
  std::expected<std::monostate, FailInfo>
  Configure(Scene& scene, const JSONSceneData& data) override;

private:
  EventHandler& m_event_handler;
};

} // namespace steamrot
```

### 6.3 Register Configurator

Create `src/scenes/JSONSceneConfiguratorRegistry.h`:

```cpp
#pragma once

#include "ConfiguratorRegistry.h"
#include "JSONSceneConfigurator.h"
#include "JSONSceneData.h"
#include "Scene.h"

namespace steamrot {

template<>
struct ConfiguratorRegistry<DataSourceType::JSON, 
                           JSONSceneData, 
                           Scene> {
  using ConfiguratorType = IConfigurator<JSONSceneData, Scene>;
  
  static std::unique_ptr<ConfiguratorType> 
  CreateConfigurator(EventHandler& event_handler,
                    ConfiguratorFactory<DataSourceType::JSON>& factory) {
    return std::make_unique<JSONSceneConfigurator>(event_handler);
  }
};

} // namespace steamrot
```

### 6.4 Use New Data Type

```cpp
// Just change the template parameter!
ConfiguratorFactory<DataSourceType::JSON> factory(event_handler);

auto configurator = factory.GetConfigurator<JSONSceneData, Scene>();
Scene scene;
configurator.value()->Configure(scene, json_data);
```

## Step 7: Testing

### 7.1 Create Test for ConfiguratorFactory

Create `tests/unit/data_providers/ConfiguratorFactory.test.cpp`:

```cpp
#include "ConfiguratorFactory.h"
#include "FlatbuffersSceneConfiguratorRegistry.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConfiguratorFactory creates scene configurator", 
          "[unit][ConfiguratorFactory]") {
  steamrot::EventHandler event_handler;
  steamrot::ConfiguratorFactory<steamrot::DataSourceType::Flatbuffers> 
      factory(event_handler);
  
  auto configurator = factory.GetConfigurator<
      steamrot::SceneDataFbs, steamrot::Scene>();
  
  REQUIRE(configurator.has_value());
  REQUIRE(configurator.value() != nullptr);
}

TEST_CASE("ConfiguratorFactory caches configurators", 
          "[unit][ConfiguratorFactory]") {
  steamrot::EventHandler event_handler;
  steamrot::ConfiguratorFactory<steamrot::DataSourceType::Flatbuffers> 
      factory(event_handler);
  
  auto config1 = factory.GetConfigurator<
      steamrot::SceneDataFbs, steamrot::Scene>();
  auto config2 = factory.GetConfigurator<
      steamrot::SceneDataFbs, steamrot::Scene>();
  
  // Should return same instance
  REQUIRE(config1.value() == config2.value());
}
```

## Summary

This implementation guide provides:
1. ✅ Template infrastructure for type-safe configuration
2. ✅ Registry pattern for compile-time dispatch
3. ✅ Factory for caching and dependency injection
4. ✅ Refactoring guide for existing code
5. ✅ Extension guide for new data types
6. ✅ Testing approach

The pattern eliminates:
- ❌ Switch statements in factories
- ❌ Type erasure
- ❌ Scattered coupling
- ❌ Code duplication

And provides:
- ✅ Compile-time type safety
- ✅ Easy extensibility
- ✅ Clear coupling points
- ✅ Code reusability
