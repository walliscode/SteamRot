# Layering Architecture - Executive Summary

**Date**: December 15, 2025  
**Purpose**: Executive overview of codebase layering analysis

---

## Problem Statement

**Question**: "How do we deal with EngineResources which needs knowledge of level 2 classes to construct?"

**Current Issue**: EngineResources lives in `src/types/core/` (Layer 1 - data layer) but contains EventHandler and AssetManager (Layer 2 - logic/services), violating the architectural principle that the data layer should have zero dependencies on the logic layer.

---

## TL;DR

**Problem**: Layer 1 depends on Layer 2 ❌

**Solution**: Move EngineResources from Layer 1 to Layer 2 ✅

**Effort**: 2-3 hours, low risk, mechanical refactor

**Impact**: Clean three-layer architecture, better testability, clearer rules

---

## Current State

### What We Have

```
src/types/core/EngineResources.h  (Layer 1 - DATA)
  ├── sf::RenderWindow game_window;     ✅ External lib (OK)
  ├── EventHandler event_handler;       ❌ Layer 2 class (WRONG)
  ├── AssetManager asset_manager;       ❌ Layer 2 class (WRONG)
  ├── sf::Vector2i mouse_position;      ✅ POD type (OK)
  └── size_t loop_number;               ✅ POD type (OK)
```

### Why It's Wrong

1. **Layering violation**: Data layer (Layer 1) depends on logic layer (Layer 2)
2. **Can't test independently**: Need to build EventHandler/AssetManager to use EngineResources
3. **Violates design principle**: Types package should have zero dependencies on other packages
4. **Confusing for developers**: Unclear where complex objects should live

---

## Recommended Solution

### Three-Layer Architecture

```
Layer 1: Data + Interfaces
  ├── types/core/           # Pure data (EngineResourcesConfig stays here)
  ├── interfaces/           # Abstract interfaces
  └── components/           # Component data

Layer 2: Logic + Services
  ├── events/               # EventHandler implementation
  ├── assets/               # AssetManager implementation
  ├── logic/                # Game logic
  ├── entity/               # Entity management
  ├── data_providers/       # Data loading
  └── resources/  ← NEW     # EngineResources moves here

Layer 3: Orchestration
  ├── context/              # Reference wrappers (GameContext, SceneContext)
  ├── scenes/               # Scene management
  ├── display/              # Display management
  └── engine/               # Top-level (Engine, GameEngine, TestEngine)
```

### Migration Steps

1. **Create `src/resources/` package** in Layer 2
2. **Move `EngineResources.h`** from `types/core/` to `resources/`
3. **Update include paths** (search and replace, mechanical)
4. **Update CMakeLists.txt** dependencies
5. **Build and test** to verify no regressions

**Estimated Time**: 2-3 hours  
**Risk Level**: LOW (mechanical refactor, no logic changes)

---

## Benefits

### Before (Current)

❌ Layer 1 depends on Layer 2 (violation)  
❌ Can't compile types/ independently  
❌ Can't test data layer in isolation  
❌ Confusing architecture rules  

### After (Proposed)

✅ Clean layer separation (no violations)  
✅ Can compile Layer 1 independently  
✅ Can test data layer with zero mocks  
✅ Clear architectural boundaries  
✅ Better developer onboarding  
✅ Easier to maintain and extend  

---

## Key Insights

### 1. Config vs. Runtime Separation

**EngineResourcesConfig** (stays in Layer 1):
- Pure data: `window_width`, `window_height`, etc.
- POD types only
- No includes of complex classes
- Can be tested with zero dependencies

**EngineResources** (moves to Layer 2):
- Runtime container: owns EventHandler, AssetManager
- Contains complex objects with behavior
- Lives in resources/ package
- Appropriate for Layer 2 (services)

### 2. Ownership vs. Reference Pattern

**EngineResources** (Layer 2):
- **OWNS** the actual objects
- Lives on Engine
- Lifetime matches Engine lifetime

**GameContext/SceneContext** (Layer 3):
- **REFERENCES** the owned objects
- Lightweight wrappers
- Cheap to copy and pass around

### 3. Construction is Top-Down

```
Engine (Layer 3)
  ↓ constructs
EngineResources (Layer 2)
  ↓ contains
EventHandler, AssetManager (Layer 2)
  ↓ implements
IEventHandler, IAssetManager (Layer 1)
```

---

## Architecture Principles

1. **Layer 1 has ZERO dependencies** on Layer 2 or 3
2. **Layer 2 depends ONLY on Layer 1**
3. **Layer 3 depends on Layer 1 + 2**
4. **Dependencies flow downward only**
5. **Top layer constructs everything**, passes by reference

---

## Implementation Guide

### Phase 1: Core Migration (Week 1, 2-3 hours)

- [ ] Create `src/resources/` directory
- [ ] Create `src/resources/CMakeLists.txt`
- [ ] Move `EngineResources.h` from `types/core/` to `resources/`
- [ ] Update all `#include "EngineResources.h"` paths
- [ ] Update CMakeLists.txt dependencies (context, engine)
- [ ] Build and verify: `cmake --build --preset Debug`
- [ ] Test and verify: `ctest --preset Debug`

### Phase 2: Documentation Update (Week 1, 1-2 hours)

- [ ] Update architecture diagrams
- [ ] Update README.md references
- [ ] Update RESOURCE_CONTEXT_ARCHITECTURE.md
- [ ] Create/update layering quick references

### Phase 3: Optional Enhancements (Week 2-3, 4-8 hours)

- [ ] Extract IEventHandler interface
- [ ] Extract IAssetManager interface
- [ ] Update mocks to use interfaces
- [ ] Add dependency checking to CI/CD

---

## Documents Available

### Comprehensive Analysis
📄 [LAYERING_AND_ARCHITECTURE_ANALYSIS.md](./LAYERING_AND_ARCHITECTURE_ANALYSIS.md) - Full 26KB analysis with detailed solutions, pros/cons, and migration guide

### Quick Reference
📋 [LAYERING_QUICK_REFERENCE.md](./LAYERING_QUICK_REFERENCE.md) - 10KB quick lookup guide for layering rules and common patterns

### Visual Diagrams
🗺️ [LAYERING_DIAGRAMS.md](./LAYERING_DIAGRAMS.md) - 27KB visual representation with architecture diagrams, data flow, and construction patterns

---

## Decision Points

### Should We Do This?

**YES** - This migration:
- ✅ Fixes architectural violation
- ✅ Makes architecture clearer
- ✅ Improves testability
- ✅ Low risk (mechanical refactor)
- ✅ Quick turnaround (2-3 hours)
- ✅ Aligns with best practices

**Consider NOT doing this only if**:
- ❌ You're about to do a major refactor anyway
- ❌ The codebase is in active heavy development
- ❌ You prefer "if it ain't broke, don't fix it"

### When Should We Do This?

**Best Times**:
1. During a maintenance sprint
2. Before starting new features
3. As part of architecture improvements
4. When onboarding new developers

**Avoid During**:
1. Major feature development
2. Pre-release stabilization
3. Critical bug fixing periods

---

## Success Criteria

After migration, verify:

1. ✅ All files in `types/` have zero Layer 2 dependencies
2. ✅ `EngineResources.h` lives in `src/resources/`
3. ✅ All includes updated correctly
4. ✅ All CMakeLists.txt dependencies correct
5. ✅ Full test suite passes
6. ✅ Documentation updated
7. ✅ No new compiler warnings

---

## Questions?

### Q: Why not just keep it as-is?

**A**: It violates clean architecture principles, confuses developers, and makes testing harder. The fix is simple and low-risk.

### Q: Why not use interfaces instead?

**A**: Interfaces solve a different problem. EngineResources is an *ownership container* - it should own the actual objects, not just reference them. Moving to Layer 2 is the right solution.

### Q: What if something breaks?

**A**: This is a mechanical refactor with no logic changes. If includes are updated correctly and CMake is configured properly, nothing should break. Full test suite should pass.

### Q: Can we do this incrementally?

**A**: Not really. Since EngineResources is a core struct used everywhere, it's better to do this as one clean migration. But the entire migration is only 2-3 hours.

---

## Next Steps

1. **Review this analysis** - Get team approval
2. **Schedule migration** - Pick a good time (maintenance sprint?)
3. **Execute Phase 1** - Core migration (2-3 hours)
4. **Execute Phase 2** - Update docs (1-2 hours)
5. **Consider Phase 3** - Optional interface extraction (later)

---

**Contact**: For questions or clarifications, refer to the comprehensive analysis documents listed above.

**Document Version**: 1.0  
**Last Updated**: December 15, 2025
