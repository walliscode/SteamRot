# SteamRot File Naming Conventions

## Overview

This document establishes a cohesive naming system for files in the SteamRot project. The goal is to provide clear visual distinction between classes, free functions, and test utilities while enabling easy extensibility.

## Core Principles

1. **Visual Distinction**: File names should immediately indicate their content type
2. **Namespace Alignment**: File names should reflect their namespace structure
3. **Extensibility**: Easy to add new functions to existing namespaces
4. **Consistency**: Same patterns across production and test code
5. **Clarity**: No ambiguity about what a file contains

## Naming Patterns

### 1. Class Files (PascalCase)

**Pattern**: `ClassName.h` / `ClassName.cpp`

**When to use**: Files that contain a single primary class definition

**Examples**:
- `EntityManager.h` / `EntityManager.cpp`
- `LogicFactory.h` / `LogicFactory.cpp`
- `UIActionLogic.h` / `UIActionLogic.cpp`

**Namespace**: Typically just `steamrot`

**Special prefixes**:
- Components: `C` prefix (e.g., `CUserInterface`, `CGrimoireMachina`)
- No other prefixes for classes

---

### 2. Free Function Files (Subsystem Namespace Pattern)

**Pattern**: `subsystem_functions.h` / `subsystem_functions.cpp`

**When to use**: Files containing free functions grouped by subsystem/domain

**File naming**: `{subsystem}_{purpose}.h/cpp`
- Always snake_case
- First part identifies the subsystem
- Second part describes the category of functions

**Namespace**: `steamrot::{subsystem}` or `steamrot::{subsystem}::{purpose}`

**Examples**:

**Current (to be standardized)**:
```
src/logic/collision.h        → steamrot::collision
src/logic/ui_helpers.h        → steamrot::ui_helpers
src/entity/emp_helpers.h      → steamrot::emp_helpers
src/events/event_helpers.h    → steamrot::event_helpers
```

**Proposed standardization**:
```
src/logic/logic_collision.h   → steamrot::logic::collision
src/logic/logic_ui.h          → steamrot::logic::ui
src/entity/entity_memory.h    → steamrot::entity::memory
src/events/event_conversion.h → steamrot::event::conversion
```

**Benefits of new pattern**:
- Clear subsystem identification (logic, entity, events)
- Easy to add new function files to a subsystem
- Namespace structure matches file structure
- Avoids naming conflicts across subsystems

---

### 3. Template Utility Files (PascalCase with "Utils" suffix)

**Pattern**: `SubsystemUtils.h` (header-only)

**When to use**: Template-only utility functions for a subsystem

**Examples**:
```
Current: src/entity/ArchetypeHelpers.h
Proposed: src/entity/ArchetypeUtils.h
```

**Namespace**: `steamrot::{subsystem}`

**Characteristics**:
- Header-only (templates)
- PascalCase to distinguish from regular free function files
- "Utils" suffix makes purpose clear
- No `.cpp` file

---

### 4. Test Helper Files (snake_case with namespace prefix)

**Pattern**: `{subsystem}_test_helpers.h` / `{subsystem}_test_helpers.cpp`

**When to use**: Helper functions specifically for testing a subsystem

**Namespace**: `steamrot::tests::{subsystem}` or `steamrot::tests`

**Examples**:
```
Current: tests/logic/logic_helpers.h
Better:  tests/logic/logic_test_helpers.h

Current: tests/entity/configuration_helpers.h
Better:  tests/entity/entity_test_helpers.h
```

**Benefits**:
- Clear indication these are test-only utilities
- Subsystem prefix shows what they're testing
- Consistent with production code patterns
- Easy to find related test helpers

---

### 5. Test Files (Class/Module name with .test.cpp)

**Pattern**: `{ModuleName}.test.cpp`

**When to use**: Unit tests for a specific module (class or function file)

**Examples**:
- `EntityManager.test.cpp` - tests EntityManager class
- `logic_collision.test.cpp` - tests logic collision functions
- `entity_memory.test.cpp` - tests entity memory functions

**Naming matches the module being tested**:
- Class test: matches class name (PascalCase)
- Function file test: matches function file name (snake_case)

---

## Directory Structure and Namespaces

### Production Code (`src/`)

```
src/
├── subsystem/
│   ├── ClassName.h/cpp              # Classes: steamrot::ClassName
│   ├── subsystem_category.h/cpp     # Free functions: steamrot::subsystem::category
│   └── SubsystemUtils.h             # Templates: steamrot::subsystem
```

**Example - Logic subsystem**:
```
src/logic/
├── Logic.h/cpp                       # Base class: steamrot::Logic
├── UIActionLogic.h/cpp              # Derived class: steamrot::UIActionLogic
├── LogicFactory.h/cpp               # Factory class: steamrot::LogicFactory
├── logic_collision.h/cpp            # Free functions: steamrot::logic::collision
├── logic_ui.h/cpp                   # Free functions: steamrot::logic::ui
└── LogicUtils.h                     # Templates: steamrot::logic
```

### Test Code (`tests/`)

```
tests/
├── subsystem/
│   ├── ClassName.test.cpp           # Tests for ClassName
│   ├── module_name.test.cpp         # Tests for free function modules
│   └── subsystem_test_helpers.h/cpp # Test utilities: steamrot::tests::subsystem
```

**Example - Logic tests**:
```
tests/logic/
├── UIActionLogic.test.cpp           # Tests UIActionLogic class
├── logic_collision.test.cpp         # Tests logic collision functions
├── logic_ui.test.cpp                # Tests logic UI functions
└── logic_test_helpers.h/cpp         # Test utilities: steamrot::tests::logic
```

---

## Namespace Guidelines

### Pattern: File-to-Namespace Mapping

| File Pattern | Namespace Pattern | Example |
|--------------|-------------------|---------|
| `ClassName.h` | `steamrot` | `steamrot::EntityManager` |
| `subsystem_category.h` | `steamrot::subsystem::category` | `steamrot::logic::collision` |
| `SubsystemUtils.h` | `steamrot::subsystem` | `steamrot::entity` (ArchetypeUtils) |
| `subsystem_test_helpers.h` | `steamrot::tests::subsystem` | `steamrot::tests::logic` |

### Namespace Extensibility

**Adding functions to an existing namespace:**

If you want to add more collision functions to `steamrot::logic::collision`:
1. Open `src/logic/logic_collision.h/cpp`
2. Add your functions to the existing namespace
3. No new files needed unless creating a new category

**Adding a new category to a subsystem:**

To add rendering helpers to the logic subsystem:
1. Create `src/logic/logic_render.h/cpp`
2. Use namespace `steamrot::logic::render`
3. Follows the established pattern

**Example - Growing the logic subsystem**:
```cpp
// src/logic/logic_collision.h
namespace steamrot::logic::collision {
  bool IsMouseOverBounds(...);
  void CheckMouseOverUIElement(...);
  // Easy to add more collision functions here
}

// src/logic/logic_render.h (new file)
namespace steamrot::logic::render {
  void DrawDebugBounds(...);
  void RenderGrid(...);
  // New category of functions
}
```

---

## Migration Strategy

### Phase 1: Documentation (Current)
- Document the new system
- Get team alignment
- No code changes yet

### Phase 2: New Files Follow Convention
- All new files use the new naming convention
- Existing files remain unchanged
- Gradual adoption

### Phase 3: Opportunistic Refactoring (Optional)
- Rename files when making significant changes
- Update includes and references
- One subsystem at a time

### Recommended Migration Priority
1. **High Priority**: Test helpers (least disruptive)
   - `logic_helpers.h` → `logic_test_helpers.h`
   - `configuration_helpers.h` → `entity_test_helpers.h`

2. **Medium Priority**: Template utilities
   - `ArchetypeHelpers.h` → `ArchetypeUtils.h`

3. **Low Priority**: Production free function files
   - `collision.h` → `logic_collision.h`
   - `ui_helpers.h` → `logic_ui.h`
   - `emp_helpers.h` → `entity_memory.h`

**Note**: Migration is optional. The key is that all NEW files follow the convention.

---

## Quick Reference Flowchart

```
What are you creating?
│
├─ A class with methods?
│  └─ Use: ClassName.h/cpp (PascalCase)
│     Example: EntityManager.h
│
├─ Free functions for a subsystem?
│  └─ Use: subsystem_category.h/cpp (snake_case)
│     Example: logic_collision.h
│     Namespace: steamrot::subsystem::category
│
├─ Template utilities?
│  └─ Use: SubsystemUtils.h (PascalCase, header-only)
│     Example: ArchetypeUtils.h
│     Namespace: steamrot::subsystem
│
├─ Test helper functions?
│  └─ Use: subsystem_test_helpers.h/cpp (snake_case)
│     Example: logic_test_helpers.h
│     Namespace: steamrot::tests::subsystem
│
└─ Unit tests?
   └─ Use: ModuleName.test.cpp
      Example: EntityManager.test.cpp or logic_collision.test.cpp
```

---

## Benefits Summary

### For Developers
- ✅ Immediately know what a file contains by its name
- ✅ Easy to find where to add new functions
- ✅ Consistent patterns across codebase
- ✅ Clear namespace organization

### For Extensibility
- ✅ Easy to add new function categories to a subsystem
- ✅ Clear namespace boundaries prevent collisions
- ✅ Scalable as the project grows
- ✅ No ambiguity about where new code belongs

### For Maintenance
- ✅ Clear distinction between production and test code
- ✅ Easier refactoring (namespace scope is clear)
- ✅ Better IDE support (namespace autocomplete)
- ✅ Reduced cognitive load

---

## Examples in Context

### Example 1: Adding Collision Functions to Logic

**Scenario**: Need to add new collision detection functions

**Old way** (ambiguous):
```
src/logic/collision.h  # Is this a class or functions?
```

**New way** (clear):
```
src/logic/logic_collision.h  # Obviously logic subsystem functions
namespace steamrot::logic::collision { ... }
```

### Example 2: Adding Entity Memory Functions

**Scenario**: Need helper functions for entity memory operations

**Old way** (inconsistent):
```
src/entity/emp_helpers.h  # What does "emp" mean?
namespace steamrot::emp_helpers { ... }
```

**New way** (clear):
```
src/entity/entity_memory.h  # Clear purpose
namespace steamrot::entity::memory { ... }
```

### Example 3: Growing Test Utilities

**Scenario**: Adding more test helper functions for logic

**File**: `tests/logic/logic_test_helpers.h`
```cpp
namespace steamrot::tests::logic {
  // Existing helpers
  void CheckStaticLogicCollections(...);
  
  // Easy to add more
  void SetupMockLogicContext(...);
  void VerifyLogicExecution(...);
}
```

**Benefits**:
- All logic test helpers in one place
- Clear namespace prevents conflicts
- Easy to find and extend

---

## Special Cases

### Legacy Files (Grandfathered)

Some existing files don't follow the convention. That's okay:
- `tilelogic.cpp` - legacy display code
- Existing helper files can remain until refactored

### Single-Function Files

If a file contains only one function, you can:
- Still use the namespace pattern for consistency
- Or create a class if the function has state

### Cross-Cutting Utilities

For utilities that span multiple subsystems:
- Use: `core_utilities.h` or `common_functions.h`
- Namespace: `steamrot::util` or `steamrot::common`
- Document clearly in file header

---

## Validation Checklist

Before committing a new file, verify:

- [ ] File name clearly indicates its content (class vs functions)
- [ ] Namespace matches the file name pattern
- [ ] Name includes subsystem prefix (for free function files)
- [ ] Test files have `.test.cpp` suffix
- [ ] Test helpers have `_test_helpers` suffix
- [ ] Doxygen documentation in file header
- [ ] Consistent with this guide

---

## Version History

- **v1.0** (2024): Initial naming convention proposal
  - Established patterns for classes, free functions, templates, and test helpers
  - Defined namespace alignment strategy
  - Created extensibility guidelines

---

## Questions and Discussion

For questions about this naming system:
1. Check this document first
2. Look at examples in the codebase
3. When in doubt, ask the team

For proposing changes to this system:
1. Document the problem with current convention
2. Propose alternative with examples
3. Discuss trade-offs with team
4. Update this document after consensus
