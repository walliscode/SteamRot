# Nested Data Handling Patterns Analysis

## Problem Statement

Currently, accessing nested data structures through data providers is clunky. For example:
- When loading SceneData, getting nested AssetData requires multiple separate calls through viewers
- EngineData is split across multiple methods (LoadEngineResourcesConfig, LoadEngineConfig, LoadEngineState)
- No consistent pattern for wrapping related data structures
- No caching mechanism to easily access nested data

## Current Implementation

### Current Pattern: Separate Methods for Each Data Type

```cpp
// Example: Accessing AssetConfig from EngineDataProvider
auto provider_result = factory.GetEngineDataProvider();
IEngineDataProvider& provider = *provider_result.value();

// Get main config
auto resources_config = provider.LoadEngineResourcesConfig();

// To get nested AssetConfig, need separate viewer
auto viewer_result = provider.GetAssetConfigViewer();
auto asset_config = viewer_result.value()->ProvideAssetConfig();
```

**Issues:**
1. Multiple method calls to access nested data
2. No single unified data structure
3. Viewer pattern adds complexity for simple data access
4. No caching - data may be loaded multiple times

### Current Data Structures

**EngineData - Split across multiple types:**
- `EngineResourcesConfig` - Window configuration
- `EngineConfig` - Display and user preferences (contains nested DisplayConfig, UserPreferencesConfig)
- `EngineState` - Runtime state and subscriptions
- `AssetConfig` - Asset configuration (fonts, ui_styles)

These are loaded separately via different methods.

**SceneData - Minimal structure:**
```cpp
struct SceneData {
  SceneInfo scene_info;
  // No nested data yet
};
```

## Proposed Solution: Unified Data Wrapper Pattern

### Design Principles

1. **Wrapper Structs**: Create wrapper structs that contain all related data including nested data
2. **Single Load Method**: Providers return complete wrapper structs via single method
3. **Accessor Methods**: Wrappers provide accessor methods for easy nested data access
4. **Caching Optional**: Providers can cache wrappers if needed for performance
5. **Backward Compatibility**: Existing separate methods remain for gradual migration

### Proposed Pattern

#### 1. Create Unified Data Wrappers

```cpp
// Unified Engine Data
struct EngineData {
  EngineResourcesConfig resources_config;
  EngineConfig engine_config;
  EngineState engine_state;
  AssetConfig asset_config;  // Nested data included directly
  
  // Accessor methods for convenience
  const AssetConfig& GetAssetConfig() const { return asset_config; }
  const DisplayConfig& GetDisplayConfig() const { return engine_config.display; }
  const UserPreferencesConfig& GetUserPreferences() const { 
    return engine_config.user_preferences; 
  }
};

// Unified Scene Data (when needed in future)
struct SceneData {
  SceneInfo scene_info;
  AssetConfig asset_config;  // If scenes need their own asset configs
  
  // Accessor methods
  const AssetConfig& GetAssetConfig() const { return asset_config; }
};
```

#### 2. Provider Interface Returns Complete Data

```cpp
class IEngineDataProvider {
public:
  // New unified method
  virtual std::expected<EngineData, FailInfo> LoadEngineData() const = 0;
  
  // Existing methods remain for backward compatibility
  virtual std::expected<EngineResourcesConfig, FailInfo>
  LoadEngineResourcesConfig() const = 0;
  
  virtual std::expected<EngineConfig, FailInfo> LoadEngineConfig() const = 0;
  
  virtual std::expected<EngineState, FailInfo> LoadEngineState() const = 0;
  
  // Viewer methods can be deprecated or kept for special use cases
  virtual std::expected<std::unique_ptr<IAssetConfigViewer>, FailInfo>
  GetAssetConfigViewer() const = 0;
};
```

#### 3. Implementation Caches and Builds Complete Structure

```cpp
class FlatbuffersEngineDataProvider : public IEngineDataProvider {
private:
  FlatbuffersDataLoader m_loader;
  mutable std::optional<EngineData> m_cached_data;  // Optional caching

public:
  std::expected<EngineData, FailInfo> LoadEngineData() const override {
    // Check cache
    if (m_cached_data.has_value()) {
      return m_cached_data.value();
    }
    
    EngineData data;
    
    // Load all components
    auto resources_result = LoadEngineResourcesConfig();
    if (!resources_result) return std::unexpected(resources_result.error());
    data.resources_config = resources_result.value();
    
    auto config_result = LoadEngineConfig();
    if (!config_result) return std::unexpected(config_result.error());
    data.engine_config = config_result.value();
    
    auto state_result = LoadEngineState();
    if (!state_result) return std::unexpected(state_result.error());
    data.engine_state = state_result.value();
    
    // Load nested AssetConfig
    auto viewer_result = GetAssetConfigViewer();
    if (viewer_result) {
      auto asset_result = viewer_result.value()->ProvideAssetConfig();
      if (asset_result) {
        data.asset_config = asset_result.value();
      }
    }
    
    // Cache if desired
    m_cached_data = data;
    
    return data;
  }
};
```

#### 4. Simplified Usage

```cpp
// Single call to get all engine data
auto provider_result = factory.GetEngineDataProvider();
auto engine_data_result = provider_result.value()->LoadEngineData();

if (engine_data_result) {
  const EngineData& data = engine_data_result.value();
  
  // Easy access to nested data
  const AssetConfig& assets = data.GetAssetConfig();
  const DisplayConfig& display = data.GetDisplayConfig();
  
  // Or direct member access
  ConfigureWindow(data.resources_config);
  LoadAssets(data.asset_config);
}
```

## Migration Strategy

### Phase 1: Create Wrapper Structs (Current)
- Create `EngineData` wrapper struct with all nested data
- Add accessor methods for convenience
- Document the pattern

### Phase 2: Add LoadEngineData() Method (Future)
- Implement `LoadEngineData()` in `IEngineDataProvider` interface
- Implement in `FlatbuffersEngineDataProvider`
- Keep existing methods for backward compatibility
- Add tests for new method

### Phase 3: Migrate Callers (Future)
- Update `Engine::StartUp()` to use new `LoadEngineData()`
- Update other callers as needed
- Verify all functionality works

### Phase 4: Apply to Other Data Providers (Future)
- Create wrappers for SceneData if needed
- Apply same pattern to SceneManagerData
- Standardize across all data providers

### Phase 5: Deprecate Old Methods (Far Future)
- Mark old separate load methods as deprecated
- Eventually remove if no longer needed

## Benefits

1. **Consistency**: Unified pattern across all data providers
2. **Simplicity**: Single method call instead of multiple
3. **Easy Nesting**: Nested data included directly in wrapper
4. **Accessor Convenience**: Accessor methods provide clean syntax
5. **Caching**: Optional caching for performance
6. **Future-Proof**: Easy to add new nested data types
7. **Backward Compatible**: Existing code continues to work

## Examples of Application

### Example 1: Engine Initialization
```cpp
// Before (clunky)
auto provider = factory.GetEngineDataProvider().value();
auto resources = provider->LoadEngineResourcesConfig().value();
auto config = provider->LoadEngineConfig().value();
auto state = provider->LoadEngineState().value();
auto viewer = provider->GetAssetConfigViewer().value();
auto assets = viewer->ProvideAssetConfig().value();

ConfigureEngineResources(engine_resources, resources);
LoadAssets(assets);

// After (clean)
auto provider = factory.GetEngineDataProvider().value();
auto engine_data = provider->LoadEngineData().value();

ConfigureEngineResources(engine_resources, engine_data.resources_config);
LoadAssets(engine_data.GetAssetConfig());
```

### Example 2: Scene Configuration
```cpp
// If SceneData needs asset configuration in future

// Before (clunky)
auto provider = factory.GetSceneDataProvider().value();
auto scene_data = provider->ProvideDefaultSceneData(type).value();
auto viewer = provider->GetAssetConfigViewer().value();
auto assets = viewer->ProvideAssetConfig().value();

ConfigureScene(scene, scene_data, assets);

// After (clean)
auto provider = factory.GetSceneDataProvider().value();
auto scene_data = provider->ProvideDefaultSceneData(type).value();

ConfigureScene(scene, scene_data);  // scene_data includes assets
```

## Decision Points

### Question 1: Should base structs always nest related data?
**Answer**: Yes, if the nested data is logically part of the parent concept.
- EngineData should include AssetConfig (assets are part of engine resources)
- SceneData should include AssetConfig if scenes have their own assets
- Use composition to build complete data structures

### Question 2: Should providers create the full struct?
**Answer**: Yes, providers should return complete, fully-populated structures.
- Provider's responsibility is to load and assemble all data
- Caller shouldn't have to make multiple calls to get related data
- Simplifies caller code and reduces errors

### Question 3: Should we cache the main struct?
**Answer**: Yes, when appropriate for performance.
- Engine data loaded once at startup: cache recommended
- Scene data loaded multiple times: consider caching per scene type
- Use `mutable std::optional<DataType>` for optional caching
- Clear cache when data needs to be reloaded

### Question 4: How to handle backward compatibility?
**Answer**: Keep existing methods, add new unified method.
- Existing separate load methods remain functional
- New `LoadEngineData()` method provides unified access
- Gradual migration path
- Eventually deprecate old methods if desired

## Implementation Checklist

- [x] Document current clunky patterns
- [x] Analyze problem and propose solution
- [x] Design unified data wrapper pattern
- [x] Define EngineData wrapper struct
- [x] Define migration strategy
- [x] Document decision points and benefits

## Future Considerations

1. **FlatBuffers Schema Changes**: If we want FlatBuffers to directly serialize EngineData wrapper, we'd need schema updates. Current approach keeps wrappers in C++ only.

2. **Data Validation**: Wrapper structs could include validation methods to ensure data consistency.

3. **Builder Pattern**: For complex construction, consider builder pattern for EngineData.

4. **Immutability**: Consider making wrapper structs immutable after construction.

## Conclusion

The unified data wrapper pattern provides a consistent, simple way to handle nested data structures. By wrapping related data together and providing accessor methods, we eliminate clunky multi-call patterns and make the codebase easier to work with and extend in the future.

The migration strategy allows gradual adoption without breaking existing code, and the pattern can be applied consistently across all data providers in the system.
