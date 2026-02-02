# Test Harness Architecture Analysis

## Executive Summary

This document provides an architectural analysis of the SteamRot test harness infrastructure, evaluating its design, implementation, and appropriate usage patterns.

## Overview

The test harness provides a comprehensive, data-driven testing framework for the SteamRot game engine. It enables:

- Loading test configurations from JSON files (compiled to FlatBuffers binary)
- Creating test environments with controlled state
- Simulating game ticks with coordinated input, events, and logic execution
- Capturing and comparing state at each tick
- Data-driven testing using Catch2 generators

## Architecture Components

### 1. Data Loading Layer

**Files:**
- `FlatbuffersTestDataLoader.h/cpp` - File discovery and binary loading
- `ITestDataProvider.h` - Abstract interface for test data provision
- `FlatbuffersTestDataProvider.h/cpp` - Converts FlatBuffers to C++ structs

**Responsibilities:**
- Discover `.test_data.bin` files in adjacent `data/` directories
- Load binary FlatBuffers data from files
- Convert FlatBuffers schemas to C++ runtime structs (`TestData`, `TestMetaData`, etc.)
- Handle error cases (missing directories, invalid data)

**Test Coverage:** ✅ **EXCELLENT**
- 64+ test cases across harness unit tests
- Error handling thoroughly tested (null inputs, missing directories, empty data)
- Success paths validated with real test data
- Edge cases covered (empty vectors, missing optional fields)

### 2. Test Execution Layer

**Files:**
- `TestEngine.h/cpp` - Game engine subclass for testing
- `harness_runner.h/cpp` - High-level test orchestration functions
- `SimulationRunner.h/cpp` - Executes simulation steps

**Responsibilities:**
- Create controlled test environments (TestEngine extends Engine)
- Execute game ticks with TestData configuration
- Run simulation steps (Logic class execution)
- Capture engine snapshots at each tick (data bank)
- Coordinate input/event/simulation execution per tick

**Test Coverage:** ✅ **GOOD**
- TestEngine construction, startup, and execution tested
- Data bank capture verified
- Tick counting validated
- Simulation execution tested (empty, single, multiple steps)
- Error handling for invalid simulation steps

### 3. Comparison Layer

**Files:**
- Entity pool matchers (in `tests/matchers/`)
- Event bus matchers (in `tests/matchers/`)
- `harness_runner.cpp` - Snapshot comparison functions

**Responsibilities:**
- Compare captured state (data bank) with expected state (tick snapshots)
- Use Catch2 matchers for formatted output
- Provide detailed mismatch information
- Support test context (test name, description, tick number)

**Test Coverage:** ✅ **GOOD**
- Matchers tested independently in `tests/unit/matchers/`
- Snapshot comparison functions tested
- Success and failure paths validated
- Test context enrichment verified

## Data Flow

```
JSON Test Data (.test_data.json)
    ↓ (CMake compile time)
Binary FlatBuffers (.test_data.bin)
    ↓ (FlatbuffersTestDataLoader)
FlatBuffers Pointers (TestDataFbs*)
    ↓ (FlatbuffersTestDataProvider)
C++ Structs (TestData)
    ↓ (TestEngine)
Game Simulation (ticks)
    ↓ (Data Bank Capture)
Engine Snapshots per Tick
    ↓ (harness_runner)
Comparison with Expected Snapshots
    ↓ (Catch2 Matchers)
Test Pass/Fail
```

## Key Design Decisions

### 1. FlatBuffers for Test Data ✅

**Rationale:**
- Schema validation at compile time
- Type-safe data structures
- Same technology used in production (consistency)
- Forward/backward compatibility with schema evolution

**Benefits:**
- Catches data errors early (at build time)
- No JSON parsing at runtime
- Efficient binary format
- Strong typing prevents errors

### 2. Separation of Concerns ✅

The harness is well-organized into distinct modules:

1. **Data Loading** - Discovery, loading, parsing
2. **Test Execution** - TestEngine, simulation, orchestration
3. **Comparison** - Matchers, snapshot comparison

**Benefits:**
- Clear responsibilities
- Easy to test in isolation
- Easy to extend (add new data types, execution modes, comparisons)
- Minimal coupling between layers

### 3. Tick-Based Execution ✅

Tests execute in discrete ticks with coordinated:
- Input injection (mouse/keyboard)
- Event injection (engine events)
- Simulation execution (Logic classes)
- State capture (data bank)

**Benefits:**
- Precise timing control
- Realistic game loop simulation
- Clear execution timeline
- Easy to debug (tick-by-tick analysis)

### 4. Data-Driven with Catch2 Generators ✅

Tests use Catch2's `GENERATE` with `from_range()` to iterate test configs:

```cpp
auto configs = steamrot::tests::load_test_data_configs();
const auto *config = GENERATE_COPY(from_range(configs.value()));
auto result = steamrot::tests::RunTestEngineTest(config);
REQUIRE(result.has_value());
```

**Benefits:**
- One test case, many scenarios
- Easy to add new test data (just add JSON files)
- Clear test organization
- Catch2 reports each config as separate test

## Error Handling Assessment

### ✅ Strengths

1. **Consistent use of std::expected** - All functions return `std::expected<T, FailInfo>`
2. **Comprehensive null checks** - FlatBuffers pointers validated before use
3. **Error propagation** - Errors bubble up the stack with context
4. **FailInfo with FailMode** - Structured error information with categorization
5. **Unit tests for error paths** - Missing directories, null inputs, invalid data all tested

### ⚠️ Areas for Consideration

1. **Error recovery** - No retry mechanisms (acceptable for testing)
2. **Partial failure handling** - If one test data file fails to load, entire load fails (could be more granular)
3. **Error message quality** - Generally good, could include more context in some cases (file paths, expected values)

## Appropriate Usage Analysis

### When to Use test_data.json (TestDataConfig)

✅ **APPROPRIATE:**

1. **Integration Tests** - Testing multiple systems together
   - Example: UI collision + action logic interaction
   - Example: Event propagation through multiple ticks
   - Example: Scene transitions with state validation

2. **System Tests** - End-to-end workflows
   - Example: Complete user interaction (mouse move → click → scene change)
   - Example: Multi-step game mechanics (crafting, combat)

3. **Complex Scenarios** - Multiple ticks, coordinated actions
   - Example: Event lifetime testing over multiple ticks
   - Example: Input sequences with timing requirements
   - Example: State transformations through simulation steps

4. **Regression Tests** - Capturing known-good behavior
   - Example: Specific bug reproductions
   - Example: Feature behavior validation

**Why:** TestDataConfig provides full game simulation capabilities with TestEngine, tick-based execution, and comprehensive state capture. It's designed for testing game logic and interactions.

### When NOT to Use test_data.json

❌ **INAPPROPRIATE:**

1. **Pure Unit Tests** - Testing single components in isolation
   - Example: Component default construction
   - Example: Single function behavior
   - Example: Data structure manipulation
   - **Problem:** Game logic changes affect TestEngine behavior, which can break unit tests unrelated to the change
   - **Alternative:** Direct instantiation, traditional Catch2 tests

2. **Simple Data Configuration Tests** - Basic entity/component setup
   - Example: FlatBuffers entity configuration
   - Example: Component field mapping
   - **Problem:** Overkill - TestEngine overhead unnecessary
   - **Alternative:** Use entity_test_data.json (EntityCollectionFbs) directly

3. **Test Infrastructure Tests** - Testing the test harness itself
   - Example: FlatbuffersTestDataLoader functionality
   - Example: Matcher behavior
   - **Problem:** Circular dependency - using the thing to test itself
   - **Alternative:** Direct instantiation, mock data

4. **Static Validation Tests** - No simulation required
   - Example: Schema validation
   - Example: Data structure correctness
   - **Problem:** TestEngine simulation is unnecessary overhead
   - **Alternative:** Simple loading and validation

### The Problem with test_data.json for Unit Tests

**User's Concern:** "test_data.json is only for testing actual game logic, the problem with using this for unit tests is that the parts of it could change due to changing game logic and affect the tests."

**Analysis:** ✅ **AGREE - This is a valid concern.**

**Why this is problematic:**

1. **Cascading Failures:**
   - Change to Logic class behavior → TestEngine simulation changes → Unrelated unit tests fail
   - Example: Modify UICollisionLogic → All tests using simulation fail, even if testing unrelated components

2. **Brittle Tests:**
   - Unit tests should test ONE thing in isolation
   - TestEngine introduces many moving parts (event system, archetypes, logic execution)
   - Changes to any part can break tests

3. **Unclear Failures:**
   - When a unit test using TestEngine fails, is it:
     - The component being tested?
     - The simulation setup?
     - A change in Logic class behavior?
     - An event system change?

4. **Maintenance Burden:**
   - Game logic evolves frequently
   - Unit tests should be stable and fast to run
   - Rebuilding test data when game logic changes is overhead

**Recommendation:**

✅ **Use test_data.json (TestDataConfig) ONLY for:**
- Integration tests (multiple systems)
- System tests (end-to-end)
- Complex scenarios (multiple ticks, simulations)

✅ **For unit tests, use:**
- **Direct instantiation** - Create objects directly in test code
- **entity_test_data.json** - Simpler, no simulation (just entity configuration)
- **Test helpers** - Reusable setup functions (TestScenarios, etc.)
- **Minimal dependencies** - Avoid TestEngine unless absolutely necessary

## Recommendations

### 1. Documentation ✅ (Already Exists)

The harness README is comprehensive (~1500 lines) covering:
- Overview and workflow
- API reference
- Examples
- Tick-based execution
- Input/event sequences
- EventBus testing

**Action:** Add this analysis document to clarify appropriate usage.

### 2. Separate Simple Data Files ✅ (Already Done)

The codebase already distinguishes:
- `test_data.json` (TestDataConfig) - Full simulation scenarios
- `entity_test_data.json` (EntityCollectionFbs) - Simple entity data for unit tests

**Action:** Document this distinction clearly (see TESTING_BEST_PRACTICES.md).

### 3. Test Categorization ⚠️ (Needs Improvement)

Current structure:
```
tests/
├── unit/           # Mix of true unit tests and integration tests
├── integration/    # Integration tests
└── system/         # System tests (future)
```

**Issue:** Some "unit" tests use TestEngine/simulation (making them integration tests).

**Action:** 
- Add guidelines for categorizing tests
- Consider moving TestEngine-based tests to integration/
- Keep unit/ for true isolation tests

### 4. Error Message Enhancement 🔧 (Optional)

Current error messages are good but could be enhanced:

**Current:**
```
"Input Flatbuffers TestData is null."
```

**Enhanced:**
```
"Input Flatbuffers TestData is null. File: test_data.bin, Expected: TestDataFbs pointer"
```

**Action:** Consider adding more context (file paths, expected values) in FlatbuffersTestDataProvider error messages.

### 5. Partial Failure Handling 🔧 (Optional)

Current behavior: If one test data file fails to load, entire batch fails.

**Alternative:** Continue loading other files, report which failed.

**Pros:** More resilient to partial failures
**Cons:** More complex error handling, may hide problems

**Action:** Evaluate if this is needed (current behavior may be correct - fail fast).

## Conclusion

### Overall Assessment: ✅ **EXCELLENT**

The test harness is well-designed, thoroughly tested, and appropriately separated into layers. The architecture supports data-driven testing effectively.

### Key Strengths:

1. ✅ Clean separation of concerns (loading, execution, comparison)
2. ✅ Comprehensive error handling with std::expected
3. ✅ Thorough unit test coverage (64+ test cases)
4. ✅ Well-documented (~1500 line README)
5. ✅ Extensible design (easy to add new data types, execution modes)
6. ✅ Type-safe with FlatBuffers schemas

### Key Recommendations:

1. ✅ **Clarify appropriate usage** - TestDataConfig for integration/system, not pure unit tests (this document addresses this)
2. ✅ **Document test categorization** - Guidelines for when to use each approach (next document will address this)
3. 🔧 **Consider test organization** - Separate true unit tests from integration tests in directory structure (optional enhancement)
4. 🔧 **Enhance error messages** - More context in some error cases (optional enhancement)

### User's Original Concerns:

1. ✅ "Check that test harness is sensible" - **YES, well-designed**
2. ✅ "test_data.json only for game logic, not unit tests" - **AGREE, valid concern addressed**
3. ✅ "Check error handling tested" - **YES, thoroughly tested**
4. ✅ "Make guide for adding testing/helpers" - **See next documents**

## Next Steps

See the following companion documents:
1. `TESTING_BEST_PRACTICES.md` - When to use each testing approach
2. `ADDING_TEST_HELPERS_GUIDE.md` - Guide for extending test infrastructure
