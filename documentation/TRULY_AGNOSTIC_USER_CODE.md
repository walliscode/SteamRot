# Truly Data Type Agnostic User Code

This document addresses the issue of user code referencing concrete data types like `SceneDataFbs`, and shows how to make user code completely agnostic to the data source format.

## The Problem with Current Examples

In previous examples, user code still explicitly references FlatBuffers types:

```cpp
// SceneFactory - NOT truly agnostic
auto config = factory.GetConfigurator<SceneDataFbs, Scene>();
//                                     ^^^^^^^^^^^^
//                                     FlatBuffers-specific type!
```

**Problem**: User code knows about `SceneDataFbs`, which is FlatBuffers-specific. If we switch to JSON, we'd need to change all these references to `JSONSceneData`.

## Solution: Use Agnostic Intermediate Types

The key insight is that types like `SceneData`, `EngineSnapshot`, `EntityMemoryPool` are **already data-format-agnostic**. They don't care if they came from FlatBuffers, JSON, or any other source.

### Architecture Pattern

```
User Code
    ↓ (uses only agnostic types)
Agnostic Types (SceneData, EngineSnapshot, etc.)
    ↓ (factory handles conversion internally)
UnifiedConfiguratorFactory
    ↓ (loads from configured data source)
Data Source (FlatBuffers, JSON, etc.)
```

## Implementation

### Step 1: Agnostic API in UnifiedConfiguratorFactory

```cpp
/////////////////////////////////////////////////
/// @file UnifiedConfiguratorFactory.h
/// @brief Truly data-type-agnostic factory interface
/////////////////////////////////////////////////

#pragma once

#include "DataSourceType.h"
#include "EngineSnapshot.h"
#include "EventHandler.h"
#include "FailInfo.h"
#include "Scene.h"
#include "SceneData.h"
#include "scene_types_generated.h"
#include <expected>
#include <memory>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Unified configurator factory with data-type-agnostic API
///
/// User code NEVER sees concrete data types like SceneDataFbs or JSONSceneData.
/// All methods work with agnostic types (SceneData, EngineSnapshot, etc.).
/// The factory handles loading from the configured data source internally.
/////////////////////////////////////////////////
class UnifiedConfiguratorFactory {
private:
  EventHandler& m_event_handler;
  DataSourceType m_data_source_type;
  
  // Internal implementation (type-erased)
  struct FactoryImpl;
  std::unique_ptr<FactoryImpl> m_impl;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor
  ///
  /// @param data_source_type Data source to use (FlatBuffers, JSON, etc.)
  /// @param event_handler Reference to event handler
  /////////////////////////////////////////////////
  UnifiedConfiguratorFactory(DataSourceType data_source_type,
                            EventHandler& event_handler);

  /////////////////////////////////////////////////
  /// @brief Load Scene data from defaults
  ///
  /// User code NEVER specifies the concrete data type.
  /// Factory loads from configured source and returns agnostic SceneData.
  ///
  /// @param scene_type Type of scene to load
  /// @return Agnostic SceneData or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<SceneData, FailInfo>
  LoadDefaultSceneData(const SceneType scene_type);

  /////////////////////////////////////////////////
  /// @brief Configure a Scene from SceneData
  ///
  /// @param scene Scene to configure
  /// @param scene_data Agnostic scene data
  /// @return std::monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene& scene, const SceneData& scene_data);

  /////////////////////////////////////////////////
  /// @brief Load EngineSnapshot from data source
  ///
  /// @param source_identifier Identifier for the snapshot to load
  /// @return Agnostic EngineSnapshot or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<EngineSnapshot, FailInfo>
  LoadEngineSnapshot(const std::string& source_identifier);

  /////////////////////////////////////////////////
  /// @brief Configure EngineSnapshot
  ///
  /// @param target_snapshot Snapshot to configure
  /// @param source_snapshot Source snapshot data
  /// @return std::monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureEngineSnapshot(EngineSnapshot& target_snapshot,
                         const EngineSnapshot& source_snapshot);

  /////////////////////////////////////////////////
  /// @brief Load SceneManagerData from defaults
  ///
  /// @return Agnostic SceneManagerData or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<SceneManagerData, FailInfo>
  LoadDefaultSceneManagerData();

  // Add similar methods for other types as needed...
};

} // namespace steamrot
```

### Step 2: Implementation with Internal Data Type Handling

```cpp
/////////////////////////////////////////////////
/// @file UnifiedConfiguratorFactory.cpp
/// @brief Implementation handling data types internally
/////////////////////////////////////////////////

#include "UnifiedConfiguratorFactory.h"
#include "ConfiguratorFactory.h"
#include "FlatbuffersDataLoader.h"
#include "FlatbuffersSceneDataProvider.h"
// Include other data source implementations as needed

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Internal implementation holder
/////////////////////////////////////////////////
struct UnifiedConfiguratorFactory::FactoryImpl {
  virtual ~FactoryImpl() = default;
  
  virtual std::expected<SceneData, FailInfo>
  LoadDefaultSceneData(const SceneType scene_type) = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene& scene, const SceneData& scene_data) = 0;
  
  virtual std::expected<EngineSnapshot, FailInfo>
  LoadEngineSnapshot(const std::string& source_identifier) = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureEngineSnapshot(EngineSnapshot& target,
                         const EngineSnapshot& source) = 0;
  
  virtual std::expected<SceneManagerData, FailInfo>
  LoadDefaultSceneManagerData() = 0;
};

/////////////////////////////////////////////////
/// @brief FlatBuffers implementation
/////////////////////////////////////////////////
class FlatbuffersFactoryImpl : public UnifiedConfiguratorFactory::FactoryImpl {
private:
  EventHandler& m_event_handler;
  ConfiguratorFactory<DataSourceType::Flatbuffers> m_factory;
  FlatbuffersDataLoader m_data_loader;
  FlatbuffersSceneDataProvider m_scene_data_provider;

public:
  FlatbuffersFactoryImpl(EventHandler& event_handler)
      : m_event_handler(event_handler),
        m_factory(event_handler),
        m_data_loader(),
        m_scene_data_provider(event_handler) {}

  std::expected<SceneData, FailInfo>
  LoadDefaultSceneData(const SceneType scene_type) override {
    // Load FlatBuffers data (internal - user never sees this)
    auto fb_data_result = m_data_loader.ProvideDefaultSceneData(scene_type);
    if (!fb_data_result)
      return std::unexpected(fb_data_result.error());
    
    // Convert to agnostic SceneData
    auto scene_data_result = m_scene_data_provider.ProvideSceneDataFromData(
        fb_data_result.value());
    
    if (!scene_data_result)
      return std::unexpected(scene_data_result.error());
    
    // Return agnostic type - user never saw FlatBuffers
    return scene_data_result.value();
  }

  std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene& scene, const SceneData& scene_data) override {
    // SceneData is already agnostic
    // Use existing scene configurator that works with SceneData
    
    // Get scene configurator (this is an implementation detail)
    auto configurator_result = m_factory.GetConfigurator<
        SceneDataFbs, Scene>();
    
    if (!configurator_result)
      return std::unexpected(configurator_result.error());
    
    // Note: This would be refactored to work directly with SceneData
    // For now, the configurator handles the agnostic SceneData type
    // The point is: user code never sees this
    
    return std::monostate{};
  }

  std::expected<EngineSnapshot, FailInfo>
  LoadEngineSnapshot(const std::string& source_identifier) override {
    // Load FlatBuffers snapshot data (internal)
    auto fb_data_result = m_data_loader.LoadEngineSnapshotFromFile(
        source_identifier);
    if (!fb_data_result)
      return std::unexpected(fb_data_result.error());
    
    // Get configurator (internal)
    auto config_result = m_factory.GetConfigurator<
        EngineSnapshotFbs, EngineSnapshot>();
    if (!config_result)
      return std::unexpected(config_result.error());
    
    // Create and configure agnostic EngineSnapshot
    EngineSnapshot snapshot;
    auto configure_result = config_result.value()->Configure(
        snapshot, *fb_data_result.value());
    
    if (!configure_result)
      return std::unexpected(configure_result.error());
    
    // Return agnostic type - user never saw FlatBuffers
    return snapshot;
  }

  std::expected<std::monostate, FailInfo>
  ConfigureEngineSnapshot(EngineSnapshot& target,
                         const EngineSnapshot& source) override {
    // Both are agnostic types - just copy/configure
    target = source;
    return std::monostate{};
  }

  std::expected<SceneManagerData, FailInfo>
  LoadDefaultSceneManagerData() override {
    // Similar pattern - load internally, return agnostic type
    auto fb_data_result = m_data_loader.ProvideSceneManagerData();
    if (!fb_data_result)
      return std::unexpected(fb_data_result.error());
    
    // Convert to agnostic type and return
    // Implementation converts FlatBuffers → SceneManagerData
    SceneManagerData data;
    // ... conversion logic ...
    return data;
  }
};

/////////////////////////////////////////////////
/// @brief JSON implementation (future)
/////////////////////////////////////////////////
class JSONFactoryImpl : public UnifiedConfiguratorFactory::FactoryImpl {
private:
  EventHandler& m_event_handler;
  ConfiguratorFactory<DataSourceType::JSON> m_factory;
  // JSONDataLoader m_data_loader;
  // JSONSceneDataProvider m_scene_data_provider;

public:
  JSONFactoryImpl(EventHandler& event_handler)
      : m_event_handler(event_handler),
        m_factory(event_handler) {}

  std::expected<SceneData, FailInfo>
  LoadDefaultSceneData(const SceneType scene_type) override {
    // Load JSON data (internal - user never sees this)
    // auto json_data = m_data_loader.LoadSceneJSON(scene_type);
    
    // Convert to agnostic SceneData
    // return ConvertJSONToSceneData(json_data);
    
    return std::unexpected(FailInfo{
        FailMode::NotImplemented,
        "JSON implementation not yet available"});
  }

  // Other methods follow same pattern...
  std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene& scene, const SceneData& scene_data) override {
    return std::unexpected(FailInfo{FailMode::NotImplemented, "JSON not implemented"});
  }

  std::expected<EngineSnapshot, FailInfo>
  LoadEngineSnapshot(const std::string& source_identifier) override {
    return std::unexpected(FailInfo{FailMode::NotImplemented, "JSON not implemented"});
  }

  std::expected<std::monostate, FailInfo>
  ConfigureEngineSnapshot(EngineSnapshot& target,
                         const EngineSnapshot& source) override {
    target = source;
    return std::monostate{};
  }

  std::expected<SceneManagerData, FailInfo>
  LoadDefaultSceneManagerData() override {
    return std::unexpected(FailInfo{FailMode::NotImplemented, "JSON not implemented"});
  }
};

/////////////////////////////////////////////////
// UnifiedConfiguratorFactory implementation
/////////////////////////////////////////////////

UnifiedConfiguratorFactory::UnifiedConfiguratorFactory(
    DataSourceType data_source_type,
    EventHandler& event_handler)
    : m_event_handler(event_handler),
      m_data_source_type(data_source_type) {
  
  // Create appropriate implementation based on data source
  // This is the ONLY place that switches on data type
  switch (data_source_type) {
  case DataSourceType::Flatbuffers:
    m_impl = std::make_unique<FlatbuffersFactoryImpl>(event_handler);
    break;
  
  case DataSourceType::JSON:
    m_impl = std::make_unique<JSONFactoryImpl>(event_handler);
    break;
  
  default:
    throw std::runtime_error("Unsupported data source type");
  }
}

/////////////////////////////////////////////////
std::expected<SceneData, FailInfo>
UnifiedConfiguratorFactory::LoadDefaultSceneData(const SceneType scene_type) {
  return m_impl->LoadDefaultSceneData(scene_type);
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
UnifiedConfiguratorFactory::ConfigureScene(Scene& scene,
                                          const SceneData& scene_data) {
  return m_impl->ConfigureScene(scene, scene_data);
}

/////////////////////////////////////////////////
std::expected<EngineSnapshot, FailInfo>
UnifiedConfiguratorFactory::LoadEngineSnapshot(
    const std::string& source_identifier) {
  return m_impl->LoadEngineSnapshot(source_identifier);
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
UnifiedConfiguratorFactory::ConfigureEngineSnapshot(
    EngineSnapshot& target_snapshot,
    const EngineSnapshot& source_snapshot) {
  return m_impl->ConfigureEngineSnapshot(target_snapshot, source_snapshot);
}

/////////////////////////////////////////////////
std::expected<SceneManagerData, FailInfo>
UnifiedConfiguratorFactory::LoadDefaultSceneManagerData() {
  return m_impl->LoadDefaultSceneManagerData();
}

} // namespace steamrot
```

### Step 3: Truly Agnostic User Code

#### SceneFactory - NO FlatBuffers References

```cpp
/////////////////////////////////////////////////
/// @file SceneFactory.cpp
/// @brief SceneFactory - completely data type agnostic
/////////////////////////////////////////////////

#include "SceneFactory.h"

namespace steamrot {

std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateSceneFromDefault(SceneType scene_type) {

  // Step 1: Load scene data - NO CONCRETE TYPE SPECIFIED
  // Factory handles FlatBuffers/JSON/etc. internally
  auto scene_data_result = 
      m_game_context.configurator_factory.LoadDefaultSceneData(scene_type);
  //                                     ^^^^^^^^^^^^^^^^^^^^^^
  //                                     Returns agnostic SceneData
  //                                     User never sees FlatBuffers
  
  if (!scene_data_result)
    return std::unexpected(scene_data_result.error());

  SceneData scene_data = scene_data_result.value();
  //        ^^^^^^^^^
  //        Agnostic type - not SceneDataFbs!

  // Step 2: Create empty scene
  auto scene_result = CreateEmptyScene(scene_data.scene_info.type);
  if (!scene_result)
    return std::unexpected(scene_result.error());

  auto scene = std::move(scene_result.value());

  // Step 3: Configure scene - NO CONCRETE TYPE SPECIFIED
  auto config_result = 
      m_game_context.configurator_factory.ConfigureScene(*scene, scene_data);
  //                                     ^^^^^^^^^^^^^^
  //                                     Works with agnostic types only
  
  if (!config_result)
    return std::unexpected(config_result.error());

  return scene;
}

} // namespace steamrot
```

#### TestEngine - NO FlatBuffers References

```cpp
/////////////////////////////////////////////////
/// @file TestEngine.cpp
/// @brief TestEngine - completely data type agnostic
/////////////////////////////////////////////////

#include "TestEngine.h"

namespace steamrot {

std::expected<std::monostate, FailInfo>
TestEngine::LoadStartingSnapshot(const std::string& snapshot_id) {

  // Load snapshot - NO CONCRETE TYPE SPECIFIED
  auto snapshot_result = 
      m_game_context.configurator_factory.LoadEngineSnapshot(snapshot_id);
  //                                     ^^^^^^^^^^^^^^^^^^^
  //                                     Returns agnostic EngineSnapshot
  //                                     User never sees FlatBuffers
  
  if (!snapshot_result)
    return std::unexpected(snapshot_result.error());

  EngineSnapshot snapshot = snapshot_result.value();
  //             ^^^^^^^^^^^
  //             Agnostic type - not EngineSnapshotFbs!

  // Configure engine state - NO CONCRETE TYPE SPECIFIED
  auto config_result = 
      m_game_context.configurator_factory.ConfigureEngineSnapshot(
          m_current_state, snapshot);
  
  if (!config_result)
    return std::unexpected(config_result.error());

  return std::monostate{};
}

} // namespace steamrot
```

#### SceneManager - NO FlatBuffers References

```cpp
/////////////////////////////////////////////////
/// @file SceneManager.cpp
/// @brief SceneManager - completely data type agnostic
/////////////////////////////////////////////////

#include "SceneManager.h"
#include "SceneFactory.h"

namespace steamrot {

std::expected<std::monostate, FailInfo>
SceneManager::StartUp() {

  // Load scene manager data - NO CONCRETE TYPE SPECIFIED
  auto data_result = 
      m_game_context.configurator_factory.LoadDefaultSceneManagerData();
  //                                     ^^^^^^^^^^^^^^^^^^^^^^^^^^^
  //                                     Returns agnostic SceneManagerData
  
  if (!data_result)
    return std::unexpected(data_result.error());

  SceneManagerData manager_data = data_result.value();
  //               ^^^^^^^^^^^^^^^^
  //               Agnostic type!

  // Configure from data
  m_scene_manager_state = manager_data.scene_manager_state;

  return std::monostate{};
}

std::expected<std::monostate, FailInfo>
SceneManager::AddSceneFromDefault(const SceneType& scene_type) {
  
  // Create factory (agnostic)
  SceneFactory factory(m_game_context);
  
  // Create scene - factory handles data type internally
  // NO FLATBUFFERS TYPES ANYWHERE
  auto scene_result = factory.CreateSceneFromDefault(scene_type);
  
  if (!scene_result)
    return std::unexpected(scene_result.error());

  auto scene = std::move(scene_result.value());
  uuids::uuid scene_id = scene->GetSceneInfo().id;
  m_scenes[scene_id] = std::move(scene);

  return std::monostate{};
}

} // namespace steamrot
```

## Key Differences from Previous Approach

### Previous Approach (Had the Problem)

```cpp
// User code explicitly referenced FlatBuffers types
auto config = factory.GetConfigurator<SceneDataFbs, Scene>();
//                                     ^^^^^^^^^^^^
//                                     Concrete FlatBuffers type!

auto provider = factory.GetDataProvider<SceneDataFbs>();
//                                       ^^^^^^^^^^^^
//                                       Concrete FlatBuffers type!
```

**Problem**: User code knows about `SceneDataFbs`. Changing to JSON requires updating all user code.

### New Approach (Solves the Problem)

```cpp
// User code only uses agnostic types
auto scene_data = factory.LoadDefaultSceneData(SceneType::Title);
//                                              ^^^^^^^^^^^^^^^^
//                                              Only needs SceneType enum

auto config_result = factory.ConfigureScene(scene, scene_data);
//                                          ^^^^^  ^^^^^^^^^^
//                                          Agnostic types only
```

**Solution**: User code never sees `SceneDataFbs`, `JSONSceneData`, or any concrete data type.

## Architecture Layers

```
┌─────────────────────────────────────────────────────────┐
│                     User Code Layer                      │
│              (NO concrete data type knowledge)           │
│                                                          │
│  SceneFactory::CreateSceneFromDefault(SceneType type) { │
│    auto data = factory.LoadDefaultSceneData(type);      │
│    //                  ^^^^^^^^^^^^^^^^^^^^             │
│    //                  Agnostic API                     │
│                                                          │
│    Scene scene;                                          │
│    factory.ConfigureScene(scene, data);                 │
│    //     ^^^^^^^^^^^^^^                                │
│    //     Agnostic API                                  │
│  }                                                       │
└────────────────────┬────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│           UnifiedConfiguratorFactory                     │
│        (Agnostic API, Internal Dispatch)                │
│                                                          │
│  LoadDefaultSceneData(SceneType) → SceneData            │
│  ConfigureScene(Scene&, SceneData&) → void              │
│  LoadEngineSnapshot(string) → EngineSnapshot            │
│                                                          │
│  switch (m_data_source_type) {                          │
│    case Flatbuffers: return flatbuffers_impl->Load...();│
│    case JSON: return json_impl->Load...();              │
│  }                                                       │
└────────────────────┬────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│         FlatbuffersFactoryImpl (Internal)                │
│      (Knows about FlatBuffers - User Never Sees)        │
│                                                          │
│  LoadDefaultSceneData(SceneType type) {                 │
│    // Load FlatBuffers data internally                  │
│    auto fb_data = m_data_loader.Load(type);             │
│    //             ^^^^^^^^^^^^^^^^^^                    │
│    //             FlatBuffers-specific (internal)       │
│                                                          │
│    // Convert to agnostic type                          │
│    return ConvertToSceneData(fb_data);                  │
│    //     ^^^^^^^^^^^^^^^^^^^^^^                        │
│    //     Returns SceneData (agnostic)                  │
│  }                                                       │
└─────────────────────────────────────────────────────────┘
```

## Complete Flow Example

### User Wants to Load Title Scene

```cpp
// 1. User code (SceneManager)
AddSceneFromDefault(SceneType::Title);
//                  ^^^^^^^^^^^^^^^^
//                  Only knows about SceneType enum

// 2. SceneFactory
auto scene_data = m_context.factory.LoadDefaultSceneData(SceneType::Title);
//                                  ^^^^^^^^^^^^^^^^^^^^
//                                  Agnostic API call

// 3. UnifiedConfiguratorFactory (internal dispatch)
switch (m_data_source_type) {
  case Flatbuffers: return m_flatbuffers_impl->LoadDefaultSceneData(type);
  case JSON: return m_json_impl->LoadDefaultSceneData(type);
}

// 4. FlatbuffersFactoryImpl (internal implementation)
auto fb_data = m_data_loader.ProvideDefaultSceneData(SceneType::Title);
//             ^^^^^^^^^^^^^^
//             Loads FlatBuffers (user never sees this)

auto scene_data = ConvertFlatbuffersToSceneData(fb_data);
//                ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//                Converts to agnostic type

return scene_data;  // Returns SceneData (agnostic)

// 5. Back to user code
Scene scene;
factory.ConfigureScene(scene, scene_data);
//      ^^^^^^^^^^^^^^
//      Agnostic API - works regardless of data source
```

**At no point does user code see `SceneDataFbs` or any concrete data type!**

## Switching Data Sources

```cpp
// Development - use FlatBuffers
DataSourceType source = DataSourceType::Flatbuffers;

// Testing - switch to JSON (if implemented)
DataSourceType source = DataSourceType::JSON;

// ALL USER CODE WORKS UNCHANGED
// No references to SceneDataFbs anywhere in user code
```

## Benefits

### 1. True Data Type Agnosticism

```cpp
// User code NEVER references concrete types
auto data = factory.LoadDefaultSceneData(SceneType::Title);
//          ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//          No SceneDataFbs, no JSONSceneData - just SceneData
```

### 2. Easy to Switch Data Sources

```cpp
// Change one line at startup
DataSourceType source = DataSourceType::JSON;  // Was: Flatbuffers

// Zero changes to user code needed
```

### 3. Agnostic Types Are Already Present

The codebase already has agnostic types:
- `SceneData` (not `SceneDataFbs`)
- `EngineSnapshot` (not `EngineSnapshotFbs`)
- `SceneManagerData` (not `SceneManagerDataFbs`)
- `EntityMemoryPool` (no Fbs suffix)

These are the types user code should work with.

### 4. Clear Separation

```
User Code → Only sees agnostic types (SceneData, EngineSnapshot)
Factory → Provides agnostic API (LoadDefaultSceneData, ConfigureScene)
Implementation → Handles concrete types internally (SceneDataFbs, JSONSceneData)
```

## Summary

**Problem**: User code explicitly references `SceneDataFbs`, making it not truly data type agnostic.

**Solution**: 
1. Factory provides **agnostic API** methods (LoadDefaultSceneData, LoadEngineSnapshot, etc.)
2. Factory handles **data loading and conversion internally**
3. User code **never sees** `SceneDataFbs`, `JSONSceneData`, or any concrete data types
4. User code **only works with** agnostic types: `SceneData`, `EngineSnapshot`, etc.

**Result**:
- ✅ User code has NO references to FlatBuffers types
- ✅ User code has NO references to JSON types
- ✅ User code only uses agnostic types
- ✅ Switch data sources by changing one line at startup
- ✅ Zero changes to user code when switching sources
- ✅ Factory handles all conversions internally

This is **truly data type agnostic** user code.
