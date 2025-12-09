# Scene Configurator Implementation Guide

**Quick-start guide for implementing the Scene configurator pattern**

---

## Start Here

If you're ready to implement the Scene configurator pattern, follow this step-by-step guide.

**Prerequisites**: Read [SCENE_CONFIGURATION_WORKFLOW_SUMMARY.md](SCENE_CONFIGURATION_WORKFLOW_SUMMARY.md) first.

---

## Implementation Order

Implement in this order to minimize breaking changes:

1. ✅ **ISceneConfigurator Interface** - New file, no dependencies
2. ✅ **DefaultSceneConfigurator** - New file, uses existing ISceneDataProvider
3. ✅ **SavedSceneConfigurator** - New file, uses existing ISaveDataProvider
4. ⚠️ **ISceneFactory Update** - BREAKING: Changes constructor
5. ⚠️ **FlatbuffersSceneFactory Update** - BREAKING: Uses configurator
6. ⚠️ **SceneManager Update** - BREAKING: New API

---

## Step 1: ISceneConfigurator Interface

### File: `src/scenes/ISceneConfigurator.h`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Strategy interface for Scene configuration from different data sources.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventHandler.h"
#include "IEntityConfigurator.h"
#include "scene_data_generated.h"
#include "scene_types_generated.h"
#include <memory>

namespace steamrot {

/////////////////////////////////////////////////
/// @class ISceneConfigurator
/// @brief Strategy interface for Scene configuration.
///
/// Implementations encapsulate different data sources (default files,
/// save files, test data, etc.) and provide unified access to FlatBuffers
/// scene data for Scene configuration.
///
/// ## Responsibilities
/// 1. Load scene data from specific source
/// 2. Cache FlatBuffers data for reuse during configuration
/// 3. Provide SceneDataFbs* to SceneFactory
/// 4. Create appropriate EntityConfigurator for the data source
///
/// ## Lifecycle
/// - Created by SceneManager
/// - Passed to SceneFactory by reference
/// - Lives for duration of Scene creation
/// - Stack-allocated (not heap)
///
/// ## Example Usage
/// ```cpp
/// ISceneDataProvider &provider = GetSceneDataProvider();
/// DefaultSceneConfigurator config(SceneType::TITLE, provider);
/// FlatbuffersSceneFactory factory(game_context, config);
/// auto scene = factory.CreateScene();
/// ```
/////////////////////////////////////////////////
class ISceneConfigurator {
public:
  /////////////////////////////////////////////////
  /// @brief Virtual destructor for proper cleanup.
  /////////////////////////////////////////////////
  virtual ~ISceneConfigurator() = default;

  /////////////////////////////////////////////////
  /// @brief Get FlatBuffers scene data for configuration.
  ///
  /// This method may load data on first call and cache it for
  /// subsequent calls during Scene configuration.
  ///
  /// @return Pointer to const SceneDataFbs, or nullptr on error
  /////////////////////////////////////////////////
  virtual const SceneDataFbs *GetSceneData() const = 0;

  /////////////////////////////////////////////////
  /// @brief Create an entity configurator for this data source.
  ///
  /// The returned configurator is configured to use the entity
  /// collection data from this scene's data source.
  ///
  /// @param event_handler Reference to the EventHandler for subscriber creation
  /// @return Unique pointer to entity configurator
  /////////////////////////////////////////////////
  virtual std::unique_ptr<IEntityConfigurator>
  CreateEntityConfigurator(EventHandler &event_handler) const = 0;

  /////////////////////////////////////////////////
  /// @brief Get the scene type being configured.
  ///
  /// @return SceneType enum value
  /////////////////////////////////////////////////
  virtual SceneType GetSceneType() const = 0;
};

} // namespace steamrot
```

### Test: `tests/scenes/ISceneConfigurator.test.cpp`

```cpp
#include "ISceneConfigurator.h"
#include <catch2/catch_test_macros.hpp>

// Mock configurator for testing
class MockSceneConfigurator : public steamrot::ISceneConfigurator {
private:
  const steamrot::SceneDataFbs *m_test_data;
  steamrot::SceneType m_scene_type;

public:
  MockSceneConfigurator(const steamrot::SceneDataFbs *data,
                       steamrot::SceneType type)
      : m_test_data(data), m_scene_type(type) {}

  const steamrot::SceneDataFbs *GetSceneData() const override {
    return m_test_data;
  }

  std::unique_ptr<steamrot::IEntityConfigurator>
  CreateEntityConfigurator(steamrot::EventHandler &) const override {
    return nullptr; // Mock
  }

  steamrot::SceneType GetSceneType() const override {
    return m_scene_type;
  }
};

TEST_CASE("ISceneConfigurator interface can be mocked", 
          "[unit][ISceneConfigurator]") {
  MockSceneConfigurator config(nullptr, steamrot::SceneType::SceneType_TITLE);
  
  REQUIRE(config.GetSceneType() == steamrot::SceneType::SceneType_TITLE);
  REQUIRE(config.GetSceneData() == nullptr);
}
```

---

## Step 2: DefaultSceneConfigurator

### File: `src/scenes/DefaultSceneConfigurator.h`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Configurator for loading Scenes from default data files.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ISceneConfigurator.h"
#include "ISceneDataProvider.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @class DefaultSceneConfigurator
/// @brief Loads Scenes from default scene data files.
///
/// Uses ISceneDataProvider to load default scene data and provides
/// FlatBuffers scene data to SceneFactory for configuration.
/////////////////////////////////////////////////
class DefaultSceneConfigurator : public ISceneConfigurator {
private:
  /////////////////////////////////////////////////
  /// @brief Scene type to load
  /////////////////////////////////////////////////
  SceneType m_scene_type;

  /////////////////////////////////////////////////
  /// @brief Reference to scene data provider
  /////////////////////////////////////////////////
  ISceneDataProvider &m_scene_data_provider;

  /////////////////////////////////////////////////
  /// @brief Cached FlatBuffers data (loaded on first GetSceneData() call)
  /////////////////////////////////////////////////
  mutable const SceneDataFbs *m_cached_scene_data{nullptr};

public:
  /////////////////////////////////////////////////
  /// @brief Constructor.
  ///
  /// @param scene_type The scene type to load
  /// @param scene_data_provider Reference to data provider
  /////////////////////////////////////////////////
  DefaultSceneConfigurator(SceneType scene_type,
                          ISceneDataProvider &scene_data_provider);

  /////////////////////////////////////////////////
  /// @brief Get FlatBuffers scene data.
  ///
  /// Loads from provider on first call, caches for subsequent calls.
  ///
  /// @return Pointer to const SceneDataFbs, or nullptr on error
  /////////////////////////////////////////////////
  const SceneDataFbs *GetSceneData() const override;

  /////////////////////////////////////////////////
  /// @brief Create entity configurator for default scene data.
  ///
  /// @param event_handler Reference to EventHandler
  /// @return Unique pointer to FlatbuffersEntityConfigurator
  /////////////////////////////////////////////////
  std::unique_ptr<IEntityConfigurator>
  CreateEntityConfigurator(EventHandler &event_handler) const override;

  /////////////////////////////////////////////////
  /// @brief Get the scene type.
  ///
  /// @return SceneType enum value
  /////////////////////////////////////////////////
  SceneType GetSceneType() const override;
};

} // namespace steamrot
```

### File: `src/scenes/DefaultSceneConfigurator.cpp`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Implementation of DefaultSceneConfigurator.
/////////////////////////////////////////////////

#include "DefaultSceneConfigurator.h"
#include "FlatbuffersDataLoader.h"
#include "FlatbuffersEntityConfigurator.h"

namespace steamrot {

/////////////////////////////////////////////////
DefaultSceneConfigurator::DefaultSceneConfigurator(
    SceneType scene_type, ISceneDataProvider &scene_data_provider)
    : m_scene_type(scene_type), m_scene_data_provider(scene_data_provider) {}

/////////////////////////////////////////////////
const SceneDataFbs *DefaultSceneConfigurator::GetSceneData() const {
  // Return cached data if available
  if (m_cached_scene_data) {
    return m_cached_scene_data;
  }

  // Load scene data from provider
  // Note: Provider returns SceneData struct, we need to load the FlatBuffers
  // This requires access to FlatbuffersDataLoader
  FlatbuffersDataLoader loader;
  auto scene_data_result = loader.LoadSceneData(m_scene_type);

  if (!scene_data_result.has_value()) {
    // Log error, return nullptr
    return nullptr;
  }

  // Cache and return FlatBuffers data
  m_cached_scene_data = scene_data_result.value();
  return m_cached_scene_data;
}

/////////////////////////////////////////////////
std::unique_ptr<IEntityConfigurator>
DefaultSceneConfigurator::CreateEntityConfigurator(
    EventHandler &event_handler) const {
  const SceneDataFbs *scene_data = GetSceneData();
  if (!scene_data || !scene_data->entity_collection()) {
    return nullptr;
  }

  return std::make_unique<FlatbuffersEntityConfigurator>(
      event_handler, *scene_data->entity_collection());
}

/////////////////////////////////////////////////
SceneType DefaultSceneConfigurator::GetSceneType() const {
  return m_scene_type;
}

} // namespace steamrot
```

### Test: `tests/scenes/DefaultSceneConfigurator.test.cpp`

```cpp
#include "DefaultSceneConfigurator.h"
#include "provider_factory.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("DefaultSceneConfigurator loads and caches scene data",
          "[unit][DefaultSceneConfigurator]") {
  // Get real provider
  steamrot::ISceneDataProvider &provider = steamrot::GetSceneDataProvider();

  steamrot::DefaultSceneConfigurator config(
      steamrot::SceneType::SceneType_TITLE, provider);

  SECTION("GetSceneType returns correct type") {
    REQUIRE(config.GetSceneType() == steamrot::SceneType::SceneType_TITLE);
  }

  SECTION("GetSceneData loads data on first call") {
    const steamrot::SceneDataFbs *data1 = config.GetSceneData();
    REQUIRE(data1 != nullptr);

    // Second call returns same cached data
    const steamrot::SceneDataFbs *data2 = config.GetSceneData();
    REQUIRE(data2 == data1);
  }

  SECTION("CreateEntityConfigurator returns valid configurator") {
    steamrot::EventHandler handler;
    auto entity_config = config.CreateEntityConfigurator(handler);
    REQUIRE(entity_config != nullptr);
  }
}
```

---

## Step 3: SavedSceneConfigurator

### File: `src/scenes/SavedSceneConfigurator.h`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Configurator for loading Scenes from save data files.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ISceneConfigurator.h"
#include "ISaveDataProvider.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @class SavedSceneConfigurator
/// @brief Loads Scenes from saved game data.
///
/// Uses ISaveDataProvider to load save data and extracts SceneDataFbs
/// from SaveDataFbs for Scene configuration.
/////////////////////////////////////////////////
class SavedSceneConfigurator : public ISceneConfigurator {
private:
  /////////////////////////////////////////////////
  /// @brief Save slot index to load from
  /////////////////////////////////////////////////
  uint32_t m_save_slot_index;

  /////////////////////////////////////////////////
  /// @brief Reference to save data provider
  /////////////////////////////////////////////////
  ISaveDataProvider &m_save_data_provider;

  /////////////////////////////////////////////////
  /// @brief Cached FlatBuffers data (loaded on first GetSceneData() call)
  /////////////////////////////////////////////////
  mutable const SceneDataFbs *m_cached_scene_data{nullptr};

public:
  /////////////////////////////////////////////////
  /// @brief Constructor.
  ///
  /// @param save_slot_index The save slot to load from
  /// @param save_data_provider Reference to save data provider
  /////////////////////////////////////////////////
  SavedSceneConfigurator(uint32_t save_slot_index,
                        ISaveDataProvider &save_data_provider);

  /////////////////////////////////////////////////
  /// @brief Get FlatBuffers scene data from save file.
  ///
  /// Loads SaveDataFbs from provider, extracts SceneDataFbs field.
  ///
  /// @return Pointer to const SceneDataFbs, or nullptr on error
  /////////////////////////////////////////////////
  const SceneDataFbs *GetSceneData() const override;

  /////////////////////////////////////////////////
  /// @brief Create entity configurator for saved scene data.
  ///
  /// @param event_handler Reference to EventHandler
  /// @return Unique pointer to FlatbuffersEntityConfigurator
  /////////////////////////////////////////////////
  std::unique_ptr<IEntityConfigurator>
  CreateEntityConfigurator(EventHandler &event_handler) const override;

  /////////////////////////////////////////////////
  /// @brief Get the scene type from save data.
  ///
  /// @return SceneType enum value
  /////////////////////////////////////////////////
  SceneType GetSceneType() const override;
};

} // namespace steamrot
```

### File: `src/scenes/SavedSceneConfigurator.cpp`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Implementation of SavedSceneConfigurator.
/////////////////////////////////////////////////

#include "SavedSceneConfigurator.h"
#include "FlatbuffersEntityConfigurator.h"
#include "save_data_generated.h"

namespace steamrot {

/////////////////////////////////////////////////
SavedSceneConfigurator::SavedSceneConfigurator(
    uint32_t save_slot_index, ISaveDataProvider &save_data_provider)
    : m_save_slot_index(save_slot_index),
      m_save_data_provider(save_data_provider) {}

/////////////////////////////////////////////////
const SceneDataFbs *SavedSceneConfigurator::GetSceneData() const {
  // Return cached data if available
  if (m_cached_scene_data) {
    return m_cached_scene_data;
  }

  // Load save data from provider
  auto save_result = m_save_data_provider.LoadSave(m_save_slot_index);
  if (!save_result.has_value()) {
    return nullptr;
  }

  // TODO: Extract SceneDataFbs from SaveData
  // When SaveData is extended to include scene_data field:
  // const SaveDataFbs *save_data_fbs = GetSaveDataFbs(save_result.value());
  // m_cached_scene_data = save_data_fbs->scene_data();

  // For now, return nullptr (save data doesn't contain scene data yet)
  return nullptr;
}

/////////////////////////////////////////////////
std::unique_ptr<IEntityConfigurator>
SavedSceneConfigurator::CreateEntityConfigurator(
    EventHandler &event_handler) const {
  const SceneDataFbs *scene_data = GetSceneData();
  if (!scene_data || !scene_data->entity_collection()) {
    return nullptr;
  }

  return std::make_unique<FlatbuffersEntityConfigurator>(
      event_handler, *scene_data->entity_collection());
}

/////////////////////////////////////////////////
SceneType SavedSceneConfigurator::GetSceneType() const {
  const SceneDataFbs *scene_data = GetSceneData();
  if (!scene_data || !scene_data->scene_info()) {
    return SceneType::SceneType_UNKNOWN;
  }

  return scene_data->scene_info()->scene_type();
}

} // namespace steamrot
```

---

## Step 4: Update ISceneFactory

### Update: `src/scenes/ISceneFactory.h`

```cpp
// BEFORE
class ISceneFactory {
protected:
  std::unique_ptr<IEntityConfigurator> m_entity_configurator{nullptr};
  const GameContext &m_game_context;
  SceneType scene_type{SceneType::SceneType_UNKNOWN};

public:
  ISceneFactory(const GameContext &game_context);
  // ...
};

// AFTER
class ISceneFactory {
protected:
  const ISceneConfigurator &m_scene_configurator;  // NEW
  std::unique_ptr<IEntityConfigurator> m_entity_configurator{nullptr};
  const GameContext &m_game_context;
  SceneType scene_type{SceneType::SceneType_UNKNOWN};

public:
  ISceneFactory(const GameContext &game_context,
                const ISceneConfigurator &scene_configurator);  // CHANGED
  // ...
};
```

### Update: `src/scenes/ISceneFactory.cpp`

```cpp
// Update constructor
ISceneFactory::ISceneFactory(const GameContext &game_context,
                             const ISceneConfigurator &scene_configurator)
    : m_game_context(game_context),
      m_scene_configurator(scene_configurator) {
  // Set scene_type from configurator
  scene_type = m_scene_configurator.GetSceneType();
}

// Update CreateScene to use configurator
std::expected<std::unique_ptr<Scene>, FailInfo> ISceneFactory::CreateScene() {
  // Create entity configurator from scene configurator
  m_entity_configurator = m_scene_configurator.CreateEntityConfigurator(
      m_game_context.event_handler);

  if (!m_entity_configurator) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer,
                 "Failed to create entity configurator"});
  }

  // Rest of CreateScene implementation...
}
```

---

## Step 5: Update FlatbuffersSceneFactory

### Update: `src/scenes/FlatbuffersSceneFactory.h`

```cpp
// BEFORE
class FlatbuffersSceneFactory : public ISceneFactory {
private:
  const SceneDataFbs *m_scene_data_fbs;  // REMOVE THIS

public:
  FlatbuffersSceneFactory(const GameContext &game_context,
                          const SceneDataFbs *scene_data_fbs);  // OLD
  // ...
};

// AFTER
class FlatbuffersSceneFactory : public ISceneFactory {
  // No additional members! Uses m_scene_configurator from base

public:
  FlatbuffersSceneFactory(const GameContext &game_context,
                          const ISceneConfigurator &scene_configurator);  // NEW
  // ...
};
```

### Update: `src/scenes/FlatbuffersSceneFactory.cpp`

```cpp
// Update constructor
FlatbuffersSceneFactory::FlatbuffersSceneFactory(
    const GameContext &game_context,
    const ISceneConfigurator &scene_configurator)
    : ISceneFactory(game_context, scene_configurator) {
  // Constructor body can be empty - base class handles everything
}

// Update ConfigureSceneResources to use configurator
std::expected<std::monostate, FailInfo>
FlatbuffersSceneFactory::ConfigureSceneResources(Scene &scene) {
  // Get scene data from configurator
  const SceneDataFbs *scene_data = m_scene_configurator.GetSceneData();

  if (!scene_data) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer,
                 "SceneData is null in ConfigureSceneResources"});
  }

  // Rest of configuration using scene_data...
}
```

---

## Step 6: Update SceneManager

### Update: `src/scenes/SceneManager.h`

```cpp
// Add new methods
class SceneManager {
public:
  // ...existing methods...

  /////////////////////////////////////////////////
  /// @brief Load a scene from default data files.
  ///
  /// @param scene_type The type of scene to load
  /// @return Success or failure information
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  LoadSceneFromDefault(SceneType scene_type);

  /////////////////////////////////////////////////
  /// @brief Load a scene from a save file.
  ///
  /// @param save_slot_index The save slot to load from
  /// @return Success or failure information
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  LoadSceneFromSave(uint32_t save_slot_index);
};
```

### Update: `src/scenes/SceneManager.cpp`

```cpp
#include "DefaultSceneConfigurator.h"
#include "SavedSceneConfigurator.h"
#include "provider_factory.h"

std::expected<std::monostate, FailInfo>
SceneManager::LoadSceneFromDefault(SceneType scene_type) {
  // 1. Get data provider
  ISceneDataProvider &provider = GetSceneDataProvider();

  // 2. Create configurator
  DefaultSceneConfigurator configurator(scene_type, provider);

  // 3. Create factory
  FlatbuffersSceneFactory factory(m_game_context, configurator);

  // 4. Create scene
  auto scene_result = factory.CreateScene();
  if (!scene_result.has_value()) {
    return std::unexpected(scene_result.error());
  }

  // 5. Store scene
  auto scene_id = scene_result.value()->GetSceneInfo().id;
  m_scenes.emplace(scene_id, std::move(scene_result.value()));

  // 6. Load assets
  auto asset_result = m_game_context.asset_manager.LoadSceneAssets(scene_type);
  if (!asset_result.has_value()) {
    return std::unexpected(asset_result.error());
  }

  return std::monostate{};
}

std::expected<std::monostate, FailInfo>
SceneManager::LoadSceneFromSave(uint32_t save_slot_index) {
  // 1. Get save provider
  ISaveDataProvider &provider = GetSaveDataProvider();

  // 2. Create configurator
  SavedSceneConfigurator configurator(save_slot_index, provider);

  // 3-6: Same as LoadSceneFromDefault
  FlatbuffersSceneFactory factory(m_game_context, configurator);

  auto scene_result = factory.CreateScene();
  if (!scene_result.has_value()) {
    return std::unexpected(scene_result.error());
  }

  auto scene_id = scene_result.value()->GetSceneInfo().id;
  m_scenes.emplace(scene_id, std::move(scene_result.value()));

  // Get scene type from configurator for assets
  SceneType scene_type = configurator.GetSceneType();
  auto asset_result = m_game_context.asset_manager.LoadSceneAssets(scene_type);
  if (!asset_result.has_value()) {
    return std::unexpected(asset_result.error());
  }

  return std::monostate{};
}
```

---

## CMakeLists.txt Updates

### In `src/scenes/CMakeLists.txt`

```cmake
# Add new source files
target_sources(scenes PRIVATE
  # ... existing files ...
  DefaultSceneConfigurator.cpp
  SavedSceneConfigurator.cpp
)
```

### In `tests/scenes/CMakeLists.txt`

```cmake
# Add new test files
add_executable(test_scenes
  # ... existing tests ...
  ISceneConfigurator.test.cpp
  DefaultSceneConfigurator.test.cpp
  SavedSceneConfigurator.test.cpp
)
```

---

## Testing Checklist

- [ ] Unit test: ISceneConfigurator with mock
- [ ] Unit test: DefaultSceneConfigurator loads and caches data
- [ ] Unit test: SavedSceneConfigurator extracts scene data
- [ ] Integration test: SceneFactory with DefaultSceneConfigurator
- [ ] Integration test: SceneFactory with SavedSceneConfigurator
- [ ] Integration test: SceneManager.LoadSceneFromDefault()
- [ ] Integration test: SceneManager.LoadSceneFromSave()
- [ ] End-to-end test: Full scene loading workflow

---

## Common Issues

### Issue: "Cannot find ISceneConfigurator.h"

**Solution**: Make sure to add include directories in CMakeLists.txt

### Issue: "Configurator returns nullptr from GetSceneData()"

**Debug steps**:
1. Check provider is loading data correctly
2. Check FlatBuffers extraction logic
3. Add logging in GetSceneData() to see where it fails

### Issue: "Factory constructor ambiguous"

**Solution**: Make sure old constructor is completely removed, not just deprecated

---

## Validation

After implementation, verify:

1. ✅ Factory has NO FlatBuffers member variables
2. ✅ Factory constructor takes `ISceneConfigurator&`
3. ✅ SceneManager creates configurators
4. ✅ Same factory code for both default and save loading
5. ✅ Tests pass for both loading paths

---

## Related Documentation

- [Complete Analysis](SCENE_FACTORY_AND_CONFIGURATOR_ANALYSIS.md)
- [Quick Reference](SCENE_FACTORY_QUICK_REF.md)
- [Visual Diagrams](SCENE_FACTORY_VISUALS.md)
- [Workflow Summary](SCENE_CONFIGURATION_WORKFLOW_SUMMARY.md)
