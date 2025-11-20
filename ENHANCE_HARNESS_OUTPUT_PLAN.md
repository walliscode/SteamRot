# Plan: Enhance Test Harness and HarnessReporter Output

**Status:** Plan Only - Awaiting Approval  
**Date:** 2025-11-20  
**Issue:** Improve test harness output readability and functionality

## Table of Contents

1. [Overview](#overview)
2. [Requirements Analysis](#requirements-analysis)
3. [Detailed Solutions](#detailed-solutions)
4. [Implementation Specifications](#implementation-specifications)
5. [XML Report Generation](#xml-report-generation)
6. [Testing Strategy](#testing-strategy)
7. [Timeline and Dependencies](#timeline-and-dependencies)

---

## Overview

This document outlines a comprehensive plan to enhance the test harness and HarnessReporter output for better readability, maintainability, and debugging capabilities. The plan addresses five main requirements from the problem statement.

---

## Requirements Analysis

### Requirement 1: Implement Tests for Output Validation

**Current State:**
- HarnessReporter.test.cpp has minimal tests (3 simple test cases)
- No validation of actual output formatting
- No tests for GENERATE behavior with multiple iterations

**Goal:**
- Comprehensive tests that validate reporter behavior
- Tests that verify output formatting is correct
- Tests that ensure proper handling of multiple test data files

**Priority:** High

---

### Requirement 2: Nested Data Indentation

**Current State:**
- Matchers output data with tabs (`\t`)
- No consistent indentation strategy for nested structures
- Hard to read deeply nested component data

**Example of Current Output (CMeta matcher):**
```
----------------------------------------
❌ CMeta Match: 
	❌ m_active:
		actual = false
		expected = true
----------------------------------------
```

**Desired Output:**
```
========================================
❌ CMeta Match
  ❌ m_active
    actual:   false
    expected: true
========================================
```

**Goal:**
- Replace tabs with consistent space-based indentation (2 spaces per level)
- Add indentation helper functions to conmat package
- Update all matchers to use new indentation system
- Better visual hierarchy for nested data

**Priority:** High

---

### Requirement 3: Fix GENERATE Verbosity

**Current State:**
- When using `GENERATE` with multiple test data files, ALL test info is printed
- Even passing tests show full output (test case name, file, line, tags)
- Makes it hard to find actual failures in output

**Example of Current Behavior:**
```
----------------------------------------
TEST CASE: 
	name: run_fixture_test works with Catch2 generators
	file: test_data_harness.test.cpp
	line: 150
	tags: [unit][harness]
----------------------------------------
[Iteration 1: sample_test_1] PASSED
----------------------------------------
TEST CASE: 
	name: run_fixture_test works with Catch2 generators
	file: test_data_harness.test.cpp
	line: 150
	tags: [unit][harness]
----------------------------------------
[Iteration 2: sample_test_2] PASSED
----------------------------------------
TEST CASE: 
	name: run_fixture_test works with Catch2 generators
	file: test_data_harness.test.cpp
	line: 150
	tags: [unit][harness]
----------------------------------------
[Iteration 3: sample_test_3] FAILED
<failure details here>
```

**Desired Behavior:**
```
========================================
TEST CASE: run_fixture_test works with Catch2 generators
  file: test_data_harness.test.cpp:150
  tags: [unit][harness]
========================================

✓ sample_test_1
✓ sample_test_2
❌ sample_test_3
  <detailed failure info only for failed test>

Summary: 2/3 passed
```

**Goal:**
- Only print detailed test case info once (at start)
- Show brief status for each GENERATE iteration
- Full details only for failures
- Add summary at the end

**Priority:** Critical (this is the main pain point)

---

### Requirement 4: General Output Improvements

**Goals:**
- Better visual hierarchy and organization
- More readable comparison output
- Clear highlighting of differences
- Professional, polished appearance

**Priority:** Medium

---

### Requirement 5: XML Report Generation

**Goal:**
- Document how to generate XML reports using Catch2
- Provide examples and best practices
- Brief guide (not implementation)

**Priority:** Low

---

## Detailed Solutions

### Solution 1: Tests for Output Validation

#### Approach

Since reporters don't have direct access to their output stream for inspection, we'll test indirectly:

**1. Test Reporter Lifecycle:**
```cpp
TEST_CASE("HarnessReporter calls all lifecycle methods", "[reporters]") {
  // Use Catch2's internal testing facilities
  // Verify testRunStarting, testCaseStarting, etc. are called
}
```

**2. Test with Multiple GENERATE Iterations:**
```cpp
TEST_CASE("HarnessReporter handles GENERATE correctly", "[reporters]") {
  auto value = GENERATE(1, 2, 3);
  
  // First iteration should print full test info
  // Subsequent iterations should print minimal info
  // This test validates the behavior visually
  REQUIRE(value > 0);
}
```

**3. Test Pass/Fail Behavior:**
```cpp
TEST_CASE("HarnessReporter formats passing tests", "[reporters]") {
  REQUIRE(true);
}

TEST_CASE("HarnessReporter formats failing tests", "[reporters]") {
  INFO("This test should show detailed failure info");
  REQUIRE(false); // Intentional failure
}
```

#### Implementation

- Add ~10-15 new test cases to `HarnessReporter.test.cpp`
- Document expected output format in comments
- Run tests manually to verify output appearance
- Consider adding a test script that captures output for regression testing

---

### Solution 2: Nested Data Indentation

#### API Design for conmat Package

Since conmat is an external library (https://github.com/walliscode/conmat), we need to propose these additions:

**Proposed Functions:**

```cpp
namespace conmat {

/////////////////////////////////////////////////
/// @brief Generate indentation string for given level
///
/// @param level Indentation level (0 = no indent, 1 = one level, etc.)
/// @param spaces_per_level Number of spaces per indentation level (default: 2)
/// @return String containing appropriate number of spaces
/////////////////////////////////////////////////
std::string Indent(size_t level, size_t spaces_per_level = 2);

/////////////////////////////////////////////////
/// @brief Format text with indentation
///
/// @param text Text to indent
/// @param level Indentation level
/// @param spaces_per_level Number of spaces per indentation level (default: 2)
/// @return Indented text
/////////////////////////////////////////////////
std::string IndentedLine(const std::string& text, size_t level, size_t spaces_per_level = 2);

/////////////////////////////////////////////////
/// @brief Format key-value pair with indentation
///
/// @param key Key name
/// @param value Value (will be converted to string)
/// @param level Indentation level
/// @param spaces_per_level Number of spaces per indentation level (default: 2)
/// @return Formatted "key: value" with indentation
/////////////////////////////////////////////////
template<typename T>
std::string IndentedKeyValue(const std::string& key, const T& value, 
                              size_t level, size_t spaces_per_level = 2);

} // namespace conmat
```

**Usage Examples:**

```cpp
// Simple indentation
std::cout << conmat::Indent(0) << "Root level\n";
std::cout << conmat::Indent(1) << "First level\n";
std::cout << conmat::Indent(2) << "Second level\n";

// Output:
// Root level
//   First level
//     Second level

// Indented line
std::cout << conmat::IndentedLine("Component Data", 0) << "\n";
std::cout << conmat::IndentedLine("m_active: false", 1) << "\n";
std::cout << conmat::IndentedLine("m_entity_active: true", 1) << "\n";

// Output:
// Component Data
//   m_active: false
//   m_entity_active: true

// Key-value pairs
std::cout << conmat::IndentedKeyValue("actual", false, 2) << "\n";
std::cout << conmat::IndentedKeyValue("expected", true, 2) << "\n";

// Output:
//     actual:   false
//     expected: true
```

#### Fallback Implementation

If conmat changes aren't immediately available, implement in SteamRot temporarily:

**File:** `tests/harness/indentation_helpers.h`

```cpp
namespace steamrot::tests::formatting {

inline std::string Indent(size_t level, size_t spaces_per_level = 2) {
  return std::string(level * spaces_per_level, ' ');
}

inline std::string IndentedLine(const std::string& text, size_t level, 
                                size_t spaces_per_level = 2) {
  return Indent(level, spaces_per_level) + text;
}

template<typename T>
std::string IndentedKeyValue(const std::string& key, const T& value,
                              size_t level, size_t spaces_per_level = 2) {
  std::ostringstream oss;
  oss << Indent(level, spaces_per_level) << key << ": " << value;
  return oss.str();
}

} // namespace steamrot::tests::formatting
```

#### Matcher Updates

Update all matchers to use indentation:

**Before (cmeta_matchers.h):**
```cpp
if (actual.m_active != m_expected.m_active) {
  oss << "\t" << conmat::TestFailed() << "m_active:" << "\n";
  oss << "\t\t" << "actual = "
      << conmat::Colorize(actual.m_active, conmat::Color::Red) << "\n";
  oss << "\t\t" << "expected = "
      << conmat::Colorize(m_expected.m_active, conmat::Color::Blue) << "\n";
}
```

**After:**
```cpp
if (actual.m_active != m_expected.m_active) {
  oss << conmat::IndentedLine(conmat::TestFailed() + "m_active", 1) << "\n";
  oss << conmat::IndentedKeyValue("actual", 
      conmat::Colorize(actual.m_active, conmat::Color::Red), 2) << "\n";
  oss << conmat::IndentedKeyValue("expected",
      conmat::Colorize(m_expected.m_active, conmat::Color::Blue), 2) << "\n";
}
```

**Matchers to Update:**
- `tests/matchers/cmeta_matchers.h`
- `tests/matchers/component_matchers.h`
- `tests/matchers/entity_memory_pool_matchers.h`
- `tests/matchers/event_matchers.h`
- `tests/matchers/ui_element_matchers.h`

#### Test Data Harness Updates

Update `test_data_harness.cpp` to use indentation in headers:

**Before:**
```cpp
oss << conmat::Divider("=", 40) << "\n";
oss << conmat::Colorize("Data Structure Comparison Tests",
                        conmat::Color::Blue) << "\n";
oss << "\t" << context.FormatTestName() << "\n";
oss << "\t" << context.FormatTickInfo() << "\n";
```

**After:**
```cpp
oss << conmat::Divider("=", 40) << "\n";
oss << conmat::Colorize("Data Structure Comparison Tests",
                        conmat::Color::Blue) << "\n";
oss << conmat::IndentedLine(context.FormatTestName(), 1) << "\n";
oss << conmat::IndentedLine(context.FormatTickInfo(), 1) << "\n";
```

---

### Solution 3: Fix GENERATE Verbosity

This is the most critical improvement requested.

#### Root Cause Analysis

Catch2 calls `testCasePartialStarting` for EACH iteration of a GENERATE test. The current implementation prints full test case info every time.

**Current HarnessReporter.cpp:**
```cpp
void HarnessReporter::testCaseStarting(const Catch::TestCaseInfo &test_info) {
  // This is called ONCE per test case (not per iteration)
  std::cout << conmat::Divider("-", 40) << "\n";
  std::cout << conmat::Colorize("TEST CASE: ", conmat::Color::Blue) << "\n";
  std::cout << "\t" << "name: " << test_info.name << "\n";
  // ... etc
}

void HarnessReporter::testCasePartialStarting(
    Catch::TestCaseInfo const &test_info, uint64_t part_number) {
  // Currently empty - this is called for EACH GENERATE iteration
}
```

#### Solution Design

**Key Insight:** Use `testCasePartialStarting` and `testCasePartialEnded` to track iterations.

**Enhanced HarnessReporter.h:**
```cpp
class HarnessReporter : public Catch::CumulativeReporterBase {
private:
  // Track current test case info
  bool m_in_generated_test = false;
  size_t m_generated_iteration_count = 0;
  size_t m_generated_passed_count = 0;
  size_t m_generated_failed_count = 0;
  std::string m_current_test_name;
  
public:
  // ... existing methods ...
  
  void testCaseStarting(const Catch::TestCaseInfo &testInfo) override;
  void testCasePartialStarting(Catch::TestCaseInfo const &testInfo,
                               uint64_t partNumber) override;
  void testCasePartialEnded(Catch::TestCaseStats const &testCaseStats,
                            uint64_t partNumber) override;
  void testCaseEnded(const Catch::TestCaseStats &testCaseStats) override;
};
```

**Enhanced HarnessReporter.cpp:**

```cpp
void HarnessReporter::testCaseStarting(const Catch::TestCaseInfo &test_info) {
  // Reset iteration tracking
  m_in_generated_test = false;
  m_generated_iteration_count = 0;
  m_generated_passed_count = 0;
  m_generated_failed_count = 0;
  m_current_test_name = test_info.name;
  
  // Print full test case info ONCE
  std::cout << "\n" << conmat::Divider("=", 60) << "\n";
  std::cout << conmat::Colorize("TEST CASE: " + test_info.name, 
                                conmat::Color::Blue) << "\n";
  
  std::filesystem::path file_path(test_info.lineInfo.file);
  std::cout << conmat::IndentedLine("file: " + file_path.filename().string() + 
                                    ":" + std::to_string(test_info.lineInfo.line), 1) << "\n";
  std::cout << conmat::IndentedLine("tags: " + test_info.tagsAsString(), 1) << "\n";
  std::cout << conmat::Divider("=", 60) << "\n\n";
}

void HarnessReporter::testCasePartialStarting(
    Catch::TestCaseInfo const &test_info, uint64_t part_number) {
  // This is called for EACH GENERATE iteration
  m_in_generated_test = true;
  m_generated_iteration_count++;
  
  // Don't print anything here - wait for result
}

void HarnessReporter::testCasePartialEnded(
    Catch::TestCaseStats const &test_case_stats, uint64_t part_number) {
  
  if (!m_in_generated_test) return;
  
  // Get test data name from INFO messages if available
  std::string test_data_name = "iteration_" + std::to_string(part_number);
  for (const auto &msg : test_case_stats.infoMessages) {
    std::string msg_str = msg.message;
    if (msg_str.find("Test name:") != std::string::npos) {
      // Extract test data name
      size_t pos = msg_str.find("Test name:") + 11;
      test_data_name = msg_str.substr(pos);
      // Trim whitespace
      test_data_name.erase(0, test_data_name.find_first_not_of(" \t\n\r"));
      test_data_name.erase(test_data_name.find_last_not_of(" \t\n\r") + 1);
      break;
    }
  }
  
  // Check if this iteration passed
  bool passed = test_case_stats.totals.assertions.allOk();
  
  if (passed) {
    m_generated_passed_count++;
    // Brief success message
    std::cout << conmat::TestPassed() << " " << test_data_name << "\n";
  } else {
    m_generated_failed_count++;
    // Detailed failure message
    std::cout << "\n" << conmat::Divider("-", 60) << "\n";
    std::cout << conmat::TestFailed() << " " << test_data_name << "\n";
    std::cout << conmat::Divider("-", 60) << "\n";
    
    // Print all INFO messages for failed test
    for (const auto &msg : test_case_stats.infoMessages) {
      std::cout << msg.message << "\n";
    }
    
    // Print assertion details
    // (These are captured by assertionEnded and printed separately)
  }
}

void HarnessReporter::testCaseEnded(const Catch::TestCaseStats &test_case_stats) {
  if (m_in_generated_test && m_generated_iteration_count > 1) {
    // Print summary for GENERATE tests
    std::cout << "\n" << conmat::Divider("-", 60) << "\n";
    std::cout << "Summary: " << m_generated_passed_count << "/" 
              << m_generated_iteration_count << " passed";
    
    if (m_generated_failed_count > 0) {
      std::cout << " (" << conmat::Colorize(std::to_string(m_generated_failed_count) + 
                                            " FAILED", conmat::Color::Red) << ")";
    }
    std::cout << "\n";
    std::cout << conmat::Divider("-", 60) << "\n";
  }
  
  // Reset state
  m_in_generated_test = false;
  m_generated_iteration_count = 0;
  m_generated_passed_count = 0;
  m_generated_failed_count = 0;
}

void HarnessReporter::assertionEnded(
    Catch::AssertionStats const &assertion_stats) {
  
  // Only print assertion details if test failed
  if (!assertion_stats.assertionResult.isOk()) {
    // Print any INFO messages
    for (const auto &msg : assertion_stats.infoMessages) {
      std::cout << conmat::IndentedLine(msg.message, 1) << "\n";
    }
    
    // Print assertion expression
    std::cout << conmat::IndentedLine(
        assertion_stats.assertionResult.m_resultData.reconstructExpression(), 1) 
              << "\n";
  }
}
```

**Expected Output After Changes:**

```
========================================
TEST CASE: run_fixture_test works with Catch2 generators
  file: test_data_harness.test.cpp:150
  tags: [unit][harness]
========================================

✓ sample_test_1
✓ sample_test_2

------------------------------------------------------------
❌ sample_test_3
------------------------------------------------------------
  Test name: sample_test_3
  Description: Test with intentional mismatch
  
  EntityMemoryPool comparison failed:
    Entity 0, Component CUserInterface:
      m_active mismatch
        actual:   false
        expected: true

------------------------------------------------------------
Summary: 2/3 passed (1 FAILED)
------------------------------------------------------------
```

---

### Solution 4: General Output Improvements

#### 4.1 Better Dividers

**Current:** Inconsistent widths (40 vs 60 characters)

**Proposed:** Standardize to 60 characters
- Use `=` for major sections (test cases, summaries)
- Use `-` for subsections (component comparisons, tick info)

#### 4.2 Alignment

For actual vs expected comparisons, align values:

**Before:**
```
actual = false
expected = true
```

**After:**
```
actual:   false
expected: true
```

#### 4.3 Color Scheme

Standardize color usage:
- **Blue:** Headers, section titles
- **Green:** Success indicators, passing tests
- **Red:** Failure indicators, failing tests
- **Cyan:** Contextual info (tick numbers, file names)
- **Yellow:** Warnings, important notes

#### 4.4 Hierarchical Structure

For nested components (e.g., UIElement with children):

```
========================================
Entity 0: CUserInterface
  ✓ m_active: true
  ✓ m_ui_name: "test_ui"
  
  root_ui_element:
    ✓ position: (100, 200)
    ✓ size: (50, 30)
    
    children:
      [0] Button:
        ❌ m_active
          actual:   false
          expected: true
        ✓ position: (10, 10)
========================================
```

---

### Solution 5: XML Report Generation

#### Overview

Catch2 has built-in support for multiple output formats. XML reporting doesn't require custom implementation.

#### Usage

**Option 1: Using ctest**
```bash
# Generate JUnit XML (common format for CI systems)
ctest --preset Debug --output-junit test_results.xml

# Generate native Catch2 XML
ctest --preset Debug --output-on-failure > test_output.txt
```

**Option 2: Direct Catch2 invocation**
```bash
# JUnit XML format (recommended for CI/CD)
./test_harness -r junit -o test_results.xml

# Native Catch2 XML format
./test_harness -r xml -o test_results.xml

# JSON format (for custom parsing)
./test_harness -r json -o test_results.json

# Console format (default)
./test_harness -r console
```

#### XML Format Examples

**JUnit XML format:**
```xml
<?xml version="1.0" encoding="UTF-8"?>
<testsuites>
  <testsuite name="test_harness" tests="10" failures="1" errors="0" time="2.456">
    <testcase classname="harness" name="run_fixture_test works with Catch2 generators" time="0.523">
      <system-out>Test name: sample_test_1</system-out>
    </testcase>
    <testcase classname="harness" name="run_fixture_test works with Catch2 generators" time="0.498">
      <failure message="EntityMemoryPool mismatch" type="CHECK_THAT">
        Entity 0, Component CUserInterface:
          m_active mismatch
            actual: false
            expected: true
      </failure>
    </testcase>
  </testsuite>
</testsuites>
```

#### Integration with CI/CD

**GitHub Actions:**
```yaml
- name: Run Tests
  run: |
    ctest --preset Debug --output-junit test_results.xml
    
- name: Publish Test Results
  uses: dorny/test-reporter@v1
  if: always()
  with:
    name: Test Results
    path: test_results.xml
    reporter: java-junit
```

**Jenkins:**
```groovy
stage('Test') {
  steps {
    sh 'ctest --preset Debug --output-junit test_results.xml'
  }
  post {
    always {
      junit 'test_results.xml'
    }
  }
}
```

#### Document Location

Create: `tests/reporters/XML_REPORT_GUIDE.md`

Contents:
- Overview of XML reporting with Catch2
- Command examples
- Format descriptions
- CI/CD integration examples
- Troubleshooting tips

---

## Implementation Specifications

### File Changes

#### New Files
- `tests/reporters/XML_REPORT_GUIDE.md` - XML reporting documentation
- `tests/harness/indentation_helpers.h` (temporary, if conmat not available)

#### Modified Files
- `tests/reporters/HarnessReporter.h` - Add state tracking fields
- `tests/reporters/HarnessReporter.cpp` - Implement selective output
- `tests/reporters/HarnessReporter.test.cpp` - Add comprehensive tests
- `tests/matchers/cmeta_matchers.h` - Use indentation
- `tests/matchers/component_matchers.h` - Use indentation
- `tests/matchers/entity_memory_pool_matchers.h` - Use indentation
- `tests/matchers/event_matchers.h` - Use indentation
- `tests/matchers/ui_element_matchers.h` - Use indentation
- `tests/harness/test_data_harness.cpp` - Use indentation in headers

### External Dependencies

#### conmat Library

The indentation functions need to be added to https://github.com/walliscode/conmat

**Proposed PR to conmat:**
- Add `Indent()`, `IndentedLine()`, `IndentedKeyValue()` functions
- Maintain backward compatibility
- Add tests for new functions
- Update conmat README with examples

**Fallback:**
If conmat changes aren't available quickly, use temporary local implementation in `indentation_helpers.h`

### Code Standards

- Follow existing SteamRot coding style (2-space indentation)
- Use Doxygen comments for new functions
- Maintain consistency with existing console_output.h patterns
- Respect NO_COLOR environment variable
- Use `std::expected` for error handling where appropriate

---

## Testing Strategy

### Unit Tests

**HarnessReporter.test.cpp:**
- Test lifecycle methods are called correctly
- Test with GENERATE (multiple iterations)
- Test pass/fail formatting
- Test summary generation
- Test with INFO messages

**indentation_helpers.test.cpp (if needed):**
- Test Indent() with various levels
- Test IndentedLine() formatting
- Test IndentedKeyValue() alignment
- Test with edge cases (level 0, large levels)

### Integration Tests

**Visual Validation:**
Run actual tests and inspect output manually:
```bash
# Run with HarnessReporter
ctest --preset Debug -R test_harness -V --reporter harness

# Check output formatting
# Verify colors (if terminal supports)
# Verify indentation levels
# Verify GENERATE behavior with multiple data files
```

**Regression Testing:**
- Capture baseline output before changes
- Capture output after changes
- Compare for expected differences
- Ensure no unexpected changes

### Manual Testing Checklist

- [ ] Run tests with colors enabled
- [ ] Run tests with NO_COLOR=1
- [ ] Run tests with single GENERATE value
- [ ] Run tests with multiple GENERATE values (10+)
- [ ] Run tests with all passing
- [ ] Run tests with some failing
- [ ] Run tests with nested components
- [ ] Verify indentation at multiple levels
- [ ] Check alignment of actual/expected values
- [ ] Verify summary statistics are correct
- [ ] Test XML output generation

---

## Timeline and Dependencies

### Phase 1: Indentation System (Week 1)
- [ ] Decide on conmat vs local implementation
- [ ] If conmat: Create PR to walliscode/conmat
- [ ] If local: Implement indentation_helpers.h
- [ ] Add tests for indentation functions
- [ ] Update 2-3 matchers as proof of concept

**Dependencies:** None  
**Risk:** Medium (external library coordination)

### Phase 2: GENERATE Fix (Week 1-2)
- [ ] Add state tracking to HarnessReporter
- [ ] Implement selective output logic
- [ ] Add tests for GENERATE behavior
- [ ] Manual testing with multiple test data files

**Dependencies:** None  
**Risk:** Low (self-contained change)

### Phase 3: Matcher Updates (Week 2)
- [ ] Update all matchers to use indentation
- [ ] Update test_data_harness.cpp
- [ ] Comprehensive testing

**Dependencies:** Phase 1 complete  
**Risk:** Low (repetitive but straightforward)

### Phase 4: Polish and Documentation (Week 2-3)
- [ ] General output improvements
- [ ] Create XML_REPORT_GUIDE.md
- [ ] Final testing and validation
- [ ] Update relevant documentation

**Dependencies:** Phases 1-3 complete  
**Risk:** Low

### Total Timeline: 2-3 weeks

---

## Conclusion

This plan addresses all five requirements from the problem statement:

1. ✅ **Tests for output validation** - Comprehensive test suite in HarnessReporter.test.cpp
2. ✅ **Nested data indentation** - Indentation system with 2-space levels (conmat or local)
3. ✅ **Fix GENERATE verbosity** - Selective output with iteration tracking
4. ✅ **Output improvements** - Better formatting, colors, alignment, hierarchy
5. ✅ **XML report guide** - Documentation for using Catch2's built-in XML support

The implementation is modular and can be done incrementally. The most critical change (GENERATE fix) is self-contained and can be implemented first for immediate benefit.

**Next Steps:**
1. Review and approve this plan
2. Decide on conmat vs local implementation for indentation
3. Begin Phase 1 implementation
