# Test Harness Improvements - Executive Summary

## Quick Overview

This document provides a concise summary of the planned improvements to the SteamRot test harness infrastructure. For the complete detailed plan, see `TEST_HARNESS_IMPROVEMENTS_PLAN.md`.

## Four Main Improvement Areas

### 1. Better Indentation with `conmat::Indent` 🎯

**Current Problem**: Matchers use tabs (`\t`) which are inconsistent and take too much space.

**Solution**: Use `conmat::Indent(level, spaces_per_level)` function.

**Example**:
```cpp
// Old way
oss << "\t" << "CUserInterface:" << "\n";
oss << "\t\t" << "ui_name: " << value << "\n";

// New way
oss << conmat::Indent(1, 2) << "CUserInterface:" << "\n";
oss << conmat::Indent(2, 2) << "ui_name: " << value << "\n";
```

**Visual Comparison**:
```
Before (tabs):                  After (conmat::Indent):
Component                       Component
	m_active: true                m_active: true
		nested_field                nested_field
			deep_value                  deep_value
```

**Required Steps**:
1. Add `conmat::Indent` function to conmat library (external repo)
2. Update SteamRot to use new conmat version
3. Replace all `\t` with `conmat::Indent(level, 2)` in matchers

**Files to Change**:
- All matcher files in `tests/matchers/`
- `tests/harness/test_data_harness.cpp`

---

### 2. Fix GENERATE Output Verbosity 🔇

**Current Problem**: When using `GENERATE_COPY(from_range(configs))`, ALL test configs print their INFO messages even when only ONE fails. This creates excessive output.

**Example of Current Issue**:
```
Test: test_001.json - INFO: Running test_001
Test: test_002.json - INFO: Running test_002  
Test: test_003.json - INFO: Running test_003 ← FAILED
Test: test_004.json - INFO: Running test_004
Test: test_005.json - INFO: Running test_005

// All 5 configs printed info, but only #3 failed!
```

**Solution**: Enhance `HarnessReporter` to track per-instance state and only output INFO for failed instances.

**Implementation Approach**:
```cpp
class HarnessReporter {
  // Track each test instance
  std::map<std::string, InstanceInfo> m_instance_data;
  
  void assertionEnded(AssertionStats const &stats) {
    if (!stats.assertionResult.isOk()) {
      m_instance_data[current_id].has_failure = true;
    }
  }
  
  void testCasePartialEnded(...) {
    // Only output INFO if this instance failed
    if (instance.has_failure) {
      for (const auto &msg : instance.info_messages) {
        std::cout << msg << "\n";
      }
    }
  }
};
```

**Expected Result**:
```
Test: test_003.json - INFO: Running test_003 ← FAILED

// Only the failed test's info is shown!
```

---

### 3. Visual Output Improvements 🎨

**A. Structured Headers**

Replace simple dividers with boxed, hierarchical headers:

```
Current:
============================================================
Data Structure Comparison Tests
	test: my_test
============================================================

Improved:
╔════════════════════════════════════════════════════════════╗
║ Data Structure Comparison Tests                           ║
╟────────────────────────────────────────────────────────────╢
║ Test: my_test                                              ║
║ Tick: [1 of 5]                                             ║
╚════════════════════════════════════════════════════════════╝
```

**B. Color-Coded Status**

```cpp
✓ PASSED - simple_test_1
✗ FAILED - complex_test_2
```

**C. Summary Statistics**

```
============================================================
Test Run Summary
============================================================
Total Tests: 25
  Passed: 23
  Failed: 2
Pass Rate: 92.0%
============================================================
```

**D. Diff-Style Output**

```
Differences in CUserInterface:
  m_active:
    - actual:   true
    + expected: false
  ui_name:
    - actual:   "menu"
    + expected: "title"
```

---

### 4. XML Report Generation 📊

**Goal**: Support CI/CD integration while keeping beautiful console output.

**Approach**: Use Catch2's built-in XML reporter (don't create custom).

**Usage**:
```bash
# Console only
./test_harness -r harness

# XML only  
./test_harness -r xml -o test_results.xml

# Both simultaneously
./test_harness -r harness -r xml::out=test_results.xml
```

**CMake Integration**:
```cmake
option(STEAMROT_TEST_XML_OUTPUT "Generate XML test reports" OFF)

if(STEAMROT_TEST_XML_OUTPUT)
  set(CATCH_REPORTER_ARGS "-r" "xml::out=test_results.xml")
else()
  set(CATCH_REPORTER_ARGS "-r" "harness")
endif()
```

**CI/CD Example** (GitHub Actions):
```yaml
- name: Run Tests with XML
  run: ctest --preset Debug --output-junit test_results.xml
  
- name: Publish Results  
  uses: EnricoMi/publish-unit-test-result-action@v2
  with:
    files: test_results.xml
```

---

## Implementation Phases

### Phase 1: conmat::Indent (External Dependency)
- Add function to conmat library
- Update SteamRot dependency
- Test integration

### Phase 2: Replace Tabs
- Update all matcher files
- Add level tracking
- Update test_data_harness

### Phase 3: Fix GENERATE
- Enhance HarnessReporter
- Test with multiple configs
- Verify selective output

### Phase 4: Visual Improvements  
- Add boxed headers
- Color-coded status
- Summary statistics
- Diff-style output

### Phase 5: XML Reporting
- Document usage
- CMake integration
- CI/CD examples

### Phase 6: Testing & Docs
- Write tests
- Update README
- Create examples

---

## Key Benefits

✅ **Consistent Indentation** - 2 spaces per level, not tabs  
✅ **Less Noise** - Only failed tests show verbose output  
✅ **Better Structure** - Visual hierarchy makes scanning easier  
✅ **Professional Look** - Colors, boxes, clear formatting  
✅ **CI/CD Ready** - XML reports for automation  

---

## Files Requiring Changes

### Matchers (Replace tabs with Indent)
- `tests/matchers/ui_element_matchers.h`
- `tests/matchers/event_matchers.h`
- `tests/matchers/entity_memory_pool_matchers.h`
- `tests/matchers/cmeta_matchers.h`
- `tests/matchers/cuser_interface_matchers.h`
- `tests/matchers/cmachina_form_matchers.h`

### Reporter (Fix GENERATE verbosity)
- `tests/reporters/HarnessReporter.h`
- `tests/reporters/HarnessReporter.cpp`

### Harness (Visual improvements)
- `tests/harness/test_data_harness.cpp`
- `tests/harness/console_output.h`

### CMake (XML support)
- `tests/CMakeLists.txt`

---

## Important Notes

⚠️ **This is a PLANNING DOCUMENT** - No code has been implemented yet  
⚠️ **External Dependency** - Requires adding `conmat::Indent` to external repo first  
⚠️ **No Breaking Changes** - All improvements maintain backward compatibility  
⚠️ **Thorough Testing Required** - Each phase needs comprehensive tests  

---

## Questions to Consider

1. **conmat::Indent Ownership**: Should this be added to conmat library or implemented locally in SteamRot?
   - **Recommendation**: Add to conmat for reusability across projects

2. **Default Indentation**: 2 spaces or 4 spaces per level?
   - **Recommendation**: 2 spaces (consistent with existing code style)

3. **GENERATE Alternative**: Use SECTION approach instead of enhanced reporter?
   - **Recommendation**: Enhanced reporter (cleaner, more flexible)

4. **Visual Symbols**: Unicode box characters or ASCII-only?
   - **Recommendation**: Unicode with ASCII fallback for compatibility

5. **XML Format**: JUnit, custom, or both?
   - **Recommendation**: Use Catch2's built-in (JUnit-compatible)

---

## Next Steps

1. **Review Plan** - Get feedback on approach and priorities
2. **Approve Changes** - Confirm which improvements to implement
3. **Phase 1 First** - Start with conmat::Indent (external dependency)
4. **Iterative Implementation** - Complete one phase before moving to next
5. **Test Thoroughly** - Verify each change doesn't break existing tests

---

## Related Documentation

- **Full Plan**: `TEST_HARNESS_IMPROVEMENTS_PLAN.md` (detailed implementation guide)
- **Current Code**: 
  - `tests/reporters/HarnessReporter.h/cpp`
  - `tests/harness/test_data_harness.h/cpp`
  - `tests/matchers/*.h`
- **External**: [conmat library](https://github.com/walliscode/conmat)
