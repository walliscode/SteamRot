# Context Configuration Quick Reference

## TL;DR

Stage 2 adds data-driven configuration for resources. Edit JSON files instead of recompiling for configuration changes.

## File Locations

- **Production Config**: `data/context/context_data.json`
- **Test Config**: `tests/data/context/test_context_data.json`
- **Full Documentation**: `documentation/CONTEXT_CONFIGURATION.md`

## Quick Start

### 1. Load Configuration

```cpp
#include "FlatbuffersDataLoader.h"
#include "ResourceConfigurator.h"

steamrot::FlatbuffersDataLoader loader;
auto context_data = loader.ProvideContextData().value();
steamrot::ResourceConfigurator configurator(context_data);
```

### 2. Configure GameResources

```cpp
steamrot::GameResources game_resources;
auto result = configurator.ConfigureGameResources(game_resources);
if (!result.has_value()) {
  // Handle error
}
// game_resources.game_window is now created and configured
// game_resources.env_type is set
```

### 3. Configure SceneResources

```cpp
steamrot::SceneResources scene_resources;
auto result = configurator.ConfigureSceneResources(
    scene_resources, steamrot::SceneType::SceneType_TITLE);
if (!result.has_value()) {
  // Handle error
}
// scene_resources.scene_texture is now created with correct dimensions
```

## Configuration Format

```json
{
  "game_context": {
    "window_width": 800,
    "window_height": 600,
    "window_title": "SteamRot",
    "framerate_limit": 60,
    "environment_type": "Production"  // "None", "Test", "Production"
  },
  "scene_contexts": [
    {
      "scene_type": "TEST",           // "UNKNOWN", "TEST", "TITLE", "CRAFTING"
      "entity_pool_size": 100,
      "render_texture_width": 800,
      "render_texture_height": 600
    }
  ]
}
```

## Common Tasks

### Adding a New Scene Configuration

1. Edit `data/context/context_data.json`
2. Add entry to `scene_contexts` array:
```json
{
  "scene_type": "NEW_SCENE",
  "entity_pool_size": 150,
  "render_texture_width": 800,
  "render_texture_height": 600
}
```
3. Build will auto-generate binary

### Changing Window Size

Edit `game_context` in JSON:
```json
{
  "game_context": {
    "window_width": 1024,
    "window_height": 768,
    ...
  }
}
```

### Creating Test Configuration

Copy production config to `tests/data/context/test_context_data.json` and modify:
```json
{
  "game_context": {
    "environment_type": "Test",
    "framerate_limit": 30,
    ...
  }
}
```

## Error Handling

All methods return `std::expected`:

```cpp
auto result = configurator.ConfigureGameResources(game_resources);
if (!result.has_value()) {
  const auto& error = result.error();
  // error.mode = FailMode enum
  // error.message = string description
}
```

## Common Errors

| Error | Cause | Solution |
|-------|-------|----------|
| `NullPointer` | Config data is null | Check ProvideContextData() succeeded |
| `MissingRequiredField` | Missing game_context or scene_contexts | Add required fields to JSON |
| `NonExistentEnumValue` | Invalid environment_type string | Use "None", "Test", or "Production" |
| `SceneTypeNotFound` | Scene not in configuration | Add scene to scene_contexts array |

## Build Integration

### Automatic Processing

Build system automatically:
1. Compiles `context_data.fbs` → `context_data_generated.h`
2. Converts JSON → binary: `context_data.json` → `context_data.bin`
3. Copies production binary to test directory

### Files Generated

- `src/flatbuffers_headers/context_data_generated.h` (C++ header)
- `data/context/context_data.bin` (production binary)
- `tests/data/context/context_data.bin` (test binary, copied from production)

## Testing

### Unit Tests
```bash
# Tests auto-discovered by CTest (user runs locally)
test_context --test-case-name="ContextConfigurator*"
```

### Integration Tests
```bash
# Full workflow tests (user runs locally)
test_context_configuration
```

## What the ResourceConfigurator Does

### ✅ Configures from JSON
- Window creation (size, title, framerate)
- Environment type
- Scene render texture dimensions

### Configuration is Direct
Resources (GameResources, SceneResources) are configured directly from ContextData. No builder pattern - resources are modified in place.

**Note**: Configurator creates and configures resources directly from JSON configuration data.

## Next Steps

Future improvements may add centralized context management for easier scene switching.

See `documentation/CONTEXT_HANDLING_IMPROVEMENT_PLAN.md` for roadmap.

## Help

- **Full Docs**: `documentation/CONTEXT_CONFIGURATION.md`
- **Implementation Summary**: `documentation/STAGE_2_SUMMARY.md`
- **Overall Plan**: `documentation/CONTEXT_HANDLING_IMPROVEMENT_PLAN.md`
- **Code**: `src/resources/ResourceConfigurator.h/cpp`
- **Tests**: `tests/integration/context_configuration/`
