# Quick Start: Isolated Matcher and Reporter Testing

## What Is This?

A new testing pattern that allows you to test Catch2 matchers and reporters **without requiring failing tests**. You can directly extract and validate string outputs in isolation.

## Why Use It?

**Before:** Testing matcher output required intentionally creating mismatches and relying on test failures.

**Now:** Test matcher string generation directly as pure unit tests.

## Quick Examples

### Testing a Matcher

```cpp
#include "cmeta_matchers.h"
#include "matcher_test_helpers.h"

TEST_CASE("CMeta matcher outputs correct error message") {
  // Setup mismatched data
  steamrot::CMeta expected;
  expected.m_active = true;
  
  steamrot::CMeta actual;
  actual.m_active = false;
  
  // Test the matcher in isolation
  steamrot::tests::CMetaEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  // Validate output directly
  REQUIRE_FALSE(result.matched);
  REQUIRE(result.description.find("m_active") != std::string::npos);
  REQUIRE(result.description.find("FAILED") != std::string::npos);
}
```

### Testing a Reporter

```cpp
#include "HarnessReporter.h"
#include "reporter_test_helpers.h"

TEST_CASE("HarnessReporter formats test case correctly") {
  // Capture reporter output
  steamrot::tests::StreamCapture capture(std::cout);
  
  Catch::ReporterConfig config(Catch::ConfigData{});
  HarnessReporter reporter(std::move(config));
  
  Catch::TestCaseInfo test_info("my_test", {"[tag]"}, 
                                 Catch::SourceLineInfo("file.cpp", 42));
  reporter.testCaseStarting(test_info);
  
  // Validate output
  std::string output = capture.GetCapturedOutput();
  REQUIRE(output.find("my_test") != std::string::npos);
  REQUIRE(output.find("file.cpp") != std::string::npos);
}
```

## Key Helper Functions

### For Matchers (`matcher_test_helpers.h`)

```cpp
// Test matcher and get both match result and description
auto result = TestMatcherOutput(actual, matcher);

// Check match status
bool matched = result.matched;

// Get description string
std::string desc = result.description;

// Validate string content
bool has_all = ContainsAllSubstrings(output, {"foo", "bar"});
bool has_none = ContainsNoneOfSubstrings(output, {"error", "fail"});
size_t count = CountSubstring(output, "field");
bool valid = ValidateOutputStructure(output, has_dividers, has_passed, has_failed);
```

### For Reporters (`reporter_test_helpers.h`)

```cpp
// Capture output during scope
{
  StreamCapture capture(std::cout);
  
  // Call reporter methods...
  
  std::string output = capture.GetCapturedOutput();
  // Validate output...
}

// Validate reporter output
bool valid = ValidateReporterOutput(output, {"header", "content"});
size_t lines = CountLines(output);
auto lines_with_error = ExtractLinesContaining(output, "error");
```

## Where Are The Tests?

All tests are tagged with `[isolated]`:

- `tests/matchers/cmeta_matchers_isolated.test.cpp`
- `tests/matchers/entity_memory_pool_matchers_isolated.test.cpp`
- `tests/matchers/cuser_interface_matchers_isolated.test.cpp`
- `tests/matchers/ui_element_matchers_isolated.test.cpp`
- `tests/matchers/event_matchers_isolated.test.cpp`
- `tests/reporters/HarnessReporter_isolated.test.cpp`

## Running The Tests

```bash
# Run all isolated tests
ctest --preset Debug -L isolated

# Run specific isolated tests
ctest --preset Debug -R cmeta_matchers_isolated
ctest --preset Debug -R reporter_isolated
```

## Adding Tests For New Matchers

1. Create `{matcher_name}_isolated.test.cpp`
2. Include matcher header and `matcher_test_helpers.h`
3. Write test cases:
   - Matching case (PASSED output)
   - Each mismatch scenario (FAILED output)
   - Output structure validation
4. Add to CMakeLists.txt:

```cmake
add_executable(test_new_matcher_isolated
  new_matcher_isolated.test.cpp
)

target_link_libraries(test_new_matcher_isolated
PRIVATE
  Catch2::Catch2WithMain
  test_matchers
  # Add required libraries
)

catch_discover_tests(test_new_matcher_isolated)
```

## Full Documentation

See [ISOLATED_MATCHER_REPORTER_TESTING.md](ISOLATED_MATCHER_REPORTER_TESTING.md) for:
- Complete API reference
- Detailed examples
- Testing strategy
- Best practices
- Coverage summary

## Benefits

✅ **No test failures needed** - Test string generation directly
✅ **Precise testing** - Test specific scenarios independently
✅ **Fast** - Pure unit tests, no integration overhead
✅ **Better coverage** - Easy to test all formatting paths
✅ **Clear intent** - Tests explicitly validate string output
✅ **Easy debugging** - When formatting breaks, tests pinpoint the issue

## Compatibility

This **complements** existing integration tests:
- **Existing tests**: Validate matchers work in Catch2 assertions
- **Isolated tests**: Validate string formatting is correct

Both are valuable and should coexist.
