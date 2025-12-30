# Workflow: Applying FlatBuffers Configuration Data

**Purpose:** Guide for applying configuration data from FlatBuffers to runtime objects  
**Audience:** Developers implementing configurators or initialization code  
**Last Updated:** 2025-12-30

---

## Overview

This workflow explains how to load and apply configuration data from FlatBuffers JSON/binary files to runtime C++ objects in the SteamRot engine.

**Common Use Cases:**
- Initializing SFML resources (windows, textures, sprites)
- Applying scene-specific settings (entity pool sizes, dimensions)
- Configuring engine-level settings (window size, framerate)

---

## Pattern: Configuration Application

### Standard Flow

```
JSON File → FlatBuffers Binary → Load via DataLoader → Apply to Runtime Object
```

### Step-by-Step Process

1. **Define FlatBuffers Schema** (`.fbs` file)
2. **Create JSON Configuration** (data files)
3. **Build Project** (generates headers and compiles binary)
4. **Load Data** (via DataLoader)
5. **Apply Configuration** (in Configurator)

---

## Example: Render Texture Initialization

This example shows how to apply render texture dimensions from configuration data.

### Step 1: Schema Already Exists

**File:** `src/types/flatbuffers/configuration/context_data.fbs`

```fbs
table SceneContextConfig {
  scene_type: SceneType;
  entity_pool_size: uint32 = 100;
  render_texture_width: uint32 = 800;
  render_texture_height: uint32 = 600;
}

table ContextData {
  game_context: GameContextConfig;
  scene_contexts: [SceneContextConfig];
}
```

### Step 2: JSON Configuration Exists

**File:** `data/defaults/context/context_data.json`

```json
{
  "scene_contexts": [
    {
      "scene_type": "TITLE",
      "entity_pool_size": 50,
      "render_texture_width": 800,
      "render_texture_height": 600
    }
  ]
}
```

### Step 3: Add DataLoader Method

**File:** `src/data_providers/FlatbuffersDataLoader.h`

```cpp
class FlatbuffersDataLoader : public DataLoader {
public:
  /////////////////////////////////////////////////
  /// @brief Provides ContextData from binary file
  /////////////////////////////////////////////////
  std::expected<const ContextData *, FailInfo> ProvideContextData() const;
};
```

**File:** `src/data_providers/FlatbuffersDataLoader.cpp`

```cpp
std::expected<const ContextData *, FailInfo>
FlatbuffersDataLoader::ProvideContextData() const {
  // Get file path based on environment and data category
  auto file_path = GetFilePathFromEnvironmentAndCategory(
      DataCategory::Context, "context_data");

  // Load binary file into buffer
  auto buffer_result = LoadBinaryFile(file_path);
  if (!buffer_result.has_value())
    return std::unexpected(buffer_result.error());

  // Parse FlatBuffers data from buffer
  const ContextData *context_data =
      GetContextData(buffer_result.value().data());

  // Validate data was loaded
  if (!context_data)
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "ContextData could not be loaded from binary file"});

  return context_data;
}
```

### Step 4: Apply in Configurator

**File:** `src/scenes/FlatbuffersSceneConfigurator.cpp`

```cpp
std::expected<std::monostate, FailInfo>
FlatbuffersSceneConfigurator::ConfigureSceneResources(
    Scene &scene, const SceneData *scene_data) {

  // Validate input
  if (!scene_data)
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "SceneData pointer is null"));

  // Load context data
  FlatbuffersDataLoader loader;
  auto context_data_result = loader.ProvideContextData();
  if (!context_data_result.has_value())
    return std::unexpected(context_data_result.error());

  const ContextData *context_data = context_data_result.value();

  // Find matching configuration for this scene type
  const SceneContextConfig *scene_config = nullptr;
  SceneType target_type = scene.GetSceneInfo().type;

  if (context_data->scene_contexts()) {
    for (const auto *config : *context_data->scene_contexts()) {
      if (config && config->scene_type() == target_type) {
        scene_config = config;
        break;
      }
    }
  }

  // Validate configuration was found
  if (!scene_config) {
    return std::unexpected(
        FailInfo{FailMode::ConfigurationNotFound,
                 "SceneContextConfig not found for scene type"});
  }

  // Apply configuration to runtime object
  unsigned int width = scene_config->render_texture_width();
  unsigned int height = scene_config->render_texture_height();

  if (!scene.m_scene_resources.scene_texture.create(width, height)) {
    return std::unexpected(
        FailInfo{FailMode::ResourceInitializationFailed,
                 "Failed to create render texture"});
  }

  return std::monostate{};
}
```

### Step 5: Add Required Includes

```cpp
#include "FlatbuffersDataLoader.h"
#include "context_data_generated.h"
```

---

## Key Patterns

### Pattern 1: Loading Data from File

```cpp
// 1. Get file path
auto file_path = GetFilePathFromEnvironmentAndCategory(
    DataCategory::Context, "context_data");

// 2. Load binary file
auto buffer_result = LoadBinaryFile(file_path);
if (!buffer_result.has_value())
    return std::unexpected(buffer_result.error());

// 3. Parse FlatBuffers
const ContextData *data = GetContextData(buffer_result.value().data());

// 4. Validate
if (!data)
    return std::unexpected(FailInfo{...});
```

### Pattern 2: Finding Matching Configuration

```cpp
// Always null-check FlatBuffers vectors/tables
if (context_data->scene_contexts()) {
    for (const auto *config : *context_data->scene_contexts()) {
        // Always null-check individual elements
        if (config && config->scene_type() == target_type) {
            // Found match
            break;
        }
    }
}
```

### Pattern 3: Applying Scalar Values

```cpp
// FlatBuffers scalar types can be accessed directly
uint32_t width = config->render_texture_width();
uint32_t height = config->render_texture_height();

// Use in initialization
texture.create(width, height);
```

### Pattern 4: Applying Optional String Values

```cpp
// Strings must be null-checked
if (config->window_title()) {
    std::string title = config->window_title()->str();
    // Use title
}
```

---

## Common Mistakes to Avoid

### ❌ Don't: Skip Null Checks

```cpp
// WRONG - can segfault
for (const auto *config : *context_data->scene_contexts()) {
    uint32_t value = config->some_field();  // Crash if config is null!
}
```

### ✅ Do: Always Null Check

```cpp
// CORRECT
if (context_data->scene_contexts()) {
    for (const auto *config : *context_data->scene_contexts()) {
        if (config) {
            uint32_t value = config->some_field();
        }
    }
}
```

### ❌ Don't: Access Strings Without Check

```cpp
// WRONG - can segfault
std::string name = config->name()->str();  // Crash if name() returns null!
```

### ✅ Do: Check String Existence

```cpp
// CORRECT
if (config->name()) {
    std::string name = config->name()->str();
}
```

### ❌ Don't: Ignore Load Failures

```cpp
// WRONG - silently fail
auto data = loader.ProvideContextData();
const ContextData *context = data.value();  // Exception if has no value!
```

### ✅ Do: Propagate Errors

```cpp
// CORRECT
auto data_result = loader.ProvideContextData();
if (!data_result.has_value())
    return std::unexpected(data_result.error());

const ContextData *context = data_result.value();
```

---

## Testing Your Configuration

### Unit Test Template

```cpp
TEST_CASE("ConfigureSceneResources initializes render texture", "[unit]") {
  // Setup
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  // Create scene and data...

  // Act
  auto result = configurator.ConfigureSceneResources(scene, scene_data);

  // Assert
  REQUIRE(result.has_value());
  
  // Verify texture dimensions (if accessible)
  // REQUIRE(texture.getSize().x == 800);
  // REQUIRE(texture.getSize().y == 600);
}
```

### Integration Test

Test the full flow:
1. Load configuration from test JSON
2. Apply to runtime object
3. Verify object is properly configured
4. Test object functionality

---

## Reference Examples

### Window Creation (Existing Pattern)

**File:** `src/engine/engine_configuration.cpp`

```cpp
std::expected<std::monostate, FailInfo>
ConfigureEngineResources(EngineResources &engine_resources,
                         const EngineResourcesConfig &config_data) {
  // Create window with configured settings
  sf::Vector2u window_size(config_data.window_width, 
                          config_data.window_height);

  std::string window_title = config_data.window_title.empty() 
      ? "SteamRot" 
      : config_data.window_title;

  engine_resources.game_window.create(
      sf::VideoMode(window_size), window_title);

  engine_resources.game_window.setFramerateLimit(
      config_data.framerate_limit);

  return std::monostate{};
}
```

---

## Troubleshooting

### Problem: "FlatbuffersDataNotFound" Error

**Cause:** Binary file not generated or in wrong location  
**Solution:** 
1. Verify JSON file exists in `data/defaults/<category>/`
2. Build project (CMake generates binaries)
3. Check build output for FlatBuffers compilation errors

### Problem: Segmentation Fault When Loading

**Cause:** Missing null checks on optional fields  
**Solution:**
1. Add null checks for all vectors/tables: `if (data->field())`
2. Add null checks for strings: `if (config->name())`
3. Check element validity in loops: `if (config)`

### Problem: Configuration Not Applied

**Cause:** Configurator method not called or logic incomplete  
**Solution:**
1. Verify `ConfigureScene()` calls your method
2. Check for TODO comments in configurator
3. Add logging to confirm code executes

---

## Best Practices

1. **Always use `std::expected`** for error handling
2. **Null-check all FlatBuffers data** before access
3. **Provide descriptive FailInfo messages** for debugging
4. **Follow existing patterns** (see engine_configuration.cpp)
5. **Validate configuration values** before applying
6. **Return early on errors** - don't continue with invalid state
7. **Test with both prod and test configurations**

---

## See Also

- **Main Analysis:** `documentation/analysis/RENDER_TEXTURE_DISPLAY_ISSUE.md`
- **Error Handling Guide:** `documentation/architecture/ERROR_HANDLING.md` (if exists)
- **FlatBuffers Documentation:** https://google.github.io/flatbuffers/
- **SFML Documentation:** https://www.sfml-dev.org/documentation/

---

## Conclusion

Applying FlatBuffers configuration follows a consistent pattern:
1. Load data via DataLoader
2. Find matching configuration
3. Extract values with null checks
4. Apply to runtime object
5. Validate and return errors if needed

Follow this workflow and the existing patterns in `engine_configuration.cpp` for consistent, safe configuration application.
