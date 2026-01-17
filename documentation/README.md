# Provider and Configurator Standardization - Documentation Index

## Overview

This directory contains comprehensive documentation for standardizing Provider and Configurator interfaces throughout the SteamRot codebase.

## Problem Statement

The codebase currently has inconsistent patterns for Provider and Configurator interfaces. This standardization effort aims to:

1. Create a consistent, predictable pattern for all data providers
2. Extract reusable configuration logic into testable free functions
3. Improve code maintainability and testability
4. Simplify the interface landscape by merging related concepts

## Documentation Structure

### 1. 📊 [PROVIDER_INTERFACE_STANDARDIZATION.md](PROVIDER_INTERFACE_STANDARDIZATION.md)

**Purpose**: Comprehensive analysis and proposal document

**Contents**:
- Current state analysis of all Provider and Configurator interfaces
- Problems with existing approach
- Proposed solution with detailed examples
- Templating decision analysis (No templates recommendation)
- Migration strategy overview
- Benefits and trade-offs
- Questions for discussion

**Audience**: Technical leads, architects, senior developers

**Read this if**: You need to understand the "why" behind the standardization or want the complete technical analysis.

---

### 2. 🛠️ [PROVIDER_IMPLEMENTATION_GUIDE.md](PROVIDER_IMPLEMENTATION_GUIDE.md)

**Purpose**: Practical, hands-on guide for implementing the pattern

**Contents**:
- Step-by-step provider creation process
- Code templates for interfaces, free functions, and implementations
- Testing patterns and examples
- Common patterns (required/optional fields, nested configs, collections)
- Best practices and anti-patterns
- File organization strategies

**Audience**: All developers implementing or modifying providers

**Read this if**: You're implementing a new provider or migrating an existing one and need concrete examples.

---

### 3. 📋 [PROVIDER_MIGRATION_PLAN.md](PROVIDER_MIGRATION_PLAN.md)

**Purpose**: Detailed, task-by-task migration plan

**Contents**:
- Complete inventory of existing Providers and Configurators
- Prioritized task list with complexity estimates
- Step-by-step migration instructions for each interface
- Success criteria and testing requirements
- Timeline estimates and dependencies
- Risk assessment
- Progress tracking metrics

**Audience**: Project managers, technical leads, developers executing migration

**Read this if**: You need to understand the scope, timeline, and specific tasks involved in the migration.

---

### 4. ⚡ [PROVIDER_QUICK_REFERENCE.md](PROVIDER_QUICK_REFERENCE.md)

**Purpose**: Quick reference card for developers

**Contents**:
- TL;DR summary of the pattern
- Code templates (copy-paste ready)
- Naming conventions table
- Common patterns cheat sheet
- Checklists for implementation and testing
- Anti-patterns to avoid

**Audience**: All developers (quick lookup)

**Read this if**: You know the pattern and just need a quick reminder or template.

---

## Key Decisions

### Decision 1: No Templates ✅

**Rationale**: Pattern is simple enough that templates would add complexity without sufficient benefit. Rely on clear documentation, conventions, and code review instead.

**Alternatives Considered**:
- Template helpers for return types
- CRTP for compile-time polymorphism

**See**: PROVIDER_INTERFACE_STANDARDIZATION.md, "Templating Decision Analysis" section

---

### Decision 2: Free Functions for Configuration ✅

**Rationale**: Extracting configuration logic into free functions provides:
- Better testability (functions testable in isolation)
- Reusability (logic can be shared across implementations)
- Maintainability (changes don't require modifying classes)
- Flexibility (new data sources can reuse existing logic)

**Pattern**: 
```cpp
namespace steamrot::config {
  std::expected<std::monostate, FailInfo>
  configure_my_data(MyData& data, const MyDataFbs* fb_data);
}
```

**See**: PROVIDER_IMPLEMENTATION_GUIDE.md, "Step 2: Create Configuration Free Functions"

---

### Decision 3: Standardize on `Provide()` Method Name ✅

**Rationale**: Consistency across interfaces. `Provide()` clearly indicates the interface's purpose.

**Migration**: 
- `LoadEngineData()` → `Provide()`
- `ProvideSaveData()` → `Provide()`
- `GetFont()` → May remain as-is (simple getter)

**Exceptions**: Multiple provide methods can have descriptive suffixes:
- `ProvideDefault()`
- `ProvideFromData()`

**See**: PROVIDER_IMPLEMENTATION_GUIDE.md, "Naming Conventions"

---

### Decision 4: Merge Configurators into Providers ✅

**Rationale**: Separate Configurator interfaces add complexity without clear benefit. Providers can have `Configure()` methods alongside `Provide()` methods.

**Migration Strategy**:
- Phase 1: Extract free functions from Configurators
- Phase 2: Add `Configure()` methods to Provider interfaces
- Phase 3: Migrate call sites to use Providers
- Phase 4: Remove Configurator interfaces

**See**: PROVIDER_MIGRATION_PLAN.md, "Phase 3: Configurator Migration"

---

## Implementation Status

### Phase 1: Foundation ✅ COMPLETE
- [x] Analysis document created
- [x] Implementation guide created
- [x] Migration plan created
- [x] Quick reference created
- [x] Templating decision finalized

### Phase 2: Data Provider Migration ⏳ NOT STARTED
- [ ] IEngineDataProvider (Priority 1)
- [ ] ISaveDataProvider (Priority 2)
- [ ] ISceneManagerDataProvider (Priority 3)
- [ ] IUIStyleDataProvider (Priority 4)
- [ ] ISceneDataProvider (Priority 5)
- [ ] IFontProvider (Priority 6)

### Phase 3: Configurator Migration ⏳ NOT STARTED
- [ ] ISceneConfigurator (Priority 7)
- [ ] IUIElementConfigurator (Priority 8)
- [ ] IEntityConfigurator (Priority 9)

### Phase 4: Final Cleanup ⏳ NOT STARTED
- [ ] Documentation update
- [ ] Code review
- [ ] Performance review
- [ ] Final testing

**See**: PROVIDER_MIGRATION_PLAN.md for detailed task breakdown

---

## Naming Conventions Summary

| Element | Convention | Example |
|---------|-----------|---------|
| Interface | `I<Domain>Provider` | `IEngineDataProvider` |
| Concrete class | `Flatbuffers<Domain>Provider` | `FlatbuffersEngineDataProvider` |
| Primary method | `Provide()` | `Provide()`, `ProvideDefault()` |
| Configure method | `Configure()` | `Configure(Scene&)` |
| Free function | `configure_<domain>_<aspect>()` | `configure_engine_config()` |
| Free function file | `<domain>_config.h/cpp` | `engine_data_config.h` |
| Free function namespace | `steamrot::config` | `steamrot::config` |

---

## File Organization

```
src/
├── types/interfaces/
│   └── I<Domain>Provider.h           # Provider interfaces
├── <domain>/
│   ├── Flatbuffers<Domain>Provider.h  # Concrete implementations
│   ├── Flatbuffers<Domain>Provider.cpp
│   ├── <domain>_config.h             # Configuration free functions
│   └── <domain>_config.cpp

tests/unit/<domain>/
├── Flatbuffers<Domain>Provider.test.cpp  # Provider tests
└── <domain>_config.test.cpp             # Free function tests
```

---

## Quick Start for Developers

### Creating a New Provider

1. Read: [PROVIDER_QUICK_REFERENCE.md](PROVIDER_QUICK_REFERENCE.md)
2. Follow: [PROVIDER_IMPLEMENTATION_GUIDE.md](PROVIDER_IMPLEMENTATION_GUIDE.md) - "Creating a New Provider"
3. Use templates from quick reference for boilerplate

### Migrating an Existing Provider

1. Find your task in: [PROVIDER_MIGRATION_PLAN.md](PROVIDER_MIGRATION_PLAN.md)
2. Follow step-by-step instructions for your specific interface
3. Refer to [PROVIDER_IMPLEMENTATION_GUIDE.md](PROVIDER_IMPLEMENTATION_GUIDE.md) for patterns
4. Check [PROVIDER_QUICK_REFERENCE.md](PROVIDER_QUICK_REFERENCE.md) for templates

### Understanding the Architecture

1. Start with: [PROVIDER_INTERFACE_STANDARDIZATION.md](PROVIDER_INTERFACE_STANDARDIZATION.md) - "Executive Summary"
2. Read: "Current State Analysis" and "Proposed Solution" sections
3. Review: "Migration Example" for concrete before/after

---

## FAQs

### Q: Why not use templates for enforcing the pattern?

**A**: The pattern is simple enough that templates would add more complexity than value. Clear documentation, code review, and conventions are sufficient. See PROVIDER_INTERFACE_STANDARDIZATION.md, "Templating Decision Analysis".

### Q: Can I still create a Configurator interface?

**A**: Not recommended. New functionality should use the Provider pattern with `Configure()` methods. Existing Configurators will be migrated to Providers.

### Q: What if I need multiple Provide methods?

**A**: Acceptable! Use descriptive names like `ProvideDefault()` and `ProvideFromData()`. See ISceneDataProvider example in PROVIDER_IMPLEMENTATION_GUIDE.md.

### Q: Where do I put free configuration functions?

**A**: In `<domain>_config.h/cpp` files, within `steamrot::config` namespace. See PROVIDER_IMPLEMENTATION_GUIDE.md, "File Organization".

### Q: Do free functions need to be in `steamrot::config` namespace?

**A**: Recommended for consistency, but `steamrot::<domain>::config` is also acceptable for complex domains. Choose one and be consistent.

### Q: How do I test free functions?

**A**: Create unit tests in `tests/unit/<domain>/<domain>_config.test.cpp`. Test with valid data, null data, missing fields, and error cases. See examples in PROVIDER_IMPLEMENTATION_GUIDE.md.

### Q: Can Providers have dependencies like EventHandler?

**A**: Yes! Pass dependencies through constructor and store as member variables. See "Provider with Dependencies" in PROVIDER_IMPLEMENTATION_GUIDE.md.

### Q: What about FlatBuffers null checking?

**A**: Always check! Strings, vectors, and tables can be null. Primitives (int, bool, float) don't need checks. See "FlatBuffers Checklist" in PROVIDER_QUICK_REFERENCE.md.

---

## Contributing

When adding to or updating this documentation:

1. Keep documents synchronized - changes to patterns should update all relevant docs
2. Update this index if adding new documents
3. Add examples for new patterns in PROVIDER_IMPLEMENTATION_GUIDE.md
4. Update migration plan with new tasks or progress
5. Keep quick reference concise - full details go in other docs

---

## Related Documentation

- Main README.md - Project overview and general guidelines
- GitHub Copilot Instructions - AI agent guidelines
- Architecture documentation (future)

---

## Contact

Questions about Provider standardization:
- Open an issue with label `provider-standardization`
- Discuss in team meetings
- Reference this documentation in PRs

---

**Last Updated**: 2026-01-17  
**Status**: Analysis and Planning Complete - Ready for Implementation
