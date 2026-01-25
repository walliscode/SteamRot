# TestEngine Architecture Diagram

## High-Level Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                         TEST FILE (User Code)                        │
│                                                                       │
│  TEST_CASE("My test", "[unit]") {                                   │
│    auto configs = load_test_data_configs();                         │
│    const auto *config = GENERATE_COPY(from_range(configs.value()));│
│    auto result = RunTestEngineTest(config);                         │
│    REQUIRE(result.has_value());                                     │
│  }                                                                   │
└────────────────────────┬──────────────────────────────────────────┘
                         │
                         │ Uses
                         ▼
┌─────────────────────────────────────────────────────────────────────┐
│                    TEST HARNESS (Orchestration)                      │
│  ┌────────────────────────────────────────────────────────────┐    │
│  │  test_harness.h/cpp                              ❌ MISSING│    │
│  │  - RunTestEngineTest(config)                              │    │
│  │  - run_fixture_test(config)                               │    │
│  │  - Coordinates all subsystems                             │    │
│  └────────────────────────────────────────────────────────────┘    │
└─────┬────────┬──────────┬──────────┬──────────┬────────────────────┘
      │        │          │          │          │
      ▼        ▼          ▼          ▼          ▼
┌─────────┐┌────────┐┌────────┐┌─────────┐┌─────────┐
│ Loader  ││Compare ││ Sim    ││ Input   ││ Event   │
│❌ MISS ││❌ MISS ││❌ MISS ││❌ MISS  ││❌ MISS  │
└─────────┘└────────┘└────────┘└─────────┘└─────────┘
```

## Detailed Component Architecture

```
┌───────────────────────────────────────────────────────────────────────┐
│                          ORCHESTRATION LAYER                           │
│                        (test_harness - MISSING)                        │
├───────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  RunTestEngineTest(config) workflow:                                   │
│  1. Load TestDataFbs → TestData                                        │
│  2. Create TestEngine(TestData)                                        │
│  3. TestEngine.RunGame()                                               │
│  4. Get data bank snapshots                                            │
│  5. For each tick snapshot:                                            │
│     → Compare pools (uses Matchers)                                    │
│     → Compare event buses (uses Matchers)                              │
│  6. Return success/failure                                             │
│                                                                         │
└─┬─────┬─────┬────────┬──────────┬────────────┬─────────────────────┬─┘
  │     │     │        │          │            │                     │
  │     │     │        │          │            │                     │
  ▼     ▼     ▼        ▼          ▼            ▼                     ▼
┌──────────────┐  ┌──────────────────┐  ┌────────────────┐  ┌───────────┐
│ Data Loading │  │   Comparison     │  │   Simulation   │  │ Test      │
│   MISSING    │  │    MISSING       │  │    MISSING     │  │ Engine    │
├──────────────┤  ├──────────────────┤  ├────────────────┤  │ ✅ EXISTS │
│              │  │                  │  │                │  ├───────────┤
│ load_test_   │  │ run_entity_pool_ │  │ simulation_    │  │TestEngine │
│ data_configs │  │ comparison_test  │  │ runner         │  │::RunGame()│
│              │  │                  │  │                │  │           │
│ Uses:        │  │ run_event_bus_   │  │ input_         │  │Captures:  │
│ - Flatbuffers│  │ comparison_test  │  │ simulation     │  │- Snapshots│
│   TestData   │  │                  │  │                │  │- DataBank │
│   Loader ✅  │  │ Uses:            │  │ event_         │  │           │
│ - Flatbuffers│  │ - Matchers ✅    │  │ simulation     │  │Overrides: │
│   TestData   │  │   (21 ready)     │  │                │  │- Tick     │
│   Provider ✅│  │                  │  │                │  │  methods  │
└──────────────┘  └──────────────────┘  └────────────────┘  └───────────┘
```

## Data Flow

```
Test Data Files (.json)            Test Code
      │                                 │
      │ Compiled to                    │
      ▼                                 │
.test_data.bin                         │
      │                                 │
      │                                 │ Calls load_test_data_configs()
      │◄────────────────────────────────┤
      │                                 │
      │ Loaded by                       │
      ▼                                 │
FlatbuffersTestDataLoader ✅           │
      │                                 │
      │ Provides TestDataFbs*           │
      ▼                                 │
FlatbuffersTestDataProvider ✅         │
      │                                 │
      │ Converts to TestData            │
      ▼                                 │
TestData struct ✅                     │
      │                                 │
      │                                 │ Passes to RunTestEngineTest()
      │◄────────────────────────────────┤
      │                                 │
      │ Used by                         │
      ▼                                 │
TestEngine ✅                          │
      │                                 │
      │ Executes ticks                  │
      │ Captures snapshots              │
      ▼                                 │
Data Bank (map<tick, snapshot>) ✅    │
      │                                 │
      │                                 │ Compared by test_harness
      │◄────────────────────────────────┤
      │                                 │
      │ Compared using                  │
      ▼                                 │
Matchers ✅                            │
      │                                 │
      │ Results reported via            │
      ▼                                 │
HarnessReporter ✅                     │
      │                                 │
      │ Shown to user                   │
      ▼                                 │
   Console                              │
```

## Matcher Hierarchy

```
┌────────────────────────────────────────────────────────────────┐
│                     MATCHERS (All ✅ Ready)                     │
├────────────────────────────────────────────────────────────────┤
│                                                                  │
│  EntityMemoryPoolEqualsMatcher ⭐ PRIMARY MATCHER              │
│  ├── Uses ComponentMatcherBase<T> for each component           │
│  ├── CMetaEqualsMatcher                                         │
│  ├── CUserInterfaceEqualsMatcher                                │
│  ├── CGrimoireMachinaEqualsMatcher                              │
│  ├── CMachinaFormEqualsMatcher                                  │
│  └── CUIStateEqualsMatcher                                      │
│                                                                  │
│  EventBusEqualsMatcher ⭐ EVENT VALIDATION                     │
│  ├── Uses EventPacketEqualsMatcher                              │
│  └── Uses EventDataEqualsMatcher                                │
│                                                                  │
│  Supporting Matchers:                                           │
│  ├── UIElementEqualsMatcher (UI tree comparison)                │
│  ├── FragmentEqualsMatcher                                      │
│  ├── JointEqualsMatcher                                         │
│  └── SubscriberEqualsMatcher                                    │
│                                                                  │
│  Helper Infrastructure:                                         │
│  ├── ComponentMatcherBase<T> (template base)                    │
│  ├── matcher_helpers.h (utilities)                              │
│  └── test_context.h (metadata for output)                       │
│                                                                  │
└────────────────────────────────────────────────────────────────┘
```

## Implementation Dependencies

```
Phase 1: test_data_loader
         │
         │ Depends on:
         ├── FlatbuffersTestDataLoader ✅
         └── FlatbuffersTestDataProvider ✅
         │
         ▼
Phase 2: test_data_comparison
         │
         │ Depends on:
         ├── EntityMemoryPoolEqualsMatcher ✅
         └── EventBusEqualsMatcher ✅
         │
         ▼
Phase 3: simulation_runner
         │
         │ Depends on:
         ├── SimulationData ✅
         └── Logic system ✅
         │
         ▼
Phase 4: input_simulation + event_simulation
         │
         │ Depends on:
         ├── Input/Event schemas ⚠️ (verify)
         └── EventHandler ✅
         │
         ▼
Phase 5: test_harness
         │
         │ Depends on:
         ├── All above phases
         ├── TestEngine ✅
         └── Catch2 generators ✅
         │
         ▼
Phase 6: Integration & Testing
         │
         │ Verifies:
         └── End-to-end workflow
```

## Status Legend

- ✅ **Exists and works** - Implemented, tested, ready to use
- ❌ **Missing** - Needs to be implemented
- ⚠️ **Verify** - Exists but needs validation
- ⭐ **Critical** - Key component for functionality

## File Count Summary

```
┌─────────────────────────┬───────┬────────┬─────────┐
│ Component               │ Exist │ Missing│ Total   │
├─────────────────────────┼───────┼────────┼─────────┤
│ TestEngine              │   2   │   0    │   2 ✅  │
│ Data Providers          │   4   │   0    │   4 ✅  │
│ Test Harness            │   0   │   2    │   2 ❌  │
│ Data Loader             │   0   │   2    │   2 ❌  │
│ Comparison              │   0   │   2    │   2 ❌  │
│ Simulation              │   0   │   2    │   2 ❌  │
│ Input Simulation        │   0   │   2    │   2 ❌  │
│ Event Simulation        │   0   │   2    │   2 ❌  │
│ Matchers                │  42   │   0    │  42 ✅  │
│ Reporter                │   3   │   0    │   3 ✅  │
├─────────────────────────┼───────┼────────┼─────────┤
│ TOTAL (impl files)      │  51   │  12    │  63     │
└─────────────────────────┴───────┴────────┴─────────┘

Implementation files: 12 needed
Test files: 6 needed (for the 12 impl files)
Total files to create: 18
```

## Critical Path

```
START
  │
  ├─► Verify schemas exist (input/event) ⚠️
  │
  ├─► Implement test_data_loader (Phase 1)
  │   └─► Test with existing providers ✅
  │
  ├─► Implement test_data_comparison (Phase 2)
  │   └─► Test with existing matchers ✅
  │
  ├─► Implement simulation/input/event (Phases 3-4)
  │   └─► Test with TestEngine ✅
  │
  └─► Implement test_harness (Phase 5)
      └─► Tie everything together
          │
          ├─► Create example test data
          ├─► Write integration tests
          └─► Update documentation
              │
              ▼
            DONE ✅
```

---

**For detailed analysis:** See TESTENGINE_IMPLEMENTATION_ANALYSIS.md  
**For quick reference:** See TESTENGINE_QUICK_REFERENCE.md
