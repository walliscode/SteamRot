# Analysis: Replacing PathProvider Classes with Namespaces

## Status: MIGRATION COMPLETE ✓

The `PathProvider` class hierarchy has been completely replaced with namespace-based static functions using compile-time environment selection.

## Implementation Summary

### Files Added
| File | Purpose |
|------|---------|
| `src/data_handlers/paths.h` | Namespace-based path functions with compile-time environment selection |
| `src/data_handlers/paths.cpp.in` | CMake-configured source directory implementation |

### Files Removed
| File | Reason |
|------|--------|
| `PathProvider.h` | Replaced by paths.h namespace |
| `PathProvider.cpp.in` | No longer needed |
| `GamePaths.h` | Replaced by STEAMROT_ENV_PROD compile definition |
| `GamePaths.cpp.in` | No longer needed |
| `TestPaths.h` | Replaced by default test paths |
| `TestPaths.cpp.in` | No longer needed |

### How It Works

```cpp
// Default: Test environment (tests/data)
#include "paths.h"
auto path = steamrot::paths::GetDataDirectory();  // -> ${SOURCE_DIR}/tests/data

// For Production builds (data)
// CMake: target_compile_definitions(steamrot PRIVATE STEAMROT_ENV_PROD)
#include "paths.h"
auto path = steamrot::paths::GetDataDirectory();  // -> ${SOURCE_DIR}/data
```

### Available Functions
- `GetSourceDirectory()` - Returns CMake source directory
- `GetDataDirectory()` - Returns environment-specific data directory
- `GetFragmentDirectory()` - Returns `GetDataDirectory() / "fragments"`
- `GetSceneDirectory()` - Returns `GetDataDirectory() / "scenes"`
- `GetAssetsDirectory()` - Returns `GetDataDirectory() / "assets"`
- `GetFontsDirectory()` - Returns `GetAssetsDirectory() / "fonts"`
- `GetUIStylesDirectory()` - Returns `GetDataDirectory() / "ui_styles"`

### Environment Selection (Compile-Time)
| Macro | Data Directory |
|-------|---------------|
| (none/default) | `tests/data` |
| `STEAMROT_ENV_DEBUG` | `data` |
| `STEAMROT_ENV_PROD` | `data` |

### Key Changes Made

1. **DataLoader/FlatbuffersDataLoader**: Removed PathProvider constructor parameter
2. **AssetManager**: Removed PathProvider member, uses paths namespace directly
3. **GameContext/GameResources**: Removed PathProvider reference
4. **Engine/GameEngine/TestEngine**: No longer need PathProvider management
5. **EntityManager/FlatbuffersConfigurator**: Removed PathProvider parameter
6. **SceneFactory/StylesConfigurator**: Updated to use paths namespace
7. **All test files**: Simplified - no longer need TestPaths instances

### Benefits Achieved

1. **Fixed undefined behavior**: The Engine constructor bug is eliminated
2. **Simpler API**: ~30 files no longer pass PathProvider references
3. **Zero runtime overhead**: Compile-time path selection
4. **Cleaner tests**: Tests no longer need to instantiate TestPaths
