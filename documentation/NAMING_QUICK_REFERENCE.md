# File Naming Quick Reference

Quick guide for naming files in the SteamRot project. See [FILE_NAMING_CONVENTIONS.md](FILE_NAMING_CONVENTIONS.md) for complete details.

## Decision Tree

```
What am I creating?
│
├─ A class?
│  └─ ClassName.h/cpp (PascalCase)
│     namespace steamrot
│
├─ Free functions?
│  └─ subsystem_category.h/cpp (snake_case)
│     namespace steamrot::subsystem::category
│
├─ Template utilities?
│  └─ SubsystemUtils.h (PascalCase, header-only)
│     namespace steamrot::subsystem
│
├─ Test helpers?
│  └─ subsystem_test_helpers.h/cpp (snake_case)
│     namespace steamrot::tests::subsystem
│
└─ Unit tests?
   └─ ModuleName.test.cpp
```

## Examples by Type

### Classes (PascalCase)
```
✅ EntityManager.h/cpp
✅ LogicFactory.h/cpp
✅ UIActionLogic.h/cpp
✅ CUserInterface.h/cpp (Component: C prefix)
```

### Free Functions (snake_case with subsystem prefix)
```
✅ logic_collision.h/cpp       → namespace steamrot::logic::collision
✅ logic_ui.h/cpp              → namespace steamrot::logic::ui
✅ entity_memory.h/cpp         → namespace steamrot::entity::memory
✅ event_conversion.h/cpp      → namespace steamrot::event::conversion
```

### Template Utilities (PascalCase with Utils suffix)
```
✅ ArchetypeUtils.h            → namespace steamrot::entity (header-only)
✅ LogicUtils.h                → namespace steamrot::logic (header-only)
```

### Test Helpers (snake_case with test_helpers suffix)
```
✅ logic_test_helpers.h/cpp    → namespace steamrot::tests::logic
✅ entity_test_helpers.h/cpp   → namespace steamrot::tests::entity
✅ ui_test_helpers.h/cpp       → namespace steamrot::tests::ui
```

### Test Files (.test.cpp)
```
✅ EntityManager.test.cpp      (tests EntityManager class)
✅ logic_collision.test.cpp    (tests logic_collision.h functions)
✅ entity_memory.test.cpp      (tests entity_memory.h functions)
```

## Common Patterns

### Adding Functions to Existing Namespace
Already have `logic_collision.h`? Just add functions to it:
```cpp
// src/logic/logic_collision.h
namespace steamrot::logic::collision {
  bool IsMouseOverBounds(...);    // existing
  void CheckCollision(...);       // new - just add here
}
```

### Creating New Category in Subsystem
Want new rendering utilities in logic?
```cpp
// Create: src/logic/logic_render.h/cpp
namespace steamrot::logic::render {
  void DrawDebugBounds(...);
  void RenderGrid(...);
}
```

### Organizing Test Helpers
```cpp
// tests/logic/logic_test_helpers.h
namespace steamrot::tests::logic {
  void CheckStaticLogicCollections(...);
  void SetupMockLogicContext(...);
  void VerifyLogicExecution(...);
}
```

## Anti-Patterns (Don't Do This)

❌ `helpers.h` - too generic, which subsystem?
❌ `utils.h` - too generic, which subsystem?  
❌ `emp_helpers.h` - unclear abbreviation
❌ `collision.h` - missing subsystem prefix (unless it's a class)
❌ `MyClassHelper.h` - don't make helper classes, use free functions
❌ `test_logic.h` - test helpers should be `logic_test_helpers.h`

## File-to-Namespace Mapping

| File Pattern | Namespace | Example |
|--------------|-----------|---------|
| `ClassName.h` | `steamrot` | EntityManager → `steamrot::EntityManager` |
| `subsystem_category.h` | `steamrot::subsystem::category` | logic_collision.h → `steamrot::logic::collision` |
| `SubsystemUtils.h` | `steamrot::subsystem` | ArchetypeUtils.h → `steamrot::entity` |
| `subsystem_test_helpers.h` | `steamrot::tests::subsystem` | logic_test_helpers.h → `steamrot::tests::logic` |

## Subsystem Examples

### Logic Subsystem (`src/logic/`)
```
Logic.h/cpp                    # Base class
UIActionLogic.h/cpp           # Derived class
LogicFactory.h/cpp            # Factory class
logic_collision.h/cpp         # Free functions: steamrot::logic::collision
logic_ui.h/cpp                # Free functions: steamrot::logic::ui
LogicUtils.h                  # Templates: steamrot::logic
```

### Entity Subsystem (`src/entity/`)
```
EntityManager.h/cpp           # Manager class
ArchetypeManager.h/cpp        # Manager class
entity_memory.h/cpp           # Free functions: steamrot::entity::memory
ArchetypeUtils.h              # Templates: steamrot::entity
```

### Event Subsystem (`src/events/`)
```
EventHandler.h/cpp            # Handler class
Subscriber.h/cpp              # Subscriber class
event_conversion.h/cpp        # Free functions: steamrot::event::conversion
```

## When Adding New Code

### New Class
1. Create `ClassName.h/cpp` in appropriate `src/subsystem/` directory
2. Use namespace `steamrot`
3. Add to subsystem `CMakeLists.txt`

### New Free Functions
1. Determine subsystem (logic, entity, events, etc.)
2. Determine category (collision, memory, conversion, etc.)
3. Create or open `subsystem_category.h/cpp`
4. Use namespace `steamrot::subsystem::category`

### New Test Helpers
1. Create or open `tests/subsystem/subsystem_test_helpers.h/cpp`
2. Use namespace `steamrot::tests::subsystem`
3. Add to test `CMakeLists.txt`

### New Unit Tests
1. Create `tests/subsystem/ModuleName.test.cpp`
2. Name matches what you're testing
3. Add to test `CMakeLists.txt`

## Migration Notes

**Existing files can remain with current names** - this is for new code.

**Priority for refactoring** (if desired):
1. Test helpers (easiest, least disruptive)
2. Template utilities
3. Production free function files (most work, most risk)

## Questions?

See [FILE_NAMING_CONVENTIONS.md](FILE_NAMING_CONVENTIONS.md) for:
- Detailed rationale
- Migration strategy
- Special cases
- More examples
