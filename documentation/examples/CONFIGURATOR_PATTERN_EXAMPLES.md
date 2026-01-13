# Configurator Pattern - Concrete Examples

This document provides concrete, working examples of the proposed generalized configurator pattern.

## Example 1: Basic Template-Based Configurator

### Interface Definition

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Generic configurator interface
/////////////////////////////////////////////////

#pragma once

#include "FailInfo.h"
#include <expected>
#include <memory>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Generic interface for configuring objects from data
///
/// @tparam DataType The external data format (e.g., SceneDataFbs, JSONSceneData)
/// @tparam ObjectType The native object to configure (e.g., Scene, Entity)
/////////////////////////////////////////////////
template<typename DataType, typename ObjectType>
class IConfigurator {
public:
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

### Concrete Implementation

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief FlatBuffers implementation of scene configurator
/////////////////////////////////////////////////

#pragma once

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

public:
  FlatbuffersSceneConfigurator(EventHandler& event_handler)
      : m_event_handler(event_handler) {}
  
  /////////////////////////////////////////////////
  /// @brief Configure Scene from FlatBuffers data
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  Configure(Scene& scene, const SceneDataFbs& data) override {
    
    // Use free function for SceneInfo (nested type)
    if (data.scene_info()) {
      auto info_result = ConfigureSceneInfo(
          scene.GetSceneInfo(), 
          data.scene_info());
      if (!info_result) 
        return std::unexpected(info_result.error());
    }
    
    // Use free function for SceneResources (nested type)
    if (data.scene_resources()) {
      auto resources_result = ConfigureSceneResources(
          scene.GetSceneResources(),
          data.scene_resources());
      if (!resources_result)
        return std::unexpected(resources_result.error());
    }
    
    return std::monostate{};
  }
  
private:
  /////////////////////////////////////////////////
  /// @brief Free function to configure SceneInfo from FlatBuffers
  /////////////////////////////////////////////////
  static std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(SceneInfo& info, const SceneInfoFbs* fb_info) {
    if (!fb_info) {
      return std::unexpected(FailInfo{
          FailMode::FlatbuffersDataNotFound,
          "SceneInfo data is null"});
    }
    
    // Configure fields
    info.type = fb_info->type();
    // ... other fields
    
    return std::monostate{};
  }
  
  /////////////////////////////////////////////////
  /// @brief Free function to configure SceneResources from FlatBuffers
  /////////////////////////////////////////////////
  static std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(SceneResources& resources, 
                         const SceneResourcesFbs* fb_resources) {
    if (!fb_resources) {
      return std::unexpected(FailInfo{
          FailMode::FlatbuffersDataNotFound,
          "SceneResources data is null"});
    }
    
    // Configure resources
    // ...
    
    return std::monostate{};
  }
};

} // namespace steamrot
```

## Example 2: Configurator Registry

### Registry Infrastructure

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Registry for configurator implementations
/////////////////////////////////////////////////

#pragma once

#include "DataSourceType.h"
#include "EventHandler.h"
#include <memory>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Registry for mapping data sources to configurators
///
/// Uses template specialization to register concrete implementations.
/// Add new data types by creating specializations.
///
/// @tparam SourceType The data source type (Flatbuffers, JSON, etc.)
/// @tparam DataType The external data format
/// @tparam ObjectType The native object type
/////////////////////////////////////////////////
template<DataSourceType SourceType, typename DataType, typename ObjectType>
struct ConfiguratorRegistry;

// No default implementation - must specialize

/////////////////////////////////////////////////
/// @brief Specialization for FlatBuffers Scene configurator
/////////////////////////////////////////////////
template<>
struct ConfiguratorRegistry<DataSourceType::Flatbuffers, 
                           SceneDataFbs, 
                           Scene> {
  using ConfiguratorType = IConfigurator<SceneDataFbs, Scene>;
  
  static std::unique_ptr<ConfiguratorType> 
  Create(EventHandler& event_handler) {
    return std::make_unique<FlatbuffersSceneConfigurator>(event_handler);
  }
};

/////////////////////////////////////////////////
/// @brief Specialization for FlatBuffers Entity configurator
/////////////////////////////////////////////////
template<>
struct ConfiguratorRegistry<DataSourceType::Flatbuffers,
                           EntityCollectionFbs,
                           EntityMemoryPool> {
  using ConfiguratorType = IConfigurator<EntityCollectionFbs, EntityMemoryPool>;
  
  static std::unique_ptr<ConfiguratorType>
  Create(EventHandler& event_handler) {
    return std::make_unique<FlatbuffersEntityConfigurator>(event_handler);
  }
};

// Add more specializations as needed...

} // namespace steamrot
```

### Factory Using Registry

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Template-based factory for configurators
/////////////////////////////////////////////////

#pragma once

#include "ConfiguratorRegistry.h"
#include "FailInfo.h"
#include <typeindex>
#include <unordered_map>
#include <expected>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Factory for creating and caching configurators
///
/// Uses template parameters to determine which concrete
/// implementation to instantiate. No switch statements needed.
///
/// @tparam SourceType The data source type (Flatbuffers, JSON, etc.)
/////////////////////////////////////////////////
template<DataSourceType SourceType>
class ConfiguratorFactory {
private:
  EventHandler& m_event_handler;
  
  // Cache for configurator instances (keyed by type)
  std::unordered_map<std::type_index, std::unique_ptr<void>> m_cache;

public:
  ConfiguratorFactory(EventHandler& event_handler)
      : m_event_handler(event_handler) {}
  
  /////////////////////////////////////////////////
  /// @brief Get or create a configurator
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
    auto it = m_cache.find(type_id);
    if (it != m_cache.end()) {
      return static_cast<ConfiguratorType*>(it->second.get());
    }
    
    // Create new instance using registry
    auto instance = ConfiguratorRegistry<SourceType, DataType, ObjectType>
        ::Create(m_event_handler);
    
    if (!instance) {
      return std::unexpected(FailInfo{
          FailMode::NullPointer,
          "Failed to create configurator from registry"});
    }
    
    auto* ptr = instance.get();
    m_cache[type_id] = std::move(instance);
    return ptr;
  }
};

} // namespace steamrot
```

## Example 3: Usage Pattern

### Basic Usage

```cpp
#include "ConfiguratorFactory.h"
#include "FlatbuffersDataLoader.h"
#include "Scene.h"

namespace steamrot {

void ConfigureSceneExample() {
  EventHandler event_handler;
  
  // Create factory for FlatBuffers data
  ConfiguratorFactory<DataSourceType::Flatbuffers> factory(event_handler);
  
  // Get scene configurator (created and cached)
  auto configurator_result = factory.GetConfigurator<SceneDataFbs, Scene>();
  if (!configurator_result) {
    // Handle error
    return;
  }
  
  auto* configurator = configurator_result.value();
  
  // Load data
  FlatbuffersDataLoader loader;
  auto data_result = loader.ProvideDefaultSceneData(SceneType::Title);
  if (!data_result) {
    // Handle error
    return;
  }
  
  // Configure scene
  Scene scene;
  auto config_result = configurator->Configure(scene, *data_result.value());
  if (!config_result) {
    // Handle error
    return;
  }
  
  // Scene is now configured
}

} // namespace steamrot
```

### Composite Configuration (Object A contains Object B)

```cpp
/////////////////////////////////////////////////
/// @brief Configurator for Scene that contains Entities
/////////////////////////////////////////////////
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
    
    // Configure scene info
    auto info_result = ConfigureSceneInfo(
        scene.GetSceneInfo(), 
        data.scene_info());
    if (!info_result) 
      return std::unexpected(info_result.error());
    
    // Scene contains EntityMemoryPool - use its configurator
    if (data.entity_collection()) {
      // Get entity configurator from factory
      auto entity_configurator = 
          m_factory.GetConfigurator<EntityCollectionFbs, EntityMemoryPool>();
      
      if (!entity_configurator) 
        return std::unexpected(entity_configurator.error());
      
      // Configure entities using their configurator
      auto entities_result = entity_configurator.value()->Configure(
          scene.GetEntityMemoryPool(),
          *data.entity_collection());
      
      if (!entities_result)
        return std::unexpected(entities_result.error());
    }
    
    return std::monostate{};
  }
};
```

## Example 4: Adding a New Data Type (JSON)

### Step 1: Define JSON Data Structure

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief JSON data structure for scenes
/////////////////////////////////////////////////

#pragma once

#include <string>
#include <vector>

namespace steamrot {

struct JSONSceneData {
  struct SceneInfo {
    std::string id;
    std::string type;
  } scene_info;
  
  struct SceneResources {
    std::vector<std::string> textures;
    std::vector<std::string> fonts;
  } scene_resources;
  
  // ... other fields
};

} // namespace steamrot
```

### Step 2: Create JSON Configurator

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief JSON implementation of scene configurator
/////////////////////////////////////////////////

#pragma once

#include "IConfigurator.h"
#include "Scene.h"
#include "JSONSceneData.h"

namespace steamrot {

class JSONSceneConfigurator 
    : public IConfigurator<JSONSceneData, Scene> {
    
private:
  EventHandler& m_event_handler;

public:
  JSONSceneConfigurator(EventHandler& event_handler)
      : m_event_handler(event_handler) {}
  
  std::expected<std::monostate, FailInfo>
  Configure(Scene& scene, const JSONSceneData& data) override {
    // Convert JSON data to Scene
    auto info_result = ConfigureSceneInfoFromJSON(
        scene.GetSceneInfo(), 
        data.scene_info);
    if (!info_result) 
      return std::unexpected(info_result.error());
    
    // ... configure other fields
    
    return std::monostate{};
  }
  
private:
  static std::expected<std::monostate, FailInfo>
  ConfigureSceneInfoFromJSON(SceneInfo& info, 
                             const JSONSceneData::SceneInfo& json_info) {
    // Convert JSON strings to native types
    info.type = ConvertSceneType(json_info.type);
    // ... other conversions
    
    return std::monostate{};
  }
};

} // namespace steamrot
```

### Step 3: Register in ConfiguratorRegistry

```cpp
/////////////////////////////////////////////////
/// @brief Specialization for JSON Scene configurator
/////////////////////////////////////////////////
template<>
struct ConfiguratorRegistry<DataSourceType::JSON, 
                           JSONSceneData, 
                           Scene> {
  using ConfiguratorType = IConfigurator<JSONSceneData, Scene>;
  
  static std::unique_ptr<ConfiguratorType> 
  Create(EventHandler& event_handler) {
    return std::make_unique<JSONSceneConfigurator>(event_handler);
  }
};
```

### Step 4: Use JSON Configurator

```cpp
void ConfigureSceneFromJSON() {
  EventHandler event_handler;
  
  // Create factory for JSON data (just change template parameter!)
  ConfiguratorFactory<DataSourceType::JSON> factory(event_handler);
  
  // Get JSON scene configurator
  auto configurator = factory.GetConfigurator<JSONSceneData, Scene>();
  
  // Load JSON data
  JSONDataLoader loader;
  auto data = loader.LoadSceneData("scene.json");
  
  // Configure scene - same pattern as FlatBuffers
  Scene scene;
  configurator.value()->Configure(scene, data.value());
}
```

**No changes to existing code required!** The pattern handles the new data type through template specialization.

## Example 5: Free Functions for Reusability

### Scenario: Multiple Configurators Need Same Logic

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Reusable free functions for scene configuration
/////////////////////////////////////////////////

#pragma once

#include "SceneInfo.h"
#include "FailInfo.h"
#include <expected>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Configure SceneInfo from FlatBuffers data
///
/// Free function can be called from any configurator
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureSceneInfoFromFlatbuffers(SceneInfo& info, 
                                  const SceneInfoFbs* fb_info) {
  if (!fb_info) {
    return std::unexpected(FailInfo{
        FailMode::FlatbuffersDataNotFound,
        "SceneInfo data is null"});
  }
  
  info.type = fb_info->type();
  // ... other fields
  
  return std::monostate{};
}

/////////////////////////////////////////////////
/// @brief Configure SceneInfo from JSON data
///
/// Different free function for different data source
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureSceneInfoFromJSON(SceneInfo& info,
                           const std::string& json_type) {
  info.type = ConvertSceneType(json_type);
  // ... other fields
  
  return std::monostate{};
}

} // namespace steamrot
```

### Usage in Multiple Configurators

```cpp
// FlatBuffers configurator uses FlatBuffers free function
class FlatbuffersSceneConfigurator {
  std::expected<std::monostate, FailInfo>
  Configure(Scene& scene, const SceneDataFbs& data) override {
    return ConfigureSceneInfoFromFlatbuffers(
        scene.GetSceneInfo(), 
        data.scene_info());
  }
};

// JSON configurator uses JSON free function
class JSONSceneConfigurator {
  std::expected<std::monostate, FailInfo>
  Configure(Scene& scene, const JSONSceneData& data) override {
    return ConfigureSceneInfoFromJSON(
        scene.GetSceneInfo(),
        data.scene_info.type);
  }
};

// Third-party configurator can reuse same functions
class ThirdPartyConfigurator {
  std::expected<std::monostate, FailInfo>
  Configure(Scene& scene, const ThirdPartyData& data) override {
    // Can call either free function depending on data format
    if (data.is_flatbuffers) {
      return ConfigureSceneInfoFromFlatbuffers(...);
    } else {
      return ConfigureSceneInfoFromJSON(...);
    }
  }
};
```

## Key Takeaways

1. **Templates eliminate switch statements**: Type dispatch happens at compile time
2. **Registry confines coupling**: Only specializations know about concrete types
3. **Free functions promote reuse**: Stateless functions callable from anywhere
4. **Composition is natural**: Configurators easily call other configurators
5. **Extension is trivial**: Add new data type with one specialization
6. **Type safety is guaranteed**: No type erasure, full compile-time checking

## Comparison with Current Code

| Current Pattern | Proposed Pattern |
|----------------|------------------|
| `DataAccessFactory` has switch statement | `ConfiguratorRegistry` uses template specialization |
| Interface tied to one data type | Interface generic over data type |
| Mixed stateful/stateless logic | Pure stateless functions |
| Hard to add new data types | Add specialization, done |
| Coupling scattered | Coupling confined to registry |

## Migration Example

### Before (Current Code)

```cpp
// DataAccessFactory.cpp
std::expected<std::monostate, FailInfo> 
DataAccessFactory::SetFlatbuffersDataProviders() {
  m_scene_configurator = std::make_unique<FlatbuffersSceneConfigurator>();
  // More switch cases for other types...
  return std::monostate{};
}
```

### After (Proposed Pattern)

```cpp
// ConfiguratorRegistry.h
template<>
struct ConfiguratorRegistry<DataSourceType::Flatbuffers, 
                           SceneDataFbs, Scene> {
  static auto Create(EventHandler& eh) {
    return std::make_unique<FlatbuffersSceneConfigurator>(eh);
  }
};

// Usage
auto factory = ConfiguratorFactory<DataSourceType::Flatbuffers>(event_handler);
auto configurator = factory.GetConfigurator<SceneDataFbs, Scene>();
```

The new pattern is:
- More explicit (see exactly what's registered)
- More extensible (add specialization without modifying factory)
- More type-safe (compile-time dispatch)
- Less coupled (no switch statement in factory)
