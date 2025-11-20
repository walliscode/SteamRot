# XML Report Generation Guide

This guide explains how to generate XML test reports using Catch2's built-in reporting capabilities.

---

## Overview

**No custom implementation is needed** for XML reporting. Catch2 provides built-in support for multiple report formats:

- **JUnit XML** (recommended for CI/CD)
- **Native Catch2 XML** (detailed test information)
- **JSON** (for custom processing)

---

## Quick Start

### Generate JUnit XML Report

```bash
# Using ctest
ctest --preset Debug --output-junit test_results.xml

# Using Catch2 executable directly
./test_harness -r junit -o test_results.xml

# Run specific tests and generate XML
./test_harness -r junit -o results.xml "[unit][harness]"
```

### Generate Native Catch2 XML

```bash
# More detailed than JUnit XML
./test_harness -r xml -o test_results.xml
```

### Generate JSON Report

```bash
# For custom parsing/analysis
./test_harness -r json -o test_results.json
```

---

## Report Formats

### JUnit XML Format

**Best for:** CI/CD integration (Jenkins, GitHub Actions, GitLab CI, etc.)

**Example output:**
```xml
<?xml version="1.0" encoding="UTF-8"?>
<testsuites>
  <testsuite name="test_harness" 
             tests="16" 
             failures="1" 
             errors="0" 
             time="2.456" 
             timestamp="2024-01-15T10:30:00Z">
    
    <!-- Passing test -->
    <testcase classname="harness" 
              name="run_fixture_test works with Catch2 generators[sample_test_1]" 
              time="0.145">
      <system-out>
Test name: sample_test_1
Description: Sample test data for testing the harness functionality
      </system-out>
    </testcase>
    
    <!-- Passing test -->
    <testcase classname="harness" 
              name="run_fixture_test works with Catch2 generators[sample_test_2]" 
              time="0.132">
      <system-out>Test name: sample_test_2</system-out>
    </testcase>
    
    <!-- Failing test -->
    <testcase classname="harness" 
              name="run_fixture_test works with Catch2 generators[sample_mismatch_test]" 
              time="0.156">
      <failure message="EntityMemoryPool mismatch" type="CHECK_THAT">
========================================
Data Structure Comparison Tests
  test: sample_mismatch_test
  tick: N/A
========================================

EntityMemoryPool Mismatch:
  Entity 0, Component: CUserInterface
    m_active mismatch
      actual:   false
      expected: true
      </failure>
    </testcase>
    
    <!-- More test cases... -->
    
  </testsuite>
</testsuites>
```

**Key elements:**
- `<testsuites>` - Root element
- `<testsuite>` - Group of tests (usually one per executable)
  - `tests` - Total number of test cases
  - `failures` - Number of failed test cases
  - `errors` - Number of test cases with errors
  - `time` - Total execution time in seconds
- `<testcase>` - Individual test case
  - `name` - Test name (includes GENERATE iterations in brackets)
  - `classname` - Test suite/category
  - `time` - Execution time for this test
- `<failure>` - Present only for failing tests
  - `message` - Brief failure description
  - `type` - Assertion type (CHECK, REQUIRE, CHECK_THAT, etc.)
  - Text content - Full failure details
- `<system-out>` - Standard output captured during test

### Native Catch2 XML Format

**Best for:** Detailed analysis, custom processing

**Example output:**
```xml
<?xml version="1.0" encoding="UTF-8"?>
<Catch2TestRun name="test_harness" rng-seed="12345" catch2-version="3.5.0">
  
  <TestCase name="run_fixture_test works with Catch2 generators" 
            tags="[unit][harness]" 
            filename="test_data_harness.test.cpp" 
            line="150">
    
    <!-- Successful section -->
    <Section name="sample_test_1">
      <Info>Test name: sample_test_1</Info>
      <Info>Description: Sample test data for testing the harness functionality</Info>
      <OverallResult success="true" durationInSeconds="0.145"/>
    </Section>
    
    <!-- Failed section -->
    <Section name="sample_mismatch_test">
      <Info>Test name: sample_mismatch_test</Info>
      <Expression success="false" 
                  type="CHECK_THAT" 
                  filename="test_data_harness.cpp" 
                  line="247">
        <Original>
          actual_pool, EqualsEntityMemoryPool(expected_pool, context)
        </Original>
        <Expanded>
          EntityMemoryPool mismatch: Entity 0, Component CUserInterface: m_active mismatch
        </Expanded>
      </Expression>
      <OverallResult success="false" durationInSeconds="0.156"/>
    </Section>
    
    <OverallResult success="false" durationInSeconds="2.456"/>
  </TestCase>
  
  <OverallResults successes="15" failures="1" expectedFailures="0"/>
</Catch2TestRun>
```

**Key elements:**
- `<TestCase>` - Test case with full details
  - `tags` - All tags assigned to the test
  - `filename` and `line` - Source location
- `<Section>` - Each GENERATE iteration or test section
- `<Expression>` - Each assertion with original and expanded forms
- `<Info>` - INFO() messages from the test
- More detailed than JUnit XML format

### JSON Format

**Best for:** Custom parsing, data analysis tools

**Example output:**
```json
{
  "version": 1,
  "name": "test_harness",
  "testCases": [
    {
      "name": "run_fixture_test works with Catch2 generators",
      "tags": ["unit", "harness"],
      "filename": "test_data_harness.test.cpp",
      "line": 150,
      "sections": [
        {
          "name": "sample_test_1",
          "assertions": 1,
          "duration": 0.145,
          "result": "passed"
        },
        {
          "name": "sample_mismatch_test",
          "assertions": 1,
          "duration": 0.156,
          "result": "failed",
          "failures": [
            {
              "type": "CHECK_THAT",
              "filename": "test_data_harness.cpp",
              "line": 247,
              "message": "EntityMemoryPool mismatch"
            }
          ]
        }
      ]
    }
  ],
  "summary": {
    "total": 16,
    "passed": 15,
    "failed": 1
  }
}
```

---

## CI/CD Integration

### GitHub Actions

```yaml
name: Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Configure CMake
      run: cmake --preset Debug
      
    - name: Build
      run: cmake --build --preset Debug
      
    - name: Run Tests with XML Output
      run: ctest --preset Debug --output-junit test_results.xml
      continue-on-error: true  # Generate report even if tests fail
      
    - name: Publish Test Results
      uses: dorny/test-reporter@v1
      if: always()  # Run even if tests failed
      with:
        name: Test Results
        path: test_results.xml
        reporter: java-junit
        fail-on-error: true
```

**Result:** Test results displayed in GitHub Actions UI with pass/fail indicators.

### Jenkins

```groovy
pipeline {
  agent any
  
  stages {
    stage('Build') {
      steps {
        sh 'cmake --preset Debug'
        sh 'cmake --build --preset Debug'
      }
    }
    
    stage('Test') {
      steps {
        sh 'ctest --preset Debug --output-junit test_results.xml'
      }
      post {
        always {
          // Publish test results even if tests fail
          junit 'test_results.xml'
        }
      }
    }
  }
}
```

**Result:** Test results displayed in Jenkins UI with trends and history.

### GitLab CI

```yaml
test:
  stage: test
  script:
    - cmake --preset Debug
    - cmake --build --preset Debug
    - ctest --preset Debug --output-junit test_results.xml
  artifacts:
    when: always
    reports:
      junit: test_results.xml
```

**Result:** Test results displayed in GitLab merge request UI.

### Azure Pipelines

```yaml
- task: CMake@1
  inputs:
    cmakeArgs: '--preset Debug'

- task: CMake@1
  inputs:
    cmakeArgs: '--build --preset Debug'

- script: ctest --preset Debug --output-junit test_results.xml
  displayName: 'Run Tests'

- task: PublishTestResults@2
  inputs:
    testResultsFormat: 'JUnit'
    testResultsFiles: 'test_results.xml'
    failTaskOnFailedTests: true
  condition: always()
```

**Result:** Test results displayed in Azure Pipelines UI.

---

## Advanced Usage

### Multiple Report Formats

Generate multiple formats simultaneously using reporters list:

```bash
# Generate both console and JUnit XML
./test_harness -r console -r junit::out=test_results.xml

# Generate console, XML, and JSON
./test_harness -r console -r xml::out=detailed.xml -r json::out=report.json
```

### Filter Tests with XML Output

```bash
# Run only unit tests and generate XML
./test_harness -r junit -o unit_tests.xml "[unit]"

# Run only harness tests
./test_harness -r junit -o harness_tests.xml "[harness]"

# Run tests matching pattern
./test_harness -r junit -o fixture_tests.xml "fixture*"
```

### Append to Existing XML

```bash
# Useful for combining results from multiple test executables
./test_harness_1 -r junit -o all_results.xml
./test_harness_2 -r junit::out=all_results.xml::mode=append
./test_harness_3 -r junit::out=all_results.xml::mode=append
```

---

## Custom Reporter with HarnessReporter

You can use HarnessReporter for console output while also generating XML:

```bash
# Beautiful console output + XML report
./test_harness --reporter harness --reporter junit::out=results.xml

# Or with ctest
ctest --preset Debug --output-junit results.xml --verbose
```

**Best of both worlds:**
- Readable, formatted console output during development
- XML reports for CI/CD integration
- No custom implementation needed

---

## Parsing XML Reports

### Python Example

```python
import xml.etree.ElementTree as ET

def parse_junit_xml(filename):
    tree = ET.parse(filename)
    root = tree.getroot()
    
    results = {
        'total': 0,
        'passed': 0,
        'failed': 0,
        'failures': []
    }
    
    for testsuite in root.findall('testsuite'):
        results['total'] += int(testsuite.get('tests', 0))
        results['failed'] += int(testsuite.get('failures', 0))
        
        for testcase in testsuite.findall('testcase'):
            failure = testcase.find('failure')
            if failure is not None:
                results['failures'].append({
                    'name': testcase.get('name'),
                    'message': failure.get('message'),
                    'details': failure.text
                })
    
    results['passed'] = results['total'] - results['failed']
    return results

# Usage
results = parse_junit_xml('test_results.xml')
print(f"Total: {results['total']}, Passed: {results['passed']}, Failed: {results['failed']}")

for failure in results['failures']:
    print(f"\nFailed: {failure['name']}")
    print(f"Message: {failure['message']}")
    print(f"Details:\n{failure['details']}")
```

### Shell Script Example

```bash
#!/bin/bash

# Parse JUnit XML with xmllint
TOTAL=$(xmllint --xpath "sum(//testsuite/@tests)" test_results.xml)
FAILURES=$(xmllint --xpath "sum(//testsuite/@failures)" test_results.xml)
PASSED=$((TOTAL - FAILURES))

echo "Test Results:"
echo "  Total:   $TOTAL"
echo "  Passed:  $PASSED"
echo "  Failed:  $FAILURES"

# Exit with failure if any tests failed
if [ $FAILURES -gt 0 ]; then
    echo ""
    echo "Failed tests:"
    xmllint --xpath "//testcase[failure]/@name" test_results.xml | \
        sed 's/ name="/\n  - /g' | sed 's/"//g' | tail -n +2
    exit 1
fi
```

---

## Best Practices

### 1. Always Generate XML in CI/CD

```yaml
# Good: Generate XML even if tests fail
- name: Run Tests
  run: ctest --preset Debug --output-junit results.xml
  continue-on-error: true

- name: Publish Results
  if: always()
  uses: dorny/test-reporter@v1
  with:
    path: results.xml
```

### 2. Use Descriptive Test Names

Test names appear in XML reports, so make them clear:

```cpp
// Good: Descriptive name
TEST_CASE("EntityMemoryPool comparison detects component mismatches", "[unit][matcher]")

// Bad: Vague name
TEST_CASE("Test 1", "[unit]")
```

### 3. Include Context with INFO()

INFO messages appear in XML output:

```cpp
TEST_CASE("Data-driven test", "[unit]") {
    auto configs = load_test_data_configs();
    const auto *config = GENERATE_COPY(from_range(configs.value()));
    
    // This appears in XML
    INFO("Test name: " << config->metadata()->test_name()->str());
    INFO("Description: " << config->metadata()->description()->str());
    
    // Test logic...
}
```

### 4. Separate Test Executables by Category

Makes XML reports more organized:

```
test_components.xml      # Component tests
test_logic.xml          # Logic tests
test_systems.xml        # System tests
test_integration.xml    # Integration tests
```

### 5. Archive Test Results

Keep historical data for trend analysis:

```bash
# Archive with timestamp
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
./test_harness -r junit -o "results_${TIMESTAMP}.xml"

# Or use git commit hash
COMMIT=$(git rev-parse --short HEAD)
./test_harness -r junit -o "results_${COMMIT}.xml"
```

---

## Troubleshooting

### Problem: No XML file generated

**Solution:** Check reporter syntax:
```bash
# Wrong
./test_harness -r junit test_results.xml

# Correct
./test_harness -r junit -o test_results.xml
# Or
./test_harness -r junit::out=test_results.xml
```

### Problem: XML is empty or incomplete

**Solution:** Ensure test executable completes:
```bash
# Add timeout to prevent hanging
timeout 300 ./test_harness -r junit -o results.xml
```

### Problem: XML not parsed by CI system

**Solution:** Validate XML format:
```bash
# Check XML is well-formed
xmllint --noout test_results.xml

# View formatted XML
xmllint --format test_results.xml
```

### Problem: Test names are truncated

**Solution:** This is normal for GENERATE tests. Each iteration gets a unique name:
```
run_fixture_test[sample_test_1]
run_fixture_test[sample_test_2]
run_fixture_test[sample_test_3]
```

---

## Summary

**Key Points:**
- ✅ No custom implementation needed - use Catch2's built-in XML reporters
- ✅ JUnit XML format is best for CI/CD integration
- ✅ Native XML format provides more detail for custom processing
- ✅ JSON format available for programmatic analysis
- ✅ Can generate multiple formats simultaneously
- ✅ All major CI/CD systems support JUnit XML out of the box
- ✅ Compatible with HarnessReporter for beautiful console output

**Recommended Command:**
```bash
# For CI/CD
ctest --preset Debug --output-junit test_results.xml

# For development (console + XML)
./test_harness --reporter harness --reporter junit::out=results.xml
```

**Further Reading:**
- [Catch2 Documentation - Reporters](https://github.com/catchorg/Catch2/blob/devel/docs/reporters.md)
- [JUnit XML Format Specification](https://www.ibm.com/docs/en/developer-for-zos/14.1?topic=formats-junit-xml-format)
- [GitHub Actions Test Reporter](https://github.com/dorny/test-reporter)

---

**Document Status:** Plan/Documentation Only  
**Last Updated:** 2024-01-15  
**Part of:** Test Harness Enhancement Plan
