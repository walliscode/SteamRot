# Test Data Configuration System

## Overview

The Test Data Configuration System (Stage 3.1 of the Testing Improvement Plan) provides an extensible framework for creating data-driven tests using FlatBuffers. This system allows tests to be defined in JSON files and loaded at runtime, making it easy to add new test cases without modifying code.

## Key Features

- **Extensible Schema**: Easy to add new data types without breaking existing tests
- **Type-Safe**: Uses FlatBuffers for strongly-typed data
- **Discoverable**: Automatic discovery of test data files using naming convention
- **Metadata Support**: Built-in support for test metadata (name, description, tags, etc.)
- **Reusable**: TestDataLoader can be used across different test scenarios

## File Naming Convention

Test data files use the `.test_data.json` suffix for easy identification:

```
<test_name>.test_data.json  →  (compiled to)  →  <test_name>.test_data.bin
```

**Examples:**
- `entity_loading.test_data.json`
- `ui_collision.test_data.json`
- `logic_pipeline.test_data.json`

## Directory Structure

Test data files are organized in subdirectories matching the test structure:

```
tests/data/
├── examples/              # Example test data files
│   ├── example_entity_configuration.test_data.json
│   └── simple_metadata_only.test_data.json
├── components/            # Component-specific test data
├── entity/                # Entity-specific test data
├── logic/                 # Logic-specific test data
└── integration/           # Integration test data
```

## Schema Structure

The test data schema (`test_data.fbs`) defines two main tables:

### TestMetadata

Contains metadata about the test case:

```cpp
table TestMetadata {
  test_name: string (required);        // Human-readable test name
  description: string;                 // Optional test description
  tags: [string];                      // Test tags (e.g., "unit", "integration")
  expected_to_pass: bool = true;       // Expected outcome
  author: string;                      // Optional test author
  version: uint32 = 1;                 // Data format version
}
```

### TestDataConfig

Root table containing test data:

```cpp
table TestDataConfig {
  metadata: TestMetadata (required);   // Test metadata
  entity_collection: EntityCollection; // Optional entity data
  // More fields can be added here in the future
}
```

## Creating Test Data Files

### Minimal Example (Metadata Only)

```json
{
  "metadata": {
    "test_name": "simple_test",
    "description": "A simple test case",
    "tags": ["unit"],
    "expected_to_pass": true,
    "version": 1
  }
}
```

### Example with Entity Data

```json
{
  "metadata": {
    "test_name": "entity_configuration_test",
    "description": "Test entity configuration with UI components",
    "tags": ["unit", "entity"],
    "expected_to_pass": true,
    "version": 1
  },
  "entity_collection": {
    "entity_memory_pool_size": 10,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "position": { "x": 100.0, "y": 200.0 },
          "size": { "x": 50.0, "y": 30.0 },
          "visible": true
        }
      }
    ]
  }
}
```

## Compilation Process

Test data JSON files are automatically compiled to binary during the build process:

1. **Discovery**: CMake finds all `.test_data.json` files in `tests/data/`
2. **Compilation**: `flatc` compiles each JSON to `.test_data.bin`
3. **Integration**: Compiled binaries are available for tests at runtime

The compilation is handled automatically by `cmake/CompileTestData.cmake`.

## Using TestDataLoader

### Basic Usage

```cpp
#include "TestDataLoader.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Load test data", "[unit][data-driven]") {
  steamrot::tests::TestDataLoader loader;
  
  // Load a specific test data file
  auto result = loader.LoadTestData("example_entity_configuration", "examples");
  
  REQUIRE(result.has_value());
  const auto* config = result.value();
  
  // Access metadata
  REQUIRE(config->metadata() != nullptr);
  REQUIRE(config->metadata()->test_name()->str() == "example_entity_configuration");
  
  // Access entity data if present
  if (config->entity_collection()) {
    // Process entity data
  }
}
```

### Discovering Available Test Data

```cpp
TEST_CASE("Discover test data files", "[unit][data-driven]") {
  steamrot::tests::TestDataLoader loader;
  
  // Find all test data files in a subdirectory
  auto result = loader.DiscoverTestDataFiles("examples");
  
  REQUIRE(result.has_value());
  const auto& test_names = result.value();
  
  // Use discovered test names
  for (const auto& name : test_names) {
    // Load and process each test
  }
}
```

### Loading Multiple Test Data Files

```cpp
TEST_CASE("Load multiple test data", "[unit][data-driven]") {
  steamrot::tests::TestDataLoader loader;
  
  std::vector<std::string> test_names = {
    "test_case_1",
    "test_case_2",
    "test_case_3"
  };
  
  auto result = loader.LoadMultipleTestData(test_names, "components");
  
  REQUIRE(result.has_value());
  const auto& configs = result.value();
  
  for (const auto* config : configs) {
    // Process each test configuration
  }
}
```

### Using with Catch2 Generators

```cpp
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("Data-driven test with generator", "[unit][data-driven]") {
  steamrot::tests::TestDataLoader loader;
  
  // Discover all test data
  auto discovery_result = loader.DiscoverTestDataFiles("components");
  REQUIRE(discovery_result.has_value());
  
  auto test_name = GENERATE_COPY(from_range(discovery_result.value()));
  
  SECTION(test_name) {
    auto config_result = loader.LoadTestData(test_name, "components");
    REQUIRE(config_result.has_value());
    
    const auto* config = config_result.value();
    // Run test with this configuration
  }
}
```

## Adding New Data Types

The schema is designed to be extensible. To add a new data type:

### 1. Create the FlatBuffers Schema

```fbs
// new_data_type.fbs
namespace steamrot;

table NewDataType {
  field1: string;
  field2: int;
}
```

### 2. Update test_data.fbs

```fbs
include "new_data_type.fbs";

table TestDataConfig {
  metadata: TestMetadata (required);
  entity_collection: EntityCollection;
  new_data: NewDataType;  // Add new field
}
```

### 3. Rebuild

```bash
cmake --build --preset Debug
```

### 4. Use in Test Data JSON

```json
{
  "metadata": {
    "test_name": "test_with_new_data",
    "version": 1
  },
  "new_data": {
    "field1": "value",
    "field2": 42
  }
}
```

## Best Practices

### 1. Organize by Test Category

Place test data files in subdirectories matching the test structure:
- `tests/data/components/` for component tests
- `tests/data/logic/` for logic tests
- `tests/data/integration/` for integration tests

### 2. Use Descriptive Names

Name test data files clearly to indicate what they test:
- ✅ `entity_with_ui_and_grimoire.test_data.json`
- ✅ `collision_edge_case.test_data.json`
- ❌ `test1.test_data.json`
- ❌ `data.test_data.json`

### 3. Include Metadata

Always fill out the metadata fields:
- `test_name`: Clear, descriptive name
- `description`: What the test validates
- `tags`: Categorization for filtering
- `expected_to_pass`: Expected outcome

### 4. Version Your Data

Use the `version` field to track data format changes:
- Increment when making breaking changes
- Use in TestDataLoader to handle migrations

### 5. Keep Data Focused

Each test data file should test one specific scenario:
- ✅ One edge case per file
- ✅ One workflow per file
- ❌ Multiple unrelated scenarios in one file

### 6. Reuse Existing Types

Leverage existing FlatBuffers tables (EntityCollection, etc.) instead of duplicating data structures.

## Example Workflow

### Adding a New Component Test

1. **Create Test Data JSON**

```bash
# tests/data/components/new_component_default_values.test_data.json
{
  "metadata": {
    "test_name": "new_component_default_values",
    "description": "Verify NewComponent has correct default values",
    "tags": ["unit", "component", "NewComponent"],
    "expected_to_pass": true,
    "version": 1
  },
  "entity_collection": {
    "entity_memory_pool_size": 1,
    "entities": [
      {
        "index": 0,
        "c_new_component": {
          "value": 42
        }
      }
    ]
  }
}
```

2. **Build Project** (compiles JSON to binary)

```bash
cmake --build --preset Debug
```

3. **Write Test**

```cpp
TEST_CASE("NewComponent default values", "[unit][NewComponent][data-driven]") {
  steamrot::tests::TestDataLoader loader;
  auto result = loader.LoadTestData("new_component_default_values", "components");
  
  REQUIRE(result.has_value());
  const auto* config = result.value();
  
  // Load entity from test data
  // Verify component values
}
```

4. **Run Test**

```bash
ctest --preset Debug -R NewComponent
```

## Future Enhancements

The system is designed to grow. Future additions might include:

- **Event Data**: Test event sequences
- **UI Data**: Test UI configurations
- **Logic Data**: Test logic execution scenarios
- **Comparison Data**: Expected vs actual results
- **Simulation Data**: Multi-step workflow definitions

All can be added by extending `TestDataConfig` with new optional fields.

## Troubleshooting

### File Not Found Error

**Problem**: `Test data file not found`

**Solution**: 
- Check file naming: must end with `.test_data.json`
- Check file location: should be in `tests/data/<subdirectory>/`
- Rebuild project to ensure binary was generated

### Parse Error

**Problem**: `Failed to parse test data`

**Solution**:
- Validate JSON syntax
- Ensure all required fields are present (especially `metadata`)
- Check field types match schema

### Schema Not Found

**Problem**: Compilation fails with schema errors

**Solution**:
- Ensure `test_data.fbs` is in `src/flatbuffers_headers/`
- Verify `test_data.fbs` is listed in `generate_flatbuffers_headers.cmake`
- Rebuild to generate headers

## References

- **Testing Improvement Plan**: `documentation/TESTING_IMPROVEMENT_PLAN.md`
- **FlatBuffers Schema**: `src/flatbuffers_headers/test_data.fbs`
- **Example Files**: `tests/data/examples/`
- **TestDataLoader**: `tests/context/TestDataLoader.h`
