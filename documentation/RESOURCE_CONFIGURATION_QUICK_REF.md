# Resource Configuration Quick Reference

## Resource vs Context

| Aspect | Resources | Contexts |
|--------|-----------|----------|
| **Purpose** | Own actual objects | Reference owned objects |
| **Lifetime** | Lives on GameEngine/Scene | Temporary, passed around |
| **Configuration** | From resource FlatBuffers | Takes resource references |
| **Schema** | `game_resources.fbs`, `scene_resources.fbs` | `context_data.fbs` |

## Quick Setup

### Configure GameResources

```cpp
#include "ResourceConfigurator.h"
#include "FlatbuffersDataLoader.h"

// Load data
FlatbuffersDataLoader loader;
auto game_data = loader.ProvideGameResourcesData().value();
auto scene_data = loader.ProvideSceneResourcesData().value();

// Configure
ResourceConfigurator configurator(game_data, scene_data);
GameResources resources;
configurator.ConfigureGameResources(resources);
```

### Configure SceneResources

```cpp
SceneResources resources;
configurator.ConfigureSceneResources(resources, SceneType::SceneType_TITLE);
```

### Create Contexts from Resources

```cpp
// GameContext references GameResources
GameContext game_context(game_resources);

// SceneContext references multiple resources
SceneContext scene_context(scene_resources, game_resources, entity_manager);
```

## File Locations

### JSON Configuration Files

```
data/resources/
  ├── game_resources.game_resources.json
  └── scene_resources.scene_resources.json

tests/data/resources/
  ├── game_resources.game_resources.json
  └── scene_resources.scene_resources.json
```

### Schemas

```
src/flatbuffers_headers/
  ├── game_resources.fbs
  └── scene_resources.fbs
```

## Common Patterns

### Pattern 1: Configure at Startup

```cpp
void GameEngine::StartUp() {
  FlatbuffersDataLoader loader;
  
  // Load resource data
  auto game_data = loader.ProvideGameResourcesData().value();
  auto scene_data = loader.ProvideSceneResourcesData().value();
  
  // Configure resources
  ResourceConfigurator configurator(game_data, scene_data);
  configurator.ConfigureGameResources(m_game_resources);
}
```

### Pattern 2: Configure Scene Resources

```cpp
void Scene::Initialize(SceneType scene_type) {
  FlatbuffersDataLoader loader;
  auto game_data = loader.ProvideGameResourcesData().value();
  auto scene_data = loader.ProvideSceneResourcesData().value();
  
  ResourceConfigurator configurator(game_data, scene_data);
  configurator.ConfigureSceneResources(m_scene_resources, scene_type);
}
```

### Pattern 3: Testing with Custom Data

```cpp
TEST_CASE("Custom resource configuration") {
  flatbuffers::FlatBufferBuilder fbb;
  auto title = fbb.CreateString("Test");
  auto env = fbb.CreateString("Test");
  auto game_data = CreateGameResourcesData(fbb, 800, 600, title, 60, env);
  fbb.Finish(game_data);
  
  const GameResourcesData *data = GetGameResourcesData(fbb.GetBufferPointer());
  ResourceConfigurator configurator(data, nullptr);
  
  GameResources resources;
  auto result = configurator.ConfigureGameResources(resources);
  REQUIRE(result.has_value());
}
```

## JSON Structure

### GameResources

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

```json
{
  "scenes": [
    {
      "scene_type": "TITLE",
      "render_texture_width": 800,
      "render_texture_height": 600
    }
  ]
}
```

## Error Handling

```cpp
auto result = configurator.ConfigureGameResources(resources);
if (!result.has_value()) {
  const FailInfo &error = result.error();
  
  switch (error.mode) {
    case FailMode::NullPointer:
      // Resource data is null
      break;
    case FailMode::SceneTypeNotFound:
      // Requested scene not in configuration
      break;
    default:
      // Other errors
      break;
  }
  
  std::cerr << error.message << "\n";
}
```

## Key Classes

| Class | Purpose | Location |
|-------|---------|----------|
| `GameResources` | Owns game-level resources | `src/resources/GameResources.h` |
| `SceneResources` | Owns scene-level resources | `src/resources/SceneResources.h` |
| `ResourceConfigurator` | Configures resources from data | `src/resources/ResourceConfigurator.h` |
| `FlatbuffersDataLoader` | Loads FlatBuffers data | `src/data_handlers/FlatbuffersDataLoader.h` |

## See Also

- `RESOURCE_CONFIGURATION.md` - Detailed documentation
- `RESOURCE_CONTEXT_REFACTORING_PLAN.md` - Refactoring plan
- `CONTEXT_CONFIGURATION.md` - Context configuration (separate concern)
