# Case Study: EngineSnapshot Configuration

This document provides a complete case study of how `EngineSnapshot` would be configured using the proposed template-based configurator pattern. EngineSnapshot is an excellent example because:

1. It's used in multiple contexts (TestData and SaveData)
2. Data comes from different sources (test files and save files)
3. It has nested structures requiring other configurators
4. It demonstrates composition, reusability, and the template pattern

## EngineSnapshot Structure Overview

### Native C++ Structure

```cpp
// src/types/core/EngineSnapshot.h
struct EngineSnapshot {
  std::optional<size_t> tick_number;
  std::optional<EventBus> global_event_bus;
  std::optional<SceneManagerData> scene_manager_data;
  SceneCollectionData scene_collection_data;
};
```

### Nested Structures

- **EventBus**: `std::vector<EventPacket>` (collection of events)
- **SceneManagerData**: Contains `SceneManagerState` (nested struct)
- **SceneCollectionData**: `std::vector<SceneData>` (collection of scenes)

### FlatBuffers Schema

```fbs
// src/types/flatbuffers/core/engine_snapshot.fbs
table EngineSnapshotFbs {
  tick_number: uint64;
  global_event_bus: EventBusData;
  scene_manager_data: SceneManagerDataFbs;
  scene_collection_data: SceneCollectionDataFbs;
}
```

### Used In Multiple Contexts

1. **TestData**: Contains `starting_engine_snapshot` and `expected_engine_snapshots`
   - Data source: Test data files (`.test_data.json` → `.test_data.bin`)
   - Schema: `test_data.fbs`

2. **SaveData**: Contains `engine_snapshot`
   - Data source: Save game files (`.save.json` → `.save.bin`)
   - Schema: `save_data.fbs`

## Proposed Template-Based Solution

### Step 1: Create Generic EngineSnapshot Configurator Interface

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Generic interface for EngineSnapshot configuration
/////////////////////////////////////////////////

#pragma once

#include "IConfigurator.h"
#include "EngineSnapshot.h"

namespace steamrot {

// EngineSnapshot gets an interface because it's:
// 1. Standalone (has its own files)
// 2. Reused in multiple contexts (TestData, SaveData)
// 3. Complex (has nested structures)

// No need to define separate interface - use generic IConfigurator
// The template parameters provide the type safety

} // namespace steamrot
```

### Step 2: Create FlatBuffers Configurator for EngineSnapshot

```cpp
/////////////////////////////////////////////////
/// @file FlatbuffersEngineSnapshotConfigurator.h
/// @brief Configures EngineSnapshot from FlatBuffers data
/////////////////////////////////////////////////

#pragma once

#include "ConfiguratorFactory.h"
#include "EngineSnapshot.h"
#include "IConfigurator.h"
#include "engine_snapshot_generated.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Configures EngineSnapshot from FlatBuffers data
///
/// This configurator is reused by both TestData and SaveData
/// configurators through composition.
/////////////////////////////////////////////////
class FlatbuffersEngineSnapshotConfigurator
    : public IConfigurator<EngineSnapshotFbs, EngineSnapshot> {

private:
  EventHandler& m_event_handler;
  ConfiguratorFactory<DataSourceType::Flatbuffers>& m_factory;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor
  ///
  /// @param event_handler Reference to global event handler
  /// @param factory Reference to configurator factory for nested objects
  /////////////////////////////////////////////////
  FlatbuffersEngineSnapshotConfigurator(
      EventHandler& event_handler,
      ConfiguratorFactory<DataSourceType::Flatbuffers>& factory)
      : m_event_handler(event_handler), m_factory(factory) {}

  /////////////////////////////////////////////////
  /// @brief Configure EngineSnapshot from FlatBuffers data
  ///
  /// @param snapshot Native EngineSnapshot object to configure
  /// @param data FlatBuffers EngineSnapshotFbs data
  /// @return std::monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  Configure(EngineSnapshot& snapshot, const EngineSnapshotFbs& data) override;

private:
  /////////////////////////////////////////////////
  /// @brief Configure tick number (simple field - inline)
  /////////////////////////////////////////////////
  void ConfigureTickNumber(EngineSnapshot& snapshot,
                          const EngineSnapshotFbs& data);

  /////////////////////////////////////////////////
  /// @brief Configure EventBus (free function call)
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureEventBus(EngineSnapshot& snapshot, const EngineSnapshotFbs& data);

  /////////////////////////////////////////////////
  /// @brief Configure SceneManagerData (uses dedicated configurator)
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneManagerData(EngineSnapshot& snapshot,
                           const EngineSnapshotFbs& data);

  /////////////////////////////////////////////////
  /// @brief Configure SceneCollectionData (uses dedicated configurator)
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneCollectionData(EngineSnapshot& snapshot,
                              const EngineSnapshotFbs& data);
};

} // namespace steamrot
```

### Step 3: Implementation

```cpp
/////////////////////////////////////////////////
/// @file FlatbuffersEngineSnapshotConfigurator.cpp
/// @brief Implementation of FlatbuffersEngineSnapshotConfigurator
/////////////////////////////////////////////////

#include "FlatbuffersEngineSnapshotConfigurator.h"
#include "event_bus_helpers.h" // Free functions for EventBus

namespace steamrot {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersEngineSnapshotConfigurator::Configure(
    EngineSnapshot& snapshot,
    const EngineSnapshotFbs& data) {

  // 1. Configure simple field directly (tick_number)
  ConfigureTickNumber(snapshot, data);

  // 2. Configure EventBus using free function
  auto event_bus_result = ConfigureEventBus(snapshot, data);
  if (!event_bus_result)
    return std::unexpected(event_bus_result.error());

  // 3. Configure SceneManagerData using its configurator
  auto scene_manager_result = ConfigureSceneManagerData(snapshot, data);
  if (!scene_manager_result)
    return std::unexpected(scene_manager_result.error());

  // 4. Configure SceneCollectionData using its configurator
  auto scene_collection_result = ConfigureSceneCollectionData(snapshot, data);
  if (!scene_collection_result)
    return std::unexpected(scene_collection_result.error());

  return std::monostate{};
}

/////////////////////////////////////////////////
void FlatbuffersEngineSnapshotConfigurator::ConfigureTickNumber(
    EngineSnapshot& snapshot,
    const EngineSnapshotFbs& data) {

  // Simple optional field - set directly
  if (data.tick_number() > 0) {
    snapshot.tick_number = static_cast<size_t>(data.tick_number());
  }
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersEngineSnapshotConfigurator::ConfigureEventBus(
    EngineSnapshot& snapshot,
    const EngineSnapshotFbs& data) {

  // EventBus is simple enough for a free function
  if (data.global_event_bus()) {
    EventBus event_bus;
    auto result = ConfigureEventBusFromFlatbuffers(
        event_bus,
        data.global_event_bus());
    if (!result)
      return std::unexpected(result.error());

    snapshot.global_event_bus = event_bus;
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersEngineSnapshotConfigurator::ConfigureSceneManagerData(
    EngineSnapshot& snapshot,
    const EngineSnapshotFbs& data) {

  // SceneManagerData is complex and reused - use configurator from factory
  if (data.scene_manager_data()) {
    // Get configurator for SceneManagerData
    auto configurator_result = m_factory.GetConfigurator<
        SceneManagerDataFbs, SceneManagerData>();

    if (!configurator_result)
      return std::unexpected(configurator_result.error());

    // Create SceneManagerData and configure it
    SceneManagerData scene_manager_data;
    auto config_result = configurator_result.value()->Configure(
        scene_manager_data,
        *data.scene_manager_data());

    if (!config_result)
      return std::unexpected(config_result.error());

    snapshot.scene_manager_data = scene_manager_data;
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersEngineSnapshotConfigurator::ConfigureSceneCollectionData(
    EngineSnapshot& snapshot,
    const EngineSnapshotFbs& data) {

  // SceneCollectionData is complex and reused - use configurator
  if (data.scene_collection_data()) {
    // Get configurator for SceneCollectionData
    auto configurator_result = m_factory.GetConfigurator<
        SceneCollectionDataFbs, SceneCollectionData>();

    if (!configurator_result)
      return std::unexpected(configurator_result.error());

    // Configure scene collection directly into snapshot
    auto config_result = configurator_result.value()->Configure(
        snapshot.scene_collection_data,
        *data.scene_collection_data());

    if (!config_result)
      return std::unexpected(config_result.error());
  }

  return std::monostate{};
}

} // namespace steamrot
```

### Step 4: Register in ConfiguratorRegistry

```cpp
/////////////////////////////////////////////////
/// @file FlatbuffersEngineSnapshotConfiguratorRegistry.h
/// @brief Registry specialization for EngineSnapshot
/////////////////////////////////////////////////

#pragma once

#include "ConfiguratorRegistry.h"
#include "EngineSnapshot.h"
#include "FlatbuffersEngineSnapshotConfigurator.h"
#include "engine_snapshot_generated.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Registry specialization for FlatBuffers EngineSnapshot
///
/// This is the ONLY place that knows about:
/// - DataSourceType::Flatbuffers
/// - EngineSnapshotFbs (FlatBuffers type)
/// - EngineSnapshot (native type)
/// - FlatbuffersEngineSnapshotConfigurator (implementation)
/////////////////////////////////////////////////
template<>
struct ConfiguratorRegistry<DataSourceType::Flatbuffers,
                           EngineSnapshotFbs,
                           EngineSnapshot> {
  using ConfiguratorType = IConfigurator<EngineSnapshotFbs, EngineSnapshot>;

  static std::unique_ptr<ConfiguratorType>
  CreateConfigurator(EventHandler& event_handler,
                    ConfiguratorFactory<DataSourceType::Flatbuffers>& factory) {
    return std::make_unique<FlatbuffersEngineSnapshotConfigurator>(
        event_handler, factory);
  }
};

} // namespace steamrot
```

### Step 5: Usage in TestData Configurator

```cpp
/////////////////////////////////////////////////
/// @file FlatbuffersTestDataConfigurator.h
/// @brief Configures TestData from FlatBuffers
/////////////////////////////////////////////////

#pragma once

#include "ConfiguratorFactory.h"
#include "IConfigurator.h"
#include "TestData.h"
#include "test_data_generated.h"

namespace steamrot {

class FlatbuffersTestDataConfigurator
    : public IConfigurator<TestDataFbs, TestData> {

private:
  EventHandler& m_event_handler;
  ConfiguratorFactory<DataSourceType::Flatbuffers>& m_factory;

public:
  FlatbuffersTestDataConfigurator(
      EventHandler& event_handler,
      ConfiguratorFactory<DataSourceType::Flatbuffers>& factory)
      : m_event_handler(event_handler), m_factory(factory) {}

  std::expected<std::monostate, FailInfo>
  Configure(TestData& test_data, const TestDataFbs& data) override {

    // Configure metadata (free function - simple)
    ConfigureTestMetaData(test_data.meta_data, data.meta_data());

    // Configure simulation data (another configurator)
    if (data.simulation_data()) {
      auto sim_config = m_factory.GetConfigurator<
          SimulationDataFbs, SimulationData>();
      sim_config.value()->Configure(
          test_data.simulation_data,
          *data.simulation_data());
    }

    // Configure number of ticks (simple)
    test_data.number_of_ticks = data.num_ticks();

    // Configure EngineSnapshot using its configurator
    // *** KEY POINT: Reuses EngineSnapshot configurator ***
    if (data.starting_engine_snapshot()) {
      auto engine_snapshot_config = m_factory.GetConfigurator<
          EngineSnapshotFbs, EngineSnapshot>();

      if (!engine_snapshot_config)
        return std::unexpected(engine_snapshot_config.error());

      auto result = engine_snapshot_config.value()->Configure(
          test_data.starting_engine_snapshot,
          *data.starting_engine_snapshot());

      if (!result)
        return std::unexpected(result.error());
    }

    return std::monostate{};
  }
};

} // namespace steamrot
```

### Step 6: Usage in SaveData Configurator

```cpp
/////////////////////////////////////////////////
/// @file FlatbuffersSaveDataConfigurator.h
/// @brief Configures SaveData from FlatBuffers
/////////////////////////////////////////////////

#pragma once

#include "ConfiguratorFactory.h"
#include "IConfigurator.h"
#include "SaveData.h"
#include "save_data_generated.h"

namespace steamrot {

class FlatbuffersSaveDataConfigurator
    : public IConfigurator<SaveDataFbs, SaveData> {

private:
  EventHandler& m_event_handler;
  ConfiguratorFactory<DataSourceType::Flatbuffers>& m_factory;

public:
  FlatbuffersSaveDataConfigurator(
      EventHandler& event_handler,
      ConfiguratorFactory<DataSourceType::Flatbuffers>& factory)
      : m_event_handler(event_handler), m_factory(factory) {}

  std::expected<std::monostate, FailInfo>
  Configure(SaveData& save_data, const SaveDataFbs& data) override {

    // Configure save metadata (free function - simple)
    if (data.save_meta_data()) {
      ConfigureSaveMetaData(
          save_data.meta_data,
          data.save_meta_data());
    }

    // Note: SaveDataFbs doesn't directly have engine_snapshot in current schema
    // but if it did, it would look like this:
    //
    // Configure EngineSnapshot using its configurator
    // *** KEY POINT: Same configurator reused as in TestData ***
    /*
    if (data.engine_snapshot()) {
      auto engine_snapshot_config = m_factory.GetConfigurator<
          EngineSnapshotFbs, EngineSnapshot>();

      if (!engine_snapshot_config)
        return std::unexpected(engine_snapshot_config.error());

      auto result = engine_snapshot_config.value()->Configure(
          save_data.engine_snapshot,
          *data.engine_snapshot());

      if (!result)
        return std::unexpected(result.error());
    }
    */

    // Configure scene collection data
    if (data.scene_collection_data()) {
      auto scene_collection_config = m_factory.GetConfigurator<
          SceneCollectionDataFbs, SceneCollectionData>();

      scene_collection_config.value()->Configure(
          save_data.scene_collection_data,
          *data.scene_collection_data());
    }

    return std::monostate{};
  }
};

} // namespace steamrot
```

## Complete Flow Diagram

```
TestData Configuration Flow:
============================

TestDataFbs (FlatBuffers)
    │
    ▼
FlatbuffersTestDataConfigurator
    │
    ├─ meta_data → Free function
    ├─ simulation_data → SimulationData configurator
    ├─ number_of_ticks → Direct assignment
    │
    └─ starting_engine_snapshot
           │
           ▼
       Get EngineSnapshot configurator from factory
           │
           ▼
       FlatbuffersEngineSnapshotConfigurator  ◄─── REUSED
           │
           ├─ tick_number → Direct assignment
           ├─ global_event_bus → Free function
           ├─ scene_manager_data → SceneManagerData configurator
           └─ scene_collection_data → SceneCollectionData configurator


SaveData Configuration Flow:
===========================

SaveDataFbs (FlatBuffers)
    │
    ▼
FlatbuffersSaveDataConfigurator
    │
    ├─ save_meta_data → Free function
    │
    └─ engine_snapshot (hypothetical)
           │
           ▼
       Get EngineSnapshot configurator from factory
           │
           ▼
       FlatbuffersEngineSnapshotConfigurator  ◄─── SAME INSTANCE REUSED
           │
           └─ (same configuration as above)
```

## Key Takeaways from EngineSnapshot Case Study

### 1. Reusability Demonstrated

**EngineSnapshot configurator is created once, used twice:**

```cpp
// Factory creates and caches the configurator
ConfiguratorFactory<DataSourceType::Flatbuffers> factory(event_handler);

// TestData configurator gets it
auto config1 = factory.GetConfigurator<EngineSnapshotFbs, EngineSnapshot>();

// SaveData configurator gets the SAME instance
auto config2 = factory.GetConfigurator<EngineSnapshotFbs, EngineSnapshot>();

// config1 == config2 (same pointer)
```

### 2. Composition in Action

**EngineSnapshot configurator calls nested configurators:**

```cpp
// EngineSnapshot contains SceneManagerData
// Get its configurator and use it
auto scene_mgr_config = m_factory.GetConfigurator<
    SceneManagerDataFbs, SceneManagerData>();

scene_mgr_config->Configure(
    snapshot.scene_manager_data.value(),
    *data.scene_manager_data());
```

### 3. Decision Tree Applied

| Component | Decision | Rationale |
|-----------|----------|-----------|
| EngineSnapshot | Interface | Standalone, complex, reused |
| tick_number | Direct assignment | Simple primitive |
| global_event_bus | Free function | Vector conversion, not complex enough for interface |
| SceneManagerData | Configurator interface | Complex nested struct, potential reuse |
| SceneCollectionData | Configurator interface | Complex collection, reused |

### 4. Multiple Data Sources

**Same configurator works for both TestData and SaveData:**

```cpp
// Both use FlatBuffers as data source
// Same registry specialization serves both:

template<>
struct ConfiguratorRegistry<DataSourceType::Flatbuffers,
                           EngineSnapshotFbs,
                           EngineSnapshot> {
  // Used by both TestData and SaveData configurators
};
```

**If we add JSON support for testing:**

```cpp
// Just add new specialization
template<>
struct ConfiguratorRegistry<DataSourceType::JSON,
                           JSONEngineSnapshot,
                           EngineSnapshot> {
  static auto CreateConfigurator(...) {
    return std::make_unique<JSONEngineSnapshotConfigurator>(...);
  }
};

// TestData can now use either FlatBuffers or JSON
// SaveData still uses FlatBuffers
// Both coexist without modification to existing code
```

### 5. Type Safety Maintained

```cpp
// Full type information throughout
IConfigurator<EngineSnapshotFbs, EngineSnapshot>* config = ...;
                ^^^^^^^^^^^^^^^^^^  ^^^^^^^^^^^^^^
                Data type           Object type

// No casting needed
config->Configure(snapshot, flatbuffers_data);
//                ^^^^^^^^^  ^^^^^^^^^^^^^^^^
//                Native     Concrete FlatBuffers type
```

### 6. No Coupling in Parent Configurators

**TestData configurator doesn't know:**
- How EngineSnapshot is configured
- What data format EngineSnapshot uses
- Implementation details of EngineSnapshot

**It only knows:**
```cpp
// Interface type (generic)
IConfigurator<EngineSnapshotFbs, EngineSnapshot>* config;

// How to request it
config = factory.GetConfigurator<EngineSnapshotFbs, EngineSnapshot>();

// How to use it
config->Configure(test_data.starting_engine_snapshot, *data.starting_engine_snapshot());
```

## Registry Specializations Needed

```cpp
// 1. EngineSnapshot (main type)
template<>
struct ConfiguratorRegistry<Flatbuffers, EngineSnapshotFbs, EngineSnapshot>;

// 2. SceneManagerData (nested complex type)
template<>
struct ConfiguratorRegistry<Flatbuffers, SceneManagerDataFbs, SceneManagerData>;

// 3. SceneCollectionData (nested complex type)
template<>
struct ConfiguratorRegistry<Flatbuffers, SceneCollectionDataFbs, SceneCollectionData>;

// 4. TestData (parent type)
template<>
struct ConfiguratorRegistry<Flatbuffers, TestDataFbs, TestData>;

// 5. SaveData (parent type)
template<>
struct ConfiguratorRegistry<Flatbuffers, SaveDataFbs, SaveData>;
```

**Total coupling points: 5 specializations**
**All other code uses interfaces - no coupling**

## Testing Example

```cpp
TEST_CASE("EngineSnapshot configuration from FlatBuffers", "[unit]") {
  EventHandler event_handler;
  ConfiguratorFactory<DataSourceType::Flatbuffers> factory(event_handler);

  // Load FlatBuffers data
  FlatbuffersDataLoader loader;
  auto fb_data = loader.LoadEngineSnapshot("test_snapshot.bin");

  // Get configurator
  auto configurator = factory.GetConfigurator<
      EngineSnapshotFbs, EngineSnapshot>();

  REQUIRE(configurator.has_value());

  // Configure
  EngineSnapshot snapshot;
  auto result = configurator.value()->Configure(snapshot, *fb_data);

  REQUIRE(result.has_value());
  REQUIRE(snapshot.tick_number.has_value());
  REQUIRE(snapshot.tick_number.value() == 42);
}

TEST_CASE("EngineSnapshot configurator is cached", "[unit]") {
  EventHandler event_handler;
  ConfiguratorFactory<DataSourceType::Flatbuffers> factory(event_handler);

  // Get configurator twice
  auto config1 = factory.GetConfigurator<EngineSnapshotFbs, EngineSnapshot>();
  auto config2 = factory.GetConfigurator<EngineSnapshotFbs, EngineSnapshot>();

  // Should be same instance
  REQUIRE(config1.value() == config2.value());
}
```

## Summary

The EngineSnapshot case study demonstrates:

1. **Template-based interfaces** preserve type information (no type erasure)
2. **Registry specializations** confine coupling to explicit points
3. **Composition** enables reuse (EngineSnapshot config used by TestData and SaveData)
4. **Factory caching** optimizes performance (single instance reused)
5. **Nested configurators** handle complex structures cleanly
6. **Free functions** handle simple conversions without interface overhead
7. **Multiple data sources** supported without modifying existing code

This approach provides a clear, reusable, and type-safe way to configure EngineSnapshot from different data sources while maintaining clean separation of concerns.
