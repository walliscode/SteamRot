# Simulation Runner Design - Executive Summary

## Overview

This directory contains comprehensive documentation for the **Simulation Runner** component of the TestEngine. This analysis addresses the requirements for a robust, data-driven simulation system that can execute combinations of free functions and Logic classes without requiring recompilation.

## Problem Statement

The TestEngine needs a simulation runner that:
- Allows specification of free functions and/or Logic classes via JSON
- Executes them in configurable order and combination
- Isolates specific effects within the Engine context
- Is robust and doesn't require rebuilding for new test scenarios
- Handles Logic class instantiation and archetype application correctly

## Documentation Structure

### 1. Main Design Document

**File**: `SIMULATION_RUNNER_DESIGN.md` (31KB)

**Contents**:
- Complete architectural design
- Component responsibilities and interfaces
- Logic class instantiation strategy (per-step, stack-allocated)
- Free function execution approach
- Archetype management (automatic via EntityManager)
- Robustness analysis and error handling
- JSON configuration schema and validation
- Integration with TestEngine
- Future extensibility considerations

**Target Audience**: Technical leads, architects, senior developers

**Key Decisions**:
- Per-step Logic instantiation (no caching, no shared state)
- Direct function dispatch via switch statements
- SceneContext provided by TestEngine from active Scene
- std::expected for error handling throughout
- Validation at load time via FlatBuffers provider

### 2. Implementation Guide

**File**: `SIMULATION_RUNNER_IMPLEMENTATION.md` (15KB)

**Contents**:
- Step-by-step implementation instructions
- Code templates for all components
- Header and source file examples
- CMake integration instructions
- Testing strategy and test templates
- Validation checklist
- Common issues and solutions
- Extension guide for new Logic classes and functions

**Target Audience**: Developers implementing the design

**Provides**:
- Copy-paste ready code snippets
- Build integration steps
- Test creation templates
- Troubleshooting guide

### 3. Configuration Examples

**File**: `examples/simulation_examples.md`

**Contents**:
- Quick reference JSON examples
- Simple single-step tests
- Multi-step sequences
- Multi-tick simulations

**Target Audience**: Test authors, QA engineers

**Provides**:
- Ready-to-use JSON templates
- Common patterns for different test scenarios

## Key Design Features

### 1. Simplicity

**Per-Step Instantiation**:
```cpp
// Each step creates a fresh Logic instance
UIActionLogic logic(context);  // Stack-allocated
logic.RunLogic();              // Execute
// Automatic cleanup via RAII
```

**Benefits**:
- No state management
- No instance caching complexity
- Clear lifecycle
- Isolated execution

### 2. Flexibility

**Mixed Execution**:
```json
{
  "steps": [
    {"function_type": "CheckMouseOverNestedUIElement"},
    {"logic_class_type": "UICollisionLogic"},
    {"function_type": "ProcessButtonElementActions"},
    {"logic_class_type": "UIActionLogic"}
  ]
}
```

**Allows**:
- Fine-grained control over execution order
- Isolation of specific function effects
- Testing Logic classes individually or in sequences

### 3. Robustness

**Error Handling**:
- std::expected throughout the call chain
- Descriptive error messages with context
- Validation at configuration load time
- Graceful failure with actionable information

**Type Safety**:
- Compile-time type checking via C++ type system
- FlatBuffers schema validation
- Enum synchronization enforced by conversion functions

### 4. Extensibility

**Adding New Logic Classes**:
1. Implement Logic class (standard process)
2. Add to C++ enum
3. Add to FlatBuffers enum
4. Add case to switch statement
5. Add conversion function

**No Core Changes Needed**:
- Simulation runner doesn't need modification
- TestEngine integration stays the same
- JSON schema extends naturally

## Implementation Roadmap

### Phase 1: Foundation (2-3 days)
- Create SimulationRunner class
- Implement Logic class execution
- Implement free function execution
- Update TestEngine::TickSceneLogic

### Phase 2: Validation (1-2 days)
- Add error handling and validation
- Enhance error messages
- Test error paths

### Phase 3: Testing (2-3 days)
- Write unit tests for SimulationRunner
- Write integration tests for TestEngine
- Create test data files with various scenarios

### Phase 4: Documentation (1 day)
- Update test harness README
- Add inline code documentation
- Create usage examples

**Total Estimated Effort**: 6-9 days

## Integration Points

### With TestEngine

```cpp
void TestEngine::TickSceneLogic() {
  // Get simulation data from m_test_data
  // Get SceneContext from active Scene
  // Create SimulationRunner
  // Execute simulation
  // Handle errors
}
```

**Execution Flow**:
1. TestEngine constructed with TestData (includes SimulationData)
2. Engine initialization creates Scene with entities
3. Each tick, TickSceneLogic runs simulation
4. Simulation executes configured steps in order
5. State captured after simulation completes
6. Snapshot compared with expected values

### With Existing Systems

**EntityManager**: Provides archetypes automatically based on component state

**SceneContext**: Passed to Logic instances and functions for resource access

**FlatBuffers**: Configuration loaded from JSON, converted to C++ structs

**Error Handling**: Uses existing FailInfo/FailMode infrastructure

## Robustness Analysis

### Design Strengths

1. **No State Bugs**: Per-step instantiation prevents state accumulation
2. **Type Safe**: Compile-time checking and runtime validation
3. **Testable**: Clear separation of concerns
4. **Maintainable**: Simple code structure, easy to understand
5. **Extensible**: Adding new capabilities is straightforward
6. **Performant**: Stack allocation, no unnecessary heap usage

### Error Scenarios Handled

- Empty simulation steps
- Invalid enum values (None)
- Missing or inactive Scene
- Invalid SceneContext
- Function execution failures
- Logic class construction failures

### Validation Strategy

**Load Time**:
- FlatBuffers schema validation
- Exactly one element per step (function OR logic class)
- Enum values in valid range

**Runtime**:
- Scene existence and active state
- SceneContext validity
- Step execution success

## Future Enhancements

Documented but not required for initial implementation:

1. **Per-tick simulation control** - Different steps for different ticks
2. **Conditional execution** - Execute based on component/archetype state
3. **Parameterized functions** - Pass runtime parameters to functions
4. **Execution metrics** - Track performance and timing
5. **Nested simulations** - Reusable simulation sequences

These can be added later without breaking existing functionality.

## Success Criteria

The implementation is successful when:

1. ✅ Tests can specify Logic/function sequences in JSON
2. ✅ No recompilation needed for new test scenarios
3. ✅ Simulation executes in configured order
4. ✅ State changes are correctly captured
5. ✅ Error handling provides clear feedback
6. ✅ New Logic classes/functions easy to add
7. ✅ Tests run reliably and consistently

## Resources

### Documentation Files

- **SIMULATION_RUNNER_DESIGN.md** - Complete design specification
- **SIMULATION_RUNNER_IMPLEMENTATION.md** - Developer implementation guide
- **examples/simulation_examples.md** - JSON configuration examples

### Related Code

- **TestEngine** - `tests/harness/TestEngine.{h,cpp}`
- **SimulationData** - `src/types/test_structs/SimulationData.h`
- **Logic Classes** - `src/logic/*.{h,cpp}`
- **SceneContext** - `src/context/SceneContext.h`
- **FlatBuffers Schema** - `src/types/flatbuffers/testing/simulation_data.fbs`

### Additional Context

- **Test Harness README** - `tests/harness/README.md`
- **Copilot Instructions** - `.github/copilot-instructions.md`
- **Main README** - `README.md`

## Conclusion

This design provides a complete, well-thought-out solution to the simulation runner requirements. It:

- **Solves the core problem**: Data-driven test execution without recompilation
- **Is robust**: Comprehensive error handling and validation
- **Is maintainable**: Simple, clear code structure
- **Is extensible**: Easy to add new capabilities
- **Is practical**: Ready for immediate implementation

The design balances simplicity with functionality, preferring straightforward solutions over complex abstractions. It integrates cleanly with the existing TestEngine architecture and follows established patterns in the codebase.

**Implementation can proceed with confidence** based on this design documentation.

---

**Documentation Version**: 1.0  
**Date**: 2026-01-25  
**Status**: Complete - Ready for Implementation  
**Author**: GitHub Copilot (Analysis and Documentation)
