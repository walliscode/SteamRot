# Test Harness Improvements - Visual Examples

This document provides side-by-side visual comparisons of current vs. improved output.

## Table of Contents

1. [Indentation Improvements](#indentation-improvements)
2. [GENERATE Output Reduction](#generate-output-reduction)
3. [Header Formatting](#header-formatting)
4. [Matcher Output](#matcher-output)
5. [Test Summary](#test-summary)
6. [Complete Example](#complete-example)

---

## Indentation Improvements

### Current (Using Tabs)

```
[FAILED] EntityMemoryPool Comparison
============================================================
  Test: ui_workflow_001
  Tick: [3 of 10]
------------------------------------------------------------
  Differences:
    * Entity[0]:
	CMeta:
		m_active: actual=true, expected=false
		m_entity_active: actual=true, expected=true
	CUserInterface:
		m_active: actual=true, expected=false
		ui_name: actual="menu_panel", expected="title_panel"
		root_ui_element:
			position: actual=(100,200), expected=(150,250)
			child_elements[0]:
				position: actual=(10,10), expected=(10,10)
				label: actual="Click Me", expected="Submit"
```

**Issues**:
- Tab width varies by terminal
- Deeply nested content extends far to the right
- Difficult to track indentation level

### Improved (Using conmat::Indent)

```
[FAILED] EntityMemoryPool Comparison
============================================================
  Test: ui_workflow_001
  Tick: [3 of 10]
------------------------------------------------------------
  Differences:
  Entity[0]:
    CMeta:
      m_active:
        - actual:   true
        + expected: false
      m_entity_active: true (matches)
    CUserInterface:
      m_active:
        - actual:   true
        + expected: false
      ui_name:
        - actual:   "menu_panel"
        + expected: "title_panel"
      root_ui_element:
        position:
          - actual:   (100, 200)
          + expected: (150, 250)
        child_elements[0]:
          position: (10, 10) (matches)
          label:
            - actual:   "Click Me"
            + expected: "Submit"
```

**Improvements**:
- Consistent 2-space indentation per level
- Diff-style output (- for actual, + for expected)
- Fields that match shown inline
- Easier to scan and understand hierarchy

---

## GENERATE Output Reduction

### Current Behavior

```
Running Data-Driven Tests
============================================================

Test: test_harness_basic_001.json
INFO: Loading test configuration...
INFO: Creating fixture...
INFO: Executing ticks...
✓ Test passed

Test: test_harness_basic_002.json
INFO: Loading test configuration...
INFO: Creating fixture...
INFO: Executing ticks...
✓ Test passed

Test: test_harness_workflow_001.json
INFO: Loading test configuration...
INFO: Creating fixture...
INFO: Executing ticks...
✗ Test FAILED - Entity mismatch at tick 3

Test: test_harness_workflow_002.json
INFO: Loading test configuration...
INFO: Creating fixture...
INFO: Executing ticks...
✓ Test passed

Test: test_harness_simulation_001.json
INFO: Loading test configuration...
INFO: Creating fixture...
INFO: Executing ticks...
✓ Test passed

============================================================
Tests: 5 | Passed: 4 | Failed: 1
============================================================
```

**Problem**: All 5 tests printed INFO messages, but only 1 failed. This creates 5x the output needed.

### Improved Behavior

```
Running Data-Driven Tests
============================================================

✓ test_harness_basic_001.json
✓ test_harness_basic_002.json

✗ test_harness_workflow_001.json
  INFO: Loading test configuration...
  INFO: Creating fixture...
  INFO: Executing ticks...
  FAILED - Entity mismatch at tick 3
  
  [FAILED] EntityMemoryPool Comparison
  ============================================================
    Test: test_harness_workflow_001
    Tick: [3 of 10]
  ------------------------------------------------------------
    Differences:
    Entity[0]:
      CUserInterface:
        ui_name:
          - actual:   "menu_panel"
          + expected: "title_panel"
  ============================================================

✓ test_harness_workflow_002.json
✓ test_harness_simulation_001.json

============================================================
Test Run Summary
============================================================
Total Tests: 5
  Passed: 4
  Failed: 1
Pass Rate: 80.0%

Failed Tests:
  ✗ test_harness_workflow_001.json (Tick 3)
============================================================
```

**Improvements**:
- Passing tests: Single line with ✓
- Failed tests: Full INFO output + details
- Clear summary at the end
- 80% less verbose output for passing tests

---

## Header Formatting

### Current Headers

```
============================================================
Data Structure Comparison Tests
	test: ui_workflow_001
	tick: [3/10]
============================================================
```

**Issues**:
- Plain dividers
- Inconsistent indentation (tabs)
- No visual hierarchy

### Improved Headers (Option A: Boxed)

```
╔════════════════════════════════════════════════════════════╗
║ Data Structure Comparison Tests                           ║
╟────────────────────────────────────────────────────────────╢
║ Test: ui_workflow_001                                      ║
║ Tick: [3 of 10]                                            ║
╚════════════════════════════════════════════════════════════╝
```

### Improved Headers (Option B: Enhanced ASCII)

```
┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
┃ Data Structure Comparison Tests                           ┃
┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃ Test: ui_workflow_001                                      ┃
┃ Tick: [3 of 10]                                            ┃
┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
```

### Improved Headers (Option C: Colorful)

```
════════════════════════════════════════════════════════════
 DATA STRUCTURE COMPARISON TESTS
════════════════════════════════════════════════════════════
  Test: ui_workflow_001
  Tick: [3 of 10]
────────────────────────────────────────────────────────────
```

*(In actual terminal: title in blue bold, test name in cyan, dividers in yellow)*

**Recommendation**: Option C (Colorful) - Best balance of readability and compatibility.

---

## Matcher Output

### Current CMeta Matcher Output

```
	CMeta Match: 
------------------------------------------------------------
	m_active:
		actual = true
		expected = false
------------------------------------------------------------
	m_entity_active:
		actual = true
		expected = true
------------------------------------------------------------
```

**Issues**:
- Tabs for indentation
- Redundant dividers
- No diff-style comparison

### Improved CMeta Matcher Output

```
CMeta Comparison:
  m_active:
    - actual:   true
    + expected: false
  m_entity_active: true (matches)
```

**Improvements**:
- Consistent 2-space indentation
- Diff-style for mismatches
- Inline display for matches
- No redundant dividers

### Current UIElement Matcher Output (Nested)

```
UIElement types differ: actual=PanelElement, expected=ButtonElement
child[depth=1].position: actual=(10,10), expected=(20,20); 
child[depth=2].label: actual='Submit', expected='Cancel'; 
child[depth=2].is_mouse_over: actual=false, expected=true; 
```

**Issues**:
- All on one line or awkwardly split
- Difficult to parse nested structure
- No visual hierarchy

### Improved UIElement Matcher Output (Nested)

```
UIElement Comparison:
  Type Mismatch:
    - actual:   PanelElement
    + expected: ButtonElement

  Child Elements:
    [depth=1]:
      position:
        - actual:   (10, 10)
        + expected: (20, 20)
    
    [depth=2]:
      label:
        - actual:   "Submit"
        + expected: "Cancel"
      is_mouse_over:
        - actual:   false
        + expected: true
```

**Improvements**:
- Clear hierarchical structure
- Easy to see nesting depth
- Grouped by comparison type
- Scannable and readable

---

## Test Summary

### Current (No Summary)

Tests just end after last assertion. No overall summary provided.

### Improved Summary

```
════════════════════════════════════════════════════════════
 Test Run Summary
════════════════════════════════════════════════════════════
Total Tests: 25
  ✓ Passed: 23
  ✗ Failed: 2
  ⊘ Skipped: 0

Pass Rate: 92.0%

Total Assertions: 487
  ✓ Passed: 485
  ✗ Failed: 2

Duration: 2.34 seconds

Failed Tests:
  ✗ test_harness_workflow_001.json
    - Reason: Entity mismatch at tick 3
  ✗ test_harness_negative_002.json  
    - Reason: Expected failure did not occur

════════════════════════════════════════════════════════════
```

**Benefits**:
- Quick overview of test run
- Pass rate calculation
- List of failed tests with reasons
- Performance metrics

---

## Complete Example

### Before: Current Output

```
------------------------------------------------------------
TEST CASE: 
	name: run_fixture_test works with Catch2 generators
	file: test_data_harness.test.cpp
	line: 150
	tags: [unit][harness]
------------------------------------------------------------
Test name: test_harness_workflow_001
Description: Test UI workflow with multiple ticks

Test name: test_harness_workflow_002
Description: Test another UI workflow

Test name: test_harness_workflow_003
Description: Test yet another UI workflow
============================================================
Data Structure Comparison Tests
	test: test_harness_workflow_003
	tick: [5/10]
============================================================
[FAILED] EntityMemoryPool Comparison
============================================================
  Test: test_harness_workflow_003
  Tick: [5 of 10]
------------------------------------------------------------
  Differences:
    * Entity[2]:
	CUserInterface:
		m_active: actual=true, expected=false
		ui_name: actual="menu", expected="title"
		root_ui_element:
			position: actual=(100,200), expected=(150,250)
			size: actual=(300,400), expected=(350,450)
			child_elements[0]:
				label: actual="Click", expected="Submit"
============================================================

Result: 1 test failed
```

### After: Improved Output

```
┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
┃ TEST CASE: run_fixture_test works with Catch2 generators ┃
┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃ File: test_data_harness.test.cpp:150                      ┃
┃ Tags: [unit][harness]                                     ┃
┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

Running Data-Driven Tests...

✓ test_harness_workflow_001
✓ test_harness_workflow_002

✗ test_harness_workflow_003
  Description: Test yet another UI workflow

════════════════════════════════════════════════════════════
 Data Structure Comparison Tests
════════════════════════════════════════════════════════════
  Test: test_harness_workflow_003
  Tick: [5 of 10]
────────────────────────────────────────────────────────────

[FAILED] EntityMemoryPool Comparison

Entity[2]:
  CUserInterface:
    m_active:
      - actual:   true
      + expected: false
    ui_name:
      - actual:   "menu"
      + expected: "title"
    root_ui_element:
      position:
        - actual:   (100, 200)
        + expected: (150, 250)
      size:
        - actual:   (300, 400)
        + expected: (350, 450)
      child_elements[0]:
        label:
          - actual:   "Click"
          + expected: "Submit"

════════════════════════════════════════════════════════════

✗ Test failed: see details above
```

### Key Improvements in Complete Example

1. **Reduced Noise**: Only failed test shows full details
2. **Clear Structure**: Boxed headers, consistent indentation
3. **Diff-Style Output**: Easy to spot differences
4. **Visual Hierarchy**: Indentation shows nesting
5. **Status Indicators**: ✓ and ✗ for quick scanning
6. **Color Coding**: (In terminal) Colors highlight important info
7. **Scannable**: Can quickly find failures

---

## XML Output Example

### Generated XML Report

```xml
<?xml version="1.0" encoding="UTF-8"?>
<Catch2TestRun name="test_harness" filters="*">
  <Group name="test_harness">
    <TestCase name="run_fixture_test works with Catch2 generators" 
              tags="[unit][harness]" 
              filename="test_data_harness.test.cpp" 
              line="150">
      <Section name="test_harness_workflow_001">
        <OverallResults successes="10" failures="0"/>
      </Section>
      <Section name="test_harness_workflow_002">
        <OverallResults successes="10" failures="0"/>
      </Section>
      <Section name="test_harness_workflow_003">
        <Expression success="false" 
                    type="REQUIRE" 
                    filename="test_data_harness.cpp" 
                    line="358">
          <Original>result.has_value()</Original>
          <Expanded>false</Expanded>
          <Info>Entity mismatch at tick 5</Info>
        </Expression>
        <OverallResults successes="10" failures="1"/>
      </Section>
      <OverallResults successes="30" failures="1"/>
    </TestCase>
  </Group>
  <OverallResults successes="30" failures="1"/>
</Catch2TestRun>
```

### CI/CD Display (GitHub Actions)

When uploaded to GitHub Actions, this XML is displayed as:

```
Test Results

❌ run_fixture_test works with Catch2 generators
   ✅ test_harness_workflow_001 (10 assertions)
   ✅ test_harness_workflow_002 (10 assertions)  
   ❌ test_harness_workflow_003 (10 pass, 1 fail)
      → Entity mismatch at tick 5

Total: 30 passed, 1 failed
```

---

## Color Reference

### Color Usage in Terminal Output

**Status Colors**:
- ✓ Green - Success
- ✗ Red - Failure
- ⊘ Yellow - Skipped
- • Blue - Info

**Comparison Colors**:
- Red - Actual values (what we got)
- Blue - Expected values (what we wanted)
- Green - Matching values
- Yellow - Headers and dividers

**Example with Color Annotations**:

```
[FAILED] EntityMemoryPool Comparison          ← Red bold
════════════════════════════════════════      ← Yellow
  Test: test_workflow_003                     ← Cyan
────────────────────────────────────────      ← Yellow

Entity[2]:                                    ← Bold white
  CUserInterface:                             ← White
    m_active:                                 ← White
      - actual:   true                        ← Red
      + expected: false                       ← Blue
    ui_name:                                  ← White  
      - actual:   "menu"                      ← Red
      + expected: "title"                     ← Blue

════════════════════════════════════════      ← Yellow
```

---

## Environment Variable Control

### Color Control

```bash
# Enable colors (default)
./test_harness

# Disable colors
NO_COLOR=1 ./test_harness
STEAMROT_NO_COLOR=1 ./test_harness

# Force colors even if not a TTY
FORCE_COLOR=1 ./test_harness
```

### Output Format Control

```bash
# Console output (improved)
./test_harness -r harness

# XML output
./test_harness -r xml -o results.xml

# Both
./test_harness -r harness -r xml::out=results.xml

# Verbose (shows passing tests too)
./test_harness -r harness -v

# Quiet (minimal output)
./test_harness -r harness -q
```

---

## Implementation Notes

### Character Sets

**Unicode Box Characters** (Requires UTF-8 terminal):
```
╔═══╗  ┏━━━┓  ┌───┐  ╭───╮
║   ║  ┃   ┃  │   │  │   │
╚═══╝  ┗━━━┛  └───┘  ╰───╯
```

**ASCII Fallback** (Universal):
```
+===+  /---\  .---.
|   |  |   |  |   |
+===+  \---/  '---'
```

**Detection**:
```cpp
bool SupportsUnicode() {
  const char* lang = std::getenv("LANG");
  return lang && std::string(lang).find("UTF-8") != std::string::npos;
}
```

### Indentation Levels

**Recommended Structure**:
```
Level 0: Main headers, test case names
Level 1: Component names, section headers
Level 2: Field names
Level 3: Sub-field names, nested structures
Level 4+: Deep nesting (UIElements, etc.)
```

**Example**:
```
Entity[0]:                              // Level 0
  CUserInterface:                       // Level 1
    root_ui_element:                    // Level 2
      position: (100, 200)              // Level 3
      child_elements[0]:                // Level 3
        position: (10, 10)              // Level 4
        child_elements[0]:              // Level 4
          label: "Deep"                 // Level 5
```

---

## Conclusion

These visual improvements provide:

✅ **Clarity** - Easy to understand test results at a glance  
✅ **Efficiency** - Only show details when needed  
✅ **Professionalism** - Clean, consistent formatting  
✅ **Debuggability** - Clear hierarchical structure  
✅ **Compatibility** - Works in various terminal environments  

The combination of improved indentation, selective output, enhanced formatting, and XML support creates a modern, professional test reporting system suitable for both development and CI/CD environments.
