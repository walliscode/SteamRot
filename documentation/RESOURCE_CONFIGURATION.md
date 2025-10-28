# Resource and Context Configuration Separation

## Overview

As of the resource/context separation refactoring, SteamRot now has a clear separation between:

1. **Resource Configuration** - Configuration of actual game resources (windows, textures, etc.)
2. **Context Configuration** - Configuration of how contexts reference and use resources

This separation allows resources to be configured independently from contexts, improving modularity and testability.

## Architecture

### Two-Layer System

**Layer 1: Resources (Ownership)**
- Own the actual resource objects (windows, textures, etc.)
- Configured from resource-specific FlatBuffers schemas
- No dependencies on contexts
- Live at appropriate scope levels (game, scene)

**Layer 2: Contexts (References)**
- Lightweight reference containers
- Reference resources owned elsewhere
- Cheap to copy (just references)
- Provide convenient access to resources

## Resource Configuration

### GameResources

**Data Schema**: `src/flatbuffers_headers/game_resources.fbs`

```fbs
table GameResourcesData {
  window_width: uint32 = 800;
  window_height: uint32 = 600;
  window_title: string;
  framerate_limit: uint32 = 60;
  environment_type: string; // "None", "Test", "Production"
}
```

**JSON Configuration**: `data/resources/game_resources.game_resources.json`

```json
{
  "window_width": 800,
  "window_height": 600,
  "window_title": "SteamRot",
  "framerate_limit": 60,
  "environment_type": "Production"
}
```

### SceneResources

**Data Schema**: `src/flatbuffers_headers/scene_resources.fbs`

```fbs
table SceneResourcesData {
  scene_type: SceneType;
  render_texture_width: uint32 = 800;
  render_texture_height: uint32 = 600;
}

table SceneResourcesCollection {
  scenes: [SceneResourcesData];
}
```

**JSON Configuration**: `data/resources/scene_resources.scene_resources.json`

```json
{
  "scenes": [
    {
      "scene_type": "TEST",
      "render_texture_width": 800,
      "render_texture_height": 600
    },
    {
      "scene_type": "TITLE",
      "render_texture_width": 800,
      "render_texture_height": 600
    }
  ]
}
```

## Using ResourceConfigurator

### Direct Usage (Recommended)

```cpp
#include "ResourceConfigurator.h"
#include "FlatbuffersDataLoader.h"
#include "GameResources.h"
#include "SceneResources.h"

// Load resource data
steamrot::FlatbuffersDataLoader loader;
auto game_data = loader.ProvideGameResourcesData().value();
auto scene_data = loader.ProvideSceneResourcesData().value();

// Create configurator
steamrot::ResourceConfigurator configurator(game_data, scene_data);

// Configure game resources
steamrot::GameResources game_resources;
configurator.ConfigureGameResources(game_resources);

// Configure scene resources
steamrot::SceneResources scene_resources;
configurator.ConfigureSceneResources(scene_resources, 
                                     steamrot::SceneType::SceneType_TITLE);
```

### Via ContextConfigurator (Convenience)

For backward compatibility, ContextConfigurator provides a convenience wrapper:

```cpp
#include "ContextConfigurator.h"
#include "FlatbuffersDataLoader.h"

steamrot::FlatbuffersDataLoader loader;
auto context_data = loader.ProvideContextData().value();

steamrot::ContextConfigurator configurator(context_data);

// Internally loads resource data files and uses ResourceConfigurator
steamrot::GameResources game_resources;
configurator.ConfigureGameResources(game_resources);
```

## Key Benefits

### 1. Independent Configuration

Resources can be configured without needing context data:

```cpp
// Just need resource data, not context data
auto game_data = loader.ProvideGameResourcesData().value();
auto scene_data = loader.ProvideSceneResourcesData().value();

ResourceConfigurator configurator(game_data, scene_data);
// Configure resources independently
```

### 2. Separation of Concerns

- **Resource concerns**: Window size, texture dimensions, environment type
- **Context concerns**: How components reference and use resources

### 3. Easier Testing

Test resources without needing full context setup:

```cpp
TEST_CASE("Configure resources directly") {
  auto game_data = CreateTestGameResourcesData();
  auto scene_data = CreateTestSceneResourcesData();
  
  ResourceConfigurator configurator(game_data, scene_data);
  GameResources resources;
  
  auto result = configurator.ConfigureGameResources(resources);
  REQUIRE(result.has_value());
}
```

### 4. Flexible Resource Loading

Resources can be loaded from different sources:
- Files (production)
- In-memory FlatBuffers (testing)
- Multiple configurations (different environments)

## Migration Guide

### Old Approach (Pre-Separation)

```cpp
// Everything came from ContextData
FlatbuffersDataLoader loader;
auto context_data = loader.ProvideContextData().value();

ContextConfigurator configurator(context_data);
GameResources resources;
configurator.ConfigureGameResources(resources);
```

### New Approach (Post-Separation)

```cpp
// Option 1: Direct (recommended for new code)
FlatbuffersDataLoader loader;
auto game_data = loader.ProvideGameResourcesData().value();
auto scene_data = loader.ProvideSceneResourcesData().value();

ResourceConfigurator configurator(game_data, scene_data);
GameResources resources;
configurator.ConfigureGameResources(resources);

// Option 2: Via ContextConfigurator (backward compatible)
FlatbuffersDataLoader loader;
auto context_data = loader.ProvideContextData().value();

ContextConfigurator configurator(context_data);
GameResources resources;
configurator.ConfigureGameResources(resources);
// Internally uses ResourceConfigurator
```

## File Locations

### Production Configuration
- `data/resources/game_resources.game_resources.json`
- `data/resources/scene_resources.scene_resources.json`

### Test Configuration
- `tests/data/resources/game_resources.game_resources.json`
- `tests/data/resources/scene_resources.scene_resources.json`

### Schemas
- `src/flatbuffers_headers/game_resources.fbs`
- `src/flatbuffers_headers/scene_resources.fbs`

### Source Code
- `src/resources/ResourceConfigurator.h/cpp`
- `src/resources/GameResources.h`
- `src/resources/SceneResources.h`

## Build Integration

The CMake build system automatically:
1. Compiles FlatBuffers schemas to C++ headers
2. Converts JSON files to binary `.bin` files
3. Copies production binaries to test directory

To add new resource configurations:
1. Update the FlatBuffers schema (`.fbs` file)
2. Update the JSON configuration file
3. Rebuild the project (schemas and binaries auto-generated)

## Best Practices

### 1. Use ResourceConfigurator Directly

For new code, prefer direct usage of ResourceConfigurator:

```cpp
ResourceConfigurator configurator(game_data, scene_data);
```

### 2. Separate Resource and Context Concerns

- Use resource files for: window size, texture dimensions, environment type
- Use context files for: runtime references, dependencies between systems

### 3. Test with Custom Data

Create custom FlatBuffers data for testing edge cases:

```cpp
flatbuffers::FlatBufferBuilder fbb;
auto title = fbb.CreateString("Test");
auto env = fbb.CreateString("Test");
auto game_data = CreateGameResourcesData(fbb, 640, 480, title, 30, env);
fbb.Finish(game_data);

const GameResourcesData *data = GetGameResourcesData(fbb.GetBufferPointer());
```

### 4. Validate Early

Load and validate resource configuration at application startup:

```cpp
// In GameEngine::StartUp()
auto game_data_result = loader.ProvideGameResourcesData();
if (!game_data_result.has_value()) {
  // Handle error and exit gracefully
  std::cerr << game_data_result.error().message << "\n";
  return;
}
```

## Related Documentation

- `RESOURCE_CONTEXT_REFACTORING_PLAN.md` - Overall refactoring plan
- `CONTEXT_CONFIGURATION.md` - Context configuration (separate from resources)
- `CONTEXT_HANDLING_IMPROVEMENT_PLAN.md` - Long-term improvement plan

## Future Enhancements

Potential future improvements:
1. Runtime resource reloading (hot-reload)
2. Per-scene resource configurations
3. Resource validation at compile-time
4. Resource pooling and caching
