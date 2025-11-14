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

[EOF]
