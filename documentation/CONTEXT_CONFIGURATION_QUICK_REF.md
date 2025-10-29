# Context Configuration Quick Reference

## TL;DR

Data-driven context configuration allows editing JSON files instead of recompiling for configuration changes.

## File Locations

- **Production Config**: `data/context/context_data.json`
- **Test Config**: `tests/data/context/test_context_data.json`
- **Full Documentation**: `documentation/CONTEXT_CONFIGURATION.md`

## Quick Start

### 1. Load Configuration

```cpp
#include "FlatbuffersDataLoader.h"
#include "ContextConfigurator.h"

steamrot::FlatbuffersDataLoader loader;
auto context_data = loader.ProvideContextData().value();
steamrot::ContextConfigurator configurator(context_data);
```

### 2. Create GameContext

```cpp
auto builder = configurator.CreateGameContextBuilder().value();

// Add runtime objects
builder.SetWindow(window_ptr)
       .SetEventHandler(handler_ptr)
       .SetAssetManager(assets_ptr)
       .SetLoopNumber(loop_num_ptr);

auto context = builder.Build().value();
```

### 3. Create LogicContext for Scene

```cpp
auto logic_builder = configurator.CreateLogicContextBuilder(
    steamrot::SceneType::SceneType_TITLE).value();

// Set runtime objects
logic_builder.SetSceneEntities(entities_ptr)
             .SetArchetypes(archetypes_ptr)
             .SetSceneTexture(texture_ptr)
             .SetGameWindow(window_ptr)
             .SetAssetManager(assets_ptr)
             .SetEventHandler(handler_ptr)
             .SetMousePosition(mouse_pos_ptr);

auto logic_context = logic_builder.Build().value();
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
auto result = configurator.CreateGameContextBuilder();
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

## What the Configurator Does

### ✅ Sets from Configuration
- Environment type

### ❌ Still Needs Runtime Objects
- Window
- Event Handler
- Asset Manager
- Loop Number
- Scene Entities
- Archetypes
- Scene Texture
- Mouse Position

**Note**: Configurator prepares builder with static settings. Runtime objects must still be added before calling `Build()`.

## Future Enhancements

Future enhancements will add `ContextDirector` for:
- Static registry of builders by scene type
- Centralized context lifecycle management
- Easier scene switching

## Help

- **Full Docs**: `documentation/CONTEXT_CONFIGURATION.md`
- **Code**: `src/context/ContextConfigurator.h/cpp`
- **Tests**: `tests/unit/context/ContextConfigurator.test.cpp`
