# Style Guide

[← Back to Documentation](../README.md) | [Style & Conventions](../README.md#naming--style)

This document describes SteamRot's code style and formatting conventions, based on the Google C++ Style Guide with project-specific customizations.

**Related Documentation:**
- [File Naming Conventions](../naming/FILE_NAMING_CONVENTIONS.md) - Naming system
- [Naming Quick Reference](../naming/NAMING_QUICK_REFERENCE.md) - Quick lookup

---

## Style Guide

In general we follow the
[Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).

### File Naming Conventions

The project uses a cohesive naming system to distinguish between classes, free functions, and test utilities. See:

- **[FILE_NAMING_CONVENTIONS.md](documentation/FILE_NAMING_CONVENTIONS.md)** - Complete naming system documentation
- **[NAMING_QUICK_REFERENCE.md](documentation/NAMING_QUICK_REFERENCE.md)** - Quick reference guide

**Quick Summary**:
- **Classes**: `ClassName.h/cpp` (PascalCase) → `steamrot::ClassName`
- **Functions**: All functions use PascalCase (e.g., `GetComponent`, `ProcessUIActionsAndEvents`)
- **Free Function Files**: `subsystem_category.h/cpp` (snake_case files) → `steamrot::subsystem::category`
- **Template Utils**: `SubsystemUtils.h` (PascalCase, header-only) → `steamrot::subsystem`
- **Test Helpers**: `subsystem_test_helpers.h/cpp` (snake_case files) → `steamrot::tests::subsystem`
- **Test Files**: `ModuleName.test.cpp` (matches module being tested)

### Formatting

Certain readability formatting elements have been used by B Wallis. Inspiration
for this (or just directly lifting) has been take from the
[SFML Repository](https://github.com/SFML/SFML/tree/master)

#### Spacing between functions

////////////////////////////////////////////////////////////

#### Funtionality brief

//////////////////////////////////////////////////////////// /// |brief ///
////////////////////////////////////////////////////////////

### Namespace Conventions

SteamRot uses nested namespaces to organize code by subsystem and functionality.

#### Top-Level Namespace

All code is in the `steamrot` namespace:
```cpp
namespace steamrot {
  // All code here
}
```

#### Free Function Namespaces

Free function modules use nested namespaces following the pattern `steamrot::subsystem::category`:

**Logic Functions**:
- `steamrot::logic::action` - Action processing functions (`src/logic/logic_action.{h,cpp}`)
- `steamrot::logic::collision` - Collision detection functions (`src/logic/logic_collision.{h,cpp}`)
- `steamrot::logic::ui` - UI helper functions (`src/logic/logic_ui.{h,cpp}`)
- `steamrot::logic::render` - UI rendering functions (`src/logic/logic_render.{h,cpp}`)

**Event Functions**:
- `steamrot::event` - Event factory functions (creating events from data) (`src/events/event_factory.{h,cpp}`)
- `steamrot::events` - Event processing functions (event bus cycles) (`src/events/event_handler_tick.{h,cpp}`)

**Note**: See [EVENT_NAMESPACES.md](../architecture/EVENT_NAMESPACES.md) for detailed explanation of `event` vs `events`.

**Entity Memory**:
- `steamrot::entity::memory` - Entity memory pool access functions (`src/entity/entity_memory.{h,cpp}`)

#### Test Namespaces

All test infrastructure is in the `steamrot::tests` namespace:
```cpp
namespace steamrot::tests {
  // Test infrastructure here
}
```

#### Namespace Guidelines

1. **Use nested namespaces** for related functionality (e.g., `logic::action`, not `logic_action`)
2. **Be consistent** with existing patterns in the same subsystem
3. **Keep names short** but descriptive (e.g., `render` not `rendering_functions`)
4. **Document distinctions** when similar namespaces exist (like `event` vs `events`)
5. **Avoid abbreviations** unless they're well-established (e.g., `ui` is fine)

#### Adding New Namespaces

When adding a new namespace:
1. Check existing namespace patterns in the subsystem
2. Use nested namespaces for subcategories
3. Document the purpose in a comment at the top of the file
4. Update this style guide
5. Consider if free functions would be better as class methods

[EOF]
