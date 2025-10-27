# Context Configuration Documentation

## Overview

Stage 2 of the Context Handling Improvement Plan introduces data-driven configuration for context initialization. This allows game and scene settings to be externalized to JSON files, making it easier to adjust configurations without recompiling.

## Configuration Files

### Location

- **Production**: `data/context/context_data.json`
- **Test**: `tests/data/context/test_context_data.json` (optional override for testing)

### Schema

The configuration uses FlatBuffers schema defined in `src/flatbuffers_headers/context_data.fbs`.

## Configuration Structure

### Root Structure

```json
{
  "game_context": { ... },
  "scene_contexts": [ ... ]
}
```

### GameContextConfig

Configures global game settings:

```json
{
  "game_context": {
    "window_width": 800,
    "window_height": 600,
    "window_title": "SteamRot",
    "framerate_limit": 60,
    "environment_type": "Production"
  }
}
```

**Fields:**
- `window_width` (uint32, default: 800): Width of the game window in pixels
- `window_height` (uint32, default: 600): Height of the game window in pixels
- `window_title` (string): Title displayed in the window title bar
- `framerate_limit` (uint32, default: 60): Maximum frames per second
- `environment_type` (string): Environment type - one of:
  - `"None"`: No specific environment (default)
  - `"Test"`: Test environment
  - `"Production"`: Production environment

### SceneContextConfig

Configures scene-specific settings. Multiple scenes can be configured:

```json
{
  "scene_contexts": [
    {
      "scene_type": "TEST",
      "entity_pool_size": 100,
      "render_texture_width": 800,
      "render_texture_height": 600
    },
    {
      "scene_type": "TITLE",
      "entity_pool_size": 50,
      "render_texture_width": 800,
      "render_texture_height": 600
    }
  ]
}
```

**Fields:**
- `scene_type` (SceneType): Scene identifier - one of:
  - `"UNKNOWN"`: Unknown scene type
  - `"TEST"`: Test scene
  - `"TITLE"`: Title screen scene
  - `"CRAFTING"`: Crafting scene
- `entity_pool_size` (uint32, default: 100): Maximum number of entities for this scene
- `render_texture_width` (uint32, default: 800): Width of scene render texture in pixels
- `render_texture_height` (uint32, default: 600): Height of scene render texture in pixels

## Usage Pattern

### 1. Load Configuration Data

```cpp
#include "FlatbuffersDataLoader.h"
#include "ContextConfigurator.h"

steamrot::PathProvider path_provider{steamrot::EnvironmentType::Production};
steamrot::FlatbuffersDataLoader loader;

auto context_data_result = loader.ProvideContextData();
if (!context_data_result.has_value()) {
  // Handle error
}
```

### 2. Create Configurator

```cpp
const steamrot::ContextData* context_data = context_data_result.value();
steamrot::ContextConfigurator configurator(context_data);
```

### 3. Create GameContextBuilder

```cpp
auto builder_result = configurator.CreateGameContextBuilder();
if (!builder_result.has_value()) {
  // Handle error
}

auto builder = builder_result.value();
```

### 4. Set Runtime Objects

The builder from the configurator has static configuration (like environment type) pre-set, but still requires runtime objects:

```cpp
builder.SetWindow(window_ptr)
       .SetEventHandler(event_handler_ptr)
       .SetAssetManager(asset_manager_ptr)
       .SetLoopNumber(loop_num_ptr);
```

### 5. Build Context

```cpp
auto context_result = builder.Build();
if (context_result.has_value()) {
  steamrot::GameContext context = context_result.value();
  // Use context
}
```

### Creating LogicContext from Configuration

```cpp
auto logic_builder_result = configurator.CreateLogicContextBuilder(
    steamrot::SceneType::SceneType_TITLE);

if (logic_builder_result.has_value()) {
  auto logic_builder = logic_builder_result.value();
  
  // Set runtime objects
  logic_builder.SetSceneEntities(scene_entities_ptr)
               .SetArchetypes(archetypes_ptr)
               .SetSceneTexture(scene_texture_ptr)
               .SetGameWindow(game_window_ptr)
               .SetAssetManager(asset_manager_ptr)
               .SetEventHandler(event_handler_ptr)
               .SetMousePosition(mouse_position_ptr);
  
  auto logic_context = logic_builder.Build();
  // Use logic context
}
```

## Build System Integration

### Schema Compilation

The FlatBuffers schema is automatically compiled during the build process:

1. `context_data.fbs` → `context_data_generated.h` (C++ header)
2. Included in the `flatbuffers_headers` library

### JSON to Binary Conversion

JSON configuration files are automatically converted to binary format during build:

1. `data/context/context_data.json` → `data/context/context_data.bin`
2. Production binary is copied to `tests/data/context/context_data.bin` for testing

This happens via CMake macro `flatbuffers_generate_for_type()` in `convert_json_to_binary.cmake`.

## Error Handling

The `ContextConfigurator` returns `std::expected` for all operations, providing detailed error information on failure:

### Common Error Cases

1. **Null Config Data**
   - `FailMode::NullPointer`: Configuration data pointer is null
   
2. **Missing Game Context**
   - `FailMode::MissingRequiredField`: `game_context` field not present in config
   
3. **Invalid Environment Type**
   - `FailMode::NonExistentEnumValue`: Unknown environment type string
   
4. **Missing Scene Contexts**
   - `FailMode::MissingRequiredField`: `scene_contexts` field not present
   
5. **Scene Type Not Found**
   - `FailMode::SceneTypeNotFound`: Requested scene type not configured

### Example Error Handling

```cpp
auto builder_result = configurator.CreateGameContextBuilder();
if (!builder_result.has_value()) {
  const auto& error = builder_result.error();
  
  switch (error.mode) {
    case steamrot::FailMode::NullPointer:
      // Handle null config
      break;
    case steamrot::FailMode::NonExistentEnumValue:
      // Handle invalid environment type
      break;
    default:
      // Handle other errors
      break;
  }
  
  // Access error message
  std::string error_msg = error.message;
}
```

## Best Practices

### 1. Validate Configuration at Startup

Load and validate configuration early in the application startup to catch errors before they impact runtime:

```cpp
// In GameEngine::StartUp()
auto config_result = LoadAndValidateConfiguration();
if (!config_result.has_value()) {
  // Display error and exit gracefully
}
```

### 2. Use Test Configuration for Testing

Create separate test configurations with appropriate values for test scenarios:

```json
{
  "game_context": {
    "environment_type": "Test",
    "framerate_limit": 30
  }
}
```

### 3. Keep Defaults Reasonable

Use sensible defaults in the schema so missing values don't cause issues:

```fbs
table GameContextConfig {
  window_width: uint32 = 800;  // Default value
  window_height: uint32 = 600;
}
```

### 4. Document Configuration Changes

When adding new configuration fields:
1. Update the FlatBuffers schema
2. Update this documentation
3. Update example JSON files
4. Add validation in ContextConfigurator if needed

## Future Enhancements

### Stage 3: Context Management

## Stage 3: ContextDirector Integration

**Status**: ✅ IMPLEMENTED (See `documentation/STAGE_3_SUMMARY.md`)

Stage 3 introduces `ContextDirector` for centralized context lifecycle management:

- Static registry for LogicContextBuilders by scene type
- Centralized context creation and management
- Automatic integration with SceneFactory
- Backward compatible with existing code

### Using ContextDirector

#### 1. Register Builders from Configuration

```cpp
#include "ContextDirector.h"
#include "ContextConfigurator.h"
#include "FlatbuffersDataLoader.h"

// Load configuration
steamrot::FlatbuffersDataLoader loader;
auto context_data = loader.ProvideContextData().value();

// Create configurator
steamrot::ContextConfigurator configurator(context_data);

// Register builders for all configured scenes
for (const auto* scene_config : *context_data->scene_contexts()) {
  // Get builder from configurator
  auto builder = configurator.CreateLogicContextBuilder(
      scene_config->scene_type()).value();
  
  // Configure builder with runtime objects
  builder.SetSceneEntities(scene_entities_ptr)
         .SetArchetypes(archetypes_ptr)
         .SetSceneTexture(scene_texture_ptr)
         .SetGameWindow(game_window_ptr)
         .SetAssetManager(asset_manager_ptr)
         .SetEventHandler(event_handler_ptr)
         .SetMousePosition(mouse_position_ptr);
  
  // Register with ContextDirector
  steamrot::ContextDirector::RegisterLogicContextBuilder(
      scene_config->scene_type(), builder);
}
```

#### 2. SceneFactory Automatic Integration

SceneFactory automatically uses ContextDirector when builders are registered:

```cpp
steamrot::SceneFactory factory;

// If builder is registered with ContextDirector, it will be used
// Otherwise, falls back to Scene::GetLogicContext()
auto scene = factory.CreateDefaultScene(
    steamrot::SceneType::SceneType_TITLE, game_context);
```

#### 3. Manual Context Building

You can also build contexts directly from ContextDirector:

```cpp
// Check if builder is registered
if (steamrot::ContextDirector::HasBuilder(steamrot::SceneType::SceneType_TITLE)) {
  // Build context
  auto context = steamrot::ContextDirector::BuildLogicContext(
      steamrot::SceneType::SceneType_TITLE);
  
  if (context.has_value()) {
    // Use context
    steamrot::LogicContext logic_ctx = context.value();
  }
}
```

#### 4. Lifecycle Management

```cpp
// Clear all registered builders (useful for testing)
steamrot::ContextDirector::ClearBuilders();

// Get a copy of a builder for further configuration
auto builder = steamrot::ContextDirector::GetLogicContextBuilder(
    steamrot::SceneType::SceneType_TITLE).value();
```

### Benefits of ContextDirector

1. **Centralized Management**: All LogicContext builders in one place
2. **Reusable Contexts**: Register once, build multiple times
3. **Configuration Integration**: Works seamlessly with ContextConfigurator
4. **Backward Compatible**: Existing code continues to work
5. **Test Friendly**: Easy to clear and reset for testing

## Future Enhancements (Stage 4 and Beyond)

The next stages will introduce:

- `TestContextDirector` - Similar pattern for test infrastructure
- `TestResources` - Permanent storage for test objects
- Enhanced test configuration via `test_context_data.fbs`
- Improved test context API

See `documentation/CONTEXT_HANDLING_IMPROVEMENT_PLAN.md` for details.

## Related Files

### Source Files
- `src/flatbuffers_headers/context_data.fbs` - Schema definition
- `src/context/ContextConfigurator.h/cpp` - Configuration loader
- `src/context/ContextDirector.h/cpp` - Context lifecycle manager (Stage 3)
- `src/data_handlers/FlatbuffersDataLoader.h/cpp` - Data loading
- `src/scenes/SceneFactory.cpp` - Integrated with ContextDirector (Stage 3)

### Configuration Files
- `data/context/context_data.json` - Production configuration
- `tests/data/context/test_context_data.json` - Test configuration

### Tests
- `tests/unit/context/ContextConfigurator.test.cpp` - Unit tests
- `tests/unit/context/ContextDirector.test.cpp` - ContextDirector unit tests (Stage 3)
- `tests/integration/context_configuration/` - Configuration integration tests
- `tests/integration/context_director/` - ContextDirector integration tests (Stage 3)
- `tests/unit/scenes/SceneFactory.test.cpp` - SceneFactory with ContextDirector tests (Stage 3)

### Documentation
- `documentation/CONTEXT_HANDLING_IMPROVEMENT_PLAN.md` - Overall plan
- `documentation/STAGE_2_SUMMARY.md` - Stage 2 implementation summary
- `documentation/STAGE_3_SUMMARY.md` - Stage 3 implementation summary
- This file - Configuration format reference
