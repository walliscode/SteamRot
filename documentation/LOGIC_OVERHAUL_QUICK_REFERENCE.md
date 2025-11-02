# Logic Overhaul Quick Reference

**See `LOGIC_OVERHAUL_PLAN.md` for the complete plan.**

## When to Use What

### Quick Decision Tree

```
New logic code to write...

1. Infrastructure (archetype access, iteration)?
   → Add to Logic base class as protected template method

2. Needs full SceneContext access?
   → If specific to one Logic class: Private helper method
   → If shared across classes: Protected Logic base method

3. Operates on components/UI elements?
   → If used by one Logic class: Free function in class header
   → If used by multiple classes: Free function in domain header
   
4. Domain utility (UI, collision, data)?
   → Free function in domain namespace (logic::domain::)
```

## Wrapper Functions (Planned)

### GetEntitiesWithExactComponents

Returns entity IDs for entities with exactly the specified components.

```cpp
// In any Logic class
auto entity_ids = GetEntitiesWithExactComponents<CUserInterface>();
if (entity_ids.empty()) {
  return;  // No entities found
}

for (size_t entity_id : entity_ids) {
  CUserInterface &ui = entity::memory::GetComponent<CUserInterface>(
      entity_id, m_scene_context.scene_entities);
  // Process component
}
```

### GetEntitiesWithComponents

Returns entity IDs for entities with at least the specified components (may have additional).

```cpp
// In any Logic class
auto entity_ids = GetEntitiesWithComponents<CUserInterface>();
// Returns all entities that have CUserInterface, regardless of other components
```

### ForEachEntityExact

Iterate entities with exact component match.

```cpp
// Single function
ForEachEntityExact<CUserInterface>([&](size_t entity_id, CUserInterface &ui) {
  ProcessUIElement(*ui.m_root_element);
});

// Multiple components
ForEachEntityExact<CUserInterface, CGrimoireMachina>(
    [&](size_t entity_id, CUserInterface &ui, CGrimoireMachina &grimoire) {
  UpdateGrimoireUI(ui, grimoire);
});

// Multiple functions (applied in sequence)
ForEachEntityExact<CUserInterface>(
  [&](size_t id, CUserInterface &ui) { ui.m_frame_count++; },
  [&](size_t id, CUserInterface &ui) { ProcessUILogic(ui); },
  [&](size_t id, CUserInterface &ui) { LogActivity(id); }
);
```

### ForEachEntityWith

Iterate entities that contain the specified components (flexible matching).

```cpp
// Process all entities with CUserInterface (regardless of other components)
ForEachEntityWith<CUserInterface>([&](size_t entity_id, CUserInterface &ui) {
  ProcessAnyUIElement(ui);
});
```

## Header Organization Patterns

### Pattern 1: Class-Specific Free Functions

Use when function is primarily used by one Logic class.

**File:** `UIActionLogic.h`
```cpp
namespace steamrot {

class UIActionLogic : public Logic {
  // ... class definition
};

// ========================================
// Free Function Declarations
// ========================================

void ProcessUIActionsAndEvents(UIElement &ui_element,
                               EventHandler &event_handler,
                               const SceneContext &scene_context);

} // namespace steamrot
```

### Pattern 2: Shared Domain Functions

Use when function is used by multiple Logic classes.

**File:** `logic_ui.h`
```cpp
namespace steamrot::logic::ui {

std::vector<std::string>
GetAllFragmentNames(const CGrimoireMachina &grimoire_machina);

std::vector<std::string>
GetAllJointNames(const CGrimoireMachina &grimoire_machina);

} // namespace steamrot::logic::ui
```

## Testing Strategy

### Level 1: Free Function Tests

**Test Data:** `data/function_tests/process_button_actions.test_data.json`
```json
{
  "metadata": {
    "test_name": "process_button_actions_mouse_over",
    "tags": ["unit", "function", "button"]
  },
  "simulation_data": {
    "steps": [
      {
        "simulation_type": "Action",
        "execution_mode": "Function",
        "function_type": "ProcessButtonElementActions"
      }
    ]
  }
}
```

**Test Code:**
```cpp
TEST_CASE("ProcessButtonElementActions tests", "[unit][function]") {
  auto configs = steamrot::tests::load_test_data_configs("free_functions");
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

### Level 2: Composition Tests

**Test Data:** `data/composition_tests/collision_then_action.test_data.json`
```json
{
  "metadata": {
    "test_name": "collision_then_action_workflow",
    "tags": ["unit", "composition"]
  },
  "simulation_data": {
    "steps": [
      {
        "simulation_type": "Collision",
        "execution_mode": "Function",
        "function_type": "CheckMouseOverUIElement"
      },
      {
        "simulation_type": "Action",
        "execution_mode": "Function",
        "function_type": "ProcessButtonElementActions"
      }
    ]
  }
}
```

### Level 3: Logic Class Tests

**Test Data:** `data/logic_class_tests/ui_action_logic_full.test_data.json`
```json
{
  "metadata": {
    "test_name": "ui_action_logic_button_click",
    "tags": ["unit", "logic", "UIActionLogic"]
  },
  "simulation_data": {
    "steps": [
      {
        "simulation_type": "Action",
        "execution_mode": "LogicClass",
        "logic_class_type": "UIActionLogic"
      }
    ]
  }
}
```

## Before/After Example

### Before (Current)

```cpp
void UIActionLogic::ProcessLogic() {
  // 7 lines of boilerplate
  ArchetypeID archetype_id = GenerateArchetypeIDfromTypes<CUserInterface>();
  const auto it = m_scene_context.archetypes.find(archetype_id);
  if (it != m_scene_context.archetypes.end()) {
    const Archetype &archetype = it->second;
    for (size_t entity_id : archetype) {
      CUserInterface &ui = entity::memory::GetComponent<CUserInterface>(
          entity_id, m_scene_context.scene_entities);
      
      // 3 lines of actual logic
      ProcessNestedUIActionsAndEvents(*ui.m_root_element,
                                      m_scene_context.event_handler,
                                      m_scene_context);
    }
  }
}
```

### After (Planned)

```cpp
void UIActionLogic::ProcessLogic() {
  // 3 lines total - all logic, no boilerplate
  ForEachEntityExact<CUserInterface>([&](size_t entity_id, CUserInterface &ui) {
    ProcessNestedUIActionsAndEvents(*ui.m_root_element,
                                    m_scene_context.event_handler,
                                    m_scene_context);
  });
}
```

**Improvement:** 70% reduction in code, 100% reduction in boilerplate

**Key Benefits:**
- Returns empty vector instead of optional reference wrapper (simpler)
- Maintains memory contiguity (no tuple creation overhead)
- Supports multiple functions via fold expressions
- Flexible querying (exact match or "contains" semantics)

## Benefits Summary

- **Reduced boilerplate:** 50%+ reduction in archetype access code
- **Better testability:** 3-level testing strategy (function → composition → class)
- **Clear organization:** Consistent patterns for all Logic code
- **Data-driven tests:** 80%+ of Logic tests data-driven
- **Maintainability:** Clear guidelines and examples

## Implementation Phases

1. **Phase 1 (Weeks 1-2):** Foundation - Add wrapper functions, establish guidelines
2. **Phase 2 (Weeks 3-4):** Refactor - Apply to existing Logic classes
3. **Phase 3 (Weeks 5-7):** Testing - Create data-driven test suites
4. **Phase 4 (Week 8):** Validation - Ensure quality, complete documentation

## Status

🔵 **Planning Phase** - No implementation yet

This plan requires:
- Team review and approval
- Phase 1 of Test Harness Adoption Plan to be complete
- Phase 2.1 and 2.2 of Test Harness Adoption Plan to be complete

## Resources

- **Full Plan:** `LOGIC_OVERHAUL_PLAN.md`
- **Separation Rationale:** `PHASE_2_3_SEPARATION_SUMMARY.md`
- **Test Harness Adoption Plan:** `TEST_HARNESS_ADOPTION_PLAN.md`
- **Test Harness Documentation:** `tests/harness/README.md`

## Questions?

Refer to the full plan document for:
- Detailed rationale for each decision
- Risk analysis and mitigation strategies
- Success metrics and validation criteria
- Future enhancement possibilities
