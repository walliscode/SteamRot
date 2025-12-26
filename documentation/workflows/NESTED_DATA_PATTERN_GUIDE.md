# Nested Data Pattern Guide

## Overview

This guide explains how to consistently handle nested data structures in the SteamRot codebase. Follow these patterns when adding new data types or working with existing data providers.

## Core Pattern

### The Three Rules

1. **Base structs must contain nested structs** - If data is logically related, nest it directly
2. **Providers create the full struct** - Providers assemble complete data structures with all nested data
3. **Cache the main struct** - Optionally cache complete structures for easy access to nested data

## When to Apply This Pattern

Apply this pattern when:
- Adding new data that is logically part of an existing structure
- Creating a new data provider
- Refactoring existing data access patterns
- Data has a parent-child or contains relationship

Do not apply when:
- Data is truly independent (no logical nesting relationship)
- Data comes from different sources that shouldn't be coupled
- Real-time data that changes frequently (caching may not be appropriate)

## Step-by-Step Guide

### Adding Nested Data to Existing Structure

**Example: Adding new nested data to EngineData**

#### Step 1: Define the nested data struct

```cpp
// File: src/types/engine/AudioConfig.h
namespace steamrot {

struct AudioConfig {
  float master_volume{1.0f};
  float music_volume{0.8f};
  float sfx_volume{1.0f};
  bool mute_when_unfocused{true};
};

} // namespace steamrot
```

#### Step 2: Add to parent wrapper struct

```cpp
// File: src/types/engine/EngineData.h
#include "AudioConfig.h"
#include "AssetConfig.h"
#include "EngineConfig.h"
#include "EngineResourcesConfig.h"
#include "EngineState.h"

namespace steamrot {

struct EngineData {
  EngineResourcesConfig resources_config;
  EngineConfig engine_config;
  EngineState engine_state;
  AssetConfig asset_config;
  AudioConfig audio_config;  // New nested data
  
  // Accessor methods
  const AssetConfig& GetAssetConfig() const { return asset_config; }
  const AudioConfig& GetAudioConfig() const { return audio_config; }
  const DisplayConfig& GetDisplayConfig() const { 
    return engine_config.display; 
  }
};

} // namespace steamrot
```

#### Step 3: Update provider to load nested data

```cpp
// File: src/data_providers/FlatbuffersEngineDataProvider.cpp

std::expected<EngineData, FailInfo> 
FlatbuffersEngineDataProvider::LoadEngineData() const {
  
  EngineData data;
  
  // Load existing components
  auto resources_result = LoadEngineResourcesConfig();
  if (!resources_result) return std::unexpected(resources_result.error());
  data.resources_config = resources_result.value();
  
  // ... load other components ...
  
  // Load new nested AudioConfig
  auto audio_result = LoadAudioConfig();
  if (!audio_result) return std::unexpected(audio_result.error());
  data.audio_config = audio_result.value();
  
  return data;
}
```

#### Step 4: Use the nested data

```cpp
// Example usage in Engine initialization
auto engine_data = provider->LoadEngineData().value();

// Easy access via accessor
const AudioConfig& audio = engine_data.GetAudioConfig();
ConfigureAudioSystem(audio);

// Or direct member access
if (engine_data.audio_config.mute_when_unfocused) {
  // Handle muting
}
```

### Creating New Data Provider with Nested Data

**Example: Creating a new UIConfigurationData provider**

#### Step 1: Define all data structures (base and nested)

```cpp
// File: src/types/ui_config/ThemeConfig.h
struct ThemeConfig {
  std::string theme_name;
  std::vector<ColorDefinition> colors;
};

// File: src/types/ui_config/LayoutConfig.h
struct LayoutConfig {
  uint32_t default_padding{10};
  uint32_t default_margin{5};
};

// File: src/types/ui_config/UIConfigurationData.h
#include "LayoutConfig.h"
#include "ThemeConfig.h"

struct UIConfigurationData {
  ThemeConfig theme;
  LayoutConfig layout;
  
  // Accessor methods
  const ThemeConfig& GetTheme() const { return theme; }
  const LayoutConfig& GetLayout() const { return layout; }
};
```

#### Step 2: Create provider interface

```cpp
// File: src/interfaces/IUIConfigurationProvider.h

class IUIConfigurationProvider {
public:
  virtual ~IUIConfigurationProvider() = default;
  
  // Single unified method returns complete structure
  virtual std::expected<UIConfigurationData, FailInfo>
  LoadUIConfiguration() const = 0;
  
  // Optional: Separate methods for individual components
  // (for backward compatibility or special use cases)
  virtual std::expected<ThemeConfig, FailInfo>
  LoadThemeConfig() const = 0;
  
  virtual std::expected<LayoutConfig, FailInfo>
  LoadLayoutConfig() const = 0;
};
```

#### Step 3: Implement provider

```cpp
// File: src/data_providers/FlatbuffersUIConfigurationProvider.h

class FlatbuffersUIConfigurationProvider : public IUIConfigurationProvider {
private:
  FlatbuffersDataLoader m_loader;
  mutable std::optional<UIConfigurationData> m_cached_data;
  
public:
  std::expected<UIConfigurationData, FailInfo>
  LoadUIConfiguration() const override;
  
  std::expected<ThemeConfig, FailInfo>
  LoadThemeConfig() const override;
  
  std::expected<LayoutConfig, FailInfo>
  LoadLayoutConfig() const override;
};

// File: src/data_providers/FlatbuffersUIConfigurationProvider.cpp

std::expected<UIConfigurationData, FailInfo>
FlatbuffersUIConfigurationProvider::LoadUIConfiguration() const {
  
  // Check cache
  if (m_cached_data.has_value()) {
    return m_cached_data.value();
  }
  
  UIConfigurationData data;
  
  // Load all nested components
  auto theme_result = LoadThemeConfig();
  if (!theme_result) return std::unexpected(theme_result.error());
  data.theme = theme_result.value();
  
  auto layout_result = LoadLayoutConfig();
  if (!layout_result) return std::unexpected(layout_result.error());
  data.layout = layout_result.value();
  
  // Cache the complete structure
  m_cached_data = data;
  
  return data;
}

std::expected<ThemeConfig, FailInfo>
FlatbuffersUIConfigurationProvider::LoadThemeConfig() const {
  // Load from FlatBuffers
  auto fb_result = m_loader.ProvideThemeConfigFbs();
  if (!fb_result) return std::unexpected(fb_result.error());
  
  // Convert to native struct
  ThemeConfig config;
  const auto* fb_data = fb_result.value();
  if (fb_data->theme_name()) {
    config.theme_name = fb_data->theme_name()->str();
  }
  // ... convert other fields ...
  
  return config;
}
```

#### Step 4: Register with factory

```cpp
// File: src/data_access/DataAccessFactory.h

class DataAccessFactory {
public:
  std::expected<std::unique_ptr<IUIConfigurationProvider>, FailInfo>
  GetUIConfigurationProvider() const;
};

// File: src/data_access/DataAccessFactory.cpp

std::expected<std::unique_ptr<IUIConfigurationProvider>, FailInfo>
DataAccessFactory::GetUIConfigurationProvider() const {
  return std::make_unique<FlatbuffersUIConfigurationProvider>();
}
```

## Best Practices

### 1. Accessor Methods

Always provide accessor methods for nested data:

```cpp
struct ParentData {
  NestedData nested;
  
  // Good: Const accessor
  const NestedData& GetNested() const { return nested; }
  
  // Also good: Non-const accessor if mutability is needed
  NestedData& GetNested() { return nested; }
};
```

### 2. Const Correctness

```cpp
// Provider methods should be const
std::expected<DataType, FailInfo> LoadData() const override;

// Accessor methods should return const references
const NestedType& GetNested() const { return nested; }

// Use mutable for caching in const methods
mutable std::optional<DataType> m_cached_data;
```

### 3. Error Handling

Always check results when loading nested data:

```cpp
std::expected<ParentData, FailInfo> LoadParentData() const {
  ParentData data;
  
  // Check each nested data load
  auto nested_result = LoadNestedData();
  if (!nested_result) {
    return std::unexpected(nested_result.error());
  }
  data.nested = nested_result.value();
  
  return data;
}
```

### 4. Caching Strategy

```cpp
// Cache when data is:
// - Expensive to load
// - Loaded frequently
// - Stable (doesn't change during runtime)

mutable std::optional<DataType> m_cached_data;

std::expected<DataType, FailInfo> LoadData() const {
  if (m_cached_data.has_value()) {
    return m_cached_data.value();
  }
  
  // Load data...
  DataType data = /* ... */;
  
  m_cached_data = data;
  return data;
}

// Provide method to invalidate cache if needed
void InvalidateCache() { m_cached_data.reset(); }
```

### 5. Documentation

Document the nesting relationship:

```cpp
////////////////////////////////////////////////////////////
/// @brief Complete engine configuration data.
///
/// Contains all engine-level configuration including:
/// - resources_config: Window and resource settings
/// - engine_config: Display and user preferences
/// - engine_state: Runtime state and subscriptions
/// - asset_config: Asset loading configuration (nested)
/// - audio_config: Audio system configuration (nested)
///
/// Use accessor methods for convenient access to nested data.
////////////////////////////////////////////////////////////
struct EngineData {
  // ...
};
```

## Common Patterns

### Pattern 1: Optional Nested Data

Some nested data may be optional:

```cpp
struct GameData {
  PlayerData player;
  std::optional<MultiplayerConfig> multiplayer;  // Optional
  
  bool HasMultiplayerConfig() const { 
    return multiplayer.has_value(); 
  }
  
  const MultiplayerConfig& GetMultiplayerConfig() const {
    if (!multiplayer.has_value()) {
      throw std::runtime_error("No multiplayer config available");
    }
    return multiplayer.value();
  }
};
```

### Pattern 2: Collections of Nested Data

```cpp
struct LevelData {
  LevelInfo info;
  std::vector<EntityConfig> entities;  // Collection of nested data
  
  const std::vector<EntityConfig>& GetEntities() const { 
    return entities; 
  }
  
  size_t GetEntityCount() const { 
    return entities.size(); 
  }
};
```

### Pattern 3: Deep Nesting

Limit nesting depth for maintainability:

```cpp
// Good: 2-3 levels of nesting
struct GameData {
  PlayerData player;  // Level 1
  
  struct PlayerData {
    InventoryData inventory;  // Level 2
    
    struct InventoryData {
      std::vector<ItemData> items;  // Level 3 - stop here
    };
  };
};

// Avoid: Too deep nesting
// Consider flattening or refactoring if you need more than 3 levels
```

### Pattern 4: Shared Nested Data

When multiple parent types share the same nested data:

```cpp
// Define shared nested data once
struct AssetConfig {
  std::vector<FontData> fonts;
  std::vector<std::string> ui_styles;
};

// Multiple parents can include it
struct EngineData {
  AssetConfig asset_config;  // Shared
  // ...
};

struct SceneData {
  AssetConfig asset_config;  // Shared type, different instance
  // ...
};
```

#### Should You Create a Shared Provider for Shared Types?

**Question**: If AssetConfig is used in both EngineData and SceneData, should we create an `IAssetDataProvider` interface?

**Answer**: No. Shared *type* doesn't require shared *provider*.

**Why not:**
- Each context may need different configurations (engine assets vs scene-specific assets)
- Each wrapper struct owns its own instance
- Adds interface complexity without benefit
- Provider pattern is for abstracting data *sources*, not for code reuse

**Instead, do this:**

```cpp
// Option 1: Each provider loads directly
class FlatbuffersEngineDataProvider {
  std::expected<EngineData, FailInfo> LoadEngineData() const {
    EngineData data;
    // Load AssetConfig from engine data source
    data.asset_config = LoadAssetConfigFromEngineSource();
    return data;
  }
};

class FlatbuffersSceneDataProvider {
  std::expected<SceneData, FailInfo> LoadSceneData() const {
    SceneData data;
    // Load AssetConfig from scene data source
    data.asset_config = LoadAssetConfigFromSceneSource();
    return data;
  }
};

// Option 2: Share helper functions, not interfaces
namespace asset_helpers {
  AssetConfig ConvertFromFbs(const AssetConfigFbs* fbs_data) {
    // Shared conversion logic
  }
}

class FlatbuffersEngineDataProvider {
  std::expected<EngineData, FailInfo> LoadEngineData() const {
    EngineData data;
    auto fbs_data = m_loader.GetEngineAssetConfigFbs();
    data.asset_config = asset_helpers::ConvertFromFbs(fbs_data);
    return data;
  }
};
```

**Key principle**: Use helper functions or composition for shared logic, not provider interfaces. Provider interfaces abstract *where* data comes from, not *how* it's converted.

## Testing Nested Data

### Test the Complete Structure

```cpp
TEST_CASE("EngineData contains all nested data", "[unit][EngineData]") {
  steamrot::FlatbuffersEngineDataProvider provider;
  auto result = provider.LoadEngineData();
  
  REQUIRE(result.has_value());
  const auto& data = result.value();
  
  // Test nested data is populated
  REQUIRE(!data.asset_config.fonts.empty());
  REQUIRE(data.audio_config.master_volume >= 0.0f);
  
  // Test accessors work
  const auto& assets = data.GetAssetConfig();
  REQUIRE(&assets == &data.asset_config);
}
```

### Test Caching Behavior

```cpp
TEST_CASE("Provider caches loaded data", "[unit][Provider]") {
  steamrot::FlatbuffersEngineDataProvider provider;
  
  // First load
  auto result1 = provider.LoadEngineData();
  REQUIRE(result1.has_value());
  
  // Second load should return cached data
  auto result2 = provider.LoadEngineData();
  REQUIRE(result2.has_value());
  
  // Verify it's the same data (implementation dependent)
}
```

## Migration Checklist

When migrating existing code to use this pattern:

- [ ] Identify related data that should be nested
- [ ] Create wrapper struct with nested data members
- [ ] Add accessor methods to wrapper struct
- [ ] Update provider interface to return wrapper
- [ ] Implement unified load method in provider
- [ ] Add caching if appropriate
- [ ] Keep old methods for backward compatibility
- [ ] Update tests for new pattern
- [ ] Document the nesting relationship
- [ ] Update calling code to use new pattern
- [ ] Verify all functionality still works

## References

- See `documentation/analysis/NESTED_DATA_HANDLING_PATTERNS.md` for detailed analysis
- See existing implementations:
  - `src/types/core/EngineConfig.h` - Example of nested DisplayConfig and UserPreferencesConfig
  - `src/data_providers/FlatbuffersEngineDataProvider.cpp` - Example provider implementation
  - `src/types/assets/AssetConfig.h` - Example of shared nested data structure
