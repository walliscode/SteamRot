# Provider Interface Standardization - Analysis and Proposal

## Executive Summary

This document analyzes the current Provider and Configurator interfaces in the SteamRot codebase and proposes a standardized approach for consistency, maintainability, and testability.

## Current State Analysis

### Existing Provider Interfaces

| Interface | Main Method(s) | Return Type | Pattern |
|-----------|---------------|-------------|---------|
| `IEngineDataProvider` | `LoadEngineData()` | `std::expected<EngineData, FailInfo>` | Load data |
| `ISceneDataProvider` | `ProvideDefaultSceneData()`, `ProvideSceneDataFromData()` | `std::expected<SceneData, FailInfo>` | Provide data |
| `ISaveDataProvider` | `ProvideSaveData()` | `std::expected<SaveData, FailInfo>` | Provide data |
| `IUIStyleDataProvider` | `ProvideUIStyles()` | `std::expected<std::vector<UIStyle>, FailInfo>` | Provide data |
| `ISceneManagerDataProvider` | `ProvideSceneManagerData()` | `std::expected<SceneManagerData, FailInfo>` | Provide data |
| `IFontProvider` | `GetFont()` | `std::expected<std::shared_ptr<const sf::Font>, FailInfo>` | Get resource |

### Existing Configurator Interfaces

| Interface | Main Methods | Purpose |
|-----------|--------------|---------|
| `IEntityConfigurator` | `ConfigureEntityMemoryPool()`, `ConfigureFirstLayerComponents()`, `ConfigureSecondLayerComponents()`, `ConfigureComponent()`, `ConfigureCUserInterface()`, etc. | Configure entity components from data |
| `ISceneConfigurator` | `ConfigureScene()`, `ConfigureSceneInfo()`, `ConfigureSceneResources()`, `ConfigureSceneConfig()` | Configure scene from data |
| `IUIElementConfigurator` | `CreateRootUIElement()` | Create and configure UI elements |
| `IUIStyleConfigurator` | (not yet implemented) | Configure UI styles |

### Key Observations

1. **Naming Inconsistency**: Methods use `Load*`, `Provide*`, or `Get*` interchangeably
2. **Dual Responsibility**: Some concrete implementations (e.g., `FlatbuffersSceneDataProvider`) have both `Provide*` methods (in interface) and `Configure*` methods (not in interface)
3. **Mixed Patterns**: 
   - Providers load/provide data
   - Configurators configure objects from data
   - Some classes do both
4. **No Common Base**: No enforcement of consistent patterns
5. **Private Implementation Details**: Concrete classes have private `Populate*` or `Configure*` methods that contain reusable logic but are buried in class implementations

## Problems with Current Approach

1. **Discoverability**: Hard to know which class/interface to use for a given task
2. **Testability**: Logic buried in private methods is harder to test independently
3. **Reusability**: Configuration logic tied to specific implementations can't be reused
4. **Maintainability**: Inconsistent patterns make the codebase harder to understand
5. **Extensibility**: Adding new data sources requires reimplementing configuration logic

## Proposed Solution

### Core Principles

1. **Single Responsibility**: Separate data loading from configuration logic
2. **Consistency**: All Provider interfaces follow the same pattern
3. **Testability**: Extract reusable logic into free functions
4. **Flexibility**: Support multiple data sources without duplicating logic

### Proposed Interface Pattern

Each `IProvider` interface should follow this pattern:

```cpp
class IDataProvider {
public:
  virtual ~IDataProvider() = default;
  
  // Main interface method - provides configured data
  virtual std::expected<DataType, FailInfo> Provide() const = 0;
};
```

Optional extended pattern with explicit configuration:

```cpp
class IDataProvider {
public:
  virtual ~IDataProvider() = default;
  
  // Configure an existing object from data source
  virtual std::expected<std::monostate, FailInfo> 
  Configure(DataType& data) const = 0;
  
  // Provide a new configured object (calls Configure internally)
  virtual std::expected<DataType, FailInfo> Provide() const = 0;
};
```

### Free Function Pattern

Extract concrete implementation logic into free functions:

```cpp
// In engine_data_config.h
namespace steamrot::config {

/////////////////////////////////////////////////
/// @brief Configure EngineConfig from FlatBuffers data
///
/// @param engine_config Reference to EngineConfig to populate
/// @param fb_data FlatBuffers data source
/// @return std::expected<std::monostate, FailInfo>
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
configure_engine_config(EngineConfig& engine_config, 
                       const EngineConfigFbs* fb_data);

/////////////////////////////////////////////////
/// @brief Configure EngineState from FlatBuffers data
///
/// @param engine_state Reference to EngineState to populate
/// @param fb_data FlatBuffers data source
/// @return std::expected<std::monostate, FailInfo>
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
configure_engine_state(EngineState& engine_state,
                      const EngineStateFbs* fb_data);

} // namespace steamrot::config
```

Then the concrete provider uses these free functions:

```cpp
// In FlatbuffersEngineDataProvider.cpp
std::expected<EngineData, FailInfo> 
FlatbuffersEngineDataProvider::Provide() const {
  // Load raw data
  auto raw_data = m_loader.LoadEngineDataFbs();
  if (!raw_data.has_value()) return std::unexpected(raw_data.error());
  
  EngineData engine_data;
  
  // Use free functions for configuration
  auto config_result = config::configure_engine_config(
    engine_data.config, raw_data.value()->engine_config());
  if (!config_result.has_value()) return std::unexpected(config_result.error());
  
  auto state_result = config::configure_engine_state(
    engine_data.state, raw_data.value()->engine_state());
  if (!state_result.has_value()) return std::unexpected(state_result.error());
  
  return engine_data;
}
```

### Collapsing Configurators

Instead of separate Configurator interfaces, absorb their functionality into Provider patterns:

**Before:**
```cpp
// Separate interfaces
class ISceneDataProvider { ... };
class ISceneConfigurator { ... };

// Usage requires both
FlatbuffersSceneDataProvider provider(event_handler);
auto scene_data = provider.ProvideDefaultSceneData(scene_type);

FlatbuffersSceneConfigurator configurator;
configurator.ConfigureScene(scene, scene_data.value());
```

**After:**
```cpp
// Single provider interface with configuration capability
class ISceneProvider {
public:
  virtual ~ISceneProvider() = default;
  
  // Configure existing scene from data
  virtual std::expected<std::monostate, FailInfo>
  Configure(Scene& scene, const SceneData& scene_data) const = 0;
  
  // Provide new scene data
  virtual std::expected<SceneData, FailInfo>
  Provide(const SceneType scene_type) const = 0;
};

// Usage is simpler
FlatbuffersSceneProvider provider(event_handler);
auto scene_data = provider.Provide(scene_type);
provider.Configure(scene, scene_data.value());
```

### Benefits of This Approach

1. **Testability**: Free functions can be tested independently
2. **Reusability**: Configuration logic can be used by multiple providers
3. **Consistency**: All providers follow the same pattern
4. **Simplicity**: Fewer interfaces, clearer responsibilities
5. **Maintainability**: Changes to configuration logic don't require changing provider implementations
6. **Extensibility**: Adding new data sources only requires implementing provider interface, configuration logic is reused

## Templating Decision Analysis

### Option A: No Templates (Discipline-Based)

**Approach**: Rely on coding conventions and code review

```cpp
// Each interface defines its own return types
class IEngineDataProvider {
public:
  virtual std::expected<EngineData, FailInfo> Provide() const = 0;
};

class ISceneDataProvider {
public:
  virtual std::expected<SceneData, FailInfo> Provide() const = 0;
};
```

**Pros:**
- Simple, no template complexity
- No template compilation overhead
- Easy to understand for all skill levels
- Flexible - each provider can have unique method signatures

**Cons:**
- No compile-time enforcement of pattern
- Easy to deviate from conventions
- Manual checking required

**Recommendation**: ⭐ **Best choice** - The pattern is simple enough that templates add more complexity than value.

### Option B: Simple Template Helpers

**Approach**: Template helpers for common patterns

```cpp
// Template helper for standard pattern
template<typename DataType>
class IProvider {
public:
  virtual ~IProvider() = default;
  virtual std::expected<DataType, FailInfo> Provide() const = 0;
};

// Usage
class IEngineDataProvider : public IProvider<EngineData> {};
class ISceneDataProvider : public IProvider<SceneData> {};
```

**Pros:**
- Some compile-time checking
- Clear inheritance relationship
- Enforces return type consistency

**Cons:**
- Doesn't help with method naming (`Configure` vs `Provide`)
- Limited flexibility for providers needing different signatures
- Adds template complexity for minimal benefit

**Recommendation**: ⚠️ Possible but not necessary - Only if we want explicit type relationships.

### Option C: CRTP (Curiously Recurring Template Pattern)

**Approach**: Compile-time polymorphism

```cpp
template<typename Derived, typename DataType>
class IProviderCRTP {
public:
  std::expected<DataType, FailInfo> Provide() const {
    return static_cast<const Derived*>(this)->ProvideImpl();
  }
  
protected:
  ~IProviderCRTP() = default;
};

class FlatbuffersEngineDataProvider 
  : public IProviderCRTP<FlatbuffersEngineDataProvider, EngineData> {
public:
  std::expected<EngineData, FailInfo> ProvideImpl() const {
    // Implementation
  }
};
```

**Pros:**
- No virtual function overhead
- Compile-time polymorphism
- Can enforce additional compile-time checks

**Cons:**
- Significantly more complex
- Harder to understand and maintain
- Overkill for this use case
- Loses runtime polymorphism (interface pointers)

**Recommendation**: ❌ **Not recommended** - Too complex for the benefit provided.

### Final Recommendation

**Use Option A: No Templates**

Reasoning:
1. The pattern is simple and clear without templates
2. Each provider has specific needs (parameters, return types) that templates would restrict
3. The codebase already has good discipline with std::expected pattern
4. Code reviews can catch deviations
5. Keep it simple - YAGNI (You Aren't Gonna Need It)

Document the pattern clearly in this guide and rely on:
- Clear naming conventions
- Code review
- Documentation
- Examples

## Implementation Naming Conventions

### Interface Names
- Use `I<Domain>Provider` pattern
- Examples: `IEngineDataProvider`, `ISceneProvider`, `IEntityProvider`

### Method Names
- **Primary Method**: `Provide()` - returns configured data object
- **Configuration Method**: `Configure()` - configures existing object
- **Specialized Methods**: Can exist but should be clearly named
  - `ProvideForScene()`, `ProvideDefault()`, etc.

### Free Function Names
- Use snake_case
- Pattern: `configure_<domain>_<aspect>()`
- Examples:
  - `configure_engine_config()`
  - `configure_scene_info()`
  - `configure_ui_element()`

### File Organization
- **Interfaces**: `src/types/interfaces/I<Domain>Provider.h`
- **Concrete Providers**: `src/<domain>/Flatbuffers<Domain>Provider.h/cpp`
- **Free Functions**: `src/<domain>/<domain>_config.h/cpp` or `src/<domain>/config/<aspect>_config.h/cpp`

**Important**: Free functions should be placed in the same CMake library as the provider to avoid circular dependencies in the build system. See PROVIDER_IMPLEMENTATION_GUIDE.md, "Layering and CMake Dependencies" section for detailed guidance on respecting the dependency hierarchy.

## Migration Strategy

### Phase 1: Documentation and Analysis ✓
- [x] Document current state
- [x] Analyze patterns and problems
- [x] Propose solution
- [x] Make templating decision

### Phase 2: Extract Free Functions
For each Provider/Configurator:
1. Identify reusable configuration logic
2. Create free function header file
3. Move logic from private methods to free functions
4. Update concrete implementation to use free functions
5. Add unit tests for free functions
6. Verify existing tests still pass

**Priority Order:**
1. `EngineDataProvider` (simplest, good starting point)
2. `SceneDataProvider` (moderate complexity)
3. `EntityConfigurator` (most complex)
4. Remaining providers

### Phase 3: Rename for Consistency
1. Standardize method names to `Provide()`
2. Update all call sites
3. Update tests

### Phase 4: Collapse Configurators
1. Merge Configurator methods into Provider interfaces
2. Update concrete implementations
3. Remove old Configurator interfaces
4. Update all usage sites

### Phase 5: Documentation and Examples
1. Update README with new patterns
2. Create example code
3. Update architecture documentation

## Migration Example: EngineDataProvider

### Current State

```cpp
// Interface
class IEngineDataProvider {
public:
  virtual std::expected<EngineData, FailInfo> LoadEngineData() const = 0;
};

// Implementation
class FlatbuffersEngineDataProvider : public IEngineDataProvider {
private:
  std::expected<std::monostate, FailInfo> 
  PopulateEngineConfig(EngineConfig& config, const EngineConfigFbs* data) const {
    // Configuration logic here
  }
  
public:
  std::expected<EngineData, FailInfo> LoadEngineData() const override {
    // Load data
    // Call PopulateEngineConfig
    // Return result
  }
};
```

### After Migration

```cpp
// In src/types/interfaces/IEngineDataProvider.h
class IEngineDataProvider {
public:
  virtual ~IEngineDataProvider() = default;
  virtual std::expected<EngineData, FailInfo> Provide() const = 0;
};

// In src/data_providers/engine_data_config.h
namespace steamrot::config {

std::expected<std::monostate, FailInfo>
configure_engine_config(EngineConfig& config, const EngineConfigFbs* fb_data);

std::expected<std::monostate, FailInfo>
configure_engine_state(EngineState& state, const EngineStateFbs* fb_data);

} // namespace steamrot::config

// In src/data_providers/engine_data_config.cpp
namespace steamrot::config {

std::expected<std::monostate, FailInfo>
configure_engine_config(EngineConfig& config, const EngineConfigFbs* fb_data) {
  if (!fb_data) {
    return std::unexpected(
      FailInfo{FailMode::FlatbuffersDataNotFound, "EngineConfigFbs is null"});
  }
  
  // Configuration logic (extracted from class)
  config.display.window_title = fb_data->display()->window_title()->str();
  config.display.framerate_limit = fb_data->display()->framerate_limit();
  
  return std::monostate{};
}

// ... other free functions ...

} // namespace steamrot::config

// In src/data_providers/FlatbuffersEngineDataProvider.h
class FlatbuffersEngineDataProvider : public IEngineDataProvider {
private:
  FlatbuffersDataLoader m_loader;
  
public:
  std::expected<EngineData, FailInfo> Provide() const override;
};

// In src/data_providers/FlatbuffersEngineDataProvider.cpp
std::expected<EngineData, FailInfo> 
FlatbuffersEngineDataProvider::Provide() const {
  // Load raw FlatBuffers data
  auto fb_data_result = m_loader.LoadEngineDataFbs();
  if (!fb_data_result.has_value()) 
    return std::unexpected(fb_data_result.error());
  
  const auto* fb_data = fb_data_result.value();
  EngineData engine_data;
  
  // Use free functions for configuration
  auto config_result = config::configure_engine_config(
    engine_data.config, fb_data->engine_config());
  if (!config_result.has_value()) 
    return std::unexpected(config_result.error());
  
  auto state_result = config::configure_engine_state(
    engine_data.state, fb_data->engine_state());
  if (!state_result.has_value()) 
    return std::unexpected(state_result.error());
  
  // ... other configurations ...
  
  return engine_data;
}
```

### Test Example

```cpp
// tests/unit/data_providers/engine_data_config.test.cpp
#include "engine_data_config.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("configure_engine_config populates display settings", "[unit][config]") {
  // Create test FlatBuffers data
  // ...
  
  steamrot::EngineConfig config;
  auto result = steamrot::config::configure_engine_config(config, fb_data);
  
  REQUIRE(result.has_value());
  REQUIRE(config.display.window_title == "Test Window");
  REQUIRE(config.display.framerate_limit == 60);
}

TEST_CASE("configure_engine_config handles null data", "[unit][config]") {
  steamrot::EngineConfig config;
  auto result = steamrot::config::configure_engine_config(config, nullptr);
  
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}
```

## Questions for Discussion

1. **Templating**: Agree with no templates approach?
2. **Naming**: Should all methods be `Provide()` or allow variations like `ProvideDefault()`?
3. **Free Functions**: Should we use namespace `steamrot::config` or `steamrot::<domain>::config`?
4. **Migration Priority**: Start with EngineDataProvider?
5. **Breaking Changes**: This will require updating all call sites. Acceptable?

## Conclusion

This proposal provides a clear path to standardizing Provider and Configurator interfaces throughout the SteamRot codebase. The approach:

1. ✅ Creates consistent, predictable patterns
2. ✅ Improves testability through free functions
3. ✅ Maintains simplicity by avoiding templates
4. ✅ Enhances reusability of configuration logic
5. ✅ Provides clear migration path

The key insight is that **simplicity and discipline are better than complex template enforcement** for this use case. Clear documentation, good examples, and code review will maintain consistency without adding template complexity.

## Next Steps

1. Review and approve this proposal
2. Create tracking issue for migration
3. Begin Phase 2: Extract free functions (starting with EngineDataProvider)
4. Update documentation as we go
5. Validate with tests throughout migration
