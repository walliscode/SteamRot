# Testing Overview

[← Back to Documentation](README.md) | [Testing Guides](README.md#testing)

This document provides a comprehensive overview of testing in SteamRot, including test structure, classification, TDD workflow, and infrastructure.

**Related Documentation:**
- [Test Data Configuration](TEST_DATA_CONFIGURATION.md) - Data-driven testing
- [Test Harness Adoption](TEST_HARNESS_ADOPTION_PLAN.md) - Migration plan
- [Workflows](../workflows/) - Implementation guides with testing

---

#### Testing Strategy

1. **Unit Tests**: Test Logic class in isolation
   - Constructor
   - ProcessLogic() with various input states
   - Edge cases and error conditions

2. **Integration Tests**: Test Logic with LogicFactory
   - Verify Logic is created for correct scenes
   - Verify correct order in LogicVector
   - Verify correct LogicType assignment

3. **System Tests**: Test Logic with full game context
   - Use TestContext for realistic scenarios
   - Test interaction with other Logic classes
   - Test component state changes

#### Debugging Tips

- Use `CAPTURE()` in tests to output variable values on failure
- Check archetype exists before accessing entities
- Verify component data is properly initialized in test setup
- Run Logic tests individually during development: `./test_logic "[YourLogic]"`
- Use `std::cout` for debugging (will appear in test output)

### Actions

This will attempt to describe the intent behind how actions are set up and how
an operator can add actions to the game.

#### Action Generation

Currently, actions are designed around capturing user input. Though I imagine
this will be expanded to act on global events e.t.c. The events are handled at
the beginning of the game loop (keyboard input and mouse input), this generates
a bit flag that. The bit flag will be available globally or passed to any Action
Managers that need it.

ActionManager instances will be placed in key locations such as Scene instance
and the DisplayManager. They will compare this global bit flag against their own
internal map of actions and generate another bit flag, this time of actions
(that have been defined in an enum). This will be made available to the Scene
Logic to allow for flow control.

These bit flags will be reset each tick of the game loop, so that they are only
valid for the current tick.

#### Action Registration

There is an enum called Actions which the ActionManager has access to. All
actions will be registered in this enum, the predicition being that their will
not be massives on actions that a user can take.

An operator then creates a json which maps keys to actions. The string
representation of the keys and mouse are checked by internal static maps for
correctness. The internal logic of the ActionManager creates bitset id for each
action that maps one or more key/mouse bindings to the action (as a bitflag).

So the steps an operator would take to add an action are:

- check the Action enum to see if the action exists, if not add it to the enum
  and recompile the code.
- create a json segment of the scene JSON that maps the key/mouse to the action.
  The json file should be in the format of:

```json
{
  "actions": [
    {
      "name": "action_name",
      "inputs": [{ "type": "keyboard/mouse", "value": "key_name/mouse_name" }]
    }
  ]
}
```

## Classes

### Logic Class

The Logic class will be an abstract class. The derived classes will be
responsible for changing the game state, such as movement logic, physics logic,
e.t.c.

An inherited member function (PerformLogic()) will take in a unique pointer to
the EntityMemoryPool as well as a container of indices to the entities that need
to be updated. The logic class will then be responsible for updating the game
state of the entities in the container.

It will not be responsible for determining which archetype to use.

Logic objects will provided by the [LogicFactory](#logicfactory) abstract class.

### LogicFactory

The LogicFactory abstract class will be responsible for creating Logic objects.
It will have a pure virtual function (CreateLogic()) that will return a unique
pointer to a Logic object. So a MovementLogicFactory will create a MovementLogic
object depending on the parameters passed to it.

## Testing

SteamRot uses Catch2 for testing with a clear test taxonomy to distinguish between unit, integration, and system tests.

### Test Directory Structure

```
tests/
├── unit/                    # Unit tests organized by subsystem
│   ├── assets/
│   ├── components/
│   ├── data_handlers/
│   ├── display/
│   ├── entity/
│   ├── events/
│   ├── logic/
│   ├── scenes/
│   ├── systems/
│   └── user_interface/
├── integration/             # Integration tests (2+ components)
│   └── scene_change/
├── system/                  # End-to-end tests (future)
├── perf/                    # Performance tests (future)
│   └── benchmarks/
├── data/                    # Test data files
└── context/                 # Test utilities (TestContext, helpers)
```

**Note**: All unit tests have been migrated to the `unit/` directory and organized by subsystem. Integration tests are in `integration/` directory. System tests and performance tests directories are ready for future use.

### Test Classification

All tests are tagged with Catch2 tags to enable filtering by test type:

- **`[unit]`** - Unit tests: Test a single class/function in isolation with mocked dependencies
- **`[integration]`** - Integration tests: Test interactions between multiple components
- **`[system]`** - System tests: End-to-end tests through the entire system (future)
- **`[perf]`** - Performance tests: Benchmarks for critical code paths (future)
- **`[.visual]`** - Visual confirmation tests: Tests requiring user input or visual verification (hidden tag - excluded by default)

### Running Tests

**Build and run all tests:**
```bash
# Configure with preset
cmake --preset Debug

# Build
cmake --build --preset Debug

# Run all tests
ctest --preset Debug
```

**Run specific test types:**
```bash
# Run only unit tests
ctest --preset Debug -L unit

# Run only integration tests
ctest --preset Debug -L integration

# Run tests from a specific subsystem
ctest --preset Debug -R logic
```

**Run tests with verbose output:**
```bash
ctest --preset Debug --output-on-failure
```

**Visual confirmation tests:**

Some tests require user input or visual confirmation (they open SFML windows and wait for interaction). These tests are tagged with `[.visual]` (hidden tag), which means they are **automatically excluded** from default test runs.

```bash
# Run all tests (visual tests excluded by default due to hidden tag)
ctest --preset Debug

# Run ONLY visual tests (explicit request for manual verification)
ctest --preset Debug --test-args "[.visual]"
# or with test executable:
./build/Debug/tests/unit/logic/test_logic "[.visual]"

# Run all hidden tests
./build/Debug/tests/unit/logic/test_logic "[.]"
```

### Writing Tests

All tests should include appropriate Catch2 tags:

```cpp
TEST_CASE("ClassName constructor", "[unit][ClassName]") {
  // Unit test implementation
}

TEST_CASE("Feature workflow", "[integration][feature_name]") {
  // Integration test implementation
}

TEST_CASE("Visual confirmation test", "[unit][ClassName][.visual]") {
  // Test requiring user input or visual confirmation
  // These tests open windows and wait for user interaction
  // Hidden tag [.visual] means this test is excluded by default
}
```

### Test-Driven Development (TDD) in SteamRot

SteamRot follows a Test-Driven Development approach for all new features and components. This section documents how to implement TDD effectively in this repository.

#### TDD Workflow

The standard TDD workflow is:

1. **Write the test first** - Define expected behavior before implementing
2. **Run the test** - Verify it fails (red)
3. **Implement minimal code** - Make the test pass (green)
4. **Run all tests** - Ensure no regressions
5. **Refactor** - Improve code while keeping tests green
6. **Repeat** - Continue with next feature

#### Using Reusable Test Infrastructure

SteamRot provides reusable test infrastructure to reduce boilerplate and improve test quality:

##### TestScenarios Class

Provides pre-configured entity and component setups:

```cpp
#include "TestScenarios.h"

TEST_CASE("Logic processes entities correctly", "[unit][MyLogic]") {
  // Create 10 entities with UI components
  auto pool = steamrot::tests::TestScenarios::CreatePoolWithMultipleUIEntities(10);
  
  // Create populated archetype manager
  auto manager = steamrot::tests::TestScenarios::CreatePopulatedArchetypeManager(pool);
  
  // Test your logic...
}
```

**Available methods:**
- `CreateEmptyPool()` - Empty entity pool
- `CreatePoolWithNEntities(n)` - Pool with n default entities
- `CreatePoolWithArchetype(archetype_id, n)` - Pool with entities matching archetype
- `CreatePoolWithSingleUIEntity()` - One UI entity
- `CreatePoolWithMultipleUIEntities(n)` - n UI entities
- `CreatePoolWithSingleGrimoireEntity()` - One grimoire entity
- `CreatePopulatedArchetypeManager(pool)` - Configured archetype manager
- `ActivateComponent<T>(entity_id, pool)` - Activate specific component
- `DeactivateComponent<T>(entity_id, pool)` - Deactivate specific component

##### Test Assertions

Domain-specific assertions for clearer test intent:

```cpp
#include "test_assertions.h"

TEST_CASE("Component configuration", "[unit][MyComponent]") {
  MyComponent component;
  component.m_active = true;
  
  // Use domain-specific assertions
  steamrot::tests::AssertComponentActive(component, true, "MyTest");
  steamrot::tests::AssertArchetypeExists(archetype_id, manager, "MyTest");
  steamrot::tests::AssertEntityInArchetype(0, archetype_id, manager, "MyTest");
  steamrot::tests::AssertArchetypeEntityCount(archetype_id, 5, manager, "MyTest");
}
```

**Available assertions:**
- `AssertComponentActive(component, expected, test_name)` - Verify activation state
- `AssertEntityInArchetype(entity_id, archetype_id, manager, test_name)` - Verify membership
- `AssertArchetypeExists(archetype_id, manager, test_name)` - Verify archetype presence
- `AssertArchetypeEntityCount(archetype_id, count, manager, test_name)` - Verify entity count
- `AssertEntityPoolValid(pool, test_name)` - Verify pool consistency

##### ComponentTestMixin

Template for testing Component contract compliance:

```cpp
#include "component_test_mixin.h"

TEST_CASE("MyComponent follows Component contract", "[unit][MyComponent]") {
  // Run all standard component tests
  steamrot::tests::ComponentTestMixin<MyComponent>::RunAllTests();
  
  // Or run individual tests
  steamrot::tests::ComponentTestMixin<MyComponent>::TestDefaultConstruction();
  steamrot::tests::ComponentTestMixin<MyComponent>::TestComponentRegisterIndex();
  steamrot::tests::ComponentTestMixin<MyComponent>::TestActivation();
  steamrot::tests::ComponentTestMixin<MyComponent>::TestCopyConstruction();
  steamrot::tests::ComponentTestMixin<MyComponent>::TestCopyAssignment();
}
```

This ensures all components:
- Are default-constructible
- Return correct register index
- Can be activated/deactivated
- Are copyable (if needed)

##### LogicTestBase

Base class for Logic tests to reduce boilerplate:

```cpp
#include "logic_test_base.h"

class MyLogicTest : public steamrot::tests::LogicTestBase<MyLogic> {
protected:
  void TestProcessWithSingleEntity() override {
    SetUp();
    
    // Create entity with required components
    auto& game_context = test_context->GetGameContext();
    // ... setup entities ...
    
    logic = CreateLogic();
    REQUIRE_NOTHROW(logic->RunLogic());
    
    // Verify expected behavior
    // REQUIRE(...);
  }
};

TEST_CASE_METHOD(MyLogicTest, "MyLogic construction", "[unit][MyLogic]") {
  TestConstruction();
}

TEST_CASE_METHOD(MyLogicTest, "MyLogic with entities", "[unit][MyLogic]") {
  TestProcessWithSingleEntity();
  TestProcessWithMultipleEntities(5);
}
```

**Provided test methods:**
- `TestConstruction()` - Verify Logic instantiates without errors
- `TestProcessWithEmptyArchetype()` - Verify graceful handling of no entities
- `TestProcessWithSingleEntity()` - Override to test single entity processing
- `TestProcessWithMultipleEntities(n)` - Override to test batch processing
- `RunStandardTests()` - Run all standard tests at once

#### TDD Examples for Common Scenarios

##### Example 1: Adding a New Component (TDD)

**Step 1: Write the test first**

```cpp
// tests/components/CNewComponent.test.cpp
#include "CNewComponent.h"
#include "component_test_mixin.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CNewComponent follows Component contract", "[unit][CNewComponent]") {
  steamrot::tests::ComponentTestMixin<steamrot::CNewComponent>::RunAllTests();
}

TEST_CASE("CNewComponent has correct default values", "[unit][CNewComponent]") {
  steamrot::CNewComponent component;
  REQUIRE(component.m_data_field == "default");
  REQUIRE(component.m_value == 0);
}
```

**Step 2: Run the test (it will fail - component doesn't exist yet)**

```bash
cmake --build --preset Debug
# Compilation will fail - CNewComponent doesn't exist
```

**Step 3: Implement minimal Component**

```cpp
// src/components/CNewComponent.h
#pragma once
#include "Component.h"
#include <string>

namespace steamrot {
struct CNewComponent : public Component {
  CNewComponent() = default;
  std::string m_data_field{"default"};
  int m_value{0};
  size_t GetComponentRegisterIndex() const override;
};
}
```

**Step 4: Run tests - verify they pass**

```bash
cmake --build --preset Debug
ctest --preset Debug -R CNewComponent
```

**Step 5: Add FlatBuffers schema and configurator (with tests)**

Continue the TDD cycle for configuration...

##### Example 2: Adding a New Logic Class (TDD)

**Step 1: Write the test first**

```cpp
// tests/logic/NewLogic.test.cpp
#include "NewLogic.h"
#include "logic_test_base.h"
#include "TestScenarios.h"
#include <catch2/catch_test_macros.hpp>

class NewLogicTest : public steamrot::tests::LogicTestBase<steamrot::NewLogic> {
protected:
  void TestProcessWithSingleEntity() override {
    SetUp();
    
    // Create entity with required components
    // Use TestScenarios for setup
    
    logic = CreateLogic();
    REQUIRE_NOTHROW(logic->RunLogic());
    
    // Verify expected state changes
  }
};

TEST_CASE_METHOD(NewLogicTest, "NewLogic construction", "[unit][NewLogic]") {
  TestConstruction();
}

TEST_CASE_METHOD(NewLogicTest, "NewLogic processes entities", "[unit][NewLogic]") {
  TestProcessWithSingleEntity();
}
```

**Step 2: Run test (will fail - Logic doesn't exist)**

**Step 3: Implement minimal Logic class**

```cpp
// src/logic/NewLogic.h & .cpp
// Implement ProcessLogic() with minimal behavior to make test pass
```

**Step 4: Run tests - verify they pass**

**Step 5: Add to LogicFactory (with tests)**

Update LogicFactory tests first, then implementation.

#### TDD Best Practices for SteamRot

1. **Always write tests first** - Define behavior before implementation
2. **Use reusable infrastructure** - Leverage TestScenarios, assertions, mixins
3. **Test one thing at a time** - Each TEST_CASE should verify a single behavior
4. **Use descriptive names** - Test names should describe what is being tested
5. **Tag appropriately** - Use `[unit]`, `[integration]`, or `[system]` tags
6. **Keep tests fast** - Unit tests should run in milliseconds
7. **Avoid test interdependence** - Each test should be independent
8. **Test edge cases** - Include tests for boundary conditions and error cases
9. **Refactor tests too** - Keep test code clean and maintainable
10. **Run tests frequently** - After every change, run relevant tests

#### Testing Checklist

Before committing code, ensure:

- [ ] All new code has tests written first (TDD)
- [ ] Tests use reusable infrastructure where applicable
- [ ] Tests are tagged with `[unit]`, `[integration]`, or `[system]`
- [ ] All tests pass: `ctest --preset Debug`
- [ ] Test names clearly describe what is being tested
- [ ] Tests are independent and can run in any order
- [ ] Edge cases and error conditions are tested
- [ ] Test code is clean and well-organized

### user_interface

#### Testing UI Elements

The TestUIElementDataProvider.h file defines a class with static methods that
provide some specific methods. (This could be updated in the future to provide
randomly generated data so we can provide a wider range of test cases).

So any new UIElement type will need to have a static method added to this class
to provide data.

The ui_element_factory_helpers.h defines a series of functions that will compare
the flatbuffers data to the produced UIElement (from the UIElementFactory
configure).

Any new UIElement type will need to have a new test function added to this file.

The ui_element_factory_helpers.cpp file also contains a full template
specialization for each flatbuffers data type to UIElement type. Any new
UIElement type will need to have a new template specialization added to this
file. These templates are to allow for testing a nested strcture of UIElements.
