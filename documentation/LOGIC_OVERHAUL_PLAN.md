# Logic Class Overhaul Plan

## Overview

This document provides a comprehensive plan for overhauling the Logic class architecture and integrating it with the test harness system for data-driven testing. This plan separates Phase 2.3 from the Test Harness Adoption Plan to focus specifically on Logic class improvements, free function organization, and testing strategies.

**This is a planning document only - no implementation will occur as part of this phase.**

## Current State Analysis

### Existing Logic Class Structure

The current `Logic` base class (`src/logic/Logic.h`):
- Abstract base class with pure virtual `ProcessLogic()` method
- Stores `SceneContext` member (`m_scene_context`)
- Provides `RunLogic()` interface method
- Very minimal - mostly a contract for derived classes

### Current Logic Implementations

Example implementations (e.g., `UIActionLogic`, `UICollisionLogic`, `UIRenderLogic`):
- Each has a private `ProcessLogic()` override
- Manually access archetypes via `m_scene_context.archetypes.find(archetype_id)`
- Manually iterate through entity IDs in archetype
- Manually get components via `entity::memory::GetComponent<T>(entity_id, m_scene_context.scene_entities)`
- Mix archetype access pattern with actual logic processing

### Current Free Function Organization

Free functions exist in two patterns:

**Pattern 1: Separate namespace header** (e.g., `logic_collision.h`):
- Functions in nested namespaces: `steamrot::logic::collision::`
- Separate .h/.cpp files
- Domain-focused (collision, ui helpers)

**Pattern 2: Class header with free functions** (e.g., `UIActionLogic.h`):
- Free functions declared after class in same header
- Functions in top-level `steamrot::` namespace
- Closely related to specific Logic class

### Current Test Approach

Logic tests (e.g., `UIActionLogic.test.cpp`):
- Manually create `TestFixture`
- Manually instantiate Logic classes
- Test Logic class as a whole (not individual functions)
- Some tests for individual free functions separately
- No data-driven approach yet

## Problems to Address

### 1. Repetitive Archetype Access Pattern

**Problem:** Every Logic class repeats the same boilerplate:
```cpp
ArchetypeID archetype_id = GenerateArchetypeIDfromTypes<CComponent>();
const auto it = m_scene_context.archetypes.find(archetype_id);
if (it != m_scene_context.archetypes.end()) {
  const Archetype &archetype = it->second;
  for (size_t entity_id : archetype) {
    CComponent &comp = entity::memory::GetComponent<CComponent>(
        entity_id, m_scene_context.scene_entities);
    // ... actual logic ...
  }
}
```

**Impact:**
- Increases code duplication
- Makes tests verbose
- Obscures actual logic intent
- Error-prone (easy to forget null checks)

### 2. Unclear Separation Between Infrastructure and Logic

**Problem:** Logic classes mix:
- Archetype lookup (infrastructure)
- Entity iteration (infrastructure)
- Component retrieval (infrastructure)
- Actual business logic (core purpose)

**Impact:**
- Hard to test individual pieces
- Difficult to reuse logic patterns
- Unclear what is testable in isolation

### 3. Inconsistent Free Function Organization

**Problem:** Free functions sometimes in:
- Separate namespaced headers (`logic::collision::`)
- Same header as class (`UIActionLogic.h`)
- Different naming conventions

**Impact:**
- Unclear where to put new free functions
- Inconsistent namespace usage
- Difficult to discover related functions

### 4. Limited Testing Granularity

**Problem:** Current tests primarily test:
- Entire Logic class execution
- Some individual free functions
- Not the infrastructure layer (archetype access)
- Not the composition of functions into Logic class

**Impact:**
- Large test surface area per test
- Difficult to isolate failures
- Hard to use data-driven approach
- Cannot test logic incrementally

## Proposed Improvements

### 1. Add Archetype Access Wrapper Functions

**Goal:** Eliminate repetitive archetype lookup and iteration pattern.

#### Proposed API Design

Add to `Logic` base class (`src/logic/Logic.h`):

```cpp
protected:
  /////////////////////////////////////////////////
  /// @brief Get archetype for given component types
  ///
  /// Convenience wrapper for archetype lookup. Returns std::nullopt
  /// if archetype doesn't exist.
  ///
  /// @tparam Components Component types to generate archetype from
  /// @return Optional archetype reference
  /////////////////////////////////////////////////
  template <typename... Components>
  std::optional<std::reference_wrapper<const Archetype>>
  GetArchetype() const {
    ArchetypeID archetype_id = GenerateArchetypeIDfromTypes<Components...>();
    const auto it = m_scene_context.archetypes.find(archetype_id);
    
    if (it != m_scene_context.archetypes.end()) {
      return std::ref(it->second);
    }
    return std::nullopt;
  }

  /////////////////////////////////////////////////
  /// @brief Execute function for each entity in archetype
  ///
  /// Convenience wrapper for iterating entities in archetype and
  /// retrieving components. Automatically handles archetype lookup
  /// and component access.
  ///
  /// @tparam Components Component types for archetype
  /// @tparam Func Function type (lambda or function pointer)
  /// @param func Function to execute for each entity (receives entity_id and component references)
  /////////////////////////////////////////////////
  template <typename... Components, typename Func>
  void ForEachEntity(Func func) {
    auto archetype_opt = GetArchetype<Components...>();
    if (!archetype_opt.has_value()) {
      return;
    }
    
    const Archetype &archetype = archetype_opt.value().get();
    for (size_t entity_id : archetype) {
      auto components = std::make_tuple(
          std::ref(entity::memory::GetComponent<Components>(
              entity_id, m_scene_context.scene_entities))...);
      
      std::apply([&](auto&... comps) {
        func(entity_id, comps...);
      }, components);
    }
  }
```

#### Benefits

- **Reduces boilerplate:** Eliminates 6+ lines per archetype access
- **Consistent error handling:** Uniform null-checking
- **Type-safe:** Template ensures component types match
- **Readable:** Intent is clear (`ForEachEntity<CUserInterface>`)
- **Testable:** Wrapper functions can be tested independently

#### Example Usage

**Before:**
```cpp
void UIActionLogic::ProcessLogic() {
  ArchetypeID archetype_id = GenerateArchetypeIDfromTypes<CUserInterface>();
  const auto it = m_scene_context.archetypes.find(archetype_id);
  if (it != m_scene_context.archetypes.end()) {
    const Archetype &archetype = it->second;
    for (size_t entity_id : archetype) {
      CUserInterface &ui_component = entity::memory::GetComponent<CUserInterface>(
          entity_id, m_scene_context.scene_entities);
      ProcessNestedUIActionsAndEvents(*ui_component.m_root_element,
                                      m_scene_context.event_handler,
                                      m_scene_context);
    }
  }
}
```

**After:**
```cpp
void UIActionLogic::ProcessLogic() {
  ForEachEntity<CUserInterface>([&](size_t entity_id, CUserInterface &ui_component) {
    ProcessNestedUIActionsAndEvents(*ui_component.m_root_element,
                                    m_scene_context.event_handler,
                                    m_scene_context);
  });
}
```

### 2. Establish Free Function vs Class Method Guidelines

**Goal:** Clear, consistent rationale for organizing logic code.

#### Decision Framework

```
Is this code...
├─ Pure infrastructure (archetype access, iteration)?
│  └─ YES → Protected method on Logic base class
│
├─ Specific to one Logic class AND tightly coupled to m_scene_context?
│  └─ YES → Private helper method in Logic derived class
│
├─ Reusable logic that operates on components/entities?
│  ├─ Used by single Logic class?
│  │  └─ YES → Free function in same file as Logic class (bottom of .h/.cpp)
│  └─ Used by multiple Logic classes?
│     └─ YES → Free function in separate header (e.g., logic_ui.h, logic_collision.h)
│
└─ Domain-specific utility (UI, collision, data extraction)?
   └─ YES → Free function in domain namespace (e.g., logic::ui::, logic::collision::)
```

#### Rationale: Free Functions

**Advantages:**
- **Testability:** Can be tested in isolation without Logic class
- **Reusability:** Can be called from multiple Logic classes or contexts
- **Composability:** Can be combined to build complex Logic
- **Data-driven testing:** Perfect for test harness simulations
- **Clear dependencies:** Parameters make dependencies explicit

**When to use:**
- Logic that operates on components/UI elements directly
- Operations that don't need full SceneContext
- Functions that could be reused in different contexts
- Logic suitable for incremental testing

#### Rationale: Class Methods

**Advantages:**
- **Access to context:** Direct access to `m_scene_context`
- **Encapsulation:** Private helpers for internal organization
- **Type safety:** Strong coupling when appropriate

**When to use:**
- Infrastructure code (archetype wrappers - already on base class)
- Private helpers that use multiple SceneContext members
- Logic tightly coupled to specific Logic class contract

### 3. Header File Organization Guidelines

**Goal:** Consistent, discoverable organization of free functions.

#### Pattern 1: Free Functions in Logic Class Header (Recommended for Class-Specific Functions)

**Use when:** Free function is primarily used by one Logic class

**Example:** `UIActionLogic.h`
```cpp
#pragma once

namespace steamrot {

// ========================================
// Logic Class Declaration
// ========================================

class UIActionLogic : public Logic {
private:
  void ProcessLogic() override;
public:
  UIActionLogic(const SceneContext scene_context);
};

// ========================================
// Free Function Declarations
// ========================================

/////////////////////////////////////////////////
/// @brief Process UI actions for an element
/////////////////////////////////////////////////
void ProcessUIActionsAndEvents(UIElement &ui_element,
                               EventHandler &event_handler,
                               const SceneContext &scene_context);

/////////////////////////////////////////////////
/// @brief Process nested UI actions recursively
/////////////////////////////////////////////////
void ProcessNestedUIActionsAndEvents(UIElement &ui_element,
                                     EventHandler &event_handler,
                                     const SceneContext &scene_context);

} // namespace steamrot
```

**Benefits:**
- Easy to find functions related to a Logic class
- Clear association between class and functions
- Single file for all class-related code

**Implementation:**
- `.cpp` file contains both class methods and free function implementations
- Free functions at bottom of file
- Visual separators between sections

#### Pattern 2: Free Functions in Separate Domain Header (Recommended for Shared Utilities)

**Use when:** Free functions are used by multiple Logic classes

**Example:** `logic_ui.h`
```cpp
#pragma once

namespace steamrot::logic::ui {

/////////////////////////////////////////////////
/// @brief Get all fragment names from grimoire
/////////////////////////////////////////////////
std::vector<std::string>
GetAllFragmentNames(const CGrimoireMachina &grimoire_machina);

/////////////////////////////////////////////////
/// @brief Get all joint names from grimoire
/////////////////////////////////////////////////
std::vector<std::string>
GetAllJointNames(const CGrimoireMachina &grimoire_machina);

} // namespace steamrot::logic::ui
```

**Benefits:**
- Clear domain organization
- Easy to discover related utilities
- Namespace prevents naming conflicts
- Reusable across multiple Logic classes

**Naming conventions:**
- File: `logic_<domain>.h` (snake_case)
- Namespace: `steamrot::logic::<domain>`
- Functions: `PascalCase` or `camelCase` (match project style)

### 4. Improve Testing Granularity

**Goal:** Enable incremental, data-driven testing of Logic components.

#### Testing Strategy: Bottom-Up Composition

```
Level 1: Free Functions (Pure Logic)
  ↓ Test individually with simple inputs
Level 2: Free Function Combinations  
  ↓ Test composition patterns
Level 3: Logic Class (Orchestration)
  ↓ Test full Logic execution with real SceneContext
```

#### Test Organization

```
tests/unit/logic/
├── data/                                    # Test data for data-driven tests
│   ├── function_tests/                     # Free function test data
│   │   ├── process_button_actions.test_data.json
│   │   ├── process_nested_ui_actions.test_data.json
│   │   └── check_mouse_collision.test_data.json
│   ├── composition_tests/                  # Multi-function test data
│   │   ├── collision_then_action.test_data.json
│   │   └── full_ui_workflow.test_data.json
│   └── logic_class_tests/                  # Full Logic class test data
│       ├── ui_action_logic_full.test_data.json
│       └── ui_collision_logic_full.test_data.json
├── free_functions/                          # Free function unit tests
│   ├── process_button_actions.test.cpp
│   ├── process_nested_ui_actions.test.cpp
│   └── collision_functions.test.cpp
└── logic_classes/                           # Logic class tests
    ├── UIActionLogic.test.cpp
    ├── UICollisionLogic.test.cpp
    └── UIRenderLogic.test.cpp
```

### 5. Test Harness Integration for Data-Driven Logic Tests

**Goal:** Leverage test harness simulation system for data-driven Logic testing.

#### Current Simulation Support (Already Implemented)

The test harness already supports:
- **SimulationType:** Action, Movement, Render, Collision
- **ExecutionMode:** Function, LogicClass
- **Simulation steps:** Sequential execution of Logic classes and free functions

**Example simulation data:**
```json
{
  "simulation_data": {
    "description": "Test button click workflow",
    "steps": [
      {
        "simulation_type": "Collision",
        "execution_mode": "Function",
        "function_type": "CheckMouseOverUIElement",
        "description": "Check mouse collision"
      },
      {
        "simulation_type": "Action",
        "execution_mode": "Function",
        "function_type": "ProcessButtonElementActions",
        "description": "Process button click"
      }
    ]
  }
}
```

#### Proposed Enhancements

##### 1. Add More Free Functions to Simulation System

**Current functions supported:**
- `ProcessUIActionsAndEvents`
- `ProcessNestedUIActionsAndEvents`
- `ProcessButtonElementActions`
- `ProcessDropDownListElementActions`

**Functions to add:**
- Collision functions (`CheckMouseOverUIElement`, `CheckMouseOverNestedUIElement`)
- UI helper functions (`GetAllFragmentNames`, `GetAllJointNames`)
- New free functions as they're created

**Implementation:** Update `simulation_runner.cpp` dispatcher for new functions.

##### 2. Free Function-Only Test Data Files

**Goal:** Test free functions in isolation before composing into Logic classes.

**Example:** `process_button_actions.test_data.json`
```json
{
  "metadata": {
    "test_name": "process_button_actions_mouse_over",
    "description": "Test ProcessButtonElementActions with mouse over",
    "tags": ["unit", "function", "button"],
    "expected_to_pass": true,
    "version": 1
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 2,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "button_ui",
          "start_visible": true,
          "root_ui_element": {
            "button_data": {
              "label": "Test Button",
              "is_mouse_over": true,
              "position": { "x": 100, "y": 100 },
              "size": { "x": 100, "y": 50 }
            }
          }
        }
      }
    ]
  },
  "simulation_data": {
    "description": "Call ProcessButtonElementActions directly",
    "steps": [
      {
        "simulation_type": "Action",
        "execution_mode": "Function",
        "function_type": "ProcessButtonElementActions",
        "description": "Process button with mouse over"
      }
    ]
  },
  "expected_entity_collection": {
    "entity_memory_pool_size": 2,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "button_ui",
          "start_visible": true
        }
      }
    ]
  }
}
```

**Benefits:**
- Tests single function in isolation
- Clear expected behavior
- Fast to execute
- Easy to debug failures

##### 3. Composition Test Data Files

**Goal:** Test combinations of free functions (building blocks for Logic classes).

**Example:** `collision_then_action.test_data.json`
```json
{
  "metadata": {
    "test_name": "collision_then_action_workflow",
    "description": "Test collision detection followed by action processing",
    "tags": ["unit", "composition", "workflow"],
    "expected_to_pass": true,
    "version": 1
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 2,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "button_ui",
          "start_visible": true,
          "root_ui_element": {
            "button_data": {
              "label": "Click Me",
              "position": { "x": 100, "y": 100 },
              "size": { "x": 100, "y": 50 }
            }
          }
        }
      }
    ]
  },
  "input_sequence": {
    "inputs": [
      {
        "input_type": "MouseMove",
        "input_data": { "position": { "x": 150, "y": 125 } },
        "tick": 0
      }
    ]
  },
  "simulation_data": {
    "description": "Collision detection, then action processing",
    "steps": [
      {
        "simulation_type": "Collision",
        "execution_mode": "Function",
        "function_type": "CheckMouseOverUIElement",
        "description": "Detect mouse collision"
      },
      {
        "simulation_type": "Action",
        "execution_mode": "Function",
        "function_type": "ProcessButtonElementActions",
        "description": "Process button action"
      }
    ]
  },
  "expected_entity_collection": {
    "entity_memory_pool_size": 2,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "root_ui_element": {
            "button_data": {
              "is_mouse_over": true
            }
          }
        }
      }
    ]
  }
}
```

**Benefits:**
- Tests function composition
- Validates function interfaces work together
- Builds confidence before full Logic class testing
- Mirrors actual Logic class structure

##### 4. Full Logic Class Test Data Files

**Goal:** Test complete Logic class with data-driven approach.

**Example:** `ui_action_logic_full.test_data.json`
```json
{
  "metadata": {
    "test_name": "ui_action_logic_button_click",
    "description": "Full UIActionLogic with button click",
    "tags": ["unit", "logic", "UIActionLogic"],
    "expected_to_pass": true,
    "version": 1
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 5,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "main_menu",
          "start_visible": true,
          "root_ui_element": {
            "button_data": {
              "label": "Start Game",
              "is_mouse_over": true
            }
          }
        }
      }
    ]
  },
  "simulation_data": {
    "description": "Execute UIActionLogic",
    "steps": [
      {
        "simulation_type": "Action",
        "execution_mode": "LogicClass",
        "logic_class_type": "UIActionLogic",
        "description": "Run full UIActionLogic"
      }
    ]
  },
  "expected_entity_collection": {
    "entity_memory_pool_size": 5,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "main_menu",
          "start_visible": true
        }
      }
    ]
  }
}
```

**Benefits:**
- Tests Logic class as used in production
- Validates integration of all functions
- Data-driven parameterization for edge cases
- Regression testing

## Implementation Roadmap

### Phase 1: Foundation (Weeks 1-2)

**Goal:** Establish infrastructure improvements

1. **Add archetype wrapper functions to Logic base class**
   - Implement `GetArchetype<Components...>()`
   - Implement `ForEachEntity<Components...>(Func)`
   - Add unit tests for wrapper functions
   - Document usage patterns

2. **Establish and document guidelines**
   - Document decision framework for free functions vs methods
   - Document header organization patterns
   - Create examples for each pattern
   - Update project README

3. **Extend simulation runner**
   - Add collision free functions to dispatcher
   - Add UI helper functions to dispatcher
   - Test new function support

### Phase 2: Refactor Existing Logic Classes (Weeks 3-4)

**Goal:** Apply improvements to existing Logic classes

1. **Refactor UIActionLogic**
   - Replace manual archetype access with `ForEachEntity`
   - Verify free functions are appropriately organized
   - Update tests

2. **Refactor UICollisionLogic**
   - Replace manual archetype access with `ForEachEntity`
   - Move collision functions to `logic_collision.h` if needed
   - Update tests

3. **Refactor UIRenderLogic**
   - Replace manual archetype access with `ForEachEntity`
   - Update tests

4. **Refactor UIStateLogic**
   - Replace manual archetype access with `ForEachEntity`
   - Update tests

5. **Refactor CraftingRenderLogic**
   - Replace manual archetype access with `ForEachEntity`
   - Update tests

### Phase 3: Create Data-Driven Tests (Weeks 5-7)

**Goal:** Establish data-driven testing for all Logic

#### Week 5: Free Function Tests

1. **Create test data for individual free functions**
   - ProcessButtonElementActions
   - ProcessDropDownListElementActions
   - CheckMouseOverUIElement
   - CheckMouseOverNestedUIElement
   - UI helper functions

2. **Write test cases using test harness**
   - Load test data configs
   - Use simulation system to execute functions
   - Verify expected outcomes

3. **Document patterns for future functions**

#### Week 6: Composition Tests

1. **Create test data for function compositions**
   - Collision → Action workflows
   - Render → State workflows
   - Complex multi-step scenarios

2. **Write composition test cases**
   - Test multiple functions in sequence
   - Verify state transitions between steps
   - Test edge cases

#### Week 7: Full Logic Class Tests

1. **Create test data for Logic classes**
   - UIActionLogic with various scenarios
   - UICollisionLogic with various scenarios
   - UIRenderLogic with various scenarios
   - UIStateLogic with various scenarios
   - CraftingRenderLogic with various scenarios

2. **Migrate existing tests to data-driven approach**
   - Convert manual tests to test data files
   - Keep old tests initially for validation
   - Remove old tests after validation

3. **Document data-driven testing workflow**

### Phase 4: Validation and Documentation (Week 8)

**Goal:** Ensure quality and document learnings

1. **Validate improvements**
   - All existing tests pass
   - New tests provide better coverage
   - Code is more readable and maintainable
   - Test execution time acceptable

2. **Complete documentation**
   - Update README with Logic testing patterns
   - Document wrapper function usage
   - Document free function guidelines
   - Update TEST_HARNESS_ADOPTION_PLAN.md

3. **Training and examples**
   - Create example Logic class following patterns
   - Create example test suite for new Logic
   - Review with team

## Success Metrics

### Code Quality Metrics

- **Reduced boilerplate:** 50%+ reduction in archetype access code
- **Function extraction:** 80%+ of logic in testable free functions
- **Consistency:** 100% of Logic classes use wrapper functions
- **Organization:** Clear pattern for all new Logic classes

### Test Quality Metrics

- **Test coverage:** 90%+ coverage for Logic classes
- **Test granularity:** 3+ test levels (function, composition, class)
- **Data-driven tests:** 80%+ of Logic tests data-driven
- **Test execution time:** < 10 seconds for all Logic tests

### Documentation Metrics

- **Guidelines documented:** 100% of patterns documented
- **Examples provided:** 2+ examples per pattern
- **Team adoption:** 100% of new Logic follows guidelines

## Benefits Summary

### For Developers

- **Less boilerplate:** Focus on logic, not infrastructure
- **Clear patterns:** Know exactly where to put code
- **Easier testing:** Test incrementally, not all-at-once
- **Better debugging:** Isolated functions easier to debug

### For Testing

- **Data-driven:** Change test cases without code changes
- **Incremental:** Test building blocks, then compositions
- **Fast execution:** Free function tests are very fast
- **Better coverage:** Granular tests catch more issues

### For Maintenance

- **Consistent code:** Easy to navigate and understand
- **Discoverable:** Clear organization of related functions
- **Reusable:** Functions can be used in new contexts
- **Documented:** Patterns and rationale clear

## Risks and Mitigations

### Risk 1: Wrapper Function Performance

**Risk:** Template wrapper functions may have overhead

**Mitigation:**
- Wrappers are inline-able by compiler
- Modern compilers optimize template code well
- Measure performance after implementation
- Optimize only if proven necessary

### Risk 2: Over-Extraction of Functions

**Risk:** Too many free functions, code becomes fragmented

**Mitigation:**
- Follow decision framework consistently
- Only extract when there's clear reuse or testability benefit
- Keep related functions together in same file
- Regular code reviews to validate decisions

### Risk 3: Learning Curve

**Risk:** Team needs to learn new patterns

**Mitigation:**
- Comprehensive documentation with examples
- Refactor existing code as reference
- Pair programming for first implementations
- Regular team reviews

### Risk 4: Test Data Maintenance

**Risk:** Many test data files to maintain

**Mitigation:**
- Clear naming conventions
- Organize by function/feature
- Validate test data files in CI
- Document test data schema changes

## Future Enhancements

### Potential Improvements (Post-Initial Implementation)

1. **Logic Composition Builder**
   - Fluent API for composing Logic from functions
   - Type-safe function chaining
   - Compile-time validation

2. **Archetype Query DSL**
   - More expressive archetype queries
   - Multiple archetype iteration in single loop
   - Filtered entity iteration

3. **Test Data Generators**
   - Generate test data from code annotations
   - Automatically create edge cases
   - Mutation testing for Logic

4. **Performance Profiling Integration**
   - Measure free function performance
   - Compare Logic class performance
   - Identify optimization opportunities

## Conclusion

This Logic overhaul plan provides a comprehensive approach to:
1. **Improving** the Logic class with infrastructure wrappers
2. **Organizing** free functions consistently and discoverably
3. **Testing** incrementally from functions to compositions to classes
4. **Integrating** with the test harness for data-driven testing

The plan is structured for incremental adoption over 8 weeks, with clear success metrics and risk mitigations. The benefits include reduced boilerplate, better testability, clearer organization, and improved maintainability.

**This is a planning document - implementation will follow team review and approval.**

## Appendix A: Decision Tree for Logic Code Organization

```
New logic code to write...

1. Is it pure infrastructure (archetype access, iteration)?
   YES → Add to Logic base class as protected template method
   NO  → Continue to Q2

2. Does it need full SceneContext access?
   YES → Continue to Q3
   NO  → Continue to Q4

3. Is it specific to one Logic class?
   YES → Private helper method in that Logic class
   NO  → Protected helper method in Logic base class

4. Does it operate on components/UI elements?
   YES → Continue to Q5
   NO  → Continue to Q6

5. Will it be used by one Logic class only?
   YES → Free function in Logic class header (bottom)
   NO  → Free function in separate domain header

6. Is it a domain utility (UI, collision, data)?
   YES → Free function in domain namespace (logic::domain::)
   NO  → Reconsider design - may not belong in Logic layer
```

## Appendix B: Example Refactoring

### Before: UIActionLogic without Wrappers

```cpp
// UIActionLogic.cpp
void UIActionLogic::ProcessLogic() {
  // 7 lines of boilerplate
  ArchetypeID archetype_id = GenerateArchetypeIDfromTypes<CUserInterface>();
  const auto it = m_scene_context.archetypes.find(archetype_id);
  if (it != m_scene_context.archetypes.end()) {
    const Archetype &archetype = it->second;
    for (size_t entity_id : archetype) {
      CUserInterface &ui_component = entity::memory::GetComponent<CUserInterface>(
          entity_id, m_scene_context.scene_entities);
      
      // 3 lines of actual logic
      ProcessNestedUIActionsAndEvents(*ui_component.m_root_element,
                                      m_scene_context.event_handler,
                                      m_scene_context);
    }
  }
}
```

**Lines:** 10 total (7 boilerplate, 3 logic)

### After: UIActionLogic with Wrappers

```cpp
// UIActionLogic.cpp
void UIActionLogic::ProcessLogic() {
  // 3 lines total - all logic
  ForEachEntity<CUserInterface>([&](size_t entity_id, CUserInterface &ui_component) {
    ProcessNestedUIActionsAndEvents(*ui_component.m_root_element,
                                    m_scene_context.event_handler,
                                    m_scene_context);
  });
}
```

**Lines:** 3 total (0 boilerplate, 3 logic)

**Improvement:** 70% reduction in code, 100% reduction in boilerplate

## Appendix C: Example Test Progression

### Level 1: Free Function Test

```cpp
TEST_CASE("ProcessButtonElementActions adds event when mouse over", 
          "[unit][function][button]") {
  auto configs = steamrot::tests::load_test_data_configs("free_functions");
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // Test harness executes function via simulation
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

### Level 2: Composition Test

```cpp
TEST_CASE("Collision then action workflow processes button correctly",
          "[unit][composition][workflow]") {
  auto configs = steamrot::tests::load_test_data_configs("composition_tests");
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // Test harness executes: CheckMouseOverUIElement -> ProcessButtonElementActions
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

### Level 3: Logic Class Test

```cpp
TEST_CASE("UIActionLogic processes button clicks correctly",
          "[unit][logic][UIActionLogic]") {
  auto configs = steamrot::tests::load_test_data_configs("logic_class_tests");
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // Test harness executes full UIActionLogic class
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

**Progression:** Function → Composition → Class (building up confidence)
