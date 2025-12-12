# Test Harness Organization - Quick Reference

[← Back to Full Analysis](TEST_HARNESS_ORGANIZATION_ANALYSIS.md) | [Testing Overview](../testing/TESTING_OVERVIEW.md)

## TL;DR

**Problem**: Test harness has direct FlatBuffers dependencies everywhere, violating repository's abstraction principles.

**Solution**: Create three-layer architecture mirroring main codebase:
1. **Layer 1**: Test logic (pure C++ structs, NO FlatBuffers)
2. **Layer 2**: Provider interfaces (abstract data loading)
3. **Layer 3**: FlatBuffers implementations (.cpp files only)

**Critical Requirement**: TestConfig must align with SaveData structure. TestEngine loads SceneData collections exactly like Engine/GameEngine. Only differences: simulation data (test execution) and data extraction (validation snapshots).

---

## Current vs. Proposed Architecture

### Current (❌ Problematic)

```cpp
// test_harness.h
#include "test_data_generated.h"  // ❌ FlatBuffers in header

std::expected<std::vector<const TestDataConfig*>, FailInfo>
load_test_data_configs();  // ❌ Returns FlatBuffers pointers

// TestEngine.h
#include "test_data_generated.h"  // ❌ FlatBuffers in header

class TestEngine {
  TestEngine(const TestDataConfig* config);  // ❌ Takes FlatBuffers pointer
};
```

### Proposed (✅ Clean)

```cpp
// test_harness.h
#include "TestConfig.h"  // ✅ Native C++ struct

std::expected<std::vector<TestConfig>, FailInfo>
load_test_data_configs();  // ✅ Returns native structs

// TestEngine.h
#include "TestConfig.h"  // ✅ Native C++ struct

class TestEngine {
  TestEngine(const TestConfig& config);  // ✅ Takes native struct reference
};
```

---

## Three-Layer Architecture

```
╔═══════════════════════════════════════════════════════════╗
║ LAYER 1: Test Orchestration & Logic                      ║
║ ─────────────────────────────────────────────────────     ║
║ • test_harness.h/cpp                                      ║
║ • TestEngine.h/cpp                                        ║
║ • simulation_runner.h/cpp                                 ║
║ • input_simulation.h/cpp                                  ║
║ • event_simulation.h/cpp                                  ║
║                                                            ║
║ Uses: Native C++ structs (TestConfig, SimulationConfig)  ║
║ NO FlatBuffers includes                                   ║
╚═════════════════════╤═════════════════════════════════════╝
                      │ LoadData()
╔═════════════════════╧═════════════════════════════════════╗
║ LAYER 2: Provider Interfaces                             ║
║ ─────────────────────────────────────────────────────     ║
║ • ITestDataProvider.h                                     ║
║ • ISimulationDataProvider.h                               ║
║ • IInputSequenceProvider.h                                ║
║                                                            ║
║ Returns: Native C++ structs                               ║
║ NO FlatBuffers includes                                   ║
╚═════════════════════╤═════════════════════════════════════╝
                      │ Implementation
╔═════════════════════╧═════════════════════════════════════╗
║ LAYER 3: FlatBuffers Implementations                     ║
║ ─────────────────────────────────────────────────────     ║
║ • FlatbuffersTestDataProvider.cpp                         ║
║ • FlatbuffersSimulationDataProvider.cpp                   ║
║ • FlatbuffersInputSequenceProvider.cpp                    ║
║                                                            ║
║ FlatBuffers ONLY in .cpp files                            ║
║ Converts: FlatBuffers → Native structs                    ║
╚═══════════════════════════════════════════════════════════╝
```

---

## Component Sorting

### By Layer

| Layer | Component | Purpose | FlatBuffers? |
|-------|-----------|---------|--------------|
| **1** | test_harness.h/cpp | Catch2 integration | ❌ Remove |
| **1** | TestEngine.h/cpp | Test execution | ❌ Remove |
| **1** | simulation_runner.h/cpp | Logic execution | ❌ Remove |
| **1** | input_simulation.h/cpp | Input simulation | ❌ Remove |
| **1** | event_simulation.h/cpp | Event simulation | ❌ Remove |
| **1** | test_data_comparison.h/cpp | Data validation | ❌ Remove |
| **1** | SceneSnapshot.h | State capture | ✅ Already clean |
| **2** | ITestDataProvider.h | Abstract loading | ❌ None |
| **2** | ISimulationDataProvider.h | Abstract simulation | ❌ None |
| **2** | IInputSequenceProvider.h | Abstract input | ❌ None |
| **2** | IEventSequenceProvider.h | Abstract events | ❌ None |
| **3** | FlatbuffersTestDataProvider.cpp | Concrete loading | ✅ .cpp only |
| **3** | FlatbuffersSimulationDataProvider.cpp | Concrete simulation | ✅ .cpp only |
| **3** | FlatbuffersInputSequenceProvider.cpp | Concrete input | ✅ .cpp only |
| **3** | FlatbuffersEventSequenceProvider.cpp | Concrete events | ✅ .cpp only |

### By Responsibility

| Responsibility | Current Files | New Organization |
|----------------|---------------|------------------|
| **Data Loading** | test_data_loader.h/cpp | ITestDataProvider + FlatbuffersTestDataProvider |
| **Test Execution** | test_harness.h/cpp, TestEngine.h/cpp | orchestration/ directory |
| **Simulation** | simulation_runner.h/cpp | simulation/ directory |
| **Input/Events** | input_simulation.h/cpp, event_simulation.h/cpp | simulation/ directory |
| **Comparison** | test_data_comparison.h/cpp | orchestration/ directory |
| **Data Structures** | (scattered) | data_structures/ directory |
| **Providers** | (none) | providers/ + flatbuffers_providers/ |

---

## Directory Reorganization

### Current Structure

```
tests/harness/
├── test_harness.h/cpp           # Mixed responsibilities
├── test_data_loader.h/cpp       # FlatBuffers exposed
├── test_data_comparison.h/cpp   # FlatBuffers exposed
├── TestEngine.h/cpp             # FlatBuffers exposed
├── simulation_runner.h/cpp      # FlatBuffers exposed
├── input_simulation.h/cpp       # FlatBuffers exposed
├── event_simulation.h/cpp       # FlatBuffers exposed
└── SceneSnapshot.h              # ✓ Clean
```

### Proposed Structure

```
tests/harness/
├── orchestration/               # Layer 1: Core logic
│   ├── test_harness.h/cpp
│   ├── TestEngine.h/cpp
│   └── test_data_comparison.h/cpp
│
├── simulation/                  # Layer 1: Simulation
│   ├── simulation_runner.h/cpp
│   ├── input_simulation.h/cpp
│   └── event_simulation.h/cpp
│
├── data_structures/             # Layer 1: Native structs
│   ├── TestConfig.h
│   ├── SimulationConfig.h
│   ├── InputSequence.h
│   ├── EventSequence.h
│   ├── SnapshotConfig.h
│   └── SceneSnapshot.h
│
├── providers/                   # Layer 2: Interfaces
│   ├── ITestDataProvider.h
│   ├── ISimulationDataProvider.h
│   ├── IInputSequenceProvider.h
│   └── IEventSequenceProvider.h
│
└── flatbuffers_providers/       # Layer 3: Implementations
    ├── FlatbuffersTestDataProvider.cpp
    ├── FlatbuffersSimulationDataProvider.cpp
    ├── FlatbuffersInputSequenceProvider.cpp
    └── FlatbuffersEventSequenceProvider.cpp
```

---

## Key Native Structs to Create

### TestConfig.h (Aligns with SaveData)

```cpp
// TestConfig mirrors SaveData structure
// TestEngine loads SceneData like GameEngine
struct TestConfig {
  TestMetadata metadata;  // Similar to SaveData::Metadata
  
  // SAME AS PRODUCTION: Scene data collection
  std::vector<SceneData> scenes;  // TestEngine loads like GameEngine
  
  // TEST-SPECIFIC (only differences):
  std::optional<SimulationConfig> simulation_data;  // How to execute
  std::optional<InputSequence> input_sequence;      // Input simulation
  std::optional<EventSequence> event_sequence;      // Event injection
  
  // TEST VALIDATION (extraction at end):
  uint32_t num_ticks{1};
  std::vector<SnapshotConfig> tick_snapshots;  // Expected states
};
```

**Key Architectural Requirement**: TestConfig must align with SaveData. The only differences are simulation data (how to run test) and data extraction (validation). Entity data lives in SceneData (same as production).

### SimulationConfig.h

```cpp
struct SimulationStep {
  SimulationType simulation_type;
  ExecutionMode execution_mode;
  std::optional<FunctionType> function_type;
  std::optional<LogicClassType> logic_class_type;
  std::string description;
};

struct SimulationConfig {
  std::vector<SimulationStep> steps;
  std::string description;
};
```

### InputSequence.h

```cpp
struct InputEvent {
  InputType input_type;
  InputData input_data;  // variant
  uint32_t tick;
  std::string description;
};

struct InputSequence {
  std::vector<InputEvent> inputs;
  std::string description;
};
```

### EventSequence.h

```cpp
struct EventTestData {
  uint32_t tick;
  EventPacket event_packet;
  std::string description;
};

struct EventSequence {
  std::vector<EventTestData> events;
  std::string description;
};
```

---

## Migration Checklist

### Phase 1: Data Structures ✅

- [ ] Create `tests/harness/data_structures/` directory
- [ ] Create `TestConfig.h`
- [ ] Create `SimulationConfig.h`
- [ ] Create `InputSequence.h`
- [ ] Create `EventSequence.h`
- [ ] Create `SnapshotConfig.h`

### Phase 2: Provider Interfaces ✅

- [ ] Create `tests/harness/providers/` directory
- [ ] Create `ITestDataProvider.h`
- [ ] Create `ISimulationDataProvider.h`
- [ ] Create `IInputSequenceProvider.h`
- [ ] Create `IEventSequenceProvider.h`

### Phase 3: FlatBuffers Implementations ✅

- [ ] Create `tests/harness/flatbuffers_providers/` directory
- [ ] Implement `FlatbuffersTestDataProvider.cpp`
- [ ] Implement `FlatbuffersSimulationDataProvider.cpp`
- [ ] Implement `FlatbuffersInputSequenceProvider.cpp`
- [ ] Implement `FlatbuffersEventSequenceProvider.cpp`
- [ ] Write unit tests for providers

### Phase 4: Refactor Components ✅

- [ ] Update `test_harness.h/cpp` (remove FlatBuffers, use TestConfig)
- [ ] Update `TestEngine.h/cpp` (remove FlatBuffers, use TestConfig)
- [ ] Update `simulation_runner.h/cpp` (use SimulationConfig)
- [ ] Update `input_simulation.h/cpp` (use InputSequence)
- [ ] Update `event_simulation.h/cpp` (use EventSequence)
- [ ] Update `test_data_comparison.h/cpp` (use SnapshotConfig)

### Phase 5: Reorganize Directories ✅

- [ ] Move files to new directory structure
- [ ] Update CMakeLists.txt
- [ ] Update include paths
- [ ] Verify builds

### Phase 6: Update Documentation ✅

- [ ] Update `TEST_DATA_CONFIGURATION.md`
- [ ] Update `TESTING_OVERVIEW.md`
- [ ] Create `TEST_HARNESS_ARCHITECTURE.md`
- [ ] Update `FILLING_TEST_DATA.md`

---

## Benefits Summary

| Benefit | Impact |
|---------|--------|
| **Architectural Consistency** | Test harness matches main codebase |
| **Reduced Coupling** | No FlatBuffers in test logic headers |
| **Improved Build Times** | Fewer recompilations on schema changes |
| **Better Testing** | Unit test providers independently |
| **Maintainability** | Clear responsibilities per layer |
| **Future-Proofing** | Easy to swap serialization formats |

---

## API Changes

### Before (Current)

```cpp
// Returns FlatBuffers pointers
auto configs = steamrot::tests::load_test_data_configs();
const TestDataConfig* config = configs.value()[0];  // FlatBuffers type

// Takes FlatBuffers pointer
steamrot::tests::TestEngine engine(config);
```

### After (Proposed)

```cpp
// Returns native structs
auto configs = steamrot::tests::load_test_data_configs();
const TestConfig& config = configs.value()[0];  // Native struct

// Takes native struct reference
steamrot::tests::TestEngine engine(config);
```

---

## Common Patterns

### Data Loading Pattern

```cpp
// Provider returns native struct, NO FlatBuffers exposure
ITestDataProvider& provider = GetTestDataProvider();
auto result = provider.LoadTestConfig("my_test");
if (result.has_value()) {
  const TestConfig& config = result.value();  // Native struct
  // Use config...
}
```

### Conversion Pattern (Inside Provider)

```cpp
// FlatbuffersTestDataProvider.cpp (ONLY in .cpp file)
#include "test_data_generated.h"  // OK in .cpp

TestConfig FlatbuffersTestDataProvider::ConvertToNative(
    const TestDataConfig* fb_config) const {
  
  TestConfig config;
  
  // Convert FlatBuffers → Native
  if (fb_config->metadata()) {
    config.metadata.test_name = fb_config->metadata()->test_name()->str();
    // ...
  }
  
  return config;
}
```

---

## References

- [Full Analysis](TEST_HARNESS_ORGANIZATION_ANALYSIS.md) - Complete architectural analysis
- [Data Provider System](../DATA_PROVIDER_SYSTEM.md) - Main codebase provider pattern
- [Testing Overview](../testing/TESTING_OVERVIEW.md) - General testing information
- [Test Data Configuration](../testing/TEST_DATA_CONFIGURATION.md) - Test data format reference
