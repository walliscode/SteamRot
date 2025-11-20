# Test Data Naming Examples

[← Back to Examples](../README.md) | [Test Data Naming Conventions](../../testing/TEST_DATA_NAMING_CONVENTIONS.md)

This directory contains examples demonstrating the test data naming conventions and organizational patterns.

---

## Examples Included

### 1. Integration Test Directory README
**File**: [EXAMPLE_INTEGRATION_TEST_README.md](EXAMPLE_INTEGRATION_TEST_README.md)

**Purpose**: Template for well-organized integration test directories

**Contains**:
- Test coverage matrix with IDs, files, scenarios, components, and status
- Detailed scenario descriptions for each test
- Quick guide for adding new tests
- Test execution commands
- Common patterns and debugging tips

**Use as template for**:
- `tests/integration/ui_workflows/README.md`
- `tests/integration/scene_transitions/README.md`
- `tests/integration/entity_lifecycle/README.md`

---

### 2. Test Catalog
**File**: [EXAMPLE_TEST_CATALOG.md](EXAMPLE_TEST_CATALOG.md)

**Purpose**: Template for comprehensive catalog for large test suites (100+ tests)

**Contains**:
- Test statistics (pass/fail counts)
- Coverage matrices by component and feature
- Test listings organized by category
- Gap analysis and recommendations
- Complexity distribution visualization
- Maintenance tracking (recently added, failing, skipped)

**Use for**:
- Central test visibility for large projects
- Manual maintenance or auto-generation (future tooling)
- Published at `documentation/testing/TEST_CATALOG.md`

---

## Using These Templates

### Integration Test Directory README

**Use for**:
- New integration test directories
- 5+ tests in one category
- Complex tests (multi-tick, multi-component)
- Team documentation needs

**Workflow**:
1. Copy EXAMPLE_INTEGRATION_TEST_README.md
2. Rename to README.md in your test directory
3. Update category name and description
4. Fill in test coverage matrix
5. Add scenario details for each test
6. Update as tests are added/modified

---

### Test Catalog

**Use for**:
- Test suites with 100+ integration/system tests
- Multiple teams working on different areas
- Coverage visibility needs
- Test strategy planning

**Options**:
1. **Manual**: Copy template, update as tests added
2. **Semi-Auto**: Script extracts metadata, manual curation
3. **Full-Auto**: Script generates entire catalog (future)

---

## Naming Patterns Demonstrated

### Unit Tests (Descriptive Names)

```
✓ ui_collision_basic.test_data.json
✓ configurator_basic.test_data.json
✓ pool_comparison_equal.test_data.json
```

**Pattern**: `{component}_{aspect}[_{variant}].test_data.json`

---

### Integration Tests (ID-Based)

```
✓ ui_workflow_001.test_data.json
✓ ui_workflow_002.test_data.json
✓ scene_transition_001.test_data.json
✓ entity_lifecycle_001.test_data.json
```

**Pattern**: `{test_area}_{ID}.test_data.json`

---

### System Tests (Scenario Names)

```
✓ crafting_success_001.test_data.json
✓ crafting_failure_001.test_data.json
✓ combat_player_death_001.test_data.json
```

**Pattern**: `{scenario_name}_{ID}.test_data.json`

---

## Metadata Examples

### Simple Unit Test

```json
{
  "metadata": {
    "test_name": "ui_collision_basic",
    "description": "Tests UICollisionLogic with mouse at (150, 125) over a panel at (100, 100) size (200, 150). Expects m_mouse_over_active = true after collision detection.",
    "tags": ["unit", "UICollisionLogic", "collision"],
    "expected_to_pass": true,
    "version": 1
  }
}
```

---

### Complex Integration Test

```json
{
  "metadata": {
    "test_name": "ui_workflow_001",
    "description": "Multi-step UI interaction workflow: User clicks button on panel A (tick 1), which triggers visibility change on panel B (tick 2), then panel B receives mouse hover (tick 3) and click (tick 4). Tests: UICollisionLogic, UIActionLogic, event propagation, and CUIState updates across 4 ticks.",
    "tags": ["integration", "ui", "workflow", "multi-tick", "event-driven"],
    "expected_to_pass": true,
    "author": "TestTeam",
    "version": 1
  }
}
```

---

### System Test with Scenario

```json
{
  "metadata": {
    "test_name": "crafting_success_001",
    "description": "End-to-end crafting scenario: Player opens crafting menu (tick 1-2), selects health potion recipe (tick 3-4), confirms craft with sufficient materials (tick 5-6), receives item in inventory (tick 7-8), UI updates to reflect new inventory state (tick 9-10). Tests full crafting pipeline including UI, inventory system, recipe validation, and state management.",
    "tags": ["system", "crafting", "e2e", "success-path", "inventory"],
    "expected_to_pass": true,
    "author": "GameplayTeam",
    "version": 1
  }
}
```

---

## Directory Structure Examples

### Small Project (< 50 tests)

```
tests/
├── unit/
│   ├── components/data/
│   ├── entity/data/
│   └── logic/data/
├── integration/
│   └── ui_workflows/data/
└── harness/data/
```

**Organization**: Simple structure, descriptive names for most tests

---

### Medium Project (50-200 tests)

```
tests/
├── unit/
│   ├── components/data/
│   ├── entity/data/
│   └── logic/data/
├── integration/
│   ├── ui_workflows/
│   │   ├── data/          # ID-based: ui_workflow_001.test_data.json
│   │   └── README.md      # Coverage matrix
│   ├── scene_transitions/
│   │   ├── data/          # ID-based: scene_transition_001.test_data.json
│   │   └── README.md
│   └── entity_lifecycle/
│       ├── data/
│       └── README.md
└── system/
    ├── crafting_scenarios/
    │   ├── data/          # Scenario-based: crafting_success_001.test_data.json
    │   └── README.md
    └── combat_scenarios/
        ├── data/
        └── README.md
```

**Organization**: ID-based naming for integration, directory READMEs for documentation

---

### Large Project (200+ tests)

```
Same as medium, plus:

documentation/testing/
├── TEST_CATALOG.md        # Comprehensive catalog
└── test_coverage_reports/ # Generated coverage reports
```

**Organization**: Full catalog with auto-generation, systematic gap analysis

---

## Quick Reference

### Choosing a Naming Strategy

| Test Type | Complexity | Strategy | Example |
|-----------|-----------|----------|---------|
| Unit | Simple | Descriptive short name | `collision_basic.test_data.json` |
| Integration | Complex | ID-based catalog | `ui_workflow_001.test_data.json` |
| System | Scenario | Scenario name + ID | `crafting_success_001.test_data.json` |

### When to Create a Directory README

- ✓ Integration test directory with 5+ tests
- ✓ Tests are complex (multi-step, multi-component)
- ✓ Multiple developers working in the area
- ✓ Need documentation of test patterns

### When to Create a Catalog

- ✓ 100+ integration/system tests
- ✓ Multiple test categories
- ✓ Need coverage visibility
- ✓ Planning test strategy

---

## See Also

- [Test Data Naming Conventions](../../testing/TEST_DATA_NAMING_CONVENTIONS.md) - Complete guide
- [Test Data Configuration](../../testing/TEST_DATA_CONFIGURATION.md) - Schema reference
- [Filling Test Data Workflow](../../workflows/FILLING_TEST_DATA.md) - How-to guide
- [Testing Overview](../../testing/TESTING_OVERVIEW.md) - General testing

---

**Last Updated**: 2025-11-20
