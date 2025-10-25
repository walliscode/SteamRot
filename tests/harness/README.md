# Test Harness

## Overview

The test harness provides a unified, simplified interface for loading test data configurations for data-driven testing with Catch2 generators. This consolidates functionality previously spread across `TestDataLoader` and `test_data_generator`.

## Purpose

- Provide a single, simple API for loading test data
- Eliminate redundancy between different test data loading approaches
- Work seamlessly with Catch2 generators and matchers
- Keep the interface minimal and easy to use

## Directory Structure

```
harness/
├── test_data_harness.h           # Unified API for loading test data
├── test_data_harness.cpp         # Implementation
├── test_data_harness.test.cpp    # Unit tests
├── CMakeLists.txt                # Build configuration
├── README.md                     # This file
└── data/                         # Sample test data files
    ├── sample_test_1.test_data.json
    ├── sample_test_2.test_data.json
    └── sample_test_3.test_data.json
```

## Usage

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

## Integration with Matchers

The test harness is designed to work seamlessly with the existing matcher infrastructure in `tests/unit/matchers/`. Use matchers for assertions:

```cpp
#include "test_data_harness.h"
#include "entity_memory_pool_matchers.h"

TEST_CASE("Test with matchers", "[unit]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // Use matchers for assertions
  // ... configure entity pools from config ...
  // REQUIRE_THAT(actual_pool, EqualsEntityMemoryPool(expected_pool));
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
