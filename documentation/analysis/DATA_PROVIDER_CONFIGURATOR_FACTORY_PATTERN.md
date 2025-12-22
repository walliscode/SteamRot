# Data Provider and Configurator Factory Pattern Analysis

## Executive Summary

This document analyzes the current data provider and configurator architecture in SteamRot and proposes a factory pattern solution to enable extensible, switchable data format support.

**Current State:**
- Multiple interface/implementation pairs exist for data handling (Provider and Configurator patterns)
- Selection mechanism is hardcoded with static instances
- No runtime configurability or extensibility for different data formats
- Cannot easily switch between FlatBuffers, XML, JSON, or test data sources

**Proposed Solution:**
- Implement factory classes that provide abstract interfaces
- Add configuration mechanism to select data format at engine initialization
- Maintain backward compatibility while enabling future extensibility
- Follow existing architectural patterns in the codebase

**Impact:**
- **Low risk**: Changes are additive, existing code continues to work
- **High value**: Enables testing, modding, and future data format support
- **Effort**: 2-3 days for complete implementation with tests

---

## Table of Contents

1. [Problem Statement](#problem-statement)
2. [Current Architecture](#current-architecture)
3. [Analysis of Interface Patterns](#analysis-of-interface-patterns)
4. [Proposed Solution](#proposed-solution)
5. [Implementation Plan](#implementation-plan)
6. [Testing Strategy](#testing-strategy)
7. [Future Considerations](#future-considerations)

---

## Problem Statement

### The Issue

From the problem statement:
> "we have a few interface classes that deal with data types (flatbuffers, xml e.t.c.) as their concrete implementation. currently the selection process is a bit janky and non switchable extensible"

### Specific Problems Identified

1. **Hardcoded Selection**: Provider and configurator selection uses static instances
   ```cpp
   // In provider_factory.cpp
   IEngineDataProvider &GetEngineDataProvider() {
     static FlatbuffersEngineDataProvider provider;  // ← Hardcoded
     return provider;
   }
   
   // In SceneFactory.cpp
   ISceneConfigurator &GetSceneConfigurator() {
     static FlatbuffersSceneConfigurator configurator;  // ← Hardcoded
     return configurator;
   }
   ```

2. **Non-Extensible**: Cannot add new data format implementations without code changes
   - Want to support XML? Must modify provider_factory.cpp
   - Want to use test data? Must change hardcoded statics
   - Want user to choose format? No mechanism exists

3. **Testing Limitations**: Cannot easily inject mock providers/configurators for testing
   - Static instances make testing difficult
   - No dependency injection support
   - Cannot simulate different data scenarios

4. **No Central Control**: Selection logic scattered across multiple files
   - `provider_factory.cpp` handles providers
   - `SceneFactory.cpp` handles scene configurator
   - `FlatbuffersEntityConfigurator` directly instantiated in scene configurator
   - No single point of configuration

### Use Cases That Should Be Supported

1. **Development**: Use FlatBuffers (current default)
2. **Testing**: Use in-memory test data providers
3. **Modding**: Allow users to provide data in XML or JSON
4. **Save Files**: Different format for user save data vs. default data
5. **Network**: Load data from network source (future)

---

## Current Architecture

### Interface/Implementation Pairs

The codebase follows a provider/configurator pattern with clear separation:

#### Provider Interfaces (Load/Provide Data)

| Interface | FlatBuffers Implementation | Purpose |
|-----------|---------------------------|---------|
| `IEngineDataProvider` | `FlatbuffersEngineDataProvider` | Engine configuration, state, resources |
| `ISceneDataProvider` | `FlatbuffersSceneDataProvider` | Scene data |
| `ISceneManagerDataProvider` | `FlatbuffersSceneManagerDataProvider` | Scene manager data |
| `IAssetDataProvider` | `FlatbuffersAssetDataProvider` | Asset loading (fonts, textures, etc.) |
| `IUIStyleDataProvider` | `FlatbuffersUIStyleDataProvider` | UI style configurations |
| `ISubscriberViewer` | `FlatbuffersSubscriberViewer` | Subscriber configurations |

#### Configurator Interfaces (Apply Data to Objects)

| Interface | FlatBuffers Implementation | Purpose |
|-----------|---------------------------|---------|
| `ISceneConfigurator` | `FlatbuffersSceneConfigurator` | Configure Scene objects |
| `IEntityConfigurator` | `FlatbuffersEntityConfigurator` | Configure entity components |
| `IUIElementConfigurator` | `FlatbuffersUIElementConfigurator` | Configure UI elements |
| `IUIStyleConfigurator` | (not yet implemented) | Configure UI styles |

### Current Selection Mechanism

**File: `src/data_providers/provider_factory.h/cpp`**

```cpp
// Current implementation - hardcoded FlatBuffers
IEngineDataProvider &GetEngineDataProvider() {
  static FlatbuffersEngineDataProvider provider;
  return provider;
}

ISceneDataProvider &GetSceneDataProvider() {
  static FlatbuffersSceneDataProvider provider;
  return provider;
}

IAssetDataProvider &GetAssetDataProvider() {
  static FlatbuffersAssetDataProvider provider;
  return provider;
}

ISceneManagerDataProvider &GetSceneManagerDataProvider() {
  static FlatbuffersSceneManagerDataProvider provider;
  return provider;
}
```

**File: `src/scenes/SceneFactory.cpp`**

```cpp
ISceneConfigurator &GetSceneConfigurator() {
  static FlatbuffersSceneConfigurator configurator;
  return configurator;
}
```

**File: `src/scenes/FlatbuffersSceneConfigurator.cpp`**

```cpp
std::expected<std::monostate, FailInfo>
FlatbuffersSceneConfigurator::ConfigureEntities(Scene &scene, const SceneData *scene_data) {
  // Directly instantiates FlatbuffersEntityConfigurator
  FlatbuffersEntityConfigurator entity_configurator(
      scene.GetSceneContext().event_handler,
      *fbs_scene_data->scene_data_fbs->entity_collection());
  // ...
}
```

**File: `src/entity/FlatbuffersEntityConfigurator.cpp`**

```cpp
std::expected<std::monostate, FailInfo>
FlatbuffersEntityConfigurator::ConfigureCUserInterface(CUserInterface &c_ui_component) {
  // Directly instantiates FlatbuffersUIElementConfigurator
  FlatbuffersUIElementConfigurator ui_configurator(m_event_handler, *ui_data);
  // ...
}
```

### Problems with Current Approach

1. **Scattered Logic**: Selection happens in multiple places
2. **Static Lifetime**: Cannot change during runtime
3. **No Configuration**: No way to specify which implementation to use
4. **Tight Coupling**: Configurators directly instantiate other configurators
5. **Testing Difficulty**: Cannot inject test implementations

---

## Analysis of Interface Patterns

### Pattern 1: Provider Pattern

**Purpose**: Load/provide data from various sources

**Characteristics**:
- Stateless (mostly)
- Return data as native C++ structs or expected<T, FailInfo>
- Do not modify game state
- Can be implemented for different data sources

**Example**:
```cpp
class IEngineDataProvider {
public:
  virtual std::expected<EngineResourcesConfig, FailInfo>
  LoadEngineResourcesConfig() const = 0;
  
  virtual std::expected<EngineConfig, FailInfo>
  LoadEngineConfig() const = 0;
  
  virtual std::expected<EngineState, FailInfo>
  LoadEngineState() const = 0;
};
```

### Pattern 2: Configurator Pattern

**Purpose**: Apply data to game objects

**Characteristics**:
- May have state (references to data, event handler, etc.)
- Modify game objects passed to them
- Separate concerns: data loading vs. object configuration
- Can be implemented for different data formats

**Example**:
```cpp
class ISceneConfigurator {
public:
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(Scene &scene, const SceneData *scene_data) = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureEntities(Scene &scene, const SceneData *scene_data) = 0;
};
```

### Pattern 3: Factory Functions (Current Approach)

**Current State**:
```cpp
// Simple factory function - returns static instance
IEngineDataProvider &GetEngineDataProvider() {
  static FlatbuffersEngineDataProvider provider;
  return provider;
}
```

**Limitations**:
- No configurability
- Single implementation only
- Cannot be tested with mocks
- No abstraction over creation logic

---

## Proposed Solution

### Overview

Implement a **Factory Manager** that sits in the Engine and provides configured access to data providers and configurators. The factory will:

1. Be configurable at engine initialization
2. Provide consistent access to all providers and configurators
3. Support multiple implementations (FlatBuffers, XML, JSON, Test, etc.)
4. Maintain backward compatibility

### Architecture Diagram

```
┌────────────────────────────────────────────────────────────┐
│                        Engine                               │
│                                                            │
│  ┌──────────────────────────────────────────────────────┐ │
│  │           DataAccessFactory                          │ │
│  │                                                      │ │
│  │  Configuration: DataFormatType (enum)               │ │
│  │  - FlatBuffers (default)                            │ │
│  │  - XML                                              │ │
│  │  - JSON                                             │ │
│  │  - Test                                             │ │
│  │                                                      │ │
│  │  Methods:                                           │ │
│  │  + GetEngineDataProvider() -> IEngineDataProvider&  │ │
│  │  + GetSceneDataProvider() -> ISceneDataProvider&    │ │
│  │  + GetAssetDataProvider() -> IAssetDataProvider&    │ │
│  │  + GetSceneConfigurator() -> ISceneConfigurator&    │ │
│  │  + GetEntityConfigurator() -> IEntityConfigurator&  │ │
│  │  + GetUIElementConfigurator() -> ...                │ │
│  └──────────────────────────────────────────────────────┘ │
│                          │                                 │
│                          │ provides                        │
│                          ▼                                 │
│  ┌─────────────┐  ┌──────────────┐  ┌─────────────┐     │
│  │ Providers   │  │Configurators │  │   Loaders   │     │
│  │             │  │              │  │             │     │
│  │ Engine      │  │ Scene        │  │ Data        │     │
│  │ Scene       │  │ Entity       │  │             │     │
│  │ Asset       │  │ UIElement    │  │             │     │
│  └─────────────┘  └──────────────┘  └─────────────┘     │
└────────────────────────────────────────────────────────────┘
```

### Key Components

#### 1. DataFormatType Enum

**File: `src/types/core/DataFormatType.h`**

```cpp
namespace steamrot {

////////////////////////////////////////////////////////////
/// @brief Enum for supported data format types
////////////////////////////////////////////////////////////
enum class DataFormatType : uint8_t {
  FlatBuffers = 0,  // Default format
  XML = 1,          // Future support
  JSON = 2,         // Future support
  Test = 3,         // For testing with mock data
};

} // namespace steamrot
```

#### 2. DataAccessFactory Class

**File: `src/engine/DataAccessFactory.h`**

```cpp
namespace steamrot {

////////////////////////////////////////////////////////////
/// @class DataAccessFactory
/// @brief Central factory for all data providers and configurators
///
/// This factory provides configured access to data providers and
/// configurators based on the selected data format type. It replaces
/// the scattered static instances with a centralized, configurable
/// system.
///
/// Usage:
/// ```cpp
/// DataAccessFactory factory(DataFormatType::FlatBuffers);
/// IEngineDataProvider& provider = factory.GetEngineDataProvider();
/// ```
////////////////////////////////////////////////////////////
class DataAccessFactory {
private:
  ////////////////////////////////////////////////////////////
  /// @brief Currently configured data format type
  ////////////////////////////////////////////////////////////
  DataFormatType m_data_format_type;

  ////////////////////////////////////////////////////////////
  /// @brief Cached provider instances (owned by factory)
  ////////////////////////////////////////////////////////////
  std::unique_ptr<IEngineDataProvider> m_engine_provider;
  std::unique_ptr<ISceneDataProvider> m_scene_provider;
  std::unique_ptr<IAssetDataProvider> m_asset_provider;
  std::unique_ptr<ISceneManagerDataProvider> m_scene_manager_provider;
  std::unique_ptr<IUIStyleDataProvider> m_ui_style_provider;

  ////////////////////////////////////////////////////////////
  /// @brief Cached configurator instances (owned by factory)
  ////////////////////////////////////////////////////////////
  std::unique_ptr<ISceneConfigurator> m_scene_configurator;

  ////////////////////////////////////////////////////////////
  /// @brief Initialize all providers and configurators
  ////////////////////////////////////////////////////////////
  void InitializeProviders();
  void InitializeConfigurators();

public:
  ////////////////////////////////////////////////////////////
  /// @brief Constructor with data format type
  ///
  /// @param format_type Data format to use for providers/configurators
  ////////////////////////////////////////////////////////////
  explicit DataAccessFactory(DataFormatType format_type = DataFormatType::FlatBuffers);

  ////////////////////////////////////////////////////////////
  /// @brief Get the engine data provider
  ////////////////////////////////////////////////////////////
  IEngineDataProvider& GetEngineDataProvider();

  ////////////////////////////////////////////////////////////
  /// @brief Get the scene data provider
  ////////////////////////////////////////////////////////////
  ISceneDataProvider& GetSceneDataProvider();

  ////////////////////////////////////////////////////////////
  /// @brief Get the asset data provider
  ////////////////////////////////////////////////////////////
  IAssetDataProvider& GetAssetDataProvider();

  ////////////////////////////////////////////////////////////
  /// @brief Get the scene manager data provider
  ////////////////////////////////////////////////////////////
  ISceneManagerDataProvider& GetSceneManagerDataProvider();

  ////////////////////////////////////////////////////////////
  /// @brief Get the scene configurator
  ////////////////////////////////////////////////////////////
  ISceneConfigurator& GetSceneConfigurator();

  ////////////////////////////////////////////////////////////
  /// @brief Create an entity configurator instance
  ///
  /// Entity configurators are stateful and created per-use
  ///
  /// @param event_handler Reference to event handler
  /// @param entity_collection_data Entity data to configure from
  ////////////////////////////////////////////////////////////
  std::unique_ptr<IEntityConfigurator> CreateEntityConfigurator(
      EventHandler& event_handler,
      const void* entity_collection_data);

  ////////////////////////////////////////////////////////////
  /// @brief Create a UI element configurator instance
  ///
  /// @param event_handler Reference to event handler
  /// @param ui_data UI element data to configure from
  ////////////////////////////////////////////////////////////
  std::unique_ptr<IUIElementConfigurator> CreateUIElementConfigurator(
      EventHandler& event_handler,
      const void* ui_data);

  ////////////////////////////////////////////////////////////
  /// @brief Get current data format type
  ////////////////////////////////////////////////////////////
  DataFormatType GetDataFormatType() const { return m_data_format_type; }
};

} // namespace steamrot
```

#### 3. Updated Engine Class

**File: `src/engine/Engine.h`** (additions)

```cpp
class Engine {
protected:
  // ... existing members ...

  ////////////////////////////////////////////////////////////
  /// @brief Data access factory for providers and configurators
  ////////////////////////////////////////////////////////////
  DataAccessFactory m_data_access_factory;

public:
  ////////////////////////////////////////////////////////////
  /// @brief Constructor with optional data format type
  ///
  /// @param format_type Data format to use (defaults to FlatBuffers)
  ////////////////////////////////////////////////////////////
  explicit Engine(DataFormatType format_type = DataFormatType::FlatBuffers);

  ////////////////////////////////////////////////////////////
  /// @brief Get the data access factory
  ////////////////////////////////////////////////////////////
  DataAccessFactory& GetDataAccessFactory() { return m_data_access_factory; }
};
```

### Configuration Mechanism

#### Option 1: Constructor Parameter (Recommended)

```cpp
// In main.cpp or game initialization
DataFormatType format = DataFormatType::FlatBuffers;  // Could be from config file
GameEngine engine(format);
engine.RunGame();
```

#### Option 2: Configuration File

**File: `data/engine/engine_config.json`** (example)

```json
{
  "data_format": "FlatBuffers",
  "engine_settings": {
    ...
  }
}
```

Load during engine startup:
```cpp
auto config = LoadEngineConfig();
DataFormatType format = ParseDataFormatType(config.data_format);
```

#### Option 3: Environment Variable (For Testing)

```cpp
const char* format_env = std::getenv("STEAMROT_DATA_FORMAT");
DataFormatType format = format_env ? ParseDataFormatType(format_env) 
                                    : DataFormatType::FlatBuffers;
```

### Migration Path

#### Phase 1: Create Factory Infrastructure (Non-Breaking)

1. Create `DataFormatType.h` enum
2. Create `DataAccessFactory.h/cpp` class
3. Implement for FlatBuffers (existing implementations)
4. Add to Engine class as member

**Changes**:
- New files only
- No modifications to existing code
- Factory coexists with old system

#### Phase 2: Update Provider Access (Breaking, Internal Only)

1. Update `Engine::StartUp()` to use factory
2. Update `SceneFactory` to receive factory reference
3. Update configurators to receive factory reference

**Changes**:
- `Engine.cpp` - use `m_data_access_factory` instead of `GetEngineDataProvider()`
- `SceneFactory.cpp` - receive factory in constructor
- `FlatbuffersSceneConfigurator` - receive factory, use it to create entity configurator

#### Phase 3: Deprecate Old System

1. Mark `provider_factory.h` functions as deprecated
2. Update all call sites to use factory
3. Remove old factory functions

**Changes**:
- Remove `provider_factory.h/cpp`
- Update any remaining direct uses

#### Phase 4: Add New Format Support (Future)

1. Implement `XMLEngineDataProvider : IEngineDataProvider`
2. Implement `XMLSceneDataProvider : ISceneDataProvider`
3. Update factory to create XML providers when `DataFormatType::XML`

**Changes**:
- New implementation files
- Update factory switch statements
- No changes to engine or other code

---

## Implementation Plan

### Phase 1: Foundation (Day 1, Morning)

**Goal**: Create factory infrastructure without breaking existing code

**Tasks**:
1. Create `src/types/core/DataFormatType.h`
2. Create `src/engine/DataAccessFactory.h`
3. Create `src/engine/DataAccessFactory.cpp`
4. Implement factory for FlatBuffers format
5. Write unit tests for factory creation

**Files**:
- New: `src/types/core/DataFormatType.h`
- New: `src/engine/DataAccessFactory.h`
- New: `src/engine/DataAccessFactory.cpp`
- New: `tests/unit/engine/DataAccessFactory.test.cpp`

**Validation**:
- Factory can be instantiated
- Factory returns correct provider instances
- All providers work as before

### Phase 2: Engine Integration (Day 1, Afternoon)

**Goal**: Integrate factory into Engine class

**Tasks**:
1. Add factory member to Engine class
2. Update Engine constructor to accept DataFormatType
3. Update Engine::StartUp() to use factory
4. Update GameEngine and TestEngine constructors
5. Write integration tests

**Files**:
- Modified: `src/engine/Engine.h`
- Modified: `src/engine/Engine.cpp`
- Modified: `src/engine/GameEngine.h`
- Modified: `src/engine/GameEngine.cpp`
- New: `tests/integration/engine/EngineDataAccess.test.cpp`

**Validation**:
- Engine starts up correctly
- Providers accessed through factory
- Existing functionality unchanged

### Phase 3: SceneFactory Integration (Day 2, Morning)

**Goal**: Update scene creation to use factory

**Tasks**:
1. Add factory reference to SceneFactory
2. Update SceneFactory::CreateSceneFromDefault()
3. Pass factory to scene configurator
4. Update scene configurator to use factory for entity configurator
5. Write tests

**Files**:
- Modified: `src/scenes/SceneFactory.h`
- Modified: `src/scenes/SceneFactory.cpp`
- Modified: `src/scenes/FlatbuffersSceneConfigurator.h`
- Modified: `src/scenes/FlatbuffersSceneConfigurator.cpp`
- Modified: `tests/unit/scenes/SceneFactory.test.cpp`

**Validation**:
- Scenes created correctly
- Entity configuration works
- UI element configuration works

### Phase 4: Cleanup and Documentation (Day 2, Afternoon)

**Goal**: Remove old system, add documentation

**Tasks**:
1. Remove `provider_factory.h/cpp`
2. Update remaining call sites
3. Add documentation to README
4. Update architecture diagrams
5. Add examples

**Files**:
- Deleted: `src/data_providers/provider_factory.h`
- Deleted: `src/data_providers/provider_factory.cpp`
- Modified: `README.md`
- New: `documentation/architecture/DATA_ACCESS_FACTORY.md`

**Validation**:
- Code compiles without warnings
- All tests pass
- Documentation is clear

### Phase 5: Testing and Validation (Day 3)

**Goal**: Comprehensive testing of new system

**Tasks**:
1. Run full test suite
2. Test with different data format types
3. Integration testing with full game loop
4. Performance validation
5. Code review preparation

**Validation**:
- All tests pass
- No performance regression
- Code review ready

---

## Testing Strategy

### Unit Tests

#### DataAccessFactory Tests

**File: `tests/unit/engine/DataAccessFactory.test.cpp`**

```cpp
TEST_CASE("DataAccessFactory creates FlatBuffers providers", "[unit][DataAccessFactory]") {
  steamrot::DataAccessFactory factory(steamrot::DataFormatType::FlatBuffers);
  
  SECTION("GetEngineDataProvider returns valid provider") {
    auto& provider = factory.GetEngineDataProvider();
    REQUIRE(dynamic_cast<steamrot::FlatbuffersEngineDataProvider*>(&provider) != nullptr);
  }
  
  SECTION("GetSceneDataProvider returns valid provider") {
    auto& provider = factory.GetSceneDataProvider();
    REQUIRE(dynamic_cast<steamrot::FlatbuffersSceneDataProvider*>(&provider) != nullptr);
  }
  
  // ... more tests for each provider
}

TEST_CASE("DataAccessFactory creates configurators", "[unit][DataAccessFactory]") {
  steamrot::DataAccessFactory factory(steamrot::DataFormatType::FlatBuffers);
  
  SECTION("GetSceneConfigurator returns valid configurator") {
    auto& configurator = factory.GetSceneConfigurator();
    REQUIRE(dynamic_cast<steamrot::FlatbuffersSceneConfigurator*>(&configurator) != nullptr);
  }
}

TEST_CASE("DataAccessFactory supports Test format", "[unit][DataAccessFactory]") {
  // Future: Test that Test format creates test providers
  // For now, verify it doesn't crash
  steamrot::DataAccessFactory factory(steamrot::DataFormatType::Test);
  SUCCEED("Test format factory created without crash");
}
```

### Integration Tests

#### Engine Integration Test

**File: `tests/integration/engine/EngineDataAccess.test.cpp`**

```cpp
TEST_CASE("Engine uses DataAccessFactory for data access", "[integration][Engine]") {
  steamrot::TestEngine engine(steamrot::DataFormatType::FlatBuffers);
  
  SECTION("Engine starts up with factory") {
    auto result = engine.TestStartUp();
    REQUIRE(result.has_value());
  }
  
  SECTION("Factory accessible through engine") {
    auto& factory = engine.GetDataAccessFactory();
    REQUIRE(factory.GetDataFormatType() == steamrot::DataFormatType::FlatBuffers);
  }
}
```

### Mock Data Provider (Future)

**File: `tests/context/MockDataProvider.h`**

```cpp
class MockEngineDataProvider : public IEngineDataProvider {
public:
  std::expected<EngineResourcesConfig, FailInfo>
  LoadEngineResourcesConfig() const override {
    // Return test data
    return EngineResourcesConfig{ /* test values */ };
  }
  // ... implement other methods
};
```

---

## Future Considerations

### Support for Additional Data Formats

Once the factory pattern is in place, adding new formats is straightforward:

#### XML Support Example

```cpp
class XMLEngineDataProvider : public IEngineDataProvider {
private:
  XMLDataLoader m_loader;

public:
  std::expected<EngineResourcesConfig, FailInfo>
  LoadEngineResourcesConfig() const override {
    // Load from XML file
    auto xml_doc = m_loader.LoadXML("engine_resources.xml");
    return ParseEngineResourcesConfigFromXML(xml_doc);
  }
};
```

Update factory:
```cpp
void DataAccessFactory::InitializeProviders() {
  switch (m_data_format_type) {
  case DataFormatType::FlatBuffers:
    m_engine_provider = std::make_unique<FlatbuffersEngineDataProvider>();
    break;
  case DataFormatType::XML:
    m_engine_provider = std::make_unique<XMLEngineDataProvider>();
    break;
  // ... other formats
  }
}
```

### Hybrid Format Support

Support different formats for different data types:

```cpp
class DataAccessFactory {
private:
  DataFormatType m_engine_format;
  DataFormatType m_scene_format;
  DataFormatType m_asset_format;

public:
  DataAccessFactory(
      DataFormatType engine_format,
      DataFormatType scene_format = DataFormatType::FlatBuffers,
      DataFormatType asset_format = DataFormatType::FlatBuffers)
    : m_engine_format(engine_format),
      m_scene_format(scene_format),
      m_asset_format(asset_format) {}
};
```

Use case: Engine config in JSON, scene data in FlatBuffers, assets from XML.

### Runtime Format Switching

For advanced scenarios (hot-reloading, modding):

```cpp
class DataAccessFactory {
public:
  std::expected<std::monostate, FailInfo> 
  SwitchDataFormat(DataFormatType new_format) {
    m_data_format_type = new_format;
    InitializeProviders();
    InitializeConfigurators();
    return std::monostate{};
  }
};
```

### Async Data Loading

Factory could support async providers:

```cpp
class IAsyncEngineDataProvider : public IEngineDataProvider {
public:
  virtual std::future<std::expected<EngineResourcesConfig, FailInfo>>
  LoadEngineResourcesConfigAsync() const = 0;
};
```

---

## Conclusion

The proposed factory pattern solution:

1. **Solves the core problem**: Provides switchable, extensible data format support
2. **Follows existing patterns**: Uses provider/configurator pattern already in codebase
3. **Low risk**: Changes are incremental and backward-compatible
4. **High value**: Enables testing, modding, and future data format support
5. **Well-scoped**: Clear implementation phases with validation at each step

The factory sits naturally in the Engine, provides a single point of configuration, and makes the system easily extensible for future data formats without modifying existing code.

### Recommended Next Steps

1. **Review this analysis** with team/stakeholders
2. **Validate approach** - confirm factory pattern fits requirements
3. **Implement Phase 1** - create foundation without breaking changes
4. **Iterate** - complete phases 2-5 with testing at each step
5. **Document** - update architecture docs with new pattern

### Questions for Discussion

1. Should we support hybrid formats (different formats for different data types)?
2. Do we need runtime format switching, or is initialization-time sufficient?
3. Should configuration come from constructor parameter, config file, or both?
4. What additional data formats should we prioritize (XML, JSON, YAML)?
5. Should the factory be a singleton in Engine, or passed as parameter to subsystems?

---

## References

- `USER_INTERFACE_DECOUPLING_ANALYSIS.md` - Provider/configurator pattern analysis
- `FONT_PROVIDER_DECOUPLING.md` - Interface abstraction example
- Repository code: `src/data_providers/`, `src/interfaces/`, `src/configuration/`
- Repository code: `src/engine/Engine.h`, `src/scenes/SceneFactory.cpp`
