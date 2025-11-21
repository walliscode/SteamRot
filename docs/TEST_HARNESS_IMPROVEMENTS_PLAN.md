# Test Harness and HarnessReporter Improvements Plan

## Overview

This document outlines planned improvements to the test harness infrastructure and HarnessReporter to enhance test output readability, structure, and aesthetics.

## Current State Analysis

### Components Examined
1. **HarnessReporter** (`tests/reporters/HarnessReporter.h/cpp`)
   - Custom Catch2 reporter for data-driven tests
   - Currently outputs test case metadata (name, file, line, tags)
   - Prints assertion results and INFO messages
   - Uses conmat for basic formatting (Divider, Colorize)

2. **test_data_harness** (`tests/harness/test_data_harness.h/cpp`)
   - Loads test configurations from JSON (via FlatBuffers)
   - Executes data-driven tests with RunFixtureTest()
   - Outputs comparison headers using conmat::Divider and conmat::Colorize
   - Integrates with Catch2 GENERATE for parameterized tests

3. **Matchers** (`tests/matchers/`)
   - Custom matchers for entity, component, and event comparison
   - Output detailed mismatch information
   - Use conmat::Colorize for highlighting differences (Red=actual, Blue=expected)
   - Use conmat::TestFailed() for failure indicators
   - Currently use tabs (`\t`) for indentation

4. **console_output.h** (`tests/harness/console_output.h`)
   - Helper functions for formatted console output
   - Provides PrintSuccess, PrintError, PrintInfo, PrintSectionHeader
   - Checks for color support via environment variables

### Current Issues

1. **GENERATE Macro Verbosity**
   - When using `GENERATE_COPY(from_range(configs.value()))`, every test data file is processed
   - If only one test fails, information for ALL configs is printed
   - This creates excessive output when testing multiple JSON files

2. **Nested Data Indentation**
   - Current matchers use tabs (`\t`) for indentation
   - Tabs take up too much space for deeply nested structures
   - No consistent approach to hierarchical data display
   - Nested UIElements, EventPackets, etc., need better visual structure

3. **Limited Structural Formatting**
   - Headers and dividers are basic
   - No consistent visual hierarchy
   - Difficult to scan large test outputs

4. **No XML Report Generation**
   - Currently only console output
   - No support for CI/CD XML reporting

## Proposed Improvements

### 1. Enhanced Indentation with conmat::Indent

**Goal**: Implement and use `conmat::Indent` function for better control over nested data display.

**Implementation Requirements**:

The `conmat::Indent` function should accept:
- `level` (int): Indentation depth level (0 = no indent)
- `spaces_per_level` (int): Number of spaces per level (default: 2)

**Usage Pattern**:
```cpp
// Level 0: No indent
oss << conmat::Indent(0, 2) << "Component: CUserInterface" << "\n";

// Level 1: 2 spaces
oss << conmat::Indent(1, 2) << "ui_name: " << value << "\n";

// Level 2: 4 spaces
oss << conmat::Indent(2, 2) << "root_element:" << "\n";

// Level 3: 6 spaces
oss << conmat::Indent(3, 2) << "position: (100, 200)" << "\n";
```

**Advantages Over Tabs**:
- Consistent spacing across terminals
- Fine-grained control over indentation depth
- Better readability for deeply nested structures
- Easier to align mixed content

**Required Changes**:

1. **Add to conmat library** (external repo: https://github.com/walliscode/conmat)
   - File: `include/conmat.h` or similar
   - Function signature:
     ```cpp
     namespace conmat {
       std::string Indent(int level, int spaces_per_level = 2);
     }
     ```
   - Implementation:
     ```cpp
     std::string Indent(int level, int spaces_per_level) {
       return std::string(level * spaces_per_level, ' ');
     }
     ```

2. **Update SteamRot matchers to use conmat::Indent**
   - Replace all `\t` with `conmat::Indent(level, 2)`
   - Add level tracking through recursive comparison functions
   - Files to update:
     - `tests/matchers/ui_element_matchers.h`
     - `tests/matchers/event_matchers.h`
     - `tests/matchers/entity_memory_pool_matchers.h`
     - `tests/matchers/cmeta_matchers.h`
     - `tests/matchers/cuser_interface_matchers.h`
     - `tests/matchers/cmachina_form_matchers.h`

3. **Update test_data_harness output**
   - Use consistent indentation levels for headers
   - Apply hierarchical formatting to data structure comparisons

**Example Before/After**:

**Before (using tabs)**:
```
[FAILED] EntityMemoryPool Comparison
============================================================
  Test: my_test
  Tick: [1 of 5]
------------------------------------------------------------
  Differences:
    * Entity[0]:
	CUserInterface:
		m_active: actual=true, expected=false
		ui_name: actual="menu", expected="title"
```

**After (using conmat::Indent)**:
```
[FAILED] EntityMemoryPool Comparison
============================================================
  Test: my_test
  Tick: [1 of 5]
------------------------------------------------------------
  Differences:
  Entity[0]:
    CUserInterface:
      m_active: actual=true, expected=false
      ui_name: actual="menu", expected="title"
```

### 2. Fix GENERATE Macro Verbose Output

**Problem**: With `GENERATE_COPY(from_range(configs))`, Catch2 creates separate test instances for each config. When one fails, all configs' INFO messages are displayed.

**Solution Approaches**:

#### Option A: Use SECTION per config (Recommended)
```cpp
TEST_CASE("Data-driven test with sections", "[unit][harness]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  for (const auto *config : configs.value()) {
    SECTION(config->metadata()->test_name()->str()) {
      auto result = steamrot::tests::RunFixtureTest(config);
      REQUIRE(result.has_value());
    }
  }
}
```

**Benefits**:
- Each config runs in its own SECTION
- Only failed SECTION's output is shown
- Clear separation in output
- Better failure isolation

**Drawbacks**:
- Not using GENERATE (different pattern)
- Requires test structure changes

#### Option B: Conditional INFO based on failure
```cpp
TEST_CASE("Data-driven test with conditional INFO", "[unit][harness]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  auto result = steamrot::tests::RunFixtureTest(config);
  
  // Only add INFO if test failed
  if (!result.has_value()) {
    INFO("Test name: " << config->metadata()->test_name()->str());
    INFO("Description: " << config->metadata()->description()->str());
    FAIL(result.error().message);
  }
}
```

**Benefits**:
- Still uses GENERATE
- Only failed tests show INFO
- Less verbose output

**Drawbacks**:
- FAIL() might not capture all context
- Requires result checking before assertions

#### Option C: Enhanced HarnessReporter (Recommended)

Enhance `HarnessReporter` to track and conditionally output per-test-case-instance data:

```cpp
class HarnessReporter : public Catch::CumulativeReporterBase {
private:
  // Track current test case instance info
  struct InstanceInfo {
    std::string test_name;
    std::vector<std::string> info_messages;
    bool has_failure = false;
  };
  
  std::map<std::string, InstanceInfo> m_instance_data;
  std::string m_current_instance_id;
  
public:
  // ... existing methods ...
  
  void assertionEnded(Catch::AssertionStats const &stats) override {
    if (!stats.assertionResult.isOk()) {
      m_instance_data[m_current_instance_id].has_failure = true;
    }
  }
  
  void testCasePartialEnded(Catch::TestCaseStats const &stats, 
                           uint64_t partNumber) override {
    // Only output if this instance failed
    const auto &instance = m_instance_data[m_current_instance_id];
    if (instance.has_failure) {
      // Output stored info messages
      for (const auto &msg : instance.info_messages) {
        std::cout << msg << "\n";
      }
    }
  }
};
```

**Benefits**:
- Preserves GENERATE usage
- Reporter-level solution (cleaner)
- Can aggregate failure information
- Most flexible approach

**Drawbacks**:
- More complex reporter implementation
- Requires understanding Catch2 reporter lifecycle

**Recommendation**: Implement **Option C** (Enhanced HarnessReporter) as it provides the best balance of functionality and code cleanliness.

### 3. Additional Output Improvements

#### A. Structured Headers with Visual Hierarchy

**Current**:
```
============================================================
Data Structure Comparison Tests
	test: my_test_name
	tick: [1/5]
============================================================
```

**Improved**:
```
╔════════════════════════════════════════════════════════════╗
║ Data Structure Comparison Tests                           ║
╟────────────────────────────────────────────────────────────╢
║ Test: my_test_name                                         ║
║ Tick: [1 of 5]                                             ║
╚════════════════════════════════════════════════════════════╝
```

**Implementation**: Add functions to conmat or console_output.h:
```cpp
namespace conmat {
  std::string BoxedHeader(const std::string &title, 
                          const std::vector<std::string> &lines);
  std::string SectionDivider(char c = '─', int width = 60);
}
```

#### B. Color-Coded Status Indicators

Enhance visual distinction between success/failure/info:

```cpp
// In HarnessReporter
void testCaseEnded(const Catch::TestCaseStats &stats) override {
  if (stats.totals.testCases.passed == stats.totals.testCases.total) {
    std::cout << conmat::Colorize("✓ PASSED", conmat::Color::Green);
  } else {
    std::cout << conmat::Colorize("✗ FAILED", conmat::Color::Red);
  }
  std::cout << " - " << stats.testInfo->name << "\n";
}
```

#### C. Summary Statistics

Add test run summary in `testRunEndedCumulative()`:

```cpp
void HarnessReporter::testRunEndedCumulative() override {
  std::cout << "\n";
  std::cout << conmat::Divider("=", 60) << "\n";
  std::cout << conmat::Colorize("Test Run Summary", conmat::Color::Bold) << "\n";
  std::cout << conmat::Divider("=", 60) << "\n";
  
  const auto &totals = m_testRunStats.totals;
  
  std::cout << "Total Tests: " << totals.testCases.total << "\n";
  std::cout << conmat::Colorize("  Passed: ", conmat::Color::Green) 
            << totals.testCases.passed << "\n";
  std::cout << conmat::Colorize("  Failed: ", conmat::Color::Red)
            << totals.testCases.failed << "\n";
  
  if (totals.testCases.total > 0) {
    double pass_rate = (double)totals.testCases.passed / totals.testCases.total * 100;
    std::cout << "Pass Rate: " << std::fixed << std::setprecision(1) 
              << pass_rate << "%\n";
  }
  
  std::cout << conmat::Divider("=", 60) << "\n";
}
```

#### D. Improved Difference Reporting

For matchers, add diff-style output:

```
Differences in CUserInterface:
  m_active:
    - actual:   true
    + expected: false
  ui_name:
    - actual:   "menu"
    + expected: "title"
```

Implementation:
```cpp
std::string FormatDifference(const std::string &field,
                            const std::string &actual,
                            const std::string &expected,
                            int indent_level = 0) {
  std::ostringstream oss;
  oss << conmat::Indent(indent_level, 2) << field << ":\n";
  oss << conmat::Indent(indent_level + 1, 2) 
      << conmat::Colorize("- actual:   " + actual, conmat::Color::Red) << "\n";
  oss << conmat::Indent(indent_level + 1, 2)
      << conmat::Colorize("+ expected: " + expected, conmat::Color::Blue) << "\n";
  return oss.str();
}
```

### 4. XML Report Generation

**Goal**: Support XML output for CI/CD integration while maintaining readable console output.

**Approach**: Use Catch2's built-in XML reporter alongside HarnessReporter.

#### Command-Line Usage

Catch2 supports multiple reporters simultaneously:

```bash
# Console output only (current)
./test_harness -r harness

# XML output only
./test_harness -r xml -o test_results.xml

# Both console and XML
./test_harness -r harness -r xml::out=test_results.xml

# Or using environment variable
CATCH_REPORTER=xml::out=test_results.xml ./test_harness
```

#### CMake Integration

Update test configuration to optionally generate XML:

```cmake
# tests/CMakeLists.txt
option(STEAMROT_TEST_XML_OUTPUT "Generate XML test reports" OFF)

if(STEAMROT_TEST_XML_OUTPUT)
  set(CATCH_REPORTER_ARGS "-r" "xml::out=${CMAKE_BINARY_DIR}/test_results.xml")
else()
  set(CATCH_REPORTER_ARGS "-r" "harness")
endif()

# Add to test command
add_test(
  NAME test_harness
  COMMAND test_harness ${CATCH_REPORTER_ARGS}
)
```

#### CI/CD Integration Example (GitHub Actions)

```yaml
- name: Run Tests with XML Output
  run: |
    ctest --preset Debug --output-junit test_results.xml
    
- name: Publish Test Results
  uses: EnricoMi/publish-unit-test-result-action@v2
  if: always()
  with:
    files: test_results.xml
```

#### Custom XML Enhancement (Optional)

If needed, create a custom XML reporter that extends Catch2's XML with harness-specific metadata:

```cpp
class HarnessXMLReporter : public Catch::XmlReporter {
public:
  HarnessXMLReporter(Catch::ReporterConfig &&config)
      : XmlReporter(CATCH_MOVE(config)) {}
  
  void testCaseStarting(const Catch::TestCaseInfo &testInfo) override {
    XmlReporter::testCaseStarting(testInfo);
    
    // Add custom attributes for harness tests
    // Extract test data config name from INFO messages
    // Add as XML attributes or properties
  }
};

CATCH_REGISTER_REPORTER("harness-xml", HarnessXMLReporter);
```

**XML Output Structure** (using Catch2 built-in):

```xml
<?xml version="1.0" encoding="UTF-8"?>
<Catch2TestRun name="test_harness" filters="*">
  <Group name="test_harness">
    <TestCase name="Data-driven test" tags="[unit][harness]" 
              filename="test_data_harness.test.cpp" line="150">
      <Section name="sample_test_1">
        <Expression success="true" type="REQUIRE" filename="..." line="...">
          <Original>result.has_value()</Original>
          <Expanded>true</Expanded>
        </Expression>
      </Section>
    </TestCase>
  </Group>
  <OverallResults successes="10" failures="0" expectedFailures="0"/>
</Catch2TestRun>
```

## Implementation Tests

### Test Requirements

1. **Indentation Tests**
   - Verify conmat::Indent produces correct spacing
   - Test with various levels (0, 1, 5, 10)
   - Test with different spaces_per_level values

2. **Reporter Tests**
   - Verify HarnessReporter only outputs failed test instance info
   - Test with mix of passing and failing tests
   - Verify summary statistics are accurate

3. **Matcher Output Tests**
   - Verify nested indentation in matchers
   - Test deeply nested UIElement comparison
   - Verify color codes are applied correctly

4. **XML Output Tests**
   - Verify XML is well-formed
   - Verify test results are accurately captured
   - Test with both passing and failing tests

### Test Files to Create/Update

1. `tests/reporters/HarnessReporter.test.cpp`
   - Add tests for enhanced reporter functionality
   - Test selective INFO output

2. `tests/harness/test_data_harness.test.cpp`
   - Add tests using SECTION approach
   - Verify improved output formatting

3. `tests/matchers/*.test.cpp` (if needed)
   - Verify indentation in matcher output
   - Test with nested structures

## Implementation Checklist

### Phase 1: conmat::Indent Function
- [ ] Add `conmat::Indent` function to conmat library
  - [ ] Open PR to https://github.com/walliscode/conmat
  - [ ] Implement function with tests
  - [ ] Merge and tag new version
- [ ] Update SteamRot dependency to new conmat version
  - [ ] Update `cmake/FetchContentModules/conmat.cmake` with new tag
- [ ] Write unit tests for Indent function usage

### Phase 2: Replace Tabs with Indent
- [ ] Update all matcher files to use conmat::Indent
  - [ ] `tests/matchers/ui_element_matchers.h`
  - [ ] `tests/matchers/event_matchers.h`
  - [ ] `tests/matchers/entity_memory_pool_matchers.h`
  - [ ] `tests/matchers/cmeta_matchers.h`
  - [ ] `tests/matchers/cuser_interface_matchers.h`
  - [ ] `tests/matchers/cmachina_form_matchers.h`
- [ ] Add level tracking to recursive comparison functions
- [ ] Update test_data_harness.cpp formatting
- [ ] Update TestContext formatting methods

### Phase 3: Fix GENERATE Verbosity
- [ ] Enhance HarnessReporter with instance tracking
  - [ ] Add instance data structures
  - [ ] Implement selective output logic
  - [ ] Track assertion states per instance
- [ ] Update test_data_harness.test.cpp
  - [ ] Test with multiple configs
  - [ ] Verify only failed tests show output
- [ ] Document new behavior

### Phase 4: Visual Improvements
- [ ] Add boxed header function to conmat or console_output
- [ ] Enhance testCaseEnded with status indicators
- [ ] Implement testRunEndedCumulative with summary
- [ ] Add diff-style difference reporting to matchers
- [ ] Update all output functions consistently

### Phase 5: XML Reporting
- [ ] Document XML reporter usage
- [ ] Add CMake option for XML output
- [ ] Create example CI/CD configuration
- [ ] Test XML output with sample tests
- [ ] (Optional) Implement custom HarnessXMLReporter

### Phase 6: Testing and Documentation
- [ ] Write/update tests for all changes
- [ ] Run full test suite
- [ ] Update README with new reporter features
- [ ] Add examples to documentation
- [ ] Create screenshot/examples of new output

## Expected Benefits

1. **Improved Readability**
   - Consistent indentation makes nested data easier to scan
   - Visual hierarchy clarifies structure
   - Color coding highlights important information

2. **Reduced Noise**
   - Only failed tests show verbose output
   - Better signal-to-noise ratio in test results
   - Easier to identify actual problems

3. **Better Debugging**
   - Clear context (test name, tick, description) in failures
   - Structured difference reporting
   - Hierarchical data display

4. **CI/CD Integration**
   - XML reports for automated systems
   - Standardized output format
   - Historical test tracking

5. **Professional Appearance**
   - Clean, well-formatted output
   - Consistent styling
   - Beautiful and functional

## Alternative Approaches Considered

### 1. Custom HTML Reporter
**Pros**: Beautiful, interactive output
**Cons**: Overkill for current needs, requires web view
**Decision**: Defer until XML proves insufficient

### 2. JSON Output Format
**Pros**: Machine-readable, flexible
**Cons**: Not standard for test reporting, requires custom tooling
**Decision**: XML is industry standard for test reports

### 3. Completely Custom Reporter (Not Extending Catch2)
**Pros**: Full control
**Cons**: Loss of Catch2 features, maintenance burden
**Decision**: Extend Catch2, don't replace it

## References

- [Catch2 Documentation - Reporters](https://github.com/catchorg/Catch2/blob/devel/docs/reporters.md)
- [Catch2 Documentation - Generators](https://github.com/catchorg/Catch2/blob/devel/docs/generators.md)
- [conmat Library](https://github.com/walliscode/conmat)
- [JUnit XML Format](https://llg.cubic.org/docs/junit/)

## Notes

- This is a **planning document only** - no implementation yet
- All code examples are illustrative and may require adjustment
- Prioritize maintainability and consistency
- Follow existing SteamRot code style guidelines
- Test all changes thoroughly before merging
