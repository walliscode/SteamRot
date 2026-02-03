# CGrimoireMachina Refactoring: Design Proposal

**Date**: 2026-02-03  
**Status**: Design Proposal  
**Related Document**: [CGRIMOIREMACHINA_REFACTORING_ANALYSIS.md](./CGRIMOIREMACHINA_REFACTORING_ANALYSIS.md)

---

## Overview

This document provides a concrete design proposal for refactoring CGrimoireMachina based on the **Hybrid Approach** recommended in the analysis document. The design focuses on:

1. Moving static grimoire catalog data to AssetManager
2. Keeping dynamic working state separate
3. Creating a new component for entity-specific machina instances
4. Maintaining backward compatibility during migration

---

## Design Goals

1. **Separation of Concerns**: Static catalog data vs. dynamic runtime state
2. **Simplified Access**: Direct access to grimoire without archetype lookups
3. **Minimal Breaking Changes**: Gradual migration path
4. **Better Semantics**: Data location matches its conceptual role
5. **Testability**: Easier to test in isolation

---

## Proposed Class Structure

### 1. GrimoireMachina (New Standalone Class)

**Location**: `src/assets/GrimoireMachina.h` / `src/assets/GrimoireMachina.cpp`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the GrimoireMachina class.
/////////////////////////////////////////////////

#pragma once

#include "Fragment.h"
#include "Joint.h"
#include "MachinaForm.h"
#include <map>
#include <string>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
/// @class GrimoireMachina
/// @brief Global catalog of all fragments, joints, and machina form templates
///        available in the game.
///
/// GrimoireMachina serves as a read-only database of craftable components.
/// It is loaded once at game initialization and provides access to all
/// available fragments, joints, and machina form templates.
/////////////////////////////////////////////////
class GrimoireMachina {
private:
  /////////////////////////////////////////////////
  /// @brief All available fragments in the game (read-only catalog)
  /////////////////////////////////////////////////
  std::map<std::string, Fragment> m_all_fragments;

  /////////////////////////////////////////////////
  /// @brief All available joints in the game (read-only catalog)
  /////////////////////////////////////////////////
  std::map<std::string, Joint> m_all_joints;

  /////////////////////////////////////////////////
  /// @brief Collection of all available MachinaForm templates.
  ///        These are designed to be copied, not used directly.
  /////////////////////////////////////////////////
  std::map<std::string, MachinaForm> m_machina_form_templates;

public:
  GrimoireMachina() = default;

  /////////////////////////////////////////////////
  /// @brief Get all available fragments
  ///
  /// @return Const reference to the fragments map
  /////////////////////////////////////////////////
  const std::map<std::string, Fragment> &GetAllFragments() const;

  /////////////////////////////////////////////////
  /// @brief Get all available joints
  ///
  /// @return Const reference to the joints map
  /////////////////////////////////////////////////
  const std::map<std::string, Joint> &GetAllJoints() const;

  /////////////////////////////////////////////////
  /// @brief Get all machina form templates
  ///
  /// @return Const reference to the machina form templates map
  /////////////////////////////////////////////////
  const std::map<std::string, MachinaForm> &GetMachinaFormTemplates() const;

  /////////////////////////////////////////////////
  /// @brief Get a specific fragment by name
  ///
  /// @param name Fragment name
  /// @return Optional containing the fragment if found
  /////////////////////////////////////////////////
  std::optional<Fragment> GetFragment(const std::string &name) const;

  /////////////////////////////////////////////////
  /// @brief Get a specific joint by name
  ///
  /// @param name Joint name
  /// @return Optional containing the joint if found
  /////////////////////////////////////////////////
  std::optional<Joint> GetJoint(const std::string &name) const;

  /////////////////////////////////////////////////
  /// @brief Get a specific machina form template by name
  ///
  /// @param name MachinaForm template name
  /// @return Optional containing the machina form if found
  /////////////////////////////////////////////////
  std::optional<MachinaForm> GetMachinaFormTemplate(const std::string &name) const;

  /////////////////////////////////////////////////
  /// @brief Get all fragment names (for UI population)
  ///
  /// @return Vector of all fragment names
  /////////////////////////////////////////////////
  std::vector<std::string> GetAllFragmentNames() const;

  /////////////////////////////////////////////////
  /// @brief Get all joint names (for UI population)
  ///
  /// @return Vector of all joint names
  /////////////////////////////////////////////////
  std::vector<std::string> GetAllJointNames() const;

  /////////////////////////////////////////////////
  /// @brief Get all machina form template names
  ///
  /// @return Vector of all template names
  /////////////////////////////////////////////////
  std::vector<std::string> GetAllTemplateNames() const;

  /////////////////////////////////////////////////
  /// @brief Add a fragment to the catalog (used during loading)
  ///
  /// @param name Fragment name
  /// @param fragment Fragment data
  /////////////////////////////////////////////////
  void AddFragment(const std::string &name, const Fragment &fragment);

  /////////////////////////////////////////////////
  /// @brief Add a joint to the catalog (used during loading)
  ///
  /// @param name Joint name
  /// @param joint Joint data
  /////////////////////////////////////////////////
  void AddJoint(const std::string &name, const Joint &joint);

  /////////////////////////////////////////////////
  /// @brief Add a machina form template (used during loading)
  ///
  /// @param name Template name
  /// @param form MachinaForm template
  /////////////////////////////////////////////////
  void AddMachinaFormTemplate(const std::string &name, const MachinaForm &form);
};

} // namespace steamrot
```

### 2. MachinaForm (Refactored - No Longer a Component)

**Location**: `src/types/core/MachinaForm.h` / `src/types/core/MachinaForm.cpp`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the MachinaForm class
/////////////////////////////////////////////////

#pragma once

#include "Fragment.h"
#include "Joint.h"
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
/// @class MachinaForm
/// @brief MachinaForm is a data store of a combination of Fragments and Joints
///
/// Each MachinaForm contains all the Fragments and Joints that define a
/// specific entity design. Not designed to be modified once created; game
/// mechanics involve making new designs. Modification = copy then modify.
///
/// NOTE: MachinaForm is NO LONGER a Component. It is a pure data class.
/////////////////////////////////////////////////
class MachinaForm {
public:
  MachinaForm() = default;

  /////////////////////////////////////////////////
  /// @brief Contains all Fragments for this MachinaForm
  /////////////////////////////////////////////////
  std::vector<Fragment> m_fragments;

  /////////////////////////////////////////////////
  /// @brief Contains all Joints for this MachinaForm
  /////////////////////////////////////////////////
  std::vector<Joint> m_joints;

  /////////////////////////////////////////////////
  /// @brief Name/identifier for this form
  /////////////////////////////////////////////////
  std::string m_form_name{"unnamed"};
};

} // namespace steamrot
```

### 3. CCraftingState (New Component - Working State)

**Location**: `src/types/components/CCraftingState.h`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the CCraftingState component
/////////////////////////////////////////////////

#pragma once

#include "Component.h"
#include "MachinaForm.h"
#include <memory>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief CCraftingState holds the working state for crafting UI
///
/// This component contains the current work-in-progress machina form
/// being built or edited. This is scene/UI-specific state, separate
/// from the global grimoire catalog.
/////////////////////////////////////////////////
struct CCraftingState : public Component {
  CCraftingState() = default;

  /////////////////////////////////////////////////
  /// @brief Copy constructor for deep copying
  /////////////////////////////////////////////////
  CCraftingState(const CCraftingState &other)
      : Component(other),
        m_holding_form(other.m_holding_form
                           ? std::make_unique<MachinaForm>(*other.m_holding_form)
                           : nullptr) {}

  /////////////////////////////////////////////////
  /// @brief Copy assignment operator
  /////////////////////////////////////////////////
  CCraftingState &operator=(const CCraftingState &other) {
    if (this != &other) {
      Component::operator=(other);
      m_holding_form = other.m_holding_form
                           ? std::make_unique<MachinaForm>(*other.m_holding_form)
                           : nullptr;
    }
    return *this;
  }

  /////////////////////////////////////////////////
  /// @brief A holding form used to build up a new structure
  /////////////////////////////////////////////////
  std::unique_ptr<MachinaForm> m_holding_form{nullptr};
};

} // namespace steamrot
```

### 4. CMachinaInstance (New Component - Entity Association)

**Location**: `src/types/components/CMachinaInstance.h`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the CMachinaInstance component
/////////////////////////////////////////////////

#pragma once

#include "Component.h"
#include "MachinaForm.h"
#include <memory>
#include <string>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief CMachinaInstance represents an entity's equipped machina form
///
/// This component associates an entity with a specific machina form.
/// The form can be a copy from a template or a custom creation.
/////////////////////////////////////////////////
struct CMachinaInstance : public Component {
  CMachinaInstance() = default;

  /////////////////////////////////////////////////
  /// @brief Copy constructor
  /////////////////////////////////////////////////
  CMachinaInstance(const CMachinaInstance &other)
      : Component(other),
        m_form(other.m_form ? std::make_unique<MachinaForm>(*other.m_form)
                            : nullptr),
        m_template_name(other.m_template_name) {}

  /////////////////////////////////////////////////
  /// @brief Copy assignment operator
  /////////////////////////////////////////////////
  CMachinaInstance &operator=(const CMachinaInstance &other) {
    if (this != &other) {
      Component::operator=(other);
      m_form = other.m_form ? std::make_unique<MachinaForm>(*other.m_form)
                            : nullptr;
      m_template_name = other.m_template_name;
    }
    return *this;
  }

  /////////////////////////////////////////////////
  /// @brief The actual machina form instance for this entity
  /////////////////////////////////////////////////
  std::unique_ptr<MachinaForm> m_form{nullptr};

  /////////////////////////////////////////////////
  /// @brief Optional: Name of the template this was created from
  /////////////////////////////////////////////////
  std::string m_template_name{""};

  // Future: Add entity-specific state here
  // - Damage/wear
  // - Modifications
  // - Stats
};

} // namespace steamrot
```

### 5. Updated AssetManager

**Location**: `src/assets/AssetManager.h`

```cpp
// Add to existing AssetManager class:

private:
  /////////////////////////////////////////////////
  /// @brief Global grimoire catalog
  /////////////////////////////////////////////////
  std::unique_ptr<GrimoireMachina> m_grimoire_machina;

public:
  /////////////////////////////////////////////////
  /// @brief Load grimoire data from configuration
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> LoadGrimoireData();

  /////////////////////////////////////////////////
  /// @brief Get read-only access to the grimoire
  ///
  /// @return Const reference to GrimoireMachina
  /////////////////////////////////////////////////
  const GrimoireMachina &GetGrimoireMachina() const;
```

---

## Data Flow

### Loading Phase (Game Initialization)

```
1. Game Start
   ↓
2. AssetManager::LoadAssets()
   ↓
3. AssetManager::LoadGrimoireData()
   ↓
4. FlatBuffers/JSON → GrimoireMachina
   ↓
5. GrimoireMachina populated with:
   - All fragments
   - All joints
   - All machina form templates
```

### Runtime Access (Logic Classes)

```
Before (Current):
Logic → Archetype Lookup → Find Entity → GetComponent<CGrimoireMachina>

After (Proposed):
Logic → SceneContext/LogicContext → AssetManager → GetGrimoireMachina()
```

### Crafting Workflow

```
1. Player enters crafting scene
   ↓
2. Scene has entity with CCraftingState component
   ↓
3. Player selects fragments/joints from GrimoireMachina catalog
   ↓
4. Selections stored in CCraftingState::m_holding_form
   ↓
5. Player confirms design
   ↓
6. Create new CMachinaInstance from holding form
   ↓
7. Attach CMachinaInstance to player entity
```

---

## Migration Strategy

### Phase 0: Preparation (No Breaking Changes)

1. **Create new classes** (don't use them yet):
   - `GrimoireMachina` class
   - `CCraftingState` component
   - `CMachinaInstance` component
   - Refactored `MachinaForm` (new location, but keep old too)

2. **Add to AssetManager**:
   - Add `m_grimoire_machina` member
   - Add `LoadGrimoireData()` method
   - Add `GetGrimoireMachina()` accessor

3. **Update FlatBuffers schemas**:
   - Create grimoire data schema
   - Keep existing entity schemas

4. **Create data files**:
   - New grimoire JSON/binary files
   - Keep existing scene entity files

**Result**: New code exists but isn't used; old code still works.

### Phase 1: Parallel Implementation

1. **Load grimoire data in AssetManager**:
   - `LoadGrimoireData()` populates GrimoireMachina
   - Verify data loads correctly

2. **Update ONE Logic class** to use both approaches:
   - Keep archetype lookup (old way)
   - Add AssetManager access (new way)
   - Compare results to verify equivalence

3. **Add comprehensive tests**:
   - Unit tests for GrimoireMachina
   - Integration tests for AssetManager loading
   - Tests comparing old vs. new access

**Result**: Both old and new systems work; can validate correctness.

### Phase 2: Migration

1. **Update all Logic classes**:
   - Remove archetype lookups for CGrimoireMachina
   - Use `asset_manager.GetGrimoireMachina()` instead
   - Update helper functions in `ui_helpers.cpp`

2. **Update contexts**:
   - Ensure AssetManager reference available in all contexts
   - Update LogicContext, SceneContext as needed

3. **Deprecate old component**:
   - Mark CGrimoireMachina as deprecated
   - Add compiler warnings
   - Keep temporarily for compatibility

**Result**: All code uses new system; old component unused but exists.

### Phase 3: Cleanup

1. **Remove CGrimoireMachina component**:
   - Remove from ComponentRegister
   - Remove FlatBuffers configuration
   - Remove test data

2. **Remove old CMachinaForm component**:
   - Remove from ComponentRegister
   - Keep as pure data class

3. **Add new components to ComponentRegister**:
   - Add CCraftingState
   - Add CMachinaInstance

4. **Update documentation**:
   - Update all guides and examples
   - Add migration guide for any external code

**Result**: Clean codebase with new architecture.

---

## Impact Analysis

### Files to Create

1. `src/assets/GrimoireMachina.h`
2. `src/assets/GrimoireMachina.cpp`
3. `src/types/core/MachinaForm.h` (moved/refactored)
4. `src/types/core/MachinaForm.cpp` (if needed)
5. `src/types/components/CCraftingState.h`
6. `src/types/components/CMachinaInstance.h`
7. `data/grimoire/` (new data directory)
8. FlatBuffers schema for grimoire data

### Files to Modify

**Core Classes**:
- `src/assets/AssetManager.h` - Add grimoire member and methods
- `src/assets/AssetManager.cpp` - Implement grimoire loading
- `src/types/components/containers.h` - Update ComponentRegister

**Logic Classes** (Access Pattern Changes):
- `src/logic/ui_action.cpp` - Update dropdown population
- `src/logic/ui_helpers.h` - Update helper function signatures
- `src/logic/ui_helpers.cpp` - Update helper implementations
- `src/logic/CraftingRenderLogic.h` - Update to use new access
- `src/logic/CraftingRenderLogic.cpp` - Implementation updates

**Context Classes**:
- `src/types/core/SceneContext.h` - Ensure AssetManager reference
- `src/types/core/LogicContext.h` - Ensure AssetManager reference

**Data Providers**:
- Create new provider for grimoire data loading
- Update asset configuration

**Tests** (Many files):
- All tests accessing CGrimoireMachina
- AssetManager tests
- Logic class tests
- Integration tests
- Test data files

### Files to Delete (Eventually)

1. `src/types/components/CGrimoireMachina.h`
2. `src/types/components/CMachinaForm.h` (old location)
3. Related test files
4. Old FlatBuffers schemas (after migration)

---

## Testing Strategy

### Unit Tests

1. **GrimoireMachina Tests**:
   - Test adding fragments/joints/templates
   - Test retrieval by name
   - Test GetAllNames() methods
   - Test empty state handling

2. **AssetManager Tests**:
   - Test LoadGrimoireData() success
   - Test LoadGrimoireData() failure cases
   - Test GetGrimoireMachina() access

3. **Component Tests**:
   - CCraftingState construction, copy, assignment
   - CMachinaInstance construction, copy, assignment

### Integration Tests

1. **Asset Loading Integration**:
   - Load complete asset configuration including grimoire
   - Verify all data loaded correctly
   - Test with various data files

2. **Logic Access Integration**:
   - Create scene with AssetManager
   - Access grimoire from Logic classes
   - Verify correct data retrieval

3. **Crafting Workflow Integration**:
   - Test complete crafting flow
   - Create CCraftingState
   - Build form in holding slot
   - Create CMachinaInstance from holding form

### Migration Validation Tests

1. **Equivalence Tests** (Phase 1):
   - Access grimoire old way (archetype)
   - Access grimoire new way (AssetManager)
   - Assert results are identical

2. **Regression Tests**:
   - Run full test suite after each phase
   - Ensure no functionality broken

---

## Risk Mitigation

### Risk: Breaking Changes Cascade

**Mitigation**:
- Use phased migration with parallel implementation
- Keep old system working during transition
- Comprehensive testing at each phase

### Risk: Data File Reorganization Errors

**Mitigation**:
- Validate data loading extensively
- Keep old data files until fully migrated
- Use schema versioning

### Risk: Context Dependencies

**Mitigation**:
- Document all context updates needed
- Use compile-time checks where possible
- Add runtime validation for AssetManager presence

### Risk: Test Data Updates

**Mitigation**:
- Update test data incrementally
- Use test data versioning
- Maintain backwards compatibility during migration

---

## Timeline Estimate

### Phase 0: Preparation
- **Effort**: 2-3 days
- **Deliverables**: New classes created, compiles but unused

### Phase 1: Parallel Implementation
- **Effort**: 3-4 days
- **Deliverables**: Both systems work, validation tests pass

### Phase 2: Migration
- **Effort**: 4-5 days
- **Deliverables**: All code uses new system

### Phase 3: Cleanup
- **Effort**: 2-3 days
- **Deliverables**: Old code removed, documentation updated

**Total Estimate**: 11-15 days of development work

---

## Success Criteria

1. ✅ GrimoireMachina loads correctly from data files
2. ✅ All Logic classes access grimoire via AssetManager
3. ✅ No archetype lookups for grimoire data remain
4. ✅ All tests pass
5. ✅ No performance regression
6. ✅ Documentation updated
7. ✅ CCraftingState and CMachinaInstance components work correctly
8. ✅ Old CGrimoireMachina component removed from ComponentRegister
9. ✅ Code cleaner and more maintainable

---

## Future Enhancements

Once the refactoring is complete, these features become easier to add:

1. **Grimoire Progression System**:
   - Track unlocked fragments/joints/forms
   - Save unlock state per player profile
   - UI for locked vs. unlocked items

2. **Grimoire Filtering/Search**:
   - Search fragments by name or properties
   - Filter by categories or tags
   - Sort by various criteria

3. **Dynamic Grimoire Loading**:
   - Load grimoire data from DLC or mods
   - Hot-reload grimoire data during development
   - Support multiple grimoire catalogs

4. **Performance Optimizations**:
   - Lazy loading of grimoire data
   - Caching of frequently accessed items
   - Index structures for fast lookup

---

**Document Version**: 1.0  
**Last Updated**: 2026-02-03  
**Author**: GitHub Copilot Agent  
**Status**: Ready for review and approval
