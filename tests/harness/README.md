# Test Harness

## Overview

The test harness provides a unified, simplified interface for:
1. Loading test data configurations for data-driven testing with Catch2 generators
2. Creating and configuring TestFixture instances from test data
3. Running comparison tests between expected and actual entity states

This consolidates functionality for resource-based testing and data-driven test execution.

## Purpose

- Provide a single, simple API for loading test data
- Integrate TestFixture for resource setup and management
- Enable data-driven testing with automatic fixture creation
- Work seamlessly with Catch2 generators and matchers
- Keep the interface minimal and easy to use

## Directory Structure

```
harness/
├── test_data_harness.h           # Unified API for loading test data
├── test_data_harness.cpp         # Implementation
├── test_data_harness.test.cpp    # Unit tests
├── TestFixture.h                 # Resource management for tests
├── TestFixture.cpp               # Implementation
├── CMakeLists.txt                # Build configuration
├── README.md                     # This file
└── data/                         # Sample test data files
    ├── sample_test_1.test_data.json
    ├── sample_test_2.test_data.json
    └── sample_test_3.test_data.json
```

## Usage

### TestFixture Integration (Recommended)

The main wrapper function for data-driven testing with TestFixture:

```cpp
#include "test_data_harness.h"
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("Data-driven test with TestFixture", "[unit][my_component]") {
  // Load test configurations
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  // Use Catch2 generator to iterate through configs
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // Create fixture, configure entities, and run comparisons
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

This pattern:
- Loads test data from adjacent `data/` directory
- Creates a TestFixture for each configuration
- Configures entities from `start_entity_collection`
- Compares with `expected_entity_collection` automatically
- Ready for future simulation functionality

### Manual Fixture Creation

For more control, create the fixture manually:

```cpp
#include "test_data_harness.h"

TEST_CASE("Manual fixture creation", "[unit]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = configs.value()[0];
  
  // Create and configure fixture from test data
  auto fixture_result = steamrot::tests::create_fixture_from_test_data(config);
  REQUIRE(fixture_result.has_value());
  
  auto &fixture = fixture_result.value();
  
  // Access resources and entity manager
  auto &entity_mgr = fixture.GetEntityManager();
  auto &game_resources = fixture.GetGameResources();
  // ... test logic ...
}
```

### Basic Usage - Adjacent Data Directory

The simplest and recommended approach - load test data from an adjacent `data/` directory:

```cpp
#include "test_data_harness.h"
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("Parameterized test with test data", "[unit][my_component]") {
  // One simple call to load all test data
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  // Use with Catch2 generator
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // Process the test data
  REQUIRE(config->metadata() != nullptr);
  // ... test logic ...
}
```

### Loading from Specific Subdirectory

When you need to load test data from a specific test directory:

```cpp
#include "test_data_harness.h"

TEST_CASE("Load from subdirectory", "[unit]") {
  // Load from tests/entity/data/
  auto configs = steamrot::tests::load_test_data_configs("entity");
  REQUIRE(configs.has_value());
  
  for (const auto *config : configs.value()) {
    // Process each config
  }
}
```

### Using the Top-Level Wrapper

The `run_test_data_config()` wrapper validates that test data is properly structured:

```cpp
#include "test_data_harness.h"

TEST_CASE("Validate test data configuration", "[unit]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // Validate the configuration is well-formed
  auto result = steamrot::tests::run_test_data_config(config);
  REQUIRE(result.has_value());
  
  INFO("Test name: " << config->metadata()->test_name()->str());
}
```

### Running Entity Memory Pool Comparison Tests

The `run_entity_memory_pool_comparison_test()` function compares two EntityMemoryPool instances directly:

```cpp
#include "test_data_harness.h"

TEST_CASE("Entity memory pool comparison test", "[unit]") {
  // Create and configure pools
  EntityMemoryPool actual_pool;
  EntityMemoryPool expected_pool;
  
  // ... configure pools, simulate logic, etc ...
  
  // Compare pools using matcher
  steamrot::tests::run_entity_memory_pool_comparison_test(actual_pool, expected_pool);
}
```

## API

### `load_test_data_configs()`

Loads all test data from the adjacent `data/` directory (determined via `__FILE__` macro).

**Parameters:** None

**Returns:** `std::expected<std::vector<const TestDataConfig *>, FailInfo>`

**Example:**
```cpp
auto configs = steamrot::tests::load_test_data_configs();
```

### `load_test_data_configs(subdirectory)`

Loads all test data from `tests/<subdirectory>/data/` directory.

**Parameters:**
- `subdirectory`: Test subdirectory name (e.g., "entity", "components")

**Returns:** `std::expected<std::vector<const TestDataConfig *>, FailInfo>`

**Example:**
```cpp
auto configs = steamrot::tests::load_test_data_configs("entity");
```

### `run_test_data_config(config)`

Top-level wrapper that runs tests based on TestDataConfig contents. This examines the configuration and validates that the appropriate data is present for testing.

**Parameters:**
- `config`: Pointer to TestDataConfig object

**Returns:** `std::expected<std::monostate, FailInfo>`

**Example:**
```cpp
auto configs = steamrot::tests::load_test_data_configs();
REQUIRE(configs.has_value());

const auto *config = configs.value()[0];
auto result = steamrot::tests::run_test_data_config(config);
REQUIRE(result.has_value());
```

**Supported Data Types:**
- Entity Memory Pool comparisons (`start_entity_collection` + `expected_entity_collection`)
- Future: Event sequences, UI configurations, Logic tests, etc.

**Note:** This wrapper performs validation only. For actual test execution, use specialized test functions like `run_entity_memory_pool_comparison_test()`.

### `run_entity_memory_pool_comparison_test(actual, expected)`

Compares two EntityMemoryPool instances using the EqualsEntityMemoryPool matcher.

**Parameters:**
- `actual`: The actual EntityMemoryPool to test
- `expected`: The expected EntityMemoryPool to compare against

**Returns:** None (uses Catch2 assertions internally)

**Example:**
```cpp
EntityMemoryPool actual_pool;
EntityMemoryPool expected_pool;

// Configure pools, simulate logic, etc.
// ...

steamrot::tests::run_entity_memory_pool_comparison_test(actual_pool, expected_pool);
```

**Behavior:**
- Compares pools using `EqualsEntityMemoryPool` matcher
- Provides detailed error messages on mismatch
- Allows pools to be instantiated and manipulated before comparison (e.g., simulating logic)

### `create_fixture_from_test_data(config, scene_type)`

Creates and configures a TestFixture from test data configuration.

**Parameters:**
- `config`: Pointer to TestDataConfig containing entity setup
- `scene_type`: Scene type for the fixture (default: SceneType_TEST)

**Returns:** `std::expected<TestFixture, FailInfo>`

**Example:**
```cpp
auto configs = steamrot::tests::load_test_data_configs();
const auto *config = configs.value()[0];

auto fixture_result = steamrot::tests::create_fixture_from_test_data(config);
REQUIRE(fixture_result.has_value());

auto &fixture = fixture_result.value();
// Use fixture for testing
```

**Behavior:**
- Creates TestFixture with specified scene type
- Initializes game and scene resources
- Configures entities from `start_entity_collection` if present
- Generates archetypes automatically

### `run_fixture_test(config)`

Wrapper function for data-driven testing with TestFixture and Catch2 generators.

**Parameters:**
- `config`: Test data configuration

**Returns:** `std::expected<std::monostate, FailInfo>`

**Example:**
```cpp
TEST_CASE("Data-driven test with fixture", "[unit]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

**Behavior:**
1. Creates TestFixture from test data configuration
2. Configures entities from `start_entity_collection`
3. If `expected_entity_collection` is present, compares entity states automatically
4. Leaves room for future simulation functionality

**This is the main wrapper function for data-driven testing with TestFixture.**

## Integration with Matchers

The test harness is designed to work seamlessly with the existing matcher infrastructure in `tests/matchers/`. The `run_entity_memory_pool_comparison_test()` function uses the `EqualsEntityMemoryPool` matcher internally:

```cpp
#include "test_data_harness.h"

TEST_CASE("Test entity pools with matcher", "[unit]") {
  EntityMemoryPool actual_pool;
  EntityMemoryPool expected_pool;
  
  // Configure and manipulate pools...
  
  // Automatically uses EqualsEntityMemoryPool matcher for comparison
  steamrot::tests::run_entity_memory_pool_comparison_test(actual_pool, expected_pool);
}
```

## Key Features

- **Simple API**: One function call to load test data
- **Automatic Discovery**: Finds and loads all .test_data.bin files
- **Error Handling**: Uses `std::expected` for graceful error handling
- **Generator-Friendly**: Returns vectors suitable for Catch2's `from_range()`
- **Flexible**: Works with adjacent directories or specific subdirectories
- **Type-Safe**: Returns strongly-typed TestDataConfig pointers

## Migration Guide

### From test_data_generator functions

Old:
```cpp
auto test_names = steamrot::tests::get_test_names_for_generator();
auto test_name = GENERATE_COPY(from_range(test_names.value()));
steamrot::tests::TestDataLoader loader;
auto config = loader.LoadTestData(test_name, "subdirectory");
```

New:
```cpp
auto configs = steamrot::tests::load_test_data_configs();
const auto *config = GENERATE_COPY(from_range(configs.value()));
```

### From TestDataLoader

Old:
```cpp
steamrot::tests::TestDataLoader loader;
auto test_names = loader.DiscoverTestDataFiles("subdirectory");
auto configs = loader.LoadMultipleTestData(test_names.value(), "subdirectory");
```

New:
```cpp
auto configs = steamrot::tests::load_test_data_configs("subdirectory");
```

## Notes

- The harness consolidates functionality from `TestDataLoader` and `test_data_generator`
- Prioritizes simplicity - one call to get all test data
- Works with adjacent `data/` directory by default (via `__FILE__`)
- Seamlessly integrates with Catch2 generators and existing matchers
- Uses free functions following project conventions

## Test Data File Format

Test data JSON files must follow the `test_data.fbs` schema located in `src/flatbuffers_headers/test_data.fbs`.

**Required fields:**
- `metadata` (required) - Test metadata including `test_name`, `description`, `tags`, etc.

**Entity collection fields:**
- `start_entity_collection` - Starting state for comparison tests
- `expected_entity_collection` - Expected state for comparison tests

**Example:**
```json
{
  "metadata": {
    "test_name": "my_test",
    "description": "Test description",
    "tags": ["unit"],
    "expected_to_pass": true,
    "version": 1
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 5,
    "entities": [...]
  },
  "expected_entity_collection": {
    "entity_memory_pool_size": 5,
    "entities": [...]
  }
}
```

**Important:** The schema uses `start_entity_collection` and `expected_entity_collection`, NOT `entity_collection`. Always refer to `src/flatbuffers_headers/test_data.fbs` for the authoritative schema definition.
