# Adding UI Elements

[← Back to Documentation](../README.md) | [Workflows Overview](../README.md#workflows)

This guide explains how to add new UI element types to SteamRot. UI elements are derived from `UIElement` and designed as pure data containers.

**Related Documentation:**
- [Testing Overview](../testing/TESTING_OVERVIEW.md) - UI element testing
- [Style Guide](../style/STYLE_GUIDE.md) - Code conventions

---

### UI Elements

#### Adding New Element types

Each element is derived from UIElement contained in `src/user_interface/`,
UIElement contains a virtual destructor to allow for polymorphism

The UIElement and derived types are designed to be pure data containers with no
methods.

The UIElement contains data common to all UI elements such as position, size,
visibility e.t.c.

Once a new UIElement type has been created, a style and drawing method will need
to be created for it.

Creating tests for this is covered under Testing
