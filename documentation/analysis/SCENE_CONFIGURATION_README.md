# Scene Configuration Architecture Documentation

**Comprehensive planning for Scene configuration workflow from file data to fully configured Scene**

**Status**: ✅ Planning Complete | ⏸️ Implementation Pending

---

## Document Overview

This directory contains complete planning documentation for the Scene configuration architecture redesign. The goal is to decouple FlatBuffers from game code while supporting multiple data sources (default and save data).

### Reading Order

1. **Start Here**: [SCENE_CONFIGURATION_WORKFLOW_SUMMARY.md](SCENE_CONFIGURATION_WORKFLOW_SUMMARY.md)
   - Executive summary addressing problem statement
   - High-level architecture overview
   - Quick answers to key questions

2. **Deep Dive**: [SCENE_FACTORY_AND_CONFIGURATOR_ANALYSIS.md](SCENE_FACTORY_AND_CONFIGURATOR_ANALYSIS.md)
   - Complete architectural analysis
   - Design decisions and rationale
   - Problem statement breakdown
   - Benefits and trade-offs

3. **Quick Reference**: [SCENE_FACTORY_QUICK_REF.md](SCENE_FACTORY_QUICK_REF.md)
   - Interface definitions
   - Usage patterns
   - Code examples
   - Common pitfalls

4. **Visual Guide**: [SCENE_FACTORY_VISUALS.md](SCENE_FACTORY_VISUALS.md)
   - Sequence diagrams
   - Class hierarchies
   - Data flow diagrams
   - Before/after comparisons

5. **Implementation**: [SCENE_CONFIGURATOR_IMPLEMENTATION_GUIDE.md](SCENE_CONFIGURATOR_IMPLEMENTATION_GUIDE.md)
   - Step-by-step implementation
   - Complete code examples
   - Test templates
   - CMakeLists.txt updates

---

## Quick Summary

### Problem

How to configure Scenes from file data (default or save) without:
1. Coupling FlatBuffers into game code
2. Creating duplicate intermediate structs
3. Conditional logic in factory
4. Losing type safety

### Solution

**Strategy Pattern with Configurators**

```
ISceneConfigurator (strategy interface)
    ├─ DefaultSceneConfigurator (for default data)
    └─ SavedSceneConfigurator (for save data)
        ↓
SceneFactory (uses strategy)
    ↓
Configured Scene
```

### Key Components

| Component | Responsibility | File |
|-----------|----------------|------|
| `ISceneConfigurator` | Strategy interface | `src/scenes/ISceneConfigurator.h` |
| `DefaultSceneConfigurator` | Default data loading | `src/scenes/DefaultSceneConfigurator.{h,cpp}` |
| `SavedSceneConfigurator` | Save data loading | `src/scenes/SavedSceneConfigurator.{h,cpp}` |
| `ISceneFactory` | Scene creation (updated) | `src/scenes/ISceneFactory.{h,cpp}` |
| `FlatbuffersSceneFactory` | FlatBuffers config (updated) | `src/scenes/FlatbuffersSceneFactory.{h,cpp}` |
| `SceneManager` | Orchestration (updated) | `src/scenes/SceneManager.{h,cpp}` |

**Note**: For multiple data formats, use format-prefixed names (e.g., `FlatbuffersDefaultSceneConfigurator`, `XmlDefaultSceneConfigurator`). See [Architecture Analysis](SCENE_FACTORY_AND_CONFIGURATOR_ANALYSIS.md#multi-format-naming-pattern) for details.

---

## Architecture at a Glance

### Current (Coupled)

```cpp
// ❌ Direct FlatBuffers coupling
FlatbuffersSceneFactory factory(game_context, scene_data_fbs);
```

### Proposed (Decoupled)

```cpp
// ✅ Strategy pattern
ISceneDataProvider &provider = GetSceneDataProvider();
DefaultSceneConfigurator config(scene_type, provider);
FlatbuffersSceneFactory factory(game_context, config);  // No FlatBuffers!
```

---

## Benefits

### ✅ Decoupling
- Factory never sees FlatBuffers types directly
- Easy to add new data sources (JSON, Lua, network)
- Clean separation of concerns

### ✅ No Duplication
- Configure Scene directly from FlatBuffers
- Single source of truth
- No intermediate struct maintenance

### ✅ Type Safety
- Strong interfaces
- Compiler-enforced contracts
- `std::expected` error handling

### ✅ Testability
- Mock configurators for testing
- Independent component testing
- Clear dependencies

---

## Implementation Status

### ✅ Complete
- [x] Problem analysis
- [x] Architecture design
- [x] Documentation (108KB total)
- [x] Code examples
- [x] Test templates
- [x] Implementation guide

### ⏸️ Pending
- [ ] Create ISceneConfigurator interface
- [ ] Implement DefaultSceneConfigurator
- [ ] Implement SavedSceneConfigurator
- [ ] Update ISceneFactory
- [ ] Update FlatbuffersSceneFactory
- [ ] Update SceneManager
- [ ] Write tests

---

## Usage Examples

### Load Default Scene

```cpp
std::expected<std::monostate, FailInfo>
SceneManager::LoadSceneFromDefault(SceneType scene_type) {
  // 1. Get provider
  ISceneDataProvider &provider = GetSceneDataProvider();
  
  // 2. Create configurator
  DefaultSceneConfigurator config(scene_type, provider);
  
  // 3. Create factory with configurator
  FlatbuffersSceneFactory factory(m_game_context, config);
  
  // 4. Create scene
  auto scene = factory.CreateScene();
  
  // 5. Store scene
  m_scenes.emplace(scene->GetSceneInfo().id, std::move(scene));
  
  return std::monostate{};
}
```

### Load Saved Scene

```cpp
std::expected<std::monostate, FailInfo>
SceneManager::LoadSceneFromSave(uint32_t save_slot) {
  // 1. Get save provider
  ISaveDataProvider &provider = GetSaveDataProvider();
  
  // 2. Create configurator
  SavedSceneConfigurator config(save_slot, provider);
  
  // 3-5: Same as default loading!
  FlatbuffersSceneFactory factory(m_game_context, config);
  auto scene = factory.CreateScene();
  m_scenes.emplace(scene->GetSceneInfo().id, std::move(scene));
  
  return std::monostate{};
}
```

**Notice**: Same factory code for both paths!

---

## Document Statistics

| Document | Size | Purpose |
|----------|------|---------|
| Workflow Summary | 18KB | Executive overview |
| Full Analysis | 19KB | Complete architecture |
| Quick Reference | 14KB | Code patterns |
| Visuals | 34KB | Diagrams and flows |
| Implementation Guide | 23KB | Step-by-step code |
| **Total** | **108KB** | **Complete planning** |

---

## Key Design Decisions

### 1. Configurators Cache Data

Configurators load data once and cache it for the lifetime of Scene creation.

### 2. SceneManager Owns Configurators

Configurators are stack-allocated in SceneManager, lifetime matches Scene creation.

### 3. Factory References Configurator

Factory stores `const ISceneConfigurator&`, doesn't own configurator.

### 4. No Intermediate Native Struct

Factory configures Scene directly from `SceneDataFbs*`, no conversion needed.

### 5. Strategy Over Conditionals

Configurators implement strategy pattern, no `if (is_save)` in factory.

---

## Testing Strategy

### Unit Tests
- ISceneConfigurator with mocks
- DefaultSceneConfigurator data loading
- SavedSceneConfigurator extraction
- Individual configurator logic

### Integration Tests
- SceneFactory with DefaultSceneConfigurator
- SceneFactory with SavedSceneConfigurator
- SceneManager loading workflows

### End-to-End Tests
- Complete default scene loading
- Complete saved scene loading
- Multiple scenes from different sources

---

## Common Questions

### Q: Why not use ISceneDataProvider directly in factory?

**A**: That couples factory to provider types. Configurator provides abstraction layer.

### Q: Why do we need configurators if providers already provide data?

**A**: Providers return their native format. Configurators extract what factory needs (SceneDataFbs*) and handle different provider types uniformly.

### Q: Can we reuse a configurator for multiple scenes?

**A**: No, one configurator per scene. Each configurator is specific to one scene type or save slot.

### Q: What happens if SaveDataFbs doesn't contain SceneDataFbs yet?

**A**: SavedSceneConfigurator.GetSceneData() returns nullptr. Factory handles error gracefully. Update schema when ready.

### Q: Do we need virtual methods or overloaded methods?

**A**: VIRTUAL methods for polymorphism. Overloaded methods don't work with runtime polymorphism.

---

## Related Files

### Existing Files (Will be modified)
```
src/scenes/ISceneFactory.h
src/scenes/ISceneFactory.cpp
src/scenes/FlatbuffersSceneFactory.h
src/scenes/FlatbuffersSceneFactory.cpp
src/scenes/SceneManager.h
src/scenes/SceneManager.cpp
```

### New Files (Will be created)
```
src/scenes/ISceneConfigurator.h
src/scenes/DefaultSceneConfigurator.h
src/scenes/DefaultSceneConfigurator.cpp
src/scenes/SavedSceneConfigurator.h
src/scenes/SavedSceneConfigurator.cpp

tests/scenes/ISceneConfigurator.test.cpp
tests/scenes/DefaultSceneConfigurator.test.cpp
tests/scenes/SavedSceneConfigurator.test.cpp
```

---

## Next Steps

1. **Review Planning**: Review all documentation, ensure design is sound
2. **Approve Architecture**: Get sign-off on strategy pattern approach
3. **Begin Implementation**: Follow implementation guide step-by-step
4. **Write Tests**: Create tests alongside implementation
5. **Validate**: Ensure no FlatBuffers coupling in factory
6. **Document**: Update README with actual implementation notes

---

## Contact

**Documentation Created**: December 9, 2025  
**Issue**: walliscode/SteamRot (plan workflow for scene data)  
**Status**: Planning Complete, Ready for Implementation

---

## Document Index

All documents in this analysis:

1. [SCENE_CONFIGURATION_README.md](SCENE_CONFIGURATION_README.md) ← You are here
2. [SCENE_CONFIGURATION_WORKFLOW_SUMMARY.md](SCENE_CONFIGURATION_WORKFLOW_SUMMARY.md)
3. [SCENE_FACTORY_AND_CONFIGURATOR_ANALYSIS.md](SCENE_FACTORY_AND_CONFIGURATOR_ANALYSIS.md)
4. [SCENE_FACTORY_QUICK_REF.md](SCENE_FACTORY_QUICK_REF.md)
5. [SCENE_FACTORY_VISUALS.md](SCENE_FACTORY_VISUALS.md)
6. [SCENE_CONFIGURATOR_IMPLEMENTATION_GUIDE.md](SCENE_CONFIGURATOR_IMPLEMENTATION_GUIDE.md)
