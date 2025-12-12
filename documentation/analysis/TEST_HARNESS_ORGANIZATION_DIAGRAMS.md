# Test Harness Organization - Visual Diagrams

[← Back to Full Analysis](TEST_HARNESS_ORGANIZATION_ANALYSIS.md) | [Quick Reference](TEST_HARNESS_ORGANIZATION_QUICK_REF.md)

## Overview

This document provides visual diagrams to illustrate the proposed test harness reorganization.

---

## Current Architecture Problems

### FlatBuffers Dependency Cascade

```
┌──────────────────────────────────────────────────────────────┐
│                     TEST CODE (Unit Tests)                   │
└────────────────────────────────┬─────────────────────────────┘
                                 │
                                 ▼
┌──────────────────────────────────────────────────────────────┐
│                   test_harness.h                             │
│  #include "test_data_generated.h"  ← FlatBuffers!           │
│                                                               │
│  std::vector<const TestDataConfig*> ← FlatBuffers pointer!  │
│  load_test_data_configs()                                    │
└────────────────────────────────┬─────────────────────────────┘
                                 │
                 ┌───────────────┼────────────────┐
                 ▼               ▼                ▼
┌─────────────────────┐ ┌────────────────┐ ┌──────────────────┐
│   TestEngine.h      │ │ simulation_    │ │ input_           │
│   #include          │ │ runner.h       │ │ simulation.h     │
│   test_data_        │ │ #include       │ │ #include         │
│   generated.h       │ │ simulation_    │ │ input_test_data_ │
│                     │ │ generated.h    │ │ generated.h      │
│   ← FlatBuffers!    │ │ ← FlatBuffers! │ │ ← FlatBuffers!   │
└─────────────────────┘ └────────────────┘ └──────────────────┘

❌ Problem: FlatBuffers types exposed throughout entire stack
❌ Problem: Recompilation cascade when schemas change
❌ Problem: Cannot swap serialization formats
❌ Problem: Test logic tightly coupled to data format
```

---

## Proposed Architecture Solution

### Three-Layer Separation

```
┌──────────────────────────────────────────────────────────────┐
│                     TEST CODE (Unit Tests)                   │
│  Uses: Native C++ structs (TestConfig, SimulationConfig)    │
└────────────────────────────────┬─────────────────────────────┘
                                 │
                                 ▼
╔══════════════════════════════════════════════════════════════╗
║              LAYER 1: Test Orchestration & Logic             ║
║══════════════════════════════════════════════════════════════║
║                                                               ║
║  ┌────────────────────┐  ┌────────────────────┐             ║
║  │  test_harness.h    │  │  TestEngine.h      │             ║
║  │  Uses: TestConfig  │  │  Uses: TestConfig  │             ║
║  │  NO FlatBuffers ✓  │  │  NO FlatBuffers ✓  │             ║
║  └────────────────────┘  └────────────────────┘             ║
║                                                               ║
║  ┌────────────────────┐  ┌────────────────────┐             ║
║  │ simulation_        │  │ input_simulation.h │             ║
║  │ runner.h           │  │ Uses: InputSequence│             ║
║  │ Uses: Simulation   │  │ NO FlatBuffers ✓   │             ║
║  │ Config             │  └────────────────────┘             ║
║  │ NO FlatBuffers ✓   │                                      ║
║  └────────────────────┘                                      ║
║                                                               ║
╚══════════════════════════╤═══════════════════════════════════╝
                           │ LoadData()
                           ▼
╔══════════════════════════════════════════════════════════════╗
║              LAYER 2: Provider Interfaces (Abstract)         ║
║══════════════════════════════════════════════════════════════║
║                                                               ║
║  ┌──────────────────────────────────────────────┐            ║
║  │  class ITestDataProvider {                   │            ║
║  │    virtual std::expected<TestConfig, ...>    │            ║
║  │    LoadTestConfig(...) const = 0;            │            ║
║  │  };                                           │            ║
║  │  Returns: Native C++ structs                 │            ║
║  │  NO FlatBuffers in interface ✓               │            ║
║  └──────────────────────────────────────────────┘            ║
║                                                               ║
║  ┌──────────────────────────────────────────────┐            ║
║  │  class ISimulationDataProvider {             │            ║
║  │    virtual std::expected<SimulationConfig>   │            ║
║  │    LoadSimulationConfig(...) const = 0;      │            ║
║  │  };                                           │            ║
║  └──────────────────────────────────────────────┘            ║
║                                                               ║
╚══════════════════════════╤═══════════════════════════════════╝
                           │ Implementation
                           ▼
╔══════════════════════════════════════════════════════════════╗
║          LAYER 3: FlatBuffers Implementations (Concrete)     ║
║══════════════════════════════════════════════════════════════║
║                                                               ║
║  ┌──────────────────────────────────────────────┐            ║
║  │  FlatbuffersTestDataProvider.cpp             │            ║
║  │  #include "test_data_generated.h"            │            ║
║  │  ↑ FlatBuffers ONLY in .cpp file ✓           │            ║
║  │                                               │            ║
║  │  TestConfig ConvertToNative(                 │            ║
║  │      const TestDataConfig* fb) {             │            ║
║  │    // Convert FlatBuffers → Native           │            ║
║  │    TestConfig config;                        │            ║
║  │    config.metadata.test_name =               │            ║
║  │        fb->metadata()->test_name()->str();   │            ║
║  │    return config;                            │            ║
║  │  }                                            │            ║
║  └──────────────────────────────────────────────┘            ║
║                                                               ║
║  ┌──────────────────────────────────────────────┐            ║
║  │  FlatbuffersSimulationDataProvider.cpp       │            ║
║  │  #include "simulation_generated.h"           │            ║
║  │  ↑ FlatBuffers ONLY in .cpp file ✓           │            ║
║  └──────────────────────────────────────────────┘            ║
║                                                               ║
╚══════════════════════════════════════════════════════════════╝

✅ Benefit: Clean layer separation
✅ Benefit: FlatBuffers isolated to implementations
✅ Benefit: Can swap serialization formats
✅ Benefit: Minimal recompilation on schema changes
```

---

## Data Flow Comparison

### Current Data Flow

```
Test File (.test_data.json)
        ↓
    flatc compile
        ↓
Binary File (.test_data.bin)
        ↓
┌───────────────────────────┐
│ test_data_loader.cpp      │
│ Loads binary              │
│ Parses FlatBuffers        │
└────────────┬──────────────┘
             ↓
   const TestDataConfig*  ← FlatBuffers pointer
             ↓
┌───────────────────────────┐
│ test_harness.h            │
│ Returns FB pointer        │ ← ❌ FlatBuffers exposed
└────────────┬──────────────┘
             ↓
┌───────────────────────────┐
│ TestEngine.h              │
│ Takes FB pointer          │ ← ❌ FlatBuffers exposed
└────────────┬──────────────┘
             ↓
┌───────────────────────────┐
│ simulation_runner.h       │
│ Reads FB data directly    │ ← ❌ FlatBuffers exposed
└───────────────────────────┘
```

### Proposed Data Flow

```
Test File (.test_data.json)
        ↓
    flatc compile
        ↓
Binary File (.test_data.bin)
        ↓
┌───────────────────────────────────────────────┐
│ FlatbuffersTestDataProvider.cpp               │
│ ┌─────────────────────────────────────────┐   │
│ │ 1. Loads binary file                    │   │
│ │ 2. Parses FlatBuffers                   │   │
│ │ 3. const TestDataConfig* fb ← internal  │   │
│ │ 4. TestConfig native = Convert(fb)      │   │
│ │ 5. return native                        │   │
│ └─────────────────────────────────────────┘   │
│ FlatBuffers ONLY inside this file             │
└────────────────────┬──────────────────────────┘
                     ↓
              TestConfig  ← ✅ Native struct
                     ↓
┌───────────────────────────┐
│ test_harness.h            │
│ Returns TestConfig        │ ← ✅ Native struct
└────────────┬──────────────┘
             ↓
┌───────────────────────────┐
│ TestEngine.h              │
│ Takes TestConfig&         │ ← ✅ Native struct
└────────────┬──────────────┘
             ↓
┌───────────────────────────┐
│ simulation_runner.h       │
│ Uses SimulationConfig     │ ← ✅ Native struct
└───────────────────────────┘

FlatBuffers never escape Layer 3
```

---

## Directory Structure Transformation

### Before (Current)

```
tests/harness/
│
├── test_harness.h            ← Exposes FlatBuffers types
├── test_harness.cpp
│
├── test_data_loader.h        ← Exposes FlatBuffers types
├── test_data_loader.cpp      ← Includes *_generated.h
│
├── TestEngine.h              ← Exposes FlatBuffers types
├── TestEngine.cpp
│
├── simulation_runner.h       ← Exposes FlatBuffers types
├── simulation_runner.cpp
│
├── input_simulation.h        ← Exposes FlatBuffers types
├── input_simulation.cpp
│
├── event_simulation.h        ← Exposes FlatBuffers types
├── event_simulation.cpp
│
├── test_data_comparison.h    ← Exposes FlatBuffers types
├── test_data_comparison.cpp
│
└── SceneSnapshot.h           ← ✓ Already clean

❌ Mixed concerns
❌ FlatBuffers everywhere
❌ No clear organization
```

### After (Proposed)

```
tests/harness/
│
├── orchestration/                    ← Layer 1
│   ├── test_harness.h                ← ✓ Native structs only
│   ├── test_harness.cpp
│   ├── TestEngine.h                  ← ✓ Native structs only
│   ├── TestEngine.cpp
│   ├── test_data_comparison.h        ← ✓ Native structs only
│   └── test_data_comparison.cpp
│
├── simulation/                       ← Layer 1
│   ├── simulation_runner.h           ← ✓ Native structs only
│   ├── simulation_runner.cpp
│   ├── input_simulation.h            ← ✓ Native structs only
│   ├── input_simulation.cpp
│   ├── event_simulation.h            ← ✓ Native structs only
│   └── event_simulation.cpp
│
├── data_structures/                  ← Layer 1
│   ├── TestConfig.h                  ← ✓ Pure C++ struct
│   ├── SimulationConfig.h            ← ✓ Pure C++ struct
│   ├── InputSequence.h               ← ✓ Pure C++ struct
│   ├── EventSequence.h               ← ✓ Pure C++ struct
│   ├── SnapshotConfig.h              ← ✓ Pure C++ struct
│   └── SceneSnapshot.h               ← ✓ Pure C++ struct
│
├── providers/                        ← Layer 2
│   ├── ITestDataProvider.h           ← ✓ Abstract interface
│   ├── ISimulationDataProvider.h     ← ✓ Abstract interface
│   ├── IInputSequenceProvider.h      ← ✓ Abstract interface
│   └── IEventSequenceProvider.h      ← ✓ Abstract interface
│
└── flatbuffers_providers/            ← Layer 3
    ├── FlatbuffersTestDataProvider.cpp       ← FlatBuffers HERE
    ├── FlatbuffersSimulationDataProvider.cpp ← FlatBuffers HERE
    ├── FlatbuffersInputSequenceProvider.cpp  ← FlatBuffers HERE
    └── FlatbuffersEventSequenceProvider.cpp  ← FlatBuffers HERE
    
✅ Clear layer separation
✅ FlatBuffers isolated to Layer 3
✅ Organized by responsibility
```

---

## Component Dependency Graph

### Current (Tangled)

```
                    test_harness.h
                           │
        ┌──────────────────┼──────────────────┐
        │                  │                   │
        ▼                  ▼                   ▼
  TestEngine.h    simulation_runner.h   test_data_loader.h
        │                  │                   │
        │                  │                   │
        └──────────────────┴───────────────────┘
                           │
                           ▼
                  test_data_generated.h  ← FlatBuffers
                           │
        ┌──────────────────┼──────────────────┐
        │                  │                   │
        ▼                  ▼                   ▼
 simulation_         input_test_data_    event_packet_data_
 generated.h         generated.h         generated.h

❌ Everything depends on FlatBuffers
❌ Changes cascade through entire system
```

### Proposed (Clean Layers)

```
Test Code
    │
    ▼
┌─────────────────────────────────────────────┐
│         Layer 1: Test Logic                 │
│                                              │
│  test_harness ─→ TestEngine ─→ simulation_  │
│      │              │            runner      │
│      │              │              │         │
│      └──────────────┴──────────────┘         │
│                     │                        │
│                     ▼                        │
│           data_structures/                  │
│           (TestConfig, etc.)                │
└─────────────────────┬───────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────┐
│         Layer 2: Provider Interfaces        │
│                                              │
│  ITestDataProvider ─→ ISimulationData...    │
│                                              │
└─────────────────────┬───────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────┐
│         Layer 3: Implementations            │
│                                              │
│  Flatbuffers...Provider ─→ *_generated.h    │
│  (FlatBuffers contained here)               │
└─────────────────────────────────────────────┘

✅ Unidirectional dependencies
✅ FlatBuffers isolated to Layer 3
✅ Clear separation of concerns
```

---

## API Transformation Example

### Current API (FlatBuffers Exposed)

```cpp
// test_harness.h
#include "test_data_generated.h"  // ❌ FlatBuffers in header

namespace steamrot::tests {

// ❌ Returns FlatBuffers pointers
std::expected<std::vector<const TestDataConfig*>, FailInfo>
load_test_data_configs(const std::string& subdirectory = "");

// ❌ Takes FlatBuffers pointer
std::expected<std::monostate, FailInfo>
RunTestEngineTest(const TestDataConfig* config);

}
```

```cpp
// Usage in test
#include "test_harness.h"
#include "test_data_generated.h"  // ❌ Test needs FlatBuffers

TEST_CASE("My test") {
  auto configs = steamrot::tests::load_test_data_configs();
  const TestDataConfig* config = configs.value()[0];  // ❌ FlatBuffers pointer
  
  auto result = steamrot::tests::RunTestEngineTest(config);
  REQUIRE(result.has_value());
}
```

### Proposed API (Native Structs)

```cpp
// test_harness.h
#include "TestConfig.h"  // ✅ Native struct

namespace steamrot::tests {

// ✅ Returns native structs
std::expected<std::vector<TestConfig>, FailInfo>
load_test_data_configs(const std::string& subdirectory = "");

// ✅ Takes native struct reference
std::expected<std::monostate, FailInfo>
RunTestEngineTest(const TestConfig& config);

}
```

```cpp
// Usage in test
#include "test_harness.h"
// NO FlatBuffers includes needed! ✅

TEST_CASE("My test") {
  auto configs = steamrot::tests::load_test_data_configs();
  const TestConfig& config = configs.value()[0];  // ✅ Native struct
  
  auto result = steamrot::tests::RunTestEngineTest(config);
  REQUIRE(result.has_value());
}
```

---

## Provider Pattern Detail

### Interface Definition (Layer 2)

```cpp
// tests/harness/providers/ITestDataProvider.h

#pragma once
#include "TestConfig.h"  // Native struct
#include "FailInfo.h"
#include <expected>
#include <vector>
#include <string>

namespace steamrot::tests {

class ITestDataProvider {
public:
  virtual ~ITestDataProvider() = default;
  
  // Load single test configuration
  virtual std::expected<TestConfig, FailInfo>
  LoadTestConfig(const std::string& test_name,
                 const std::string& subdirectory = "") const = 0;
  
  // Load all test configurations from directory
  virtual std::expected<std::vector<TestConfig>, FailInfo>
  LoadAllTestConfigs(const std::string& subdirectory = "") const = 0;
  
  // Discover available test names
  virtual std::expected<std::vector<std::string>, FailInfo>
  DiscoverTestNames(const std::string& subdirectory = "") const = 0;
};

} // namespace steamrot::tests
```

### Implementation (Layer 3)

```cpp
// tests/harness/flatbuffers_providers/FlatbuffersTestDataProvider.cpp

#include "ITestDataProvider.h"
#include "test_data_generated.h"  // ✅ FlatBuffers ONLY in .cpp
#include <fstream>

namespace steamrot::tests {

class FlatbuffersTestDataProvider : public ITestDataProvider {
public:
  std::expected<TestConfig, FailInfo>
  LoadTestConfig(const std::string& test_name,
                 const std::string& subdirectory) const override {
    
    // 1. Load binary file
    std::string file_path = BuildFilePath(test_name, subdirectory);
    std::ifstream file(file_path, std::ios::binary);
    // ... read file ...
    
    // 2. Parse FlatBuffers (stays inside this function)
    const TestDataConfig* fb_config = 
        GetTestDataConfig(buffer.data());
    
    // 3. Convert to native struct
    TestConfig native_config = ConvertToNative(fb_config);
    
    // 4. Return native struct (FlatBuffers never escape)
    return native_config;
  }
  
private:
  // Conversion happens inside implementation
  TestConfig ConvertToNative(const TestDataConfig* fb) const {
    TestConfig config;
    
    // Convert metadata
    if (fb->metadata()) {
      config.metadata.test_name = 
          fb->metadata()->test_name()->str();
      config.metadata.description = 
          fb->metadata()->description() 
              ? fb->metadata()->description()->str() 
              : "";
      // ...
    }
    
    // Convert entity collections
    if (fb->start_entity_collection()) {
      config.start_entity_collection = 
          ConvertEntityCollection(fb->start_entity_collection());
    }
    
    // ... convert other fields ...
    
    return config;
  }
};

} // namespace steamrot::tests
```

---

## Migration Path Visualization

### Phase 1: Create Native Structs

```
tests/harness/data_structures/
    ├── TestConfig.h         [NEW]
    ├── SimulationConfig.h   [NEW]
    ├── InputSequence.h      [NEW]
    ├── EventSequence.h      [NEW]
    └── SnapshotConfig.h     [NEW]
```

### Phase 2: Create Provider Interfaces

```
tests/harness/providers/
    ├── ITestDataProvider.h           [NEW]
    ├── ISimulationDataProvider.h     [NEW]
    ├── IInputSequenceProvider.h      [NEW]
    └── IEventSequenceProvider.h      [NEW]
```

### Phase 3: Implement Providers

```
tests/harness/flatbuffers_providers/
    ├── FlatbuffersTestDataProvider.cpp         [NEW]
    ├── FlatbuffersSimulationDataProvider.cpp   [NEW]
    ├── FlatbuffersInputSequenceProvider.cpp    [NEW]
    └── FlatbuffersEventSequenceProvider.cpp    [NEW]
```

### Phase 4: Refactor Components

```
tests/harness/
    ├── test_harness.h              [MODIFY: Remove FlatBuffers]
    ├── TestEngine.h                [MODIFY: Remove FlatBuffers]
    ├── simulation_runner.h         [MODIFY: Remove FlatBuffers]
    ├── input_simulation.h          [MODIFY: Remove FlatBuffers]
    ├── event_simulation.h          [MODIFY: Remove FlatBuffers]
    └── test_data_comparison.h      [MODIFY: Remove FlatBuffers]
```

### Phase 5: Reorganize Directories

```
tests/harness/
    ├── orchestration/          [MOVE FILES]
    ├── simulation/             [MOVE FILES]
    ├── data_structures/        [ALREADY EXISTS]
    ├── providers/              [ALREADY EXISTS]
    └── flatbuffers_providers/  [ALREADY EXISTS]
```

---

## Summary Diagram

```
┌──────────────────────────────────────────────────────────────┐
│                    GOAL: Organized Test Harness              │
└──────────────────────────────────────────────────────────────┘
                             │
        ┌────────────────────┼────────────────────┐
        ▼                    ▼                    ▼
┌──────────────┐    ┌──────────────┐    ┌──────────────┐
│   Clear      │    │  FlatBuffers │    │  Consistent  │
│   Layer      │    │  Isolated to │    │  with Main   │
│   Separation │    │  Layer 3     │    │  Codebase    │
└──────────────┘    └──────────────┘    └──────────────┘
        │                    │                    │
        └────────────────────┴────────────────────┘
                             │
                             ▼
               ┌────────────────────────┐
               │  Maintainable,         │
               │  Extensible,           │
               │  Future-Proof          │
               │  Test Infrastructure   │
               └────────────────────────┘
```

---

## References

- [Full Analysis](TEST_HARNESS_ORGANIZATION_ANALYSIS.md)
- [Quick Reference](TEST_HARNESS_ORGANIZATION_QUICK_REF.md)
- [Data Provider System](../DATA_PROVIDER_SYSTEM.md)
- [Testing Overview](../testing/TESTING_OVERVIEW.md)
