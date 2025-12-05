# Data Provider System - Migration Complete

## Overview

The SteamRot engine now uses an interface-based data provider system that abstracts data sources (FlatBuffers, JSON, XML, Lua) from game code. This document explains the current state after Phase 3 completion.

## Architecture

### Three-Layer Design

```
┌─────────────────────────────────────────────┐
│         Game Code (Engine, Scenes)          │
│  Uses native C++ structs and interfaces     │
└─────────────────┬───────────────────────────┘
                  │
                  ↓ LoadData()
┌─────────────────────────────────────────────┐
│      Provider Interfaces (Abstract)         │
│  - IEngineDataProvider                      │
│  - ISceneDataProvider                       │
│  - IAssetDataProvider                       │
│  - IFragmentDataProvider                    │
│  - IGameConfigProvider                      │
└─────────────────┬───────────────────────────┘
                  │
                  ↓ Implementation
┌─────────────────────────────────────────────┐
│   FlatBuffers Implementations (Concrete)    │
│  - FlatbuffersEngineDataProvider            │
│  - FlatbuffersSceneDataProvider             │
│  - FlatbuffersAssetDataProvider             │
│  - FlatbuffersFragmentDataProvider          │
│  - FlatbuffersGameConfigProvider            │
└─────────────────────────────────────────────┘
```

## Provider Interfaces

### IEngineDataProvider
**Purpose**: Load engine core configuration (window, framerate)  
**Returns**: Native C++ structs  
**Location**: `src/data_providers/IEngineDataProvider.h`

```cpp
struct EngineCoreData {
  uint32_t window_width{800};
  uint32_t window_height{600};
  std::string window_title{"SteamRot"};
  uint32_t framerate_limit{60};
};

class IEngineDataProvider {
  virtual std::expected<EngineCoreData, FailInfo>
  LoadEngineCoreData() const = 0;
};
```

**Usage**:
```cpp
#include "provider_factory.h"

IEngineDataProvider& provider = GetEngineDataProvider();
auto result = provider.LoadEngineCoreData();
if (result.has_value()) {
  const EngineCoreData& data = result.value();
  // Use native struct directly
  uint32_t width = data.window_width;  // Direct member access
}
```

### ISceneDataProvider
**Purpose**: Load scene configuration (entity pool size, render texture dimensions)  
**Returns**: Native C++ structs  
**Location**: `src/data_providers/ISceneDataProvider.h`

```cpp
struct SceneCoreData {
  uint32_t entity_pool_size{100};
  uint32_t render_texture_width{800};
  uint32_t render_texture_height{600};
};

class ISceneDataProvider {
  virtual std::expected<SceneCoreData, FailInfo>
  LoadSceneCoreData(SceneType scene_type) const = 0;
};
```

**Usage**:
```cpp
ISceneDataProvider& provider = GetSceneDataProvider();
auto result = provider.LoadSceneCoreData(SceneType::SceneType_TITLE);
```

### IAssetDataProvider
**Purpose**: Load asset file lists (fonts, textures, styles)  
**Returns**: Native C++ structs  
**Location**: `src/data_providers/IAssetDataProvider.h`

```cpp
struct FontData {
  std::string name;
};

struct UIStyleReference {
  std::string name;
};

struct AssetData {
  std::vector<FontData> fonts;
  std::vector<UIStyleReference> ui_styles;
};

class IAssetDataProvider {
  virtual std::expected<AssetData, FailInfo>
  LoadAssetData() const = 0;
  
  virtual std::expected<AssetData, FailInfo>
  LoadSceneAssetData(SceneType scene_type) const = 0;
};
```

### IFragmentDataProvider
**Purpose**: Load UI fragment data  
**Returns**: Native C++ Fragment struct (already existed)  
**Location**: `src/data_providers/IFragmentDataProvider.h`

```cpp
class IFragmentDataProvider {
  virtual std::expected<Fragment, FailInfo>
  LoadFragment(const std::string &fragment_name) const = 0;
};
```

### IGameConfigProvider
**Purpose**: Load complex game configuration (subscriptions, event buses, scene manager)  
**Returns**: **FlatBuffers types** (native struct conversion deferred to Phase 4)  
**Location**: `src/data_providers/IGameConfigProvider.h`

```cpp
class IGameConfigProvider {
  virtual std::expected<const EngineDataFbs*, FailInfo>
  LoadEngineConfig() const = 0;
  
  virtual std::expected<const SceneManagerData*, FailInfo>
  LoadSceneManagerConfig() const = 0;
};
```

**Note**: This provider still returns FlatBuffers types because subscriber/event configuration hasn't been converted to native structs yet (Phase 4 work).

## Usage Examples

### Engine Startup
```cpp
// src/engine/Engine.cpp
#include "provider_factory.h"

std::expected<std::monostate, FailInfo> Engine::StartUp() {
  // Load core config via provider
  IEngineDataProvider& data_provider = GetEngineDataProvider();
  auto engine_core_result = data_provider.LoadEngineCoreData();
  
  // Configure GameCore from native struct
  auto configure_result = 
      core::ConfigureGameCore(m_game_core, engine_core_result.value());
  
  return std::monostate{};
}
```

### Scene Creation
```cpp
// src/scenes/SceneFactory.cpp
#include "provider_factory.h"

ISceneDataProvider& data_provider = GetSceneDataProvider();
auto scene_core_result = data_provider.LoadSceneCoreData(scene_type);

auto configure_result =
    core::ConfigureSceneCore(scene_ptr->m_scene_core, 
                           scene_core_result.value());
```

### Asset Loading
```cpp
// src/assets/AssetManager.cpp
#include "provider_factory.h"

IAssetDataProvider& asset_provider = GetAssetDataProvider();
auto asset_data_result = asset_provider.LoadAssetData();

AssetData& asset_data = asset_data_result.value();
for (const auto& font_data : asset_data.fonts) {
  auto add_font_result = AddFont(font_data.name);
}
```

## Provider Factory

All providers are accessed through factory functions in `provider_factory.h`:

```cpp
namespace steamrot {
  IEngineDataProvider& GetEngineDataProvider();
  ISceneDataProvider& GetSceneDataProvider();
  IAssetDataProvider& GetAssetDataProvider();
  IFragmentDataProvider& GetFragmentDataProvider();
  IGameConfigProvider& GetGameConfigProvider();
}
```

**Current Implementation**: All return FlatBuffers-based providers (singleton pattern)  
**Future**: Factory can be made configurable to return JSON/Lua/XML providers

## Migration Status

### ✅ Completed (Phase 3)
- All provider interfaces defined
- FlatBuffers implementations created
- Engine, SceneFactory, AssetManager migrated to providers
- Old IGameDataProvider system removed
- No source code directly instantiates FlatbuffersDataLoader

### 🔧 Using Providers (FlatBuffers Backend)
- `Engine.cpp` - IEngineDataProvider
- `GameEngine.cpp` - IGameConfigProvider  
- `SceneFactory.cpp` - ISceneDataProvider
- `AssetManager.cpp` - IAssetDataProvider

### ⏳ Deferred to Phase 4
- Native structs for subscriber data
- Native structs for event bus data
- Native structs for entity/component data
- UI style provider (StylesConfigurator works fine as-is)

## Adding New Providers

### Example: JSON Engine Data Provider

1. **Implement Interface**:
```cpp
// JsonEngineDataProvider.h
class JsonEngineDataProvider : public IEngineDataProvider {
  std::expected<EngineCoreData, FailInfo>
  LoadEngineCoreData() const override {
    // Load JSON file
    std::ifstream file("engine_config.json");
    nlohmann::json j;
    file >> j;
    
    // Convert to native struct
    EngineCoreData data;
    data.window_width = j["window"]["width"];
    data.window_height = j["window"]["height"];
    // ...
    
    return data;
  }
};
```

2. **Update Factory** (optional):
```cpp
// provider_factory.cpp
IEngineDataProvider& GetEngineDataProvider() {
  #ifdef USE_JSON_PROVIDERS
    static JsonEngineDataProvider provider;
  #else
    static FlatbuffersEngineDataProvider provider;
  #endif
  return provider;
}
```

3. **Game code unchanged!** Engine.cpp doesn't need to change.

## Benefits

✅ **Format Flexibility**: Easy to add JSON, Lua, XML implementations  
✅ **Clean Game Code**: Works with native C++ structs, not FlatBuffers  
✅ **Testability**: Can mock providers for unit tests  
✅ **Separation of Concerns**: Data loading vs configuration logic separated  
✅ **Interface-based Design**: Game code depends on abstractions, not implementations

## Known Limitations

1. **IGameConfigProvider**: Still returns FlatBuffers types (Phase 4 will fix)
2. **Entity System**: FlatbuffersConfigurator not yet migrated (Phase 4)
3. **UI Styles**: StylesConfigurator uses FlatbuffersDataLoader directly (low priority)
4. **Wrapper Pattern**: Providers wrap FlatbuffersDataLoader (optimization opportunity)

## Related Documentation

- `documentation/proposals/DATA_LOADING_INTERFACE_README.md` - Full documentation index
- `documentation/proposals/DATA_LOADING_INTERFACE_SYSTEM.md` - Technical analysis
- `documentation/proposals/DATA_LOADING_INTERFACE_CHECKLIST.md` - Implementation checklist
- `documentation/proposals/DATA_LOADING_INTERFACE_QUICK_REF.md` - Quick reference guide

## Future Work (Phase 4)

When converting complex types to native structs:

1. Define native structs for subscribers, event buses, scene manager
2. Create providers that return native structs
3. Update configurators to accept native structs
4. Remove FlatbuffersDataLoader entirely

**Estimated Effort**: 4-6 weeks  
**Risk**: High (deep integration with event/entity systems)  
**Recommendation**: Defer until there's a clear need

---

**Status**: Phase 3 Complete (December 2024)  
**Next Step**: User validation and testing
