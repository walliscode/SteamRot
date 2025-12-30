# Path Configuration Solution

## Overview

This document explains the path configuration system in SteamRot and the solution to the asset loading issue.

## Problem Statement

The SteamRot executable was incorrectly loading assets from `tests/data/` instead of `data/` despite having `STEAMROT_ENV_DEBUG` defined.

## Root Cause

The macro `STEAMROT_ENV_DEBUG` was defined as `PRIVATE` in the SteamRot executable's CMakeLists.txt:

```cmake
# Old approach (BROKEN)
target_compile_definitions(SteamRot PRIVATE STEAMROT_ENV_DEBUG)
```

**Why this didn't work:**

1. The SteamRot executable has the macro defined
2. BUT the libraries (`data_providers`, `assets`) that actually include `paths.h` do NOT have it
3. `PRIVATE` definitions don't propagate to dependent libraries
4. Since `paths.h` uses inline functions, the path selection happens at compile time where the header is included
5. Libraries compiled without the macro → use default `tests/data`

## Solution: Interface Library

Created a centralized `environment_config` INTERFACE library:

```cmake
# src/CMakeLists.txt
add_library(environment_config INTERFACE)
target_compile_definitions(environment_config INTERFACE STEAMROT_ENV_DEBUG)
```

**Key Points:**

- `INTERFACE` means the definition propagates to all targets that link to it
- Defined once in `src/CMakeLists.txt` for all production code
- Tests do NOT link to `environment_config`, so they default to `tests/data`

## How It Works

### Production Code Flow

1. `data_providers` library links to `environment_config`
   ```cmake
   target_link_libraries(data_providers PUBLIC ... environment_config)
   ```

2. When `data_providers` compiles and includes `paths.h`, it has `STEAMROT_ENV_DEBUG` defined

3. `paths.h` inline function resolves to `data/`:
   ```cpp
   inline std::filesystem::path GetDataDirectory() {
   #if defined(STEAMROT_ENV_PROD) || defined(STEAMROT_ENV_DEBUG)
     return GetSourceDirectory() / "data";  // ← This branch
   #else
     return GetSourceDirectory() / "tests" / "data";
   #endif
   }
   ```

4. All production code that uses paths → `data/`

### Test Code Flow

1. Test executables do NOT link to `environment_config`

2. When test code includes `paths.h`, no macro is defined

3. `paths.h` inline function resolves to `tests/data/`:
   ```cpp
   inline std::filesystem::path GetDataDirectory() {
   #if defined(STEAMROT_ENV_PROD) || defined(STEAMROT_ENV_DEBUG)
     return GetSourceDirectory() / "data";
   #else
     return GetSourceDirectory() / "tests" / "data";  // ← This branch
   #endif
   }
   ```

4. All test code that uses paths → `tests/data/`

## Benefits

1. **Single Source of Truth**: Environment defined once in `src/CMakeLists.txt`
2. **Proper Propagation**: INTERFACE ensures all dependent targets get the definition
3. **Clear Separation**: Production vs test environments clearly separated
4. **Discoverable**: Anyone reading CMakeLists can see the configuration
5. **Maintainable**: Easy to change environment by modifying one line
6. **Type-Safe**: Compile-time path selection (no runtime overhead)

## Alternative Approaches Considered

### Option 1: Runtime Environment Selection

**Pros:**
- Could switch environments at runtime
- More flexible

**Cons:**
- Requires passing environment context everywhere
- Runtime overhead
- More complex API
- Harder to ensure consistent usage

**Verdict:** Not needed for current requirements. Compile-time selection is sufficient.

### Option 2: Separate Test Libraries

**Pros:**
- Complete separation of test and production code

**Cons:**
- Duplicated build artifacts
- Increased build time
- More complex CMake configuration

**Verdict:** Overkill for this problem. Interface library is cleaner.

## Usage Guidelines

### For Production Code

Just link to the appropriate libraries that already link to `environment_config`:

```cmake
target_link_libraries(my_production_target
  PRIVATE
  data_providers  # Already links to environment_config
  assets          # Already links to environment_config
)
```

### For Test Code

Do NOT link to `environment_config`:

```cmake
target_link_libraries(my_test_target
  PRIVATE
  Catch2::Catch2WithMain
  data_providers  # Gets the compiled library, but test doesn't get the macro
)
```

When test code includes `paths.h` directly, it will default to `tests/data`.

### To Change Environment

Edit `src/CMakeLists.txt`:

```cmake
# For production/debug (data/ directory)
target_compile_definitions(environment_config INTERFACE STEAMROT_ENV_DEBUG)

# For production only
target_compile_definitions(environment_config INTERFACE STEAMROT_ENV_PROD)

# For test environment (tests/data directory) - comment out the line or use nothing
# (no definition)
```

## Testing

Added `tests/unit/data_providers/paths.test.cpp` with test cases that verify:

1. Test builds use `tests/data` (no environment_config)
2. Path hierarchy is correct (assets within data, fonts within assets, etc.)
3. All path functions return valid paths

## Migration Notes

**Before:**
- Macro defined in multiple places
- PRIVATE definitions that didn't propagate
- Unclear which code used which paths
- Easy to misconfigure

**After:**
- Macro defined once in `src/CMakeLists.txt`
- INTERFACE library ensures propagation
- Clear separation: production vs test
- Hard to misconfigure (one place to change)

## References

- `src/CMakeLists.txt` - Environment configuration
- `src/data_providers/paths.h` - Path functions implementation
- `tests/unit/data_providers/paths.test.cpp` - Path behavior tests
