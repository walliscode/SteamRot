# EntityMemoryPool Matcher with Test Metadata

## Overview

The `EntityMemoryPoolEqualsMatcher` has been enhanced to support optional test metadata that is included in failure messages. This makes it easier to identify which test case failed when running data-driven tests with Catch2 generators.

## Key Features

- **Automatic metadata extraction** from test data JSON files
- **Clear failure messages** that include test name and description
- **Backward compatible** - works with or without metadata
- **Seamless integration** with existing test harness functions

## Usage Examples

### 1. Automatic Metadata (Recommended)

When using `run_fixture_test()` with test data configurations, metadata is automatically extracted and included in failure messages:

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

### 2. Manual Metadata

You can also pass metadata manually when calling the comparison function directly:

```cpp
#include "test_data_harness.h"

TEST_CASE("Manual metadata usage", "[unit]") {
  steamrot::EntityMemoryPool actual_pool;
  steamrot::EntityMemoryPool expected_pool;
  
  // ... configure pools ...
  
  // Pass metadata string manually
  std::string metadata = "Test: my_test, Description: Tests feature X";
  steamrot::tests::run_entity_memory_pool_comparison_test(
      actual_pool, expected_pool, metadata);
}
```

### 3. Direct Matcher Usage

For fine-grained control, use the matcher directly:

```cpp
#include "entity_memory_pool_matchers.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Direct matcher with metadata", "[unit]") {
  steamrot::EntityMemoryPool actual_pool;
  steamrot::EntityMemoryPool expected_pool;
  
  // ... configure pools ...
  
  // Use matcher with metadata
  REQUIRE_THAT(actual_pool, 
      steamrot::tests::EqualsEntityMemoryPool(expected_pool, 
          "Test: pool_comparison, Description: Testing pool equality"));
}
```

### 4. Without Metadata (Backward Compatible)

The matcher still works without metadata for backward compatibility:

```cpp
#include "entity_memory_pool_matchers.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Matcher without metadata", "[unit]") {
  steamrot::EntityMemoryPool actual_pool;
  steamrot::EntityMemoryPool expected_pool;
  
  // ... configure pools ...
  
  // Works without metadata
  REQUIRE_THAT(actual_pool, 
      steamrot::tests::EqualsEntityMemoryPool(expected_pool));
}
```

## Example Failure Messages

### Without Metadata
```
EntityMemoryPool mismatch: Pool sizes differ: actual =5, expected =3;
```

### With Metadata
```
EntityMemoryPool mismatch [Test: pool_comparison_different_values, Description: Entity memory pool with different component values for comparison testing]: Pool sizes differ: actual =5, expected =3;
```

### With Component Mismatches
```
EntityMemoryPool mismatch [Test: ui_state_test]: CMeta mismatch at index 0: m_active differs: actual=true, expected=false; CUserInterface mismatch at index 1: ...
```

## Benefits

1. **Faster Debugging**: Immediately see which test case failed without searching through test files
2. **Better CI/CD Reports**: More informative build logs when tests fail
3. **Parameterized Testing**: Essential when running multiple test cases with Catch2 generators
4. **Documentation**: Failure messages include test descriptions from JSON files

## Test Data JSON Structure

Metadata is extracted from the `metadata` section of test data JSON files:

```json
{
  "metadata": {
    "test_name": "pool_comparison_different_values",
    "description": "Entity memory pool with different component values",
    "tags": ["unit", "entity", "memory_pool"],
    "expected_to_pass": true,
    "version": 1
  },
  "start_entity_collection": { ... },
  "expected_entity_collection": { ... }
}
```

The `test_name` and `description` fields are automatically extracted and formatted as:
```
Test: <test_name>, Description: <description>
```

## Implementation Details

### EntityMemoryPoolEqualsMatcher Class

The matcher has been enhanced with:
- `m_test_metadata` member variable to store metadata
- Constructor overload that accepts metadata string
- Modified `describe()` method that includes metadata in error messages

### Helper Functions

Two overloaded versions of `EqualsEntityMemoryPool()`:
```cpp
// Without metadata
EqualsEntityMemoryPool(const EntityMemoryPool &expected)

// With metadata
EqualsEntityMemoryPool(const EntityMemoryPool &expected, 
                       const std::string &test_metadata)
```

### Test Harness Integration

The `run_fixture_test()` function automatically:
1. Extracts `test_name` and `description` from test data config
2. Formats them into a metadata string
3. Passes the metadata to the comparison function

## Best Practices

1. **Use Automatic Metadata**: Prefer `run_fixture_test()` for data-driven tests
2. **Descriptive Test Names**: Use clear, descriptive names in JSON metadata
3. **Include Descriptions**: Add meaningful descriptions to help debug failures
4. **Consistent Format**: Keep test_name and description consistent across test files

## Related Documentation

- `documentation/TEST_DATA_CONFIGURATION.md` - Complete test data system documentation
- `.github/copilot-instructions.md` - Test Data Configuration System section
- `tests/matchers/entity_memory_pool_matchers.h` - Matcher implementation
- `tests/harness/test_data_harness.h` - Test harness functions
