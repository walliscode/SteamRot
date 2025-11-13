# EventBus Test Data Infrastructure

## Summary

This implementation adds comprehensive support for EventBus state testing in the SteamRot test harness. Tests can now initialize, validate, and compare EventBus state at various points during test execution.

## What Was Added

### 1. FlatBuffers Schema for EventBus State

**File:** `src/flatbuffers_headers/event_bus_data.fbs`

A new FlatBuffers schema that represents a snapshot of EventBus state:
```fbs
table EventBusData {
  events: [EventPacketData];
  description: string;
}
```

This schema can be used to:
- Initialize EventBus state at test start
- Define expected EventBus state at test end
- Validate EventBus state at tick snapshots

### 2. EventBus Conversion Utilities

**Files:**
- `src/events/event_bus_conversion.h`
- `src/events/event_bus_conversion.cpp`

Two key functions for converting between FlatBuffers data and runtime EventBus:

```cpp
// Convert EventBusData to EventBus
std::expected<EventBus, FailInfo>
ConvertEventBusDataToEventBus(const EventBusData *event_bus_data);

// Populate EventHandler's global event bus from EventBusData
std::expected<std::monostate, FailInfo>
ConfigureEventHandlerFromEventBusData(const EventBusData *event_bus_data,
                                      EventHandler &event_handler);
```

### 3. Extended Test Data Schema

**File:** `src/flatbuffers_headers/test_data.fbs`

Added three new optional fields to the test data configuration:

```fbs
table TestDataConfig {
  // ... existing fields ...
  
  // Initialize EventBus at test start
  start_event_bus: EventBusData;
  
  // Expected EventBus state at test end
  expected_event_bus: EventBusData;
  
  // ... other fields ...
}

table TickSnapshot {
  // ... existing fields ...
  
  // Expected EventBus state at this tick
  event_bus: EventBusData;
  
  // ... other fields ...
}
```

### 4. Test Harness Integration

**Files:**
- `tests/harness/test_data_harness.h`
- `tests/harness/test_data_harness.cpp`
- `tests/harness/tick_executor.cpp`

Added EventBus comparison and configuration support:

```cpp
// Compare two EventBus instances
void run_event_bus_comparison_test(const EventBus &actual,
                                   const EventBus &expected,
                                   bool expected_to_pass = true);

// Compare with metadata for better error messages
void run_event_bus_comparison_test(const EventBus &actual,
                                   const EventBus &expected,
                                   const std::string &test_metadata,
                                   bool expected_to_pass = true);
```

**Automatic Integration:**
- `create_fixture_from_test_data()` now configures EventBus from `start_event_bus`
- `run_fixture_test()` now compares `expected_event_bus` automatically
- `compare_tick_snapshot()` now validates EventBus state at tick snapshots

### 5. Unit Tests

**File:** `tests/unit/events/event_bus_conversion.test.cpp`

Comprehensive unit tests covering:
- Null input handling
- Empty EventBusData conversion
- Single event conversion
- Multiple event conversion
- EventHandler configuration from EventBusData

### 6. Sample Test Data

**Files:**
- `tests/harness/data/simple_event_bus_test.test_data.json`
- `tests/harness/data/event_bus_snapshot_test.test_data.json`

Example test data demonstrating:
- Basic EventBus initialization and final state comparison
- Tick-by-tick EventBus state validation
- Event lifetime management testing

### 7. Documentation

**File:** `tests/harness/README.md`

Added comprehensive documentation section covering:
- EventBus testing overview
- Key concepts (EventBusData, start_event_bus, expected_event_bus, tick snapshots)
- Example test data
- API reference for EventBus comparison and configuration
- Benefits and use cases

## Usage Examples

### Basic EventBus Test

```json
{
  "metadata": {
    "test_name": "simple_event_test",
    "description": "Test event lifetime management"
  },
  "start_event_bus": {
    "events": [
      {
        "event_lifetime": 2,
        "event_type": "EVENT_TEST"
      }
    ]
  },
  "num_ticks": 1,
  "expected_event_bus": {
    "events": [
      {
        "event_lifetime": 1,
        "event_type": "EVENT_TEST"
      }
    ]
  }
}
```

### EventBus with Tick Snapshots

```json
{
  "metadata": {
    "test_name": "tick_snapshot_test"
  },
  "start_event_bus": {
    "events": [{"event_lifetime": 3, "event_type": "EVENT_TEST"}]
  },
  "num_ticks": 2,
  "tick_snapshots": [
    {
      "tick": 0,
      "entity_collection": {...},
      "event_bus": {
        "events": [{"event_lifetime": 2, "event_type": "EVENT_TEST"}]
      }
    },
    {
      "tick": 1,
      "entity_collection": {...},
      "event_bus": {
        "events": [{"event_lifetime": 1, "event_type": "EVENT_TEST"}]
      }
    }
  ]
}
```

### C++ Test Usage

```cpp
TEST_CASE("Data-driven EventBus test", "[unit][event_bus]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // EventBus initialization, tick snapshots, and final comparison
  // all happen automatically
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

## Key Features

1. **Data-Driven Testing**: Define EventBus tests in JSON without writing C++ code
2. **Tick-by-Tick Validation**: Verify EventBus state at any tick during execution
3. **Automatic Integration**: Works seamlessly with existing test harness
4. **Event Lifetime Testing**: Validate that event lifetimes are managed correctly
5. **Reuses Existing Infrastructure**: Leverages EventBus matchers and event conversion logic

## Testing Scenarios Enabled

- Event lifetime management (decrement and removal)
- Event propagation through the system
- Event processing by subscribers
- State transitions over multiple ticks
- Regression testing for event system behavior

## Build Integration

All FlatBuffers schemas are automatically compiled during the build process:
- JSON test data files are converted to .bin files
- Generated headers are created for all schemas
- No manual compilation steps required

## Next Steps for Users

1. **Create JSON test data** files in `tests/<module>/data/` directories
2. **Use the test harness** with `load_test_data_configs()` and `run_fixture_test()`
3. **Validate EventBus state** at start, during ticks, and at end
4. **Reference sample files** for examples and patterns

## Files Modified/Created

### Created
- `src/flatbuffers_headers/event_bus_data.fbs`
- `src/events/event_bus_conversion.h`
- `src/events/event_bus_conversion.cpp`
- `tests/unit/events/event_bus_conversion.test.cpp`
- `tests/harness/data/simple_event_bus_test.test_data.json`
- `tests/harness/data/event_bus_snapshot_test.test_data.json`

### Modified
- `src/flatbuffers_headers/generate_flatbuffers_headers.cmake`
- `src/flatbuffers_headers/test_data.fbs`
- `src/events/CMakeLists.txt`
- `tests/unit/events/CMakeLists.txt`
- `tests/harness/test_data_harness.h`
- `tests/harness/test_data_harness.cpp`
- `tests/harness/tick_executor.cpp`
- `tests/harness/README.md`

## Design Decisions

1. **Reuse EventPacketData**: Instead of creating a new schema, reused existing `EventPacketData` schema for consistency
2. **Mirror EntityMemoryPool Pattern**: Followed the same pattern as EntityMemoryPool testing for consistency
3. **Automatic Processing**: EventBus configuration and comparison happen automatically in test harness
4. **Optional Fields**: All EventBus fields are optional to maintain backward compatibility
5. **Per-Tick Validation**: Added EventBus to TickSnapshot for granular validation

## Adherence to Requirements

✅ Created FlatBuffers schema for EventBus data  
✅ Created configurator for converting .bin to in-game EventHandler  
✅ Treated EventBus like EntityMemoryPool in test harness  
✅ Support for testing EventBus state at ticks (snapshots)  
✅ Support for initial and expected EventBus state  
✅ Did not build (as instructed)  
