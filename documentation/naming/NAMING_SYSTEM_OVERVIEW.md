# File Naming System Overview

## Purpose

This document provides a high-level overview of the SteamRot file naming conventions and directs you to detailed resources.

## The Problem We're Solving

The SteamRot project had inconsistent file naming that made it difficult to:
- Distinguish between classes and free functions at a glance
- Know where to add new utility functions
- Organize growing subsystems (especially logic)
- Maintain clear separation between production and test code
- Scale the codebase as more functions are added

## The Solution: A Cohesive Naming System

We've established clear patterns that provide:
1. **Visual distinction** between file types
2. **Namespace alignment** for easy navigation
3. **Extensibility** as subsystems grow
4. **Consistency** across the codebase

## Quick Start

### I'm creating a new file, what do I name it?

Use this decision tree:

```
Creating a class? → ClassName.h/cpp
Creating free functions? → subsystem_category.h/cpp
Creating template utilities? → SubsystemUtils.h
Creating test helpers? → subsystem_test_helpers.h/cpp
Writing unit tests? → ModuleName.test.cpp
```

**See:** [NAMING_QUICK_REFERENCE.md](NAMING_QUICK_REFERENCE.md) for detailed examples

### I need to add functions to an existing namespace

Open the appropriate `subsystem_category.h/cpp` file and add them there.

**Example:** Adding collision functions to logic subsystem:
- Open `src/logic/logic_collision.h/cpp`
- Add your function to `namespace steamrot::logic::collision`
- Done!

### I need to create a new category of functions

Create a new file following the pattern: `subsystem_category.h/cpp`

**Example:** Adding rendering utilities to logic subsystem:
- Create `src/logic/logic_render.h/cpp`
- Use namespace `steamrot::logic::render`
- Follows the established pattern

## Documentation Structure

### For Quick Reference
📄 **[NAMING_QUICK_REFERENCE.md](NAMING_QUICK_REFERENCE.md)** (5KB)
- Decision trees
- Pattern examples
- Quick lookup tables
- Anti-patterns to avoid

**Best for:** Daily development, quick lookups

### For Complete Understanding
📕 **[FILE_NAMING_CONVENTIONS.md](FILE_NAMING_CONVENTIONS.md)** (12KB)
- Complete rationale
- Detailed patterns and rules
- Namespace guidelines
- Migration strategy
- Benefits analysis

**Best for:** Understanding the system, making decisions, onboarding

### For Visual Learners
📊 **[naming_system_diagram.md](naming_system_diagram.md)** (17KB)
- ASCII decision trees
- Visual comparisons
- Growth patterns
- Step-by-step guides
- Color-coded examples

**Best for:** Understanding relationships, seeing the big picture

### For Concrete Examples
📚 **[examples/naming_examples/README.md](examples/naming_examples/README.md)** (8KB)
- Real scenario walkthroughs
- Directory structure examples
- Naming convention examples
- Implementation patterns

**Best for:** Seeing it in action, understanding patterns

## The System at a Glance

### File Naming Patterns

| Type | Pattern | Example | Namespace |
|------|---------|---------|-----------|
| **Class** | `ClassName.h/cpp` | `EntityManager.h` | `steamrot` |
| **Functions** | `subsystem_category.h/cpp` | `logic_collision.h` | `steamrot::logic::collision` |
| **Templates** | `SubsystemUtils.h` | `ArchetypeUtils.h` | `steamrot::subsystem` |
| **Test Helpers** | `subsystem_test_helpers.h/cpp` | `logic_test_helpers.h` | `steamrot::tests::subsystem` |
| **Tests** | `ModuleName.test.cpp` | `EntityManager.test.cpp` | N/A |

### Key Principles

1. **PascalCase** = Classes and template utilities
2. **snake_case** = Free function files and test helpers
3. **Subsystem prefix** = Makes function files obvious (e.g., `logic_collision.h`)
4. **Namespace matches filename** = Easy to navigate
5. **Test suffix** = Clear separation (`_test_helpers`)

## Example: Logic Subsystem

A well-organized logic subsystem looks like:

```
src/logic/
├── Logic.h/cpp                    # Base class
├── UIActionLogic.h/cpp           # Derived class
├── LogicFactory.h/cpp            # Factory class
├── logic_collision.h/cpp         # Functions: collision utilities
├── logic_ui.h/cpp                # Functions: UI utilities
└── LogicUtils.h                  # Templates: logic utilities

tests/logic/
├── UIActionLogic.test.cpp        # Tests for class
├── logic_collision.test.cpp      # Tests for functions
└── logic_test_helpers.h/cpp      # Test utilities
```

**Clear distinctions:**
- Capital letter = Class
- `logic_` prefix = Functions in logic subsystem
- `_test_helpers` = Test utilities
- `.test.cpp` = Unit tests

## When to Use Each Pattern

### Use `ClassName.h/cpp` when:
- Creating a class with member variables and methods
- Building managers, factories, handlers
- Implementing polymorphic behavior

### Use `subsystem_category.h/cpp` when:
- Creating utility functions for a subsystem
- Functions don't need to maintain state
- Grouping related operations by domain

### Use `SubsystemUtils.h` when:
- Creating template-only utilities
- Header-only implementations
- Generic algorithms for a subsystem

### Use `subsystem_test_helpers.h/cpp` when:
- Creating test utilities for a subsystem
- Helper functions used across multiple test files
- Mock data providers or validation functions

## Migration Strategy

### For New Code
✅ **Use the new conventions immediately** - no exceptions

### For Existing Code
⏳ **Migrate opportunistically:**
1. Test helpers first (easiest, least risk)
2. Template utilities second
3. Production code when convenient

❌ **No need for a big refactor** - existing files can remain

## Common Questions

### Q: What if I'm not sure which pattern to use?

**A:** Ask yourself:
1. Am I creating a class? → Use `ClassName.h`
2. Am I creating functions? → Use `subsystem_category.h`
3. Still not sure? → Check [NAMING_QUICK_REFERENCE.md](NAMING_QUICK_REFERENCE.md)

### Q: What about files that don't fit these patterns?

**A:** For cross-cutting concerns:
- Use `common_utilities.h` or similar
- Document clearly why it doesn't fit a subsystem
- Keep to a minimum

### Q: Do I need to rename existing files?

**A:** No! Existing files are grandfathered. The key is:
- New files follow the convention
- Update when convenient (not required)

### Q: What about single-function files?

**A:** Still use the namespace pattern for consistency. A file with one function is fine!

### Q: How do I handle growing files?

**A:** If a category file grows too large:
1. Consider splitting into sub-categories
2. Create new files: `subsystem_category_subcategory.h`
3. Keep the namespace structure aligned

## Benefits Summary

### For Developers
- ✅ Know what's in a file by its name
- ✅ Find where to add new functions instantly
- ✅ Navigate codebase faster
- ✅ Less mental overhead

### For the Project
- ✅ Consistent patterns
- ✅ Scales as code grows
- ✅ Clear subsystem boundaries
- ✅ Easier onboarding
- ✅ Better IDE support

### For Extensibility
- ✅ Add functions without creating files
- ✅ Create new categories easily
- ✅ No naming conflicts
- ✅ Clear ownership of code

## Implementation Checklist

When creating a new file:

- [ ] Determined file type (class, functions, templates, test)
- [ ] Chosen appropriate naming pattern
- [ ] Used correct namespace structure
- [ ] Added file to CMakeLists.txt
- [ ] Added Doxygen documentation
- [ ] Verified consistency with this guide

## Getting Help

1. **Quick question?** → Check [NAMING_QUICK_REFERENCE.md](NAMING_QUICK_REFERENCE.md)
2. **Need examples?** → See [examples/naming_examples/README.md](examples/naming_examples/README.md)
3. **Want complete details?** → Read [FILE_NAMING_CONVENTIONS.md](FILE_NAMING_CONVENTIONS.md)
4. **Visual learner?** → Review [naming_system_diagram.md](naming_system_diagram.md)
5. **Still stuck?** → Ask the team

## Related Documentation

- **README.md** - Project overview and workflows
- **.github/copilot-instructions.md** - AI agent guidelines
- **documentation/examples/** - Code examples and patterns

## Version

- **v1.0** (2024) - Initial naming system documentation

---

**Remember:** The goal is clarity and consistency. When in doubt, prioritize making your intent obvious to other developers.
