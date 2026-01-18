# Provider Implementation Guide

## Quick Reference

This guide provides practical examples for implementing the standardized Provider pattern.

## Creating a New Provider

### Step 1: Define the Interface

```cpp
// src/types/interfaces/IMyDataProvider.h

#pragma once

#include "MyData.h"
#include "FailInfo.h"
#include <expected>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Interface for providing MyData
/////////////////////////////////////////////////
class IMyDataProvider {
public:
  /////////////////////////////////////////////////
  /// @brief Virtual destructor
  /////////////////////////////////////////////////
  virtual ~IMyDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Provide configured MyData object
  ///
  /// @return MyData object or error
  /////////////////////////////////////////////////
  virtual std::expected<MyData, FailInfo> Provide() const = 0;
};

} // namespace steamrot
```

### Step 2: Create Configuration Free Functions

```cpp
// src/my_domain/my_data_config.h

#pragma once

#include "MyData.h"
#include "MySubConfig.h"
#include "FailInfo.h"
#include "my_data_generated.h"
#include <expected>

namespace steamrot::config {

/////////////////////////////////////////////////
/// @brief Configure MyData from FlatBuffers source
///
/// @param my_data Reference to MyData object to configure
/// @param fb_data FlatBuffers data source
/// @return Success or error information
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
configure_my_data(MyData& my_data, const MyDataFbs* fb_data);

/////////////////////////////////////////////////
/// @brief Configure MySubConfig from FlatBuffers source
///
/// @param sub_config Reference to MySubConfig to configure
/// @param fb_config FlatBuffers config source
/// @return Success or error information
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
configure_my_sub_config(MySubConfig& sub_config, 
                        const MySubConfigFbs* fb_config);

} // namespace steamrot::config
```

```cpp
// src/my_domain/my_data_config.cpp

#include "my_data_config.h"

namespace steamrot::config {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
configure_my_data(MyData& my_data, const MyDataFbs* fb_data) {
  // Null check
  if (!fb_data) {
    return std::unexpected(
      FailInfo{FailMode::FlatbuffersDataNotFound,
               "MyDataFbs is null"});
  }

  // Configure required fields
  if (!fb_data->name()) {
    return std::unexpected(
      FailInfo{FailMode::FlatbuffersDataNotFound,
               "MyDataFbs missing required 'name' field"});
  }
  my_data.name = fb_data->name()->str();

  // Configure optional fields
  if (fb_data->description()) {
    my_data.description = fb_data->description()->str();
  }

  // Configure primitive fields (no null check needed)
  my_data.value = fb_data->value();
  my_data.enabled = fb_data->enabled();

  // Configure nested sub-config if present
  if (fb_data->sub_config()) {
    auto result = configure_my_sub_config(
      my_data.sub_config, fb_data->sub_config());
    if (!result.has_value()) 
      return std::unexpected(result.error());
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
configure_my_sub_config(MySubConfig& sub_config,
                        const MySubConfigFbs* fb_config) {
  if (!fb_config) {
    return std::unexpected(
      FailInfo{FailMode::FlatbuffersDataNotFound,
               "MySubConfigFbs is null"});
  }

  // Configure fields
  sub_config.setting_a = fb_config->setting_a();
  sub_config.setting_b = fb_config->setting_b();

  return std::monostate{};
}

} // namespace steamrot::config
```

### Step 3: Implement Concrete Provider

```cpp
// src/data_providers/FlatbuffersMyDataProvider.h

#pragma once

#include "IMyDataProvider.h"
#include "FlatbuffersDataLoader.h"
#include "my_data_generated.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @brief FlatBuffers implementation of IMyDataProvider
/////////////////////////////////////////////////
class FlatbuffersMyDataProvider : public IMyDataProvider {
private:
  /////////////////////////////////////////////////
  /// @brief Data loader for FlatBuffers files
  /////////////////////////////////////////////////
  FlatbuffersDataLoader m_loader;

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor
  /////////////////////////////////////////////////
  FlatbuffersMyDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Provide MyData loaded from FlatBuffers
  ///
  /// @return Configured MyData or error
  /////////////////////////////////////////////////
  std::expected<MyData, FailInfo> Provide() const override;
};

} // namespace steamrot
```

```cpp
// src/data_providers/FlatbuffersMyDataProvider.cpp

#include "FlatbuffersMyDataProvider.h"
#include "my_data_config.h"

namespace steamrot {

/////////////////////////////////////////////////
std::expected<MyData, FailInfo> 
FlatbuffersMyDataProvider::Provide() const {
  // Load raw FlatBuffers data
  auto fb_result = m_loader.LoadMyDataFbs();
  if (!fb_result.has_value()) {
    return std::unexpected(fb_result.error());
  }

  const MyDataFbs* fb_data = fb_result.value();
  MyData my_data;

  // Use free function for configuration
  auto config_result = config::configure_my_data(my_data, fb_data);
  if (!config_result.has_value()) {
    return std::unexpected(config_result.error());
  }

  return my_data;
}

} // namespace steamrot
```

### Step 4: Add Tests

```cpp
// tests/unit/my_domain/my_data_config.test.cpp

#include "my_data_config.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("configure_my_data populates all fields", "[unit][config]") {
  // Create test FlatBuffers data
  flatbuffers::FlatBufferBuilder builder;
  auto name = builder.CreateString("test_name");
  auto desc = builder.CreateString("test_description");
  
  auto fb_data = CreateMyDataFbs(builder, name, desc, 42, true);
  builder.Finish(fb_data);
  
  // Get pointer to data
  const auto* data = flatbuffers::GetRoot<MyDataFbs>(
    builder.GetBufferPointer());

  // Configure
  steamrot::MyData my_data;
  auto result = steamrot::config::configure_my_data(my_data, data);

  // Verify
  REQUIRE(result.has_value());
  REQUIRE(my_data.name == "test_name");
  REQUIRE(my_data.description == "test_description");
  REQUIRE(my_data.value == 42);
  REQUIRE(my_data.enabled == true);
}

TEST_CASE("configure_my_data handles null data", "[unit][config]") {
  steamrot::MyData my_data;
  auto result = steamrot::config::configure_my_data(my_data, nullptr);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("configure_my_data handles missing required field", "[unit][config]") {
  // Create FlatBuffers data without required 'name' field
  flatbuffers::FlatBufferBuilder builder;
  auto fb_data = CreateMyDataFbs(builder, 0, 0, 42, true);
  builder.Finish(fb_data);
  
  const auto* data = flatbuffers::GetRoot<MyDataFbs>(
    builder.GetBufferPointer());

  steamrot::MyData my_data;
  auto result = steamrot::config::configure_my_data(my_data, data);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}
```

```cpp
// tests/unit/data_providers/FlatbuffersMyDataProvider.test.cpp

#include "FlatbuffersMyDataProvider.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FlatbuffersMyDataProvider::Provide loads data", 
          "[unit][FlatbuffersMyDataProvider]") {
  steamrot::FlatbuffersMyDataProvider provider;
  auto result = provider.Provide();

  REQUIRE(result.has_value());
  // Add assertions based on test data file
}
```

## Provider with Dependencies

When a provider needs dependencies (like EventHandler):

```cpp
// src/types/interfaces/IMyDataProvider.h
class IMyDataProvider {
protected:
  EventHandler& m_event_handler;

public:
  IMyDataProvider(EventHandler& event_handler) 
    : m_event_handler(event_handler) {}
    
  virtual ~IMyDataProvider() = default;
  virtual std::expected<MyData, FailInfo> Provide() const = 0;
};

// src/data_providers/FlatbuffersMyDataProvider.h
class FlatbuffersMyDataProvider : public IMyDataProvider {
private:
  FlatbuffersDataLoader m_loader;

public:
  FlatbuffersMyDataProvider(EventHandler& event_handler)
    : IMyDataProvider(event_handler) {}

  std::expected<MyData, FailInfo> Provide() const override;
};
```

## Provider with Configure Method

For providers that also support configuring existing objects:

```cpp
// src/types/interfaces/IMyDataProvider.h
class IMyDataProvider {
public:
  virtual ~IMyDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Configure existing MyData object from data source
  ///
  /// @param my_data Reference to object to configure
  /// @return Success or error
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  Configure(MyData& my_data) const = 0;

  /////////////////////////////////////////////////
  /// @brief Provide new configured MyData object
  ///
  /// Internally creates new object and calls Configure()
  /// @return Configured object or error
  /////////////////////////////////////////////////
  virtual std::expected<MyData, FailInfo> Provide() const = 0;
};

// Implementation
std::expected<MyData, FailInfo> 
FlatbuffersMyDataProvider::Provide() const {
  MyData my_data;
  auto result = Configure(my_data);
  if (!result.has_value()) {
    return std::unexpected(result.error());
  }
  return my_data;
}

std::expected<std::monostate, FailInfo>
FlatbuffersMyDataProvider::Configure(MyData& my_data) const {
  auto fb_result = m_loader.LoadMyDataFbs();
  if (!fb_result.has_value()) {
    return std::unexpected(fb_result.error());
  }
  
  return config::configure_my_data(my_data, fb_result.value());
}
```

## Provider with Multiple Provide Methods

For providers that need different ways to provide data:

```cpp
class ISceneDataProvider {
public:
  virtual ~ISceneDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Provide default scene data for given scene type
  /////////////////////////////////////////////////
  virtual std::expected<SceneData, FailInfo>
  ProvideDefault(const SceneType scene_type) const = 0;

  /////////////////////////////////////////////////
  /// @brief Provide scene data from existing FlatBuffers data
  /////////////////////////////////////////////////
  virtual std::expected<SceneData, FailInfo>
  ProvideFromData(const SceneDataFbs* fb_data) const = 0;
};
```

## Organizing Free Functions

### Option 1: Single Config File (Simple Projects)

```
src/my_domain/
  ├── my_data_config.h
  └── my_data_config.cpp
```

All configuration functions in one file.

### Option 2: Multiple Config Files (Complex Projects)

```
src/my_domain/config/
  ├── main_config.h
  ├── main_config.cpp
  ├── sub_config_a.h
  ├── sub_config_a.cpp
  ├── sub_config_b.h
  └── sub_config_b.cpp
```

Separate files for different aspects of configuration.

### Option 3: Grouped by Data Source

```
src/my_domain/config/
  ├── flatbuffers_config.h    # FlatBuffers-specific
  ├── flatbuffers_config.cpp
  ├── json_config.h           # JSON-specific
  └── json_config.cpp
```

Separate implementations for different data sources.

## Layering and CMake Dependencies

### Understanding the Dependency Hierarchy

The SteamRot codebase follows a layered architecture enforced through CMake library dependencies. When placing free configuration functions, you must respect these layers to avoid circular dependencies.

**Current Dependency Hierarchy** (from foundation to high-level):

```
Layer 0: External Dependencies (SFML, flatbuffers, spdlog, etc.)
         ↓
Layer 1: logger
         ↓
Layer 2: types (interface library - headers only)
         ↓
Layer 3: events
         ↓
Layer 4: entity, user_interface (parallel - depend on types + events)
         ↓
Layer 5: data_providers (depends on types + events + entity)
         ↓
Layer 6: assets (depends on data_providers + user_interface + types)
         ↓
Layer 7: resources, logic, context
         ↓
Layer 8: scenes, engine
```

**Key Rule**: A library can only depend on libraries in lower layers. Free functions must be placed where they don't violate this rule.

### Where to Place Free Configuration Functions

#### Option A: In the Same Library as the Provider (Recommended)

Place configuration free functions in the same library as the concrete provider implementation.

**Example**: For `FlatbuffersEngineDataProvider` (in `data_providers` library):
```
src/data_providers/
  ├── FlatbuffersEngineDataProvider.h
  ├── FlatbuffersEngineDataProvider.cpp
  ├── engine_data_config.h          ← Free functions here
  └── engine_data_config.cpp
```

**Advantages**:
- No additional CMake complexity
- Free functions can use same dependencies as provider
- Natural grouping with provider implementation

**When to use**: Default choice for most providers

#### Option B: In a Lower-Layer Library (For Shared Logic)

If configuration logic needs to be shared across multiple layers, place it in a lower layer that all consumers can depend on.

**Example**: Configuration logic shared between `entity` and `data_providers`:
```
src/entity/
  ├── entity_config.h               ← Free functions here
  └── entity_config.cpp

src/data_providers/
  └── FlatbuffersEntityProvider.cpp  ← Uses entity_config
```

**When to use**: When multiple libraries at different layers need the same configuration logic

#### Option C: In the `types` Library (For Very Low-Level Config)

For configuration functions that only depend on type definitions (no SFML, no EventHandler), consider adding them to `types`.

**Example**: Simple data structure configuration:
```
src/types/core/
  ├── EngineData.h
  ├── engine_data_config.h          ← Free functions here
  └── engine_data_config.cpp
```

**Requirements**:
- Must not depend on SFML types
- Must not depend on EventHandler or other high-level types
- Must be pure data transformation

**When to use**: Rarely - only for pure data structure manipulation

### Circular Dependency Issues

**Problem**: If you place free functions in the wrong layer, you can create circular dependencies.

**Example of WRONG placement**:
```
❌ BAD: user_interface_config.h in data_providers/
   - data_providers depends on user_interface
   - If data_providers contains user_interface_config
   - And user_interface wants to use those configs
   - Circular dependency!
```

**Solution**: Place `user_interface_config.h` in `user_interface/` library:
```
✅ GOOD: user_interface_config.h in user_interface/
   - user_interface owns its config functions
   - data_providers can use them (depends on user_interface anyway)
   - No circular dependency
```

### CMake Integration

When adding free function files to a library, update its `CMakeLists.txt`:

```cmake
# In src/data_providers/CMakeLists.txt
add_library(data_providers
  FlatbuffersEngineDataProvider.cpp
  FlatbuffersSceneDataProvider.cpp
  engine_data_config.cpp              # Add this
  scene_data_config.cpp               # Add this
  # ... other files
)
```

### Guidelines by Domain

| Domain | Config Functions Location | Rationale |
|--------|--------------------------|-----------|
| **EngineData** | `src/data_providers/engine_data_config.h/cpp` | Provider owns the config logic |
| **SceneData** | `src/data_providers/scene_data_config.h/cpp` | Provider owns the config logic |
| **SaveData** | `src/data_providers/save_data_config.h/cpp` | Provider owns the config logic |
| **UIStyleData** | `src/data_providers/ui_style_data_config.h/cpp` | Provider owns the config logic |
| **EntityData** | `src/entity/entity_config.h/cpp` | Shared by entity and data_providers |
| **UIElementData** | `src/user_interface/ui_element_config.h/cpp` | Owned by user_interface |
| **SceneManagerData** | `src/data_providers/scene_manager_data_config.h/cpp` | Provider owns the config logic |

### Testing Layering

To verify you haven't created circular dependencies:

```bash
# Build the project - CMake will fail if circular dependencies exist
cmake --preset Debug
cmake --build --preset Debug

# If you see errors like:
# "cycle in constraint graph" or
# "circular dependency between targets"
# You've placed free functions in the wrong layer!
```

### Quick Decision Tree

```
Where should I put configure_X() free functions?

1. Does the config logic depend on EventHandler, AssetManager, 
   or other high-level components?
   YES → Put in same library as provider (data_providers, entity, etc.)
   NO → Continue to 2

2. Is the config logic needed by multiple libraries at different layers?
   YES → Put in lowest layer that has all dependencies
   NO → Continue to 3

3. Does the config only manipulate plain data structures?
   YES → Consider putting in types/core/ or types/components/
   NO → Put in same library as provider (default)
```

### Migration Consideration

When migrating existing providers, **always place free functions in the same library as the provider first**. This is the safest approach and matches current dependency structure. You can refactor to a lower layer later if sharing becomes necessary.

## Migration Checklist

When migrating an existing Provider/Configurator:

- [ ] **Identify reusable logic**
  - [ ] Find private `Populate*` or `Configure*` methods
  - [ ] Find logic that could be reused by other data sources

- [ ] **Create free function files**
  - [ ] Create `<domain>_config.h`
  - [ ] Create `<domain>_config.cpp`
  - [ ] Place in appropriate directory

- [ ] **Extract free functions**
  - [ ] Move configuration logic to free functions
  - [ ] Make functions testable (no hidden dependencies)
  - [ ] Use consistent naming: `configure_<aspect>()`

- [ ] **Update concrete provider**
  - [ ] Remove private configuration methods
  - [ ] Call free functions instead
  - [ ] Simplify Provide() implementation

- [ ] **Add tests for free functions**
  - [ ] Test with valid data
  - [ ] Test with null data
  - [ ] Test with missing required fields
  - [ ] Test with edge cases

- [ ] **Update existing tests**
  - [ ] Verify provider tests still pass
  - [ ] Update any tests that relied on private methods

- [ ] **Update documentation**
  - [ ] Update interface documentation
  - [ ] Add examples to guide
  - [ ] Update architecture docs

## Common Patterns

### Pattern: Required vs Optional Fields

```cpp
std::expected<std::monostate, FailInfo>
configure_my_data(MyData& data, const MyDataFbs* fb_data) {
  if (!fb_data) {
    return std::unexpected(
      FailInfo{FailMode::FlatbuffersDataNotFound, "Data is null"});
  }

  // Required field - return error if missing
  if (!fb_data->required_field()) {
    return std::unexpected(
      FailInfo{FailMode::FlatbuffersDataNotFound,
               "Missing required field 'required_field'"});
  }
  data.required_field = fb_data->required_field()->str();

  // Optional field - only set if present
  if (fb_data->optional_field()) {
    data.optional_field = fb_data->optional_field()->str();
  }

  // Primitive field - always accessible
  data.count = fb_data->count();

  return std::monostate{};
}
```

### Pattern: Nested Configuration

```cpp
std::expected<std::monostate, FailInfo>
configure_parent(Parent& parent, const ParentFbs* fb_parent) {
  if (!fb_parent) return std::unexpected(/*...*/);

  // Configure parent fields
  parent.name = fb_parent->name()->str();

  // Configure nested child
  if (fb_parent->child()) {
    auto result = configure_child(parent.child, fb_parent->child());
    if (!result.has_value()) {
      return std::unexpected(result.error());
    }
  }

  return std::monostate{};
}
```

### Pattern: Collection Configuration

```cpp
std::expected<std::monostate, FailInfo>
configure_collection(MyData& data, const MyDataFbs* fb_data) {
  if (!fb_data) return std::unexpected(/*...*/);

  // Configure vector of items
  if (fb_data->items()) {
    data.items.reserve(fb_data->items()->size());
    
    for (const auto* fb_item : *fb_data->items()) {
      Item item;
      auto result = configure_item(item, fb_item);
      if (!result.has_value()) {
        return std::unexpected(result.error());
      }
      data.items.push_back(std::move(item));
    }
  }

  return std::monostate{};
}
```

### Pattern: Conditional Configuration

```cpp
std::expected<std::monostate, FailInfo>
configure_with_mode(MyData& data, const MyDataFbs* fb_data) {
  if (!fb_data) return std::unexpected(/*...*/);

  // Configure based on mode
  switch (fb_data->mode()) {
    case ModeFbs_Simple: {
      auto result = configure_simple_mode(data, fb_data->simple_config());
      if (!result.has_value()) return std::unexpected(result.error());
      break;
    }
    case ModeFbs_Advanced: {
      auto result = configure_advanced_mode(data, fb_data->advanced_config());
      if (!result.has_value()) return std::unexpected(result.error());
      break;
    }
    default:
      return std::unexpected(
        FailInfo{FailMode::InvalidEnumValue, "Unknown mode"});
  }

  return std::monostate{};
}
```

## Best Practices

### 1. Always Check for Null

FlatBuffers fields can be null. Always check before dereferencing:

```cpp
// ❌ BAD - Can segfault
data.name = fb_data->name()->str();

// ✅ GOOD - Safe
if (fb_data->name()) {
  data.name = fb_data->name()->str();
}
```

### 2. Provide Clear Error Messages

```cpp
// ❌ BAD - Generic message
return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound, "Error"});

// ✅ GOOD - Specific message
return std::unexpected(
  FailInfo{FailMode::FlatbuffersDataNotFound,
           "MyDataFbs missing required 'name' field"});
```

### 3. Keep Free Functions Pure

Free functions should not have hidden dependencies:

```cpp
// ❌ BAD - Hidden global dependency
std::expected<std::monostate, FailInfo>
configure_data(MyData& data, const MyDataFbs* fb_data) {
  data.id = GlobalIDGenerator::GetNext(); // Hidden dependency!
  // ...
}

// ✅ GOOD - All dependencies explicit
std::expected<std::monostate, FailInfo>
configure_data(MyData& data, const MyDataFbs* fb_data, 
               IDGenerator& id_gen) {
  data.id = id_gen.GetNext();
  // ...
}
```

### 4. Test Free Functions Independently

Free functions should be testable without provider:

```cpp
TEST_CASE("configure_data works with minimal input", "[unit][config]") {
  // Create minimal FlatBuffers data
  // Call free function directly
  // Verify output
}
```

### 5. Document Preconditions

```cpp
/////////////////////////////////////////////////
/// @brief Configure MyData from FlatBuffers source
///
/// @param my_data Reference to MyData object to configure
/// @param fb_data FlatBuffers data source (must not be null)
/// @return Success or error
///
/// @pre my_data should be default-constructed
/// @pre fb_data must contain 'name' and 'value' fields
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
configure_my_data(MyData& my_data, const MyDataFbs* fb_data);
```

## Summary

The standardized Provider pattern:

1. **Interfaces** define `Provide()` (and optionally `Configure()`)
2. **Free functions** contain reusable configuration logic
3. **Concrete providers** load data and call free functions
4. **Tests** cover both free functions and providers

Benefits:
- ✅ Consistency across codebase
- ✅ Testability of configuration logic
- ✅ Reusability across data sources
- ✅ Simplicity without template complexity

Follow this guide when creating new providers or migrating existing ones.
