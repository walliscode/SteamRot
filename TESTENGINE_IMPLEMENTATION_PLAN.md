# TestEngine Implementation Plan

**Project:** Get TestEngine fully operational for data-driven testing  
**Estimated Duration:** 8-14 days  
**Status:** Planning Phase  
**Last Updated:** January 25, 2026

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Prerequisites](#prerequisites)
3. [Phase 1: Comparison Layer](#phase-1-comparison-layer)
4. [Phase 2: Simulation System](#phase-2-simulation-system)
5. [Phase 3: Input/Event Simulation](#phase-3-inputevent-simulation)
6. [Phase 4: Orchestration Layer](#phase-4-orchestration-layer)
7. [Phase 5: Integration & Documentation](#phase-5-integration--documentation)
8. [Testing Strategy](#testing-strategy)
9. [Risk Mitigation](#risk-mitigation)
10. [Success Criteria](#success-criteria)

---

## Executive Summary

### Goal
Enable developers to write data-driven tests using TestEngine with test configurations defined in JSON files.

### Current State
- **77% Complete**: 51 of 66 files implemented
- **Ready**: TestEngine core, 21 matchers, reporter, data providers
- **Missing**: 15 files (10 implementation + 5 test files)

### Phases
5 sequential phases over 8-14 days, each with clear deliverables and acceptance criteria.

### Target Outcome
```cpp
TEST_CASE("Feature test", "[unit]") {
  FlatbuffersTestDataProvider provider(std::filesystem::path(__FILE__).parent_path());
  auto test_data_vec = provider.ProviderAllTestData().value();
  const auto& test_data = GENERATE_COPY(from_range(test_data_vec));
  
  auto result = steamrot::tests::RunTestEngineTest(test_data);
  REQUIRE(result.has_value());
}
```

---

## Prerequisites

### Before Starting Implementation

#### 1. Validate FlatBuffers Schemas ⚠️ CRITICAL

**Task:** Verify these schemas exist and have required structures:

**Input Simulation Schema:**
```bash
# Check if schema exists
ls src/types/flatbuffers/testing/input_test_data.fbs

# Required tables:
# - InputSequence with array of InputEventData
# - InputEventData with: tick, input_type, input_data
```

**Event Simulation Schema:**
```bash
# Check if schema exists
ls src/types/flatbuffers/testing/event_test_data.fbs

# Required tables:
# - EventSequence with array of EventTestData
# - EventTestData with: tick, event_packet
```

**Action Items:**
- [ ] Review `src/types/flatbuffers/testing/input_test_data.fbs`
- [ ] Review `src/types/flatbuffers/testing/event_test_data.fbs`
- [ ] If schemas don't match expected structure, decide: update schema or adapt implementation
- [ ] Document any schema changes needed

#### 2. Set Up Development Environment

**Build Configuration:**
```bash
# Configure with Debug preset
cmake --preset Debug

# Build harness library (should compile without errors)
cmake --build --preset Debug --target harness

# Run existing harness tests
ctest --preset Debug -R test_harness
```

**Expected Result:** Existing tests pass, harness library compiles cleanly.

#### 3. Create Work Branch Strategy

**Recommended Approach:**
- Create feature branches for each phase
- Merge to main implementation branch after phase completion
- Allows parallel review of completed phases

```bash
# Example branch naming
git checkout -b feature/testengine-phase1-comparison
# After phase 1 complete and reviewed
git checkout -b feature/testengine-phase2-simulation
```

---

## Phase 1: Comparison Layer

**Duration:** 1 day  
**Files:** 2 implementation files  
**Dependencies:** EntityMemoryPoolEqualsMatcher ✅, EventBusEqualsMatcher ✅

### Objective
Provide wrapper functions for comparing EntityMemoryPool and EventBus instances using existing matchers.

### Files to Create

#### 1.1 Create `tests/harness/test_data_comparison.h`

**Location:** `/home/runner/work/SteamRot/SteamRot/tests/harness/test_data_comparison.h`

**Content Structure:**
```cpp
#pragma once

#include "containers.h"
#include "EventBus.h"
#include "FailInfo.h"
#include "test_context.h"
#include <expected>
#include <string>

namespace steamrot::tests {

// EntityMemoryPool comparison
void run_entity_memory_pool_comparison_test(
    const EntityMemoryPool& actual,
    const EntityMemoryPool& expected,
    bool expected_to_pass = true);

void run_entity_memory_pool_comparison_test(
    const EntityMemoryPool& actual,
    const EntityMemoryPool& expected,
    const std::string& test_metadata,
    bool expected_to_pass = true);

void run_entity_memory_pool_comparison_test(
    const EntityMemoryPool& actual,
    const EntityMemoryPool& expected,
    const TestContext& context,
    bool expected_to_pass = true);

// EventBus comparison
void run_event_bus_comparison_test(
    const EventBus& actual,
    const EventBus& expected,
    bool expected_to_pass = true);

void run_event_bus_comparison_test(
    const EventBus& actual,
    const EventBus& expected,
    const std::string& test_metadata,
    bool expected_to_pass = true);

} // namespace steamrot::tests
```

**Key Design Decisions:**
- Use `void` return type (throws on failure via Catch2 REQUIRE_THAT)
- Support three patterns: no metadata, string metadata, TestContext
- `expected_to_pass` parameter enables negative testing

**Checklist:**
- [ ] Create header file with proper guards
- [ ] Add Doxygen documentation for each function
- [ ] Follow project style guide (2-space indent, visual dividers)
- [ ] Add to git: `git add tests/harness/test_data_comparison.h`

#### 1.2 Create `tests/harness/test_data_comparison.cpp`

**Location:** `/home/runner/work/SteamRot/SteamRot/tests/harness/test_data_comparison.cpp`

**Implementation Strategy:**
```cpp
#include "test_data_comparison.h"
#include "EntityMemoryPoolEqualsMatcher.h"
#include "EventBusEqualsMatcher.h"
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

void run_entity_memory_pool_comparison_test(
    const EntityMemoryPool& actual,
    const EntityMemoryPool& expected,
    bool expected_to_pass) {
  
  if (expected_to_pass) {
    REQUIRE_THAT(actual, EqualsEntityMemoryPool(expected));
  } else {
    REQUIRE_THAT(actual, !EqualsEntityMemoryPool(expected));
  }
}

void run_entity_memory_pool_comparison_test(
    const EntityMemoryPool& actual,
    const EntityMemoryPool& expected,
    const std::string& test_metadata,
    bool expected_to_pass) {
  
  if (expected_to_pass) {
    REQUIRE_THAT(actual, EqualsEntityMemoryPool(expected, test_metadata));
  } else {
    REQUIRE_THAT(actual, !EqualsEntityMemoryPool(expected, test_metadata));
  }
}

// Similar implementations for TestContext variant and EventBus functions
```

**Checklist:**
- [ ] Implement all 5 function overloads
- [ ] Use existing matchers (EntityMemoryPoolEqualsMatcher, EventBusEqualsMatcher)
- [ ] Handle positive and negative test cases
- [ ] Add to git: `git add tests/harness/test_data_comparison.cpp`

#### 1.3 Update `tests/harness/CMakeLists.txt`

**Add to library sources:**
```cmake
add_library(harness
  TestEngine.cpp
  FlatbuffersTestDataLoader.cpp
  FlatbuffersTestDataProvider.cpp
  test_data_comparison.cpp      # ADD THIS LINE
)

# Ensure matchers are linked
target_link_libraries(harness
  PUBLIC
  engine
  types
  matchers  # Ensure this is present
)
```

**Checklist:**
- [ ] Add test_data_comparison.cpp to harness library
- [ ] Verify matchers library is linked
- [ ] Test build: `cmake --build --preset Debug --target harness`

#### 1.4 Create Unit Tests

**Create:** `tests/unit/harness/test_data_comparison.test.cpp`

**Test Cases:**
```cpp
#include "test_data_comparison.h"
#include "TestScenarios.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("run_entity_memory_pool_comparison_test with matching pools", 
          "[unit][test_data_comparison]") {
  auto pool1 = steamrot::tests::TestScenarios::CreatePoolWithMultipleUIEntities(5);
  auto pool2 = steamrot::tests::TestScenarios::CreatePoolWithMultipleUIEntities(5);
  
  REQUIRE_NOTHROW(
    steamrot::tests::run_entity_memory_pool_comparison_test(pool1, pool2)
  );
}

TEST_CASE("run_entity_memory_pool_comparison_test with mismatched pools fails",
          "[unit][test_data_comparison]") {
  auto pool1 = steamrot::tests::TestScenarios::CreatePoolWithMultipleUIEntities(5);
  auto pool2 = steamrot::tests::TestScenarios::CreatePoolWithMultipleUIEntities(3);
  
  REQUIRE_THROWS(
    steamrot::tests::run_entity_memory_pool_comparison_test(pool1, pool2)
  );
}

// Add tests for: metadata variants, negative tests, EventBus comparison
```

**Checklist:**
- [ ] Test matching pools (should pass)
- [ ] Test mismatched pools (should throw)
- [ ] Test with metadata string
- [ ] Test with TestContext
- [ ] Test negative testing (expected_to_pass = false)
- [ ] Test EventBus comparison
- [ ] Add to CMakeLists: `tests/unit/harness/CMakeLists.txt`

#### 1.5 Build and Test

**Commands:**
```bash
# Build harness library
cmake --build --preset Debug --target harness

# Build test executable
cmake --build --preset Debug --target test_harness

# Run tests
ctest --preset Debug -R test_harness --output-on-failure
```

**Checklist:**
- [ ] Library compiles without errors
- [ ] Tests compile without errors
- [ ] All tests pass
- [ ] No memory leaks (run with valgrind if available)

### Phase 1 Deliverables

- [ ] `test_data_comparison.h` - Header with 5 function declarations
- [ ] `test_data_comparison.cpp` - Implementation using matchers
- [ ] Updated `tests/harness/CMakeLists.txt`
- [ ] `test_data_comparison.test.cpp` - Unit tests
- [ ] Updated `tests/unit/harness/CMakeLists.txt`
- [ ] All tests passing

### Phase 1 Acceptance Criteria

✅ **Functionality:**
- Can compare two EntityMemoryPools and get detailed output
- Can compare two EventBuses and get detailed output
- Metadata appears in failure messages
- Negative tests work correctly

✅ **Quality:**
- Code follows project style guide
- All functions documented with Doxygen
- Unit tests achieve >90% code coverage
- No compiler warnings

✅ **Integration:**
- Builds cleanly as part of harness library
- Tests pass in CI/CD pipeline

---

## Phase 2: Simulation System

**Duration:** 2-3 days  
**Files:** 3 files (2 implementation + 1 test)  
**Dependencies:** SimulationData ✅, Logic system ✅

### Objective
Execute simulation steps (Logic classes or free functions) defined in test data.

### Files to Create

#### 2.1 Create `tests/harness/simulation_runner.h`

**Key Functions:**
```cpp
namespace steamrot::tests {

// Execute simulation steps for specific tick
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

**Design Considerations:**
- May need to create `TestEngineFixture` helper class
- Fixture provides access to: GameResources, EventHandler, EntityMemoryPool, SceneManager
- Parse `SimulationData::steps` to identify Logic classes or free functions
- Handle both `FunctionEnum` and `LogicClassEnum` variants

**Checklist:**
- [ ] Define TestEngineFixture class (if needed)
- [ ] Create simulation_runner.h with function declarations
- [ ] Document expected SimulationData structure
- [ ] Add Doxygen comments

#### 2.2 Create `tests/harness/simulation_runner.cpp`

**Implementation Approach:**

**For Logic Classes:**
```cpp
// Extract LogicClassEnum from SimulationElement
if (std::holds_alternative<LogicClassEnum>(element)) {
  auto logic_enum = std::get<LogicClassEnum>(element);
  
  // Create Logic instance based on enum
  switch (logic_enum) {
    case LogicClassEnum::UIActionLogic:
      auto logic = std::make_unique<UIActionLogic>(logic_context);
      logic->RunLogic();
      break;
    // ... other cases
  }
}
```

**For Free Functions:**
```cpp
// Extract FunctionEnum from SimulationElement
if (std::holds_alternative<FunctionEnum>(element)) {
  auto func_enum = std::get<FunctionEnum>(element);
  
  // Call function based on enum
  switch (func_enum) {
    case FunctionEnum::ProcessUIActionsAndEvents:
      ProcessUIActionsAndEvents(/* parameters */);
      break;
    // ... other cases
  }
}
```

**Checklist:**
- [ ] Implement TestEngineFixture if needed
- [ ] Implement execute_simulation_for_tick
- [ ] Implement execute_simulation
- [ ] Handle both Logic classes and free functions
- [ ] Error handling for invalid enum values
- [ ] Add to CMakeLists.txt

#### 2.3 Create Unit Tests

**Create:** `tests/unit/harness/simulation_runner.test.cpp`

**Test Strategy:**
```cpp
TEST_CASE("execute_simulation with Logic class", "[unit][simulation_runner]") {
  // Create SimulationData with LogicClassEnum
  steamrot::SimulationData sim_data;
  steamrot::SimulationElement element = steamrot::LogicClassEnum::UIActionLogic;
  sim_data.steps.push_back(steamrot::SimulationStep(element));
  
  // Create fixture (TestEngineFixture)
  // Execute simulation
  // Verify Logic was executed (check state changes)
}

TEST_CASE("execute_simulation with free function", "[unit][simulation_runner]") {
  // Similar but with FunctionEnum
}

TEST_CASE("execute_simulation_for_tick executes only matching tick", "[unit][simulation_runner]") {
  // Verify tick filtering works
}
```

**Checklist:**
- [ ] Test Logic class execution
- [ ] Test free function execution
- [ ] Test per-tick filtering
- [ ] Test error handling for invalid enums
- [ ] Test empty simulation data
- [ ] Add to CMakeLists.txt

#### 2.4 Build and Test

**Checklist:**
- [ ] Build succeeds
- [ ] All tests pass
- [ ] Integration with TestEngine verified

### Phase 2 Deliverables

- [ ] `simulation_runner.h`
- [ ] `simulation_runner.cpp`
- [ ] `simulation_runner.test.cpp`
- [ ] Updated CMakeLists.txt files
- [ ] (Optional) `TestEngineFixture` helper class

### Phase 2 Acceptance Criteria

✅ Can execute Logic classes from SimulationData  
✅ Can execute free functions from SimulationData  
✅ Per-tick execution filtering works  
✅ Error handling for invalid configurations  
✅ All tests passing

---

## Phase 3: Input/Event Simulation

**Duration:** 2-3 days  
**Files:** 4 files (4 implementation)  
**Dependencies:** Input/Event schemas ⚠️, EventHandler ✅

### Objective
Simulate user input sequences and inject engine events on a per-tick basis.

### 3.1 Input Simulation

#### Create `tests/harness/input_simulation.h`

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

**Design:**
- Parse `InputSequence` from test data
- For each tick, inject keyboard/mouse input into event system
- Update input state in GameResources

**Checklist:**
- [ ] Create header with function declarations
- [ ] Document InputSequence and InputEventData structures
- [ ] Add Doxygen comments

#### Create `tests/harness/input_simulation.cpp`

**Implementation:**
```cpp
std::expected<std::monostate, FailInfo>
execute_input_event(
    const InputEventData& input_event,
    TestEngineFixture& fixture) {
  
  // Based on input type (keyboard/mouse)
  if (input_event.type == InputType::Keyboard) {
    // Update keyboard state in fixture
    fixture.GetEventHandler().HandleKeyPress(input_event.key);
  } else if (input_event.type == InputType::Mouse) {
    // Update mouse state
    fixture.GetEventHandler().HandleMouseClick(input_event.button, input_event.position);
  }
  
  return std::monostate{};
}
```

**Checklist:**
- [ ] Implement all three functions
- [ ] Handle keyboard input
- [ ] Handle mouse input (buttons, position)
- [ ] Filter by tick number
- [ ] Add to CMakeLists.txt

#### Create `tests/unit/harness/input_simulation.test.cpp`

**Test Cases:**
```cpp
TEST_CASE("execute_input_event injects keyboard input", "[unit][input_simulation]") {
  // Create InputEventData for keyboard
  // Execute
  // Verify event handler received input
}

TEST_CASE("execute_input_events_for_tick filters by tick", "[unit][input_simulation]") {
  // Create sequence with events at different ticks
  // Execute for tick 1
  // Verify only tick 1 events executed
}
```

**Checklist:**
- [ ] Test keyboard input
- [ ] Test mouse input
- [ ] Test tick filtering
- [ ] Test error cases
- [ ] Add to CMakeLists.txt

### 3.2 Event Simulation

#### Create `tests/harness/event_simulation.h`

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

**Checklist:**
- [ ] Create header
- [ ] Document EventSequence structure
- [ ] Add Doxygen comments

#### Create `tests/harness/event_simulation.cpp`

**Implementation:**
```cpp
std::expected<std::monostate, FailInfo>
execute_event_test_data(
    const EventTestData& event_data,
    TestEngineFixture& fixture) {
  
  // Create EventPacket from test data
  steamrot::EventPacket packet = CreateEventPacketFromTestData(event_data);
  
  // Inject into waiting room event bus
  fixture.GetEventHandler().AddEventToWaitingRoom(packet);
  
  return std::monostate{};
}

// Don't forget to process waiting room!
void ProcessEventsAfterTick(TestEngineFixture& fixture) {
  fixture.GetEventHandler().ProcessWaitingRoomEventBus();
}
```

**Checklist:**
- [ ] Implement all three functions
- [ ] Handle event packet creation
- [ ] Inject into waiting room
- [ ] Document when to call ProcessWaitingRoomEventBus
- [ ] Add to CMakeLists.txt

#### Create `tests/unit/harness/event_simulation.test.cpp`

**Test Cases:**
```cpp
TEST_CASE("execute_event_test_data injects event", "[unit][event_simulation]") {
  // Create EventTestData
  // Execute
  // Verify event in waiting room
}

TEST_CASE("events processed after ProcessWaitingRoomEventBus", "[unit][event_simulation]") {
  // Inject events
  // Process waiting room
  // Verify events in global event bus
}
```

**Checklist:**
- [ ] Test event injection
- [ ] Test waiting room processing
- [ ] Test tick filtering
- [ ] Test multiple events
- [ ] Add to CMakeLists.txt

### 3.3 Build and Test

**Checklist:**
- [ ] All 4 files compile
- [ ] All tests pass
- [ ] Integration tests verify input/event coordination

### Phase 3 Deliverables

- [ ] `input_simulation.h/cpp`
- [ ] `input_simulation.test.cpp`
- [ ] `event_simulation.h/cpp`
- [ ] `event_simulation.test.cpp`
- [ ] Updated CMakeLists.txt files

### Phase 3 Acceptance Criteria

✅ Can inject keyboard/mouse input per tick  
✅ Can inject engine events per tick  
✅ Waiting room event bus processing works  
✅ Tick coordination verified  
✅ All tests passing

---

## Phase 4: Orchestration Layer

**Duration:** 2-3 days  
**Files:** 3 files (2 implementation + 1 test)  
**Dependencies:** All previous phases, TestEngine ✅, FlatbuffersTestDataProvider ✅

### Objective
Create the main API that ties everything together: `RunTestEngineTest()` and `run_fixture_test()`.

### Files to Create

#### 4.1 Create `tests/harness/test_harness.h`

**Key Functions:**
```cpp
namespace steamrot::tests {

// Main API - Run TestEngine with TestData and compare results
std::expected<std::monostate, FailInfo>
RunTestEngineTest(const TestData& test_data);

// Alternative fixture-based approach
std::expected<std::monostate, FailInfo>
run_fixture_test(const TestData& test_data);

} // namespace steamrot::tests
```

**Checklist:**
- [ ] Create header with function declarations
- [ ] Add comprehensive Doxygen documentation
- [ ] Explain workflow for each function

#### 4.2 Create `tests/harness/test_harness.cpp`

**RunTestEngineTest Implementation:**

```cpp
std::expected<std::monostate, FailInfo>
RunTestEngineTest(const TestData& test_data) {
  
  // 1. Create TestEngine with TestData
  TestEngine engine(test_data);
  
  // 2. Run the engine for specified ticks
  auto run_result = engine.RunGame();
  if (!run_result.has_value()) {
    return std::unexpected(run_result.error());
  }
  
  // 3. Get data bank (map of tick -> EngineSnapshot)
  const auto& data_bank = engine.GetDataBank();
  
  // 4. For each expected snapshot, compare with data bank
  for (const auto& [tick, expected_snapshot] : test_data.expected_engine_snapshots) {
    
    // Verify tick exists in data bank
    auto it = data_bank.find(tick);
    if (it == data_bank.end()) {
      return std::unexpected(FailInfo{
        FailMode::DataNotFound,
        std::format("Tick {} not found in data bank", tick)
      });
    }
    
    const auto& actual_snapshot = it->second;
    
    // Compare EntityMemoryPools for each scene
    for (size_t scene_idx = 0; scene_idx < actual_snapshot.scene_collection_data.size(); ++scene_idx) {
      
      // Extract EntityMemoryPool from actual
      const auto& actual_pool = /* extract from actual_snapshot */;
      const auto& expected_pool = /* extract from expected_snapshot */;
      
      // Use comparison function from Phase 1
      TestContext context{
        test_data.meta_data.test_name,
        test_data.meta_data.test_description,
        static_cast<uint32_t>(tick),
        static_cast<uint32_t>(test_data.number_of_ticks)
      };
      
      run_entity_memory_pool_comparison_test(actual_pool, expected_pool, context);
    }
    
    // Compare EventBus if present
    if (expected_snapshot.has_event_bus) {
      run_event_bus_comparison_test(
        actual_snapshot.global_event_bus,
        expected_snapshot.global_event_bus,
        test_data.meta_data.test_name
      );
    }
  }
  
  return std::monostate{};
}
```

**run_fixture_test Implementation:**

```cpp
std::expected<std::monostate, FailInfo>
run_fixture_test(const TestData& test_data) {
  
  // 1. Create test fixture with game resources
  TestEngineFixture fixture;
  
  // 2. Configure starting entities from test_data.starting_engine_snapshot
  auto config_result = ConfigureFixtureFromSnapshot(
    fixture,
    test_data.starting_engine_snapshot
  );
  if (!config_result.has_value()) {
    return std::unexpected(config_result.error());
  }
  
  // 3. For each tick from 0 to number_of_ticks - 1
  for (size_t tick = 0; tick < test_data.number_of_ticks; ++tick) {
    
    // Execute input sequence for this tick (Phase 3)
    if (test_data.has_input_sequence) {
      auto input_result = execute_input_events_for_tick(
        test_data.input_sequence, tick, fixture
      );
      if (!input_result.has_value()) {
        return std::unexpected(input_result.error());
      }
    }
    
    // Execute event sequence for this tick (Phase 3)
    if (test_data.has_event_sequence) {
      auto event_result = execute_events_for_tick(
        test_data.event_sequence, tick, fixture
      );
      if (!event_result.has_value()) {
        return std::unexpected(event_result.error());
      }
      
      // Process waiting room
      fixture.GetEventHandler().ProcessWaitingRoomEventBus();
    }
    
    // Execute simulation steps for this tick (Phase 2)
    auto sim_result = execute_simulation_for_tick(
      test_data.simulation_data, tick, fixture
    );
    if (!sim_result.has_value()) {
      return std::unexpected(sim_result.error());
    }
    
    // Tick the event bus
    fixture.GetEventHandler().TickEventBus();
  }
  
  // 4. Compare final state with expected
  if (test_data.has_expected_final_state) {
    const auto& actual_pool = fixture.GetEntityMemoryPool();
    const auto& expected_pool = /* extract from test_data */;
    
    run_entity_memory_pool_comparison_test(
      actual_pool,
      expected_pool,
      test_data.meta_data.test_name
    );
  }
  
  return std::monostate{};
}
```

**Checklist:**
- [ ] Implement RunTestEngineTest
- [ ] Implement run_fixture_test
- [ ] Handle all error cases
- [ ] Coordinate all subsystems (comparison, simulation, input, events)
- [ ] Add to CMakeLists.txt

#### 4.3 Create Unit Tests

**Create:** `tests/unit/harness/test_harness.test.cpp`

**Test Strategy:**

```cpp
TEST_CASE("RunTestEngineTest with simple TestData", "[unit][test_harness]") {
  // Create minimal TestData with:
  // - 1 tick
  // - Simple starting snapshot
  // - Simple expected snapshot
  
  steamrot::TestData test_data;
  test_data.meta_data.test_name = "simple_test";
  test_data.number_of_ticks = 1;
  // ... configure snapshots
  
  auto result = steamrot::tests::RunTestEngineTest(test_data);
  REQUIRE(result.has_value());
}

TEST_CASE("RunTestEngineTest with multiple ticks and snapshots", "[unit][test_harness]") {
  // Test with 3 ticks
  // Expected snapshots at tick 1, 2, 3
  // Verify all comparisons happen
}

TEST_CASE("run_fixture_test executes input sequence", "[unit][test_harness]") {
  // TestData with input sequence
  // Verify inputs were executed
}

TEST_CASE("run_fixture_test executes event sequence", "[unit][test_harness]") {
  // TestData with event sequence
  // Verify events were injected and processed
}

TEST_CASE("run_fixture_test executes simulation", "[unit][test_harness]") {
  // TestData with simulation steps
  // Verify simulation was executed
}
```

**Checklist:**
- [ ] Test RunTestEngineTest with 1 tick
- [ ] Test RunTestEngineTest with multiple ticks
- [ ] Test with input sequences
- [ ] Test with event sequences
- [ ] Test with simulation steps
- [ ] Test combined (input + events + simulation)
- [ ] Test error handling
- [ ] Add to CMakeLists.txt

#### 4.4 Build and Test

**Commands:**
```bash
# Full build
cmake --build --preset Debug

# Run all harness tests
ctest --preset Debug -R test_harness --output-on-failure
```

**Checklist:**
- [ ] Compiles without errors
- [ ] All tests pass
- [ ] Integration verified end-to-end

### Phase 4 Deliverables

- [ ] `test_harness.h`
- [ ] `test_harness.cpp`
- [ ] `test_harness.test.cpp`
- [ ] Updated CMakeLists.txt files

### Phase 4 Acceptance Criteria

✅ RunTestEngineTest works with real TestData  
✅ run_fixture_test coordinates all subsystems  
✅ Tick-based execution works correctly  
✅ Comparison happens automatically  
✅ All error paths handled  
✅ All tests passing

---

## Phase 5: Integration & Documentation

**Duration:** 1-2 days  
**Files:** Example test data, integration tests, documentation updates

### Objective
Verify end-to-end workflow, create examples, update documentation.

### 5.1 Create Example Test Data

#### Create Example JSON Files

**Location:** `tests/unit/harness/data/examples/`

**File:** `simple_ui_test.test_data.json`
```json
{
  "meta_data": {
    "test_name": "simple_ui_interaction",
    "test_description": "Test UI button click updates state",
    "tags": ["unit", "ui"],
    "will_pass": true,
    "version": 1
  },
  "num_ticks": 2,
  "starting_engine_snapshot": {
    "tick_number": 0,
    "scene_collection_data": {
      "scene_data": [{
        "scene_info": {"scene_type": "TITLE"},
        "entity_collection": {
          "entity_memory_pool_size": 10,
          "entities": [{
            "index": 0,
            "c_user_interface": {
              "ui_name": "test_button",
              "is_visible": true,
              "root_ui_element": {
                "base_data": {
                  "position": {"x": 100, "y": 200},
                  "size": {"x": 50, "y": 30}
                }
              }
            }
          }]
        }
      }]
    }
  }
}
```

**Checklist:**
- [ ] Create 3-5 example test data files
- [ ] Cover different scenarios: UI, events, simulation
- [ ] Ensure examples build to .bin files
- [ ] Document each example's purpose

### 5.2 Create Integration Test

**Create:** `tests/integration/test_engine/full_workflow.test.cpp`

```cpp
#include "test_harness.h"
#include "FlatbuffersTestDataProvider.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("Full TestEngine workflow with real test data", "[integration][test_engine]") {
  // Load real test data
  FlatbuffersTestDataProvider provider(
    std::filesystem::path(__FILE__).parent_path() / "data"
  );
  
  auto test_data_vec = provider.ProviderAllTestData();
  REQUIRE(test_data_vec.has_value());
  
  // Run test for each TestData
  const auto& test_data = GENERATE_COPY(from_range(test_data_vec.value()));
  
  INFO("Running test: " << test_data.meta_data.test_name);
  
  auto result = steamrot::tests::RunTestEngineTest(test_data);
  REQUIRE(result.has_value());
}
```

**Checklist:**
- [ ] Create integration test directory structure
- [ ] Add test data files for integration test
- [ ] Create CMakeLists.txt for integration tests
- [ ] Verify tests run in CI/CD

### 5.3 Update Documentation

#### Update tests/harness/README.md

**Sections to Update:**
- [ ] Add "Getting Started" section with simple example
- [ ] Update "Primary Workflow" with actual code
- [ ] Add "Examples" section linking to example test data
- [ ] Update implementation status (all phases complete)
- [ ] Add troubleshooting section

#### Update Main Analysis Documents

**TESTENGINE_IMPLEMENTATION_ANALYSIS.md:**
- [ ] Mark all phases as complete ✅
- [ ] Add "Implementation Complete" section
- [ ] Document any deviations from plan
- [ ] Add "Lessons Learned"

**TESTENGINE_QUICK_REFERENCE.md:**
- [ ] Update status to "Complete"
- [ ] Add "Quick Start" examples
- [ ] Update file counts

### 5.4 Performance and Cleanup

**Tasks:**
- [ ] Run full test suite: `ctest --preset Debug`
- [ ] Check for memory leaks: `valgrind --tool=memcheck`
- [ ] Run static analysis if available
- [ ] Remove any temporary/debug code
- [ ] Ensure all files have proper headers and documentation

### Phase 5 Deliverables

- [ ] 3-5 example test data files
- [ ] Integration test with real test data
- [ ] Updated tests/harness/README.md
- [ ] Updated analysis documents
- [ ] All tests passing
- [ ] Clean code (no TODOs, debug prints)

### Phase 5 Acceptance Criteria

✅ End-to-end workflow verified with real test data  
✅ Examples demonstrate all features  
✅ Documentation complete and accurate  
✅ Full test suite passes  
✅ Code quality checks pass  
✅ Ready for production use

---

## Testing Strategy

### Unit Testing

**Coverage Goal:** >90% for all new code

**Approach:**
- Test each function in isolation
- Mock dependencies where needed
- Use TestScenarios for entity pool setup
- Verify error paths

**Tools:**
- Catch2 for test framework
- TestScenarios for test data setup
- Custom matchers for assertions

### Integration Testing

**Coverage:** End-to-end workflows

**Approach:**
- Use real TestData configurations
- Verify all subsystems work together
- Test with Catch2 generators
- Use HarnessReporter for output

### Regression Testing

**Ensure:**
- Existing harness tests still pass
- TestEngine tests unaffected
- Matcher tests unaffected
- No performance degradation

---

## Risk Mitigation

### Risk 1: FlatBuffers Schema Mismatch

**Risk:** Input/Event schemas don't match expected structure

**Mitigation:**
- Validate schemas BEFORE Phase 3
- If mismatch found, decide: update schema or adapt implementation
- Document decision and rationale

**Contingency:**
- If schema update needed, add 1-2 days to timeline
- If adaptation needed, may require refactoring in Phase 3

### Risk 2: TestEngineFixture Complexity

**Risk:** TestEngineFixture class becomes complex and hard to test

**Mitigation:**
- Keep fixture minimal - only essential state
- Use composition over inheritance
- Write unit tests for fixture itself
- Document fixture's responsibilities clearly

**Contingency:**
- If fixture too complex, consider breaking into smaller helpers
- Add 0.5-1 day for refactoring if needed

### Risk 3: Integration Issues

**Risk:** Phases work individually but fail when integrated

**Mitigation:**
- Write integration tests after each phase
- Test combinations early (e.g., simulation + input)
- Regular code reviews
- Incremental integration

**Contingency:**
- Set aside 1 day buffer in Phase 5 for integration fixes

### Risk 4: Performance Issues

**Risk:** TestEngine too slow for large test suites

**Mitigation:**
- Profile performance during Phase 4
- Optimize hot paths
- Consider parallel test execution

**Contingency:**
- If performance unacceptable, add 1-2 days for optimization

---

## Success Criteria

### Functional Success

✅ **Developers can write data-driven tests:**
```cpp
TEST_CASE("My feature", "[unit]") {
  FlatbuffersTestDataProvider provider(std::filesystem::path(__FILE__).parent_path());
  auto test_data_vec = provider.ProviderAllTestData().value();
  const auto& test_data = GENERATE_COPY(from_range(test_data_vec));
  
  auto result = steamrot::tests::RunTestEngineTest(test_data);
  REQUIRE(result.has_value());
}
```

✅ **Test data can define:**
- Starting entity states
- Expected states at specific ticks
- Input sequences (keyboard/mouse per tick)
- Event sequences (engine events per tick)
- Simulation steps (Logic classes or functions per tick)

✅ **The harness automatically:**
- Loads test data from JSON files
- Creates and runs TestEngine
- Executes inputs/events/simulation per tick
- Captures snapshots at each tick
- Compares with expected states
- Produces detailed failure messages

### Quality Success

✅ **Code Quality:**
- All new code follows project style guide
- >90% unit test coverage
- All functions documented with Doxygen
- No compiler warnings
- Static analysis passes

✅ **Performance:**
- Tests execute in reasonable time (<1s per test typically)
- No memory leaks
- Efficient use of resources

✅ **Documentation:**
- README.md updated with examples
- Analysis documents reflect final state
- Examples demonstrate all features
- Troubleshooting guide available

### Project Success

✅ **Timeline:** Completed in 8-14 days  
✅ **Integration:** Works with existing codebase  
✅ **Adoption:** Other developers can use immediately  
✅ **Maintainability:** Code is clear and well-documented

---

## Appendix

### A. Quick Reference Commands

**Build:**
```bash
cmake --preset Debug
cmake --build --preset Debug
```

**Test:**
```bash
ctest --preset Debug --output-on-failure
ctest --preset Debug -R test_harness
ctest --preset Debug -R test_harness -V  # Verbose
```

**Clean:**
```bash
cmake --build --preset Debug --target clean
rm -rf build/
```

### B. File Checklist

**Phase 1:**
- [ ] `tests/harness/test_data_comparison.h`
- [ ] `tests/harness/test_data_comparison.cpp`
- [ ] `tests/unit/harness/test_data_comparison.test.cpp`

**Phase 2:**
- [ ] `tests/harness/simulation_runner.h`
- [ ] `tests/harness/simulation_runner.cpp`
- [ ] `tests/unit/harness/simulation_runner.test.cpp`

**Phase 3:**
- [ ] `tests/harness/input_simulation.h`
- [ ] `tests/harness/input_simulation.cpp`
- [ ] `tests/unit/harness/input_simulation.test.cpp`
- [ ] `tests/harness/event_simulation.h`
- [ ] `tests/harness/event_simulation.cpp`
- [ ] `tests/unit/harness/event_simulation.test.cpp`

**Phase 4:**
- [ ] `tests/harness/test_harness.h`
- [ ] `tests/harness/test_harness.cpp`
- [ ] `tests/unit/harness/test_harness.test.cpp`

**Phase 5:**
- [ ] Example test data files (3-5)
- [ ] Integration test
- [ ] Updated documentation

### C. Contact and Support

**Questions:**
- Review TESTENGINE_IMPLEMENTATION_ANALYSIS.md for detailed design decisions
- Check TESTENGINE_ARCHITECTURE.md for diagrams
- See TESTENGINE_QUICK_REFERENCE.md for quick lookup

**Issues:**
- Check if schemas need updating (common issue in Phase 3)
- Verify matcher library is linked (common issue in Phase 1)
- Ensure __FILE__ macro usage for data loading (common issue in tests)

---

**END OF IMPLEMENTATION PLAN**
