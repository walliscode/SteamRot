# AssetManager Analysis - Data Provider Integration

**Date:** 2025-12-24  
**Related:** ASSET_MANAGER_REFINEMENTS.md, ASSET_MANAGER_ANALYSIS.md  
**Status:** Integration Pattern Documentation

---

## Overview

This document addresses how AssetConfig integrates with existing data providers (IEngineDataProvider and ISceneDataProvider) and how asset loading can be orchestrated through SceneFactory rather than individual scenes.

---

## Table of Contents

1. [Current Structure](#current-structure)
2. [AssetConfig Embedding](#assetconfig-embedding)
3. [Data Provider Views](#data-provider-views)
4. [SceneFactory Integration](#scenefactory-integration)
5. [Implementation Details](#implementation-details)

---

## Current Structure

### Existing Data in FlatBuffers

**SceneDataFbs (scenes/scene_data.fbs):**
```fbs
table SceneDataFbs {
    scene_info: SceneInfoFbs;
    scene_resources: SceneResourcesFbs;
    assets: AssetCollection;           // ← Already contains asset config!
    entity_collection: EntityCollectionFbs;
    logic_collection_data: LogicCollectionData;
}
```

**AssetCollection (assets/assets.fbs):**
```fbs
table AssetCollection {
    fonts: [FontDataFbs];
    ui_styles: [string];
}
```

**EngineResourcesConfigFbs (engine/engine_resources_config.fbs):**
```fbs
table EngineResourcesConfigFbs {
    window_width: uint32 = 800;
    window_height: uint32 = 600;
    window_title: string;
    framerate_limit: uint32 = 60;
    // NOTE: No assets field currently
}
```

### Key Observation

**AssetConfig is already embedded in SceneData!**
- The `assets: AssetCollection` field in `SceneDataFbs` IS the AssetConfig
- We just need to rename `AssetCollection` → `AssetConfig` for consistency
- No structural changes needed for scene-level assets

**EngineResourcesConfig needs extension:**
- Currently lacks asset configuration
- Should be extended to include default assets
- This would enable loading default assets at engine startup

---

## AssetConfig Embedding

### Proposed Structure

#### 1. Rename in FlatBuffers Schema

**File: `src/types/flatbuffers/assets/assets.fbs`**

```fbs
namespace steamrot;

table FontConfigFbs {
    name: string (required);
}

// Renamed: AssetCollection → AssetConfig
table AssetConfig {
    fonts: [FontConfigFbs];
    ui_styles: [string];
    // Future extensions:
    // textures: [TextureConfigFbs];
    // sounds: [SoundConfigFbs];
}

root_type AssetConfig;
```

#### 2. Update EngineResourcesConfigFbs

**File: `src/types/flatbuffers/engine/engine_resources_config.fbs`**

```fbs
include "../assets/assets.fbs";

namespace steamrot;

table EngineResourcesConfigFbs {
    window_width: uint32 = 800;
    window_height: uint32 = 600;
    window_title: string;
    framerate_limit: uint32 = 60;
    
    // NEW: Default assets to load at engine startup
    default_assets: AssetConfig;
}

root_type EngineResourcesConfigFbs;
```

#### 3. Update SceneDataFbs

**File: `src/types/flatbuffers/scenes/scene_data.fbs`**

```fbs
include "../assets/assets.fbs";
// ... other includes ...

namespace steamrot;

table SceneDataFbs {
    scene_info: SceneInfoFbs;
    scene_resources: SceneResourcesFbs;
    
    // Renamed: assets → scene_assets (for clarity)
    scene_assets: AssetConfig;
    
    entity_collection: EntityCollectionFbs;
    logic_collection_data: LogicCollectionData;
}
```

### Updated Native Types

**File: `src/types/core/EngineResourcesConfig.h`**

```cpp
#pragma once

#include "AssetConfig.h"
#include <cstdint>
#include <string>

namespace steamrot {

struct EngineResourcesConfig {
  uint32_t window_width{800};
  uint32_t window_height{600};
  std::string window_title{"SteamRot"};
  uint32_t framerate_limit{60};
  
  // NEW: Default asset configuration
  AssetConfig default_assets;
};

} // namespace steamrot
```

**File: `src/types/core/SceneData.h`**

```cpp
#pragma once

#include "AssetConfig.h"
#include "SceneInfo.h"

namespace steamrot {

struct SceneData {
  virtual ~SceneData() = default;
  
  SceneInfo scene_info;
  
  // NEW: Scene-specific asset configuration
  AssetConfig scene_assets;
};

} // namespace steamrot
```

---

## Data Provider Views

### Pattern: Expose AssetConfig Through Existing Providers

Instead of a separate `IAssetConfigProvider`, expose AssetConfig through existing providers using accessor methods.

### IEngineDataProvider Extension

**File: `src/interfaces/IEngineDataProvider.h`**

```cpp
class IEngineDataProvider {
public:
  virtual ~IEngineDataProvider() = default;

  // Existing methods
  virtual std::expected<EngineResourcesConfig, FailInfo>
  LoadEngineResourcesConfig() const = 0;
  
  virtual std::expected<EngineConfig, FailInfo> 
  LoadEngineConfig() const = 0;
  
  virtual std::expected<EngineState, FailInfo> 
  LoadEngineState() const = 0;

  ////////////////////////////////////////////////////////////
  /// @brief Get default asset configuration (view/accessor)
  ///
  /// Returns the default asset configuration from EngineResourcesConfig.
  /// This is a convenience method that loads EngineResourcesConfig and
  /// extracts the asset configuration.
  ///
  /// @return Default AssetConfig or failure information
  ////////////////////////////////////////////////////////////
  virtual std::expected<AssetConfig, FailInfo>
  GetDefaultAssetConfig() const = 0;
};
```

**Implementation Example:**

```cpp
// In FlatbuffersEngineDataProvider
std::expected<AssetConfig, FailInfo>
FlatbuffersEngineDataProvider::GetDefaultAssetConfig() const {
  // Load full EngineResourcesConfig
  auto config_result = LoadEngineResourcesConfig();
  if (!config_result.has_value())
    return std::unexpected(config_result.error());
  
  // Extract and return asset config (view)
  return config_result.value().default_assets;
}
```

### ISceneDataProvider Extension

**File: `src/interfaces/ISceneDataProvider.h`**

```cpp
class ISceneDataProvider {
public:
  virtual ~ISceneDataProvider() = default;

  // Existing method
  virtual std::expected<std::unique_ptr<SceneData>, FailInfo>
  ProvideDefaultSceneData(const SceneType scene_type) const = 0;

  ////////////////////////////////////////////////////////////
  /// @brief Get scene-specific asset configuration (view/accessor)
  ///
  /// Returns the asset configuration for a specific scene type.
  /// This is a convenience method that loads SceneData and
  /// extracts the asset configuration.
  ///
  /// @param scene_type The type of scene to get assets for
  /// @return Scene AssetConfig or failure information
  ////////////////////////////////////////////////////////////
  virtual std::expected<AssetConfig, FailInfo>
  GetSceneAssetConfig(const SceneType scene_type) const = 0;
};
```

**Implementation Example:**

```cpp
// In FlatbuffersSceneDataProvider
std::expected<AssetConfig, FailInfo>
FlatbuffersSceneDataProvider::GetSceneAssetConfig(
    const SceneType scene_type) const {
  // Load full SceneData
  auto scene_data_result = ProvideDefaultSceneData(scene_type);
  if (!scene_data_result.has_value())
    return std::unexpected(scene_data_result.error());
  
  // Extract and return asset config (view)
  return scene_data_result.value()->scene_assets;
}
```

### Benefits of the View Pattern

1. **No New Provider:** Reuses existing IEngineDataProvider and ISceneDataProvider
2. **Convenience:** Simple accessor methods for common use case
3. **Consistency:** Follows existing pattern of provider interfaces
4. **Flexibility:** Can still access full EngineResourcesConfig or SceneData if needed
5. **Clear Intent:** Method name `GetSceneAssetConfig()` makes purpose obvious

---

## SceneFactory Integration

### Current Pattern (Scenes Load Assets)

**Current Flow:**
```
Engine → SceneManager → Scene::Initialize()
                           ↓
                        LoadSceneAssets()
```

**Problem:** Each scene responsible for loading its own assets.

### Proposed Pattern (SceneFactory Loads Assets)

**Proposed Flow:**
```
Engine → SceneManager → SceneFactory::CreateScene()
                           ↓
                        1. Get scene config
                        2. Load scene assets
                        3. Create scene instance
                        4. Return configured scene
```

**Benefits:**
- **Centralized:** Asset loading in one place
- **Consistent:** All scenes use same loading pattern
- **Testable:** SceneFactory can be tested with mock providers
- **Lazy Loading:** Assets loaded on-demand when scene created

### Implementation

#### SceneFactory Enhancement

**File: `src/scenes/SceneFactory.h`**

```cpp
class SceneFactory {
private:
  DataAccessFactory &m_data_access_factory;
  AssetManager &m_asset_manager;
  
  ////////////////////////////////////////////////////////////
  /// @brief Load assets for a specific scene type
  ///
  /// Called internally before creating scene instance.
  ///
  /// @param scene_type The type of scene to load assets for
  /// @return Success or failure information
  ////////////////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  LoadSceneAssets(SceneType scene_type);

public:
  SceneFactory(DataAccessFactory &data_access_factory,
               AssetManager &asset_manager);

  ////////////////////////////////////////////////////////////
  /// @brief Create a fully configured scene
  ///
  /// 1. Loads scene-specific assets
  /// 2. Creates scene instance
  /// 3. Configures scene with loaded data
  ///
  /// @param scene_type The type of scene to create
  /// @param scene_context Context for the scene
  /// @return Unique pointer to configured scene
  ////////////////////////////////////////////////////////////
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateScene(SceneType scene_type, const SceneContext &scene_context);
};
```

**File: `src/scenes/SceneFactory.cpp`**

```cpp
std::expected<std::monostate, FailInfo>
SceneFactory::LoadSceneAssets(SceneType scene_type) {
  // Get scene data provider
  auto scene_provider_result = m_data_access_factory.GetSceneDataProvider();
  if (!scene_provider_result.has_value())
    return std::unexpected(scene_provider_result.error());
  
  // Get scene asset config (view method)
  auto config_result = 
      scene_provider_result.value()->GetSceneAssetConfig(scene_type);
  if (!config_result.has_value())
    return std::unexpected(config_result.error());
  
  // Load assets based on config
  return m_asset_manager.LoadAssets(config_result.value());
}

std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateScene(SceneType scene_type, 
                         const SceneContext &scene_context) {
  // 1. Load scene assets FIRST
  auto load_assets_result = LoadSceneAssets(scene_type);
  if (!load_assets_result.has_value())
    return std::unexpected(load_assets_result.error());
  
  // 2. Create scene instance (existing logic)
  std::unique_ptr<Scene> scene;
  switch (scene_type) {
    case SceneType::SceneType_TEST:
      scene = std::make_unique<TestScene>(scene_context);
      break;
    case SceneType::SceneType_TITLE:
      scene = std::make_unique<TitleScene>(scene_context);
      break;
    case SceneType::SceneType_CRAFTING:
      scene = std::make_unique<CraftingScene>(scene_context);
      break;
    default:
      return std::unexpected(FailInfo{
          FailMode::NonExistentEnumValue,
          "Invalid scene type"});
  }
  
  // 3. Configure scene with scene data (existing logic)
  // ...
  
  return scene;
}
```

#### SceneManager Simplification

**File: `src/scenes/SceneManager.cpp`**

```cpp
std::expected<std::monostate, FailInfo>
SceneManager::ChangeScene(SceneType scene_type) {
  // SceneFactory handles asset loading now!
  auto scene_result = m_scene_factory.CreateScene(scene_type, scene_context);
  if (!scene_result.has_value())
    return std::unexpected(scene_result.error());
  
  // Set the new scene
  m_current_scene = std::move(scene_result.value());
  
  return std::monostate{};
}
```

**Removed from scenes:**
- ❌ `Scene::LoadAssets()` method removed
- ❌ Individual scenes no longer call AssetManager
- ✅ SceneFactory handles all asset loading

---

## Implementation Details

### Phase 1: FlatBuffers Schema Updates

**Task 1.1: Rename AssetCollection → AssetConfig**
- [ ] Update `assets.fbs`: `AssetCollection` → `AssetConfig`
- [ ] Update `assets.fbs`: `FontDataFbs` → `FontConfigFbs`
- [ ] Update `scene_data.fbs`: `assets` → `scene_assets`
- [ ] Regenerate FlatBuffers headers

**Task 1.2: Add AssetConfig to EngineResourcesConfigFbs**
- [ ] Update `engine_resources_config.fbs`: Add `default_assets` field
- [ ] Regenerate FlatBuffers headers

**Task 1.3: Update Native Types**
- [ ] Update `EngineResourcesConfig.h`: Add `AssetConfig default_assets`
- [ ] Update `SceneData.h`: Add `AssetConfig scene_assets`

### Phase 2: Data Provider Extensions

**Task 2.1: Extend IEngineDataProvider**
- [ ] Add `GetDefaultAssetConfig()` method to interface
- [ ] Implement in `FlatbuffersEngineDataProvider`
- [ ] Test with unit tests

**Task 2.2: Extend ISceneDataProvider**
- [ ] Add `GetSceneAssetConfig(SceneType)` method to interface
- [ ] Implement in `FlatbuffersSceneDataProvider`
- [ ] Test with unit tests

### Phase 3: SceneFactory Integration

**Task 3.1: Add Asset Loading to SceneFactory**
- [ ] Add `AssetManager&` member to SceneFactory
- [ ] Add private `LoadSceneAssets(SceneType)` method
- [ ] Call `LoadSceneAssets()` in `CreateScene()` before scene instantiation
- [ ] Test scene creation with asset loading

**Task 3.2: Remove Asset Loading from Scenes**
- [ ] Remove `LoadAssets()` methods from individual scenes
- [ ] Verify scenes no longer call AssetManager directly
- [ ] Update scene tests

**Task 3.3: Update SceneManager**
- [ ] Pass `AssetManager` reference to SceneFactory constructor
- [ ] Verify `ChangeScene()` works with new pattern
- [ ] Integration tests for scene transitions with asset loading

### Phase 4: Engine Startup Integration

**Task 4.1: Load Default Assets at Startup**
- [ ] In Engine initialization:
  ```cpp
  // Get default asset config from engine data provider
  auto engine_provider = data_access_factory.GetEngineDataProvider();
  auto default_config = engine_provider->GetDefaultAssetConfig();
  
  // Load default assets
  asset_manager.LoadAssets(default_config);
  ```
- [ ] Test engine startup with default assets
- [ ] Verify window creation and asset availability

### Data Flow Summary

**Engine Startup:**
```
Engine::Initialize()
  ↓
IEngineDataProvider::GetDefaultAssetConfig()
  ↓
AssetManager::LoadAssets(default_config)
  ↓
Default assets loaded (fonts, UI styles)
```

**Scene Change:**
```
SceneManager::ChangeScene(scene_type)
  ↓
SceneFactory::CreateScene(scene_type)
  ↓
  1. SceneFactory::LoadSceneAssets(scene_type)
     ↓
     ISceneDataProvider::GetSceneAssetConfig(scene_type)
     ↓
     AssetManager::LoadAssets(scene_config)
  ↓
  2. Create scene instance
  ↓
  3. Configure scene with scene data
  ↓
Return configured scene
```

---

## Benefits Summary

### 1. AssetConfig Embedding
- ✅ Already exists in SceneData (just needs renaming)
- ✅ Natural extension for EngineResourcesConfig
- ✅ All asset config in one schema
- ✅ No separate config files needed

### 2. Data Provider Views
- ✅ Reuses existing providers (no new interfaces)
- ✅ Simple accessor methods (`GetDefaultAssetConfig()`, `GetSceneAssetConfig()`)
- ✅ Maintains single source of truth
- ✅ Easy to test

### 3. SceneFactory Integration
- ✅ Centralized asset loading
- ✅ Consistent pattern across all scenes
- ✅ Lazy loading (assets loaded when scene created)
- ✅ Simpler individual scenes (no asset loading logic)

### 4. Overall Architecture
- ✅ Clear separation: Data providers provide config, AssetManager loads assets, SceneFactory orchestrates
- ✅ Testable: Each component can be tested independently
- ✅ Flexible: Easy to add new asset types or loading sources
- ✅ Maintainable: Asset loading logic in one place

---

## Comparison: Before vs After

### Before (Proposed in Earlier Docs)

```
IAssetConfigProvider (separate interface)
  ↓
AssetManager::LoadAssets(config)
```

**Issues:**
- New interface needed
- Duplicate data (asset config separate from scene/engine config)
- Unclear where asset config lives

### After (Integrated Pattern)

```
IEngineDataProvider::GetDefaultAssetConfig()
  ↓ (view/accessor)
EngineResourcesConfig.default_assets
  ↓
AssetManager::LoadAssets(config)

ISceneDataProvider::GetSceneAssetConfig(scene_type)
  ↓ (view/accessor)
SceneData.scene_assets
  ↓
AssetManager::LoadAssets(config)
```

**Benefits:**
- ✅ No new interface
- ✅ Single source of truth (asset config embedded in existing configs)
- ✅ Clear ownership (engine config owns defaults, scene config owns scene assets)
- ✅ View pattern provides convenient access

---

## Migration Impact

### Changes to Previous Recommendations

**ASSET_MANAGER_REFINEMENTS.md Updates:**

1. **Remove IAssetConfigProvider Interface:**
   - ❌ Don't create separate `IAssetConfigProvider`
   - ✅ Instead: Extend `IEngineDataProvider` and `ISceneDataProvider` with view methods

2. **Update AssetManager Interface:**
   - ✅ Keep `LoadAssets(const AssetConfig &config)` as primary method
   - ✅ Update implementation to use provider views

3. **Update TODO List:**
   - Phase 1: Add FlatBuffers schema updates
   - Phase 2: Extend existing providers (not create new ones)
   - Phase 3: Add SceneFactory integration tasks

### Example Usage Patterns

**Loading Default Assets (Engine Startup):**
```cpp
// In Engine::Initialize()
auto engine_provider = data_access_factory.GetEngineDataProvider();
if (!engine_provider.has_value())
  return std::unexpected(engine_provider.error());

auto default_config = engine_provider.value()->GetDefaultAssetConfig();
if (!default_config.has_value())
  return std::unexpected(default_config.error());

auto load_result = asset_manager.LoadAssets(default_config.value());
```

**Loading Scene Assets (Scene Creation):**
```cpp
// In SceneFactory::LoadSceneAssets()
auto scene_provider = data_access_factory.GetSceneDataProvider();
if (!scene_provider.has_value())
  return std::unexpected(scene_provider.error());

auto scene_config = scene_provider.value()->GetSceneAssetConfig(scene_type);
if (!scene_config.has_value())
  return std::unexpected(scene_config.error());

return asset_manager.LoadAssets(scene_config.value());
```

---

## Document Metadata

**Created:** 2025-12-24  
**Author:** GitHub Copilot Agent  
**Purpose:** Integration pattern for AssetConfig with existing data providers  
**Status:** Ready for review  
**Related Documents:**
- ASSET_MANAGER_REFINEMENTS.md (architectural improvements)
- ASSET_MANAGER_ANALYSIS.md (base analysis)
- ASSET_MANAGER_TODO.md (implementation checklist)

**User Feedback Addressed:**
- AssetConfig embedded in EngineResourcesConfig and SceneData
- View/accessor methods on existing data providers
- SceneFactory handles asset loading (lazy loading pattern)
- Centralized asset loading orchestration
