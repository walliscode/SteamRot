# Test Harness Organization Analysis

[← Back to Documentation](../README.md) | [Testing Overview](../testing/TESTING_OVERVIEW.md)

## Executive Summary

This document analyzes the SteamRot test harness architecture and provides recommendations for organizing and sorting its components to align with the repository's strategic direction of abstracting FlatBuffers and maintaining clean architectural boundaries.

**Key Finding**: The test harness currently has **direct FlatBuffers dependencies** in most components, which conflicts with the main codebase's provider-based abstraction strategy.

**Primary Recommendation**: Create a **three-layer test harness architecture** that mirrors the main codebase structure.

**Critical Architectural Requirement**: TestConfig must align with SaveData structure - TestEngine should load a collection of SceneData (exactly like Engine/GameEngine). The only differences are: simulation data (how to execute the test) and data extraction at the end (validation via snapshots).

---

## Table of Contents

- [Background](#background)
- [Current Architecture](#current-architecture)
- [FlatBuffers Dependency Analysis](#flatbuffers-dependency-analysis)
- [Proposed Reorganization](#proposed-reorganization)
- [Component Classification](#component-classification)
- [Migration Strategy](#migration-strategy)
- [Benefits](#benefits)
- [Implementation Plan](#implementation-plan)

---

## Background

### Repository Context

The SteamRot repository has undergone significant architectural refactoring with the goal of:

1. **Removing FlatBuffers from main code** - FlatBuffers dependencies isolated to provider implementations
2. **Abstract data sources** - Use provider interfaces (IEngineDataProvider, ISceneDataProvider, etc.)
3. **Layer separation** - Clear boundaries between data, logic, and orchestration layers
4. **Build stratification** - Minimize circular dependencies and reduce recompilation

### Current Main Codebase Architecture

```
┌─────────────────────────────────────────────┐
│         Game Code (Engine, Scenes)          │
│  Uses native C++ structs and interfaces     │
│  NO FlatBuffers dependencies                │
└─────────────────┬───────────────────────────┘
                  │
                  ↓ LoadData()
┌─────────────────────────────────────────────┐
│      Provider Interfaces (Abstract)         │
│  - IEngineDataProvider                      │
│  - ISceneDataProvider                       │
│  - IAssetDataProvider                       │
│  NO FlatBuffers in headers                  │
└─────────────────┬───────────────────────────┘
                  │
                  ↓ Implementation
┌─────────────────────────────────────────────┐
│   FlatBuffers Implementations (Concrete)    │
│  - FlatbuffersEngineDataProvider            │
│  - FlatbuffersSceneDataProvider             │
│  FlatBuffers ONLY in .cpp files             │
└─────────────────────────────────────────────┘
```

### Problem Statement

**The test harness does NOT follow this architecture.** FlatBuffers types are exposed throughout the test harness, creating:

- **Tight coupling** to FlatBuffers serialization format
- **Recompilation cascades** when test schemas change
- **Inconsistent architecture** between production and test code
- **Difficult migration** if switching serialization formats
- **Unclear responsibilities** across test components

---

## Current Architecture

### Directory Structure

```
tests/harness/
├── test_harness.h/cpp           # Top-level orchestration
├── test_data_loader.h/cpp       # File loading and parsing
├── test_data_comparison.h/cpp   # Data validation
├── TestEngine.h/cpp             # Test execution engine
├── simulation_runner.h/cpp      # Logic/function execution
├── input_simulation.h/cpp       # Input event simulation
├── event_simulation.h/cpp       # Event injection
├── SceneSnapshot.h              # Scene state capture
└── data/                        # Test data files
```

### Current Component Relationships

```
test_harness.h
    ├─→ test_data_generated.h (DIRECT FlatBuffers)
    ├─→ test_data_loader.h
    ├─→ test_data_comparison.h
    └─→ TestEngine.h
           ├─→ test_data_generated.h (DIRECT FlatBuffers)
           ├─→ simulation_generated.h (DIRECT FlatBuffers)
           └─→ simulation_runner.h
                  ├─→ simulation_generated.h (DIRECT FlatBuffers)
                  └─→ Logic classes

test_data_loader.h
    └─→ test_data_generated.h (DIRECT FlatBuffers)

test_data_comparison.h
    ├─→ test_data_generated.h (DIRECT FlatBuffers)
    └─→ engine_state_generated.h (DIRECT FlatBuffers)

input_simulation.h
    ├─→ input_test_data_generated.h (DIRECT FlatBuffers)
    └─→ flatbuffers/vector.h (DIRECT FlatBuffers)

event_simulation.h
    ├─→ event_packet_data_generated.h (DIRECT FlatBuffers)
    └─→ flatbuffers/vector.h (DIRECT FlatBuffers)
```

---

## FlatBuffers Dependency Analysis

### Files with Direct FlatBuffers Dependencies

| File | FlatBuffers Include | Impact | Reason |
|------|---------------------|--------|--------|
| `test_harness.h` | `test_data_generated.h` | HIGH | Core API exposes FlatBuffers types |
| `test_data_loader.h` | `test_data_generated.h` | HIGH | Returns FlatBuffers pointers directly |
| `test_data_comparison.h` | `test_data_generated.h`, `engine_state_generated.h` | HIGH | Accepts FlatBuffers types for comparison |
| `TestEngine.h` | `test_data_generated.h`, `simulation_generated.h` | HIGH | Constructor takes FlatBuffers config |
| `simulation_runner.h` | `simulation_generated.h` | MEDIUM | Reads simulation steps from FlatBuffers |
| `input_simulation.h` | `input_test_data_generated.h`, `flatbuffers/vector.h` | MEDIUM | Processes FlatBuffers input sequences |
| `event_simulation.h` | `event_packet_data_generated.h`, `flatbuffers/vector.h` | MEDIUM | Processes FlatBuffers event sequences |
| `SceneSnapshot.h` | None | NONE | Pure C++ data structure ✓ |

### Dependency Summary

- **7 of 8** header files have direct FlatBuffers dependencies
- **All core API functions** expose FlatBuffers types
- **Test data flows** directly from `.test_data.bin` files to FlatBuffers types to test functions
- **No abstraction layer** exists between serialization format and test logic

### Architectural Violations

1. **Header Pollution**: FlatBuffers headers included in test harness .h files
2. **Type Exposure**: FlatBuffers types (`TestDataConfig*`, `SimulationData*`, etc.) in public APIs
3. **Tight Coupling**: Test logic directly manipulates FlatBuffers vectors and tables
4. **No Polymorphism**: Cannot swap data sources (e.g., JSON → FlatBuffers → YAML)

---

## Proposed Reorganization

### Three-Layer Architecture

Mirror the main codebase architecture with three distinct layers:

```
┌─────────────────────────────────────────────────────────────┐
│  LAYER 1: Test Orchestration & Logic (Pure C++)            │
│  ────────────────────────────────────────────────────────   │
│  • test_harness.h/cpp - Catch2 integration                 │
│  • TestEngine.h/cpp - Test execution engine                 │
│  • simulation_runner.h/cpp - Logic execution                │
│  • input_simulation.h/cpp - Input simulation                │
│  • event_simulation.h/cpp - Event simulation                │
│  • test_data_comparison.h/cpp - Data validation             │
│  • SceneSnapshot.h - State capture                          │
│                                                              │
│  Uses: Native C++ structs (TestConfig, SimulationConfig)   │
│  NO FlatBuffers includes in headers                         │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ↓ LoadData()
┌─────────────────────────────────────────────────────────────┐
│  LAYER 2: Data Provider Interfaces (Abstract)              │
│  ────────────────────────────────────────────────────────   │
│  • ITestDataProvider.h - Abstract test data loading         │
│  • ISimulationDataProvider.h - Abstract simulation loading  │
│  • IInputSequenceProvider.h - Abstract input loading        │
│  • IEventSequenceProvider.h - Abstract event loading        │
│  • ISnapshotComparisonProvider.h - Abstract comparison      │
│                                                              │
│  Returns: Native C++ structs                                │
│  NO FlatBuffers includes in headers                         │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ↓ Implementation
┌─────────────────────────────────────────────────────────────┐
│  LAYER 3: FlatBuffers Implementations (Concrete)           │
│  ────────────────────────────────────────────────────────   │
│  • FlatbuffersTestDataProvider.cpp                          │
│  • FlatbuffersSimulationDataProvider.cpp                    │
│  • FlatbuffersInputSequenceProvider.cpp                     │
│  • FlatbuffersEventSequenceProvider.cpp                     │
│  • FlatbuffersSnapshotComparisonProvider.cpp                │
│                                                              │
│  Uses: FlatBuffers ONLY in .cpp files                       │
│  Converts: FlatBuffers → Native C++ structs                 │
└─────────────────────────────────────────────────────────────┘
```

### New Directory Structure

```
tests/
├── harness/
│   ├── orchestration/           # Layer 1: Core test logic
│   │   ├── test_harness.h/cpp
│   │   ├── TestEngine.h/cpp
│   │   └── test_data_comparison.h/cpp
│   │
│   ├── simulation/              # Layer 1: Simulation components
│   │   ├── simulation_runner.h/cpp
│   │   ├── input_simulation.h/cpp
│   │   └── event_simulation.h/cpp
│   │
│   ├── data_structures/         # Layer 1: Native C++ structs
│   │   ├── TestConfig.h
│   │   ├── SimulationConfig.h
│   │   ├── InputSequence.h
│   │   ├── EventSequence.h
│   │   ├── SnapshotConfig.h
│   │   └── SceneSnapshot.h
│   │
│   ├── providers/               # Layer 2: Abstract interfaces
│   │   ├── ITestDataProvider.h
│   │   ├── ISimulationDataProvider.h
│   │   ├── IInputSequenceProvider.h
│   │   └── IEventSequenceProvider.h
│   │
│   └── flatbuffers_providers/   # Layer 3: FlatBuffers implementations
│       ├── FlatbuffersTestDataProvider.h/cpp
│       ├── FlatbuffersSimulationDataProvider.h/cpp
│       ├── FlatbuffersInputSequenceProvider.h/cpp
│       └── FlatbuffersEventSequenceProvider.h/cpp
│
└── data/                        # Test data files (.test_data.json/bin)
```

---

## Component Classification

### Layer 1: Test Orchestration & Logic (Pure C++)

**Purpose**: Core test execution logic independent of serialization format

**Components**:

1. **test_harness.h/cpp**
   - Current: Exposes `TestDataConfig*` (FlatBuffers)
   - Future: Uses `TestConfig` (native struct)
   - Functions: `load_test_data_configs()`, `run_fixture_test()`, `RunTestEngineTest()`

2. **TestEngine.h/cpp**
   - Current: Constructor takes `const TestDataConfig*`
   - Future: Constructor takes `const TestConfig&`
   - Responsibility: Execute test with inputs, events, simulation, and validation

3. **simulation_runner.h/cpp**
   - Current: Takes `const SimulationData*`
   - Future: Takes `const SimulationConfig&`
   - Responsibility: Execute Logic classes and free functions

4. **input_simulation.h/cpp**
   - Current: Takes `flatbuffers::Vector<InputEvent>*`
   - Future: Takes `const InputSequence&`
   - Responsibility: Simulate user input (keyboard, mouse)

5. **event_simulation.h/cpp**
   - Current: Takes `flatbuffers::Vector<EventPacketData>*`
   - Future: Takes `const EventSequence&`
   - Responsibility: Inject events into EventHandler

6. **test_data_comparison.h/cpp**
   - Current: Takes `const TestDataConfig*` for comparison
   - Future: Takes `const SnapshotConfig&` for comparison
   - Responsibility: Compare actual vs expected state

7. **SceneSnapshot.h**
   - Current: Pure C++ struct ✓
   - Future: No changes needed ✓
   - Responsibility: Capture scene state at a tick

### Layer 2: Data Provider Interfaces (Abstract)

**Purpose**: Abstract data loading from serialization format

**New Components**:

1. **ITestDataProvider.h**
   ```cpp
   class ITestDataProvider {
   public:
     virtual ~ITestDataProvider() = default;
     
     virtual std::expected<TestConfig, FailInfo>
     LoadTestConfig(const std::string& test_name) const = 0;
     
     virtual std::expected<std::vector<TestConfig>, FailInfo>
     LoadAllTestConfigs(const std::string& directory) const = 0;
   };
   ```

2. **ISimulationDataProvider.h**
   ```cpp
   class ISimulationDataProvider {
   public:
     virtual ~ISimulationDataProvider() = default;
     
     virtual std::expected<SimulationConfig, FailInfo>
     LoadSimulationConfig(const TestConfig& config) const = 0;
   };
   ```

3. **IInputSequenceProvider.h**
   ```cpp
   class IInputSequenceProvider {
   public:
     virtual ~IInputSequenceProvider() = default;
     
     virtual std::expected<InputSequence, FailInfo>
     LoadInputSequence(const TestConfig& config) const = 0;
   };
   ```

4. **IEventSequenceProvider.h**
   ```cpp
   class IEventSequenceProvider {
   public:
     virtual ~IEventSequenceProvider() = default;
     
     virtual std::expected<EventSequence, FailInfo>
     LoadEventSequence(const TestConfig& config) const = 0;
   };
   ```

### Layer 3: FlatBuffers Implementations (Concrete)

**Purpose**: Convert FlatBuffers data to native C++ structs

**New Components**:

1. **FlatbuffersTestDataProvider.cpp**
   - Implements `ITestDataProvider`
   - Includes `test_data_generated.h` ONLY in .cpp
   - Converts `TestDataConfig` (FlatBuffers) → `TestConfig` (native)

2. **FlatbuffersSimulationDataProvider.cpp**
   - Implements `ISimulationDataProvider`
   - Includes `simulation_generated.h` ONLY in .cpp
   - Converts `SimulationData` (FlatBuffers) → `SimulationConfig` (native)

3. **FlatbuffersInputSequenceProvider.cpp**
   - Implements `IInputSequenceProvider`
   - Includes `input_test_data_generated.h` ONLY in .cpp
   - Converts FlatBuffers vectors → `InputSequence` (native)

4. **FlatbuffersEventSequenceProvider.cpp**
   - Implements `IEventSequenceProvider`
   - Includes `event_packet_data_generated.h` ONLY in .cpp
   - Converts FlatBuffers vectors → `EventSequence` (native)

---

## TestConfig and SaveData Alignment

### Architectural Requirement

**TestConfig must mirror SaveData structure** to ensure TestEngine operates identically to Engine/GameEngine. This alignment is critical for:

1. **Consistent data loading** - Both engines load SceneData collections
2. **Realistic testing** - Tests validate production data flow patterns
3. **Easy migration** - Test data can be converted to/from save data
4. **Shared validation** - Same scene configuration patterns

### SaveData Structure (Production)

```cpp
struct SaveData {
  struct Metadata {
    std::string save_name;
    std::string created_at;
    std::string last_modified;
    std::string game_version;
    uint64_t play_time_seconds{0};
    uint32_t slot_index{0};
  } metadata;
  
  SceneType current_scene_type{SceneType::SceneType_UNKNOWN};
  uint32_t version{1};
  
  // Note: Scene state data will be added when implemented
  // For now, SceneData loaded separately by Engine/SceneManager
};
```

### SceneData Structure (Production)

```cpp
// From scene_data.fbs
table SceneDataFbs {
  scene_info: SceneInfoFbs;
  scene_resources: SceneResourcesFbs;
  assets: AssetCollection;
  entity_collection: EntityCollectionFbs;
  logic_collection_data: LogicCollectionData;
}
```

### TestConfig Structure (Proposed)

```cpp
struct TestConfig {
  TestMetadata metadata;  // Similar to SaveData::Metadata
  
  // SAME AS PRODUCTION: Scene data collection
  std::vector<SceneData> scenes;  // TestEngine loads these like GameEngine
  
  // TEST-SPECIFIC ADDITIONS (only differences):
  std::optional<SimulationConfig> simulation_data;  // How to execute test
  std::optional<InputSequence> input_sequence;      // User input simulation
  std::optional<EventSequence> event_sequence;      // Event injection
  
  // TEST VALIDATION (extraction at the end):
  uint32_t num_ticks{1};
  std::vector<SnapshotConfig> tick_snapshots;  // Expected states at ticks
};
```

### Key Differences (TestConfig vs SaveData)

| Aspect | SaveData | TestConfig |
|--------|----------|------------|
| **Core Data** | SceneData collection | SceneData collection ✓ (SAME) |
| **Loading Pattern** | Loaded by GameEngine | Loaded by TestEngine ✓ (SAME) |
| **Simulation** | N/A | SimulationConfig (test execution) |
| **Input** | Real user input | InputSequence (simulated) |
| **Events** | Real game events | EventSequence (injected) |
| **Validation** | N/A | SnapshotConfig (expected states) |

### Benefits of Alignment

1. **Architectural Consistency**: TestEngine operates like GameEngine
2. **Data Reuse**: Test data can become save data and vice versa
3. **Validation**: Tests validate production data patterns
4. **Migration Path**: Easy to add scene state to SaveData later
5. **Learning Curve**: Understanding one system helps understand the other

---

## Migration Strategy

### Phase 1: Create Native Data Structures

**Goal**: Define C++ structs that replace FlatBuffers types in test logic

**Tasks**:
1. Create `tests/harness/data_structures/TestConfig.h`
2. Create `tests/harness/data_structures/SimulationConfig.h`
3. Create `tests/harness/data_structures/InputSequence.h`
4. Create `tests/harness/data_structures/EventSequence.h`
5. Create `tests/harness/data_structures/SnapshotConfig.h`

**Example - TestConfig.h**:
```cpp
#pragma once
#include "SceneData.h"  // Production SceneData struct
#include <string>
#include <vector>
#include <optional>
#include <cstdint>

namespace steamrot::tests {

struct TestMetadata {
  std::string test_name;
  std::string description;
  std::vector<std::string> tags;
  bool expected_to_pass{true};
  std::string author;
  uint32_t version{1};
};

// TestConfig aligns with SaveData structure
// The key difference: adds simulation_data for test execution
struct TestConfig {
  TestMetadata metadata;
  
  // Scene data collection (mirrors SaveData/GameEngine pattern)
  // TestEngine loads scenes just like Engine/GameEngine
  std::vector<SceneData> scenes;
  
  // Test-specific additions:
  std::optional<SimulationConfig> simulation_data;  // How to execute test
  std::optional<InputSequence> input_sequence;      // User input simulation
  std::optional<EventSequence> event_sequence;      // Event injection
  
  // Test validation:
  uint32_t num_ticks{1};
  std::vector<SnapshotConfig> tick_snapshots;  // Expected states at ticks
  
  // Note: start_entity_collection and expected_entity_collection removed
  // Entity data now lives in SceneData (same as production code)
};

} // namespace steamrot::tests
```

### Phase 2: Create Provider Interfaces

**Goal**: Define abstract interfaces for data loading

**Tasks**:
1. Create `tests/harness/providers/ITestDataProvider.h`
2. Create `tests/harness/providers/ISimulationDataProvider.h`
3. Create `tests/harness/providers/IInputSequenceProvider.h`
4. Create `tests/harness/providers/IEventSequenceProvider.h`

**Example - ITestDataProvider.h**:
```cpp
#pragma once
#include "TestConfig.h"
#include "FailInfo.h"
#include <expected>
#include <string>
#include <vector>

namespace steamrot::tests {

class ITestDataProvider {
public:
  virtual ~ITestDataProvider() = default;
  
  virtual std::expected<TestConfig, FailInfo>
  LoadTestConfig(const std::string& test_name, 
                 const std::string& subdirectory = "") const = 0;
  
  virtual std::expected<std::vector<TestConfig>, FailInfo>
  LoadAllTestConfigs(const std::string& subdirectory = "") const = 0;
  
  virtual std::expected<std::vector<std::string>, FailInfo>
  DiscoverTestNames(const std::string& subdirectory = "") const = 0;
};

} // namespace steamrot::tests
```

### Phase 3: Implement FlatBuffers Providers

**Goal**: Implement concrete providers that convert FlatBuffers to native structs

**Tasks**:
1. Create `FlatbuffersTestDataProvider.cpp` (implementation only, no .h)
2. Create `FlatbuffersSimulationDataProvider.cpp`
3. Create `FlatbuffersInputSequenceProvider.cpp`
4. Create `FlatbuffersEventSequenceProvider.cpp`

**Example - FlatbuffersTestDataProvider.cpp**:
```cpp
#include "ITestDataProvider.h"
#include "test_data_generated.h"  // ONLY in .cpp file
#include <fstream>

namespace steamrot::tests {

class FlatbuffersTestDataProvider : public ITestDataProvider {
public:
  std::expected<TestConfig, FailInfo>
  LoadTestConfig(const std::string& test_name,
                 const std::string& subdirectory) const override {
    // 1. Load .test_data.bin file
    // 2. Parse FlatBuffers
    // 3. Convert TestDataConfig (FB) → TestConfig (native)
    // 4. Return native struct
  }
  
  std::expected<std::vector<TestConfig>, FailInfo>
  LoadAllTestConfigs(const std::string& subdirectory) const override {
    // Implementation
  }
  
  std::expected<std::vector<std::string>, FailInfo>
  DiscoverTestNames(const std::string& subdirectory) const override {
    // Implementation
  }

private:
  TestConfig ConvertFlatBuffersToNative(const TestDataConfig* fb_config) const {
    TestConfig config;
    
    // Convert metadata
    if (fb_config->metadata()) {
      config.metadata.test_name = fb_config->metadata()->test_name()->str();
      config.metadata.description = 
          fb_config->metadata()->description() 
              ? fb_config->metadata()->description()->str() 
              : "";
      // ... convert other fields ...
    }
    
    // Convert entity collections
    if (fb_config->start_entity_collection()) {
      config.start_entity_collection = ConvertEntityCollection(
          fb_config->start_entity_collection());
    }
    
    // ... convert other fields ...
    
    return config;
  }
};

} // namespace steamrot::tests
```

### Phase 4: Refactor Test Harness Components

**Goal**: Update test harness to use providers and native structs

**Tasks**:
1. Update `test_harness.h/cpp` to use `TestConfig` instead of `TestDataConfig*`
2. Update `TestEngine.h/cpp` to take `TestConfig` in constructor
3. Update `simulation_runner.h/cpp` to use `SimulationConfig`
4. Update `input_simulation.h/cpp` to use `InputSequence`
5. Update `event_simulation.h/cpp` to use `EventSequence`
6. Update `test_data_comparison.h/cpp` to use `SnapshotConfig`

**Example - Updated test_harness.h**:
```cpp
#pragma once
#include "TestConfig.h"  // Native struct, NOT FlatBuffers
#include "FailInfo.h"
#include <expected>
#include <vector>

namespace steamrot::tests {

// Load test configs using provider (implementation detail hidden)
std::expected<std::vector<TestConfig>, FailInfo>
load_test_data_configs(const std::string& subdirectory = "");

// Run test with native struct
std::expected<std::monostate, FailInfo>
RunTestEngineTest(const TestConfig& config);  // Reference, not pointer!

} // namespace steamrot::tests
```

### Phase 5: Update Documentation

**Goal**: Document new architecture and migration guide

**Tasks**:
1. Update `TEST_DATA_CONFIGURATION.md` to explain provider pattern
2. Update `TESTING_OVERVIEW.md` to reference new architecture
3. Create `TEST_HARNESS_ARCHITECTURE.md` with layer diagrams
4. Update examples in `FILLING_TEST_DATA.md`

---

## Benefits

### 1. Architectural Consistency

- **Test harness matches main codebase** architecture
- **Same patterns** used for data loading (providers)
- **Consistent abstractions** across production and test code

### 2. Reduced Coupling

- **No FlatBuffers in test logic** headers
- **Serialization format isolated** to provider implementations
- **Easy to swap** data sources (FlatBuffers → JSON → YAML)

### 3. Improved Build Times

- **Fewer recompilations** when test schemas change
- **FlatBuffers headers** only in .cpp files
- **Test logic independent** of serialization format

### 4. Better Testing

- **Unit test providers** independently
- **Mock providers** for isolated testing
- **Test data validation** separated from test execution

### 5. Maintainability

- **Clear responsibilities** for each layer
- **Easy to extend** with new data types
- **Predictable structure** for new developers

### 6. Future-Proofing

- **Format-agnostic** test harness
- **Easy migration** to new serialization formats
- **Supports multiple** data sources simultaneously

---

## Implementation Plan

### Priority 1: Core Infrastructure (Week 1)

- [ ] Create `tests/harness/data_structures/` directory
- [ ] Define `TestConfig` struct
- [ ] Define `SimulationConfig` struct
- [ ] Define `InputSequence` struct
- [ ] Define `EventSequence` struct
- [ ] Define `SnapshotConfig` struct

### Priority 2: Provider Interfaces (Week 1)

- [ ] Create `tests/harness/providers/` directory
- [ ] Define `ITestDataProvider` interface
- [ ] Define `ISimulationDataProvider` interface
- [ ] Define `IInputSequenceProvider` interface
- [ ] Define `IEventSequenceProvider` interface

### Priority 3: FlatBuffers Implementations (Week 2)

- [ ] Create `tests/harness/flatbuffers_providers/` directory
- [ ] Implement `FlatbuffersTestDataProvider`
- [ ] Implement `FlatbuffersSimulationDataProvider`
- [ ] Implement `FlatbuffersInputSequenceProvider`
- [ ] Implement `FlatbuffersEventSequenceProvider`
- [ ] Write unit tests for each provider

### Priority 4: Refactor Test Harness (Week 2-3)

- [ ] Update `test_harness.h/cpp` to use native structs
- [ ] Update `TestEngine.h/cpp` to use native structs
- [ ] Update `simulation_runner.h/cpp` to use native structs
- [ ] Update `input_simulation.h/cpp` to use native structs
- [ ] Update `event_simulation.h/cpp` to use native structs
- [ ] Update `test_data_comparison.h/cpp` to use native structs

### Priority 5: Update Tests (Week 3)

- [ ] Update existing test files to use new API
- [ ] Verify all tests pass with new architecture
- [ ] Update test data files if needed

### Priority 6: Documentation (Week 3-4)

- [ ] Update all test documentation
- [ ] Create architecture diagrams
- [ ] Write migration guide
- [ ] Update README files

---

## Conclusion

The test harness requires reorganization to align with the repository's architectural goals of abstracting FlatBuffers and maintaining clean layer boundaries. The proposed three-layer architecture provides:

1. **Clear separation** between test logic, abstraction, and implementation
2. **Consistency** with main codebase architecture
3. **Reduced coupling** to serialization format
4. **Improved maintainability** and extensibility

**Next Steps**:
1. Review and approve this analysis
2. Begin Phase 1: Create native data structures
3. Incrementally migrate components following the phased approach
4. Update documentation alongside code changes

This reorganization will make the test harness more maintainable, extensible, and consistent with the overall repository architecture.
