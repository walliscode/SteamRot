# Test Data Configuration System Examples

This directory contains example code demonstrating how to use the Test Data Configuration System.

## Files in This Directory

### TestDataLoader_examples.cpp

Comprehensive examples showing all features of the TestDataLoader:

1. **Basic Loading** - Load a single test data file
2. **Discovery** - Automatically find all test data files in a directory
3. **Data-Driven Testing** - Use Catch2 generators for parameterized tests
4. **Batch Loading** - Load multiple test data files at once
5. **Metadata Usage** - Use test metadata to control test behavior
6. **Error Handling** - Handle missing files and errors gracefully
7. **Organization** - Work with subdirectory organization

### How to Use These Examples

These are demonstration files showing patterns and usage. To use them in your own tests:

1. Copy relevant patterns to your test files
2. Replace example data references with your actual test data
3. Adapt the validation logic to your specific needs

### Running These Examples

Note: These files are in `documentation/examples/` for reference only. They are not compiled as part of the test suite. To use the patterns:

1. Create your test data JSON files in `tests/<test_executable_dir>/data/`
2. Build the project to compile JSON to binary
3. Use TestDataLoader in your tests following these patterns

## Quick Start

### 1. Create Test Data JSON

```json
// tests/components/data/my_test.test_data.json
{
  "metadata": {
    "test_name": "my_test",
    "description": "My test description",
    "tags": ["unit"],
    "expected_to_pass": true,
    "version": 1
  }
}
```

### 2. Build Project

```bash
cmake --build --preset Debug
```

This compiles `my_test.test_data.json` → `my_test.test_data.bin`

### 3. Load in Test

```cpp
#include "TestDataLoader.h"

TEST_CASE("My test", "[unit]") {
  steamrot::tests::TestDataLoader loader;
  auto result = loader.LoadTestData("my_test", "components");
  REQUIRE(result.has_value());
  
  const auto* config = result.value();
  // Use config...
}
```

## See Also

- **Complete Documentation**: `documentation/TEST_DATA_CONFIGURATION.md`
- **Schema**: `src/flatbuffers_headers/test_data.fbs`
- **Example Data**: `tests/context/data/`
- **TestDataLoader**: `tests/context/TestDataLoader.h`
