# UI Workflow Integration Tests

[← Back to Examples](../README.md) | [Test Data Naming Conventions](../../testing/TEST_DATA_NAMING_CONVENTIONS.md)

## Overview

This directory contains integration tests for multi-step UI interaction workflows. These tests validate the interaction between UICollisionLogic, UIActionLogic, event propagation, and CUIState updates across multiple game ticks.

**Test Level**: Integration  
**Primary Components**: UICollisionLogic, UIActionLogic, CUIState, EventBus  
**Typical Duration**: 2-10 ticks per test

---

## Test Coverage Matrix

| ID | File | Scenario | Components Tested | Ticks | Status |
|----|------|----------|-------------------|-------|--------|
| 001 | ui_workflow_001.test_data.json | Button click triggers visibility change | UICollision, UIAction, CUIState, Events | 4 | ✓ Pass |
| 002 | ui_workflow_002.test_data.json | Nested panel hover detection with z-ordering | UICollision, nested UI | 2 | ✓ Pass |
| 003 | ui_workflow_003.test_data.json | Multi-button state machine workflow | UIAction, CUIState, Events | 6 | ✓ Pass |
| 004 | ui_workflow_004.test_data.json | Panel visibility toggle cascade | UIAction, CUIState | 5 | ✓ Pass |
| 005 | ui_workflow_005.test_data.json | Mouse drag interaction across panels | UICollision, UIAction, Input | 8 | ✗ Fail (WIP) |

**Legend**:
- ✓ Pass - Test passing consistently
- ✗ Fail - Test failing (under investigation)
- ⚠ Skip - Test skipped (known issue)
- 🔧 WIP - Test under development

---

## Test Scenarios Detail

### ui_workflow_001: Button Click → Visibility Change

**Description**: Tests the complete workflow of a button click triggering a visibility change on a different panel.

**Flow**:
1. **Setup**: Panel A (visible) with button, Panel B (hidden)
2. **Tick 1**: Mouse moves over button on Panel A
   - Expected: `m_mouse_over_active = true` on button
3. **Tick 2**: Mouse clicks button
   - Expected: ButtonClicked event generated
4. **Tick 3**: UIActionLogic processes event
   - Expected: Panel B `m_visible = true`
5. **Tick 4**: Verify final state
   - Expected: Panel A visible, Panel B visible, button clicked

**Components**:
- UICollisionLogic (mouse detection)
- UIActionLogic (event processing)
- CUIState (visibility management)
- EventBus (event propagation)

**Tags**: `integration`, `ui`, `workflow`, `multi-tick`, `event-driven`

---

### ui_workflow_002: Nested Panel Hover with Z-Ordering

**Description**: Tests collision detection with nested UI panels and proper z-order handling.

**Flow**:
1. **Setup**: Parent panel with nested child panel (overlapping region)
2. **Tick 1**: Mouse over overlapping region
   - Expected: Child panel detects hover (higher z-order priority)
   - Expected: Parent panel `m_mouse_over_active = false`
3. **Tick 2**: Verify state persists

**Components**:
- UICollisionLogic (z-order detection)
- CUserInterface (nested structure)

**Tags**: `integration`, `ui`, `collision`, `nested-ui`, `z-order`

---

### ui_workflow_003: Multi-Button State Machine

**Description**: Tests complex state transitions with multiple buttons forming a state machine.

**Flow**:
1. **Setup**: 3 buttons (Start, Stop, Reset), Panel with state indicator
2. **Tick 1-2**: Click Start button → state = "Running"
3. **Tick 3-4**: Click Stop button → state = "Stopped"
4. **Tick 5-6**: Click Reset button → state = "Idle"

**Components**:
- UICollisionLogic (multiple button detection)
- UIActionLogic (state transitions)
- CUIState (state management)
- EventBus (button events)

**Tags**: `integration`, `ui`, `workflow`, `state-machine`, `multi-tick`

---

## Adding New Tests

### Quick Guide

1. **Choose next ID**: Use next sequential number (e.g., `006`)
2. **Create file**: `data/ui_workflow_006.test_data.json`
3. **Fill metadata**:
   ```json
   {
     "metadata": {
       "test_name": "ui_workflow_006",
       "description": "Detailed description of scenario with specific tick-by-tick expectations",
       "tags": ["integration", "ui", "workflow", "your-feature"],
       "expected_to_pass": true,
       "author": "YourName",
       "version": 1
     }
   }
   ```
4. **Update this README**: Add row to coverage matrix and detail section
5. **Run test**: `ctest --preset Debug -R ui_workflow_006`
6. **Update status**: Mark as Pass/Fail in matrix

### Template

```json
{
  "metadata": {
    "test_name": "ui_workflow_XXX",
    "description": "[Describe the complete workflow: setup, tick-by-tick actions, expected outcomes]",
    "tags": ["integration", "ui", "workflow", "[specific-feature]"],
    "expected_to_pass": true,
    "author": "[Your Name]",
    "version": 1
  },
  "simulation_data": {
    "description": "[High-level simulation description]",
    "steps": [
      {
        "simulation_type": "Collision",
        "execution_mode": "LogicClass",
        "logic_class_type": "UICollisionLogic",
        "description": "[What this step tests]"
      }
    ]
  },
  "start_data_collection": {
    "entity_collection": {
      "entity_memory_pool_size": 5,
      "entities": [
        // Define initial entity state
      ]
    }
  },
  "expected_data_collection": {
    "entity_collection": {
      "entity_memory_pool_size": 5,
      "entities": [
        // Define expected final state
      ]
    }
  },
  "num_ticks": 4
}
```

---

## Test Execution

### Run All UI Workflow Tests
```bash
ctest --preset Debug -R ui_workflow
```

### Run Specific Test
```bash
ctest --preset Debug -R ui_workflow_001
```

### Verbose Output on Failure
```bash
ctest --preset Debug -R ui_workflow --output-on-failure
```

---

## Common Patterns

### Pattern 1: Mouse Hover → Click → Action

Most UI tests follow this pattern:
1. Setup entities with UI components
2. Simulate mouse movement (input_sequence)
3. Detect collision (UICollisionLogic)
4. Simulate mouse click
5. Process action (UIActionLogic)
6. Verify state changes

### Pattern 2: Event-Driven State Changes

Tests that verify event propagation:
1. Generate event (button click, state change)
2. Event added to EventBus
3. UIActionLogic processes during Action phase
4. Verify components updated correctly

### Pattern 3: Multi-Panel Interactions

Tests with multiple UI panels:
1. Define spatial relationships (overlapping, nested, separate)
2. Test collision priority (z-order)
3. Verify isolation (actions on one panel don't affect others)

---

## Debugging Tips

1. **Use tick_snapshots**: Add intermediate snapshots to verify state at specific ticks
   ```json
   "tick_snapshots": [
     {
       "tick": 2,
       "data_collection": { /* expected state at tick 2 */ },
       "description": "After mouse hover detected"
     }
   ]
   ```

2. **Check metadata.description**: Ensure it fully describes expected behavior at each tick

3. **Verify input_sequence timing**: Make sure inputs trigger at correct ticks

4. **Test incrementally**: Start with simple scenario, add complexity

5. **Use descriptive tags**: Tags help filter and categorize related failures

---

## Related Documentation

- [Test Data Naming Conventions](../../testing/TEST_DATA_NAMING_CONVENTIONS.md) - Overall naming strategy
- [Filling Test Data Workflow](../../workflows/FILLING_TEST_DATA.md) - How to create test data files
- [Test Data Configuration](../../testing/TEST_DATA_CONFIGURATION.md) - Schema reference
- [Logic System](../../architecture/LOGIC_SYSTEM.md) - How Logic classes work

---

**Last Updated**: 2025-11-20  
**Maintained By**: SteamRot Testing Team
