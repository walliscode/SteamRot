# Test Data System Documentation

## Overview

This document describes the SteamRot test data system, which provides a standardized way to define and load test data using JSON files and FlatBuffers. The system enables data-driven testing with reusable comparison utilities.

## Architecture

### Key Components

1. **FlatBuffers Schema** (`test_data.fbs`)
   - Defines the structure for test data
   - Uses unions to support multiple data levels (EntityMemoryPool, Scene, EventBus, Components)
   - Includes metadata for test identification and extensibility

2. **TestDataLoader** (`TestDataLoader.h/cpp`)
   - Loads test data from binary FlatBuffers files
   - Configures EntityMemoryPool from before/after/intermediate states
   - Provides access to test metadata

3. **Test Comparison Utilities** (`test_comparisons.h/cpp`)
   - Reusable comparison functions for components, pools, event buses, and archetypes
   - Returns detailed ComparisonResult with differences
   - Template-based for extensibility

## Test Data Structure

### TestDataFile (Root)

Each test data file contains:
- **version**: Schema version for backward compatibility
- **test_case**: The complete test case data

### TestCase

A test case includes:
- **metadata**: Test identification and configuration
- **before_state**: Initial state before test execution
- **after_state**: Expected state after test execution
- **intermediate_states**: Optional states at specific ticks (for multi-step tests)

### TestMetadata

Metadata includes:
- **test_name**: Human-readable test identifier (required)
- **test_description**: Optional description
- **tick**: Simulation tick for this state
- **test_tags**: Array of tags for categorization (e.g., "unit", "integration", "entity")
- **expected_result**: PASS, FAIL, or SKIP

### TestDataLevel (Union)

Supports different granularities of test data:
- **EntityMemoryPoolData**: Direct entity pool data
- **SceneData**: Scene-level data (multiple scenes possible)
- **EventBusData**: Event bus state for event system tests
- **ComponentData**: Component-specific test data

## Creating Test Data

### Step 1: Create JSON File

Create a JSON file in `tests/data/test_cases/` following this structure:

```json
{
  "version": 1,
  "test_case": {
    "metadata": {
      "test_name": "My Test Case",
      "test_description": "Description of what this test validates",
      "tick": 0,
      "test_tags": ["unit", "component"],
      "expected_result": "PASS"
    },
    "before_state": {
      "metadata": {
        "test_name": "Before State",
        "tick": 0,
        "expected_result": "PASS"
      },
      "data_type": "EntityMemoryPoolData",
      "data": {
        "entity_memory_pool_size": 10,
        "entities": [
          {
            "index": 0,
            "c_user_interface": {
              "ui_element_type": "UIElementType_BUTTON"
            }
          }
        ]
      }
    },
    "after_state": {
      "metadata": {
        "test_name": "After State",
        "tick": 1,
        "expected_result": "PASS"
      },
      "data_type": "EntityMemoryPoolData",
      "data": {
        "entity_memory_pool_size": 10,
        "entities": [
          {
            "index": 0,
            "c_user_interface": {
              "ui_element_type": "UIElementType_BUTTON"
            },
            "c_ui_state": {
              "is_hovered": true
            }
          }
        ]
      }
    }
  }
}
```

### Step 2: Convert to Binary

Use FlatBuffers' `flatc` tool to convert JSON to binary:

```bash
flatc --binary src/flatbuffers_headers/test_data.fbs tests/data/test_cases/my_test.json
```

This creates `my_test.bin` that can be loaded at test runtime.

### Step 3: Use in Tests

```cpp
#include "TestDataLoader.h"
#include "test_comparisons.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Load and verify test data", "[unit][test_data]") {
  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  steamrot::tests::TestDataLoader loader(path_provider);
  
  auto load_result = loader.LoadFromBinary("test_cases/my_test.bin");
  REQUIRE(load_result.has_value());
  
  // Get metadata
  const auto* metadata = loader.GetMetadata();
  REQUIRE(metadata != nullptr);
  REQUIRE(metadata->test_name()->str() == "My Test Case");
  
  // Configure before state
  steamrot::EntityMemoryPool before_pool;
  auto before_result = loader.ConfigurePoolFromBeforeState(before_pool);
  REQUIRE(before_result.has_value());
  
  // Run your test logic here...
  // Modify before_pool based on your system under test
  
  // Configure expected after state
  steamrot::EntityMemoryPool after_pool;
  auto after_result = loader.ConfigurePoolFromAfterState(after_pool);
  REQUIRE(after_result.has_value());
  
  // Compare results
  auto comparison = steamrot::tests::CompareEntityMemoryPools(before_pool, after_pool);
  REQUIRE(comparison.passed);
}
```

## Data Levels

### EntityMemoryPoolData

Use for tests that operate on entity pools:
- Component activation/deactivation
- Entity configuration
- Memory pool operations

### SceneData

Use for scene-level tests:
- Scene transitions
- Multiple entity sets
- Scene-specific logic

### EventBusData

Use for event system tests:
- Event generation
- Event handling
- Subscriber notifications

### ComponentData

Use for component-specific tests:
- Component value validation
- Component state changes
- Component relationships

## Comparison Utilities

### Component Comparisons

```cpp
// Compare individual components
auto result = steamrot::tests::CompareComponents(actual_component, expected_component);
REQUIRE(result.passed);
if (!result.passed) {
  std::cout << result.message << std::endl;
  for (const auto& diff : result.differences) {
    std::cout << "  - " << diff << std::endl;
  }
}
```

### Pool Comparisons

```cpp
// Compare entire entity memory pools
auto result = steamrot::tests::CompareEntityMemoryPools(actual_pool, expected_pool);
REQUIRE(result.passed);
```

### Event Bus Comparisons

```cpp
// Compare event buses
auto result = steamrot::tests::CompareEventBuses(actual_bus, expected_bus);
REQUIRE(result.passed);
```

### Archetype Manager Comparisons

```cpp
// Compare archetype structures
auto result = steamrot::tests::CompareArchetypeManagers(actual_manager, expected_manager);
REQUIRE(result.passed);
```

### Component Activation Comparisons

```cpp
// Verify component activation across entities
auto result = steamrot::tests::CompareComponentActivation<CUserInterface>(
    pool, entity_count, expected_active);
REQUIRE(result.passed);
```

## Extending the System

### Adding New Component Comparisons

To add comparison support for a new component type, add a template specialization in `test_comparisons.cpp`:

```cpp
template <>
ComparisonResult CompareComponents<CNewComponent>(
    const CNewComponent &actual, const CNewComponent &expected) {
  std::vector<std::string> differences;

  if (actual.m_active != expected.m_active) {
    std::ostringstream oss;
    oss << "m_active differs: actual=" << actual.m_active
        << ", expected=" << expected.m_active;
    differences.push_back(oss.str());
  }

  // Add more field comparisons...

  if (differences.empty()) {
    return ComparisonResult(true);
  }
  return CreateFailedComparison(differences);
}
```

Don't forget to add an explicit instantiation at the end of the file:

```cpp
template ComparisonResult CompareComponentsForEntities<CNewComponent>(
    const EntityMemoryPool &actual, const EntityMemoryPool &expected,
    const std::vector<size_t> &entity_ids);
```

### Adding New Data Levels

To add a new test data level:

1. Update `test_data.fbs` schema:
   ```fbs
   union TestDataLevel {
     // ... existing types ...
     NewDataType: NewDataTypeTable,
   }
   
   table NewDataTypeTable {
     // Define your data structure
   }
   ```

2. Regenerate FlatBuffers headers:
   ```bash
   cmake --build --preset Debug
   ```

3. Update `TestDataLoader::ConfigurePoolFromSnapshot()` to handle the new type:
   ```cpp
   case TestDataLevel_NewDataType: {
     const NewDataTypeTable* data = data_level->AsNewDataType();
     // Handle configuration...
   }
   ```

## Best Practices

1. **One Test Per File**: Each JSON file should contain exactly one test case
2. **Descriptive Names**: Use clear, descriptive test names and filenames
3. **Meaningful Tags**: Tag tests appropriately (unit, integration, component type, etc.)
4. **Minimal Data**: Include only the data needed for the test
5. **Document Expectations**: Use test_description to explain what's being validated
6. **Intermediate States**: Use for multi-step tests or tests validating behavior over time
7. **Version Control**: Include both JSON and binary files in version control
8. **Comparison Results**: Always check comparison results and log differences on failure

## Future Enhancements

Potential improvements to the system:

1. **Automatic JSON to Binary Conversion**: CMake script to auto-convert during build
2. **Test Data Validation**: Schema validation for JSON files before conversion
3. **Test Generator**: Tools to generate test data from running game state
4. **Diff Visualization**: Better visualization of comparison differences
5. **Performance Benchmarks**: Support for performance test data
6. **Test Data Versioning**: Migration tools for schema updates
7. **Parameterized Tests**: Support for test data variants

## Related Documentation

- See `TESTING_IMPROVEMENT_PLAN.md` for overall testing strategy
- See `README.md` for general testing guidelines
- See FlatBuffers documentation for schema syntax: https://flatbuffers.dev/
