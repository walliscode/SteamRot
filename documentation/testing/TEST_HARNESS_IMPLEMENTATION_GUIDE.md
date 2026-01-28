# Test Harness Implementation Guide

**Purpose**: Step-by-step guide to complete the test harness infrastructure  
**Audience**: Developers implementing test harness features using TDD

---

## Table of Contents

1. [Overview](#overview)
2. [Current State](#current-state)
3. [Missing Components](#missing-components)
4. [Implementation Phases](#implementation-phases)
5. [GitHub Issues Templates](#github-issues-templates)
6. [Testing Strategy](#testing-strategy)

---

## Overview

The test harness enables data-driven testing with these capabilities:

1. **Load test data** from JSON/FlatBuffers files
2. **Modify UUIDs** in test data for scene identification  
3. **Process data** through TestEngine
4. **Extract data** from TestEngine data bank
5. **Run matchers** to compare actual vs expected data

This guide provides TDD issues and examples for implementing the missing pieces.

---

## Current State

### ✅ What Exists

- **TestEngine**: Runs tests, stores snapshots in data bank
- **FlatbuffersTestDataProvider**: Loads test data from files  
- **Matchers**: EntityMemoryPool, SceneData, EventBus comparers
- **TestData struct**: Includes `expected_engine_snapshots` map
- **SimulationRunner**: Executes Logic classes per tick

### ❌ What's Missing

1. **UUID Manager**: Generate/assign UUIDs for scenes
2. **Data Extractor**: Extract data from TestEngine data bank by tick/UUID
3. **Test Orchestrator**: Run complete workflow (validate → run → compare)
4. **Test Data Validator**: Validate test data before execution
5. **FlatBuffers Schema**: Support for `expected_engine_snapshots` in JSON

---

## Missing Components

### 1. UuidManager
**Purpose**: Generate and manage scene UUIDs

**Files**: `tests/harness/UuidManager.{h,cpp}`, `tests/unit/harness/UuidManager.test.cpp`

**Key Methods**:
- `AssignUuidsToTestData(TestData &)` - Assign UUIDs to all scenes
- `GetUuidForSceneType(SceneType)` - Get UUID by scene type
- `MapUuid()`, `GetActualUuid()` - Map expected → actual UUIDs

### 2. DataExtractor
**Purpose**: Extract data from TestEngine data bank

**Files**: `tests/harness/DataExtractor.{h,cpp}`, `tests/unit/harness/DataExtractor.test.cpp`

**Key Methods** (all static):
- `ExtractEntityPool(data_bank, tick, uuid)` - Get entity pool
- `ExtractEventBus(data_bank, tick)` - Get event bus
- `ExtractSnapshot(data_bank, tick)` - Get complete snapshot
- `ExtractSceneData(data_bank, tick, uuid)` - Get scene data

### 3. TestOrchestrator
**Purpose**: Orchestrate complete test workflow

**Files**: `tests/harness/TestOrchestrator.{h,cpp}`, `tests/unit/harness/TestOrchestrator.test.cpp`

**Workflow**:
1. Validate test data
2. Assign UUIDs
3. Run TestEngine
4. Extract results
5. Compare with expected data using matchers

### 4. TestDataValidator
**Purpose**: Validate test data structure

**Files**: `tests/harness/TestDataValidator.{h,cpp}`, `tests/unit/harness/TestDataValidator.test.cpp`

**Checks**:
- Required fields present (test name, ticks > 0)
- Tick numbers consistent
- Warnings for missing expected data

### 5. FlatBuffers Schema Updates
**Purpose**: Support loading expected snapshots from JSON

**Files**: `src/types/flatbuffers/testing/test_data.fbs`, `tests/harness/FlatbuffersTestDataProvider.cpp`

**Changes**:
- Add `TickEngineSnapshotMapEntry` table
- Add `expected_engine_snapshots` field to TestDataFbs
- Implement loading in FlatbuffersTestDataProvider

---

## Implementation Phases

### Phase 1: FlatBuffers Schema (2-4 hours)
1. Add `expected_engine_snapshots` to schema
2. Implement loading in FlatbuffersTestDataProvider
3. Test loading from JSON

### Phase 2: UUID Manager (2-4 hours)
1. Implement UuidManager class
2. Add UUID assignment methods
3. Write unit tests

### Phase 3: Data Extractor (4-6 hours)
1. Implement DataExtractor class
2. Add all extraction methods
3. Write comprehensive unit tests

### Phase 4: Test Data Validator (3-5 hours)
1. Implement TestDataValidator class
2. Add validation checks
3. Write unit tests

### Phase 5: Test Orchestrator (4-6 hours)
1. Implement TestOrchestrator class
2. Integrate all components
3. Write integration tests

### Phase 6: Integration & Docs (4-6 hours)
1. Update harness_runner to use TestOrchestrator
2. Create example test data files
3. Update documentation
4. Write end-to-end integration tests

**Total**: 19-31 hours

---

## GitHub Issues Templates

### Issue Template: Add expected_engine_snapshots to schema

**Title**: Extend TestDataFbs schema to support tick-based expected engine snapshots

**Description**:
```
## Goal
Add FlatBuffers schema support for expected engine snapshots at specific ticks.

## Tasks
- [ ] Add TickEngineSnapshotMapEntry table to test_data.fbs
- [ ] Add expected_engine_snapshots field to TestDataFbs
- [ ] Verify schema compiles
- [ ] Test loading JSON with expected snapshots

## Files to Modify
- src/types/flatbuffers/testing/test_data.fbs

## Example
```fbs
table TickEngineSnapshotMapEntry {
  tick: uint32;
  snapshot: EngineSnapshotFbs;
}

table TestDataFbs {
  // ... existing fields ...
  expected_engine_snapshots: [TickEngineSnapshotMapEntry];
}
```

## Success Criteria
- Schema compiles
- Generated headers include new field
- Can load JSON with expected snapshots
```

**Labels**: `enhancement`, `test-infrastructure`, `flatbuffers`  
**Estimate**: 1-2 hours

---

### Issue Template: Implement UuidManager

**Title**: Create UuidManager to generate and map scene UUIDs

**Description**:
```
## Goal
Create UuidManager class to handle UUID generation and mapping for test scenarios.

## Tasks
- [ ] Create UuidManager.{h,cpp}
- [ ] Implement AssignUuids() for SceneInfo vector
- [ ] Implement GetUuidForSceneType()
- [ ] Implement MapUuid() and GetActualUuid()
- [ ] Implement AssignUuidsToTestData()
- [ ] Write comprehensive unit tests

## Class Interface
```cpp
namespace steamrot::tests {
class UuidManager {
public:
  std::expected<std::monostate, FailInfo>
  AssignUuids(std::vector<SceneInfo> &scene_infos);
  
  std::optional<uuids::uuid> GetUuidForSceneType(SceneType) const;
  
  void MapUuid(const uuids::uuid &expected, const uuids::uuid &actual);
  
  std::optional<uuids::uuid> GetActualUuid(const uuids::uuid &expected) const;
  
  std::expected<std::monostate, FailInfo>
  AssignUuidsToTestData(TestData &test_data);
};
}
```

## Test Cases
- Generate unique UUIDs
- Map scene types to UUIDs
- Assign to TestData (starting + expected snapshots)
- Same scene type gets same UUID across snapshots

## Success Criteria
- All tests pass
- UUIDs correctly assigned
- Mappings work as expected
```

**Labels**: `enhancement`, `test-infrastructure`  
**Estimate**: 2-3 hours

---

### Issue Template: Implement DataExtractor

**Title**: Create DataExtractor to extract data from TestEngine data bank

**Description**:
```
## Goal
Create DataExtractor with static methods to extract data from TestEngine data bank.

## Tasks
- [ ] Create DataExtractor.{h,cpp}
- [ ] Implement ExtractEntityPool(data_bank, tick, uuid)
- [ ] Implement ExtractEventBus(data_bank, tick)
- [ ] Implement ExtractSnapshot(data_bank, tick)
- [ ] Implement ExtractSceneData(data_bank, tick, uuid)
- [ ] Write comprehensive unit tests

## Test Cases
- Extract with valid tick and UUID
- Extract with invalid tick (not found)
- Extract with invalid UUID (scene not found)
- Extract from empty data bank
- EventBus extraction when field not present
- Variant type mismatch handling

## Success Criteria
- All extraction methods work
- Comprehensive error handling
- Informative error messages
- Tests pass
```

**Labels**: `enhancement`, `test-infrastructure`  
**Estimate**: 4-6 hours

---

### Issue Template: Implement TestDataValidator

**Title**: Create TestDataValidator to validate test data before execution

**Description**:
```
## Goal
Create TestDataValidator to check test data for issues before running tests.

## Tasks
- [ ] Create TestDataValidator.{h,cpp}
- [ ] Create ValidationResult struct (errors + warnings)
- [ ] Implement Validate() static method
- [ ] Implement ValidateMetadata(), ValidateTicks(), etc.
- [ ] Write unit tests

## Validation Checks
**Errors** (fail validation):
- Test name is empty
- Number of ticks is zero
- Expected snapshot tick > number_of_ticks

**Warnings** (don't fail):
- Test description is empty
- No scenes in starting snapshot
- No expected snapshots defined

## Success Criteria
- Detects common issues
- Clear error messages
- Errors vs warnings correctly classified
- Tests pass
```

**Labels**: `enhancement`, `test-infrastructure`, `validation`  
**Estimate**: 3-5 hours

---

### Issue Template: Implement TestOrchestrator

**Title**: Create TestOrchestrator to run complete test workflow

**Description**:
```
## Goal
Create TestOrchestrator to orchestrate complete test workflow.

## Dependencies
- All Phase 1-4 issues must be complete

## Tasks
- [ ] Create TestOrchestrator.{h,cpp}
- [ ] Implement RunTest() method
- [ ] Integrate UuidManager, TestDataValidator, DataExtractor
- [ ] Integrate matchers for comparison
- [ ] Write comprehensive integration tests

## RunTest() Workflow
1. Validate test data (fail fast if invalid)
2. Assign UUIDs to test data
3. Create and run TestEngine
4. Get data bank from TestEngine
5. For each expected snapshot:
   - Extract actual snapshot at tick
   - For each scene: compare entity pools using matcher
   - Compare event bus if present

## Test Cases
- End-to-end test with valid test data
- Validation errors prevent execution
- Mismatches are detected
- Error handling at each stage

## Success Criteria
- Complete workflow runs end-to-end
- Comparison detects mismatches
- Informative error messages
- Integration tests pass
```

**Labels**: `enhancement`, `test-infrastructure`, `integration`  
**Estimate**: 4-6 hours

---

## Testing Strategy

### TDD Workflow

For each issue:
1. **Write failing test** for the feature
2. **Run test** - verify it fails (red)
3. **Implement minimum code** to make test pass
4. **Run test** - verify it passes (green)
5. **Refactor** if needed
6. **Repeat**

### Unit Test Pattern

```cpp
TEST_CASE("ClassName::Method does X", "[unit][ClassName]") {
  // Arrange
  steamrot::tests::ClassName instance;
  auto input = CreateTestInput();
  
  // Act
  auto result = instance.Method(input);
  
  // Assert
  REQUIRE(result.has_value());
  REQUIRE(result.value() == expected);
}
```

### Integration Test Pattern

```cpp
TEST_CASE("Complete workflow succeeds", "[integration][harness]") {
  // Arrange
  steamrot::TestData test_data = CreateValidTestData();
  
  // Act
  steamrot::tests::TestOrchestrator orchestrator;
  auto result = orchestrator.RunTest(test_data);
  
  // Assert
  REQUIRE(result.has_value());
}
```

---

## Quick Reference

### Key Existing Files
- `tests/harness/TestEngine.{h,cpp}` - Test execution engine
- `tests/harness/FlatbuffersTestDataProvider.{h,cpp}` - Data loading
- `tests/matchers/EntityMemoryPoolEqualsMatcher.{h,cpp}` - Comparison
- `src/types/test_structs/TestData.h` - Test data structure

### Files to Create
- `tests/harness/UuidManager.{h,cpp}` (Phase 2)
- `tests/harness/DataExtractor.{h,cpp}` (Phase 3)
- `tests/harness/TestDataValidator.{h,cpp}` (Phase 4)
- `tests/harness/TestOrchestrator.{h,cpp}` (Phase 5)

### Key Concepts
- **Data Bank**: Map of tick → EngineSnapshot in TestEngine
- **UUID Mapping**: Expected UUID → Actual UUID for scene matching
- **Tick-Based Testing**: Compare state at specific ticks
- **TestContext**: Enriches matcher output with metadata
- **Orchestration**: Complete workflow from data to validation

---

## Next Steps

1. Review this implementation guide
2. Create GitHub issues using templates above
3. Assign to milestones (Phase 1-6)
4. Implement in order using TDD
5. Update documentation as you go

**Estimated Total Effort**: 19-31 hours of development time
