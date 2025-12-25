# Nested Data Handling - Quick Reference

## TL;DR

**Problem**: Accessing nested data (like AssetConfig inside EngineData) is clunky - requires multiple method calls through viewers.

**Solution**: Create wrapper structs that contain all related data, providers return complete structures, optionally cache for easy access.

## The Three Rules

1. **Base structs must contain nested structs** - Nest logically related data directly
2. **Providers create the full struct** - Return complete, populated structures  
3. **Cache the main struct** - Optionally cache for easy nested data access

## Quick Example

### Before (Clunky)
```cpp
// Multiple calls to get nested data
auto provider = factory.GetEngineDataProvider().value();
auto resources = provider->LoadEngineResourcesConfig().value();
auto viewer = provider->GetAssetConfigViewer().value();
auto assets = viewer->ProvideAssetConfig().value();

ConfigureEngine(resources);
LoadAssets(assets);
```

### After (Clean)
```cpp
// Single call, easy access to nested data
auto provider = factory.GetEngineDataProvider().value();
auto engine_data = provider->LoadEngineData().value();

ConfigureEngine(engine_data.resources_config);
LoadAssets(engine_data.GetAssetConfig());
```

## Pattern Structure

```cpp
// 1. Define wrapper struct with nested data
struct ParentData {
  BaseConfig base_config;
  NestedConfig nested_config;  // Nested data included directly
  
  // Accessor methods for convenience
  const NestedConfig& GetNestedConfig() const { return nested_config; }
};

// 2. Provider returns complete structure
class IDataProvider {
public:
  virtual std::expected<ParentData, FailInfo> LoadData() const = 0;
};

// 3. Implementation with optional caching
class FlatbuffersDataProvider : public IDataProvider {
private:
  mutable std::optional<ParentData> m_cached_data;
  
public:
  std::expected<ParentData, FailInfo> LoadData() const override {
    if (m_cached_data.has_value()) {
      return m_cached_data.value();
    }
    
    ParentData data;
    // Load and populate all fields including nested data
    // ...
    
    m_cached_data = data;
    return data;
  }
};
```

## When to Use

✅ **Use this pattern when:**
- Adding new data that's logically part of an existing structure
- Creating a new data provider
- Data has a parent-child or contains relationship

❌ **Don't use when:**
- Data is truly independent
- Data comes from different sources that shouldn't be coupled
- Real-time data that changes frequently (caching may not work)

## Checklist for Adding Nested Data

- [ ] Define nested data struct
- [ ] Add to parent wrapper struct
- [ ] Add accessor method: `const Nested& GetNested() const`
- [ ] Update provider to load nested data in unified method
- [ ] Add caching if data is expensive to load
- [ ] Test complete structure loads correctly
- [ ] Update callers to use new pattern

## Key Benefits

- ✨ **Consistency** - Same pattern everywhere
- 🎯 **Simplicity** - One call instead of many
- 🔗 **Easy Nesting** - Direct access to nested data
- 🚀 **Performance** - Optional caching
- 📈 **Future-Proof** - Easy to extend

## Full Documentation

- 📖 **Analysis**: `documentation/analysis/NESTED_DATA_HANDLING_PATTERNS.md`
- 📘 **Developer Guide**: `documentation/workflows/NESTED_DATA_PATTERN_GUIDE.md`

## Examples in Codebase

**Good Examples** (already using parts of this pattern):
- `src/types/core/EngineConfig.h` - Nests DisplayConfig and UserPreferencesConfig
- `src/data_providers/FlatbuffersEngineDataProvider.cpp` - Loads multiple data types

**Future Improvements**:
- Create unified `EngineData` wrapper
- Add `LoadEngineData()` method to return complete structure
- Migrate `Engine::StartUp()` to use unified pattern

## Common Patterns

### Optional Nested Data
```cpp
struct GameData {
  PlayerData player;
  std::optional<MultiplayerConfig> multiplayer;  // Optional
  
  bool HasMultiplayer() const { return multiplayer.has_value(); }
};
```

### Collections of Nested Data
```cpp
struct LevelData {
  std::vector<EntityConfig> entities;  // Collection
  
  const std::vector<EntityConfig>& GetEntities() const { return entities; }
  size_t GetEntityCount() const { return entities.size(); }
};
```

### Shared Nested Type
```cpp
// Same type used in multiple parents
struct AssetConfig { /* ... */ };

struct EngineData {
  AssetConfig asset_config;  // Shared type
};

struct SceneData {
  AssetConfig asset_config;  // Same type, different instance
};
```

## Best Practices

1. **Always use const accessors**: `const Type& GetData() const`
2. **Check errors when loading**: Use `std::expected` and check results
3. **Document nesting**: Explain what nested data is included
4. **Limit nesting depth**: Keep to 2-3 levels maximum
5. **Test the complete structure**: Verify all nested data loads correctly

## Migration Strategy

Current codebase can migrate gradually:

**Phase 1** (Analysis only - current):
- ✅ Document pattern and create guides

**Phase 2** (Future):
- Create wrapper structs (e.g., `EngineData`)
- Add unified load methods
- Keep old methods for backward compatibility

**Phase 3** (Future):
- Migrate callers to new pattern
- Test thoroughly

**Phase 4** (Far Future):
- Deprecate and eventually remove old methods

## Questions?

See full documentation for:
- Detailed before/after examples
- Step-by-step implementation guides
- Testing strategies
- Decision points and rationale
