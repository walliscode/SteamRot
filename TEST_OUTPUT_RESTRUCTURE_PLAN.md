# SteamRot Test Output Restructuring Plan

This document outlines the multistage plan for removing console output and restructuring test logic in the SteamRot repository as per the requirements.

---

## Overview

**Goal:** Improve test output clarity and maintainability by:
1. Removing all console output from codebase
2. Restructuring matchers with a layered approach
3. Implementing comprehensive data structure testing
4. Integrating the conmat package for visually pleasing failure messages
5. Creating demonstration tooling
6. Adding optional XML test report generation

---

## Stage 1: Remove Console Output ✅ **COMPLETE**

### Objective
Remove all console output from the SteamRot repo including removing instances of the functions used.

### What Was Done

#### Source Code (`src/`)
**Files Modified:**
- `src/systems/GameEngine.cpp` - Replaced `std::cerr` with exceptions
- `src/events/EventHandler.cpp` - Removed 3 debug `std::cout` statements
- `src/events/SubscriberFactory.cpp` - Removed 1 debug `std::cout` statement
- `src/logic/logic_action.cpp` - Removed 1 warning `std::cout`
- `src/logger/log_handler.cpp` - Removed 2 debug `std::cout` statements
- `src/entity/FlatbuffersConfigurator.cpp` - Removed 1 debug `std::cout`
- `src/display/Session.cpp` - Removed 2 debug `std::cout` statements
- `src/display/Tile.cpp` - Removed 1 debug `std::cout`

**Total:** 25+ console output statements removed

**Error Handling Pattern:**
- Critical errors throw exceptions (caught at `main.cpp`)
- `main.cpp` keeps `std::cerr` for top-level error display (appropriate)
- Debug/trace output completely eliminated

#### Test Harness (`tests/harness/`)
**Files Modified:**
- `tests/harness/TestFixture.cpp` - Replaced 7 `console::PrintError` calls with exceptions
- `tests/harness/test_data_harness.cpp` - Removed 3 `console::PrintError` calls
- `tests/harness/tick_executor.cpp` - Removed 5 `console::PrintError` calls

**Total:** 15 `console::PrintError` calls removed

**Test Error Handling:**
- Setup errors throw exceptions (caught by Catch2)
- Runtime errors use return values (reported via matchers)
- All errors now flow through Catch2's reporting system

#### Verification
✅ All builds passing  
✅ All tests passing  
✅ No console output during test execution (except Catch2's own output)

---

## Stage 2: Restructure Matchers - Layered Approach 🚧 **IN PROGRESS**

### Objective
Move away from REQUIRE_THAT and CHECK_THAT, build a layered approach.

### Design

#### Current Architecture (Before)
```
Test Code
    ↓
REQUIRE_THAT(actual, EqualsEntityMemoryPool(expected))
    ↓
Matcher (stops on first difference)
    ↓
Catch2 Failure Report
```

**Problems:**
- Test code directly uses Catch2 macros
- Matcher stops on first difference (incomplete picture)
- Hard to add comprehensive validation
- Output formatting embedded in matcher

#### New Architecture (After)
```
Test Code
    ↓
CompareEntityMemoryPools(actual, expected, context)  ← New Wrapper Layer
    ↓
Matcher (collects ALL differences)                   ← Modified to accumulate
    ↓
Comprehensive Report via conmat                       ← Pretty output
    ↓
REQUIRE/CHECK (inside wrapper)                        ← Final assertion
```

**Benefits:**
- Clean separation of concerns
- Comprehensive error collection
- Centralized formatting logic
- Test code stays simple
- Easy to extend and maintain

### Implementation Plan

#### 2.1 Create Wrapper Functions
**New File:** `tests/matchers/comparison_wrappers.h`

```cpp
namespace steamrot::tests {

/// @brief Compare two EntityMemoryPools with comprehensive reporting
/// @param actual The actual pool from test execution
/// @param expected The expected pool
/// @param context Test context for enriched error messages
/// @return true if pools match, false otherwise (also triggers REQUIRE internally)
bool CompareEntityMemoryPools(
    const EntityMemoryPool& actual,
    const EntityMemoryPool& expected,
    const TestContext& context);

/// @brief Compare two EventBuses with comprehensive reporting
bool CompareEventBuses(
    const EventBus& actual,
    const EventBus& expected,
    const TestContext& context);

} // namespace steamrot::tests
```

**Implementation:**
```cpp
bool CompareEntityMemoryPools(
    const EntityMemoryPool& actual,
    const EntityMemoryPool& expected,
    const TestContext& context) {
    
    // Create matcher with context
    EntityMemoryPoolEqualsMatcher matcher(expected, context);
    
    // Perform comparison (matcher collects ALL differences)
    bool matches = matcher.match(actual);
    
    // If failed, format and report
    if (!matches) {
        std::string formatted_error = matcher.describe();
        REQUIRE(matches); // This will fail and show formatted_error
    }
    
    return matches;
}
```

#### 2.2 Update Matchers to Collect All Differences

**Modify:** `tests/matchers/entity_memory_pool_matchers.h`

```cpp
class EntityMemoryPoolEqualsMatcher : public Catch::Matchers::MatcherBase<EntityMemoryPool> {
private:
    const EntityMemoryPool &m_expected;
    mutable std::vector<std::string> m_all_differences;  // ← NEW: Collect all differences
    std::optional<TestContext> m_context;
    
    template <typename TComponent>
    void CompareComponentVector(
        const std::vector<TComponent> &actual_vec,
        const std::vector<TComponent> &expected_vec,
        std::vector<std::string>& differences) const {  // ← NEW: accumulate parameter
        
        for (size_t i = 0; i < expected_vec.size(); ++i) {
            ComponentEqualsMatcher<TComponent> matcher(expected_vec[i]);
            if (!matcher.match(actual_vec[i])) {
                // Don't return - collect the difference and continue
                differences.push_back(
                    std::format("{} at index {}: {}", 
                        typeid(TComponent).name(), i, matcher.describe()));
            }
        }
        // Continue checking even if differences found
    }
    
public:
    bool match(const EntityMemoryPool &actual) const override {
        m_all_differences.clear();
        
        // Check pool sizes
        size_t actual_size = entity::memory::GetMemoryPoolSize(actual);
        size_t expected_size = entity::memory::GetMemoryPoolSize(m_expected);
        
        if (actual_size != expected_size) {
            m_all_differences.push_back(
                std::format("Pool sizes differ: actual={}, expected={}", 
                    actual_size, expected_size));
            // CONTINUE checking components even if sizes differ
        }
        
        // Compare ALL component vectors (don't stop on first failure)
        CompareAllComponentVectors(actual, m_expected, m_all_differences);
        
        // Return true only if no differences found
        return m_all_differences.empty();
    }
};
```

#### 2.3 Update Test Files

**Example Update in `tick_executor.cpp`:**

```cpp
// OLD:
REQUIRE_THAT(actual_pool, EqualsEntityMemoryPool(expected_pool, context));

// NEW:
CompareEntityMemoryPools(actual_pool, expected_pool, context);
```

**Example Update in `test_data_harness.cpp`:**

```cpp
// OLD:
REQUIRE_THAT(actual, matcher);

// NEW:
CompareEntityMemoryPools(actual, expected, context);
```

### Files to Modify

1. **New Files:**
   - `tests/matchers/comparison_wrappers.h`
   - `tests/matchers/comparison_wrappers.cpp`

2. **Modify Matchers:**
   - `tests/matchers/entity_memory_pool_matchers.h`
   - `tests/matchers/event_matchers.h`
   - `tests/matchers/component_matchers.h`
   - Component-specific matchers (cmeta, cuser_interface, etc.)

3. **Update Test Harness:**
   - `tests/harness/tick_executor.cpp` (~4 REQUIRE_THAT calls)
   - `tests/harness/test_data_harness.cpp` (~6 REQUIRE_THAT calls)

4. **Update Test Files:**
   - All files using REQUIRE_THAT/CHECK_THAT with matchers (~109 total)

---

## Stage 3: Comprehensive Data Structure Testing ⏳ **NEXT**

### Objective
Any data structure testing will be in a wrapper function to keep code clean. Data structures and all their parts will all be tested so we don't stop on failure.

### Implementation

#### 3.1 Component-Level Comprehensive Testing

**Update Component Matchers:**

```cpp
template <typename TComponent>
class ComponentEqualsMatcher : public Catch::Matchers::MatcherBase<TComponent> {
private:
    const TComponent &m_expected;
    mutable std::vector<std::string> m_field_differences;  // Collect all field mismatches
    
public:
    bool match(const TComponent &actual) const override {
        m_field_differences.clear();
        
        // Check base Component fields
        if (actual.m_active != m_expected.m_active) {
            m_field_differences.push_back(
                std::format("m_active: expected={}, actual={}", 
                    m_expected.m_active, actual.m_active));
            // Continue checking other fields instead of returning
        }
        
        // Check derived class fields
        // ... (add all field comparisons, never return early)
        
        return m_field_differences.empty();
    }
    
    std::string describe() const override {
        if (m_field_differences.empty()) {
            return "Components match";
        }
        
        std::ostringstream oss;
        oss << "Component differences:\n";
        for (const auto& diff : m_field_differences) {
            oss << "  • " << diff << "\n";
        }
        return oss.str();
    }
};
```

#### 3.2 Pool-Level Comprehensive Testing

```cpp
void CompareAllComponentVectors(
    const EntityMemoryPool &actual,
    const EntityMemoryPool &expected,
    std::vector<std::string>& differences) const {
    
    // Check CMeta components
    CompareComponentVector<CMeta>(
        std::get<CMeta>(actual),
        std::get<CMeta>(expected),
        differences);
    
    // Check CUserInterface components (continues even if CMeta had differences)
    CompareComponentVector<CUserInterface>(
        std::get<CUserInterface>(actual),
        std::get<CUserInterface>(expected),
        differences);
    
    // Check CGrimoireMachina components (continues even if previous had differences)
    CompareComponentVector<CGrimoireMachina>(
        std::get<CGrimoireMachina>(actual),
        std::get<CGrimoireMachina>(expected),
        differences);
    
    // ... Continue for all component types
    // Never stop early - collect ALL differences
}
```

#### 3.3 Example: Complete Failure Report

When multiple things are wrong, show everything:

```
EntityMemoryPool Comparison Failed
====================================
Pool Size Differences:
  • actual=10, expected=12

CMeta Differences:
  • Index 0:
      - m_active: expected=true, actual=false
  • Index 3:
      - m_active: expected=true, actual=false

CUserInterface Differences:
  • Index 1:
      - ui_name: expected="menu", actual="dialog"
      - start_visible: expected=true, actual=false
  • Index 2:
      - root_ui_element position.x: expected=100, actual=150
      
CGrimoireMachina Differences:
  • Index 0:
      - spell_count: expected=5, actual=3

Total: 7 differences found across 4 component types
```

**Key Point:** Every single difference is reported. No early returns.

---

## Stage 4: Success Messages ⏳ **PLANNED**

### Objective
We will add success for an overall passed data structure but not pass messages for each Component.

### Design Decision

**Approach:** Silent success (Catch2 default) with optional INFO messages

#### Option A: Single Success Message (Minimal)
```cpp
bool CompareEntityMemoryPools(...) {
    EntityMemoryPoolEqualsMatcher matcher(expected, context);
    bool matches = matcher.match(actual);
    
    if (matches) {
        INFO("✓ EntityMemoryPool comparison passed");
    } else {
        // Format and report failure
    }
    
    REQUIRE(matches);
    return matches;
}
```

**Output on Success:**
```
✓ EntityMemoryPool comparison passed
```

#### Option B: Silent Success (Recommended)
```cpp
bool CompareEntityMemoryPools(...) {
    EntityMemoryPoolEqualsMatcher matcher(expected, context);
    bool matches = matcher.match(actual);
    
    // No output on success - let Catch2 handle it
    if (!matches) {
        // Only output on failure
    }
    
    REQUIRE(matches);
    return matches;
}
```

**Output on Success:**
```
(silent - Catch2 shows test name passed at end)
```

**Recommendation:** Use Option B (silent success) because:
- Cleaner output
- Standard Catch2 behavior
- Focus on failures (the important information)
- Can still see which tests passed in final summary

#### What NOT to Do

**❌ Per-Component Success Messages:**
```
✓ CMeta at index 0 passed
✓ CMeta at index 1 passed
✓ CMeta at index 2 passed
✓ CUserInterface at index 0 passed
✓ CUserInterface at index 1 passed
...
```

This creates noise and clutters output. Only show overall success.

---

## Stage 5: Visual Output with conmat Package ⏳ **PLANNED**

### Objective
Add visually pleasing and structured fail messages using the conmat package.

### conmat Package Overview

The conmat library provides:
- Type-safe ANSI color codes (enums prevent injection)
- Text styling (Bold, Italic, Underline, etc.)
- Divider functions
- Format functions for combining colors and styles

**Already integrated:** Available at `build/Debug/_deps/conmat-src`

### Implementation

#### 5.1 Update TestContext Formatting

**File:** `tests/matchers/test_context.h`

```cpp
#include <conmat.h>

struct TestContext {
    std::string test_name;
    std::string description;
    std::optional<uint32_t> current_tick;
    std::optional<uint32_t> total_ticks;
    
    std::string FormatFailureMessage(
        const std::string &comparison_type,
        const std::string &differences) const {
        
        using namespace conmat;
        std::ostringstream oss;
        
        // Header with bold red [FAILED] indicator
        oss << "\n" << Format("[FAILED] ", FormatOptions(Color::BrightRed, Style::Bold));
        oss << Format(comparison_type + " Comparison", FormatOptions(Color::White, Style::Bold));
        
        // Top divider in yellow
        oss << "\n" << Divider("=", 80, FormatOptions(Color::Yellow));
        
        // Context section in cyan
        if (HasContent()) {
            oss << Format("\n  Context:", FormatOptions(Color::Cyan, Style::Bold));
            
            if (!test_name.empty()) {
                oss << "\n    " << Colorize("Test: ", Color::BrightBlack);
                oss << Colorize(test_name, Color::White);
            }
            
            if (current_tick.has_value()) {
                oss << "\n    " << Colorize("Tick: ", Color::BrightBlack);
                oss << Colorize("[" + std::to_string(current_tick.value()), Color::BrightBlue);
                if (total_ticks.has_value()) {
                    oss << " of " << std::to_string(total_ticks.value());
                }
                oss << "]";
            }
            
            if (!description.empty()) {
                oss << "\n    " << Colorize("Description: ", Color::BrightBlack);
                oss << Colorize(description, Color::White);
            }
        }
        
        // Middle divider
        oss << "\n" << Divider("-", 80, FormatOptions(Color::BrightBlack));
        
        // Differences section in red
        if (!differences.empty()) {
            oss << Format("\n  Differences:", FormatOptions(Color::BrightRed, Style::Bold));
            oss << "\n" << FormatDifferencesList(differences);
        }
        
        // Bottom divider
        oss << "\n" << Divider("=", 80, FormatOptions(Color::Yellow));
        
        return oss.str();
    }
    
private:
    std::string FormatDifferencesList(const std::string& differences) const {
        using namespace conmat;
        
        // Split differences by newline and format each
        std::istringstream iss(differences);
        std::ostringstream oss;
        std::string line;
        
        while (std::getline(iss, line)) {
            oss << "    " << Colorize("• ", Color::Red);
            
            // Highlight field names in yellow
            // Highlight "expected" values in green
            // Highlight "actual" values in red
            // (Can use regex or simple string replacement)
            
            oss << FormatDifferenceLine(line) << "\n";
        }
        
        return oss.str();
    }
    
    std::string FormatDifferenceLine(const std::string& line) const {
        using namespace conmat;
        
        // Simple formatting - can be enhanced
        // Look for patterns like "field: expected=X, actual=Y"
        // and colorize accordingly
        
        std::string result = line;
        
        // Highlight "expected" in green
        size_t pos = result.find("expected=");
        if (pos != std::string::npos) {
            // Color the value after "expected="
            // (Simplified - full implementation would use regex)
        }
        
        // Highlight "actual" in bright red
        pos = result.find("actual=");
        if (pos != std::string::npos) {
            // Color the value after "actual="
        }
        
        return result;
    }
};
```

#### 5.2 Update Matcher describe() Methods

```cpp
std::string EntityMemoryPoolEqualsMatcher::describe() const override {
    if (m_context.has_value()) {
        // Use conmat-enhanced formatting from TestContext
        std::string formatted_differences = FormatAllDifferences();
        return m_context.value().FormatFailureMessage("EntityMemoryPool", formatted_differences);
    }
    
    // Legacy support (no conmat)
    return FormatPlainText();
}

std::string FormatAllDifferences() const {
    using namespace conmat;
    std::ostringstream oss;
    
    for (size_t i = 0; i < m_all_differences.size(); ++i) {
        const auto& diff = m_all_differences[i];
        
        // Indent based on difference type
        if (diff.starts_with("Pool size")) {
            oss << Colorize(diff, Color::BrightRed) << "\n";
        } else if (diff.find(" at index ") != std::string::npos) {
            // Component-level difference
            oss << "  " << Colorize(diff, Color::Red) << "\n";
        } else {
            // Field-level difference - more indentation
            oss << "    ◦ " << diff << "\n";
        }
    }
    
    return oss.str();
}
```

#### 5.3 Color Scheme

| Element | Color | Style | Purpose |
|---------|-------|-------|---------|
| [FAILED] | Bright Red | Bold | Failure indicator |
| Comparison Type | White | Bold | Section header |
| Dividers | Yellow | - | Visual separation |
| Context Labels | Cyan | Bold | Context section |
| Context Values | White | - | Test name, etc. |
| Tick Numbers | Bright Blue | - | Temporal context |
| "Differences" Label | Bright Red | Bold | Error section |
| Bullet Points | Red | - | List markers |
| Field Names | Yellow | - | Property names |
| Expected Values | Green | - | Correct values |
| Actual Values | Bright Red | - | Incorrect values |
| Metadata | Bright Black | - | Secondary info |

#### 5.4 Example Output

**Scenario:** Multiple failures in EntityMemoryPool comparison

```
[FAILED] EntityMemoryPool Comparison
================================================================================
  Context:
    Test: sample_test_1  
    Tick: [5 of 10]
    Description: Testing UI state after user interaction
────────────────────────────────────────────────────────────────────────────────
  Differences:
    • Pool sizes differ: actual=10, expected=12
    • CMeta at index 0:
        ◦ m_active: expected=true, actual=false
    • CMeta at index 3:
        ◦ m_active: expected=true, actual=false
    • CUserInterface at index 1:
        ◦ ui_name: expected="main_menu", actual="settings_dialog"
        ◦ start_visible: expected=true, actual=false
        ◦ root_ui_element.position.x: expected=100, actual=150
        ◦ root_ui_element.position.y: expected=200, actual=200
    • CUserInterface at index 2:
        ◦ ui_name: expected="overlay", actual="overlay"
        ◦ start_visible: expected=false, actual=true
    • CGrimoireMachina at index 0:
        ◦ spell_list size: expected=5, actual=3
        ◦ active_spell_index: expected=2, actual=0

  Summary: 11 differences found across 3 component types
================================================================================
```

**Color Rendering** (actual terminal output):
- `[FAILED]` appears in **bright red bold**
- `EntityMemoryPool Comparison` in **white bold**
- Dividers (`===`, `---`) in **yellow**
- `Context:`, `Differences:` labels in **cyan bold**
- `Test: sample_test_1` with label in gray, name in white
- `[5 of 10]` in **bright blue**
- Bullet points (`•`, `◦`) in **red**
- Field names like `m_active`, `ui_name` in **yellow**
- `expected=true` values in **green**
- `actual=false` values in **bright red**

### Benefits
- Immediate visual recognition of failures vs context
- Easy to scan for specific types of errors
- Professional, modern appearance
- Color-blind friendly (uses both color AND structure)
- Terminal-safe (automatically detects color support)

---

## Stage 6: Demonstration Script ⏳ **PLANNED**

### Objective
Add .sh script for easy demonstration of the visual output, prevents building each time.

### Script Implementation

**File:** `demonstrate_test_output.sh`

```bash
#!/bin/bash
#
# SteamRot Test Output Demonstration Script
# 
# This script demonstrates the improved test output formatting
# without requiring a full rebuild of the project.
#

set -e  # Exit on error

# Color codes for the script itself (not the tests)
readonly SCRIPT_RED='\033[0;31m'
readonly SCRIPT_GREEN='\033[0;32m'
readonly SCRIPT_BLUE='\033[0;34m'
readonly SCRIPT_YELLOW='\033[1;33m'
readonly SCRIPT_CYAN='\033[0;36m'
readonly SCRIPT_NC='\033[0m' # No Color

# Configuration
readonly BUILD_DIR="./build/Debug"
readonly TEST_BINARY_DIR="${BUILD_DIR}/tests"

#########################################
# Print colored section header
#########################################
print_section() {
    local title="$1"
    echo ""
    echo -e "${SCRIPT_CYAN}========================================"
    echo -e "${title}"
    echo -e "========================================${SCRIPT_NC}"
    echo ""
}

#########################################
# Print test scenario description
#########################################
print_scenario() {
    local scenario="$1"
    local expected="$2"
    
    echo -e "${SCRIPT_BLUE}Scenario:${SCRIPT_NC} ${scenario}"
    echo -e "${SCRIPT_YELLOW}Expected Result:${SCRIPT_NC} ${expected}"
    echo "----------------------------------------"
    echo ""
}

#########################################
# Run a test and capture output
#########################################
run_test() {
    local test_binary="$1"
    local test_name="$2"
    local filter="${3:-}"
    
    if [ -n "${filter}" ]; then
        "${TEST_BINARY_DIR}/${test_binary}" "${test_name}" -c "${filter}" 2>&1 || true
    else
        "${TEST_BINARY_DIR}/${test_binary}" "${test_name}" 2>&1 || true
    fi
    
    echo ""
}

#########################################
# Main demonstration sequence
#########################################
main() {
    # Ensure we're in the project root
    cd "$(dirname "$0")"
    
    # Verify build directory exists
    if [ ! -d "${BUILD_DIR}" ]; then
        echo -e "${SCRIPT_RED}Error: Build directory not found: ${BUILD_DIR}${SCRIPT_NC}"
        echo "Please build the project first:"
        echo "  cmake --preset Debug"
        echo "  cmake --build --preset Debug"
        exit 1
    fi
    
    print_section "SteamRot Test Output Demonstration"
    
    echo "This script demonstrates the improved test output with:"
    echo "  • Comprehensive error collection (all differences shown)"
    echo "  • Hierarchical structure (components > fields)"
    echo "  • Color-coded output via conmat package"
    echo "  • Context information (test name, tick number)"
    echo ""
    echo -e "${SCRIPT_YELLOW}Note: Tests are run from pre-built binaries.${SCRIPT_NC}"
    echo "      No rebuild required to see output changes."
    echo ""
    
    # Scenario 1: Passing Test (Silent Success)
    print_section "Scenario 1: Passing Test"
    print_scenario \
        "EntityMemoryPool comparison where actual matches expected" \
        "PASS (minimal/no output - Catch2 default)"
    
    run_test \
        "matchers/test_entity_memory_pool_matchers" \
        "EntityMemoryPool matcher works correctly"
    
    # Scenario 2: Pool Size Mismatch
    print_section "Scenario 2: Pool Size Mismatch"
    print_scenario \
        "EntityMemoryPool with different sizes" \
        "FAIL with clear size difference message"
    
    # Note: Would need a test that intentionally creates size mismatch
    # For now, show example from test data
    echo -e "${SCRIPT_YELLOW}[Demo would show: Pool size mismatch with formatted output]${SCRIPT_NC}"
    echo ""
    
    # Scenario 3: Component Field Differences
    print_section "Scenario 3: Component Field Differences"
    print_scenario \
        "Multiple component fields differ from expected" \
        "FAIL with all differences listed hierarchically"
    
    echo -e "${SCRIPT_YELLOW}[Demo would show: Multiple field mismatches with:"
    echo "  - Component names in context"
    echo "  - Field-by-field differences"
    echo "  - Expected values in green, actual in red]${SCRIPT_NC}"
    echo ""
    
    # Scenario 4: Comprehensive Multi-Component Failure
    print_section "Scenario 4: Multi-Component Failure"
    print_scenario \
        "Differences across multiple component types" \
        "FAIL with complete breakdown by component type"
    
    run_test \
        "harness/test_harness_tests" \
        "run_entity_memory_pool_comparison_test with metadata"
    
    # Scenario 5: Context Information Display
    print_section "Scenario 5: Context Information"
    print_scenario \
        "Failure with rich context (test name, tick, description)" \
        "FAIL with formatted context section"
    
    echo -e "${SCRIPT_YELLOW}[Demo would show:"
    echo "  - Test name prominently displayed"
    echo "  - Current tick and total ticks"
    echo "  - Test description if provided"
    echo "  - All in structured, color-coded format]${SCRIPT_NC}"
    echo ""
    
    # Summary
    print_section "Demonstration Complete"
    
    echo "Key Features Demonstrated:"
    echo ""
    echo "  ${SCRIPT_GREEN}✓${SCRIPT_NC} Passing tests are silent (Catch2 default)"
    echo "  ${SCRIPT_GREEN}✓${SCRIPT_NC} Comprehensive failure reporting (all differences)"
    echo "  ${SCRIPT_GREEN}✓${SCRIPT_NC} Hierarchical error structure (components → fields)"
    echo "  ${SCRIPT_GREEN}✓${SCRIPT_NC} Color-coded output with conmat"
    echo "  ${SCRIPT_GREEN}✓${SCRIPT_NC} Rich context information (test name, tick)"
    echo "  ${SCRIPT_GREEN}✓${SCRIPT_NC} Professional, scannable output"
    echo ""
    echo "Compare this to previous output:"
    echo "  ${SCRIPT_RED}✗${SCRIPT_NC} Console spam with printf debugging"
    echo "  ${SCRIPT_RED}✗${SCRIPT_NC} Only first failure shown"
    echo "  ${SCRIPT_RED}✗${SCRIPT_NC} Unclear context and structure"
    echo "  ${SCRIPT_RED}✗${SCRIPT_NC} Difficult to scan and understand"
    echo ""
    
    print_section "Additional Options"
    
    echo "Run specific test categories:"
    echo "  ${SCRIPT_CYAN}./build/Debug/tests/matchers/test_entity_memory_pool_matchers${SCRIPT_NC}"
    echo "  ${SCRIPT_CYAN}./build/Debug/tests/harness/test_harness_tests${SCRIPT_NC}"
    echo ""
    echo "Run with Catch2 options:"
    echo "  ${SCRIPT_CYAN}ctest --preset Debug --output-on-failure${SCRIPT_NC}"
    echo "  ${SCRIPT_CYAN}ctest --preset Debug -R \"EntityMemoryPool\"${SCRIPT_NC}"
    echo ""
    echo "Generate XML reports:"
    echo "  ${SCRIPT_CYAN}ctest --preset Debug -T test --output-junit results.xml${SCRIPT_NC}"
    echo ""
}

# Run the demonstration
main "$@"
```

### Usage

```bash
# Make executable
chmod +x demonstrate_test_output.sh

# Run demonstration
./demonstrate_test_output.sh

# Output will show:
# - Multiple test scenarios
# - Before/after comparison
# - Key features highlighted
# - No rebuild required
```

### Benefits

1. **No Rebuild Required:**
   - Uses pre-built test binaries
   - Fast execution
   - Easy to demo to stakeholders

2. **Comprehensive Demonstration:**
   - Shows multiple scenarios
   - Compares before/after
   - Highlights key improvements

3. **Easy to Update:**
   - Add new scenarios easily
   - Modify test selection
   - Customize for different audiences

4. **Self-Documenting:**
   - Clear scenario descriptions
   - Expected results shown
   - Feature list included

---

## Stage 7: XML Test Reports ⏳ **PLANNED**

### Objective
Potentially use Catch2 to output optional XML test reports as well as terminal output.

### Why XML Reports?

**Use Cases:**
- CI/CD integration (GitHub Actions, Jenkins, etc.)
- Test result history tracking
- Dashboard visualization
- Automated reporting
- Integration with test management tools

**Formats Supported by Catch2:**
- XML (Catch2's native format)
- JUnit XML (widely compatible)
- Both simultaneously

### Implementation

#### 7.1 CMake Configuration

**Add to `tests/CMakeLists.txt`:**

```cmake
#
# Optional XML Test Report Generation
#

option(STEAMROT_TEST_XML_OUTPUT "Generate XML test reports" OFF)
option(STEAMROT_TEST_JUNIT_OUTPUT "Generate JUnit XML test reports" OFF)

if(STEAMROT_TEST_XML_OUTPUT OR STEAMROT_TEST_JUNIT_OUTPUT)
    set(TEST_REPORT_DIR "${CMAKE_BINARY_DIR}/test_reports" 
        CACHE PATH "Test report output directory")
    
    file(MAKE_DIRECTORY ${TEST_REPORT_DIR})
    
    message(STATUS "Test reports will be generated in: ${TEST_REPORT_DIR}")
endif()

# Function to configure a test target for XML output
function(configure_test_for_xml_output test_target)
    if(STEAMROT_TEST_XML_OUTPUT)
        add_test(
            NAME ${test_target}_xml
            COMMAND ${test_target} 
                -r xml 
                -o ${TEST_REPORT_DIR}/${test_target}.xml
        )
        set_tests_properties(${test_target}_xml PROPERTIES
            LABELS "xml"
        )
    endif()
    
    if(STEAMROT_TEST_JUNIT_OUTPUT)
        add_test(
            NAME ${test_target}_junit
            COMMAND ${test_target} 
                -r junit 
                -o ${TEST_REPORT_DIR}/${test_target}_junit.xml
        )
        set_tests_properties(${test_target}_junit PROPERTIES
            LABELS "junit"
        )
    endif()
endfunction()

# Apply to all test targets
configure_test_for_xml_output(test_components)
configure_test_for_xml_output(test_entity)
configure_test_for_xml_output(test_events)
# ... apply to all test targets
```

#### 7.2 Command-Line Usage

**Manual XML Generation:**

```bash
# Generate Catch2 XML report
./build/Debug/tests/matchers/test_entity_memory_pool_matchers \
    -r xml -o results.xml

# Generate JUnit XML (CI-friendly)
./build/Debug/tests/matchers/test_entity_memory_pool_matchers \
    -r junit -o junit_results.xml

# Generate both console and XML simultaneously
./build/Debug/tests/matchers/test_entity_memory_pool_matchers \
    -r console -r xml::out=results.xml

# Detailed output with success/failure details
./build/Debug/tests/matchers/test_entity_memory_pool_matchers \
    -r xml -o results.xml -s
```

**CMake Presets:**

```bash
# Configure with XML output enabled
cmake --preset Debug -DSTEAMROT_TEST_XML_OUTPUT=ON

# Build
cmake --build --preset Debug

# Run tests - XML automatically generated
ctest --preset Debug

# Reports available in:
ls build/Debug/test_reports/
```

#### 7.3 CTest Integration

**Using CTest's Native XML Output:**

```bash
# Generate CTest XML (different format than Catch2)
ctest --preset Debug -T Test

# Output will be in:
# build/Debug/Testing/[timestamp]/Test.xml

# Generate JUnit XML directly from CTest
ctest --preset Debug -T test --output-junit results.xml
```

#### 7.4 CI/CD Integration Examples

**GitHub Actions:**

```yaml
name: Tests with XML Reports

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    
    steps:
      - uses: actions/checkout@v3
      
      - name: Configure
        run: cmake --preset Debug -DSTEAMROT_TEST_JUNIT_OUTPUT=ON
      
      - name: Build
        run: cmake --build --preset Debug
      
      - name: Test
        run: ctest --preset Debug --output-on-failure
      
      - name: Upload Test Results
        if: always()
        uses: actions/upload-artifact@v3
        with:
          name: test-results
          path: build/Debug/test_reports/*.xml
      
      - name: Publish Test Report
        uses: EnricoMi/publish-unit-test-result-action@v2
        if: always()
        with:
          files: build/Debug/test_reports/*_junit.xml
```

**Jenkins:**

```groovy
pipeline {
    agent any
    
    stages {
        stage('Configure') {
            steps {
                sh 'cmake --preset Debug -DSTEAMROT_TEST_JUNIT_OUTPUT=ON'
            }
        }
        
        stage('Build') {
            steps {
                sh 'cmake --build --preset Debug'
            }
        }
        
        stage('Test') {
            steps {
                sh 'ctest --preset Debug --output-on-failure'
            }
        }
    }
    
    post {
        always {
            junit 'build/Debug/test_reports/*_junit.xml'
        }
    }
}
```

#### 7.5 XML Report Contents

**Catch2 XML Format:**
```xml
<?xml version="1.0" encoding="UTF-8"?>
<Catch name="test_entity_memory_pool_matchers">
  <Group name="test_entity_memory_pool_matchers">
    <TestCase name="EntityMemoryPool matcher works correctly" 
              tags="[unit][matchers]" 
              filename="entity_memory_pool_matchers.test.cpp" 
              line="15">
      <OverallResult success="true" durationInSeconds="0.001234"/>
    </TestCase>
    <TestCase name="EntityMemoryPool matcher provides detailed feedback" 
              tags="[unit][matchers]">
      <Expression success="false" type="REQUIRE_THAT" 
                  filename="entity_memory_pool_matchers.test.cpp" line="45">
        <Original>
          actual, EqualsEntityMemoryPool(expected, context)
        </Original>
        <Expanded>
          [FAILED] EntityMemoryPool Comparison
          ========================================
          Context:
            Test: sample_test
          ----------------------------------------
          Differences:
            • Pool sizes differ: actual=10, expected=12
        </Expanded>
      </Expression>
      <OverallResult success="false" durationInSeconds="0.002456"/>
    </TestCase>
  </Group>
  <OverallResults successes="1" failures="1" expectedFailures="0"/>
</Catch>
```

**JUnit XML Format:**
```xml
<?xml version="1.0" encoding="UTF-8"?>
<testsuites tests="2" failures="1" disabled="0" errors="0" 
            timestamp="2025-01-01T12:00:00Z" time="0.003690">
  <testsuite name="test_entity_memory_pool_matchers" tests="2" 
             failures="1" disabled="0" errors="0" time="0.003690">
    <testcase name="EntityMemoryPool matcher works correctly" 
              status="run" time="0.001234" 
              classname="test_entity_memory_pool_matchers" />
    <testcase name="EntityMemoryPool matcher provides detailed feedback" 
              status="run" time="0.002456" 
              classname="test_entity_memory_pool_matchers">
      <failure message="Comparison failed" 
               type="REQUIRE_THAT">
[FAILED] EntityMemoryPool Comparison
========================================
Context:
  Test: sample_test
----------------------------------------
Differences:
  • Pool sizes differ: actual=10, expected=12
      </failure>
    </testcase>
  </testsuite>
</testsuites>
```

#### 7.6 Documentation

**Add to `README.md`:**

```markdown
### Generating Test Reports

#### Terminal Output (Default)
```bash
ctest --preset Debug --output-on-failure
```

#### XML Reports
```bash
# Configure with XML output
cmake --preset Debug -DSTEAMROT_TEST_XML_OUTPUT=ON

# Build and test
cmake --build --preset Debug
ctest --preset Debug

# Reports in: build/Debug/test_reports/
```

#### JUnit XML (CI Integration)
```bash
# Configure with JUnit output
cmake --preset Debug -DSTEAMROT_TEST_JUNIT_OUTPUT=ON

# Build and test
cmake --build --preset Debug
ctest --preset Debug

# Reports compatible with CI systems
```

#### Manual XML Generation
```bash
# Any test binary can generate XML directly
./build/Debug/tests/matchers/test_entity_memory_pool_matchers \
    -r xml -o my_results.xml
```

**Benefits:**
- ✅ CI/CD integration
- ✅ Historical tracking
- ✅ Dashboard visualization
- ✅ Both terminal and XML output
- ✅ No code changes required
```

---

## Current Status

### ✅ **Stage 1: Complete**
- All console output removed from source code (25+ instances)
- All console output removed from test harness (15 instances)
- Error handling improved throughout
- All tests passing

### 🚧 **Stage 2-3: Design Ready, Implementation Pending**
- Wrapper function architecture designed
- Comprehensive comparison approach defined
- Ready for implementation

### ⏳ **Stage 4-7: Planned**
- Success message strategy defined
- conmat integration plan complete
- Demonstration script designed
- XML report configuration ready

---

## Implementation Order

**Recommended Implementation Sequence:**

1. **Stage 2 & 3 Together** (Foundation):
   - Create wrapper functions
   - Update matchers for comprehensive collection
   - These are interdependent and should be done together
   - **Estimated:** 2-4 hours

2. **Stage 5** (Visual Enhancement):
   - Integrate conmat formatting
   - Update TestContext
   - Format matcher output
   - **Estimated:** 2-3 hours

3. **Stage 4** (Polish):
   - Finalize success message behavior
   - Quick decision, minimal code
   - **Estimated:** 30 minutes

4. **Stage 6** (Demo):
   - Create demonstration script
   - Test various scenarios
   - **Estimated:** 1 hour

5. **Stage 7** (Optional):
   - Add XML report configuration
   - Test CI integration
   - **Estimated:** 1-2 hours

**Total Estimated Time:** 8-12 hours for complete implementation

---

## Testing Strategy

### After Each Stage

1. **Verify Compilation:**
   ```bash
   cmake --build --preset Debug
   ```

2. **Run Unit Tests:**
   ```bash
   ctest --preset Debug --output-on-failure
   ```

3. **Run Specific Test Suite:**
   ```bash
   ./build/Debug/tests/matchers/test_entity_memory_pool_matchers
   ```

4. **Verify No Regressions:**
   ```bash
   ctest --preset Debug -R "matcher"
   ```

### Final Validation

1. **All Tests Pass:**
   ```bash
   ctest --preset Debug
   ```

2. **Visual Output Check:**
   ```bash
   ./demonstrate_test_output.sh
   ```

3. **XML Generation:**
   ```bash
   # Configure and test
   cmake --preset Debug -DSTEAMROT_TEST_XML_OUTPUT=ON
   cmake --build --preset Debug
   ctest --preset Debug
   
   # Verify reports generated
   ls build/Debug/test_reports/
   ```

---

## Success Criteria

### Functional Requirements
- [ ] No console output in source code (except main.cpp)
- [ ] No console output in test harness
- [ ] Wrapper functions between tests and matchers
- [ ] Matchers collect ALL differences (no early returns)
- [ ] Comprehensive error reporting
- [ ] Visual output with conmat
- [ ] Demonstration script works
- [ ] XML reports optional and functional

### Quality Requirements
- [ ] All existing tests still pass
- [ ] No test regressions
- [ ] Code is maintainable
- [ ] Output is readable and scannable
- [ ] Documentation is updated

### Non-Functional Requirements
- [ ] Test execution time not significantly increased
- [ ] Memory usage remains reasonable
- [ ] Build time not significantly increased
- [ ] Easy to add new matchers

---

## Conclusion

This multistage plan provides a comprehensive roadmap for improving the SteamRot test infrastructure. Stage 1 (console output removal) is complete, providing a clean foundation for the remaining enhancements.

The plan emphasizes:
- **Incremental progress** - each stage builds on the previous
- **Clear objectives** - each stage has specific, measurable goals
- **Practical implementation** - detailed code examples and patterns
- **Maintainability** - clean architecture with separation of concerns
- **Visual clarity** - professional, scannable test output

**Next Step:** Proceed with Stage 2 (wrapper functions) and Stage 3 (comprehensive comparison) implementation.
