# Scene Factory and Configurator Architecture Analysis

**Date**: December 8, 2025  
**Context**: Analysis for refactoring SceneManager data sourcing and configuration patterns  
**Status**: Planning/Analysis Phase

---

## Executive Summary

This analysis addresses the architecture for:
1. **SceneManager data sourcing** - Handling SceneData from either ISceneDataProvider (defaults) or ISaveDataProvider (saved games)
2. **Scene factory pattern** - Abstract factory vs overloaded configuration for Scene creation
3. **EntityConfigurator abstraction** - Moving from overloaded methods to proper abstract base with virtual functions

### Key Recommendations

- ✅ **SceneManager as orchestrator**: SceneManager determines data source and delegates to appropriate provider
- ✅ **Abstract Factory Pattern**: Use abstract factory for Scene configuration with abstract configurators
- ✅ **Abstract EntityConfigurator**: Define virtual methods instead of overloaded ConfigureComponent methods
- ✅ **Avoid Scene::Configure()**: Keep Scene as pure structural container, configuration happens externally

---

## Table of Contents

1. [Current Architecture Analysis](#current-architecture-analysis)
2. [Problem Statement Details](#problem-statement-details)
3. [SceneManager Data Sourcing](#scenemanager-data-sourcing)
4. [Scene Factory Pattern Analysis](#scene-factory-pattern-analysis)
5. [EntityConfigurator Abstraction](#entityconfigurator-abstraction)
6. [Recommended Architecture](#recommended-architecture)
7. [Implementation Strategy](#implementation-strategy)
8. [Migration Path](#migration-path)
9. [Open Questions](#open-questions)

---

## Current Architecture Analysis

### Current SceneManager Flow

```cpp
// SceneManager.cpp - Current Implementation
std::expected<std::monostate, FailInfo>
SceneManager::AddSceneFromDefault(const SceneType &scene_type) {
  
  // 1. Create SceneFactory
  SceneFactory scene_factory;
  
  // 2. SceneFactory creates Scene and configures it
  auto scene_creation_result =
      scene_factory.CreateDefaultScene(scene_type, m_game_context);
  
  // 3. Add to m_scenes map
  m_scenes.emplace(scene_creation_result.value()->GetSceneInfo().id,
                   std::move(scene_creation_result.value()));
  
  // 4. Load scene assets
  m_game_context.asset_manager.LoadSceneAssets(scene_type);
  
  return std::monostate{};
}
```

### Current SceneFactory Flow

```cpp
// SceneFactory.cpp - Current Implementation
std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateDefaultScene(const SceneType &scene_type,
                                 const GameContext &game_context) {
  
  // 1. Generate UUID
  uuids::uuid scene_uuid = CreateUUID();
  
  // 2. Create Scene instance (polymorphic)
  std::unique_ptr<Scene> scene_ptr{nullptr};
  switch (scene_type) {
    case SceneType::SceneType_TITLE:
      scene_ptr = std::make_unique<TitleScene>(scene_uuid, game_context);
      break;
    case SceneType::SceneType_CRAFTING:
      scene_ptr = std::make_unique<CraftingScene>(scene_uuid, game_context);
      break;
  }
  
  // 3. Get SceneData from provider
  ISceneDataProvider &data_provider = GetSceneDataProvider();
  auto scene_data_result = data_provider.LoadSceneData(scene_type);
  
  // 4. Configure render texture from SceneData
  const auto &scene_data = scene_data_result.value();
  sf::Vector2u texture_size(scene_data.render_texture_width,
                           scene_data.render_texture_height);
  scene_ptr->m_scene_resources.scene_texture = sf::RenderTexture(texture_size);
  
  // 5. Configure scene entities from default data
  auto configure_result = scene_ptr->ConfigureFromDefault();
  
  // 6. Generate archetypes
  scene_ptr->m_scene_resources.entity_manager.GenerateAllArchetypes();
  
  // 7. Create LogicMap
  LogicFactory logic_factory(scene_type, scene_ptr->GetSceneContext());
  auto logic_map_result = logic_factory.CreateLogicMap(...);
  scene_ptr->SetLogicMap(std::move(logic_map_result.value()));
  
  return scene_ptr;
}
```

### Current EntityConfigurator Pattern

```cpp
// FlatbuffersConfigurator.h - Current Implementation
class FlatbuffersConfigurator : public EntityConfigurator {
private:
  // Overloaded methods for different component types
  std::expected<std::monostate, FailInfo>
  ConfigureComponent(Component &component);
  
  std::expected<std::monostate, FailInfo>
  ConfigureComponent(const UserInterfaceData *ui_data,
                     CUserInterface &ui_component);
  
  std::expected<std::monostate, FailInfo>
  ConfigureComponent(const GrimoireMachinaData *grimoire_data,
                     CGrimoireMachina &grimoire_component);
  
  std::expected<std::monostate, FailInfo>
  ConfigureComponent(const UIStateCollectionData *ui_state_data,
                     CUIState &ui_state_component,
                     const EntityMemoryPool &entity_memory_pool);
};
```

**Problems with Current EntityConfigurator:**
- ❌ Not truly abstract - derived classes can't override behavior
- ❌ Overloaded methods don't provide polymorphism
- ❌ Can't swap configurator implementations at runtime
- ❌ Testing requires concrete FlatBuffers types
- ❌ Can't easily add new data sources (JSON, XML, Lua)

### Current Scene Configuration

```cpp
// Scene.cpp - Current Implementation
std::expected<std::monostate, FailInfo>
Scene::ConfigureFromDefault(const DataType &data_type) {
  
  // Delegates to EntityManager
  auto emp_configure_result =
      m_scene_resources.entity_manager.ConfigureEntitiesFromDefaultData(
          m_scene_info.type, data_type);
  
  return emp_configure_result;
}
```

**Note**: Scene has a configuration method, which we want to avoid per requirements.

---

## Problem Statement Details

### Requirements

1. **SceneManager Data Sourcing**
   - SceneManager should determine where SceneData comes from:
     - **Default scenes**: From ISceneDataProvider (data/scenes/*.bin)
     - **Saved games**: Extract SceneData from ISaveDataProvider (saves/slot_X/*.bin)
   - SceneData is then fed to the Scene factory

2. **Scene Factory Pattern**
   - Leaning towards **abstract factory** pattern
   - Can configure components of Scene using **abstract configurators**
   - Alternative: Overloaded configure function
   - Question: Which approach is better?

3. **Avoid Scene::Configure()**
   - Don't want configuration methods on Scene itself
   - Scene should be a structural container
   - Configuration happens externally

4. **Abstract EntityConfigurator**
   - Make EntityConfigurator properly abstract
   - Define **virtual functions for override** (not overloads)
   - Replace current overload approach in FlatbuffersConfigurator

---

## SceneManager Data Sourcing

### Design Goals

1. **SceneManager orchestrates data source selection**
2. **Unified SceneData format** for downstream code
3. **No conditional logic** in Scene or SceneFactory
4. **Support both default and saved scenes** without code duplication

### Proposed Architecture

```cpp
class SceneManager {
private:
  const GameContext &m_game_context;
  std::unordered_map<uuids::uuid, std::unique_ptr<Scene>> m_scenes;
  SceneManagerResources m_scene_manager_resources;
  SceneManagerConfig m_scene_manager_config;
  SceneManagerState m_scene_manager_state;
  
  // Data source providers
  std::unique_ptr<ISceneDataProvider> m_scene_data_provider;
  std::unique_ptr<ISaveDataProvider> m_save_data_provider;

public:
  /////////////////////////////////////////////////
  /// @brief Load scene from default data (new game, scene transitions)
  ///
  /// This is the primary method for creating new scenes from default
  /// template data.
  ///
  /// @param scene_type The type of scene to create
  /// @return UUID of created scene or failure
  /////////////////////////////////////////////////
  std::expected<uuids::uuid, FailInfo>
  LoadSceneFromDefault(const SceneType &scene_type);
  
  /////////////////////////////////////////////////
  /// @brief Load scene from saved game data
  ///
  /// Used when loading a saved game. Extracts SceneData from SaveData
  /// and restores scene state.
  ///
  /// @param slot_index The save slot to load from
  /// @return UUID of restored scene or failure
  /////////////////////////////////////////////////
  std::expected<uuids::uuid, FailInfo>
  LoadSceneFromSave(uint32_t slot_index);

private:
  /////////////////////////////////////////////////
  /// @brief Internal helper that creates Scene from SceneData
  ///
  /// Common code path for both default and saved scenes.
  ///
  /// @param scene_data The SceneData to use for creation
  /// @return Created Scene or failure
  /////////////////////////////////////////////////
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateSceneFromData(const SceneData &scene_data);
};
```

### Implementation Flow

#### Loading Default Scene

```cpp
std::expected<uuids::uuid, FailInfo>
SceneManager::LoadSceneFromDefault(const SceneType &scene_type) {
  
  // 1. Get SceneData from ISceneDataProvider
  auto scene_data_result = m_scene_data_provider->LoadSceneData(scene_type);
  if (!scene_data_result) {
    return std::unexpected(scene_data_result.error());
  }
  
  // 2. Create Scene from SceneData (common path)
  auto scene_result = CreateSceneFromData(scene_data_result.value());
  if (!scene_result) {
    return std::unexpected(scene_result.error());
  }
  
  // 3. Add to m_scenes map
  auto scene_id = scene_result.value()->GetSceneInfo().id;
  m_scenes.emplace(scene_id, std::move(scene_result.value()));
  
  // 4. Load scene assets
  m_game_context.asset_manager.LoadSceneAssets(scene_type);
  
  return scene_id;
}
```

#### Loading Saved Scene

```cpp
std::expected<uuids::uuid, FailInfo>
SceneManager::LoadSceneFromSave(uint32_t slot_index) {
  
  // 1. Load SaveData from ISaveDataProvider
  auto save_data_result = m_save_data_provider->LoadSave(slot_index);
  if (!save_data_result) {
    return std::unexpected(save_data_result.error());
  }
  
  // 2. Extract SceneData from SaveData
  //    Note: In Phase 1, this is simple - SaveData contains SceneType
  //    In Phase 2 (future), use SceneDataExtractor for nested scene states
  const SaveData &save_data = save_data_result.value();
  
  SceneData scene_data;
  scene_data.scene_type = save_data.current_scene_type;
  scene_data.scene_id = ""; // Will be generated
  // Future: scene_data populated from SaveData.scene_states
  
  // 3. Create Scene from SceneData (common path)
  auto scene_result = CreateSceneFromData(scene_data);
  if (!scene_result) {
    return std::unexpected(scene_result.error());
  }
  
  // 4. Add to m_scenes map
  auto scene_id = scene_result.value()->GetSceneInfo().id;
  m_scenes.emplace(scene_id, std::move(scene_result.value()));
  
  // 5. Load scene assets
  m_game_context.asset_manager.LoadSceneAssets(scene_data.scene_type);
  
  return scene_id;
}
```

#### Common Scene Creation

```cpp
std::expected<std::unique_ptr<Scene>, FailInfo>
SceneManager::CreateSceneFromData(const SceneData &scene_data) {
  
  // 1. Delegate to SceneFactory with SceneData
  SceneFactory scene_factory;
  
  auto scene_result = scene_factory.CreateScene(
      scene_data,
      m_game_context,
      m_scene_manager_resources);
  
  if (!scene_result) {
    return std::unexpected(scene_result.error());
  }
  
  return scene_result;
}
```

### Benefits of This Approach

✅ **SceneManager orchestrates**: Determines data source based on operation (new game vs load game)  
✅ **Unified downstream**: Scene and SceneFactory work with SceneData regardless of source  
✅ **No conditional logic**: Scene/SceneFactory don't need to know about save vs default  
✅ **Testable**: Can test with mock SceneData without file I/O  
✅ **Extensible**: Easy to add more data sources (network, procedural generation)

### Future: SceneDataExtractor Pattern

When SaveData is extended to include scene_states (Phase 2), introduce SceneDataExtractor:

```cpp
class SceneDataExtractor {
public:
  /////////////////////////////////////////////////
  /// @brief Extract SceneData from nested SaveData
  ///
  /// Converts SaveData.scene_states[i] to flat SceneData format.
  ///
  /// @param save_data The SaveData containing nested scene states
  /// @param scene_index Which scene to extract (for multi-scene saves)
  /// @return Extracted SceneData or failure
  /////////////////////////////////////////////////
  std::expected<SceneData, FailInfo>
  ExtractSceneData(const SaveData &save_data, size_t scene_index = 0);
};
```

**Usage:**
```cpp
std::expected<uuids::uuid, FailInfo>
SceneManager::LoadSceneFromSave(uint32_t slot_index) {
  auto save_data_result = m_save_data_provider->LoadSave(slot_index);
  
  // Use extractor to convert nested to flat
  SceneDataExtractor extractor;
  auto scene_data_result = extractor.ExtractSceneData(save_data_result.value());
  
  // Same common path as default scenes
  auto scene_result = CreateSceneFromData(scene_data_result.value());
  // ...
}
```

---

## Scene Factory Pattern Analysis

### Option 1: Overloaded Configure Function

**Approach:**
```cpp
class SceneFactory {
public:
  // Configure from ISceneDataProvider (default)
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateScene(const SceneType &scene_type,
              const GameContext &game_context,
              ISceneDataProvider &scene_data_provider);
  
  // Configure from SceneData (extracted from save)
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateScene(const SceneData &scene_data,
              const GameContext &game_context);
};
```

**Pros:**
- ✅ Simple to understand
- ✅ Direct - no intermediate objects

**Cons:**
- ❌ Not polymorphic - overloads resolved at compile time
- ❌ Duplicated configuration logic in each overload
- ❌ Can't swap configuration strategies at runtime
- ❌ Hard to test - requires specific parameter types
- ❌ Violates Open/Closed Principle

### Option 2: Abstract Factory Pattern (RECOMMENDED)

**Approach:**
```cpp
/////////////////////////////////////////////////
/// @class ISceneConfigurator
/// @brief Abstract interface for configuring Scene components
/////////////////////////////////////////////////
class ISceneConfigurator {
public:
  virtual ~ISceneConfigurator() = default;
  
  /////////////////////////////////////////////////
  /// @brief Configure render texture dimensions
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureRenderTexture(Scene &scene, const SceneData &scene_data) = 0;
  
  /////////////////////////////////////////////////
  /// @brief Configure entities in the scene
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureEntities(Scene &scene, const SceneData &scene_data) = 0;
  
  /////////////////////////////////////////////////
  /// @brief Configure logic for the scene
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureLogic(Scene &scene, const SceneData &scene_data) = 0;
};

/////////////////////////////////////////////////
/// @class DefaultSceneConfigurator
/// @brief Configurator for scenes loaded from default data
/////////////////////////////////////////////////
class DefaultSceneConfigurator : public ISceneConfigurator {
private:
  std::unique_ptr<IEntityConfigurator> m_entity_configurator;
  
public:
  DefaultSceneConfigurator(std::unique_ptr<IEntityConfigurator> entity_configurator)
      : m_entity_configurator(std::move(entity_configurator)) {}
  
  std::expected<std::monostate, FailInfo>
  ConfigureRenderTexture(Scene &scene, const SceneData &scene_data) override {
    sf::Vector2u texture_size(scene_data.render_texture_width,
                             scene_data.render_texture_height);
    scene.m_scene_resources.scene_texture = sf::RenderTexture(texture_size);
    return std::monostate{};
  }
  
  std::expected<std::monostate, FailInfo>
  ConfigureEntities(Scene &scene, const SceneData &scene_data) override {
    // Delegate to IEntityConfigurator
    return m_entity_configurator->ConfigureFromDefault(
        scene.m_scene_resources.entity_manager,
        scene_data.scene_type);
  }
  
  std::expected<std::monostate, FailInfo>
  ConfigureLogic(Scene &scene, const SceneData &scene_data) override {
    LogicFactory logic_factory(scene_data.scene_type, scene.GetSceneContext());
    
    FlatbuffersDataLoader data_loader;
    auto logic_data_result =
        data_loader.ProvideLogicCollectionData(scene_data.scene_type);
    
    auto logic_map_result = logic_factory.CreateLogicMap(
        logic_data_result.has_value() ? logic_data_result.value() : nullptr);
    
    if (!logic_map_result) {
      return std::unexpected(logic_map_result.error());
    }
    
    scene.SetLogicMap(std::move(logic_map_result.value()));
    return std::monostate{};
  }
};

/////////////////////////////////////////////////
/// @class SavedSceneConfigurator
/// @brief Configurator for scenes loaded from saved games
/////////////////////////////////////////////////
class SavedSceneConfigurator : public ISceneConfigurator {
private:
  std::unique_ptr<IEntityConfigurator> m_entity_configurator;
  
public:
  SavedSceneConfigurator(std::unique_ptr<IEntityConfigurator> entity_configurator)
      : m_entity_configurator(std::move(entity_configurator)) {}
  
  std::expected<std::monostate, FailInfo>
  ConfigureRenderTexture(Scene &scene, const SceneData &scene_data) override {
    // Same implementation as default
    sf::Vector2u texture_size(scene_data.render_texture_width,
                             scene_data.render_texture_height);
    scene.m_scene_resources.scene_texture = sf::RenderTexture(texture_size);
    return std::monostate{};
  }
  
  std::expected<std::monostate, FailInfo>
  ConfigureEntities(Scene &scene, const SceneData &scene_data) override {
    // Future: Different logic for restoring from save
    // For now, similar to default
    return m_entity_configurator->ConfigureFromSave(
        scene.m_scene_resources.entity_manager,
        scene_data);
  }
  
  std::expected<std::monostate, FailInfo>
  ConfigureLogic(Scene &scene, const SceneData &scene_data) override {
    // Future: Logic state restoration from save
    // For now, create fresh logic (same as default)
    LogicFactory logic_factory(scene_data.scene_type, scene.GetSceneContext());
    
    FlatbuffersDataLoader data_loader;
    auto logic_data_result =
        data_loader.ProvideLogicCollectionData(scene_data.scene_type);
    
    auto logic_map_result = logic_factory.CreateLogicMap(
        logic_data_result.has_value() ? logic_data_result.value() : nullptr);
    
    if (!logic_map_result) {
      return std::unexpected(logic_map_result.error());
    }
    
    scene.SetLogicMap(std::move(logic_map_result.value()));
    return std::monostate{};
  }
};

/////////////////////////////////////////////////
/// @class SceneFactory
/// @brief Factory for creating Scene objects with configurable strategies
/////////////////////////////////////////////////
class SceneFactory {
private:
  std::unique_ptr<ISceneConfigurator> m_configurator;
  
  uuids::uuid CreateUUID();
  
public:
  /////////////////////////////////////////////////
  /// @brief Constructor taking a configurator strategy
  ///
  /// @param configurator The configuration strategy to use
  /////////////////////////////////////////////////
  SceneFactory(std::unique_ptr<ISceneConfigurator> configurator)
      : m_configurator(std::move(configurator)) {}
  
  /////////////////////////////////////////////////
  /// @brief Create a Scene from SceneData using configured strategy
  ///
  /// @param scene_data The scene data to configure from
  /// @param game_context Reference to game-wide context
  /// @return Created and configured Scene or failure
  /////////////////////////////////////////////////
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateScene(const SceneData &scene_data,
              const GameContext &game_context);
};
```

**Implementation:**
```cpp
std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateScene(const SceneData &scene_data,
                         const GameContext &game_context) {
  
  // 1. Generate UUID
  uuids::uuid scene_uuid = CreateUUID();
  
  // 2. Create Scene instance based on SceneType
  std::unique_ptr<Scene> scene_ptr{nullptr};
  
  switch (scene_data.scene_type) {
    case SceneType::SceneType_TITLE:
      scene_ptr = std::make_unique<TitleScene>(scene_uuid, game_context);
      break;
    case SceneType::SceneType_CRAFTING:
      scene_ptr = std::make_unique<CraftingScene>(scene_uuid, game_context);
      break;
    default:
      return std::unexpected(FailInfo{FailMode::NonExistentEnumValue,
                                     "Unknown SceneType"});
  }
  
  // 3. Configure render texture using strategy
  auto texture_result = m_configurator->ConfigureRenderTexture(*scene_ptr, scene_data);
  if (!texture_result) {
    return std::unexpected(texture_result.error());
  }
  
  // 4. Configure entities using strategy
  auto entities_result = m_configurator->ConfigureEntities(*scene_ptr, scene_data);
  if (!entities_result) {
    return std::unexpected(entities_result.error());
  }
  
  // 5. Generate archetypes
  auto archetype_result =
      scene_ptr->m_scene_resources.entity_manager.GenerateAllArchetypes();
  if (!archetype_result) {
    return std::unexpected(archetype_result.error());
  }
  
  // 6. Configure logic using strategy
  auto logic_result = m_configurator->ConfigureLogic(*scene_ptr, scene_data);
  if (!logic_result) {
    return std::unexpected(logic_result.error());
  }
  
  return scene_ptr;
}
```

**Usage in SceneManager:**
```cpp
std::expected<uuids::uuid, FailInfo>
SceneManager::LoadSceneFromDefault(const SceneType &scene_type) {
  
  // 1. Get SceneData from provider
  auto scene_data_result = m_scene_data_provider->LoadSceneData(scene_type);
  
  // 2. Create configurator for default scenes
  auto entity_configurator = std::make_unique<FlatbuffersEntityConfigurator>(
      m_game_context.event_handler);
  
  auto scene_configurator = std::make_unique<DefaultSceneConfigurator>(
      std::move(entity_configurator));
  
  // 3. Create factory with configurator
  SceneFactory scene_factory(std::move(scene_configurator));
  
  // 4. Create scene
  auto scene_result = scene_factory.CreateScene(
      scene_data_result.value(),
      m_game_context);
  
  // 5. Add to map
  auto scene_id = scene_result.value()->GetSceneInfo().id;
  m_scenes.emplace(scene_id, std::move(scene_result.value()));
  
  return scene_id;
}

std::expected<uuids::uuid, FailInfo>
SceneManager::LoadSceneFromSave(uint32_t slot_index) {
  
  // 1. Load SaveData
  auto save_data_result = m_save_data_provider->LoadSave(slot_index);
  
  // 2. Extract SceneData
  SceneDataExtractor extractor;
  auto scene_data_result = extractor.ExtractSceneData(save_data_result.value());
  
  // 3. Create configurator for saved scenes
  auto entity_configurator = std::make_unique<SavedEntityConfigurator>(
      m_game_context.event_handler);
  
  auto scene_configurator = std::make_unique<SavedSceneConfigurator>(
      std::move(entity_configurator));
  
  // 4. Create factory with configurator
  SceneFactory scene_factory(std::move(scene_configurator));
  
  // 5. Create scene
  auto scene_result = scene_factory.CreateScene(
      scene_data_result.value(),
      m_game_context);
  
  // 6. Add to map
  auto scene_id = scene_result.value()->GetSceneInfo().id;
  m_scenes.emplace(scene_id, std::move(scene_result.value()));
  
  return scene_id;
}
```

**Pros of Abstract Factory:**
- ✅ **Polymorphic** - runtime strategy selection
- ✅ **Open/Closed** - extend without modifying existing code
- ✅ **Single Responsibility** - each configurator handles one strategy
- ✅ **Testable** - can mock ISceneConfigurator
- ✅ **Composable** - configurators can use sub-configurators (IEntityConfigurator)
- ✅ **Extensible** - easy to add NetworkSceneConfigurator, ProceduralSceneConfigurator, etc.
- ✅ **Reusable** - common logic in base classes, differences in overrides

**Cons:**
- ❌ More classes (more indirection)
- ❌ Slightly more complex initially

### Verdict: Abstract Factory Pattern RECOMMENDED

The abstract factory pattern provides:
- Better separation of concerns
- Runtime configurability
- Better testability
- Clear extension points for future features

The additional complexity is justified by the flexibility and maintainability gains.

---

## EntityConfigurator Abstraction

### Current Problem

```cpp
// Current: Overloaded methods (NOT polymorphic)
class FlatbuffersConfigurator : public EntityConfigurator {
private:
  std::expected<std::monostate, FailInfo>
  ConfigureComponent(Component &component);
  
  std::expected<std::monostate, FailInfo>
  ConfigureComponent(const UserInterfaceData *ui_data,
                     CUserInterface &ui_component);
  
  // More overloads...
};
```

**Problems:**
- ❌ Overloads aren't virtual - can't override in derived classes
- ❌ Can't swap implementations at runtime
- ❌ Tightly coupled to FlatBuffers types
- ❌ Hard to test without FlatBuffers data

### Proposed Abstract Base

```cpp
/////////////////////////////////////////////////
/// @class IEntityConfigurator
/// @brief Abstract interface for configuring entities
///
/// Defines virtual methods that derived classes must implement
/// to provide different configuration strategies.
/////////////////////////////////////////////////
class IEntityConfigurator {
protected:
  /////////////////////////////////////////////////
  /// @brief Reference to EventHandler for subscriber creation
  /////////////////////////////////////////////////
  EventHandler &m_event_handler;
  
public:
  /////////////////////////////////////////////////
  /// @brief Constructor
  ///
  /// @param event_handler Reference to game-wide EventHandler
  /////////////////////////////////////////////////
  IEntityConfigurator(EventHandler &event_handler)
      : m_event_handler(event_handler) {}
  
  virtual ~IEntityConfigurator() = default;
  
  /////////////////////////////////////////////////
  /// @brief Configure entities from default data (new game, scene load)
  ///
  /// Virtual method to be overridden by derived classes.
  ///
  /// @param entity_manager The EntityManager to configure
  /// @param scene_type The type of scene being configured
  /// @return Success or failure information
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureFromDefault(EntityManager &entity_manager,
                       SceneType scene_type) = 0;
  
  /////////////////////////////////////////////////
  /// @brief Configure entities from saved game data
  ///
  /// Virtual method to be overridden by derived classes.
  ///
  /// @param entity_manager The EntityManager to configure
  /// @param scene_data The scene data extracted from save
  /// @return Success or failure information
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureFromSave(EntityManager &entity_manager,
                    const SceneData &scene_data) = 0;
  
  /////////////////////////////////////////////////
  /// @brief Configure entities from test data (for testing)
  ///
  /// Virtual method to be overridden by derived classes.
  ///
  /// @param entity_manager The EntityManager to configure
  /// @param test_config Test-specific configuration
  /// @return Success or failure information
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureFromTest(EntityManager &entity_manager,
                    const TestEntityConfig &test_config) = 0;
};
```

### Concrete Implementation: FlatbuffersEntityConfigurator

```cpp
/////////////////////////////////////////////////
/// @class FlatbuffersEntityConfigurator
/// @brief FlatBuffers implementation of IEntityConfigurator
/////////////////////////////////////////////////
class FlatbuffersEntityConfigurator : public IEntityConfigurator {
private:
  FlatbuffersDataLoader m_data_loader;
  
  /////////////////////////////////////////////////
  /// @brief Internal helper to configure base Component
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureBaseComponent(Component &component);
  
  /////////////////////////////////////////////////
  /// @brief Internal helper to configure CUserInterface
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureCUserInterface(const UserInterfaceData *ui_data,
                          CUserInterface &ui_component);
  
  /////////////////////////////////////////////////
  /// @brief Internal helper to configure CGrimoireMachina
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureCGrimoireMachina(const GrimoireMachinaData *grimoire_data,
                            CGrimoireMachina &grimoire_component);
  
  /////////////////////////////////////////////////
  /// @brief Internal helper to configure all entities from EntityCollection
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureFromEntityCollection(EntityManager &entity_manager,
                                const EntityCollection *entity_collection);

public:
  FlatbuffersEntityConfigurator(EventHandler &event_handler)
      : IEntityConfigurator(event_handler) {}
  
  /////////////////////////////////////////////////
  /// @brief Configure entities from default FlatBuffers data
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureFromDefault(EntityManager &entity_manager,
                       SceneType scene_type) override {
    
    // Load FlatBuffers data for this scene type
    const SceneDataData *scene_data =
        m_data_loader.ProvideDefaultSceneData(scene_type).value();
    
    if (!scene_data || !scene_data->entity_collection()) {
      return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                     "EntityCollection not found"});
    }
    
    // Delegate to internal helper
    return ConfigureFromEntityCollection(
        entity_manager,
        scene_data->entity_collection());
  }
  
  /////////////////////////////////////////////////
  /// @brief Configure entities from saved FlatBuffers data
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureFromSave(EntityManager &entity_manager,
                    const SceneData &scene_data) override {
    
    // Future: Load entity state from SaveData
    // For now, use same logic as default
    return ConfigureFromDefault(entity_manager, scene_data.scene_type);
  }
  
  /////////////////////////////////////////////////
  /// @brief Configure entities from test data
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureFromTest(EntityManager &entity_manager,
                    const TestEntityConfig &test_config) override {
    
    // Load test-specific FlatBuffers data
    const EntityCollection *entity_collection =
        test_config.GetEntityCollection();
    
    if (!entity_collection) {
      return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                     "Test EntityCollection not found"});
    }
    
    return ConfigureFromEntityCollection(entity_manager, entity_collection);
  }
};
```

### Alternative Implementation: MockEntityConfigurator (for testing)

```cpp
/////////////////////////////////////////////////
/// @class MockEntityConfigurator
/// @brief Mock implementation for testing
/////////////////////////////////////////////////
class MockEntityConfigurator : public IEntityConfigurator {
private:
  std::function<std::expected<std::monostate, FailInfo>(EntityManager&, SceneType)>
      m_configure_from_default_fn;
  
public:
  MockEntityConfigurator(EventHandler &event_handler)
      : IEntityConfigurator(event_handler) {}
  
  void SetConfigureFromDefaultBehavior(
      std::function<std::expected<std::monostate, FailInfo>(EntityManager&, SceneType)> fn) {
    m_configure_from_default_fn = fn;
  }
  
  std::expected<std::monostate, FailInfo>
  ConfigureFromDefault(EntityManager &entity_manager,
                       SceneType scene_type) override {
    if (m_configure_from_default_fn) {
      return m_configure_from_default_fn(entity_manager, scene_type);
    }
    return std::monostate{};
  }
  
  // Similar for other virtual methods...
};
```

### Benefits of Abstract EntityConfigurator

✅ **Truly polymorphic** - virtual methods can be overridden  
✅ **Runtime swappable** - can use different configurators for different scenarios  
✅ **Testable** - can mock IEntityConfigurator in tests  
✅ **Extensible** - easy to add JsonEntityConfigurator, XmlEntityConfigurator  
✅ **Clear contract** - base class defines what derived classes must implement  
✅ **Decoupled** - game code depends on IEntityConfigurator, not FlatBuffers types

### Migration Path from Current Overloads

1. **Keep existing overloaded methods** as private helpers
2. **Add virtual interface methods** that delegate to helpers
3. **Update call sites** to use interface methods
4. **Gradually refactor** internal helpers as needed

Example:
```cpp
class FlatbuffersEntityConfigurator : public IEntityConfigurator {
private:
  // OLD: Keep these as private helpers
  std::expected<std::monostate, FailInfo>
  ConfigureComponent(Component &component);
  
  std::expected<std::monostate, FailInfo>
  ConfigureComponent(const UserInterfaceData *ui_data,
                     CUserInterface &ui_component);

public:
  // NEW: Virtual interface method
  std::expected<std::monostate, FailInfo>
  ConfigureFromDefault(EntityManager &entity_manager,
                       SceneType scene_type) override {
    // Load data and delegate to old helpers
    const SceneDataData *scene_data = LoadSceneData(scene_type);
    
    for (size_t i = 0; i < entity_collection->entities()->size(); ++i) {
      const EntityData *entity_data = entity_collection->entities()->Get(i);
      
      // Call old overloaded helper
      if (entity_data->c_user_interface()) {
        auto result = ConfigureComponent(
            entity_data->c_user_interface(),
            GetComponent<CUserInterface>(i, entity_pool));
        if (!result) return result;
      }
    }
    
    return std::monostate{};
  }
};
```

---

## Recommended Architecture

### Overall System Design

```
┌─────────────────────────────────────────────────────────────────┐
│                        SceneManager                              │
│                                                                  │
│  ┌────────────────────┐      ┌───────────────────────┐         │
│  │ ISceneDataProvider │      │ ISaveDataProvider     │         │
│  └────────────────────┘      └───────────────────────┘         │
│           │                            │                         │
│           │ LoadSceneData()            │ LoadSave()             │
│           ▼                            ▼                         │
│      SceneData                    SaveData                       │
│           │                            │                         │
│           │                            │ SceneDataExtractor     │
│           │                            │ ExtractSceneData()      │
│           │                            ▼                         │
│           └────────────────────► SceneData                      │
│                                        │                         │
│                                        ▼                         │
│                               SceneFactory                       │
│                          (with ISceneConfigurator)               │
└─────────────────────────────────────────────────────────────────┘
                                         │
                                         ▼
                                   Scene (created)
                                         │
               ┌─────────────────────────┼─────────────────────────┐
               │                         │                         │
               ▼                         ▼                         ▼
      RenderTexture               EntityManager              LogicMap
      (configured)                (via IEntityConfigurator)  (configured)
```

### Key Responsibilities

#### SceneManager
- **Orchestrates** scene lifecycle (create, load, update, destroy)
- **Determines data source** (default vs saved)
- **Provides SceneData** to SceneFactory
- **Manages scene map** (m_scenes)
- **Does NOT** configure scenes directly

#### SceneFactory
- **Creates Scene instances** (polymorphic: TitleScene, CraftingScene)
- **Delegates configuration** to ISceneConfigurator
- **Coordinates** render texture, entities, logic setup
- **Does NOT** know about data sources (works with SceneData)

#### ISceneConfigurator
- **Defines configuration strategy** (default vs saved)
- **Configures Scene components** (render texture, entities, logic)
- **Composes IEntityConfigurator** for entity configuration
- **Can be swapped** at runtime (polymorphic)

#### IEntityConfigurator
- **Defines entity configuration strategy**
- **Virtual methods** for override (not overloads)
- **Polymorphic** - can swap implementations
- **Examples**: FlatbuffersEntityConfigurator, JsonEntityConfigurator, MockEntityConfigurator

#### Scene
- **Structural container** for scene components
- **No configuration methods** (ConfigureFromDefault removed)
- **Provides access** to resources, managers, state
- **Polymorphic** for scene-specific logic (sMovement, sCollision, sAction, sRender)

### Data Flow

#### Default Scene Load
```
User Action (new game, scene transition)
    ↓
SceneManager::LoadSceneFromDefault(SceneType)
    ↓
ISceneDataProvider::LoadSceneData(SceneType)
    ↓
SceneData (struct)
    ↓
SceneFactory::CreateScene(SceneData, GameContext)
    ├─ Create Scene instance (TitleScene, CraftingScene)
    ├─ ISceneConfigurator::ConfigureRenderTexture()
    ├─ ISceneConfigurator::ConfigureEntities()
    │   └─ IEntityConfigurator::ConfigureFromDefault()
    ├─ Generate Archetypes
    └─ ISceneConfigurator::ConfigureLogic()
    ↓
Configured Scene
    ↓
SceneManager::m_scenes.emplace(id, scene)
```

#### Saved Scene Load
```
User Action (load game)
    ↓
SceneManager::LoadSceneFromSave(slot_index)
    ↓
ISaveDataProvider::LoadSave(slot_index)
    ↓
SaveData (struct with nested scene_states)
    ↓
SceneDataExtractor::ExtractSceneData(SaveData)
    ↓
SceneData (struct)
    ↓
SceneFactory::CreateScene(SceneData, GameContext)
    ├─ Create Scene instance (TitleScene, CraftingScene)
    ├─ ISceneConfigurator::ConfigureRenderTexture()
    ├─ ISceneConfigurator::ConfigureEntities()
    │   └─ IEntityConfigurator::ConfigureFromSave()
    ├─ Generate Archetypes
    └─ ISceneConfigurator::ConfigureLogic()
    ↓
Configured Scene (restored state)
    ↓
SceneManager::m_scenes.emplace(id, scene)
```

---

## Implementation Strategy

### Phase 1: Abstract EntityConfigurator

**Goal**: Convert EntityConfigurator to proper abstract base with virtual methods

**Steps**:

1. **Create IEntityConfigurator interface**
   - Define virtual methods: ConfigureFromDefault, ConfigureFromSave, ConfigureFromTest
   - Protected m_event_handler member

2. **Rename FlatbuffersConfigurator**
   - Rename to FlatbuffersEntityConfigurator
   - Inherit from IEntityConfigurator
   - Keep existing overloaded methods as private helpers
   - Implement virtual interface methods that delegate to helpers

3. **Update call sites**
   - EntityManager::ConfigureEntitiesFromDefaultData() now takes IEntityConfigurator&
   - Scene no longer has ConfigureFromDefault() method
   - Configuration happens in SceneFactory

4. **Write tests**
   - Test IEntityConfigurator interface
   - Test FlatbuffersEntityConfigurator implementation
   - Test MockEntityConfigurator for unit tests

**Files to change**:
- `src/entity/EntityConfigurator.h` → `src/entity/IEntityConfigurator.h`
- `src/entity/EntityConfigurator.cpp` → delete (interface only)
- `src/entity/FlatbuffersConfigurator.h` → `src/entity/FlatbuffersEntityConfigurator.h`
- `src/entity/FlatbuffersConfigurator.cpp` → `src/entity/FlatbuffersEntityConfigurator.cpp`
- `src/entity/EntityManager.h` (update method signatures)
- `src/entity/EntityManager.cpp` (update implementation)
- `tests/entity/FlatbuffersConfigurator.test.cpp` → update

### Phase 2: Scene Configuration Removal

**Goal**: Remove Scene::ConfigureFromDefault() method

**Steps**:

1. **Remove Scene::ConfigureFromDefault()**
   - Delete method from Scene.h and Scene.cpp
   - Configuration now happens externally in SceneFactory

2. **Update SceneFactory**
   - SceneFactory now handles all configuration
   - Directly calls IEntityConfigurator::ConfigureFromDefault()
   - No delegation to Scene::ConfigureFromDefault()

3. **Update tests**
   - Test configuration happens in SceneFactory tests
   - Scene tests focus on structural aspects

**Files to change**:
- `src/scenes/Scene.h` (remove ConfigureFromDefault)
- `src/scenes/Scene.cpp` (remove implementation)
- `src/scenes/SceneFactory.cpp` (update to configure directly)
- `tests/scenes/Scene.test.cpp` (update tests)

### Phase 3: Abstract Scene Configurator

**Goal**: Introduce ISceneConfigurator interface and implementations

**Steps**:

1. **Create ISceneConfigurator interface**
   - Define virtual methods: ConfigureRenderTexture, ConfigureEntities, ConfigureLogic
   - Create DefaultSceneConfigurator implementation
   - Create SavedSceneConfigurator implementation (stub for now)

2. **Update SceneFactory**
   - Accept ISceneConfigurator in constructor
   - Use configurator for all configuration steps
   - Remove direct configuration code

3. **Write tests**
   - Test ISceneConfigurator interface
   - Test DefaultSceneConfigurator
   - Test MockSceneConfigurator

**Files to create**:
- `src/scenes/ISceneConfigurator.h`
- `src/scenes/DefaultSceneConfigurator.h`
- `src/scenes/DefaultSceneConfigurator.cpp`
- `src/scenes/SavedSceneConfigurator.h`
- `src/scenes/SavedSceneConfigurator.cpp`
- `tests/scenes/ISceneConfigurator.test.cpp`

**Files to change**:
- `src/scenes/SceneFactory.h` (accept ISceneConfigurator)
- `src/scenes/SceneFactory.cpp` (use configurator)
- `tests/scenes/SceneFactory.test.cpp` (update tests)

### Phase 4: SceneManager Data Sourcing

**Goal**: Update SceneManager to handle data source selection

**Steps**:

1. **Add data providers to SceneManager**
   - Add ISceneDataProvider member
   - Add ISaveDataProvider member

2. **Implement LoadSceneFromDefault()**
   - Get SceneData from ISceneDataProvider
   - Create DefaultSceneConfigurator
   - Create SceneFactory with configurator
   - Delegate to CreateSceneFromData()

3. **Implement LoadSceneFromSave()**
   - Get SaveData from ISaveDataProvider
   - Extract SceneData (simple for now, SceneDataExtractor in future)
   - Create SavedSceneConfigurator
   - Create SceneFactory with configurator
   - Delegate to CreateSceneFromData()

4. **Refactor existing methods**
   - AddSceneFromDefault() calls LoadSceneFromDefault()
   - LoadTitleScene() calls LoadSceneFromDefault(SceneType_TITLE)
   - LoadCraftingScene() calls LoadSceneFromDefault(SceneType_CRAFTING)

**Files to change**:
- `src/scenes/SceneManager.h` (add providers, new methods)
- `src/scenes/SceneManager.cpp` (implement new methods)
- `tests/scenes/SceneManager.test.cpp` (update tests)

### Phase 5: Future Enhancements

**SceneDataExtractor** (when SaveData has scene_states):
- Create SceneDataExtractor class
- Implement ExtractSceneData() method
- Use in LoadSceneFromSave()

**Additional Configurators**:
- NetworkSceneConfigurator (for multiplayer)
- ProceduralSceneConfigurator (for procedural generation)
- TestSceneConfigurator (for advanced testing)

**Additional EntityConfigurators**:
- JsonEntityConfigurator (JSON data source)
- XmlEntityConfigurator (XML data source)
- LuaEntityConfigurator (Lua scripting)

---

## Migration Path

### Step-by-Step Migration

1. ✅ **Phase 1: Abstract EntityConfigurator**
   - Low risk - internal refactor
   - No external API changes
   - Can be tested independently

2. ✅ **Phase 2: Remove Scene::ConfigureFromDefault()**
   - Medium risk - changes Scene API
   - But Scene API is internal to engine
   - Tests verify correctness

3. ✅ **Phase 3: Abstract Scene Configurator**
   - Low risk - adds new classes
   - Doesn't remove existing functionality
   - Fully backwards compatible initially

4. ✅ **Phase 4: SceneManager Data Sourcing**
   - Medium risk - changes SceneManager behavior
   - But maintains existing public API
   - LoadTitleScene(), LoadCraftingScene() still work

5. ⏳ **Phase 5: Future Enhancements**
   - Low risk - adds new capabilities
   - Doesn't change existing code
   - Opt-in functionality

### Rollback Strategy

Each phase can be rolled back independently:

- **Phase 1**: Revert EntityConfigurator changes, restore overloaded methods
- **Phase 2**: Re-add Scene::ConfigureFromDefault() wrapper
- **Phase 3**: Remove ISceneConfigurator, revert SceneFactory
- **Phase 4**: Revert SceneManager, keep old AddSceneFromDefault()

### Testing Strategy

**Unit Tests**:
- Test each new class in isolation
- Mock dependencies using interfaces
- Verify behavior with various inputs

**Integration Tests**:
- Test SceneManager → SceneFactory → Scene flow
- Test default scene loading
- Test saved scene loading (when implemented)

**Regression Tests**:
- Ensure existing scene loading still works
- Verify TitleScene and CraftingScene creation
- Check entity configuration correctness

---

## Open Questions

### Question 1: SceneData Extension

**Q**: When should SceneData be extended with entity state?

**Current SceneData**:
```cpp
struct SceneData {
  SceneType scene_type;
  std::string scene_id;
  uint32_t render_texture_width;
  uint32_t render_texture_height;
};
```

**Extended SceneData** (future):
```cpp
struct SceneData {
  SceneType scene_type;
  std::string scene_id;
  uint32_t render_texture_width;
  uint32_t render_texture_height;
  
  // NEW: Entity state for save/load
  std::vector<EntityState> entity_states;  // ???
  
  // NEW: Logic state for save/load
  LogicState logic_state;  // ???
};
```

**Answer**: Extend SceneData when:
1. SaveData schema includes scene_states
2. SceneDataExtractor is implemented
3. EntityState serialization is defined

**For now**: Keep SceneData minimal (metadata only)

### Question 2: LogicFactory Integration

**Q**: Should LogicFactory also use abstract configurator pattern?

**Current**:
```cpp
LogicFactory logic_factory(scene_type, scene_context);
auto logic_map = logic_factory.CreateLogicMap(logic_collection_data);
```

**Alternative** (abstract):
```cpp
class ILogicConfigurator {
  virtual LogicCollection CreateLogicMap(SceneType, SceneContext) = 0;
};

class DefaultLogicConfigurator : public ILogicConfigurator { ... };
class SavedLogicConfigurator : public ILogicConfigurator { ... };
```

**Answer**: Not immediately necessary, but consider if:
- Logic needs different configuration strategies
- Saved games need to restore logic state
- Testing requires mock logic creation

**For now**: Keep LogicFactory as-is (it already has good abstraction)

### Question 3: Provider Ownership

**Q**: Who owns ISceneDataProvider and ISaveDataProvider?

**Options**:
1. **SceneManager owns them** (current proposal)
2. **GameContext owns them** (shared across engine)
3. **Singleton pattern** (global access)

**Recommendation**: SceneManager owns them
- SceneManager is the only consumer
- Clear ownership and lifetime
- Easy to test with dependency injection

### Question 4: Configuration Order

**Q**: What is the correct order of Scene configuration steps?

**Proposed Order**:
1. Create Scene instance
2. Configure render texture
3. Configure entities
4. Generate archetypes
5. Configure logic

**Question**: Should archetype generation happen before entity configuration?

**Answer**: No - entities must be configured first
- Archetypes are generated from configured entities
- ArchetypeManager reads component activation state
- Order is correct as proposed

### Question 5: SaveData to SceneData Conversion

**Q**: Should SaveData directly contain SceneData, or should SceneData be extracted?

**Option 1**: SaveData contains SceneData
```cpp
struct SaveData {
  Metadata metadata;
  SceneData scene_data;  // Direct embedding
};
```

**Option 2**: SceneData extracted from SaveData
```cpp
struct SaveData {
  Metadata metadata;
  SceneType current_scene_type;
  // Future: nested scene_states
};

SceneDataExtractor extractor;
SceneData scene_data = extractor.ExtractSceneData(save_data);
```

**Recommendation**: Option 2 (extraction)
- SaveData and SceneData have different purposes
- SaveData represents persistent storage
- SceneData represents runtime configuration
- Extraction allows transformation/migration

---

## Conclusion

### Summary of Recommendations

1. **SceneManager as Orchestrator**
   - ✅ SceneManager determines data source (default vs save)
   - ✅ Provides unified SceneData to SceneFactory
   - ✅ Delegates to appropriate providers (ISceneDataProvider, ISaveDataProvider)

2. **Abstract Factory Pattern for Scenes**
   - ✅ Use ISceneConfigurator interface
   - ✅ DefaultSceneConfigurator for new games
   - ✅ SavedSceneConfigurator for loaded games
   - ✅ SceneFactory accepts configurator strategy

3. **Abstract EntityConfigurator**
   - ✅ Define virtual methods (not overloads)
   - ✅ IEntityConfigurator interface
   - ✅ FlatbuffersEntityConfigurator implementation
   - ✅ Support for ConfigureFromDefault, ConfigureFromSave, ConfigureFromTest

4. **Remove Scene::Configure()**
   - ✅ Scene is structural container only
   - ✅ Configuration happens externally in SceneFactory
   - ✅ Cleaner separation of concerns

### Benefits of This Architecture

✅ **Flexibility**: Runtime strategy selection via abstract factory  
✅ **Testability**: All components mockable via interfaces  
✅ **Extensibility**: Easy to add new data sources and configurators  
✅ **Maintainability**: Clear responsibilities, single purpose classes  
✅ **Type Safety**: Working with C++ structs, not FlatBuffers types  
✅ **Future-Proof**: Ready for save/load, multiplayer, procedural generation

### Next Steps

1. **Review this analysis** with stakeholders
2. **Refine open questions** based on feedback
3. **Approve migration strategy** (phase-by-phase)
4. **Begin Phase 1** (Abstract EntityConfigurator)
5. **Iterate** based on learnings

---

## Appendix: Code Examples

### Complete Example: Loading Default Scene

```cpp
// In SceneManager
std::expected<uuids::uuid, FailInfo>
SceneManager::LoadSceneFromDefault(const SceneType &scene_type) {
  
  // 1. Get SceneData from provider
  auto scene_data_result = m_scene_data_provider->LoadSceneData(scene_type);
  if (!scene_data_result) {
    return std::unexpected(scene_data_result.error());
  }
  
  // 2. Create entity configurator
  auto entity_configurator = std::make_unique<FlatbuffersEntityConfigurator>(
      m_game_context.event_handler);
  
  // 3. Create scene configurator
  auto scene_configurator = std::make_unique<DefaultSceneConfigurator>(
      std::move(entity_configurator));
  
  // 4. Create factory with configurator
  SceneFactory scene_factory(std::move(scene_configurator));
  
  // 5. Create scene
  auto scene_result = scene_factory.CreateScene(
      scene_data_result.value(),
      m_game_context);
  
  if (!scene_result) {
    return std::unexpected(scene_result.error());
  }
  
  // 6. Add to map
  auto scene_id = scene_result.value()->GetSceneInfo().id;
  m_scenes.emplace(scene_id, std::move(scene_result.value()));
  
  // 7. Load assets
  auto asset_result = m_game_context.asset_manager.LoadSceneAssets(scene_type);
  if (!asset_result) {
    return std::unexpected(asset_result.error());
  }
  
  return scene_id;
}
```

### Complete Example: Loading Saved Scene

```cpp
// In SceneManager
std::expected<uuids::uuid, FailInfo>
SceneManager::LoadSceneFromSave(uint32_t slot_index) {
  
  // 1. Load SaveData
  auto save_data_result = m_save_data_provider->LoadSave(slot_index);
  if (!save_data_result) {
    return std::unexpected(save_data_result.error());
  }
  
  // 2. Extract SceneData from SaveData
  SceneDataExtractor extractor;
  auto scene_data_result = extractor.ExtractSceneData(save_data_result.value());
  if (!scene_data_result) {
    return std::unexpected(scene_data_result.error());
  }
  
  // 3. Create entity configurator for saved data
  auto entity_configurator = std::make_unique<FlatbuffersEntityConfigurator>(
      m_game_context.event_handler);
  
  // 4. Create scene configurator for saved data
  auto scene_configurator = std::make_unique<SavedSceneConfigurator>(
      std::move(entity_configurator));
  
  // 5. Create factory with configurator
  SceneFactory scene_factory(std::move(scene_configurator));
  
  // 6. Create scene
  auto scene_result = scene_factory.CreateScene(
      scene_data_result.value(),
      m_game_context);
  
  if (!scene_result) {
    return std::unexpected(scene_result.error());
  }
  
  // 7. Add to map
  auto scene_id = scene_result.value()->GetSceneInfo().id;
  m_scenes.emplace(scene_id, std::move(scene_result.value()));
  
  // 8. Load assets
  auto asset_result = m_game_context.asset_manager.LoadSceneAssets(
      scene_data_result.value().scene_type);
  if (!asset_result) {
    return std::unexpected(asset_result.error());
  }
  
  return scene_id;
}
```

### Complete Example: SceneFactory with Configurator

```cpp
// In SceneFactory
std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateScene(const SceneData &scene_data,
                         const GameContext &game_context) {
  
  // 1. Generate UUID
  uuids::uuid scene_uuid = CreateUUID();
  
  // 2. Create Scene instance based on SceneType
  std::unique_ptr<Scene> scene_ptr{nullptr};
  
  switch (scene_data.scene_type) {
    case SceneType::SceneType_TITLE:
      scene_ptr = std::make_unique<TitleScene>(scene_uuid, game_context);
      break;
    case SceneType::SceneType_CRAFTING:
      scene_ptr = std::make_unique<CraftingScene>(scene_uuid, game_context);
      break;
    default:
      return std::unexpected(FailInfo{FailMode::NonExistentEnumValue,
                                     "Unknown SceneType"});
  }
  
  // 3. Configure render texture using strategy
  auto texture_result = m_configurator->ConfigureRenderTexture(*scene_ptr, scene_data);
  if (!texture_result) {
    return std::unexpected(texture_result.error());
  }
  
  // 4. Configure entities using strategy
  auto entities_result = m_configurator->ConfigureEntities(*scene_ptr, scene_data);
  if (!entities_result) {
    return std::unexpected(entities_result.error());
  }
  
  // 5. Generate archetypes
  auto archetype_result =
      scene_ptr->m_scene_resources.entity_manager.GenerateAllArchetypes();
  if (!archetype_result) {
    return std::unexpected(archetype_result.error());
  }
  
  // 6. Configure logic using strategy
  auto logic_result = m_configurator->ConfigureLogic(*scene_ptr, scene_data);
  if (!logic_result) {
    return std::unexpected(logic_result.error());
  }
  
  return scene_ptr;
}
```

---

**End of Analysis**
