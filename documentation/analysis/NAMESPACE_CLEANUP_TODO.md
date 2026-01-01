# Namespace and File Naming Cleanup - Action Items

**Date Created**: 2026-01-01  
**Status**: Pending Review  
**Related Document**: [NAMESPACE_AND_FILE_NAMING_ANALYSIS.md](./NAMESPACE_AND_FILE_NAMING_ANALYSIS.md)

---

## Quick Reference

### Priority Levels
- 🔴 **HIGH**: Critical for consistency, should be addressed soon
- 🟡 **MEDIUM**: Important but not urgent, can be phased
- 🟢 **LOW**: Nice to have, address when convenient

### Risk Levels
- ✅ **Safe**: Documentation or isolated changes
- ⚠️ **Moderate**: Requires testing but low blast radius
- ⛔ **High**: Widespread changes, needs careful planning

---

## Phase 1: Documentation 🔴 HIGH / ✅ SAFE

### DOC-1: Create Namespace Strategy Document
- [ ] Create `documentation/NAMESPACE_STRATEGY.md`
- [ ] Define nested namespace usage rules
- [ ] Document singular vs plural naming convention
- [ ] Provide examples for each major subsystem
- [ ] Define interface placement rules
- [ ] Include decision rationale for future reference

**Deliverables**: `NAMESPACE_STRATEGY.md` with:
- When to use `namespace steamrot::subsystem` vs `namespace steamrot`
- Naming conventions (singular preferred: `event`, `logic`, `entity`)
- Examples for logic, event, entity, data subsystems
- Interface location guidelines

**Estimated Effort**: 2-3 hours

---

### DOC-2: Update Main README
- [ ] Add "Namespace Organization" section to main README
- [ ] Document current namespace patterns
- [ ] Add quick reference table
- [ ] Link to `NAMESPACE_STRATEGY.md` for details
- [ ] Update "Project Structure" section if needed

**Example Section**:
```markdown
## Namespace Organization

### Primary Namespaces
- `steamrot` - All production code
- `steamrot::tests` - All test infrastructure

### Subsystem Namespaces (In Progress)
- `steamrot::event` - Event system
- `steamrot::logic` - Game logic system
- `steamrot::entity` - Entity/component system

See [NAMESPACE_STRATEGY.md](documentation/NAMESPACE_STRATEGY.md) for details.
```

**Estimated Effort**: 1-2 hours

---

### DOC-3: Update Copilot Instructions
- [ ] Add namespace organization to `.github/copilot-instructions.md`
- [ ] Document file naming rules by content type
- [ ] Clarify interface placement (src/interfaces/ for production, tests/interfaces/ for tests)
- [ ] Add examples for agent guidance

**Additions Needed**:
```markdown
### Namespace Guidelines
- Use `namespace steamrot` for most code
- Use nested namespaces for subsystems with multiple helper files
- Prefer singular names: `steamrot::event` not `steamrot::events`
- Test code uses `namespace steamrot::tests`

### File Naming by Content
| Type | Pattern | Example |
|------|---------|---------|
| Class | PascalCase | `EntityManager.h` |
| Component | CPascalCase | `CUserInterface.h` |
| Interface | IPascalCase | `IDataProvider.h` |
| Free Functions | snake_case | `entity_utils.h` |
```

**Estimated Effort**: 1 hour

---

## Phase 2: Test Infrastructure 🟡 MEDIUM / ✅ SAFE

### TEST-1: Create Test Interfaces Directory
- [ ] Create `tests/interfaces/` directory
- [ ] Create `tests/interfaces/CMakeLists.txt`
- [ ] Add to parent `tests/CMakeLists.txt` with `add_subdirectory(interfaces)`

**Commands**:
```bash
mkdir tests/interfaces
touch tests/interfaces/CMakeLists.txt
```

**Estimated Effort**: 15 minutes

---

### TEST-2: Move ITestDataProvider to tests/interfaces/
- [ ] Move `tests/harness/ITestDataProvider.h` → `tests/interfaces/ITestDataProvider.h`
- [ ] Move `tests/harness/ITestDataProvider.cpp` → `tests/interfaces/ITestDataProvider.cpp`
- [ ] Update `#include "ITestDataProvider.h"` → `#include "ITestDataProvider.h"` in:
  - [ ] `tests/harness/FlatbuffersTestDataProvider.h`
  - [ ] `tests/harness/FlatbuffersTestDataProvider.cpp`
  - [ ] Any test files that include it
- [ ] Update `tests/harness/CMakeLists.txt` to remove ITestDataProvider
- [ ] Update `tests/interfaces/CMakeLists.txt` to include ITestDataProvider
- [ ] Build and test

**Git Commands**:
```bash
git mv tests/harness/ITestDataProvider.h tests/interfaces/ITestDataProvider.h
git mv tests/harness/ITestDataProvider.cpp tests/interfaces/ITestDataProvider.cpp
# Update includes in affected files
# Update CMakeLists.txt files
```

**Files to Update**:
- `tests/harness/FlatbuffersTestDataProvider.h`
- `tests/harness/FlatbuffersTestDataProvider.cpp`
- `tests/harness/CMakeLists.txt`
- `tests/interfaces/CMakeLists.txt`

**Testing**:
```bash
cmake --preset Debug
cmake --build --preset Debug
ctest --preset Debug
```

**Estimated Effort**: 1 hour

---

### TEST-3: Document Test Namespace Strategy (Future)
- [ ] Add section to `NAMESPACE_STRATEGY.md` for test namespaces
- [ ] Document use of `steamrot::tests::matchers`, `steamrot::tests::harness`, etc.
- [ ] Set guideline: Use nested namespaces for NEW test files
- [ ] Don't mass-refactor existing test files yet

**Note**: This is for future development, not immediate refactoring.

**Estimated Effort**: 30 minutes

---

### TEST-4: Update Test Harness README
- [ ] Update `tests/harness/README.md` with interface location
- [ ] Document that interfaces are in `tests/interfaces/`
- [ ] Update any diagrams or structure documentation

**Estimated Effort**: 30 minutes

---

## Phase 3: Event Subsystem Standardization 🔴 HIGH / ⚠️ MODERATE

### EVENT-1: Make Namespace Decision
- [ ] Review `NAMESPACE_AND_FILE_NAMING_ANALYSIS.md` recommendations
- [ ] Decide: `steamrot::event` (recommended) or `steamrot::events`
- [ ] Document decision in `NAMESPACE_STRATEGY.md`
- [ ] Create issue tracking the change

**Recommendation**: Use `steamrot::event` (singular)
- Matches directory name pattern
- Consistent with C++ standard library conventions
- Already used by 3 files vs 2 for `events`

**Estimated Effort**: 15 minutes (decision making)

---

### EVENT-2: Standardize Event Namespace (If choosing steamrot::event)
- [ ] Update `src/events/event_handler_tick.h`:
  - [ ] Change `namespace steamrot::events {` → `namespace steamrot::event {`
  - [ ] Update closing comment
- [ ] Update `src/events/event_handler_tick.cpp`:
  - [ ] Change `namespace steamrot::events {` → `namespace steamrot::event {`
  - [ ] Update closing comment
- [ ] Search for any code using `steamrot::events::` and update
- [ ] Build and run full test suite
- [ ] Commit changes

**Search Command**:
```bash
grep -r "steamrot::events::" src tests --include="*.h" --include="*.cpp"
```

**Testing**:
```bash
cmake --preset Debug
cmake --build --preset Debug
ctest --preset Debug --output-on-failure
```

**Estimated Effort**: 1-2 hours

---

### EVENT-3: Consider Nested Namespaces for Event Utilities (Optional)
- [ ] Review event subsystem structure
- [ ] Decide if `steamrot::event::factory` and `steamrot::event::subscriber` make sense
- [ ] Document decision (yes/no and rationale)
- [ ] If yes, create separate task for implementation

**Evaluation Criteria**:
- Does it improve clarity?
- Will it prevent name collisions?
- Is the subsystem large enough to warrant it?

**Estimated Effort**: 1 hour (evaluation only)

---

## Phase 4: File Naming Standardization 🟡 MEDIUM / ⚠️ MODERATE

### NAMING-1: Document File Naming Convention
- [ ] Add clear file naming rules to `NAMESPACE_STRATEGY.md`
- [ ] Define: PascalCase for classes, snake_case for free function files
- [ ] Provide decision tree or flowchart
- [ ] Add examples from each subsystem

**Proposed Rules**:
```markdown
## File Naming Convention

### By Content Type
| Content Type | Pattern | Example |
|--------------|---------|---------|
| **Single class definition** | PascalCase.h/cpp | EntityManager.h |
| **Multiple related classes** | PascalCase.h/cpp | EventPacket.h |
| **Free function utilities** | snake_case.h/cpp | entity_utils.h |
| **Components** | CPascalCase.h/cpp | CUserInterface.h |
| **Interfaces** | IPascalCase.h | IDataProvider.h |
| **Type definitions only** | PascalCase.h | FailInfo.h |

### Decision Tree
1. Is it a component? → Use CPascalCase
2. Is it an interface? → Use IPascalCase
3. Does it contain a single primary class? → Use PascalCase
4. Does it contain only free functions? → Use snake_case
5. Mixed content? → Prefer PascalCase for the main class
```

**Estimated Effort**: 1-2 hours

---

### LOGIC-1: Standardize Logic Subsystem Naming
- [ ] Review current logic files
- [ ] Decide: Keep `logic_action.h` or rename to `LogicAction.h`
- [ ] Document decision and rationale
- [ ] Create issues for any renames needed

**Current Files**:
- `Logic.h` (class) - ✅ Correct
- `LogicFactory.h` (class) - ✅ Correct
- `UIActionLogic.h` (class) - ✅ Correct
- `logic_action.h` (free functions) - ❓ snake_case or PascalCase?
- `logic_collision.h` (free functions) - ❓ snake_case or PascalCase?
- `logic_render.h` (free functions) - ❓ snake_case or PascalCase?
- `logic_ui.h` (free functions) - ❓ snake_case or PascalCase?

**Recommendation**: Keep snake_case for free function files (already documented rule).

**Estimated Effort**: 30 minutes (decision) + 2-4 hours (implementation if renaming)

---

### ENTITY-1: Standardize Entity Subsystem Naming
- [ ] Review current entity files
- [ ] Apply same decision as LOGIC-1
- [ ] Create issues for renames if needed

**Current Files**:
- `EntityManager.h` (class) - ✅ Correct
- `ArchetypeManager.h` (class) - ✅ Correct
- `FlatbuffersEntityConfigurator.h` (class) - ✅ Correct
- `IEntityConfigurator.h` (interface) - ✅ Correct
- `entity_memory.h` (free functions) - ❓
- `entity_types.h` (type definitions) - ❓
- `archetypes.h` (free functions) - ❓

**Estimated Effort**: 30 minutes (decision) + 2-4 hours (implementation if renaming)

---

### EVENT-4: Standardize Event Subsystem File Naming
- [ ] Apply naming convention to event files
- [ ] Decide on `event_factory.h` (snake_case if free functions)
- [ ] Decide on `subscriber_factory.h`
- [ ] Create issues for renames if needed

**Current Files**:
- `EventHandler.h` (class) - ✅ Correct
- `event_factory.h` - ❓ Contains free functions → snake_case OK
- `subscriber_factory.h` - ❓ Contains free functions → snake_case OK
- `event_bus_conversion.h` - ❓ Contains free functions → snake_case OK
- `event_handler_tick.h` - ❓ Contains free functions → snake_case OK

**Likely Conclusion**: Current naming is already consistent if we adopt snake_case for free function files.

**Estimated Effort**: 30 minutes (validation)

---

## Phase 5: Introduce Nested Namespaces (Optional) 🟢 LOW / ⛔ HIGH

**Note**: Only proceed with this phase if:
1. Codebase continues to grow significantly
2. Name collision risks increase
3. Team agrees the benefit outweighs the refactoring cost

### NS-1: Introduce steamrot::logic Namespace
- [ ] Create tracking issue
- [ ] Update all logic classes to `namespace steamrot::logic`
- [ ] Move helper namespaces inside (e.g., `steamrot::logic::action`)
- [ ] Update all `#include` statements
- [ ] Update CMakeLists.txt if needed
- [ ] Run full test suite
- [ ] Update documentation

**Estimated Effort**: 4-8 hours

---

### NS-2: Introduce steamrot::entity Namespace
- [ ] Similar process to NS-1
- [ ] Wrap all entity classes in `namespace steamrot::entity`
- [ ] Nest `archetypes` and `memory` namespaces

**Estimated Effort**: 4-8 hours

---

### NS-3: Introduce steamrot::data Namespace
- [ ] Wrap data providers and loaders
- [ ] Nest helper namespaces

**Estimated Effort**: 4-8 hours

---

### NS-4: Update All Documentation
- [ ] Update `NAMESPACE_STRATEGY.md`
- [ ] Update main README
- [ ] Update Copilot instructions
- [ ] Provide migration guide

**Estimated Effort**: 2-3 hours

---

## Phase 6: Validation and Enforcement (Ongoing) 🟢 LOW / ✅ SAFE

### VAL-1: Create Namespace Linter Script
- [ ] Create `tools/check_namespaces.py` (or similar)
- [ ] Script validates:
  - [ ] Namespace consistency within subsystems
  - [ ] File naming matches content type
  - [ ] Interface files use `I` prefix
  - [ ] Component files use `C` prefix
- [ ] Add to CI/CD pipeline (optional)
- [ ] Document usage in `tools/README.md`

**Estimated Effort**: 4-6 hours

---

### VAL-2: Add Pre-commit Hooks (Optional)
- [ ] Set up pre-commit framework
- [ ] Add namespace validation hook
- [ ] Add file naming validation hook
- [ ] Document in `CONTRIBUTING.md`

**Estimated Effort**: 2-4 hours

---

### VAL-3: Schedule Periodic Reviews
- [ ] Add quarterly review to team calendar
- [ ] Review new files for convention adherence
- [ ] Update documentation based on learnings
- [ ] Track metrics (see below)

**Estimated Effort**: 1 hour per quarter

---

## Success Metrics

### Quantitative Metrics
- [ ] **Namespace consistency**: % of subsystem files using same namespace
  - Target: 100% within each subsystem
  - Current: ~85% (estimated)

- [ ] **File naming predictability**: Survey new contributors
  - Target: 90% can predict file names correctly
  - Measure: Onboarding survey

- [ ] **Documentation completeness**: % of conventions documented
  - Target: 100%
  - Current: ~60% (estimated)

- [ ] **Zero test failures**: All tests pass after refactoring
  - Target: 100%
  - Current: Baseline before changes

### Qualitative Metrics
- [ ] **Developer feedback**: Positive comments on organization
- [ ] **Reduced questions**: Fewer "where should this go?" questions
- [ ] **Faster navigation**: Anecdotal reports of easier file finding
- [ ] **Better IDE experience**: Improved autocomplete and navigation

---

## Current Status

### Completed
- [x] Initial analysis document created
- [x] Todo list created
- [x] Recommendations prioritized

### In Progress
- [ ] Documentation phase (DOC-1, DOC-2, DOC-3)

### Blocked
- [ ] Nothing currently blocked

### Deferred
- [ ] Phase 5 (Nested Namespaces) - Waiting for team decision

---

## Notes and Decisions

### Decision Log

| Date | Decision | Rationale | Decided By |
|------|----------|-----------|------------|
| 2026-01-01 | Create analysis document first | Need comprehensive understanding before changes | Copilot |
| 2026-01-01 | Start with documentation phase | Lowest risk, highest clarity benefit | Copilot |
| TBD | Choose `event` vs `events` | Need team input | Pending |
| TBD | Adopt snake_case for free functions? | Need team input | Pending |

---

### Questions for Team

1. **Namespace naming**: Prefer `steamrot::event` (singular) or `steamrot::events` (plural)?
2. **File naming**: Keep current `logic_action.h` style or rename to `LogicAction.h`?
3. **Phase 5 scope**: Should we introduce nested namespaces like `steamrot::logic`?
4. **Timeline**: What's the urgency for each phase?
5. **Tooling**: Should we invest in automated validation?

---

## Useful Commands

### Search for Namespace Usage
```bash
# Find all namespace declarations
grep -r "^namespace" src --include="*.h" --include="*.cpp"

# Count files using specific namespace
grep -r "namespace steamrot::event" src --include="*.h" | wc -l

# Find files without namespace
find src -name "*.h" -exec grep -L "namespace" {} \;
```

### Search for File Naming Patterns
```bash
# Find PascalCase files
find src -name "[A-Z]*.h" -o -name "[A-Z]*.cpp"

# Find snake_case files
find src -name "*_*.h" -o -name "*_*.cpp"

# Find component files
find src -name "C[A-Z]*.h"
```

### Refactoring Helpers
```bash
# Rename file with git
git mv old_name.h new_name.h

# Update includes in multiple files
find src tests -name "*.cpp" -o -name "*.h" | xargs sed -i 's/"old_name.h"/"new_name.h"/g'

# Preview changes before applying
git diff --cached
```

---

## Timeline Estimate

### Optimistic (Focused Work)
- **Phase 1**: 1 week
- **Phase 2**: 1 week
- **Phase 3**: 1 week
- **Phase 4**: 2-3 weeks (incremental)
- **Phase 5**: 3-4 weeks (if needed)
- **Total**: 2-3 months

### Realistic (Part-time, Careful)
- **Phase 1**: 2 weeks
- **Phase 2**: 1-2 weeks
- **Phase 3**: 2 weeks
- **Phase 4**: 4-6 weeks (incremental)
- **Phase 5**: 6-8 weeks (if needed)
- **Total**: 4-6 months

### Conservative (Low Priority)
- **Phase 1**: 1 month
- **Phase 2**: 2 weeks
- **Phase 3**: 1 month
- **Phase 4**: 2-3 months (very incremental)
- **Phase 5**: 3-4 months (if needed)
- **Total**: 6-12 months

---

**Document Status**: Draft  
**Version**: 1.0  
**Last Updated**: 2026-01-01  
**Next Review**: After Phase 1 completion
