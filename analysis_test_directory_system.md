# Analysis: Test Directory System in SteamRot

## Executive Summary

**Yes, there is a robust system in place for directing tests to look in the test directory for data instead of the source directory.**

The system uses **compile-time macros** to select between production data (`data/`) and test data (`tests/data/`).

## System Overview: The `paths` Library

### Compile-Time Environment Selection

The `steamrot::paths` namespace (`src/data_providers/paths.h`) provides a **compile-time configurable** path system:

**Three Environments:**
1. **Test (Default)**: Uses `tests/data/` directory
2. **Debug**: Uses `data/` directory (same as production)  
3. **Production**: Uses `data/` directory

**Selection via Preprocessor Macros:**
```cpp
// In paths.h
inline std::filesystem::path GetDataDirectory() {
#if defined(STEAMROT_ENV_PROD) || defined(STEAMROT_ENV_DEBUG)
  return GetSourceDirectory() / "data";          // Production/Debug: data/
#else
  return GetSourceDirectory() / "tests" / "data"; // Test (default): tests/data/
#endif
}
```

**How to Use:**
```cpp
// Default behavior (test environment) - no macro needed
#include "paths.h"
auto path = steamrot::paths::GetDataDirectory();  // Returns: tests/data

// Production build - define macro before including
#define STEAMROT_ENV_PROD
#include "paths.h"
auto path = steamrot::paths::GetDataDirectory();  // Returns: data/
```

### CMake Configuration

The source directory path is configured at CMake time via template substitution:

**Template file** (`src/data_providers/paths.cpp.in`):
```cpp
std::filesystem::path GetSourceDirectory() {
  static const std::filesystem::path source_dir{"@CMAKE_SOURCE_DIR@"};
  return source_dir;
}
```

CMake's `configure_file()` substitutes `@CMAKE_SOURCE_DIR@` with the actual build path.

### Available Path Functions

All paths are built on top of `GetDataDirectory()`:

```cpp
steamrot::paths::GetDataDirectory()              // Base: tests/data or data/
steamrot::paths::GetDefaultsDirectory()          // {data}/defaults
steamrot::paths::GetUserDirectory()              // {data}/user
steamrot::paths::GetAssetsDirectory()            // {data}/assets
steamrot::paths::GetSceneDirectory()             // {data}/defaults/scenes
steamrot::paths::GetUIStylesDirectory()          // {data}/ui_styles
steamrot::paths::GetFontsDirectory()             // {data}/assets/fonts
// ... and more
```

## Additional Data Directory Systems

The SteamRot codebase also has CMake variables for direct access:

1. **Production Data Directory**: `data/` (CMake variable: `data_dir`)
2. **Test Data Directory**: `tests/data/` (CMake variable: `test_data_dir`)

## How to Select Test vs Production Data

### Method 1: Use the `paths` Library (Recommended for Code)

**For application code that needs to work in both test and production:**

```cpp
#include "paths.h"

// No macro needed - defaults to test environment
auto scene_path = steamrot::paths::GetSceneDirectory();
// Returns: {source_dir}/tests/data/defaults/scenes (in tests)
// Returns: {source_dir}/data/defaults/scenes (in production builds)
```

**For production builds**, the build system defines the macro:
```cmake
# In CMakeLists.txt (for production)
target_compile_definitions(my_target PRIVATE STEAMROT_ENV_PROD)
```

### Method 2: Adjacent `data/` Directory Pattern (For Unit Tests)

Tests use the `__FILE__` macro to locate an adjacent `data/` directory:

```cpp
std::filesystem::path test_file_path = __FILE__;
std::filesystem::path data_dir = test_file_path.parent_path() / "data";
std::filesystem::path bin_file_path = data_dir / "entity_test_data.bin";
```

**Example from existing code** (`tests/unit/entity/FlatbuffersEntityConfigurator.test.cpp`):
- Test file: `tests/unit/entity/FlatbuffersEntityConfigurator.test.cpp`
- Data directory: `tests/unit/entity/data/`
- Data file: `tests/unit/entity/data/entity_test_data.bin`

This pattern ensures:
- Tests are self-contained with their data
- No hardcoded paths
- Works regardless of where CMake is run
- Test data is organized alongside test code
- Useful when you need test-specific fixtures that aren't part of the main data directories

### Method 3: Test Data Loader Harness

The project includes a sophisticated test data loading system (`tests/harness/test_data_loader.h`):

```cpp
#include "test_data_loader.h"

// Load all test data from adjacent data/ directory
auto configs = load_test_data_configs();

// Or load from specific path
auto configs = LoadTestDataConfigsFromPath("/path/to/test/data");
```

**Key features:**
- Auto-discovers all `.test_data.bin` files in a directory
- Uses `__FILE__` macro via `load_test_data_configs()` macro
- Supports explicit paths via `LoadTestDataConfigsFromPath()`
- Returns `std::expected` for error handling
- Complements the `paths` library for structured test data

### Method 4: CMake Variables (For Build System)

CMake sets project-wide variables:

```cmake
# In CMakeLists.txt
cmake_path(SET data_dir ${CMAKE_SOURCE_DIR}/data)
cmake_path(SET test_data_dir ${CMAKE_SOURCE_DIR}/tests/data)
```

These can be used in CMake template files:
```cpp
constexpr const char* TEST_DATA_DIR = "@TEST_DATA_DIR@";
auto configs = LoadTestDataConfigsFromPath(TEST_DATA_DIR);
```

## Key Concepts: Compile-Time vs Runtime Selection

### The `paths` Library: Compile-Time Selection

The `paths` library uses **preprocessor macros** to select directories at **compile time**:

**Advantages:**
- ✅ Zero runtime overhead (path is determined at compile time)
- ✅ No need to pass environment flags at runtime
- ✅ Single codebase works for both test and production
- ✅ Type-safe and inline functions

**How It Works:**
1. By default (no macros defined), code uses `tests/data/`
2. When `STEAMROT_ENV_PROD` or `STEAMROT_ENV_DEBUG` is defined, code uses `data/`
3. The macro is typically set via CMake build configuration
4. The same source code compiles to different paths based on build type

**Example:**
```cpp
// Same code, different behavior based on build configuration
#include "paths.h"

void LoadSceneData() {
  auto scene_dir = steamrot::paths::GetSceneDirectory();
  // Test build: scene_dir = "{source}/tests/data/defaults/scenes"
  // Prod build:  scene_dir = "{source}/data/defaults/scenes"
}
```

### When to Use Each Method

| Method | Use Case | Selection Time | Overhead |
|--------|----------|----------------|----------|
| `paths` library | Production code, data providers | Compile-time | None |
| Adjacent `data/` | Test-specific fixtures | Runtime | Minimal |
| Test data loader | Structured test data | Runtime | Minimal |
| CMake variables | Build system, templates | Configure-time | None |

## Directory Structure

```
SteamRot/
├── data/                          # Production data
│   ├── assets/
│   ├── defaults/
│   ├── ui_styles/
│   └── user/
├── tests/
│   ├── data/                      # Global test data
│   │   ├── assets/
│   │   ├── defaults/
│   │   ├── fragments/
│   │   ├── ui_styles/
│   │   └── user/
│   └── unit/
│       ├── entity/
│       │   ├── data/              # Entity test data
│       │   │   └── entity_test_data.bin
│       │   └── FlatbuffersEntityConfigurator.test.cpp
│       └── user_interface/
│           ├── data/              # UI test data
│           └── FlatbuffersUIElementConfigurator.test.cpp
```

## Best Practices for Test Data

### For New Tests:

1. **Create adjacent `data/` directory** next to your test file
2. **Use `__FILE__` pattern** to locate data:
   ```cpp
   std::filesystem::path data_dir = 
       std::filesystem::path(__FILE__).parent_path() / "data";
   ```
3. **Use `.test_data.bin` suffix** for test data files (auto-discovered by harness)
4. **Or use test data loader harness**:
   ```cpp
   auto configs = load_test_data_configs();
   ```

### For Scene/Entity Configuration Tests:

For tests that need to verify entity or scene configuration, use the test data loader:

```cpp
#include "test_data_loader.h"

TEST_CASE("Scene configuration test", "[unit][scene]") {
  // Loads all .test_data.bin files from adjacent data/ directory
  auto configs_result = load_test_data_configs();
  REQUIRE(configs_result.has_value());
  
  const auto& configs = configs_result.value();
  
  for (const auto* config : configs) {
    // Access start_entity_collection
    if (config->start_entity_collection()) {
      // Configure entities from start state
    }
    
    // Access expected_entity_collection
    if (config->expected_entity_collection()) {
      // Verify against expected state
    }
  }
}
```

## Why This System Exists

### Benefits of the `paths` Library Approach

1. **Compile-Time Selection**: Zero runtime overhead for path resolution
2. **Single Codebase**: Same code works for test, debug, and production
3. **Type Safety**: Compile-time errors if both PROD and DEBUG defined
4. **No Configuration Files**: Environment selected via build system
5. **Isolation**: Tests use separate data directory by default
6. **CMake Integration**: Path is configured once at CMake time

### Benefits of Additional Methods

1. **Isolation**: Tests don't accidentally modify production data
2. **Organization**: Test data is co-located with tests (adjacent `data/`)
3. **Flexibility**: Multiple patterns for different use cases
4. **Portability**: Uses relative paths, not absolute
5. **Discoverability**: Auto-discovery of test data files (test data loader)
6. **Type Safety**: FlatBuffers for structured test data

## Summary of Path Selection Methods

| What | How | When | Example |
|------|-----|------|---------|
| **`paths` library** | Preprocessor macros | Compile-time | `steamrot::paths::GetSceneDirectory()` |
| **Adjacent `data/`** | `__FILE__` macro | Runtime | `std::filesystem::path(__FILE__).parent_path() / "data"` |
| **Test data loader** | `load_test_data_configs()` | Runtime | `auto configs = load_test_data_configs()` |
| **CMake variables** | `configure_file()` | Configure-time | `constexpr const char* DATA_DIR = "@data_dir@"` |

## Recommendation

For the SceneConfigurator tests mentioned in the PR:

✅ **Use the test data loader harness** with adjacent `data/` directories:

```cpp
// In tests/unit/scenes/FlatbuffersSceneConfigurator.test.cpp

TEST_CASE("Scene configuration with entities", "[unit][scene]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  for (const auto* config : configs.value()) {
    // Use config->start_entity_collection() for initial state
    // Use config->expected_entity_collection() for verification
  }
}
```

Create test data in:
- `tests/unit/scenes/data/scene_config_001.test_data.json` (compiled to .bin)

This approach:
- ✅ Uses existing infrastructure
- ✅ Keeps test data organized
- ✅ Supports before/after state comparison
- ✅ Auto-discovered by harness
- ✅ No hardcoded paths

## Conclusion

The SteamRot codebase has a **sophisticated, multi-layered system** for data path management:

### Primary System: `paths` Library (Compile-Time)

The `steamrot::paths` namespace provides **compile-time environment selection**:
- **Test environment (default)**: Uses `tests/data/` 
- **Production/Debug**: Uses `data/` (via `STEAMROT_ENV_PROD` or `STEAMROT_ENV_DEBUG` macros)
- **Zero runtime overhead**: Path selection happens at compile time
- **CMake configured**: Source directory path substituted at configure time

### Supporting Systems: Runtime Test Data Access

For test-specific needs:
- **Adjacent `data/` directories**: Test fixtures co-located with test files
- **Test data loader harness**: Auto-discovery of `.test_data.bin` files
- **CMake variables**: Direct access for build system scripts

### Key Takeaway

The `paths` library answers your question: **Yes, there is a system that can compile with different data directories (`tests/data` or `data/`) depending on compiler options (preprocessor macros).** 

The system uses:
- Preprocessor conditionals (`#if defined(STEAMROT_ENV_PROD)`)
- CMake template substitution (`@CMAKE_SOURCE_DIR@`)
- Compile-time path selection (no runtime configuration needed)

This design allows the **same source code** to work seamlessly in both test and production environments, with the build system controlling which data directory is used.
