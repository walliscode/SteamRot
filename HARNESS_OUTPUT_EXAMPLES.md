# Test Harness Output Examples

This document shows before/after examples of the proposed output improvements for the test harness and HarnessReporter.

---

## Problem 1: GENERATE Verbosity

### Current Behavior (Before)

When using `GENERATE` with multiple test data files, ALL iterations print full test case info:

```
========================================
Starting Data Driven Harness Tests
========================================
----------------------------------------
TEST CASE: 
	name: run_fixture_test works with Catch2 generators
	file: test_data_harness.test.cpp
	line: 150
	tags: [unit][harness]
----------------------------------------
Test name: sample_test_1
(test passes silently)

----------------------------------------
TEST CASE: 
	name: run_fixture_test works with Catch2 generators
	file: test_data_harness.test.cpp
	line: 150
	tags: [unit][harness]
----------------------------------------
Test name: sample_test_2
(test passes silently)

----------------------------------------
TEST CASE: 
	name: run_fixture_test works with Catch2 generators
	file: test_data_harness.test.cpp
	line: 150
	tags: [unit][harness]
----------------------------------------
Test name: sample_test_3
(test passes silently)

... repeated for 17 test data files ...
```

**Problems:**
- Repeats test case header 17 times
- Hard to find failures in the noise
- Wastes screen space
- No summary of results

### Proposed Behavior (After)

Test case header printed ONCE, brief status for each iteration, summary at end:

```
============================================================
Starting Data Driven Harness Tests
============================================================

============================================================
TEST CASE: run_fixture_test works with Catch2 generators
  file: test_data_harness.test.cpp:150
  tags: [unit][harness]
============================================================

✓ sample_test_1
✓ sample_test_2
✓ sample_test_3
✓ sample_event_sequence
✓ sample_input_sequence
✓ sample_simulation_test
✓ sample_function_simulation
✓ sample_input_event_simulation
✓ sample_tick_based_execution
✓ sample_complete_workflow
✓ event_bus_snapshot_test
✓ tick_snapshot_example
✓ simple_event_bus_test
✓ metadata_validation

❌ sample_mismatch_test
  Description: Test with intentional mismatch for validation
  
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

✓ sample_pool_size_mismatch

------------------------------------------------------------
Summary: 15/16 passed (1 FAILED)
------------------------------------------------------------
```

**Benefits:**
- Clear at-a-glance status
- Easy to spot failures
- Compact output for passing tests
- Detailed info only for failures
- Summary statistics

---

## Problem 2: Nested Data Indentation

### Current Behavior (Before)

Uses tabs, inconsistent spacing:

```
----------------------------------------
❌ CMeta Match: 
	❌ m_active:
		actual = false
		expected = true
	❌ m_entity_active:
		actual = 5
		expected = 0
----------------------------------------

----------------------------------------
❌ EntityMemoryPool Mismatch
	Entity 0:
		Component: CUserInterface
			❌ m_ui_name
				actual = "wrong_name"
				expected = "test_ui"
			root_ui_element:
				position:
					❌ x
						actual = 200
						expected = 100
```

**Problems:**
- Tabs are too wide (8 spaces typically)
- Inconsistent nesting levels
- Hard to follow hierarchy
- Misaligned key-value pairs

### Proposed Behavior (After)

Uses 2-space indentation, consistent hierarchy:

```
============================================================
❌ CMeta Match
  ❌ m_active
    actual:   false
    expected: true
  ❌ m_entity_active
    actual:   5
    expected: 0
============================================================

============================================================
❌ EntityMemoryPool Mismatch
  Entity 0
    Component: CUserInterface
      ❌ m_ui_name
        actual:   "wrong_name"
        expected: "test_ui"
      
      root_ui_element
        ✓ position: (100, 200)
        ✓ size: (50, 30)
        
        children
          [0] Button
            ❌ m_active
              actual:   false
              expected: true
            ✓ position: (10, 10)
            ✓ size: (20, 15)
============================================================
```

**Benefits:**
- Consistent 2-space indentation
- Clear visual hierarchy
- Aligned actual/expected values
- Easy to scan and understand
- Matches project coding style

---

## Problem 3: Test Case Header Formatting

### Current Behavior (Before)

```
----------------------------------------
TEST CASE: 
	name: run_fixture_test works with Catch2 generators
	file: test_data_harness.test.cpp
	line: 150
	tags: [unit][harness]
----------------------------------------
```

**Problems:**
- Awkward "TEST CASE:" on separate line
- Indented fields look cluttered
- Short dividers (40 chars)
- No color distinction

### Proposed Behavior (After)

```
============================================================
TEST CASE: run_fixture_test works with Catch2 generators
  file: test_data_harness.test.cpp:150
  tags: [unit][harness]
============================================================
```

**Benefits:**
- Title on same line as label
- File and line combined
- Longer dividers (60 chars) for better separation
- Blue color for headers (if terminal supports)
- Cleaner, more professional look

---

## Problem 4: Comparison Output

### Current Behavior (Before)

```
========================================
Data Structure Comparison Tests
	test: sample_test_1
	tick: N/A
========================================

Entity 0, Component: CUserInterface:
	actual: m_ui_name = "test_ui", m_active = false
	expected: m_ui_name = "test_ui", m_active = true

Component mismatch
```

**Problems:**
- Unclear what failed
- All data mixed together
- No visual hierarchy
- Hard to spot differences

### Proposed Behavior (After)

```
============================================================
Data Structure Comparison Tests
  test: sample_test_1
  description: Validates UI component configuration
  tick: N/A
============================================================

Entity 0
  Component: CUserInterface
    ✓ m_ui_name: "test_ui"
    ❌ m_active
      actual:   false
      expected: true
    ✓ m_start_visible: true
    
    root_ui_element
      ✓ position: (100, 200)
      ✓ size: (50, 30)
      ✓ layout: Horizontal
      ✓ spacing_strategy: None

------------------------------------------------------------
❌ EntityMemoryPool Comparison Failed
  1 component mismatch
  See details above
------------------------------------------------------------
```

**Benefits:**
- Clear pass/fail indicators (✓/❌)
- Hierarchical structure
- Only mismatches shown in detail
- Easy to identify problems
- Summary at the bottom

---

## Problem 5: EventBus Comparison

### Current Behavior (Before)

```
========================================
Global Event Bus Comparison
========================================
EventBus mismatch:
Expected 2 events, got 3
Event 0: lifetime 1, type EVENT_TEST
Event 1: lifetime 2, type EVENT_USER_INPUT
Event 2: lifetime 3, type EVENT_TEST (unexpected)
```

**Problems:**
- Linear list format
- No structure
- Hard to compare events
- Unclear what's different

### Proposed Behavior (After)

```
============================================================
Global Event Bus Comparison
  test: event_bus_snapshot_test
  tick: 2
============================================================

EventBus State
  ✓ Event count: 2
  
  Events
    [0] EVENT_TEST
      ❌ lifetime
        actual:   3
        expected: 1
      ✓ event_type: EVENT_TEST
      ✓ event_data: NONE
    
    [1] EVENT_USER_INPUT
      ✓ lifetime: 2
      ✓ event_type: EVENT_USER_INPUT
      event_data: UserInputBitsetData
        ✓ keyboard_pressed: [A, W]
        ✓ mouse_pressed: [LEFT_CLICK]

------------------------------------------------------------
❌ EventBus Comparison Failed
  1 event mismatch (lifetime)
------------------------------------------------------------
```

**Benefits:**
- Structured event listing
- Clear pass/fail per field
- Nested event_data shown properly
- Easy to spot mismatches
- Professional formatting

---

## Problem 6: Tick-Based Test Output

### Current Behavior (Before)

Minimal or no tick progress indication:

```
Executing test: sample_tick_based_execution
(silence while ticks execute)
Done
```

**Problems:**
- No progress indication
- Can't tell which tick failed
- No context for failures

### Proposed Behavior (After)

```
============================================================
TEST CASE: sample_tick_based_execution
  file: test_data_harness.test.cpp:136
  tags: [unit][harness][tick]
============================================================

Description: Demonstrates tick-based test execution

------------------------------------------------------------
Tick-Based Execution
------------------------------------------------------------

Total ticks to execute: 5

➤ Executing Tick 1 of 5
  • Inputs: 1 event(s)
  • Events: 1 event(s)
  • Simulation: 2 step(s)
✓ Tick 1 completed

➤ Executing Tick 2 of 5
  • Inputs: 0 event(s)
  • Events: 0 event(s)
  • Simulation: 2 step(s)
✓ Tick 2 completed

➤ Executing Tick 3 of 5
  • Inputs: 1 event(s)
  • Events: 0 event(s)
  • Simulation: 2 step(s)
  • Comparing snapshot
✓ Tick 3 completed

➤ Executing Tick 4 of 5
  • Inputs: 0 event(s)
  • Events: 1 event(s)
  • Simulation: 2 step(s)
✓ Tick 4 completed

➤ Executing Tick 5 of 5
  • Inputs: 0 event(s)
  • Events: 0 event(s)
  • Simulation: 2 step(s)
✓ Tick 5 completed

✓ All 5 ticks executed successfully

------------------------------------------------------------
Final State Comparison
------------------------------------------------------------

✓ EntityMemoryPool comparison passed
✓ EventBus comparison passed

------------------------------------------------------------
✓ Test Passed
------------------------------------------------------------
```

**Benefits:**
- Clear progress indication
- Context for each tick
- Easy to identify failing tick
- Summary of actions per tick
- Professional, informative output

---

## Color Scheme

All output respects the `NO_COLOR` and `STEAMROT_NO_COLOR` environment variables.

**When colors are enabled:**

- **Blue** (`\033[34m`): Headers, section titles
  - "TEST CASE:", "Data Structure Comparison Tests"
  
- **Green** (`\033[32m`): Success indicators
  - ✓ marks, passing test names, "PASSED"
  
- **Red** (`\033[31m`): Failure indicators
  - ❌ marks, failing test names, "FAILED"
  
- **Cyan** (`\033[36m`): Contextual information
  - Tick numbers `[Tick 3]`, file names, line numbers
  
- **Yellow** (`\033[33m`): Dividers, section separators
  - `━━━━`, `════`, `────`
  
- **Magenta** (`\033[35m`): Progress indicators
  - ➤ arrows for tick progress

**When colors are disabled:**
- All output uses plain text
- Symbols (✓, ❌, ➤, •) are preserved
- Dividers are preserved
- Still readable and organized

---

## Summary of Improvements

### Before
- ❌ Repeated test headers for every GENERATE iteration
- ❌ Tab-based indentation (too wide)
- ❌ No summary statistics
- ❌ Unclear failure details
- ❌ No progress indication
- ❌ Inconsistent formatting

### After
- ✅ Single test header with iteration status
- ✅ Consistent 2-space indentation
- ✅ Summary statistics (X/Y passed)
- ✅ Clear, hierarchical failure details
- ✅ Tick progress indicators
- ✅ Professional, consistent formatting
- ✅ Easy to scan and debug
- ✅ Respects NO_COLOR environment variable

---

## XML Report Format

For CI/CD integration, Catch2 can output JUnit XML:

```bash
./test_harness -r junit -o test_results.xml
```

Example output:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<testsuites>
  <testsuite name="test_harness" tests="16" failures="1" errors="0" time="2.456">
    <testcase classname="harness" 
              name="run_fixture_test works with Catch2 generators[sample_test_1]" 
              time="0.145">
      <system-out>Test name: sample_test_1</system-out>
    </testcase>
    <testcase classname="harness" 
              name="run_fixture_test works with Catch2 generators[sample_test_2]" 
              time="0.132">
      <system-out>Test name: sample_test_2</system-out>
    </testcase>
    <testcase classname="harness" 
              name="run_fixture_test works with Catch2 generators[sample_mismatch_test]" 
              time="0.156">
      <failure message="EntityMemoryPool mismatch" type="CHECK_THAT">
Entity 0, Component: CUserInterface
  m_active mismatch
    actual: false
    expected: true
      </failure>
    </testcase>
  </testsuite>
</testsuites>
```

This format is compatible with:
- Jenkins
- GitHub Actions
- GitLab CI
- Azure Pipelines
- TeamCity
- CircleCI
- Most CI/CD systems

---

## Implementation Notes

1. **Indentation functions** need to be added to conmat library or implemented locally
2. **State tracking** needs to be added to HarnessReporter for GENERATE handling
3. **All matchers** need to be updated to use new indentation system
4. **Test thoroughly** with and without colors
5. **Document** in XML_REPORT_GUIDE.md

See `ENHANCE_HARNESS_OUTPUT_PLAN.md` for full implementation details.
