# CGrimoireMachina Refactoring: Quick Reference

**Date**: 2026-02-03  
**Status**: Analysis & Design Phase

---

## Overview

This is a **quick reference guide** for the proposed CGrimoireMachina architectural refactoring. For complete details, see:

- **[Full Analysis](./CGRIMOIREMACHINA_REFACTORING_ANALYSIS.md)** - Comprehensive analysis with pros/cons
- **[Design Proposal](./CGRIMOIREMACHINA_DESIGN_PROPOSAL.md)** - Concrete implementation design

---

## The Proposal

**Move CGrimoireMachina from EntityMemoryPool to AssetManager**

### Current State
```cpp
// CGrimoireMachina is a Component in the EntityMemoryPool
struct CGrimoireMachina : public Component {
  std::map<std::string, Fragment> m_all_fragments;
  std::map<std::string, Joint> m_all_joints;
  std::map<std::string, CMachinaForm> m_machina_forms;
  std::unique_ptr<CMachinaForm> m_holding_form;
};

// Accessed via archetype lookup (verbose)
ArchetypeID id = GenerateArchetypeIDfromTypes<CGrimoireMachina>();
const CGrimoireMachina& grimoire = GetComponent<CGrimoireMachina>(...);
```

### Proposed State
```cpp
// GrimoireMachina as standalone class in AssetManager
class GrimoireMachina {
  std::map<std::string, Fragment> m_all_fragments;
  std::map<std::string, Joint> m_all_joints;
  std::map<std::string, MachinaForm> m_machina_form_templates;
  // m_holding_form moved to separate CCraftingState component
};

// Direct access (simple)
const GrimoireMachina& grimoire = asset_manager.GetGrimoireMachina();
```

---

## Key Changes

### Classes Being Created

1. **`GrimoireMachina`** - Standalone class for grimoire catalog
   - Location: `src/assets/GrimoireMachina.h`
   - Role: Global database of fragments, joints, form templates
   - Managed by: AssetManager

2. **`CCraftingState`** - Component for working state
   - Location: `src/types/components/CCraftingState.h`
   - Role: Holds work-in-progress forms during crafting
   - Contains: `m_holding_form` (moved from CGrimoireMachina)

3. **`CMachinaInstance`** - Component for entity-equipped forms
   - Location: `src/types/components/CMachinaInstance.h`
   - Role: Associates entities with machina forms
   - Contains: Instance of MachinaForm for this specific entity

### Classes Being Modified

1. **`MachinaForm`** - No longer a Component
   - Old: `src/types/components/CMachinaForm.h` (Component)
   - New: `src/types/core/MachinaForm.h` (Pure data class)
   - Change: Remove Component inheritance

2. **`AssetManager`** - Add grimoire management
   - Add: `m_grimoire_machina` member
   - Add: `LoadGrimoireData()` method
   - Add: `GetGrimoireMachina()` accessor

3. **`ComponentRegister`** - Update component list
   - Remove: CGrimoireMachina, CMachinaForm
   - Add: CCraftingState, CMachinaInstance

### Classes Being Removed (Eventually)

1. **`CGrimoireMachina`** - No longer needed
2. **`CMachinaForm` (as Component)** - Becomes pure data class

---

## Why This Change?

### Conceptual Mismatch (Current)
- CGrimoireMachina is conceptually a **database/catalog**
- But implemented as a **Component** in EntityMemoryPool
- Only one instance per scene (singleton pattern in ECS)
- Verbose archetype lookup for global data

### Better Alignment (Proposed)
- AssetManager manages shared game resources
- Grimoire is a catalog of craftable items (resource)
- Direct access without entity/archetype complexity
- Clearer separation: catalog (AssetManager) vs. instances (Components)

### Benefits
✅ **Simpler access** - No archetype lookups  
✅ **Better semantics** - Data location matches purpose  
✅ **Easier testing** - Can test grimoire independently  
✅ **Performance** - Eliminate lookup overhead  
✅ **Scalability** - Easy to add features (progression, filtering)  
✅ **Save files** - Grimoire separate from entity saves

---

## Recommended Approach: Hybrid Migration

### Phase 1: Static Catalog to AssetManager
Move static grimoire data:
- Fragment catalog → AssetManager
- Joint catalog → AssetManager
- MachinaForm templates → AssetManager

### Phase 2: Working State to Component
Keep dynamic state in component system:
- `m_holding_form` → CCraftingState component
- Per-entity forms → CMachinaInstance component

### Phase 3: Gradual Migration
1. Create new classes (don't break old code)
2. Parallel implementation (both work)
3. Migrate logic classes one by one
4. Remove old code once all migrated

---

## Access Pattern Comparison

### Before (Current - Verbose)
```cpp
// Find the archetype
ArchetypeID grimoire_archetype_id =
    archetypes::GenerateArchetypeIDfromTypes<CGrimoireMachina>();

// Find entities with that archetype
const auto it = scene_context.archetypes.find(grimoire_archetype_id);
if (it != scene_context.archetypes.end()) {
  const Archetype &archetype = it->second;
  
  // Get first entity (assuming only one)
  if (!archetype.empty()) {
    size_t entity_id = *archetype.begin();
    
    // Finally get the component
    const CGrimoireMachina &grimoire_machina =
        entity::memory::GetComponent<CGrimoireMachina>(
            entity_id, scene_context.scene_entities);
    
    // Use grimoire_machina...
    std::vector<std::string> names = GetAllFragmentNames(grimoire_machina);
  }
}
```

### After (Proposed - Simple)
```cpp
// Direct access
const GrimoireMachina &grimoire = asset_manager.GetGrimoireMachina();

// Use grimoire
std::vector<std::string> names = grimoire.GetAllFragmentNames();
```

---

## Code Organization

### New File Structure
```
src/
├── assets/
│   ├── AssetManager.h (modified)
│   ├── AssetManager.cpp (modified)
│   ├── GrimoireMachina.h (NEW)
│   └── GrimoireMachina.cpp (NEW)
│
├── types/
│   ├── components/
│   │   ├── CCraftingState.h (NEW)
│   │   ├── CMachinaInstance.h (NEW)
│   │   ├── CGrimoireMachina.h (REMOVE eventually)
│   │   └── CMachinaForm.h (REMOVE/MOVE)
│   │
│   └── core/
│       ├── MachinaForm.h (MOVED from components/)
│       └── MachinaForm.cpp (NEW if needed)
│
└── logic/
    ├── ui_action.cpp (MODIFY - update access pattern)
    ├── ui_helpers.h (MODIFY - update signatures)
    └── ui_helpers.cpp (MODIFY - update implementations)
```

---

## Data Flow Changes

### Loading (Game Initialization)
```
Old:
Scene Load → Entity Data → Configure CGrimoireMachina Component

New:
Game Start → Asset Load → Load GrimoireMachina → AssetManager
```

### Runtime Access
```
Old:
Logic → Archetype → Entity → Component

New:
Logic → AssetManager → GrimoireMachina
```

---

## Migration Phases

### Phase 0: Preparation (No Breaking Changes)
- ⏱️ 2-3 days
- Create new classes (unused)
- Add to AssetManager (parallel)
- Create new data files

### Phase 1: Parallel Implementation
- ⏱️ 3-4 days
- Both systems functional
- Validation tests comparing both
- Verify data equivalence

### Phase 2: Migration
- ⏱️ 4-5 days
- Update all Logic classes
- Update contexts
- Deprecate old component

### Phase 3: Cleanup
- ⏱️ 2-3 days
- Remove old code
- Update tests
- Update documentation

**Total**: ~11-15 days

---

## Questions to Answer

Before implementing, stakeholders should decide:

1. **Grimoire State**: Per-save (player progression) or global?
2. **Holding Form**: Where does crafting working state live?
3. **CMachinaForm Instances**: Direct instances or references to templates?
4. **Save Files**: How to persist machina forms?
5. **Progression System**: Will forms be unlockable?

---

## Files Impacted

### Create (~8 files)
- GrimoireMachina class (h/cpp)
- CCraftingState component (h)
- CMachinaInstance component (h)
- MachinaForm data class (h/cpp)
- Grimoire FlatBuffers schema
- Grimoire data files

### Modify (~15+ files)
- AssetManager (h/cpp)
- ComponentRegister
- All Logic classes accessing grimoire
- Context classes
- Data providers
- Helper functions

### Remove (~5 files)
- CGrimoireMachina (h)
- CMachinaForm component (h)
- Related test files
- Old schemas

---

## Risks & Mitigation

| Risk | Mitigation |
|------|------------|
| Breaking changes cascade | Phased migration, keep old system during transition |
| Data file errors | Extensive validation, keep old files temporarily |
| Context dependencies | Document all updates, compile-time checks |
| Test data updates | Incremental updates, maintain backwards compatibility |

---

## Success Metrics

- ✅ All grimoire access uses AssetManager (no archetype lookups)
- ✅ All tests pass
- ✅ No performance regression
- ✅ Code is cleaner and more maintainable
- ✅ Documentation fully updated

---

## Next Steps

1. **Review** - Stakeholders review analysis and design
2. **Decide** - Approve, modify, or reject proposal
3. **Plan** - Create detailed task breakdown
4. **Prototype** - Test hybrid approach in branch
5. **Implement** - Execute phased migration
6. **Test** - Comprehensive testing at each phase
7. **Document** - Update all documentation

---

## Related Documents

- **[Executive Summary](./CGRIMOIREMACHINA_EXECUTIVE_SUMMARY.md)** - Decision-maker summary with recommendations
- **[Full Analysis](./CGRIMOIREMACHINA_REFACTORING_ANALYSIS.md)** - 16KB, comprehensive analysis
- **[Design Proposal](./CGRIMOIREMACHINA_DESIGN_PROPOSAL.md)** - 21KB, concrete design
- **[Main README](../../README.md)** - Project overview and getting started

---

**Document Version**: 1.0  
**Last Updated**: 2026-02-03  
**Status**: Awaiting stakeholder decision
