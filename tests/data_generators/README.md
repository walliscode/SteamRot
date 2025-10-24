# Test Data Generators

## Overview

This directory contains utilities for data-driven testing with Catch2 generators. The functions here help discover and load test data files for use in parameterized tests.

## Purpose

To avoid cluttering individual unit test directories with generator-specific code, these utilities are centralized here. They provide a clean interface for:

1. Discovering `*.test_data.json` files in adjacent data directories
2. Loading them as `TestDataConfig` objects
3. Integrating them with Catch2's `GENERATE` functionality

## Directory Structure

```
data_generators/
├── TestDataGenerator.h        # Header with generator utility functions
├── TestDataGenerator.cpp      # Implementation
├── TestDataGenerator.test.cpp # Tests demonstrating usage
├── CMakeLists.txt             # Build configuration
├── README.md                  # This file
└── data/                      # Sample test data files
    ├── sample_test_1.test_data.json
    ├── sample_test_2.test_data.json
    └── sample_test_3.test_data.json
```

## Usage

### Basic Usage with Test Names

The simplest way to use the generator is to get test names and iterate through them:

```cpp
#include "TestDataGenerator.h"
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("My parameterized test", "[unit][my_component]") {
  // Get all test names from the subdirectory
  auto test_names = steamrot::tests::GetTestNamesForGenerator("my_component");
  REQUIRE(test_names.has_value());
  
  // Generate a test case for each test name
  auto test_name = GENERATE_COPY(from_range(test_names.value()));
  
  // Load the specific test data
  steamrot::tests::TestDataLoader loader;
  auto config = loader.LoadTestData(test_name, "my_component");
  REQUIRE(config.has_value());
  
  // Use the config in your test
  const auto *test_config = config.value();
  // ... test logic here ...
}
```

### Usage with TestDataConfig Objects

You can also load all configs upfront and iterate through them:

```cpp
#include "TestDataGenerator.h"
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("My test with configs", "[unit][my_component]") {
  // Load all test data configs from subdirectory
  auto configs = steamrot::tests::LoadTestDataForGenerator("my_component");
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
#include "TestDataGenerator.h"

TEST_CASE("Work with JSON files", "[unit]") {
  std::filesystem::path data_path = "/path/to/data";
  auto json_files = steamrot::tests::DiscoverTestDataJsonFiles(data_path);
  
  REQUIRE(json_files.has_value());
  for (const auto &file : json_files.value()) {
    // Process each JSON file
  }
}
```

## Functions

### `DiscoverTestDataJsonFiles(directory_path)`

Finds all `*.test_data.json` files in a directory.

**Parameters:**
- `directory_path`: Path to search for JSON files

**Returns:** `std::expected<std::vector<std::filesystem::path>, FailInfo>`

### `GetTestNamesForGenerator(subdirectory)`

Returns test names (without extensions) suitable for Catch2 generators.

**Parameters:**
- `subdirectory`: Test executable subdirectory (e.g., "entity", "components")

**Returns:** `std::expected<std::vector<std::string>, FailInfo>`

### `LoadTestDataForGenerator(subdirectory)`

Loads all TestDataConfig objects from a subdirectory.

**Parameters:**
- `subdirectory`: Test executable subdirectory (e.g., "entity", "components")

**Returns:** `std::expected<std::vector<const TestDataConfig *>, FailInfo>`

## Directory Parameter

The `subdirectory` parameter refers to the test executable directory:
- `"entity"` → looks in `tests/unit/entity/data/`
- `"components"` → looks in `tests/unit/components/data/`
- `"data_generators"` → looks in `tests/data_generators/data/`
- `""` (empty) → looks in `tests/data/` (backward compatibility)

## Test Data Files

Test data files must follow the naming convention: `<test_name>.test_data.json`

They are compiled to `.test_data.bin` during the build process. The generator functions work with the compiled binary files, but discover them based on the JSON filenames.

## Integration with Existing Code

These utilities build on top of:
- `TestDataLoader` from `tests/context/` - handles loading binary test data
- `TestDataConfig` FlatBuffers schema - defines test data structure
- Catch2 generators - provides parameterized test functionality

## Examples

See `TestDataGenerator.test.cpp` for comprehensive examples of all usage patterns.

## Notes

- This directory is separate from `tests/context/` to avoid cluttering the core test utilities
- The sample data files in `data/` demonstrate the expected JSON structure
- The functions handle errors gracefully using `std::expected`
- All functions work with both JSON discovery and binary loading seamlessly
