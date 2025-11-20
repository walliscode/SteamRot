# Test Data Naming Conventions

[← Back to Documentation](../README.md) | [Testing Overview](TESTING_OVERVIEW.md) | [Test Data Configuration](TEST_DATA_CONFIGURATION.md)

This document establishes naming conventions for test data JSON files (`.test_data.json`) in the SteamRot project, addressing the growing complexity as testing expands from unit to integration and system testing.

---

## Table of Contents

- [Overview](#overview)
- [The Challenge](#the-challenge)
- [Recommended Approach: Hybrid System](#recommended-approach-hybrid-system)
- [Naming Strategy by Test Level](#naming-strategy-by-test-level)
- [ID-Based Catalog System (Optional)](#id-based-catalog-system-optional)
- [Metadata Best Practices](#metadata-best-practices)
- [Directory Organization](#directory-organization)
- [Migration Strategy](#migration-strategy)
- [Tooling Recommendations](#tooling-recommendations)

---

## Overview

Test data files in SteamRot use the FlatBuffers schema defined in `test_data.fbs` and are stored as JSON files with the `.test_data.json` suffix. As the test suite grows in complexity, we need a sustainable naming and organization strategy.

**Current State:**
- 25+ test data files across multiple directories
- Mix of descriptive and generic naming
- Rich metadata support in schema (name, description, tags, author, version)
- Growing complexity with multi-tick simulations, input sequences, and event testing

---

## The Challenge

As testing complexity increases, file names face these challenges:

### Simple Descriptive Naming Issues
```
✗ ui_collision_basic.test_data.json
  - What makes it "basic"? vs. intermediate or advanced?
  
✗ sample_input_event_simulation.test_data.json
  - "sample" doesn't convey specifics
  - What scenario does it test?
  
✗ process_ui_actions_button_click.test_data.json
  - Name gets long but still incomplete
  - Hard to distinguish from similar tests
```

### Integration Test Complexity
```
✗ ui_collision_logic_with_multiple_overlapping_panels_on_tick_5_with_mouse_movement.test_data.json
  - Too long, still not capturing everything
  - Hard to type and reference
```

---

## Recommended Approach: Hybrid System

Use **descriptive names for simple tests** and **ID-based catalog for complex tests**, with **metadata as the source of truth**.

### Core Principles

1. **Metadata is King**: The metadata fields (`test_name`, `description`, `tags`) are the authoritative description
2. **File Names are Handles**: File names are convenient identifiers, not complete descriptions
3. **Discoverability**: Tooling and documentation make tests discoverable
4. **Scale Gracefully**: System works for both 10 tests and 1000 tests

---

## Naming Strategy by Test Level

### Unit Tests: Descriptive Short Names

**Pattern**: `{component}_{aspect}[_{variant}].test_data.json`

**When to use**: Simple, focused tests with clear, short descriptions

**Examples**:
```
✓ ui_collision_basic.test_data.json
✓ configurator_basic.test_data.json
✓ pool_comparison_equal.test_data.json
✓ pool_comparison_different_size.test_data.json
✓ event_bus_snapshot.test_data.json
✓ metadata_validation.test_data.json
```

**Guidelines**:
- Keep to 3-4 words max
- Use snake_case
- Component/system first, then aspect being tested
- Optional variant at end for multiple tests of same aspect
- Avoid generic words like "sample", "test", "example" in real tests

**Metadata should contain**:
```json
{
  "metadata": {
    "test_name": "ui_collision_basic",
    "description": "Tests mouse collision detection with a single UI panel element at position (100, 100) with size (200, 150). Mouse at (150, 125) should detect collision.",
    "tags": ["unit", "UICollisionLogic", "collision"],
    "expected_to_pass": true,
    "version": 1
  }
}
```

### Integration Tests: ID-Based Catalog

**Pattern**: `{test_area}_{ID}.test_data.json`

**When to use**: Complex multi-component tests, scenario testing, workflows

**Examples**:
```
✓ ui_workflow_001.test_data.json
✓ ui_workflow_002.test_data.json
✓ scene_transition_001.test_data.json
✓ scene_transition_002.test_data.json
✓ entity_lifecycle_001.test_data.json
✓ input_simulation_001.test_data.json
```

**ID Format**: Zero-padded 3-digit numbers (001-999)

**Guidelines**:
- Test area describes the domain (ui_workflow, scene_transition, entity_lifecycle)
- Sequential numbering within each test area
- Easy to reference: "Check ui_workflow_001"
- Metadata contains the full scenario description

**Metadata should contain**:
```json
{
  "metadata": {
    "test_name": "ui_workflow_001",
    "description": "Multi-step UI interaction workflow: User clicks button on panel A (tick 1), which triggers visibility change on panel B (tick 2), then panel B receives mouse hover (tick 3) and click (tick 4). Tests: UICollisionLogic, UIActionLogic, event propagation, and CUIState updates across 4 ticks.",
    "tags": ["integration", "ui", "workflow", "multi-tick", "event-driven"],
    "expected_to_pass": true,
    "author": "YourName",
    "version": 1
  }
}
```

### System/End-to-End Tests: Scenario Names with ID

**Pattern**: `{scenario_name}_{ID}.test_data.json`

**When to use**: Full gameplay scenarios, end-to-end workflows

**Examples**:
```
✓ crafting_recipe_success_001.test_data.json
✓ crafting_recipe_failure_001.test_data.json
✓ scene_to_scene_navigation_001.test_data.json
✓ player_death_respawn_001.test_data.json
```

**Guidelines**:
- Scenario name describes the user story or workflow
- Multiple variations of same scenario get different IDs
- Keep scenario names readable but concise
- Metadata contains detailed step-by-step description

---

## ID-Based Catalog System (Optional)

For projects with 100+ integration tests, consider maintaining a catalog document.

### Catalog Structure

**File**: `documentation/testing/TEST_CATALOG.md`

**Format**:
```markdown
# Test Data Catalog

## UI Workflow Tests

| ID | File | Description | Tags | Coverage |
|----|------|-------------|------|----------|
| 001 | ui_workflow_001.test_data.json | Button click -> panel visibility | integration, ui, event | UICollision, UIAction, CUIState |
| 002 | ui_workflow_002.test_data.json | Nested panel hover detection | integration, ui, collision | UICollision, nested-ui |
| 003 | ui_workflow_003.test_data.json | Multi-button state machine | integration, ui, state | UIAction, CUIState, events |

## Scene Transition Tests

| ID | File | Description | Tags | Coverage |
|----|------|-------------|------|----------|
| 001 | scene_transition_001.test_data.json | Title -> Crafting scene | integration, scene | SceneManager, EntityLoader |
| 002 | scene_transition_002.test_data.json | Crafting -> Battle scene | integration, scene | SceneManager, state-preservation |
```

### Auto-Generation Tool (Future)

Consider creating a script to auto-generate catalog from metadata:

```bash
# Future tooling
./tools/generate_test_catalog.sh
```

This would:
1. Scan all `.test_data.json` files
2. Extract metadata
3. Generate markdown catalog
4. Show test coverage matrix

---

## Metadata Best Practices

### Required Fields

Always fill out these fields completely:

```json
{
  "metadata": {
    "test_name": "descriptive_unique_name",
    "description": "Detailed description of what is being tested, including specific values, tick numbers, expected outcomes",
    "tags": ["level", "component", "feature"],
    "expected_to_pass": true,
    "version": 1
  }
}
```

### Description Guidelines

**Bad descriptions** (too vague):
```json
"description": "Tests UI collision"
"description": "Button click test"
"description": "Multi-tick simulation"
```

**Good descriptions** (specific and detailed):
```json
"description": "Tests UICollisionLogic with mouse at (150, 125) over a panel at (100, 100) size (200, 150). Expects m_mouse_over_active = true after collision detection."

"description": "Multi-tick button interaction: Tick 1 - mouse hover sets m_mouse_over_active, Tick 2 - mouse click generates ButtonClicked event, Tick 3 - UIActionLogic processes event and sets m_clicked = true."

"description": "Scene transition from Title to Crafting scene. Verifies: (1) old scene entities cleaned up, (2) new scene entities loaded from crafting_scene.bin, (3) CUserInterface components properly configured, (4) LogicFactory creates correct Logic instances."
```

### Tag Guidelines

Use hierarchical tagging:

**Level tags** (required):
- `unit` - Single component/class tests
- `integration` - Multiple components interacting
- `system` - Full scenarios/workflows

**Component tags** (what's being tested):
- `UICollisionLogic`, `UIActionLogic`, `EntityManager`, etc.
- `collision`, `event`, `state-management`, `rendering`

**Feature tags** (gameplay features):
- `ui`, `crafting`, `combat`, `scene-transition`

**Complexity tags** (test characteristics):
- `multi-tick` - Tests spanning multiple ticks
- `event-driven` - Tests with event sequences
- `input-simulation` - Tests with simulated user input
- `negative-test` - Tests expected to fail

**Example**:
```json
"tags": ["integration", "UICollisionLogic", "UIActionLogic", "ui", "multi-tick", "event-driven"]
```

---

## Directory Organization

### Current Structure (Good)
```
tests/
├── unit/
│   ├── components/data/
│   ├── entity/data/
│   └── logic/data/
├── integration/
│   └── scene_change/data/
├── harness/data/              # Test infrastructure examples
└── data/                       # Shared test data
```

### Recommended Organization for Growth

```
tests/
├── unit/
│   ├── components/
│   │   └── data/              # Component unit tests
│   │       ├── ui_*.test_data.json
│   │       └── grimoire_*.test_data.json
│   ├── entity/
│   │   └── data/              # Entity system unit tests
│   │       ├── configurator_*.test_data.json
│   │       └── pool_*.test_data.json
│   └── logic/
│       └── data/              # Logic class unit tests
│           ├── collision_*.test_data.json
│           └── action_*.test_data.json
│
├── integration/
│   ├── ui_workflows/
│   │   ├── data/              # UI integration scenarios
│   │   │   ├── ui_workflow_001.test_data.json
│   │   │   ├── ui_workflow_002.test_data.json
│   │   │   └── ...
│   │   └── README.md          # Documents what each test covers
│   ├── scene_transitions/
│   │   ├── data/
│   │   │   ├── scene_transition_001.test_data.json
│   │   │   └── ...
│   │   └── README.md
│   └── entity_lifecycle/
│       ├── data/
│       │   ├── entity_lifecycle_001.test_data.json
│       │   └── ...
│       └── README.md
│
├── system/
│   ├── crafting_scenarios/
│   │   ├── data/
│   │   │   ├── crafting_success_001.test_data.json
│   │   │   ├── crafting_failure_001.test_data.json
│   │   │   └── ...
│   │   └── README.md          # Full scenario descriptions
│   └── gameplay_flows/
│       ├── data/
│       └── README.md
│
└── harness/
    └── data/                   # Test infrastructure validation
        └── metadata_*.test_data.json
```

### Directory-Level README Pattern

Each integration/system test directory should have a README:

```markdown
# UI Workflow Integration Tests

## Overview
Tests for multi-step UI interaction workflows involving collision detection,
action processing, event propagation, and state updates.

## Test Coverage Matrix

| Test ID | File | Scenario | Components | Ticks |
|---------|------|----------|------------|-------|
| 001 | ui_workflow_001.test_data.json | Button click -> visibility change | UICollision, UIAction, CUIState | 4 |
| 002 | ui_workflow_002.test_data.json | Nested panel hover | UICollision | 2 |
| 003 | ui_workflow_003.test_data.json | Multi-button state machine | UIAction, CUIState | 6 |

## Adding New Tests

1. Create file: `ui_workflow_XXX.test_data.json` (next available ID)
2. Fill metadata with detailed description
3. Update this README with test info
4. Consider adding to TEST_CATALOG.md if complex
```

---

## Migration Strategy

### Phase 1: Keep Existing Names

**Action**: No immediate renaming required

**Rationale**: 
- Existing test names are working
- Don't break existing references
- Focus on new test conventions

**For existing tests**:
- Improve metadata descriptions if vague
- Add comprehensive tags
- Document in directory READMEs

### Phase 2: Adopt Conventions for New Tests

**Action**: Apply conventions to all new tests created

**Guidelines**:
- Unit tests: Use descriptive short names
- Integration tests: Use ID-based naming
- Always fill complete metadata
- Create directory READMEs for new test areas

### Phase 3: Optional Refactoring

**Action**: Refactor existing test names if beneficial

**When to refactor**:
- Test suite grows to 100+ files
- Naming conflicts arise
- Team agrees renaming adds value

**Migration script approach**:
```bash
# Rename files
mv old_name.test_data.json new_name.test_data.json

# Update metadata.test_name field in JSON
# Update any test references in code
# Update CMakeLists.txt if needed
# Git commit with clear migration message
```

---

## Tooling Recommendations

### Test Discovery Script

Create a script to find and list tests by criteria:

```bash
# Find all integration tests
./tools/list_tests.sh --level integration

# Find all UI-related tests
./tools/list_tests.sh --tag ui

# Find all multi-tick tests
./tools/list_tests.sh --tag multi-tick

# Show test with ID
./tools/list_tests.sh --id ui_workflow_001
```

### Test Metadata Validator

Validate metadata completeness:

```bash
# Check all test data files have complete metadata
./tools/validate_test_metadata.sh

# Output:
# ✓ ui_collision_basic.test_data.json - OK
# ✗ sample_test_1.test_data.json - Missing detailed description
# ✗ old_test.test_data.json - Missing tags
```

### Test Catalog Generator

Auto-generate catalog from metadata:

```bash
# Generate catalog markdown
./tools/generate_test_catalog.sh > documentation/testing/TEST_CATALOG.md
```

### Test Template Generator

Generate boilerplate for new tests:

```bash
# Create new integration test
./tools/new_test.sh integration ui_workflow_003

# Generates:
# - tests/integration/ui_workflows/data/ui_workflow_003.test_data.json
# - Pre-filled metadata template
# - Prompts for description and tags
```

---

## Summary

### Quick Decision Tree

**Creating a new test?**

1. **Simple unit test** (single component, straightforward scenario)
   - → Use descriptive short name: `{component}_{aspect}.test_data.json`
   - → Example: `pool_comparison_equal.test_data.json`

2. **Complex integration test** (multiple components, multi-step)
   - → Use ID-based name: `{test_area}_{ID}.test_data.json`
   - → Example: `ui_workflow_001.test_data.json`
   - → Put extensive details in metadata.description

3. **System/E2E test** (full gameplay scenario)
   - → Use scenario name with ID: `{scenario}_{ID}.test_data.json`
   - → Example: `crafting_success_001.test_data.json`

**Always**:
- Fill complete metadata (test_name, detailed description, comprehensive tags)
- Place in appropriate directory hierarchy
- Update directory README if integration/system test
- Consider adding to TEST_CATALOG.md for complex tests

### Key Takeaways

1. **Metadata is authoritative** - File names are just handles
2. **Scale the complexity** - Simple names for simple tests, IDs for complex tests
3. **Discoverability matters** - Tags, READMEs, and tooling make tests findable
4. **No forced migration** - Existing tests fine as-is, conventions for new tests
5. **Database-style when needed** - ID-based catalog for large test suites

---

## See Also

- [Test Data Configuration](TEST_DATA_CONFIGURATION.md) - Schema and API reference
- [Filling Test Data Workflow](../workflows/FILLING_TEST_DATA.md) - How to create test data files
- [Testing Overview](TESTING_OVERVIEW.md) - General testing practices

---

**Last Updated**: 2025-11-20  
**Version**: 1.0
