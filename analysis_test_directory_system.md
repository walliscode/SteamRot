# Analysis: Test Directory System in SteamRot

## Executive Summary

**Yes, there is a robust system in place for directing tests to look in the test directory for data instead of the source directory.**

## System Overview

The SteamRot codebase has two distinct data directory systems:

1. **Production Data Directory**: `data/` (CMake variable: `data_dir`)
2. **Test Data Directory**: `tests/data/` (CMake variable: `test_data_dir`)

## How Tests Access Test Data

### Method 1: Adjacent `data/` Directory Pattern (Recommended)

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

### Method 2: Test Data Loader Harness

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

### Method 3: CMake-Configured Paths (for special cases)

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

1. **Isolation**: Tests don't accidentally modify production data
2. **Organization**: Test data is co-located with tests
3. **Flexibility**: Multiple patterns for different use cases
4. **Portability**: Uses relative paths, not absolute
5. **Discoverability**: Auto-discovery of test data files
6. **Type Safety**: FlatBuffers for structured test data

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

The SteamRot codebase has a well-designed, multi-layered system for test data management that completely separates test data from production data. Tests should use the adjacent `data/` directory pattern with the test data loader harness for maximum flexibility and maintainability.
