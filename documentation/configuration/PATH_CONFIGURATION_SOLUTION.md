# Path Configuration Solution

## Overview

This document explains the path configuration system in SteamRot and the solution to the asset loading issue.

## Problem Statement

The SteamRot executable was incorrectly loading assets from `tests/data/` instead of `data/` despite having `STEAMROT_ENV_DEBUG` defined.

## Root Cause

The macro `STEAMROT_ENV_DEBUG` was defined as `PRIVATE` on the SteamRot executable, but the libraries (`data_providers`, `assets`) that actually use `paths.h` did NOT have the macro. The original implementation used inline functions in `paths.h`, which meant the path selection happened at the point where the header was included, not where it was linked.

## Solution: Recompile paths.cpp for SteamRot

The solution makes `GetDataDirectory()` a regular function (not inline) and has the SteamRot executable recompile `paths.cpp` with the production macro.

### Implementation

**paths.h** - Function declaration (not inline):
```cpp
std::filesystem::path GetDataDirectory();
```

**paths.cpp.in** - Function implementation with macro:
```cpp
std::filesystem::path GetDataDirectory() {
#if defined(STEAMROT_ENV_PROD) || defined(STEAMROT_ENV_DEBUG)
  return GetSourceDirectory() / "data";
#else
  return GetSourceDirectory() / "tests" / "data";
#endif
}
```

**steamrot/CMakeLists.txt** - Recompile paths.cpp with macro:
```cmake
add_executable(SteamRot main.cpp)

target_compile_definitions(SteamRot PRIVATE STEAMROT_ENV_DEBUG)

# Recompile paths.cpp with the production environment flag
target_sources(SteamRot PRIVATE
  ${CMAKE_BINARY_DIR}/src/data_providers/paths.cpp
)
```

## How It Works

### Default Behavior (Tests)

1. `data_providers` library compiles `paths.cpp` WITHOUT the macro
2. `GetDataDirectory()` returns `tests/data`
3. All test executables link to `data_providers` and get the test-path version
4. Tests use `tests/data` ✅

### Production Behavior (SteamRot)

1. `SteamRot` executable defines `STEAMROT_ENV_DEBUG`
2. `SteamRot` includes `paths.cpp` as a source file
3. `paths.cpp` is recompiled with the macro defined
4. `GetDataDirectory()` returns `data` for SteamRot
5. Linker uses SteamRot's version due to symbol precedence
6. SteamRot uses `data/` ✅

## Benefits

1. **Controlled by SteamRot only**: The executable controls its own path behavior
2. **No library modifications**: Don't need to link special libraries everywhere
3. **Automatic test separation**: Tests naturally use test paths
4. **Clear intent**: The macro is only defined where it's needed
5. **Minimal complexity**: Simple recompilation approach

## Comparison to Previous Approach

| Aspect | INTERFACE Library (Old) | Recompile paths.cpp (New) |
|--------|------------------------|---------------------------|
| Where defined | environment_config library | SteamRot executable |
| Test behavior | Had to avoid linking | Automatic (no special handling) |
| Library changes | Required linking to all libs | No library changes needed |
| Complexity | Medium (interface library) | Low (simple recompile) |
| User request | ❌ Required links everywhere | ✅ Only in SteamRot |

## Migration Notes

**Before (INTERFACE library approach):**
- Created `environment_config` INTERFACE library
- Linked `data_providers` and `assets` to it
- Tests had to avoid linking to it
- More complex CMake configuration

**After (Recompile approach):**
- No special library needed
- SteamRot recompiles paths.cpp directly
- Tests work automatically
- Simpler CMake configuration

## Technical Details

### Why Recompiling Works

When the same source file is compiled into multiple targets, each target gets its own object file:
- `data_providers` library has `paths.cpp.o` (without macro)
- `SteamRot` executable has `paths.cpp.o` (with macro)

At link time, the linker resolves symbols. When SteamRot links:
1. It has its own `GetDataDirectory()` symbol from its recompiled `paths.cpp`
2. It also links to `data_providers` which has a different `GetDataDirectory()` symbol
3. The linker uses SteamRot's version for SteamRot's code (symbol precedence)
4. Result: SteamRot uses production paths

### Why Tests Work Automatically

Tests link to `data_providers` library which has the test-path version of `GetDataDirectory()`. Since tests don't recompile `paths.cpp`, they simply use the library's version.

## Usage Guidelines

### For Production Code (SteamRot)

The executable's CMakeLists.txt controls the path:
```cmake
add_executable(SteamRot main.cpp)
target_compile_definitions(SteamRot PRIVATE STEAMROT_ENV_DEBUG)
target_sources(SteamRot PRIVATE ${CMAKE_BINARY_DIR}/src/data_providers/paths.cpp)
```

### For Test Code

No special configuration needed! Tests automatically use test paths:
```cmake
add_executable(test_something test.cpp)
target_link_libraries(test_something PRIVATE data_providers)
# That's it - uses tests/data automatically
```

### To Change Environment

Edit `steamrot/CMakeLists.txt`:
```cmake
# For production/debug (data/ directory)
target_compile_definitions(SteamRot PRIVATE STEAMROT_ENV_DEBUG)

# For test environment (tests/data directory) - remove the line
# (no definition)
```

## References

- `steamrot/CMakeLists.txt` - SteamRot executable configuration
- `src/data_providers/paths.cpp.in` - Path function implementation
- `src/data_providers/paths.h` - Path function declarations
