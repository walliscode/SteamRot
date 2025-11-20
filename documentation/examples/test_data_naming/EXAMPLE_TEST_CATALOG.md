# Test Data Catalog Example

[← Back to Examples](../README.md) | [Test Data Naming Conventions](../../testing/TEST_DATA_NAMING_CONVENTIONS.md)

This is an **example** of what a comprehensive test catalog could look like for a larger test suite. This catalog approach is **optional** and recommended for projects with 100+ integration tests.

---

## Purpose

The test catalog provides:
- **Quick reference** for all integration and system tests
- **Coverage visibility** showing what combinations are tested
- **Gap analysis** to identify untested scenarios
- **Documentation** of complex test scenarios

**Note**: This catalog can be auto-generated from test metadata using tooling (see [Tooling Recommendations](../../testing/TEST_DATA_NAMING_CONVENTIONS.md#tooling-recommendations)).

---

## Test Statistics

| Category | Count | Pass | Fail | Skip |
|----------|-------|------|------|------|
| Unit Tests | 45 | 45 | 0 | 0 |
| Integration Tests | 23 | 22 | 1 | 0 |
| System Tests | 8 | 7 | 0 | 1 |
| **Total** | **76** | **74** | **1** | **1** |

**Last Updated**: 2025-11-20  
**Test Pass Rate**: 97.4%

---

## UI Workflow Integration Tests

**Category**: Integration  
**Directory**: `tests/integration/ui_workflows/data/`  
**Components**: UICollisionLogic, UIActionLogic, CUIState, EventBus

| ID | File | Description | Tags | Components | Ticks | Status | Notes |
|----|------|-------------|------|------------|-------|--------|-------|
| 001 | ui_workflow_001.test_data.json | Button click triggers panel visibility change | integration, ui, event | UICollision, UIAction, CUIState | 4 | ✓ Pass | Baseline workflow |
| 002 | ui_workflow_002.test_data.json | Nested panel hover with z-order handling | integration, ui, collision | UICollision, nested-ui | 2 | ✓ Pass | Tests z-ordering |
| 003 | ui_workflow_003.test_data.json | Multi-button state machine transitions | integration, ui, state | UIAction, CUIState, Events | 6 | ✓ Pass | Complex states |
| 004 | ui_workflow_004.test_data.json | Panel visibility toggle cascade effect | integration, ui, state | UIAction, CUIState | 5 | ✓ Pass | Chain reactions |
| 005 | ui_workflow_005.test_data.json | Mouse drag interaction across panels | integration, ui, input | UICollision, UIAction | 8 | ✗ Fail | Issue #234 |
| 006 | ui_workflow_006.test_data.json | Dropdown menu selection workflow | integration, ui, dropdown | UICollision, UIAction, CDropdown | 5 | ✓ Pass | - |
| 007 | ui_workflow_007.test_data.json | Modal dialog open/close cycle | integration, ui, modal | UICollision, UIAction, CModal | 6 | ✓ Pass | - |

---

## Scene Transition Integration Tests

**Category**: Integration  
**Directory**: `tests/integration/scene_transitions/data/`  
**Components**: SceneManager, EntityManager, LogicFactory, ResourceManager

| ID | File | Description | Tags | Components | Ticks | Status | Notes |
|----|------|-------------|------|------------|-------|--------|-------|
| 001 | scene_transition_001.test_data.json | Title → Crafting scene basic transition | integration, scene | SceneManager, EntityManager | 3 | ✓ Pass | Happy path |
| 002 | scene_transition_002.test_data.json | Crafting → Battle scene with state preservation | integration, scene, state | SceneManager, state-save | 4 | ✓ Pass | Saves inventory |
| 003 | scene_transition_003.test_data.json | Battle → Crafting with entity cleanup | integration, scene, cleanup | SceneManager, EntityManager | 3 | ✓ Pass | Verifies cleanup |
| 004 | scene_transition_004.test_data.json | Rapid scene switching stress test | integration, scene, stress | SceneManager | 10 | ✓ Pass | Performance |
| 005 | scene_transition_005.test_data.json | Scene transition with pending events | integration, scene, events | SceneManager, EventBus | 5 | ✓ Pass | Event handling |

---

## Entity Lifecycle Integration Tests

**Category**: Integration  
**Directory**: `tests/integration/entity_lifecycle/data/`  
**Components**: EntityManager, ComponentRegister, ArchetypeManager

| ID | File | Description | Tags | Components | Ticks | Status | Notes |
|----|------|-------------|------|------------|-------|--------|-------|
| 001 | entity_lifecycle_001.test_data.json | Entity creation and activation | integration, entity | EntityManager, Components | 2 | ✓ Pass | - |
| 002 | entity_lifecycle_002.test_data.json | Component activation/deactivation cycle | integration, entity, component | ArchetypeManager | 5 | ✓ Pass | Archetype changes |
| 003 | entity_lifecycle_003.test_data.json | Entity destruction and memory reclaim | integration, entity, memory | EntityManager | 3 | ✓ Pass | - |
| 004 | entity_lifecycle_004.test_data.json | Bulk entity operations (100 entities) | integration, entity, bulk | EntityManager, Archetypes | 8 | ✓ Pass | Performance test |

---

## Crafting System Tests

**Category**: System  
**Directory**: `tests/system/crafting_scenarios/data/`  
**Components**: Full game systems

| ID | File | Description | Tags | Scenario | Ticks | Status | Notes |
|----|------|-------------|------|----------|-------|--------|-------|
| 001 | crafting_success_001.test_data.json | Basic recipe crafting - success path | system, crafting, success | Player crafts health potion | 10 | ✓ Pass | Happy path |
| 002 | crafting_failure_001.test_data.json | Recipe crafting - insufficient materials | system, crafting, failure | Missing ingredients | 8 | ✓ Pass | Error handling |
| 003 | crafting_advanced_001.test_data.json | Multi-step crafting with intermediates | system, crafting, complex | Sword crafting chain | 20 | ✓ Pass | 3-step recipe |
| 004 | crafting_ui_integration_001.test_data.json | Full UI interaction for crafting | system, crafting, ui | User clicks through UI | 15 | ✓ Pass | E2E workflow |
| 005 | crafting_inventory_sync_001.test_data.json | Inventory updates during crafting | system, crafting, inventory | Material consumption | 12 | ✓ Pass | State sync |

---

## Combat System Tests

**Category**: System  
**Directory**: `tests/system/combat_scenarios/data/`  
**Components**: Full game systems

| ID | File | Description | Tags | Scenario | Ticks | Status | Notes |
|----|------|-------------|------|----------|-------|--------|-------|
| 001 | combat_basic_attack_001.test_data.json | Basic melee attack action | system, combat, attack | Player attacks enemy | 5 | ✓ Pass | - |
| 002 | combat_player_death_001.test_data.json | Player death and respawn flow | system, combat, death | HP reaches 0 | 12 | ⚠ Skip | Respawn pending |
| 003 | combat_spell_casting_001.test_data.json | Spell casting with mana cost | system, combat, magic | Fireball spell | 8 | ✓ Pass | - |

---

## Coverage Matrix

### Components Tested

| Component | Unit Tests | Integration Tests | System Tests | Total Coverage |
|-----------|-----------|-------------------|--------------|----------------|
| UICollisionLogic | 5 | 7 | 2 | 14 |
| UIActionLogic | 4 | 6 | 2 | 12 |
| CUIState | 3 | 5 | 1 | 9 |
| SceneManager | 6 | 5 | 3 | 14 |
| EntityManager | 8 | 4 | 4 | 16 |
| EventBus | 4 | 3 | 2 | 9 |
| LogicFactory | 5 | 4 | 0 | 9 |
| ArchetypeManager | 6 | 3 | 0 | 9 |

### Feature Coverage

| Feature | Tests | Pass | Coverage |
|---------|-------|------|----------|
| UI Interaction | 15 | 14 | 93% |
| Scene Transitions | 8 | 8 | 100% |
| Entity Lifecycle | 10 | 10 | 100% |
| Event System | 7 | 7 | 100% |
| Crafting System | 12 | 12 | 100% |
| Combat System | 8 | 7 | 88% |

---

## Test Complexity Distribution

```
Simple (1-3 ticks):  ████████████░░░░░░░░ 45 tests (59%)
Medium (4-8 ticks):  ████████░░░░░░░░░░░░ 24 tests (32%)
Complex (9+ ticks):  ██░░░░░░░░░░░░░░░░░░  7 tests (9%)
```

---

## Gaps and Recommendations

### Missing Coverage

1. **Combat System**:
   - ⚠ No tests for enemy AI behavior
   - ⚠ Limited spell variety testing
   - ⚠ No tests for combo attacks

2. **UI System**:
   - ⚠ No tests for keyboard navigation
   - ⚠ Limited accessibility testing
   - ⚠ No tests for complex form validation

3. **Inventory System**:
   - ⚠ No tests for inventory sorting
   - ⚠ Limited stack merging tests
   - ⚠ No tests for inventory full scenarios

### Recommended Next Tests

1. `combat_ai_basic_001.test_data.json` - Enemy AI movement
2. `ui_keyboard_nav_001.test_data.json` - Tab navigation
3. `inventory_sort_001.test_data.json` - Sort by type/value

---

## Test Maintenance

### Recently Added
- ui_workflow_007 (2025-11-18) - Modal dialogs
- scene_transition_005 (2025-11-17) - Pending events
- crafting_ui_integration_001 (2025-11-15) - Full UI workflow

### Failing Tests
- ui_workflow_005 - Mouse drag across panels (Issue #234)

### Skipped Tests
- combat_player_death_001 - Respawn system not implemented

### Deprecated Tests
- None

---

## Using This Catalog

### Find Tests by Component
Search for component name in the "Components" column

### Find Tests by Feature
Use the "Tags" column or "Feature Coverage" section

### Find Test by ID
Use the ID column for quick reference: "Check ui_workflow_003"

### Check Test Status
- ✓ Pass - Stable, passing
- ✗ Fail - Currently failing, under investigation
- ⚠ Skip - Intentionally skipped
- 🔧 WIP - Work in progress

---

## Auto-Generation

This catalog can be auto-generated using:

```bash
# Future tooling
./tools/generate_test_catalog.sh > documentation/testing/TEST_CATALOG.md
```

The script would:
1. Scan all `.test_data.json` files
2. Parse metadata (name, description, tags)
3. Run tests to get current status
4. Generate markdown tables
5. Calculate coverage statistics

---

## See Also

- [Test Data Naming Conventions](../../testing/TEST_DATA_NAMING_CONVENTIONS.md) - Naming strategy
- [Test Data Configuration](../../testing/TEST_DATA_CONFIGURATION.md) - Schema reference
- [Testing Overview](../../testing/TESTING_OVERVIEW.md) - General testing practices

---

**Last Updated**: 2025-11-20  
**Version**: 1.0  
**Auto-Generated**: No (manual example)
