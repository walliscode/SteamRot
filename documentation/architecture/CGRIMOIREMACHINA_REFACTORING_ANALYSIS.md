# CGrimoireMachina Architectural Refactoring Analysis

**Date**: 2026-02-03  
**Status**: Analysis & Design Phase  
**Purpose**: Evaluate proposal to move CGrimoireMachina from EntityMemoryPool to AssetManager

---

## Executive Summary

This document analyzes the architectural proposal to refactor `CGrimoireMachina` from a Component in the EntityMemoryPool (EMP) to a standalone class managed by the `AssetManager`. The change would transform CGrimoireMachina from an entity-bound component to a global data store/database accessible throughout the application lifecycle.

---

## Current Architecture

### CGrimoireMachina as a Component

**Location**: `src/types/components/CGrimoireMachina.h`

**Current Structure**:
```cpp
struct CGrimoireMachina : public Component {
  // Database of all fragments available in the game
  std::map<std::string, Fragment> m_all_fragments;
  
  // Database of all joints available in the game
  std::map<std::string, Joint> m_all_joints;
  
  // Collection of all available MachinaForms (templates)
  std::map<std::string, CMachinaForm> m_machina_forms;
  
  // Working/holding form for building new structures
  std::unique_ptr<CMachinaForm> m_holding_form{nullptr};
};
```

**Current Integration**:
- Part of `ComponentRegister` tuple in `containers.h`
- Stored in EntityMemoryPool as a component vector
- Accessed via archetype system by Logic classes
- Typically one entity per scene contains CGrimoireMachina
- Configured via FlatBuffers through `FlatbuffersEntityConfigurator`

### CMachinaForm as a Component

**Location**: `src/types/components/CMachinaForm.h`

**Current Structure**:
```cpp
struct CMachinaForm : public Component {
  // All Fragments for this Entity/MachinaForm
  std::vector<Fragment> m_fragments;
  
  // All Joints for this Entity/MachinaForm
  std::vector<Joint> m_joints;
};
```

**Purpose**:
- Represents a specific combination of Fragments and Joints
- Defines an entity's physical structure
- Immutable by design (modification = copy + edit)

### AssetManager Current State

**Location**: `src/assets/AssetManager.h`

**Current Responsibilities**:
- Font management (loaded from files)
- UI Style management
- Implements `IFontProvider` interface

**Structure**:
```cpp
class AssetManager : public IFontProvider {
private:
  DataAccessFactory &m_data_access_factory;
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> m_fonts;
  std::unordered_map<std::string, UIStyle> m_ui_styles;
  
public:
  // Load assets from configuration
  // Provide font access
  // Provide UI style access
};
```

### Current Access Pattern

Logic classes currently access CGrimoireMachina through the archetype system:

```cpp
// From src/logic/ui_action.cpp
ArchetypeID grimoire_archetype_id =
    archetypes::GenerateArchetypeIDfromTypes<CGrimoireMachina>();

const auto it = scene_context.archetypes.find(grimoire_archetype_id);
if (it != scene_context.archetypes.end()) {
  const Archetype &archetype = it->second;
  if (!archetype.empty()) {
    size_t entity_id = *archetype.begin();
    const CGrimoireMachina &grimoire_machina =
        entity::memory::GetComponent<CGrimoireMachina>(
            entity_id, scene_context.scene_entities);
    // Use grimoire_machina...
  }
}
```

---

## Proposed Architecture

### CGrimoireMachina as a Standalone Class

**Proposed Changes**:

1. **Remove Component Inheritance**
   - No longer inherit from `Component`
   - Remove from `ComponentRegister` tuple
   - No longer part of EntityMemoryPool

2. **Move to AssetManager**
   - Add as a member of `AssetManager`
   - Managed alongside fonts and UI styles
   - Loaded during asset initialization

3. **CMachinaForm Transformation**
   - Also remove from EntityMemoryPool
   - Create new Component that references/contains CMachinaForm instances
   - CMachinaForm becomes a pure data class (not a Component)

4. **New Component for Entity Association**
   - Create something like `CEquippedMachina` or `CMachinaInstance`
   - This component would hold an instance of CMachinaForm
   - Entities needing machina forms would use this new component

### Proposed AssetManager Structure

```cpp
class AssetManager : public IFontProvider {
private:
  DataAccessFactory &m_data_access_factory;
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> m_fonts;
  std::unordered_map<std::string, UIStyle> m_ui_styles;
  
  // NEW: Global grimoire database
  std::unique_ptr<GrimoireMachina> m_grimoire_machina;
  
public:
  // Existing methods...
  
  // NEW: Access to grimoire
  const GrimoireMachina& GetGrimoireMachina() const;
  GrimoireMachina& GetGrimoireMachina();
};
```

### Proposed Access Pattern

```cpp
// Direct access via AssetManager reference
const GrimoireMachina& grimoire = asset_manager.GetGrimoireMachina();
std::vector<std::string> fragment_names = grimoire.GetAllFragmentNames();

// No archetype lookup needed
// No entity ID required
// Simpler, more direct access
```

---

## Analysis

### Conceptual Alignment

#### Current Model (Component-Based)
- **Assumption**: Grimoire data is per-scene or per-entity
- **Reality**: Only one CGrimoireMachina exists per scene
- **Implication**: Using EMP/archetype system for singleton-like access

#### Proposed Model (Asset-Based)
- **Assumption**: Grimoire data is global/game-wide
- **Reality**: All fragments, joints, forms are shared across scenes
- **Implication**: Better semantic fit for game-wide database

**Analysis**: The proposed model better reflects the actual use case. CGrimoireMachina is conceptually a **database** or **catalog**, not entity-specific data.

### Pros of the Proposed Change

#### 1. **Semantic Clarity**
- AssetManager is designed for shared, reusable game resources
- CGrimoireMachina is clearly a catalog/database of craftable items
- Better separation of concerns: entities use forms, grimoire provides forms

#### 2. **Simplified Access**
- No archetype lookup needed
- No entity ID required
- Direct access via AssetManager reference
- Reduces boilerplate in Logic classes

#### 3. **Persistence and Save File Independence**
- Currently tied to scene entity data
- Moving to AssetManager makes it independent of scene saves
- Grimoire data could be loaded once at game start
- Save files would only need to save entity-specific machina instances

#### 4. **Performance**
- Eliminates archetype lookup overhead
- One instance for entire application
- No need to duplicate across scenes
- Faster access pattern

#### 5. **Scalability**
- Easy to add new grimoire-related methods
- Could add grimoire progression system (unlockable forms)
- Could add grimoire filtering/searching
- Better encapsulation for future features

#### 6. **Testing**
- Easier to mock for unit tests
- Can test logic without full EMP setup
- Grimoire can be tested independently

### Cons of the Proposed Change

#### 1. **Breaking Change**
- Requires updates to all Logic classes accessing CGrimoireMachina
- Changes to FlatBuffers schema
- Updates to configurators
- Migration of test data
- Significant refactoring effort

#### 2. **Increased AssetManager Responsibility**
- AssetManager becomes larger, more complex
- Currently focused on rendering assets (fonts, styles)
- Adding game logic data (grimoire) broadens scope
- Could violate Single Responsibility Principle

#### 3. **Loss of Scene-Specific Grimoire State**
- If different scenes need different available forms
- If grimoire state varies by game progress
- Currently easy: different scenes = different grimoire entities
- Proposed: need additional state management

#### 4. **Save File Complexity**
- If grimoire has player-specific state (unlocked forms)
- Need to determine: is grimoire data per-save or global?
- Might need additional save/load logic for grimoire state

#### 5. **Dependency Management**
- Logic classes gain dependency on AssetManager
- Need to pass AssetManager reference through contexts
- Increases coupling to AssetManager

#### 6. **Holding Form Management**
- `m_holding_form` is working state, not asset data
- Might need to move to a different location (scene state?)
- AssetManager should be read-only for assets

---

## Alternative Approaches

### Option 1: Status Quo (Keep as Component)
- **Pros**: No changes needed, established pattern works
- **Cons**: Conceptually mismatched, verbose access pattern
- **Verdict**: Works but not ideal

### Option 2: Separate Grimoire Service Class
- Create `GrimoireService` class independent of both EMP and AssetManager
- Managed by GameContext or similar
- **Pros**: Single responsibility, clear ownership
- **Cons**: Another class to manage, need DI for access
- **Verdict**: Clean but adds complexity

### Option 3: Hybrid Approach
- Global grimoire catalog in AssetManager (m_all_fragments, m_all_joints, m_machina_forms)
- Working state (m_holding_form) remains in component system or scene state
- **Pros**: Separates static data from dynamic state
- **Cons**: Split responsibility across systems
- **Verdict**: Best of both worlds?

### Option 4: Keep in EMP, Improve Access
- Add helper functions to directly access grimoire without archetype lookup
- Cache grimoire reference in contexts
- **Pros**: Minimal changes, improves ergonomics
- **Cons**: Still conceptually a component when it's not
- **Verdict**: Bandaid solution

---

## Recommendations

### Short-Term Recommendation: Hybrid Approach (Option 3)

**Phase 1: Static Data to AssetManager**
1. Move static grimoire data to AssetManager:
   - `m_all_fragments` 
   - `m_all_joints`
   - `m_machina_forms` (as templates)

2. Keep dynamic state in component system:
   - `m_holding_form` could stay as a component
   - Or move to scene-specific state class

**Benefits**:
- Clear separation: static catalog vs. working state
- AssetManager remains focused on reusable data
- Scene state remains in scene/component system
- Gradual migration path

**Phase 2: Create New Component for Instances**
1. Remove `CMachinaForm` from ComponentRegister
2. Create `CMachinaInstance` component:
   ```cpp
   struct CMachinaInstance : public Component {
     // Reference or copy of a MachinaForm
     MachinaForm form;  // or std::string form_template_name
     
     // Entity-specific state
     // (modifications, damage, etc.)
   };
   ```

3. Entities that need machina forms use `CMachinaInstance`

### Long-Term Recommendation: Full Refactor

After validating the hybrid approach:

1. **Fully remove CGrimoireMachina from EMP**
2. **Create dedicated GrimoireService or integrate into AssetManager**
3. **Establish clear ownership**:
   - AssetManager: Static grimoire catalog
   - Scene/Component: Entity-specific machina instances
   - Scene State: Working/holding forms for crafting UI

---

## Migration Considerations

### Code Changes Required

#### 1. AssetManager Updates
```cpp
// Add grimoire data members
std::map<std::string, Fragment> m_fragments;
std::map<std::string, Joint> m_joints;
std::map<std::string, MachinaForm> m_machina_form_templates;

// Add loading methods
std::expected<std::monostate, FailInfo> LoadGrimoireData();

// Add accessors
const std::map<std::string, Fragment>& GetAllFragments() const;
const std::map<std::string, Joint>& GetAllJoints() const;
const std::map<std::string, MachinaForm>& GetMachinaFormTemplates() const;
```

#### 2. Logic Class Updates
All logic classes currently accessing CGrimoireMachina need updates:
- `src/logic/ui_action.cpp` - Update dropdown population
- `src/logic/ui_helpers.cpp` - Update helper functions
- `src/logic/CraftingRenderLogic.h` - Update rendering logic

Change from:
```cpp
// Find archetype, get entity, get component
ArchetypeID id = archetypes::GenerateArchetypeIDfromTypes<CGrimoireMachina>();
// ... lookup logic ...
const CGrimoireMachina& grimoire = GetComponent<CGrimoireMachina>(...);
```

To:
```cpp
// Direct access via AssetManager
const GrimoireMachina& grimoire = asset_manager.GetGrimoireMachina();
```

#### 3. Context Updates
Update `SceneContext`, `LogicContext`, or similar to include:
```cpp
struct SceneContext {
  // ... existing members ...
  AssetManager& asset_manager;  // Add if not already present
};
```

#### 4. FlatBuffers Schema Changes
- Create new schema for grimoire data
- Separate from entity data
- Update asset configuration schema

#### 5. Data File Reorganization
- Move grimoire data from scene/entity JSON files
- Create dedicated grimoire data files
- Update data loading logic

#### 6. Test Updates
- Update all tests accessing CGrimoireMachina
- Create new AssetManager tests for grimoire
- Update integration tests
- Update test data files

### Migration Steps

**Phase 1: Preparation**
1. Document all current CGrimoireMachina access points
2. Create new AssetManager grimoire interface
3. Add grimoire data loading to AssetManager
4. Update tests for new interface

**Phase 2: Parallel Implementation**
1. Keep CGrimoireMachina as component
2. Add grimoire to AssetManager
3. Update logic classes to use both (temporarily)
4. Validate both approaches work

**Phase 3: Migration**
1. Remove CGrimoireMachina component
2. Remove from ComponentRegister
3. Update all logic classes to use AssetManager only
4. Clean up old code paths

**Phase 4: Refinement**
1. Handle holding form state appropriately
2. Create new component for machina instances
3. Update save/load logic
4. Finalize documentation

---

## Questions for Stakeholders

1. **Grimoire State**: Should grimoire data be per-save (player progression) or global (all forms always available)?

2. **Holding Form**: Where should working state (m_holding_form) live?
   - Scene state class?
   - New component?
   - UI state component?

3. **CMachinaForm**: Should entities have:
   - Direct instances of MachinaForm?
   - References to templates?
   - Copy-on-write shared pointers?

4. **Save Files**: How should machina forms be persisted?
   - Save full form data per entity?
   - Save template name + modifications?

5. **Progression**: Is there a progression/unlock system for forms?
   - If yes, where should unlock state be managed?

6. **Performance**: Are there performance concerns?
   - How many entities will have machina forms?
   - How often is grimoire accessed?

---

## Conclusion

The proposal to move `CGrimoireMachina` to `AssetManager` has **strong merit** from a conceptual and architectural standpoint. CGrimoireMachina is fundamentally a **catalog/database** of craftable items, not entity-specific data, making it a better fit for the AssetManager.

**Recommended Approach**: 
- **Hybrid/Phased Migration** starting with static data
- **Maintain clear separation** between catalog (AssetManager) and instances (Component system)
- **Gradual refactoring** to minimize risk and validate assumptions

**Key Benefits**:
- Simpler access pattern
- Better semantic alignment
- Improved testability
- Foundation for future features (progression, filtering, etc.)

**Key Risks**:
- Breaking change requiring comprehensive updates
- Need to carefully manage working state (holding form)
- AssetManager scope expansion
- Migration effort and testing

The change should be undertaken with careful planning, comprehensive testing, and clear documentation of the new patterns.

---

## Next Steps

1. **Stakeholder Review**: Gather feedback on this analysis
2. **Decision**: Approve, modify, or reject proposal
3. **Planning**: If approved, create detailed implementation plan
4. **Prototyping**: Implement hybrid approach in isolated branch
5. **Validation**: Test with real scenarios before full migration
6. **Documentation**: Update all relevant documentation
7. **Implementation**: Execute migration in phases
8. **Testing**: Comprehensive testing at each phase

---

**Document Version**: 1.0  
**Last Updated**: 2026-02-03  
**Author**: GitHub Copilot Agent  
**Status**: Awaiting stakeholder review
