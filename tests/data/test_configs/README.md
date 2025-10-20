# Test Data Configuration System

## Overview

The test data configuration system provides a standardized way to define test cases using JSON files. This enables data-driven testing where test logic can be reused across multiple test cases without code duplication.

## Directory Structure

```
tests/data/test_configs/
├── test_data_schema.json          # JSON schema defining the structure
├── component_configs/             # Test data for component configuration
│   ├── cgrimoire_machina_test_data.json
│   ├── cuser_interface_test_data.json
│   └── cmeta_test_data.json
├── integration_configs/           # Test data for integration tests
│   └── entity_loading_test_data.json
├── ui_configs/                    # Test data for UI element tests
│   └── ui_element_factory_test_data.json
└── README.md                      # This file
```

## JSON Schema

Test data files follow the schema defined in `test_data_schema.json`. The schema ensures consistency and provides validation for test configurations.

### Schema Structure

```json
{
  "test_suite": "string (required)",
  "description": "string (optional)",
  "test_cases": [
    {
      "name": "string (required)",
      "description": "string (optional)",
      "tags": ["string", ...] (optional),
      "input": { ... } (required),
      "expected": { ... } (required),
      "should_fail": boolean (optional, default: false)
    }
  ]
}
```

### Field Descriptions

- **test_suite**: Name of the test suite (e.g., component name, feature name)
- **description**: Human-readable description of what the test suite validates
- **test_cases**: Array of individual test case configurations
  - **name**: Unique identifier for the test case
  - **description**: What this test case validates
  - **tags**: Categories for the test case (e.g., `happy_path`, `edge_case`, `error_case`)
  - **input**: Input data structure (depends on what is being tested)
  - **expected**: Expected output or state after processing the input
  - **should_fail**: If true, the test expects the operation to fail

## Example Test Data File

### Component Configuration Test Data

```json
{
  "test_suite": "CGrimoireMachina",
  "description": "Test data for CGrimoireMachina component configuration",
  "test_cases": [
    {
      "name": "default_configuration",
      "description": "Test with valid fragment and joint data",
      "tags": ["happy_path", "valid_input"],
      "input": {
        "c_grimoire_machina": {
          "fragments": ["fragment1", "fragment2"],
          "joints": ["joint1"]
        }
      },
      "expected": {
        "m_active": true,
        "m_all_fragments_count": 2,
        "m_all_joints_count": 1
      }
    }
  ]
}
```

## Common Test Tags

Use these standardized tags to categorize test cases:

- **happy_path**: Normal, expected usage scenarios
- **edge_case**: Boundary conditions and unusual but valid inputs
- **error_case**: Invalid inputs that should trigger errors
- **stress_test**: Large data sets or performance testing
- **integration**: Tests involving multiple components
- **regression**: Tests for previously fixed bugs
- **empty_data**: Tests with empty collections or strings
- **null_data**: Tests with null/missing data
- **duplicate_data**: Tests with duplicate entries
- **default_values**: Tests verifying default initialization

## Creating New Test Data Files

### 1. Choose a Location

- **Component tests**: `component_configs/` - For testing individual component configuration and behavior
- **Integration tests**: `integration_configs/` - For testing interactions between multiple components/systems
- **UI tests**: `ui_configs/` - For testing UI element creation, layout, and interaction
- **System tests**: Create new subdirectory as needed for end-to-end workflow tests

### 2. Follow Naming Convention

- Use snake_case: `component_name_test_data.json`
- Be descriptive: `ui_collision_rendering_test_data.json`

### 3. Define Test Cases

1. Start with happy path scenarios
2. Add edge cases (empty data, boundary conditions)
3. Add error cases (invalid data, should_fail: true)
4. Add stress tests if relevant (large datasets)

### 4. Use Consistent Structure

- Keep `input` structure consistent with FlatBuffers schema
- Make `expected` structure match the component's public members
- Use descriptive names and descriptions
- Tag appropriately for filtering

## Usage in Tests (Future Implementation)

Once the TestDataLoader is implemented (Section 3.2), test data will be loaded like this:

```cpp
TEST_CASE("Component configuration with test data", 
          "[unit][data-driven][CGrimoireMachina]") {
  steamrot::tests::TestDataLoader loader;
  auto test_cases = loader.LoadTestCases("cgrimoire_machina_test_data.json");
  
  auto test_case = GENERATE_COPY(from_range(test_cases));
  
  SECTION(test_case.name) {
    // Configure component with test_case.input
    // Verify against test_case.expected
  }
}
```

## Benefits

1. **Separation of Concerns**: Test data is separate from test logic
2. **Maintainability**: Easy to add new test cases without modifying code
3. **Readability**: JSON format is human-readable and easy to understand
4. **Reusability**: Same test logic can process multiple test cases
5. **Coverage**: Easy to expand test coverage by adding more test cases
6. **Documentation**: Test data serves as examples of valid configurations

## Validation

Test data files can be validated against the JSON schema using the provided validation script or standard tools:

### Using the Validation Script (Recommended)

```bash
# Validate all test data files
cd tests/data/test_configs
python3 validate_test_data.py

# Validate a specific directory
python3 validate_test_data.py component_configs/

# Validate a single file
python3 validate_test_data.py component_configs/cmeta_test_data.json
```

The validation script checks:
- Required fields (test_suite, test_cases, name, input, expected)
- Proper data types (strings, arrays, booleans, objects)
- Minimum array length requirements
- Optional field types

### Using Standard JSON Schema Validators

```bash
# Using jsonschema (Python)
jsonschema -i component_configs/cgrimoire_machina_test_data.json test_data_schema.json

# Using ajv-cli (Node.js)
ajv validate -s test_data_schema.json -d "component_configs/*.json"
```

## Next Steps

The following tasks are planned for future implementation:

1. **TestDataLoader** (Section 3.2): Load test cases from JSON files
2. **Catch2 Integration** (Section 3.3): Use GENERATE with loaded test data
3. **Test Data Generators** (Section 3.4): Generate random valid test data
4. **Convert Existing Tests**: Migrate existing tests to data-driven format

## Related Documentation

- [TESTING_IMPROVEMENT_PLAN.md](../../../documentation/TESTING_IMPROVEMENT_PLAN.md) - Overall testing strategy
- [test_data_schema.json](test_data_schema.json) - JSON schema definition
- Component test data examples in `component_configs/`
