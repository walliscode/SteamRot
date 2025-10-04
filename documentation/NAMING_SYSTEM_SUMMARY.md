# File Naming System - Implementation Summary

## What Was Created

This PR implements a comprehensive file naming system for the SteamRot project, addressing the need for clear distinction between classes, free functions, test helpers, and providing extensibility for growing subsystems.

## The Problem

The original issue identified several challenges:
1. Mixed naming conventions between files
2. No clear way to separate free functions from classes
3. Difficulty identifying test helper functions
4. Limited extensibility as subsystems (like logic) grow
5. No clear namespace organization strategy

## The Solution

A cohesive naming system with:
- **Visual distinction** through naming patterns
- **Namespace alignment** for easy navigation
- **Subsystem prefixes** for extensibility
- **Consistent patterns** across production and test code

## Naming Patterns Established

### Production Code

| Type | Pattern | Example | Namespace |
|------|---------|---------|-----------|
| **Classes** | `ClassName.h/cpp` | `EntityManager.h` | `steamrot::ClassName` |
| **Functions** | `subsystem_category.h/cpp` | `logic_collision.h` | `steamrot::subsystem::category` |
| **Templates** | `SubsystemUtils.h` | `ArchetypeUtils.h` | `steamrot::subsystem` |

### Test Code

| Type | Pattern | Example | Namespace |
|------|---------|---------|-----------|
| **Test Helpers** | `subsystem_test_helpers.h/cpp` | `logic_test_helpers.h` | `steamrot::tests::subsystem` |
| **Unit Tests** | `ModuleName.test.cpp` | `logic_collision.test.cpp` | N/A |

## Documentation Created

### Core Guides (43 KB)

1. **NAMING_SYSTEM_OVERVIEW.md** (8 KB)
   - Quick start guide
   - High-level overview
   - Navigation to other docs
   - Common questions

2. **NAMING_QUICK_REFERENCE.md** (6 KB)
   - Decision trees
   - Pattern lookup tables
   - Daily reference
   - Anti-patterns

3. **FILE_NAMING_CONVENTIONS.md** (12 KB)
   - Complete rationale
   - Detailed rules
   - Migration strategy
   - Benefits analysis
   - Extensibility guidelines

4. **naming_system_diagram.md** (17 KB)
   - Visual decision trees
   - ASCII diagrams
   - Growth patterns
   - Before/after comparisons
   - Step-by-step guides

### Supporting Documentation (14 KB)

5. **examples/naming_examples/README.md** (8 KB)
   - Real-world scenarios
   - Directory structure examples
   - Migration paths
   - Concrete walkthroughs

6. **documentation/README.md** (6 KB)
   - Documentation index
   - Navigation guide
   - Finding what you need

### Integration

7. **README.md** - Added File Naming Conventions section
8. **.github/copilot-instructions.md** - Added naming pattern reference

## Key Features

### Visual Distinction
```
✅ Clear patterns:
   ClassName.h              → Obviously a class
   logic_collision.h        → Obviously functions (logic subsystem)
   LogicUtils.h             → Obviously templates
   logic_test_helpers.h     → Obviously test utilities
```

### Namespace Alignment
```cpp
// File: src/logic/logic_collision.h
namespace steamrot::logic::collision {
  // Functions here
}

// File: src/entity/entity_memory.h  
namespace steamrot::entity::memory {
  // Functions here
}
```

### Extensibility

**Adding to existing category:**
```cpp
// Open: src/logic/logic_collision.h
namespace steamrot::logic::collision {
  bool IsMouseOverBounds(...);     // existing
  void CheckBoundingBox(...);      // new - just add here
}
```

**Creating new category:**
```cpp
// Create: src/logic/logic_render.h
namespace steamrot::logic::render {
  void DrawDebugBounds(...);       // new category
  void RenderGrid(...);
}
```

## Benefits

### For Developers
- ✅ Know what's in a file by its name
- ✅ Find where to add new functions instantly
- ✅ Navigate codebase faster
- ✅ Consistent patterns reduce cognitive load

### For the Project
- ✅ Scales as subsystems grow
- ✅ Prevents naming conflicts
- ✅ Clear subsystem boundaries
- ✅ Better IDE autocomplete
- ✅ Easier onboarding

### For Extensibility
- ✅ Add functions without new files
- ✅ Create new categories easily
- ✅ Subsystem prefixes prevent collisions
- ✅ Clear ownership of code

## Adoption Strategy

### Immediate
✅ **All new files use the convention**
- No exceptions for new code
- Clear patterns to follow
- Documentation available

### Optional
⏳ **Migrate existing files opportunistically**
- When refactoring a subsystem
- When making significant changes
- Priority: test helpers → templates → production code

### No Requirement
❌ **No forced refactoring**
- Existing files can remain
- Grandfathered in
- Migration is a choice, not a mandate

## Example Usage

### Scenario: Adding Validation Functions

**Decision:** Functions for logic subsystem
**Solution:** Create `logic_validation.h/cpp`

```cpp
// src/logic/logic_validation.h
namespace steamrot::logic::validation {
  bool IsValidLogicContext(const LogicContext& ctx);
  bool HasRequiredComponents(const ArchetypeID& id);
  std::expected<std::monostate, FailInfo> ValidateState(...);
}

// tests/logic/logic_validation.test.cpp
TEST_CASE("IsValidLogicContext with valid context", "[logic_validation]") {
  // ...
}
```

**Result:**
- Clear naming: `logic_validation.h` obviously contains logic validation functions
- Aligned namespace: `steamrot::logic::validation`
- Extensible: Easy to add more validation functions
- Testable: Test file name matches module name

### Scenario: Growing Entity Subsystem

**Current:**
```
src/entity/
├── EntityManager.h/cpp
├── ArchetypeManager.h/cpp
└── emp_helpers.h/cpp (existing)
```

**Future (following convention):**
```
src/entity/
├── EntityManager.h/cpp              # Class
├── ArchetypeManager.h/cpp           # Class
├── entity_memory.h/cpp              # Functions (new name for emp_helpers)
├── entity_configuration.h/cpp       # Functions (new)
├── entity_validation.h/cpp          # Functions (new)
└── EntityUtils.h                    # Templates (new)
```

**Benefits:**
- All entity functions clearly identified with `entity_` prefix
- Easy to add new categories
- Consistent with other subsystems

## Documentation Structure

```
documentation/
├── README.md                           # Documentation index
│
├── Naming System/
│   ├── NAMING_SYSTEM_OVERVIEW.md       # ⭐ Start here
│   ├── NAMING_QUICK_REFERENCE.md       # 📋 Daily use
│   ├── FILE_NAMING_CONVENTIONS.md      # 📕 Complete guide
│   ├── naming_system_diagram.md        # 📊 Visual guide
│   └── NAMING_SYSTEM_SUMMARY.md        # 📄 This file
│
└── examples/
    └── naming_examples/
        └── README.md                   # 📚 Concrete examples
```

## Getting Started

### For New Developers
1. Read [NAMING_SYSTEM_OVERVIEW.md](NAMING_SYSTEM_OVERVIEW.md)
2. Bookmark [NAMING_QUICK_REFERENCE.md](NAMING_QUICK_REFERENCE.md)
3. Browse [examples/naming_examples/](examples/naming_examples/)

### For Daily Work
- Quick lookup: [NAMING_QUICK_REFERENCE.md](NAMING_QUICK_REFERENCE.md)
- Decision trees in overview and quick reference
- Use the flowcharts when uncertain

### For Deep Understanding
- Complete rationale: [FILE_NAMING_CONVENTIONS.md](FILE_NAMING_CONVENTIONS.md)
- Visual examples: [naming_system_diagram.md](naming_system_diagram.md)
- Real scenarios: [examples/naming_examples/README.md](examples/naming_examples/README.md)

## Success Metrics

This naming system succeeds if:
- ✅ New files consistently follow the patterns
- ✅ Developers can quickly find where to add new functions
- ✅ Subsystems grow without naming conflicts
- ✅ File types are immediately obvious from names
- ✅ Onboarding becomes easier with clear conventions

## Future Considerations

### Potential Extensions
- Create automated linting for naming conventions
- Add naming pattern checks to CI/CD
- Generate namespace documentation automatically
- Create IDE snippets for common patterns

### Evolution
The naming system can evolve as the project grows:
- Add new patterns for new needs
- Refine existing patterns based on usage
- Update documentation with lessons learned
- Incorporate community feedback

## Conclusion

This PR provides a complete, cohesive file naming system that:

1. ✅ Addresses all concerns in the original issue
2. ✅ Provides clear visual distinction between file types
3. ✅ Enables easy extensibility with subsystem prefixes
4. ✅ Includes comprehensive documentation (57KB)
5. ✅ Supports gradual adoption (no forced refactoring)
6. ✅ Integrates with existing project documentation
7. ✅ Scales as the project grows

The system is **immediately usable** for all new code and provides **optional migration paths** for existing code. Documentation is **comprehensive yet accessible** with multiple entry points for different needs.

---

**Status:** ✅ Complete and ready for review

**Documentation:** 57 KB across 6 new files + 2 updates

**Adoption:** New code uses conventions, existing code optional

**Next Steps:** Review, feedback, and start using for new files
