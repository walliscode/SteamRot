# Path Configuration System

## Overview

The SteamRot engine uses a compile-time path configuration system to select between different data directories for tests and production builds.

## Current Implementation

### Compile-Time Environment Selection

The path system is defined in `src/data_providers/paths.h` and uses preprocessor directives to select the base data directory at compile time.

### Environments

There are **two** distinct environments:

1. **Test Environment** (Default)
   - Used by: Unit tests and integration tests
   - Base path: `${SOURCE_DIR}/tests/data`
   - Activated when: No environment macro is defined
   - Purpose: Isolated test data that won't interfere with production data

2. **Production Environment**
   - Used by: SteamRot executable
   - Base path: `${SOURCE_DIR}/data`
   - Activated when: `STEAMROT_ENV_PROD` is defined
   - Purpose: Actual game data used by the executable

### Directory Structure

Both environments follow the same structure under their base directory:

```
{base_path}/
├── defaults/          # Read-only default configuration
│   ├── scenes/
│   ├── ui_styles/
│   ├── preferences/
│   ├── engine/
│   ├── context/
│   ├── scene_manager/
│   └── asset_manager/
├── assets/            # Game assets
│   └── fonts/
├── fragments/
└── user/              # User-specific data (read-write)
    ├── preferences/
    └── saves/
```

## Path Functions

All path functions are in the `steamrot::paths` namespace:

### Core Paths
- `GetSourceDirectory()` - Root project directory (configured by CMake)
- `GetDataDirectory()` - Base data directory (environment-dependent)
- `GetDefaultsDirectory()` - Read-only defaults directory
- `GetUserDirectory()` - User-specific data directory

### Specific Paths
- `GetSceneDirectory()` - Scene data files
- `GetUIStylesDirectory()` - UI style definitions
- `GetAssetsDirectory()` - Game assets (images, sounds, etc.)
- `GetFontsDirectory()` - Font files
- `GetPreferencesDirectory()` - User preferences
- `GetSavesDirectory()` - User save files
- `GetDefaultEngineDirectory()` - Engine configuration
- `GetDefaultContextDirectory()` - Context configuration
- `GetDefaultSceneManagerDirectory()` - Scene manager configuration
- `GetDefaultAssetManagerDirectory()` - Asset manager configuration

## Usage

### For Production Executable

In `steamrot/CMakeLists.txt`:
```cmake
target_compile_definitions(SteamRot PRIVATE STEAMROT_ENV_PROD)
```

### For Tests

No definition needed - test environment is the default:
```cmake
# Tests automatically use tests/data directory
add_executable(test_something test_file.cpp)
target_link_libraries(test_something PRIVATE data_providers)
```

### In Code

```cpp
#include "paths.h"

// Get any path - environment selection is automatic
auto scene_dir = steamrot::paths::GetSceneDirectory();
auto ui_styles = steamrot::paths::GetUIStylesDirectory();
```

## Historical Note: STEAMROT_ENV_DEBUG

Previously, the system included a `STEAMROT_ENV_DEBUG` macro that was identical to `STEAMROT_ENV_PROD` (both resolved to `data/` directory). This was confusing and redundant, so it was removed in favor of the simpler two-environment system.

If you see `STEAMROT_ENV_DEBUG` in older code or commits, it should be replaced with `STEAMROT_ENV_PROD`.

## Design Rationale

### Why Compile-Time Selection?

1. **Zero Runtime Overhead**: Path selection happens at compile time
2. **Type Safety**: No runtime configuration errors
3. **Simplicity**: No need to pass configuration through the entire application
4. **Clarity**: Each build is explicitly either for testing or production

### Why Not Runtime Configuration?

Runtime configuration was considered but rejected because:
- Adds complexity for minimal benefit
- Introduces potential for runtime errors (wrong path configuration)
- The use case (switching between test and production) already happens at compile time
- Tests and production builds are separate executables anyway

### Test Data Isolation

Test data is completely separate from production data to:
- Prevent tests from modifying production data
- Allow tests to use simplified/controlled data
- Enable parallel test execution without conflicts
- Make it clear when test data is being used incorrectly

## Common Issues

### Issue: "Test paths are being used in production"

**Symptom**: Running the SteamRot executable shows paths containing `tests/data`

**Causes**:
1. The executable wasn't built with `STEAMROT_ENV_PROD` defined
2. Build cache wasn't cleared after changing the environment macro
3. Debug logging shows test-like paths (but they're actually under `data/`)

**Solution**:
1. Verify `steamrot/CMakeLists.txt` contains:
   ```cmake
   target_compile_definitions(SteamRot PRIVATE STEAMROT_ENV_PROD)
   ```
2. Clean rebuild:
   ```bash
   rm -rf build/
   cmake --preset Debug
   cmake --build --preset Debug
   ```
3. Check the actual paths in debug output - they should start with `data/`, not `tests/data/`

### Issue: "Confusing DEBUG vs PROD naming"

If you see both `STEAMROT_ENV_DEBUG` and `STEAMROT_ENV_PROD` in the code, this is legacy naming. They both resolved to the same path (`data/`), which was confusing. The system has been simplified to only use `STEAMROT_ENV_PROD` for non-test builds.

## Future Enhancements

Potential improvements to consider:

1. **User Data Paths**: Extend to support user-specific data directories outside the source tree (e.g., `~/.config/steamrot/`)
2. **Resource Packs**: Support loading additional data from multiple directories
3. **Hot Reload**: Support watching for file changes during development
4. **Path Validation**: Add compile-time checks that required data files exist

## Related Files

- `src/data_providers/paths.h` - Path function declarations
- `src/data_providers/paths.cpp.in` - CMake-configured source directory
- `steamrot/CMakeLists.txt` - Production environment definition
- `tests/*/CMakeLists.txt` - Test targets (use default test environment)
