# Namespace and File Naming Analysis for SteamRot

**Date**: 2026-01-01  
**Purpose**: Comprehensive analysis of namespace organization and file naming conventions across the SteamRot codebase with recommendations for improvements and cleanup.

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Current State Analysis](#current-state-analysis)
3. [Identified Issues](#identified-issues)
4. [Recommendations](#recommendations)
5. [Cleanup Todo List](#cleanup-todo-list)

---

## Executive Summary

### Key Findings

- **Total Source Files**: 171 files (excluding generated FlatBuffers headers)
- **Namespace Usage**: Predominantly uses `namespace steamrot`, with limited nested namespaces
- **File Naming**: Mix of PascalCase, snake_case, and inconsistent patterns
- **Interface Location**: Interfaces split between `src/interfaces/` and `tests/harness/`
- **Nested Namespaces**: Only 7 files use nested namespaces (e.g., `steamrot::event`, `steamrot::logic::ui`)

### Overall Assessment

The codebase shows **moderate consistency** but has several areas where namespace organization and file naming conventions could be improved to enhance maintainability, clarity, and adherence to the documented conventions.

---

## Current State Analysis

### 1. Namespace Organization

#### Primary Patterns

| Pattern | Count | Files | Usage |
|---------|-------|-------|-------|
| `namespace steamrot {` | 127 | Most files | Primary namespace for all production code |
| `namespace steamrot::tests {` | 53 | Test files | Test infrastructure |
| `namespace steamrot::event {` | 3 | events/ | Event factory and conversion utilities |
| `namespace steamrot::events {` | 2 | events/ | Event handler tick functionality |
| `namespace steamrot::logic::ui {` | 2 | logic/ | UI-specific logic helpers |
| `namespace steamrot::archetypes {` | 2 | entity/ | Archetype utilities |
| `namespace steamrot::entity::memory {` | 2 | entity/ | Entity memory management |
| `namespace steamrot::paths {` | 1 | data_providers/ | Path utilities |
| Anonymous `namespace {` | 1 | data_providers/ | Internal helpers |

#### Nested Namespace Usage

**Files with nested namespaces:**
1. `src/events/event_factory.h/cpp` - `steamrot::event`
2. `src/events/event_bus_conversion.h/cpp` - `steamrot::event`
3. `src/events/event_handler_tick.h/cpp` - `steamrot::events`
4. `src/logic/logic_ui.h/cpp` - `steamrot::logic::ui`
5. `src/entity/archetypes.h` - `steamrot::archetypes`
6. `src/entity/entity_memory.h/cpp` - `steamrot::entity::memory`
7. `src/data_providers/paths.h` - `steamrot::paths`

**Inconsistency Note**: Only ~4% of source files use nested namespaces, while 96% use flat `namespace steamrot {}`

### 2. File Naming Conventions

#### Source Code (`src/`)

| Directory | Pattern | Examples | Consistency |
|-----------|---------|----------|-------------|
| **components/** | `PascalCase` with `C` prefix | `CUserInterface.h`, `CGrimoireMachina.h` | ✅ Excellent |
| **logic/** | Mixed | `Logic.h`, `UIActionLogic.h`, `logic_action.h`, `logic_ui.h` | ⚠️ Inconsistent |
| **events/** | Mixed | `EventHandler.h`, `event_factory.h`, `subscriber_factory.h` | ⚠️ Inconsistent |
| **entity/** | Mixed | `EntityManager.h`, `ArchetypeManager.h`, `archetypes.h`, `entity_memory.h` | ⚠️ Inconsistent |
| **data_providers/** | Mixed | `FlatbuffersDataLoader.h`, `data_file_utils.h`, `asset_config_factory.h` | ⚠️ Inconsistent |
| **interfaces/** | `PascalCase` with `I` prefix | `IEngineDataProvider.h`, `ISceneConfigurator.h` | ✅ Excellent |
| **types/** | `PascalCase` | `FailInfo.h`, `SceneInfo.h`, `EventPacket.h` | ✅ Good |

#### Test Code (`tests/`)

| Directory | Pattern | Examples | Consistency |
|-----------|---------|----------|-------------|
| **harness/** | Mixed | `TestEngine.h`, `input_simulation.h`, `ITestDataProvider.h` | ⚠️ Inconsistent |
| **matchers/** | `PascalCase` | `CUserInterfaceEqualsMatcher.h`, `EventBusEqualsMatcher.h` | ✅ Good |
| **context/** | Mixed | `TestFixture.h`, `test_assertions.h` | ⚠️ Inconsistent |

### 3. Directory Structure

#### Source Directories

```
src/
├── assets/               # Asset management (PascalCase classes)
├── components/           # ECS components (CPascalCase)
├── configuration/        # Configuration classes (PascalCase)
├── context/              # Game context (PascalCase)
├── data_providers/       # Data loading/providers (Mixed naming)
├── display/              # Display/rendering (PascalCase)
├── engine/               # Engine core (PascalCase)
├── entity/               # Entity management (Mixed naming)
├── error_loop/           # Error handling (snake_case?)
├── events/               # Event system (Mixed naming)
├── interfaces/           # Abstract interfaces (IPascalCase) ✅
├── logger/               # Logging utilities
├── logic/                # Game logic (Mixed naming)
├── resources/            # Resource management
├── scenes/               # Scene management (PascalCase)
├── types/                # Type definitions (organized by category)
└── user_interface/       # UI configuration (PascalCase)
```

#### Test Directories

```
tests/
├── context/              # Test fixtures and assertions (Mixed)
├── harness/              # Test harness and simulation (Mixed)
├── integration/          # Integration tests
├── matchers/             # Catch2 custom matchers (PascalCase)
├── perf/                 # Performance tests
├── reporters/            # Custom test reporters
├── system/               # System tests
└── unit/                 # Unit tests (organized by src/ structure)
```

---

## Identified Issues

### Critical Issues

#### 1. **Namespace Inconsistency**

**Problem**: Minimal use of nested namespaces despite having clear subsystems.

- **Events subsystem**: Uses both `steamrot::event` and `steamrot::events` (singular vs plural)
- **Logic subsystem**: Mostly flat `steamrot`, but `logic_ui.h` uses `steamrot::logic::ui`
- **Entity subsystem**: Mostly flat, but `archetypes.h` uses `steamrot::archetypes`

**Impact**: Namespace pollution, potential name collisions, unclear module boundaries.

#### 2. **File Naming Inconsistency**

**Problem**: Mixed PascalCase and snake_case within the same directories.

**Examples**:
- `logic/`: `UIActionLogic.h` (PascalCase) vs `logic_action.h` (snake_case)
- `events/`: `EventHandler.h` (PascalCase) vs `event_factory.h` (snake_case)
- `entity/`: `EntityManager.h` (PascalCase) vs `entity_memory.h` (snake_case)

**Impact**: Confusion about what naming to use for new files, harder to predict file names.

#### 3. **Interface Location Split**

**Problem**: Interfaces exist in both `src/interfaces/` and `tests/harness/`.

**Current State**:
- **Production interfaces**: `src/interfaces/` (7 files, all with `I` prefix) ✅
- **Test interfaces**: `tests/harness/ITestDataProvider.h` ❌

**Impact**: Unclear where to place new interfaces, breaks organizational consistency.

### Moderate Issues

#### 4. **Singular vs Plural Namespace Names**

**Problem**: `steamrot::event` vs `steamrot::events` (both exist in events subsystem).

- `event_factory.h` uses `steamrot::event`
- `event_handler_tick.h` uses `steamrot::events`

**Impact**: Confusion, inconsistency, unclear which to use for new files.

#### 5. **Helper Function File Naming**

**Problem**: Helper/utility functions use inconsistent naming patterns.

**Examples**:
- `logic/logic_action.h` - Contains free functions in `steamrot::logic::action` namespace
- `entity/archetypes.h` - Contains free functions in `steamrot::archetypes` namespace
- `data_providers/data_file_utils.h` - Contains free functions in `steamrot::data` namespace

**Observation**: Files with free functions tend to use snake_case, but this isn't documented or consistent.

#### 6. **Test Infrastructure Namespace**

**Problem**: Test infrastructure consistently uses `steamrot::tests`, but organization could be clearer.

**Current**:
- All test helpers, matchers, harness, fixtures use `steamrot::tests`
- No further subdivision (e.g., `steamrot::tests::matchers`, `steamrot::tests::harness`)

**Impact**: Flat namespace for all test utilities could lead to name collisions as tests grow.

### Minor Issues

#### 7. **Anonymous Namespace Usage**

**Status**: Only 1 file uses anonymous namespace (`FlatbuffersUIStyleDataProvider.cpp`).

**Note**: This is actually good - anonymous namespaces are appropriately rare.

#### 8. **Prefix Inconsistency**

**Components**: Use `C` prefix (e.g., `CUserInterface`, `CGrimoireMachina`) ✅  
**Interfaces**: Use `I` prefix (e.g., `IEngineDataProvider`, `ISceneConfigurator`) ✅  
**Other Classes**: No prefix pattern

**Status**: Current patterns are good, but worth documenting explicitly.

---

## Recommendations

### High Priority Recommendations

#### R1: Standardize Event Subsystem Namespace

**Action**: Choose ONE namespace for events subsystem.

**Option A - Singular** (Recommended):
```cpp
// Consolidate to steamrot::event
namespace steamrot::event {
  // EventHandler, event_factory, event_bus_conversion, subscriber_factory
}
```

**Option B - Plural**:
```cpp
// Consolidate to steamrot::events
namespace steamrot::events {
  // All event-related code
}
```

**Rationale**: Singular matches the directory name pattern better and aligns with C++ standard practices (e.g., `std::chrono`, `std::filesystem`).

#### R2: Introduce Consistent Nested Namespaces

**Action**: Use nested namespaces for major subsystems that have multiple files and helper utilities.

**Recommended Structure**:

```cpp
// Logic subsystem
namespace steamrot::logic {
  // Logic.h, LogicFactory.h
  namespace action { /* logic_action.h helpers */ }
  namespace collision { /* logic_collision.h helpers */ }
  namespace render { /* logic_render.h helpers */ }
  namespace ui { /* logic_ui.h helpers */ }
}

// Event subsystem
namespace steamrot::event {
  // EventHandler.h, event_factory.h, event_bus_conversion.h
  namespace factory { /* event_factory.h */ }
  namespace subscriber { /* subscriber_factory.h */ }
}

// Entity subsystem
namespace steamrot::entity {
  // EntityManager.h, ArchetypeManager.h
  namespace archetypes { /* archetypes.h */ }
  namespace memory { /* entity_memory.h */ }
}

// Data subsystem
namespace steamrot::data {
  namespace providers { /* FlatbuffersDataLoader, etc. */ }
  namespace paths { /* paths.h */ }
  namespace utils { /* data_file_utils.h */ }
}
```

#### R3: Standardize File Naming Convention

**Action**: Adopt clear rules based on file content type.

**Proposed Rules**:

| Content Type | Naming Pattern | Examples |
|--------------|----------------|----------|
| **Class definitions** | `PascalCase.h/cpp` | `EntityManager.h`, `LogicFactory.h` |
| **Free function utilities** | `snake_case.h/cpp` | `entity_utils.h`, `event_factory.h` |
| **Components** | `CPascalCase.h/cpp` | `CUserInterface.h`, `CMeta.h` |
| **Interfaces** | `IPascalCase.h` | `IEngineDataProvider.h`, `ITestDataProvider.h` |
| **Type definitions** | `PascalCase.h` | `FailInfo.h`, `EventPacket.h` |

**Benefits**: Predictable file naming, clear distinction between classes and utilities.

#### R4: Move Test Interfaces to Dedicated Directory

**Action**: Create `tests/interfaces/` or move to `tests/context/`.

**Current Issue**:
```
tests/harness/ITestDataProvider.h  ❌
```

**Option A** (Recommended - Mirror Production):
```
tests/interfaces/ITestDataProvider.h  ✅
```

**Option B** (Consolidate with Test Context):
```
tests/context/ITestDataProvider.h  ✅
```

**Rationale**: Interfaces should have a consistent home, mirroring the production pattern.

### Medium Priority Recommendations

#### R5: Document Namespace Strategy

**Action**: Add a `NAMESPACE_STRATEGY.md` document to `documentation/`.

**Content**:
- When to use nested namespaces
- Naming conventions (singular vs plural)
- How to organize free functions
- Interface placement rules

#### R6: Subdivide Test Namespace

**Action**: Introduce nested namespaces for test infrastructure.

**Proposed**:
```cpp
namespace steamrot::tests {
  namespace matchers { /* Custom Catch2 matchers */ }
  namespace harness { /* Test harness infrastructure */ }
  namespace fixtures { /* Test fixtures */ }
  namespace assertions { /* Custom assertions */ }
}
```

#### R7: Consolidate Helper Function Files

**Action**: Review helper function files and ensure consistent organization.

**Pattern**:
- If helpers are tightly coupled to a class → Keep in same file or adjacent
- If helpers are general utilities → Group in `*_utils.h` files
- Use nested namespaces to organize helper groups

### Low Priority Recommendations

#### R8: Add EditorConfig for Consistency

**Status**: Already exists (✅), but verify configuration.

**Action**: Ensure `.editorconfig` enforces:
- 2-space indentation
- UTF-8 encoding
- LF line endings
- Trim trailing whitespace

#### R9: Create Naming Convention Quick Reference

**Action**: Add a quick reference card to the main README.

**Example Section**:
```markdown
## Naming Quick Reference

| Element | Pattern | Example |
|---------|---------|---------|
| Class | PascalCase | `EntityManager` |
| Component | CPascalCase | `CUserInterface` |
| Interface | IPascalCase | `IDataProvider` |
| Free Functions | snake_case file | `entity_utils.h` |
| Namespace | lowercase | `steamrot::event` |
```

---

## Cleanup Todo List

### Phase 1: Documentation (High Priority, Low Risk)

- [ ] **DOC-1**: Create `NAMESPACE_STRATEGY.md` in `documentation/`
  - Define when to use nested namespaces
  - Document singular vs plural naming
  - Provide examples for each subsystem
  - Define interface placement rules

- [ ] **DOC-2**: Update `README.md` with namespace quick reference
  - Add "Namespace Organization" section
  - Document current patterns
  - Link to `NAMESPACE_STRATEGY.md`

- [ ] **DOC-3**: Update `.github/copilot-instructions.md`
  - Add namespace organization guidelines
  - Document file naming rules by content type
  - Clarify interface placement

### Phase 2: Test Infrastructure Reorganization (Medium Priority, Low Risk)

- [ ] **TEST-1**: Create `tests/interfaces/` directory
  - Rationale: Mirror production structure

- [ ] **TEST-2**: Move `tests/harness/ITestDataProvider.*` to `tests/interfaces/`
  - Update `#include` paths in dependent files
  - Update `tests/harness/CMakeLists.txt`
  - Update `tests/interfaces/CMakeLists.txt` (create if needed)

- [ ] **TEST-3**: Introduce nested test namespaces (gradual)
  - Start with new files using `steamrot::tests::matchers`, etc.
  - Document the pattern in `NAMESPACE_STRATEGY.md`
  - Don't mass-refactor existing files yet

- [ ] **TEST-4**: Update test harness README
  - Document namespace organization
  - Clarify interface location

### Phase 3: Event Subsystem Standardization (High Priority, Medium Risk)

- [ ] **EVENT-1**: Decide on singular vs plural (Recommend: `steamrot::event`)

- [ ] **EVENT-2**: Standardize namespace in event files
  - Update `event_handler_tick.h/cpp` from `steamrot::events` → `steamrot::event`
  - Verify no external dependencies break
  - Run full test suite

- [ ] **EVENT-3**: Consider nested namespaces for event utilities
  - Evaluate `steamrot::event::factory` for `event_factory.h`
  - Evaluate `steamrot::event::subscriber` for `subscriber_factory.h`
  - Document decision in `NAMESPACE_STRATEGY.md`

### Phase 4: File Naming Standardization (Medium Priority, Medium Risk)

**Note**: This is a larger refactoring effort. Prioritize consistency for NEW files first.

#### Logic Subsystem

- [ ] **LOGIC-1**: Decide on naming convention for helper files
  - Option A: Keep `logic_action.h` (snake_case for free functions)
  - Option B: Rename to `LogicAction.h` (PascalCase for consistency)
  - Document decision and rationale

- [ ] **LOGIC-2**: Apply chosen convention to logic subsystem
  - Rename files if needed (use git mv)
  - Update all `#include` directives
  - Update `CMakeLists.txt`
  - Run tests after each change

#### Entity Subsystem

- [ ] **ENTITY-1**: Standardize entity file naming
  - Decide: `entity_memory.h` vs `EntityMemory.h`
  - Decide: `entity_types.h` vs `EntityTypes.h`
  - Apply consistently

- [ ] **ENTITY-2**: Consider moving free functions to `entity_utils.h`
  - Evaluate if `archetypes.h` should remain standalone
  - Consider grouping utilities

#### Events Subsystem

- [ ] **EVENT-4**: Standardize event file naming
  - Decide: `event_factory.h` vs `EventFactory.h`
  - Decide: `subscriber_factory.h` vs `SubscriberFactory.h`
  - Apply consistently across subsystem

### Phase 5: Introduce Nested Namespaces (Low Priority, Higher Risk)

**Note**: This is optional and should only be done if the codebase continues to grow.

- [ ] **NS-1**: Introduce `steamrot::logic` namespace
  - Wrap all logic classes in `namespace steamrot::logic`
  - Move helper namespaces inside (e.g., `steamrot::logic::action`)
  - Update all includes and usages
  - Run full test suite

- [ ] **NS-2**: Introduce `steamrot::entity` namespace
  - Wrap entity classes
  - Nest helper namespaces
  - Test thoroughly

- [ ] **NS-3**: Introduce `steamrot::data` namespace
  - Wrap data providers and loaders
  - Nest helper namespaces

- [ ] **NS-4**: Update documentation
  - Document all new nested namespace patterns
  - Provide migration guide for future changes

### Phase 6: Validation and Enforcement (Ongoing)

- [ ] **VAL-1**: Create automated namespace linter
  - Script to check namespace consistency
  - Verify file naming matches content type
  - Run in CI/CD pipeline

- [ ] **VAL-2**: Add pre-commit hooks
  - Check file naming conventions
  - Verify namespace usage
  - Enforce documentation updates

- [ ] **VAL-3**: Periodic reviews
  - Quarterly review of new files
  - Check adherence to conventions
  - Update documentation as needed

---

## Phased Implementation Timeline

### Immediate (Week 1-2)
- Complete **Phase 1** (Documentation)
- Complete **TEST-1** and **TEST-2** (Move ITestDataProvider)

### Short-term (Month 1)
- Complete **Phase 2** (Test Infrastructure)
- Complete **Phase 3** (Event Subsystem)

### Medium-term (Quarter 1)
- Complete **Phase 4** (File Naming) - Do incrementally
- Start **Phase 5** if justified by codebase growth

### Long-term (Ongoing)
- **Phase 6** (Validation) - Implement automation
- Continuous improvement based on team feedback

---

## Risk Assessment

| Phase | Risk Level | Impact | Mitigation |
|-------|-----------|--------|------------|
| **Phase 1** | 🟢 Low | Documentation only | Review and iterate |
| **Phase 2** | 🟢 Low | Isolated to tests | Comprehensive testing |
| **Phase 3** | 🟡 Medium | Changes production code | Incremental changes, full test suite |
| **Phase 4** | 🟡 Medium | Large refactor | Git mv preserves history, staged rollout |
| **Phase 5** | 🟠 High | Widespread changes | Optional, only if needed |
| **Phase 6** | 🟢 Low | Automation | Gradual enforcement |

---

## Success Metrics

### Measurable Goals

1. **Namespace Consistency**: 100% of files in a subsystem use the same namespace pattern
2. **File Naming Predictability**: New contributors can predict file names with 90% accuracy
3. **Documentation Completeness**: All naming conventions documented with examples
4. **Test Coverage**: No tests broken during refactoring
5. **Developer Satisfaction**: Positive feedback on clarity and organization

### Key Indicators

- Reduced time to locate files
- Fewer "where should I put this?" questions
- Cleaner `#include` hierarchies
- Better IDE autocomplete experience
- Easier onboarding for new contributors

---

## Appendix: Current Namespace Breakdown

### Production Code Namespaces

| Namespace | File Count | Purpose |
|-----------|-----------|---------|
| `steamrot` | 120+ | Primary namespace for all production code |
| `steamrot::event` | 3 | Event factory and conversion |
| `steamrot::events` | 2 | Event handler tick |
| `steamrot::logic::ui` | 2 | UI logic helpers |
| `steamrot::archetypes` | 2 | Archetype utilities |
| `steamrot::entity::memory` | 2 | Entity memory management |
| `steamrot::paths` | 1 | Path utilities |
| Anonymous | 1 | Internal helpers |

### Test Code Namespaces

| Namespace | File Count | Purpose |
|-----------|-----------|---------|
| `steamrot::tests` | 53+ | All test infrastructure |

### Namespace Depth Analysis

- **Depth 0** (Global): 0 files (excluding generated)
- **Depth 1** (`steamrot`): 127 files (74%)
- **Depth 2** (`steamrot::event`, etc.): 7 files (4%)
- **Depth 3** (`steamrot::logic::ui`): 2 files (1%)
- **Generated/Special**: ~35 files (21%)

---

## Conclusion

The SteamRot codebase demonstrates **good foundational organization** with clear areas for improvement. The primary issues are:

1. **Inconsistent use of nested namespaces** - Only 4% of files use them
2. **Mixed file naming conventions** - PascalCase vs snake_case within same directories
3. **Namespace naming inconsistency** - `event` vs `events` in same subsystem
4. **Interface location split** - Production vs test interfaces in different places

**Recommended Approach**:
- Start with documentation (Phase 1) - Low risk, high clarity
- Fix test infrastructure (Phase 2) - Isolated, low risk
- Standardize events (Phase 3) - High impact, manageable scope
- Address file naming gradually (Phase 4) - Staged approach
- Consider nested namespaces only if codebase continues to grow (Phase 5)

By following this phased approach, the codebase will achieve greater consistency, maintainability, and developer experience while minimizing risk and disruption.

---

**Document Version**: 1.0  
**Last Updated**: 2026-01-01  
**Authors**: GitHub Copilot Analysis  
**Status**: Draft - Awaiting Review
