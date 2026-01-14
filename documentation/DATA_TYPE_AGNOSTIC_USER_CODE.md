# Making User Code Data Type Agnostic

This document addresses how to make high-level user code (like SceneManager, SceneFactory) data type agnostic while still supporting a data type switching mechanism at the application level.

## Problem Statement

**User's Need**: In SceneManager, when calling `LoadTitleScene()` or `AddSceneFromDefault(SceneType)`, the code should not need to specify which data type (FlatBuffers, JSON, etc.) is being used. The data source should be configurable at a higher level.

**Current Situation**: Code explicitly uses `FlatbuffersSceneConfigurator` and knows about concrete data types.

**Desired Behavior**: 
```cpp
// SceneManager code - data type agnostic
auto scene = CreateSceneFromDefault(SceneType::Title);
// ^^^ Doesn't know if it's using FlatBuffers, JSON, or something else
```

## Solution: Application-Level Data Source Configuration

The key is to **configure the data source once at application startup**, then have all code use that configuration without knowing the specific type.

### Architecture Overview

```
Application Startup (main.cpp)
    ↓
Configure DataSourceType::Flatbuffers (or JSON, etc.)
    ↓
Create ConfiguratorFactory<Flatbuffers>
    ↓
Pass to GameContext
    ↓
User Code (SceneManager, SceneFactory)
    ↓
Uses generic interface - doesn't know concrete type
```

## Implementation

### Step 1: Type-Erased Facade Over Template Factory

Create a non-template facade that hides the template parameter from user code:

```cpp
/////////////////////////////////////////////////
/// @file ConfiguratorFacade.h
/// @brief Type-erased facade over template-based configurator factory
/////////////////////////////////////////////////

#pragma once

#include "EngineSnapshot.h"
#include "FailInfo.h"
#include "Scene.h"
#include "SceneData.h"
#include "SceneManagerData.h"
#include <expected>
#include <memory>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Type-erased interface for configuration operations
///
/// This facade hides the template-based implementation from user code.
/// User code only sees this interface - never knows about concrete
/// data types like FlatBuffers or JSON.
///
/// The concrete implementation is selected at application startup
/// via DataSourceType and remains fixed for the application lifetime.
/////////////////////////////////////////////////
class ConfiguratorFacade {
public:
  virtual ~ConfiguratorFacade() = default;

  /////////////////////////////////////////////////
  /// @brief Load and configure a Scene from default data
  ///
  /// @param scene_type The type of scene to load
  /// @return Configured SceneData or FailInfo on error
  /////////////////////////////////////////////////
  virtual std::expected<SceneData, FailInfo>
  LoadDefaultSceneData(const SceneType scene_type) = 0;

  /////////////////////////////////////////////////
  /// @brief Configure a Scene object from SceneData
  ///
  /// @param scene Scene object to configure
  /// @param scene_data Data to configure from
  /// @return std::monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene& scene, const SceneData& scene_data) = 0;

  /////////////////////////////////////////////////
  /// @brief Configure EngineSnapshot from data
  ///
  /// @param snapshot EngineSnapshot to configure
  /// @param source Variant holding the data source
  /// @return std::monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureEngineSnapshot(EngineSnapshot& snapshot, 
                         const void* source_data) = 0;

  /////////////////////////////////////////////////
  /// @brief Load SceneManagerData from defaults
  ///
  /// @return SceneManagerData or FailInfo on error
  /////////////////////////////////////////////////
  virtual std::expected<SceneManagerData, FailInfo>
  LoadDefaultSceneManagerData() = 0;

  // Add other high-level operations as needed...
};

} // namespace steamrot
```

### Step 2: Template Implementation for Each Data Source

```cpp
/////////////////////////////////////////////////
/// @file FlatbuffersConfiguratorFacade.h
/// @brief FlatBuffers implementation of ConfiguratorFacade
/////////////////////////////////////////////////

#pragma once

#include "ConfiguratorFacade.h"
#include "ConfiguratorFactory.h"
#include "DataSourceType.h"
#include "EventHandler.h"
#include "FlatbuffersDataLoader.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @brief FlatBuffers implementation of ConfiguratorFacade
///
/// This is the ONLY place that knows we're using FlatBuffers.
/// User code only sees ConfiguratorFacade interface.
/////////////////////////////////////////////////
class FlatbuffersConfiguratorFacade : public ConfiguratorFacade {
private:
  EventHandler& m_event_handler;
  ConfiguratorFactory<DataSourceType::Flatbuffers> m_factory;
  FlatbuffersDataLoader m_data_loader;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor
  ///
  /// @param event_handler Reference to event handler
  /////////////////////////////////////////////////
  FlatbuffersConfiguratorFacade(EventHandler& event_handler)
      : m_event_handler(event_handler),
        m_factory(event_handler),
        m_data_loader() {}

  /////////////////////////////////////////////////
  /// @brief Load and configure Scene from FlatBuffers default data
  /////////////////////////////////////////////////
  std::expected<SceneData, FailInfo>
  LoadDefaultSceneData(const SceneType scene_type) override {
    
    // Load FlatBuffers data
    auto fb_data_result = m_data_loader.ProvideDefaultSceneData(scene_type);
    if (!fb_data_result)
      return std::unexpected(fb_data_result.error());

    const SceneDataFbs* fb_data = fb_data_result.value();

    // Get data provider configurator
    auto provider_result = m_factory.GetDataProvider<SceneDataFbs>();
    if (!provider_result)
      return std::unexpected(provider_result.error());

    // Convert FlatBuffers to agnostic SceneData
    auto scene_data_result = 
        provider_result.value()->ProvideSceneDataFromData(fb_data);
    if (!scene_data_result)
      return std::unexpected(scene_data_result.error());

    return scene_data_result.value();
  }

  /////////////////////////////////////////////////
  /// @brief Configure Scene from SceneData (data type agnostic)
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene& scene, const SceneData& scene_data) override {
    
    // SceneData is already agnostic - just use generic configurator
    // The scene configurator works with SceneData, not FlatBuffers
    
    // For now, we can use existing ISceneConfigurator pattern
    // which already works with agnostic SceneData
    auto configurator_result = m_factory.GetConfigurator<
        SceneDataFbs, Scene>(); // Note: This would be refactored
    
    if (!configurator_result)
      return std::unexpected(configurator_result.error());
    
    // This needs adjustment - see "Complete Solution" section below
    return std::monostate{};
  }

  /////////////////////////////////////////////////
  /// @brief Configure EngineSnapshot (handles type internally)
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureEngineSnapshot(EngineSnapshot& snapshot,
                         const void* source_data) override {
    
    // Cast to FlatBuffers type (facade knows the concrete type)
    const EngineSnapshotFbs* fb_data = 
        static_cast<const EngineSnapshotFbs*>(source_data);

    // Get configurator
    auto config_result = m_factory.GetConfigurator<
        EngineSnapshotFbs, EngineSnapshot>();
    
    if (!config_result)
      return std::unexpected(config_result.error());

    // Configure
    return config_result.value()->Configure(snapshot, *fb_data);
  }

  /////////////////////////////////////////////////
  /// @brief Load SceneManagerData from FlatBuffers defaults
  /////////////////////////////////////////////////
  std::expected<SceneManagerData, FailInfo>
  LoadDefaultSceneManagerData() override {
    
    // Load FlatBuffers data
    auto fb_data_result = m_data_loader.ProvideSceneManagerData();
    if (!fb_data_result)
      return std::unexpected(fb_data_result.error());

    // Get provider
    auto provider_result = m_factory.GetDataProvider<SceneManagerDataFbs>();
    if (!provider_result)
      return std::unexpected(provider_result.error());

    // Convert to agnostic type
    return provider_result.value()->ProvideDataFromFlatbuffers(
        fb_data_result.value());
  }
};

} // namespace steamrot
```

### Step 3: Application-Level Data Source Selection

```cpp
/////////////////////////////////////////////////
/// @file main.cpp or Engine.cpp
/// @brief Application entry point - configure data source here
/////////////////////////////////////////////////

#include "ConfiguratorFacade.h"
#include "DataSourceType.h"
#include "FlatbuffersConfiguratorFacade.h"
// #include "JSONConfiguratorFacade.h"  // Future

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Factory function to create appropriate facade
///
/// This is the ONLY place in user-facing code that knows about
/// the concrete data type. Everything else uses ConfiguratorFacade.
/////////////////////////////////////////////////
std::unique_ptr<ConfiguratorFacade>
CreateConfiguratorFacade(DataSourceType source_type,
                        EventHandler& event_handler) {
  
  switch (source_type) {
  case DataSourceType::Flatbuffers:
    return std::make_unique<FlatbuffersConfiguratorFacade>(event_handler);
  
  // Future data sources:
  // case DataSourceType::JSON:
  //   return std::make_unique<JSONConfiguratorFacade>(event_handler);
  
  default:
    throw std::runtime_error("Unsupported data source type");
  }
}

} // namespace steamrot

/////////////////////////////////////////////////
// Application startup
/////////////////////////////////////////////////
int main() {
  using namespace steamrot;

  EventHandler event_handler;

  // *** ONLY PLACE WE SPECIFY DATA TYPE ***
  DataSourceType data_source = DataSourceType::Flatbuffers;
  
  // Create facade (type-erased)
  auto configurator_facade = CreateConfiguratorFacade(
      data_source, event_handler);

  // Pass facade to game context
  GameContext game_context{
      .event_handler = event_handler,
      .configurator_facade = configurator_facade.get()
      // ... other context members
  };

  // Rest of application uses generic interface
  Engine engine(game_context);
  engine.Run();

  return 0;
}
```

### Step 4: Update GameContext

```cpp
/////////////////////////////////////////////////
/// @file GameContext.h
/// @brief Game context with generic configurator facade
/////////////////////////////////////////////////

#pragma once

#include "ConfiguratorFacade.h"
#include "EventHandler.h"
// ... other includes

namespace steamrot {

struct GameContext {
  EventHandler& event_handler;
  
  /////////////////////////////////////////////////
  /// @brief Type-erased configurator facade
  ///
  /// User code uses this without knowing concrete data type
  /////////////////////////////////////////////////
  ConfiguratorFacade* configurator_facade;
  
  // ... other context members
};

} // namespace steamrot
```

### Step 5: User Code (Data Type Agnostic)

```cpp
/////////////////////////////////////////////////
/// @file SceneFactory.cpp
/// @brief SceneFactory - completely data type agnostic
/////////////////////////////////////////////////

#include "SceneFactory.h"

namespace steamrot {

/////////////////////////////////////////////////
std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateSceneFromDefault(SceneType scene_type) {

  // Step 1: Load data using generic facade
  // *** NO KNOWLEDGE OF FLATBUFFERS, JSON, ETC. ***
  auto scene_data_result = 
      m_game_context.configurator_facade->LoadDefaultSceneData(scene_type);
  
  if (!scene_data_result)
    return std::unexpected(scene_data_result.error());

  SceneData scene_data = scene_data_result.value();

  // Step 2: Create empty scene (same as before)
  auto scene_result = CreateEmptyScene(scene_data.scene_info.type);
  if (!scene_result)
    return std::unexpected(scene_result.error());

  std::unique_ptr<Scene> scene = std::move(scene_result.value());

  // Step 3: Configure scene using generic facade
  // *** NO KNOWLEDGE OF CONCRETE DATA TYPE ***
  auto config_result = 
      m_game_context.configurator_facade->ConfigureScene(*scene, scene_data);
  
  if (!config_result)
    return std::unexpected(config_result.error());

  return scene;
}

} // namespace steamrot
```

```cpp
/////////////////////////////////////////////////
/// @file SceneManager.cpp
/// @brief SceneManager - completely data type agnostic
/////////////////////////////////////////////////

#include "SceneManager.h"
#include "SceneFactory.h"

namespace steamrot {

/////////////////////////////////////////////////
std::expected<uuids::uuid, FailInfo> SceneManager::LoadTitleScene() {
  
  // *** NO KNOWLEDGE OF DATA TYPE ***
  // Just pass SceneType - facade handles the rest
  return AddSceneFromDefault(SceneType::SceneType_TITLE);
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SceneManager::AddSceneFromDefault(const SceneType& scene_type) {
  
  // Create factory (agnostic)
  SceneFactory factory(m_game_context);
  
  // Create scene from default (agnostic)
  // *** DOESN'T KNOW IF IT'S FLATBUFFERS OR JSON ***
  auto scene_result = factory.CreateSceneFromDefault(scene_type);
  
  if (!scene_result)
    return std::unexpected(scene_result.error());

  // Add scene to collection
  auto scene = std::move(scene_result.value());
  uuids::uuid scene_id = scene->GetSceneInfo().id;
  m_scenes[scene_id] = std::move(scene);

  return std::monostate{};
}

} // namespace steamrot
```

## Complete Architecture Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                    Application Startup                       │
│                                                              │
│  DataSourceType source = Flatbuffers;  ◄─── ONLY PLACE     │
│  auto facade = CreateConfiguratorFacade(source, eh);       │
│                                          ◄─── THAT KNOWS    │
│  GameContext ctx{.facade = facade.get()};   DATA TYPE      │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    User Code Layer                           │
│                  (Data Type Agnostic)                        │
│                                                              │
│  SceneManager::LoadTitleScene() {                           │
│    AddSceneFromDefault(SceneType::Title);                   │
│  }                                                           │
│                                                              │
│  SceneFactory::CreateSceneFromDefault(SceneType type) {     │
│    auto data = ctx.facade->LoadDefaultSceneData(type);      │
│    //           ^^^^^^^^^^^                                 │
│    //           Generic interface - no data type knowledge  │
│                                                              │
│    auto scene = CreateEmptyScene(type);                     │
│    ctx.facade->ConfigureScene(*scene, data);                │
│    return scene;                                             │
│  }                                                           │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│              ConfiguratorFacade Interface                    │
│              (Type-Erased Generic API)                       │
│                                                              │
│  virtual LoadDefaultSceneData(SceneType) = 0;               │
│  virtual ConfigureScene(Scene&, SceneData&) = 0;            │
│  virtual ConfigureEngineSnapshot(...) = 0;                  │
│  ...                                                         │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│        FlatbuffersConfiguratorFacade Implementation          │
│                 (Knows about FlatBuffers)                    │
│                                                              │
│  ConfiguratorFactory<Flatbuffers> m_factory;                │
│  FlatbuffersDataLoader m_data_loader;                       │
│                                                              │
│  LoadDefaultSceneData(type) {                               │
│    fb_data = m_data_loader.Load(type);    ◄─── FlatBuffers │
│    provider = m_factory.GetProvider();    ◄─── Template    │
│    return provider->ConvertToSceneData(fb_data);            │
│  }                                                           │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│         Template-Based Configurator Infrastructure           │
│                                                              │
│  ConfiguratorFactory<DataSourceType>                        │
│  ConfiguratorRegistry<Source, Data, Object>                 │
│  IConfigurator<DataType, ObjectType>                        │
│  Concrete Configurators                                      │
└─────────────────────────────────────────────────────────────┘
```

## Key Design Points

### 1. One Switch Statement is OK

The facade creation function has a switch on DataSourceType:

```cpp
std::unique_ptr<ConfiguratorFacade>
CreateConfiguratorFacade(DataSourceType source_type, EventHandler& eh) {
  switch (source_type) {
  case DataSourceType::Flatbuffers:
    return std::make_unique<FlatbuffersConfiguratorFacade>(eh);
  case DataSourceType::JSON:
    return std::make_unique<JSONConfiguratorFacade>(eh);
  // ...
  }
}
```

**This is acceptable because:**
- It happens **once** at application startup
- It's in **one place** (not scattered)
- User code never sees it
- Adding a data source requires one line here

### 2. Type Erasure at the Right Level

We use type erasure (ConfiguratorFacade interface) at the **application boundary**, but maintain compile-time type safety inside the implementation.

```
User Code
    ↓ (uses generic interface)
ConfiguratorFacade (type-erased)
    ↓ (polymorphism)
FlatbuffersConfiguratorFacade (knows concrete type)
    ↓ (uses templates)
ConfiguratorFactory<Flatbuffers> (compile-time dispatch)
    ↓
Template-based configurators (full type safety)
```

### 3. Data Flow

```
User: "Load title scene"
    ↓
Facade: LoadDefaultSceneData(SceneType::Title)
    ↓
FlatbuffersFacade: Load FlatBuffers → Convert to SceneData
    ↓
User: Receives SceneData (agnostic type)
    ↓
User: Configure scene
    ↓
Facade: ConfigureScene(scene, scene_data)
    ↓
FlatbuffersFacade: Use template configurators internally
    ↓
Scene is configured (user never knew about FlatBuffers)
```

### 4. Agnostic Data Types

Some types like `SceneData`, `EngineSnapshot`, `SceneManagerData` are **already agnostic**:

```cpp
// These structs don't know about FlatBuffers or JSON
struct SceneData {
  SceneInfo scene_info;
  SceneResourcesConfig scene_resources_config;
  AssetConfig scene_asset_config;
  // ...
};
```

The configurator's job is to convert from **concrete data format** (FlatBuffers, JSON) to **agnostic native types**.

## Example: Adding JSON Support

### Step 1: Create JSON Facade

```cpp
class JSONConfiguratorFacade : public ConfiguratorFacade {
private:
  ConfiguratorFactory<DataSourceType::JSON> m_factory;
  JSONDataLoader m_data_loader;

public:
  std::expected<SceneData, FailInfo>
  LoadDefaultSceneData(const SceneType scene_type) override {
    // Load JSON data
    auto json_data = m_data_loader.LoadSceneJSON(scene_type);
    
    // Convert to SceneData
    auto provider = m_factory.GetDataProvider<JSONSceneData>();
    return provider->ConvertToSceneData(json_data);
  }
  // ... other methods
};
```

### Step 2: Update Factory Function

```cpp
std::unique_ptr<ConfiguratorFacade>
CreateConfiguratorFacade(DataSourceType source_type, EventHandler& eh) {
  switch (source_type) {
  case DataSourceType::Flatbuffers:
    return std::make_unique<FlatbuffersConfiguratorFacade>(eh);
  
  case DataSourceType::JSON:  // ← Add this
    return std::make_unique<JSONConfiguratorFacade>(eh);
  
  default:
    throw std::runtime_error("Unsupported data source type");
  }
}
```

### Step 3: Use JSON

```cpp
// main.cpp - change one line
DataSourceType data_source = DataSourceType::JSON;  // Was: Flatbuffers

// Everything else works unchanged!
// SceneManager, SceneFactory, etc. don't need any changes
```

**Zero changes to user code!**

## Benefits

### 1. User Code is Completely Agnostic

```cpp
// SceneManager never mentions FlatBuffers or JSON
auto scene = CreateSceneFromDefault(SceneType::Title);
```

### 2. Data Source Configured Once

```cpp
// Application startup - set once
DataSourceType source = DataSourceType::Flatbuffers;
auto facade = CreateConfiguratorFacade(source, event_handler);

// Never specified again
```

### 3. Easy to Switch Data Sources

```cpp
// Development: Use FlatBuffers
DataSourceType source = DataSourceType::Flatbuffers;

// Testing: Use JSON for easier debugging
DataSourceType source = DataSourceType::JSON;

// Production: Back to FlatBuffers
DataSourceType source = DataSourceType::Flatbuffers;
```

### 4. Compile-Time Safety Preserved

Inside the facade implementation, we still have full template-based type safety:

```cpp
class FlatbuffersConfiguratorFacade {
  ConfiguratorFactory<DataSourceType::Flatbuffers> m_factory;
  //                  ^^^^^^^^^^^^^^^^^^^^^^^^^^
  //                  Compile-time type parameter
  
  auto config = m_factory.GetConfigurator<SceneDataFbs, Scene>();
  //                                      ^^^^^^^^^^^^  ^^^^^
  //                                      Full type information
};
```

### 5. One Acceptable Switch

The switch in `CreateConfiguratorFacade` is acceptable because:
- It's in **one place**
- It runs **once** at startup
- It's **hidden** from user code
- Adding a source requires **one line**

## Summary

**Question**: How to make user code data type agnostic?

**Answer**: Use a **type-erased facade pattern** at the application boundary:

1. **ConfiguratorFacade** - Generic interface (no templates, no data type knowledge)
2. **FlatbuffersConfiguratorFacade** - Concrete implementation (knows about FlatBuffers)
3. **CreateConfiguratorFacade()** - One factory function with acceptable switch
4. **Application startup** - Configure data source once, pass facade to context
5. **User code** - Uses generic facade, never knows concrete data type

**Key Insight**: We accept one switch statement at the application boundary to create the appropriate facade. This is better than having switches scattered throughout the codebase or requiring users to specify data types everywhere.

The template-based infrastructure provides type safety **inside** the facade, while the facade provides type erasure **outside** for user code.

**Result**: 
- User code: `LoadTitleScene()` - no data type specified ✓
- Data source: Configured once at startup ✓
- Switch statement: One, acceptable, hidden ✓
- Type safety: Preserved where it matters ✓
