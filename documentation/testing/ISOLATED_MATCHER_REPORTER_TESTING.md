# Isolated Matcher and Reporter Testing

## Overview

This document describes the pattern for testing matchers and reporters in isolation, without requiring failing test assertions. This approach allows direct testing of string generation and formatting logic.

## Motivation

Previously, testing matchers and reporters required:
- Intentionally creating mismatching data to trigger failures
- Relying on Catch2 test failures to see output
- Difficulty isolating string generation from test logic
- Hard to test specific formatting scenarios

The new isolated testing approach allows:
- Direct extraction of matcher `describe()` output
- Testing string generation without test failures
- Isolated validation of formatting logic
- Easier testing of edge cases and specific scenarios

## Matcher Testing Pattern

### Helper Utilities

The `matcher_test_helpers.h` file provides utilities for isolated matcher testing:

```cpp
#include "matcher_test_helpers.h"

// Test a matcher and extract both match result and description
auto result = steamrot::tests::TestMatcherOutput(actual_value, matcher_instance);

// Check if match succeeded
bool matched = result.matched;

// Extract description string
std::string description = result.description;
```

### Key Helper Functions

- **`TestMatcherOutput<T, Matcher>`**: Execute matcher and extract both match status and description
- **`GetMatcherDescription<T, Matcher>`**: Extract just the description string
- **`ContainsAllSubstrings`**: Validate that output contains expected substrings
- **`ContainsNoneOfSubstrings`**: Validate that output doesn't contain unwanted substrings
- **`CountSubstring`**: Count occurrences of a substring
- **`ValidateOutputStructure`**: Check for expected structural elements (dividers, PASSED/FAILED indicators)

### Example: Testing CMeta Matcher

```cpp
TEST_CASE("CMeta matcher describe() output for matching components",
          "[unit][matchers][CMeta][isolated]") {
  
  // Setup expected and actual components
  steamrot::CMeta expected;
  expected.m_active = true;
  
  steamrot::CMeta actual;
  actual.m_active = true;
  
  // Create matcher and test in isolation
  steamrot::tests::CMetaEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  // Validate match succeeded
  REQUIRE(result.matched);
  
  // Validate description format
  REQUIRE(result.description.find("PASSED") != std::string::npos);
  REQUIRE(result.description.find("CMeta") != std::string::npos);
}
```

### Example: Testing Mismatch Output

```cpp
TEST_CASE("CMeta matcher describe() output for field mismatch",
          "[unit][matchers][CMeta][isolated]") {
  
  steamrot::CMeta expected;
  expected.m_active = true;
  
  steamrot::CMeta actual;
  actual.m_active = false;  // Intentional mismatch
  
  steamrot::tests::CMetaEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  // Match should fail
  REQUIRE_FALSE(result.matched);
  
  // Description should contain field name and values
  REQUIRE(result.description.find("m_active") != std::string::npos);
  REQUIRE(result.description.find("actual") != std::string::npos);
  REQUIRE(result.description.find("expected") != std::string::npos);
}
```

## Reporter Testing Pattern

### Helper Utilities

The `reporter_test_helpers.h` file provides utilities for isolated reporter testing:

```cpp
#include "reporter_test_helpers.h"

// Capture output during reporter method calls
steamrot::tests::StreamCapture capture(std::cout);

// Call reporter method
reporter.testCaseStarting(test_info);

// Extract captured output
std::string output = capture.GetCapturedOutput();
```

### Key Helper Functions

- **`StreamCapture`**: RAII class to capture output stream during scope
- **`ValidateReporterOutput`**: Check that output contains expected elements
- **`CountLines`**: Count number of lines in output
- **`ValidateReporterStructure`**: Validate structural elements (dividers, colors)
- **`ExtractLinesContaining`**: Extract all lines containing a specific substring

### Example: Testing HarnessReporter

```cpp
TEST_CASE("HarnessReporter testCaseStarting output format",
          "[unit][reporters][HarnessReporter][isolated]") {
  
  // Create mock test case info
  Catch::TestCaseInfo test_info(
      "test_case_name",
      {"[tag1]", "[tag2]"},
      Catch::SourceLineInfo("test_file.cpp", 42)
  );
  
  // Capture output
  steamrot::tests::StreamCapture capture(std::cout);
  
  Catch::ReporterConfig config(Catch::ConfigData{});
  HarnessReporter reporter(std::move(config));
  
  reporter.testCaseStarting(test_info);
  
  std::string output = capture.GetCapturedOutput();
  
  // Validate output contains expected elements
  REQUIRE(output.find("test_case_name") != std::string::npos);
  REQUIRE(output.find("test_file.cpp") != std::string::npos);
  REQUIRE(output.find("42") != std::string::npos);
  REQUIRE(output.find("---") != std::string::npos);  // Divider
}
```

## Testing Strategy

### What to Test

For **Matchers**, test:
1. **Match success** - Output format when match succeeds
2. **Match failure** - Output format when match fails
3. **Field-specific mismatches** - Each field that can mismatch
4. **Multiple mismatches** - Output when multiple fields differ
5. **Structure consistency** - Dividers, headers, formatting
6. **Helper functions** - Any utility functions (e.g., `EqualsCMeta`)

For **Reporters**, test:
1. **Method outputs** - Each reporter method's string output
2. **Format consistency** - Consistent formatting across calls
3. **Required elements** - All expected information is present
4. **Structure** - Dividers, headers, indentation
5. **Edge cases** - Empty strings, special characters, etc.

### Organizing Tests

- **File naming**: `{component}_isolated.test.cpp`
  - Examples: `cmeta_matchers_isolated.test.cpp`, `HarnessReporter_isolated.test.cpp`

- **Test tags**: Always include `[isolated]` tag
  - Full tag example: `[unit][matchers][CMeta][isolated]`

- **Test organization**: Group related tests in sections
  - Matching vs mismatching cases
  - Different types of mismatches
  - Format consistency tests

## Benefits

### 1. No Test Failures Required
Test string generation without requiring assertion failures.

### 2. Precise Testing
Test specific scenarios and edge cases independently.

### 3. Better Coverage
Easier to achieve comprehensive coverage of formatting logic.

### 4. Faster Debugging
When formatting changes, isolated tests pinpoint the exact issue.

### 5. Independent of Test Data
No need for test_data.json files or test harness - pure unit tests.

## Adding Tests for New Matchers

When adding a new matcher, create an isolated test file:

1. Create `{matcher_name}_isolated.test.cpp`
2. Include matcher header and `matcher_test_helpers.h`
3. Test matching case (output format when match succeeds)
4. Test each possible mismatch scenario
5. Test output structure and consistency
6. Add to CMakeLists.txt

Example CMakeLists.txt addition:
```cmake
add_executable(test_new_matcher_isolated
  new_matcher_isolated.test.cpp
)

target_link_libraries(test_new_matcher_isolated
PRIVATE
  Catch2::Catch2WithMain
  test_matchers
  # Add required component libraries
)

catch_discover_tests(test_new_matcher_isolated)
```

## Adding Tests for New Reporters

When adding a new reporter, create an isolated test file:

1. Create `{reporter_name}_isolated.test.cpp`
2. Include reporter header and `reporter_test_helpers.h`
3. Test each reporter method's output
4. Test format consistency across methods
5. Test with various mock Catch2 structures
6. Add to CMakeLists.txt

## Compatibility with Existing Tests

The isolated testing approach **complements** existing integration tests:

- **Existing tests**: Validate matchers work within Catch2 assertions
- **Isolated tests**: Validate string generation and formatting logic

Both types of tests are valuable:
- Integration tests ensure Catch2 integration works
- Isolated tests ensure formatting is correct and comprehensive

## Future Enhancements

Potential areas for expansion:

1. **Test data support**: Add optional JSON-based test data for complex scenarios
2. **Snapshot testing**: Compare outputs against golden snapshots
3. **Performance testing**: Measure matcher performance
4. **Coverage metrics**: Track formatting code coverage
5. **Automated formatting validation**: CI checks for consistent formatting

## Related Files

- `tests/matchers/matcher_test_helpers.h` - Matcher testing utilities
- `tests/reporters/reporter_test_helpers.h` - Reporter testing utilities
- `tests/matchers/*_isolated.test.cpp` - Isolated matcher tests
- `tests/reporters/*_isolated.test.cpp` - Isolated reporter tests
