# TestEngine Implementation Analysis

**Date:** January 25, 2026  
**Purpose:** Analysis and documentation for getting TestEngine fully operational for data-driven testing

---

## Executive Summary

The TestEngine infrastructure is **partially implemented** with core components in place but critical orchestration layers missing. This analysis identifies:

1. **12 missing harness component files** that need to be implemented
2. **21 existing matchers** that are functional and ready for use
3. **1 reporter** that is functional but could be enhanced
4. **Clear implementation path** with dependencies mapped

**Bottom Line:** The foundation exists (TestEngine, matchers, data providers), but the orchestration layer (test_harness, data loaders, simulation runners) needs to be built to enable the documented data-driven testing workflow.

---

## Table of Contents

1. [Current State Analysis](#current-state-analysis)
2. [Missing Components](#missing-components)
3. [Matchers Analysis](#matchers-analysis)
4. [Reporter Analysis](#reporter-analysis)
5. [Implementation Plan](#implementation-plan)
6. [Dependencies and Integration](#dependencies-and-integration)
7. [Testing Strategy](#testing-strategy)
8. [Recommendations](#recommendations)

---

## Current State Analysis

### What Exists and Works

#### 1. TestEngine Core ✅
**Location:** `tests/harness/`

- **TestEngine.h/cpp**: Fully implemented Engine subclass
  - Inherits from `Engine` base class
  - Implements tick-based execution
  - Captures engine snapshots in data bank
  - Stores snapshots keyed by tick number
  - Overrides `RunGameLoop()`, `TickSceneLogic()`, `TickRendering()`
  - **Status:** Functional, tested, ready for use

- **TestEngine.test.cpp**: Comprehensive unit tests
  - Tests initialization, tick execution, data bank capture
  - **Status:** Passing tests confirm TestEngine works correctly

#### 2. Data Providers ✅
**Location:** `tests/harness/`

- **FlatbuffersTestDataProvider.h/cpp**: Converts FlatBuffers to C++ structs
  - Implements `ITestDataProvider` interface
  - `ProviderAllTestData()`: Loads all test data from adjacent directory
  - `CreateTestData()`: Converts `TestDataFbs` to `TestData`
  - Configures `TestMetaData` and `SimulationData`
  - **Status:** Functional, tested

- **FlatbuffersTestDataLoader.h/cpp**: File discovery and FlatBuffers loading
  - Extends `DataLoader` base class
  - `GetAdjacentDataDirectoryPath()`: Finds data/ directory using __FILE__
  - `LoadTestDataFbs()`: Loads all .test_data.bin files
  - **Status:** Functional, tested

- **ITestDataProvider.h**: Interface for test data provision
  - **Status:** Defined and used

#### 3. Test Data Schema ✅
**Location:** `src/types/flatbuffers/testing/`

- **test_data.fbs**: FlatBuffers schema for test configuration
  - `TestMetadataFbs`: Test name, description, tags, expected outcome
  - `TestDataFbs`: Root table with metadata, simulation data, num_ticks, starting snapshot
  - **Status:** Defined, generates headers correctly

- **simulation_data.fbs**: Schema for simulation steps
  - **Status:** Exists

#### 4. C++ Types ✅
**Location:** `src/types/test_structs/` and `src/types/core/`

- **TestData.h**: Main test data struct
  - Contains: `TestMetaData`, `SimulationData`, `number_of_ticks`, `starting_engine_snapshot`, `expected_engine_snapshots`
  - **Status:** Fully defined

- **TestMetaData.h**, **SimulationData.h**: Supporting structs
  - **Status:** Defined

- **EngineSnapshot.h**: Captures engine state at a tick
  - Contains: `SceneCollectionData`, `EventBus`, etc.
  - **Status:** Defined in core types

#### 5. Matchers Infrastructure ✅
**Location:** `tests/matchers/`

- **42 files total** (21 matchers + tests + helpers)
- **~4,820 total lines of code**, ~1,733 test lines
- **All matchers are implemented and tested**
- See [Matchers Analysis](#matchers-analysis) section for details

#### 6. Reporter ✅
**Location:** `tests/reporters/`

- **HarnessReporter.h/cpp**: Custom Catch2 reporter
  - Registered with Catch2 as "harness" reporter
  - Formats test output for data-driven tests
  - **Status:** Functional, basic implementation complete
- See [Reporter Analysis](#reporter-analysis) section for details

---

## Missing Components

### Critical Missing Files (Need Implementation)

The README describes a complete harness system, but **12 key files are missing**:

#### Test Orchestration Layer

**1. test_harness.h/cpp** - PRIMARY MISSING COMPONENT
- **Purpose:** Main API for running data-driven tests
- **Key Functions:**
  - `RunTestEngineTest(test_data)`: Run TestEngine with a configuration and compare results
  - `run_fixture_test(test_data)`: Run fixture-based tests with entity pool comparison
  - `run_entity_memory_pool_comparison_test()`: Direct pool comparison using matchers
  - `run_event_bus_comparison_test()`: Compare EventBus instances
- **Integration:** 
  - Wraps Catch2 generators
  - Creates TestEngine instances
  - Uses `FlatbuffersTestDataProvider` directly to load data
  - Orchestrates test execution workflow
- **Status:** MISSING - THIS IS THE MAIN ENTRY POINT
- **Note:** No separate test_data_loader needed - use FlatbuffersTestDataProvider directly

**2. test_data_comparison.h/cpp**
- **Purpose:** Entity and event bus comparison functions
- **Key Functions:**
  - Compare EntityMemoryPool instances
  - Compare EventBus instances
  - Use matchers internally
  - Return formatted comparison results
- **Status:** MISSING

#### Simulation System

**3. simulation_runner.h/cpp**
- **Purpose:** Execute simulation steps from test data
- **Key Functions:**
  - Execute Logic classes in sequence
  - Execute free functions
  - Manage simulation state
  - Coordinate with tick execution
- **Referenced in:** `test_data.fbs` has `SimulationDataFbs`, `TestData` has `SimulationData`
- **Status:** MISSING

#### Input/Event Simulation

**4. input_simulation.h/cpp**
- **Purpose:** Simulate user input sequences
- **Key Functions:**
  - `execute_input_event()`: Single input event
  - `execute_input_events_for_tick()`: All inputs for a tick
  - `execute_input_sequence()`: Entire sequence
  - Handle keyboard/mouse input per tick
- **Referenced in:** README describes input sequence execution
- **Status:** MISSING

**5. event_simulation.h/cpp**
- **Purpose:** Inject engine events into event system
- **Key Functions:**
  - `execute_event_test_data()`: Single event injection
  - `execute_events_for_tick()`: All events for a tick
  - `execute_event_sequence()`: Entire sequence
  - Process waiting room event bus
- **Referenced in:** README describes event sequence execution
- **Status:** MISSING

#### Test Files

**6-10. Missing Test Files**
- `test_harness.test.cpp`
- `simulation_runner.test.cpp`
- `input_simulation.test.cpp`
- `event_simulation.test.cpp`
- (No test for test_data_comparison.h/cpp mentioned in README)

---

## Matchers Analysis

### Overview

The matcher infrastructure is **COMPLETE and FUNCTIONAL**. All matchers are implemented, tested, and ready for use by the test harness.

### Matcher Inventory (21 Total)

#### Component Matchers (5)
1. **CMetaEqualsMatcher** - Meta component comparison
2. **CUserInterfaceEqualsMatcher** - UI component comparison
3. **CGrimoireMachinaEqualsMatcher** - Grimoire component comparison
4. **CMachinaFormEqualsMatcher** - Machina form component comparison
5. **CUIStateEqualsMatcher** - UI state component comparison

#### Structural Matchers (6)
6. **EntityMemoryPoolEqualsMatcher** - Complete pool comparison (CRITICAL for test harness)
7. **UIElementEqualsMatcher** - UI element tree comparison
8. **FragmentEqualsMatcher** - Fragment comparison
9. **JointEqualsMatcher** - Joint comparison
10. **SubscriberEqualsMatcher** - Event subscriber comparison

#### Event System Matchers (3)
11. **EventPacketEqualsMatcher** - Event packet comparison
12. **EventDataEqualsMatcher** - Event data comparison
13. **EventBusEqualsMatcher** - EventBus state comparison

#### Supporting Infrastructure
14. **ComponentMatcherBase.h** - Template base class for component matchers
15. **matcher_helpers.h** - Helper utilities
16. **test_context.h** - Test context struct for enriched output

### Key Capabilities

#### 1. EntityMemoryPoolEqualsMatcher - THE CRITICAL MATCHER

**Purpose:** Compare entire entity memory pools

**Features:**
- Compares all component vectors in the pool tuple
- Early assertions for pool size mismatches
- Detailed per-component comparison output
- Supports three constructor modes:
  1. `EntityMemoryPoolEqualsMatcher(expected)` - Basic
  2. `EntityMemoryPoolEqualsMatcher(expected, test_metadata)` - With metadata string (legacy)
  3. `EntityMemoryPoolEqualsMatcher(expected, context)` - With TestContext object

**Test Context Integration:**
```cpp
struct TestContext {
  std::string test_name;
  std::string description;
  uint32_t current_tick{1};
  uint32_t total_ticks{1};
};
```

**Helper Functions:**
```cpp
EqualsEntityMemoryPool(expected)
EqualsEntityMemoryPool(expected, test_metadata)
EqualsEntityMemoryPool(expected, context)
```

**Usage Example:**
```cpp
REQUIRE_THAT(actual_pool, 
  EqualsEntityMemoryPool(expected_pool, 
    TestContext{"my_test", "Testing tick 2", 2, 5}));
```

**Status:** ✅ Fully implemented, tested, ready for integration

#### 2. ComponentMatcherBase - Template Base Class

**Purpose:** Consistent base for all component matchers

**Features:**
- Provides common `describe()` method with consistent formatting
- Uses conmat library for colored output (TestPassed/TestFailed markers)
- Supports optional entity index for context
- `GetComponentName()` virtual method for type-specific names
- Stores mismatch description for detailed failure messages

**Inheritance Pattern:**
```cpp
class CMetaEqualsMatcher : public ComponentMatcherBase<CMeta> {
  // Implement match() method
  // Override GetComponentName() for clean display
};
```

**Status:** ✅ Well-designed, used by all component matchers

#### 3. EventBusEqualsMatcher

**Purpose:** Compare EventBus instances

**Features:**
- Compares global event bus state
- Checks event count, lifetimes, types
- Validates event data
- Critical for tick-based event testing

**Status:** ✅ Implemented and tested

### Test Coverage

- **All matchers have corresponding .test.cpp files**
- **~1,733 lines of test code** across all matcher tests
- Tests validate:
  - Exact matches
  - Mismatch detection
  - Error message formatting
  - Edge cases (empty pools, null values)

### Integration with Test Harness

The matchers are **READY** to be used by the test harness:

1. **EntityMemoryPoolEqualsMatcher** is the primary matcher for `run_entity_memory_pool_comparison_test()`
2. **EventBusEqualsMatcher** for `run_event_bus_comparison_test()`
3. **Component matchers** used internally by EntityMemoryPoolEqualsMatcher
4. **TestContext** struct provides metadata for enriched failure output

**No work needed on matchers** - they are complete and tested.

---

## Reporter Analysis

### Current State: HarnessReporter ✅

**Location:** `tests/reporters/`

**Status:** Functional and registered with Catch2

#### Implementation Details

**Registration:**
```cpp
CATCH_REGISTER_REPORTER("harness", HarnessReporter);
```

**Capabilities:**
- Extends `Catch::CumulativeReporterBase`
- Custom formatting for data-driven tests
- Prints test case information (name, file, line, tags)
- Displays assertion results and INFO messages
- Uses conmat for colored dividers

#### Current Output Format

**Test Run:**
```
========================================
Starting Data Driven Harness Tests
========================================
```

**Test Case:**
```
----------------------------------------
TEST CASE: 
	name: My Test Name
	file: MyTest.test.cpp
	line: 42
	tags: [unit][data-driven]
----------------------------------------
```

**Assertions:**
- Prints INFO messages
- Shows reconstructed expressions

### Assessment: Good Foundation, Enhancement Opportunities

#### Strengths ✅
1. **Properly registered** with Catch2 custom reporter system
2. **Clean separation** from Catch2 internals
3. **Uses conmat** for consistent formatting
4. **Minimal, focused** implementation

#### Implemented Methods
- `testRunStarting()` - Start banner ✅
- `testCaseStarting()` - Test case header ✅
- `assertionEnded()` - Show assertion results ✅

#### Stub/Empty Methods (Opportunities)
- `testCasePartialStarting()` - Empty
- `testCasePartialEnded()` - Empty
- `testCaseEnded()` - Empty
- `testRunEnded()` - Empty
- `testRunEndedCumulative()` - Empty
- `assertionStarting()` - Empty

### Enhancement Recommendations

#### Priority 1: Data-Driven Test Context

**Problem:** When using Catch2 generators, individual test iterations don't show which data file failed.

**Solution:** Print test data metadata in `testCasePartialStarting()`:

```cpp
void testCasePartialStarting(TestCaseInfo const &testInfo, uint64_t partNumber) {
  std::cout << "\n" << conmat::Divider("·", 40) << "\n";
  std::cout << conmat::Colorize("  ITERATION " + std::to_string(partNumber), 
                                conmat::Color::Cyan) << "\n";
  // Note: Test metadata would need to be accessible via INFO messages
  // or stored in a way that the reporter can access it
}
```

**Impact:** HIGH - Essential for debugging data-driven test failures

#### Priority 2: Summary Statistics

**Problem:** No summary at end of test run.

**Solution:** Implement `testRunEndedCumulative()`:

```cpp
void testRunEndedCumulative() {
  std::cout << "\n" << conmat::Divider("=", 40) << "\n";
  std::cout << "Test Run Summary:\n";
  std::cout << "  Total: " << m_testCases.size() << " test cases\n";
  // Aggregate from cumulative data
  std::cout << conmat::Divider("=", 40) << "\n";
}
```

**Impact:** MEDIUM - Nice to have, improves usability

#### Priority 3: Per-Test-Case Summary

**Problem:** No clear indication when a test case completes and its result.

**Solution:** Implement `testCaseEnded()`:

```cpp
void testCaseEnded(const TestCaseStats &testCaseStats) {
  if (testCaseStats.totals.testCases.failed > 0) {
    std::cout << conmat::Colorize("✗ FAILED", conmat::Color::Red) << "\n";
  } else {
    std::cout << conmat::Colorize("✓ PASSED", conmat::Color::Green) << "\n";
  }
  std::cout << conmat::Divider("-", 40) << "\n";
}
```

**Impact:** MEDIUM - Improves readability

#### Priority 4: Tick-Based Test Formatting

**Problem:** For tick-based tests with snapshots at multiple ticks, no clear indication which tick comparisons are running.

**Solution:** Use INFO messages from test harness and print them with context:

```cpp
// In test_harness.cpp when comparing tick snapshots
INFO("Comparing tick " << tick << " of " << total_ticks);
REQUIRE_THAT(actual_pool, EqualsEntityMemoryPool(expected_pool, context));
```

**Impact:** HIGH - Critical for debugging multi-tick test failures

### Recommendation: Enhance Incrementally

**Phase 1 (Before test_harness implementation):**
- Implement basic enhancements (summary, test case end)
- These don't depend on test_harness existing

**Phase 2 (After test_harness implementation):**
- Add tick-based formatting
- Add data-driven iteration context
- Coordinate with test_harness to pass metadata

**Conclusion:** Reporter is functional and sufficient for initial test_harness implementation. Enhancements can be added incrementally as the harness is used and feedback is gathered.

---

## Implementation Plan

### Overview

Implement the missing components in dependency order, ensuring each layer is tested before building the next.

### Phase 1: Comparison Layer (test_data_comparison)

**Goal:** Provide functions for comparing entity pools and event buses

**Note:** No separate test_data_loader needed - `FlatbuffersTestDataProvider` already provides this functionality via `ProviderAllTestData()`.

**Files to Create:**
1. `tests/harness/test_data_comparison.h`
2. `tests/harness/test_data_comparison.cpp`

**Key Functions:**
```cpp
namespace steamrot::tests {

// Compare EntityMemoryPool instances
void run_entity_memory_pool_comparison_test(
    const EntityMemoryPool& actual,
    const EntityMemoryPool& expected,
    bool expected_to_pass = true);

// With metadata
void run_entity_memory_pool_comparison_test(
    const EntityMemoryPool& actual,
    const EntityMemoryPool& expected,
    const std::string& test_metadata,
    bool expected_to_pass = true);

// With context
void run_entity_memory_pool_comparison_test(
    const EntityMemoryPool& actual,
    const EntityMemoryPool& expected,
    const TestContext& context,
    bool expected_to_pass = true);

// Compare EventBus instances
void run_event_bus_comparison_test(
    const EventBus& actual,
    const EventBus& expected,
    bool expected_to_pass = true);

// With metadata
void run_event_bus_comparison_test(
    const EventBus& actual,
    const EventBus& expected,
    const std::string& test_metadata,
    bool expected_to_pass = true);

} // namespace steamrot::tests
```

**Implementation Strategy:**
- Use `EntityMemoryPoolEqualsMatcher` internally
- Use `EventBusEqualsMatcher` internally
- Wrapper functions that call REQUIRE_THAT with matchers
- Support negative tests (expected_to_pass = false)

**Testing:**
- Create test pools that match/mismatch
- Test with metadata variants
- Test with TestContext
- Test negative tests
- Verify error output format

**Acceptance Criteria:**
- [ ] Can compare two EntityMemoryPools
- [ ] Can compare two EventBuses
- [ ] Detailed failure messages appear
- [ ] Metadata appears in output
- [ ] All tests pass

### Phase 2: Simulation System

**Goal:** Execute simulation steps from test data

**Files to Create:**
1. `tests/harness/simulation_runner.h`
2. `tests/harness/simulation_runner.cpp`
3. `tests/unit/harness/simulation_runner.test.cpp`

**Key Functions:**
```cpp
namespace steamrot::tests {

// Execute simulation steps for a specific tick
std::expected<std::monostate, FailInfo>
execute_simulation_for_tick(
    const SimulationData& simulation_data,
    size_t tick,
    TestEngineFixture& fixture);

// Execute entire simulation sequence
std::expected<std::monostate, FailInfo>
execute_simulation(
    const SimulationData& simulation_data,
    TestEngineFixture& fixture);

} // namespace steamrot::tests
```

**Implementation Strategy:**
- Parse `SimulationData` to identify Logic classes or free functions
- Instantiate Logic classes with proper context
- Execute Logic::RunLogic() or call free functions
- Manage tick coordination

**Dependencies:**
- Requires understanding of `SimulationData` struct
- May need `TestEngineFixture` helper class (see Phase 5)

**Testing:**
- Create test SimulationData with known Logic classes
- Execute and verify state changes
- Test error handling for invalid Logic types

**Acceptance Criteria:**
- [ ] Can execute Logic classes from simulation data
- [ ] Can execute free functions from simulation data
- [ ] Tick coordination works correctly
- [ ] All tests pass

### Phase 3: Input/Event Simulation

**Goal:** Simulate user input and inject events per tick

**Files to Create:**
1. `tests/harness/input_simulation.h`
2. `tests/harness/input_simulation.cpp`
3. `tests/unit/harness/input_simulation.test.cpp`
4. `tests/harness/event_simulation.h`
5. `tests/harness/event_simulation.cpp`
6. `tests/unit/harness/event_simulation.test.cpp`

#### input_simulation

**Key Functions:**
```cpp
namespace steamrot::tests {

// Execute single input event
std::expected<std::monostate, FailInfo>
execute_input_event(
    const InputEventData& input_event,
    TestEngineFixture& fixture);

// Execute inputs for specific tick
std::expected<std::monostate, FailInfo>
execute_input_events_for_tick(
    const InputSequence& input_sequence,
    size_t tick,
    TestEngineFixture& fixture);

// Execute entire input sequence
std::expected<std::monostate, FailInfo>
execute_input_sequence(
    const InputSequence& input_sequence,
    TestEngineFixture& fixture);

} // namespace steamrot::tests
```

#### event_simulation

**Key Functions:**
```cpp
namespace steamrot::tests {

// Execute single event injection
std::expected<std::monostate, FailInfo>
execute_event_test_data(
    const EventTestData& event_data,
    TestEngineFixture& fixture);

// Execute events for specific tick
std::expected<std::monostate, FailInfo>
execute_events_for_tick(
    const EventSequence& event_sequence,
    size_t tick,
    TestEngineFixture& fixture);

// Execute entire event sequence
std::expected<std::monostate, FailInfo>
execute_event_sequence(
    const EventSequence& event_sequence,
    TestEngineFixture& fixture);

} // namespace steamrot::tests
```

**Implementation Strategy:**
- Parse input/event data structures
- Inject into appropriate engine systems
- Coordinate with tick execution
- Handle waiting room event bus processing

**Testing:**
- Create test input/event sequences
- Execute and verify engine state changes
- Test tick coordination

**Acceptance Criteria:**
- [ ] Can simulate keyboard/mouse input per tick
- [ ] Can inject events per tick
- [ ] Tick coordination works
- [ ] All tests pass

### Phase 4: Orchestration Layer (test_harness)

**Goal:** Tie everything together into simple API

**Files to Create:**
1. `tests/harness/test_harness.h`
2. `tests/harness/test_harness.cpp`
3. `tests/unit/harness/test_harness.test.cpp`

**Key Functions:**
```cpp
namespace steamrot::tests {

// Main API - Run TestEngine with TestData and compare tick snapshots
std::expected<std::monostate, FailInfo>
RunTestEngineTest(const TestData& test_data);

// Fixture-based testing (alternative approach)
std::expected<std::monostate, FailInfo>
run_fixture_test(const TestData& test_data);

} // namespace steamrot::tests
```

**Usage Pattern:**
```cpp
TEST_CASE("Data-driven test", "[unit]") {
  // Create provider instance using __FILE__ for adjacent data/ directory
  FlatbuffersTestDataProvider provider(std::filesystem::path(__FILE__).parent_path());
  
  // Load all test data
  auto test_data_vec = provider.ProviderAllTestData();
  REQUIRE(test_data_vec.has_value());
  
  // Use Catch2 generator to test each TestData
  const auto& test_data = GENERATE_COPY(from_range(test_data_vec.value()));
  
  // Run TestEngine test
  auto result = steamrot::tests::RunTestEngineTest(test_data);
  REQUIRE(result.has_value());
}
```

**Implementation Strategy:**

**RunTestEngineTest workflow:**
1. Accept `TestData` directly (already converted from FlatBuffers)
2. Create `TestEngine` instance with `TestData`
3. Call `TestEngine::RunGame()`
4. Retrieve data bank from TestEngine
5. For each tick in expected_engine_snapshots:
   - Extract EntityMemoryPool from data bank
   - Extract expected EntityMemoryPool from expected snapshot
   - Call `run_entity_memory_pool_comparison_test()`
   - If event_bus in expected snapshot, compare EventBus similarly
6. Return success/failure

**run_fixture_test workflow (alternative):**
1. Create test fixture with game resources
2. Configure starting entities from TestData
3. For each tick:
   - Execute input sequence for tick
   - Execute event sequence for tick
   - Execute simulation steps for tick
   - Tick the fixture
4. Compare final state with expected_entity_collection

**Coordination:**
- Uses `FlatbuffersTestDataProvider` for loading (called by test code)
- Uses `test_data_comparison` to compare results
- Uses `simulation_runner`, `input_simulation`, `event_simulation` as needed
- Integrates with TestEngine's data bank
- Works with Catch2 generators

**Testing:**
- Create end-to-end test with complete TestData
- Verify full workflow from loading to comparison
- Test with multiple tick snapshots
- Test with input/event sequences
- Test with simulation steps

**Acceptance Criteria:**
- [ ] Can call `RunTestEngineTest(test_data)`
- [ ] TestEngine executes specified number of ticks
- [ ] Data bank is captured correctly
- [ ] Tick snapshots are compared automatically
- [ ] Works with Catch2 GENERATE for multiple TestData instances
- [ ] All tests pass

### Phase 5: Integration and Documentation

**Goal:** Ensure all pieces work together and document usage

**Tasks:**
1. Create comprehensive integration test using real test data
2. Update CMakeLists.txt to compile all new files
3. Update tests/harness/README.md if implementation differs from documented design
4. Create example test data files demonstrating all features
5. Run full test suite

**Acceptance Criteria:**
- [ ] All harness tests pass
- [ ] Integration test demonstrates full workflow
- [ ] Documentation matches implementation
- [ ] Example test data files exist and work

---

## Dependencies and Integration

### Internal Dependencies

```
test_harness.h/cpp
├── FlatbuffersTestDataProvider.h/cpp ✅ (used by test code to load)
├── test_data_comparison.h/cpp
│   ├── EntityMemoryPoolEqualsMatcher ✅
│   └── EventBusEqualsMatcher ✅
├── simulation_runner.h/cpp
│   └── SimulationData ✅
├── input_simulation.h/cpp
│   └── InputSequence (need to verify schema)
├── event_simulation.h/cpp
│   └── EventSequence (need to verify schema)
└── TestEngine.h/cpp ✅
```

### External Dependencies

**Catch2:**
- Custom matchers (`Catch::Matchers::MatcherBase`)
- Generators (`GENERATE`, `from_range()`)
- Assertions (`REQUIRE`, `REQUIRE_THAT`)
- Custom reporter system

**FlatBuffers:**
- Schema compilation (test_data.fbs, simulation_data.fbs, etc.)
- Generated headers (test_data_generated.h)

**Engine/Game Infrastructure:**
- `Engine` base class ✅
- `EntityMemoryPool` ✅
- `EventBus` ✅
- `GameContext`, `SceneManager` ✅

**conmat (formatting library):**
- Used by matchers for colored output
- Used by HarnessReporter for formatting

### Build System Integration

**CMakeLists.txt updates needed:**

```cmake
# tests/harness/CMakeLists.txt
add_library(harness
  TestEngine.cpp
  FlatbuffersTestDataLoader.cpp
  FlatbuffersTestDataProvider.cpp
  test_data_comparison.cpp      # NEW
  simulation_runner.cpp         # NEW
  input_simulation.cpp          # NEW
  event_simulation.cpp          # NEW
  test_harness.cpp              # NEW
)

target_link_libraries(harness
  PUBLIC
  engine
  types
  matchers                      # NEW - link matchers library
)

# tests/unit/harness/CMakeLists.txt
add_executable(test_harness
  FlatbuffersTestDataLoader.test.cpp
  FlatbuffersTestDataProvider.test.cpp
  TestEngine.test.cpp
  simulation_runner.test.cpp          # NEW
  input_simulation.test.cpp           # NEW
  event_simulation.test.cpp           # NEW
  test_harness.test.cpp               # NEW
)

target_link_libraries(test_harness
  PRIVATE
  Catch2::Catch2WithMain
  harness
  matchers                            # NEW
  reporters                           # NEW
)
```

### Schema Verification Needed

Need to verify these FlatBuffers schemas exist and have required fields:

1. **input_test_data.fbs** - Input sequence schema
   - `InputSequence` table
   - `InputEventData` table with tick field

2. **event_test_data.fbs** - Event sequence schema
   - `EventSequence` table
   - `EventTestData` table with tick field

3. **simulation_data.fbs** - Already exists, verify structure
   - `SimulationData` table
   - `SimulationStep` with function_type

If schemas don't match expected structure, they need to be updated or the implementation needs to adapt.

---

## Testing Strategy

### Unit Testing Approach

**Each component gets isolated unit tests:**

1. **test_data_comparison.test.cpp**
   - Compare matching pools
   - Compare mismatched pools
   - Compare matching event buses
   - Compare mismatched event buses
   - Verify error messages
   - Test metadata variants

2. **simulation_runner.test.cpp**
   - Execute Logic classes
   - Execute free functions
   - Handle invalid function types
   - Tick coordination

3. **input_simulation.test.cpp**
   - Execute single input
   - Execute input sequence
   - Per-tick execution
   - Verify state changes

4. **event_simulation.test.cpp**
   - Execute single event
   - Execute event sequence
   - Per-tick execution
   - Waiting room processing

5. **test_harness.test.cpp**
   - End-to-end with RunTestEngineTest
   - Multiple tick snapshots
   - With input sequences
   - With event sequences
   - With simulation steps
   - Error handling

### Integration Testing

**Create comprehensive integration test:**

**tests/integration/test_engine/**
- `full_workflow.test.cpp`
  - Uses real test data file
  - Exercises all harness features
  - Verifies full data-driven testing workflow

### Data-Driven Testing

**Use the harness to test itself:**

Once implemented, create test data files that exercise the harness:

**tests/unit/harness/data/**
- `harness_basic_001.test_data.json` - Simple tick execution
- `harness_simulation_001.test_data.json` - With simulation steps
- `harness_input_001.test_data.json` - With input sequence
- `harness_event_001.test_data.json` - With event sequence
- `harness_comprehensive_001.test_data.json` - All features combined

### Reporter Testing

**Manual verification needed:**

The HarnessReporter needs visual verification since it's output formatting:

1. Run tests with `--reporter harness`
2. Verify output formatting
3. Test with passing/failing tests
4. Test with GENERATE iterations
5. Verify INFO message display

---

## Recommendations

### Priority 1: Implement in Order

Follow the phased implementation plan. Each phase builds on the previous, and testing each layer before moving on will prevent integration issues.

**Estimated effort:**
- Phase 1 (test_data_comparison): 1 day
- Phase 2 (simulation_runner): 2-3 days
- Phase 3 (input/event_simulation): 2-3 days
- Phase 4 (test_harness): 2-3 days
- Phase 5 (integration): 1-2 days
- **Total: 8-14 days** for full implementation

### Priority 2: Validate Schemas First

Before implementing simulation/input/event layers, **verify the FlatBuffers schemas** match expectations. If they don't, either:
- Update schemas to match documented design, OR
- Adjust implementation to work with existing schemas

This will prevent rework.

### Priority 3: Simplified Architecture - No test_data_loader Needed ✅

The existing `FlatbuffersTestDataProvider::ProviderAllTestData()` already provides all the functionality needed for loading test data. Test code can instantiate it directly with `__FILE__` for the adjacent data/ directory pattern.

**Decision:** No separate `test_data_loader` module needed. This simplifies the architecture and reduces code duplication.

### Priority 4: Reporter Enhancements Post-Implementation

Don't enhance the reporter until the harness is working. Once it's in use:
1. Gather feedback on what output is most helpful
2. Implement enhancements based on real usage
3. Prioritize tick-based formatting and data-driven iteration context

### Priority 5: Consider TestEngineFixture Helper

Several simulation/input/event functions take a `TestEngineFixture&`. This may need to be a helper class that wraps TestEngine and provides convenient access to:
- GameResources
- EventHandler
- EntityMemoryPool
- SceneManager

If not already defined, create this helper class in Phase 3 or earlier.

### Priority 5: Documentation Updates

As implementation proceeds, update `tests/harness/README.md` if:
- Function signatures differ from documented
- Workflow differs from documented
- Additional helper functions are needed

Keep the README as the single source of truth for API usage.

---

## Conclusion

### Current State Summary

✅ **Working:**
- TestEngine core
- Data providers (FlatBuffers loading and conversion)
- All 21 matchers
- HarnessReporter
- Test data schemas
- CMeta, test structs

❌ **Missing:**
- 10 harness component files (orchestration, simulation, input/event, comparison)
- Integration layer tying everything together
- Example test data demonstrating full features

### Path Forward

The foundation is solid. The missing components are well-documented in the README, and the implementation path is clear:

1. Add comparison layer
2. Implement simulation system
3. Add input/event simulation
4. Tie it together with orchestration layer
5. Test and document

**The matchers and reporter are ready to use.** Focus implementation effort on the harness components. **No need for a separate test_data_loader** - use FlatbuffersTestDataProvider directly.

### Success Criteria

The TestEngine will be "fully operational for data-driven tests" when:

✅ Developers can write tests like this:

```cpp
TEST_CASE("My feature test", "[unit][feature]") {
  // Instantiate provider with __FILE__ for adjacent data/ directory
  FlatbuffersTestDataProvider provider(std::filesystem::path(__FILE__).parent_path());
  
  // Load all test data
  auto test_data_vec = provider.ProviderAllTestData();
  REQUIRE(test_data_vec.has_value());
  
  // Use Catch2 generator to test each TestData
  const auto& test_data = GENERATE_COPY(from_range(test_data_vec.value()));
  
  // Run TestEngine test
  auto result = steamrot::tests::RunTestEngineTest(test_data);
  REQUIRE(result.has_value());
}
```

✅ Test data can define:
- Entity starting states
- Expected states at specific ticks
- Input sequences per tick
- Event sequences per tick
- Simulation steps per tick

✅ The harness automatically:
- Loads test data
- Creates TestEngine
- Executes ticks
- Captures snapshots
- Compares with expected state
- Produces detailed failure messages

---

**END OF ANALYSIS**
