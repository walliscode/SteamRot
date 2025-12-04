# Test Data Configuration System

[← Back to Documentation](../README.md) | [Testing Overview](TESTING_OVERVIEW.md) | [Test Data Naming Conventions](TEST_DATA_NAMING_CONVENTIONS.md)

## Overview

The Test Data Configuration System provides an extensible framework for creating data-driven tests using FlatBuffers. This system allows tests to be defined in JSON files and loaded at runtime, making it easy to add new test cases without modifying code.

For naming conventions and organization strategies, see [Test Data Naming Conventions](TEST_DATA_NAMING_CONVENTIONS.md).

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

Test data files are organized within each test executable's directory:

```
tests/
├── components/
│   ├── data/                           # Component test data
│   │   ├── component_test_1.test_data.json
│   │   └── component_test_2.test_data.json
│   ├── CMakeLists.txt
│   └── *.test.cpp
├── entity/
│   ├── data/                           # Entity test data
│   │   └── entity_test.test_data.json
│   ├── CMakeLists.txt
│   └── *.test.cpp
├── logic/
│   ├── data/                           # Logic test data
│   ├── CMakeLists.txt
│   └── *.test.cpp
├── context/
│   ├── data/                           # Test infrastructure examples
│   │   ├── example_entity_configuration.test_data.json
│   │   └── simple_metadata_only.test_data.json
│   ├── CMakeLists.txt
│   └── *.cpp
└── integration/
    ├── scene_change/
    │   ├── data/                       # Scene change test data
    │   ├── CMakeLists.txt
    │   └── *.test.cpp
    └── ...
```

Each test executable directory can have its own `data/` subdirectory containing test data files specific to those tests.

## Schema Structure

The test data schema (`test_data.fbs`) defines two main tables:

### TestMetadata

Contains metadata about the test case:

```cpp
table TestMetadata {
  test_name: string (required);        // Human-readable test name
  description: string;                 // Optional test description
  tags: [string];                      // Test tags (e.g., "unit", "integration")
  expected_to_pass: bool = true;       // Expected test outcome (default: true)
  author: string;                      // Optional test author
  version: uint32 = 1;                 // Data format version
}
```

#### Understanding expected_to_pass

The `expected_to_pass` field controls how the test harness validates comparison results:

- **`expected_to_pass: true` (default)**: The test expects the actual and expected entity pools to **match**. If they don't match, the test fails. This is the typical use case for validating correct behavior.

- **`expected_to_pass: false`**: The test expects the actual and expected entity pools to **NOT match**. If they do match when they shouldn't, the test fails. This is useful for:
  - Testing error detection mechanisms
  - Validating that certain operations produce different results
  - Negative testing scenarios

Example with mismatch expectation:
```json
{
  "metadata": {
    "test_name": "validation_detects_mismatch",
    "description": "Verify validation detects incorrect state",
    "expected_to_pass": false
  },
  "start_entity_collection": { /* initial state */ },
  "expected_entity_collection": { /* intentionally different state */ }
}
```

### TestDataConfig

Root table containing test data:

```cpp
table TestDataConfig {
  metadata: TestMetadata (required);             // Test metadata
  start_entity_collection: EntityCollection;     // Starting state for comparison tests
  expected_entity_collection: EntityCollection;  // Expected state for comparison tests
  game_resources: GameResourcesData;             // Resource data for test fixtures
  scene_resources: SceneResourcesData;           // Scene-specific resources
  simulation_data: SimulationData;               // Simulation steps to execute (NEW)
  // More fields can be added here in the future
}
```

**Note on entity collections:**
- `entity_collection`: Deprecated. Use for backward compatibility with single-pool tests.
- `start_entity_collection`: Starting state for comparison tests. If not provided, defaults to a default-constructed EntityMemoryPool.
- `expected_entity_collection`: Expected state for comparison tests. Required when using `start_entity_collection`.

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

### Example with Start and Expected Entity Collections (New Format)

For comparison tests where you want to test transformations or state changes:

```json
{
  "metadata": {
    "test_name": "entity_state_comparison_test",
    "description": "Test entity state transitions",
    "tags": ["unit", "entity", "comparison"],
    "expected_to_pass": true,
    "version": 1
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 5,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "test_ui",
          "start_visible": false
        }
      }
    ]
  },
  "expected_entity_collection": {
    "entity_memory_pool_size": 5,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "test_ui",
          "start_visible": true
        }
      }
    ]
  }
}
```

**Note:** If `start_entity_collection` is omitted, the test will use a default-constructed EntityMemoryPool as the starting state.

### Example with Mismatch Expectation (expected_to_pass: false)

For negative testing or validation testing where you expect entities to differ:

```json
{
  "metadata": {
    "test_name": "entity_mismatch_detection",
    "description": "Test that validation detects different entity states",
    "tags": ["unit", "entity", "validation"],
    "expected_to_pass": false,
    "version": 1
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 3,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "original_ui",
          "start_visible": true
        }
      }
    ]
  },
  "expected_entity_collection": {
    "entity_memory_pool_size": 3,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "modified_ui",
          "start_visible": false
        }
      }
    ]
  }
}
```

In this case, the test **expects** the pools to be different. If they match, the test will fail.

## Compilation Process

Test data JSON files are automatically compiled to binary during the build process:

1. **Discovery**: CMake recursively searches for `data/` subdirectories in each test executable directory and finds all `.test_data.json` files
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
  auto result = loader.LoadTestData("example_entity_configuration", "context");
  
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

### Using RunEMPComparisonTest Wrapper

For entity memory pool comparison tests, use the `RunEMPComparisonTest` wrapper function:

```cpp
#include "TestDataLoader.h"
#include "entity_test_helpers.h"
#include "FlatbuffersConfigurator.h"
#include "TestFixture.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Run EMP comparison from test data", "[unit][entity][data-driven]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  
  // Create configurator
  steamrot::FlatbuffersConfigurator configurator{
      fixture.GetGameContext().event_handler};
  
  // Load test data
  steamrot::tests::TestDataLoader loader;
  auto result = loader.LoadTestData("entity_state_comparison_test", "unit/entity");
  
  REQUIRE(result.has_value());
  const auto* config = result.value();
  
  // Run the comparison test - wrapper handles all the logic
  steamrot::tests::RunEMPComparisonTest(config, configurator);
}
```

The `RunEMPComparisonTest` wrapper function:
- Checks if `start_entity_collection` and `expected_entity_collection` are present
- Creates and configures both pools
- Runs the comparison using `CompareEntityMemoryPools`
- Falls back to old `entity_collection` behavior for backward compatibility
- Uses default-constructed pool if `start_entity_collection` is not provided

### EntityMemoryPool Matcher with Test Metadata

When using the `run_fixture_test` or `run_entity_memory_pool_comparison_test` functions with test data configurations, test metadata (test name and description) is automatically included in failure messages. This makes it easier to identify which test case failed when multiple tests are run with Catch2 generators.

**Automatic Usage (Recommended):**
```cpp
#include "test_data_harness.h"
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("Data-driven tests with automatic metadata", "[unit][data-driven]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // Test metadata from JSON is automatically included in failure messages
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

**Manual Usage:**
```cpp
#include "test_data_harness.h"

TEST_CASE("Manual metadata usage", "[unit]") {
  // ... create or load actual and expected pools ...
  
  // Pass metadata string manually
  std::string metadata = "Test: my_test, Description: Tests feature X";
  steamrot::tests::run_entity_memory_pool_comparison_test(
      actual_pool, expected_pool, metadata);
}
```

**Example Failure Message:**
When a comparison fails, the error message now includes the test metadata:
```
EntityMemoryPool mismatch [Test: pool_comparison_different_values, Description: Entity memory pool with different component values for comparison testing]: Pool sizes differ: actual =5, expected =3;
```

This enhancement is especially useful when:
- Running parameterized tests with Catch2 generators
- Debugging multiple test cases defined in JSON files
- Working with large test suites where identifying failures quickly is important

### Using run_test_data_config Top-Level Wrapper

The test harness provides a top-level `run_test_data_config()` wrapper that validates test configurations:

```cpp
#include "test_data_harness.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("Validate and run test configurations", "[unit][data-driven]") {
  // Load all test data from adjacent data/ directory
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  // Use with Catch2 generator for parameterized testing
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // Validate the configuration structure
  auto validation_result = steamrot::tests::run_test_data_config(config);
  REQUIRE(validation_result.has_value());
  
  INFO("Test name: " << config->metadata()->test_name()->str());
  
  // Dispatch to appropriate test functions based on data type
  if (config->start_entity_collection() && config->expected_entity_collection()) {
    // Run entity comparison tests
    // steamrot::tests::RunEMPComparisonTest(config, configurator);
  }
  
  // Future: handle other data types
  // if (config->event_data()) { ... }
  // if (config->ui_data()) { ... }
}
```

The `run_test_data_config()` wrapper:
- Validates that the config is not null
- Checks that required metadata is present
- Validates entity collection consistency
- Provides a single entry point for data-driven testing
- Is extensible for future data types


### Discovering Available Test Data

```cpp
TEST_CASE("Discover test data files", "[unit][data-driven]") {
  steamrot::tests::TestDataLoader loader;
  
  // Find all test data files in a subdirectory
  auto result = loader.DiscoverTestDataFiles("context");
  
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

Place test data files in `data/` subdirectories within each test executable directory:
- `tests/components/data/` for component tests
- `tests/logic/data/` for logic tests
- `tests/integration/scene_change/data/` for integration tests

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
- `expected_to_pass`: Set to `true` if test expects pools to match, `false` if expecting mismatch

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
# tests/components/data/new_component_default_values.test_data.json
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

## Simulation Data (NEW)

### Overview

The test harness now supports **simulation data** - a powerful feature for testing complex, multi-step scenarios. Simulations allow test data to specify which Logic classes or free functions to execute and in which order.

### What Are Simulations?

Simulations are sequences of execution steps that mimic the Scene systems organization:
- **Action** - UI actions, input processing
- **Movement** - Entity movement, physics
- **Render** - Drawing and rendering
- **Collision** - Collision detection

Each step can execute either:
- **Individual free functions** (e.g., `ProcessNestedUIActionsAndEvents`)
- **Entire Logic classes** (e.g., `UIActionLogic`)

### Simulation Schema

```fbs
// src/flatbuffers_headers/simulation.fbs

enum SimulationType : byte {
  Action, Movement, Render, Collision
}

enum ExecutionMode : byte {
  Function,    // Execute individual free function
  LogicClass   // Execute entire Logic class
}

table SimulationStep {
  simulation_type: SimulationType;
  execution_mode: ExecutionMode;
  function_type: FunctionType;        // Used if mode is Function
  logic_class_type: LogicClassType;   // Used if mode is LogicClass
  description: string;
}

table SimulationData {
  steps: [SimulationStep];
  description: string;
}
```

### Example Test Data with Simulation

```json
{
  "metadata": {
    "test_name": "ui_interaction_simulation",
    "description": "Test UI collision detection, rendering, and action processing",
    "tags": ["integration", "simulation", "ui"],
    "expected_to_pass": true,
    "version": 1
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 3,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "test_ui",
          "start_visible": true,
          "root_ui_element": { /* ... */ }
        }
      }
    ]
  },
  "simulation_data": {
    "description": "Simulate complete UI interaction workflow",
    "steps": [
      {
        "simulation_type": "Collision",
        "execution_mode": "LogicClass",
        "logic_class_type": "UICollisionLogic",
        "description": "Detect mouse collision with UI elements"
      },
      {
        "simulation_type": "Render",
        "execution_mode": "LogicClass",
        "logic_class_type": "UIRenderLogic",
        "description": "Render UI to scene texture"
      },
      {
        "simulation_type": "Action",
        "execution_mode": "Function",
        "function_type": "ProcessNestedUIActionsAndEvents",
        "description": "Process UI actions recursively"
      }
    ]
  },
  "expected_entity_collection": {
    "entity_memory_pool_size": 3,
    "entities": [ /* expected state after simulation */ ]
  }
}
```

### Using Simulations in Tests

Simulations are automatically executed by the test harness:

```cpp
#include "test_data_harness.h"
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("Run simulation from test data", "[integration][simulation]") {
  // Load test configurations
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  // Use Catch2 generator
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // If config has simulation_data, it will be executed automatically
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

The workflow is:
1. **Setup** - Entities configured from `start_entity_collection`
2. **Simulate** - Steps executed in order (if `simulation_data` present)
3. **Verify** - Compare with `expected_entity_collection`

### Available Logic Classes

- `UIActionLogic` - Process UI actions and events
- `UICollisionLogic` - Check UI collision with mouse
- `UIRenderLogic` - Render UI elements
- `UIStateLogic` - Update UI state
- `CraftingRenderLogic` - Render crafting UI

### Available Free Functions

- `ProcessUIActionsAndEvents` - Process UI actions for elements
- `ProcessNestedUIActionsAndEvents` - Process UI actions recursively
- `ProcessButtonElementActions` - Process button-specific actions
- `ProcessDropDownListElementActions` - Process dropdown actions

### Extending Simulations

To add new functions or Logic classes:

1. **Add to FlatBuffers enum** (`simulation.fbs`):
   ```fbs
   enum FunctionType : byte {
     // ... existing values ...
     MyNewFunction = 50,
   }
   ```

2. **Add dispatcher case** (`simulation_runner.cpp`):
   ```cpp
   case FunctionType::MyNewFunction: {
     MyNewFunction(scene_context);
     return std::monostate{};
   }
   ```

3. **Rebuild project** to regenerate headers

4. **Use in test data**:
   ```json
   {
     "simulation_type": "Action",
     "execution_mode": "Function",
     "function_type": "MyNewFunction"
   }
   ```

### Benefits

- **Data-Driven**: Test scenarios defined in JSON, no code changes needed
- **Flexible**: Mix function calls and Logic class execution
- **Organized**: Follows Scene systems structure (Action, Movement, Render, Collision)
- **Extensible**: Easy to add new functions and Logic classes
- **Reusable**: Same simulation can be used across multiple tests

## Future Enhancements

The system is designed to grow. Future additions might include:

- **Event Data**: Test event sequences
- **UI Data**: Test UI configurations
- **Comparison Data**: More sophisticated expected vs actual comparisons

All can be added by extending `TestDataConfig` with new optional fields.

## Troubleshooting

### File Not Found Error

**Problem**: `Test data file not found`

**Solution**: 
- Check file naming: must end with `.test_data.json`
- Check file location: should be in `tests/<test_executable_dir>/data/`
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

- **FlatBuffers Schema**: `src/flatbuffers_headers/test_data.fbs`
- **Example Files**: `tests/context/data/`
- **TestDataLoader**: `tests/context/TestDataLoader.h`
