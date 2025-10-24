# Test Data Generators

## Overview

This directory contains utilities for data-driven testing with Catch2 generators. The functions here help discover and load test data files from an adjacent `data/` directory for use in parameterized tests.

## Purpose

To avoid cluttering individual unit test directories with generator-specific code, these utilities are centralized here. They provide a clean interface for:

1. Discovering `*.test_data.json` files in the adjacent `data/` directory
2. Loading them as `TestDataConfig` objects
3. Integrating them with Catch2's `GENERATE` functionality

## Directory Structure

```
data_generators/
├── test_data_generator.h        # Header with generator utility functions
├── test_data_generator.cpp      # Implementation
├── test_data_generator.test.cpp # Tests demonstrating usage
├── examples.test.cpp            # Practical examples and patterns
├── CMakeLists.txt               # Build configuration
├── README.md                    # This file
└── data/                        # Adjacent data directory
    ├── sample_test_1.test_data.json
    ├── sample_test_2.test_data.json
    └── sample_test_3.test_data.json
```

## Usage

### Basic Usage with Test Names

The simplest way to use the generator is to get test names and iterate through them:

```cpp
#include "test_data_generator.h"
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("My parameterized test", "[unit][my_component]") {
  // Get all test names from the adjacent data directory
  auto test_names = steamrot::tests::get_test_names_for_generator();
  REQUIRE(test_names.has_value());
  
  // Generate a test case for each test name
  auto test_name = GENERATE_COPY(from_range(test_names.value()));
  
  // Load the specific test data
  steamrot::tests::TestDataLoader loader;
  auto config = loader.LoadTestData(test_name, "data_generators");
  REQUIRE(config.has_value());
  
  // Use the config in your test
  const auto *test_config = config.value();
  // ... test logic here ...
}
```

### Usage with TestDataConfig Objects

You can also load all configs upfront and iterate through them:

```cpp
#include "test_data_generator.h"
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("My test with configs", "[unit][my_component]") {
  // Load all test data configs from adjacent data directory
  auto configs = steamrot::tests::load_test_data_for_generator();
  REQUIRE(configs.has_value());
  
  // Generate a test case for each config
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // Use the config directly
  REQUIRE(config->metadata() != nullptr);
  // ... test logic here ...
}
```

### Discovering JSON Files

If you need to work with the raw JSON file paths:

```cpp
#include "test_data_generator.h"

TEST_CASE("Work with JSON files", "[unit]") {
  auto json_files = steamrot::tests::discover_test_data_json_files();
  
  REQUIRE(json_files.has_value());
  for (const auto &file : json_files.value()) {
    // Process each JSON file
  }
}
```

## Functions

### `discover_test_data_json_files()`

Finds all `*.test_data.json` files in the adjacent `data/` directory.

**Parameters:** None (uses adjacent data directory)

**Returns:** `std::expected<std::vector<std::filesystem::path>, FailInfo>`

### `get_test_names_for_generator()`

Returns test names (without extensions) suitable for Catch2 generators from the adjacent `data/` directory.

**Parameters:** None (uses adjacent data directory)

**Returns:** `std::expected<std::vector<std::string>, FailInfo>`

### `load_test_data_for_generator()`

Loads all TestDataConfig objects from the adjacent `data/` directory.

**Parameters:** None (uses adjacent data directory)

**Returns:** `std::expected<std::vector<const TestDataConfig *>, FailInfo>`

## Adjacent Data Directory

All functions automatically use the `data/` directory adjacent to the source files. The directory path is determined using the `__FILE__` macro. If the adjacent `data/` directory does not exist, the functions will return an error.

## Test Data Files

Test data files must follow the naming convention: `<test_name>.test_data.json`

They are compiled to `.test_data.bin` during the build process. The generator functions work with the compiled binary files through TestDataLoader.

## Integration with Existing Code

These utilities build on top of:
- `TestDataLoader` from `tests/context/` - handles loading binary test data
- `TestDataConfig` FlatBuffers schema - defines test data structure
- Catch2 generators - provides parameterized test functionality

## Examples

See `test_data_generator.test.cpp` for comprehensive unit tests of all functions.

See `examples.test.cpp` for practical usage patterns including:
- Validation tests with generators
- Integration with entity memory pool tests
- Filtering tests by metadata tags
- Organizing multiple test suites from the same data source
- Patterns for real-world test scenarios

## Notes

- This directory is separate from `tests/context/` to avoid cluttering the core test utilities
- The sample data files in `data/` demonstrate the expected JSON structure
- The functions handle errors gracefully using `std::expected`
- All functions automatically use the adjacent `data/` directory via `__FILE__` macro
- An error is returned if the adjacent `data/` directory does not exist
