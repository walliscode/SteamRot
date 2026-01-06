# Addendum: SceneData Polymorphism and Pointer Passing

This document addresses whether SceneData still needs polymorphism and pointer passing after implementing the IEntityImporter/Exporter pattern.

## Question

> "I'm assuming we also don't need to be passing pointers of SceneData around as we have no needs of its polymorphism anymore?"

## Answer: Correct!

**Yes, you're absolutely right!** With the Importer/Exporter pattern, SceneData no longer needs to be polymorphic, and we can pass it by value or const reference instead of pointers.

## Current Situation (With Polymorphism)

### Why Polymorphism Exists Today

```cpp
// Base class with virtual destructor (for polymorphism)
struct SceneData {
  virtual ~SceneData() = default;  // ← Virtual destructor
  
  SceneInfo scene_info;
  SceneResourcesConfig scene_resources_config;
  AssetConfig scene_asset_config;
};

// Derived class adds pointer to heavy data
struct FbsSceneData : public SceneData {
  const EntityCollectionFbs *entity_collection = nullptr;
};
```

**Why pointers are needed:**
```cpp
// Provider returns base pointer to derived type
unique_ptr<SceneData> data = provider.ProvideDefaultSceneData(type);
//                     ↑ Base pointer to FbsSceneData

// Configurator receives base pointer
ConfigureScene(Scene &scene, const SceneData *scene_data);
//                            ↑ Must be pointer for polymorphism

// Then downcasts to derived type
FbsSceneData *fbs = dynamic_cast<FbsSceneData*>(scene_data);
```

**Polymorphism is needed** because:
1. Provider returns different derived types (FbsSceneData, potential JsonSceneData, etc.)
2. Configurator receives base pointer and downcasts
3. Heavy data (EntityCollectionFbs) is smuggled through inheritance

## After Importer/Exporter Pattern (No Polymorphism Needed)

### SceneData Becomes a Plain Struct

```cpp
// Remove virtual destructor - no longer polymorphic
struct SceneData {
  // No virtual destructor needed!
  
  SceneInfo scene_info;
  SceneResourcesConfig scene_resources_config;
  AssetConfig scene_asset_config;
};

// No derived types needed - FbsSceneData goes away!
```

### Why Polymorphism is No Longer Needed

1. **No Derived Types**: FbsSceneData is eliminated - heavy data goes in Importer
2. **No Downcasting**: Configurator doesn't need to downcast SceneData
3. **Single Concrete Type**: Only one SceneData struct exists (no JsonSceneData, etc.)
4. **Data Format Abstraction**: Moved to IEntityImporter implementations

### Passing SceneData by Value or Reference

```cpp
// Option 1: Pass by const reference (recommended)
std::expected<std::monostate, FailInfo>
ConfigureSceneInfo(Scene &scene, const SceneData &scene_data) {
  scene.GetSceneInfo().type = scene_data.scene_info.type;
  scene.GetSceneInfo().id = scene_data.scene_info.id;
  return std::monostate{};
}

// Option 2: Pass by value (for small structs or if copying is cheap)
std::expected<std::monostate, FailInfo>
ConfigureSceneResources(Scene &scene, SceneData scene_data) {
  // scene_data is a copy
  // ...
}

// NO LONGER NEEDED: Pointer passing for polymorphism
// ❌ ConfigureScene(Scene &scene, const SceneData *scene_data)
```

## Updated Architecture

### Provider Returns Value or Smart Pointer

```cpp
// Option A: Return by value (SceneData is lightweight)
struct SceneLoadData {
  SceneData scene_data;  // By value, not pointer!
  unique_ptr<IEntityImporter> entity_importer;
};

expected<SceneLoadData, FailInfo>
ProvideDefaultSceneLoadData(SceneType scene_type) const {
  // Create SceneData on stack
  SceneData scene_data;
  scene_data.scene_info = /* ... */;
  scene_data.scene_resources_config = /* ... */;
  scene_data.scene_asset_config = /* ... */;
  
  // Create Importer with heavy data
  auto importer = make_unique<FlatbuffersEntityImporter>(/* ... */);
  
  // Return both (scene_data moves into struct)
  return SceneLoadData{
    .scene_data = std::move(scene_data),
    .entity_importer = std::move(importer)
  };
}
```

```cpp
// Option B: Still use unique_ptr (if preferred for consistency)
struct SceneLoadData {
  unique_ptr<SceneData> scene_data;  // Concrete type, not polymorphic
  unique_ptr<IEntityImporter> entity_importer;
};
```

### SceneFactory Uses Value Semantics

```cpp
// Receive SceneLoadData
auto load_result = provider.ProvideDefaultSceneLoadData(type);
SceneLoadData &load_data = load_result.value();

// Pass by const reference (no pointer needed)
auto config_result = configurator.ConfigureScene(
    *scene, 
    load_data.scene_data);  // If scene_data is by value in struct
    // OR
    *load_data.scene_data);  // If scene_data is unique_ptr in struct

// Import entities
auto import_result = load_data.entity_importer->ImportEntities(
    scene->GetEntityMemoryPool());
```

## Benefits of Removing Polymorphism

### 1. Simpler Type System

```cpp
// Before (polymorphic)
SceneData           ← Abstract base (virtual destructor)
  └─ FbsSceneData   ← Concrete derived type

// After (monomorphic)
SceneData           ← Concrete type (no inheritance)
```

### 2. No Virtual Function Call Overhead

```cpp
// Before: Virtual destructor has runtime cost
struct SceneData {
  virtual ~SceneData() = default;  // vtable lookup
};

// After: No virtual functions
struct SceneData {
  // Trivial destructor (compiler-generated)
};
```

### 3. Better Compiler Optimizations

- **Before**: Compiler can't optimize across virtual function boundaries
- **After**: Compiler can inline, optimize, and better understand data flow

### 4. Clearer Ownership Semantics

```cpp
// Before: Pointer passing unclear ownership
void ConfigureScene(Scene &scene, const SceneData *data);
// Who owns this? When is it deleted? Nullable?

// After: Reference semantics are clear
void ConfigureScene(Scene &scene, const SceneData &data);
// Non-owning, non-null, temporary access
```

### 5. No More Dynamic Cast

```cpp
// Before: Runtime type checking
FbsSceneData *fbs = dynamic_cast<FbsSceneData*>(data);
if (!fbs) { /* error */ }

// After: No casting needed
// SceneData is what it is - concrete type
```

## Interface Changes Summary

### Before (Polymorphic Pointers)

```cpp
// Provider
virtual expected<unique_ptr<SceneData>, FailInfo>
ProvideDefaultSceneData(SceneType) const = 0;
//                       ↑ Pointer for polymorphism

// Configurator
virtual expected<std::monostate, FailInfo>
ConfigureSceneInfo(Scene &scene, const SceneData *scene_data) = 0;
//                                ↑ Pointer for polymorphism
```

### After (Value or Reference)

```cpp
// Provider (Option A - by value)
struct SceneLoadData {
  SceneData scene_data;  // By value
  unique_ptr<IEntityImporter> entity_importer;
};

virtual expected<SceneLoadData, FailInfo>
ProvideDefaultSceneLoadData(SceneType) const = 0;

// Configurator (by const reference)
virtual expected<std::monostate, FailInfo>
ConfigureSceneInfo(Scene &scene, const SceneData &scene_data) = 0;
//                                ↑ Reference, not pointer
```

## Is SceneData Still Useful?

**Yes!** SceneData remains useful as a **data container** even without polymorphism:

### What SceneData Provides

1. **Configuration Data**: SceneInfo, SceneResourcesConfig, AssetConfig
2. **Type Safety**: Structured data instead of loose parameters
3. **Encapsulation**: Groups related configuration together
4. **Reusability**: Same struct used by all scenes

### What SceneData No Longer Provides

1. ~~**Format Abstraction**: (Moved to IEntityImporter)~~
2. ~~**Heavy Data Transport**: (Moved to IEntityImporter)~~
3. ~~**Runtime Polymorphism**: (Not needed anymore)~~

## Migration Checklist

When implementing the Importer/Exporter pattern:

- [ ] Remove `virtual ~SceneData() = default;` from SceneData
- [ ] Delete `FbsSceneData` struct (no longer needed)
- [ ] Change `ISceneDataProvider` to return `SceneLoadData` (with value or unique_ptr)
- [ ] Update `ISceneConfigurator` methods to take `const SceneData&` instead of `const SceneData*`
- [ ] Update all configurator implementations to use reference instead of pointer
- [ ] Remove all `dynamic_cast<FbsSceneData*>` calls
- [ ] Remove all `const_cast` calls related to SceneData
- [ ] Update SceneFactory to use value/reference semantics
- [ ] Update tests to use value/reference semantics

## Example: Complete Before/After

### Before (Polymorphic)

```cpp
// Types
struct SceneData {
  virtual ~SceneData() = default;  // Polymorphic
  SceneInfo scene_info;
  SceneResourcesConfig scene_resources_config;
  AssetConfig scene_asset_config;
};

struct FbsSceneData : public SceneData {  // Derived type
  const EntityCollectionFbs *entity_collection = nullptr;
};

// Usage
unique_ptr<SceneData> data = provider.ProvideDefaultSceneData(type);
//          ↑ Base pointer to derived type

configurator.ConfigureScene(*scene, data.get());
//                                   ↑ Raw pointer

// Inside configurator
FbsSceneData *fbs = dynamic_cast<FbsSceneData*>(
    const_cast<SceneData*>(scene_data));  // Downcast + const_cast
```

### After (Monomorphic)

```cpp
// Types
struct SceneData {  // No virtual destructor
  SceneInfo scene_info;
  SceneResourcesConfig scene_resources_config;
  AssetConfig scene_asset_config;
};

struct SceneLoadData {
  SceneData scene_data;  // By value
  unique_ptr<IEntityImporter> entity_importer;
};

// Usage
SceneLoadData load_data = provider.ProvideDefaultSceneLoadData(type).value();
//            ↑ Concrete type

configurator.ConfigureScene(*scene, load_data.scene_data);
//                                   ↑ Direct access to value

// Inside configurator
void ConfigureSceneInfo(Scene &scene, const SceneData &scene_data) {
  // No casting needed - scene_data is what it is
  scene.GetSceneInfo().type = scene_data.scene_info.type;
}
```

## Summary Table

| Aspect | Before (Polymorphic) | After (Monomorphic) |
|--------|---------------------|---------------------|
| **Virtual Destructor** | ✓ Required | ✗ Not needed |
| **Derived Types** | FbsSceneData, etc. | None |
| **Pointer Passing** | `const SceneData*` | `const SceneData&` |
| **Dynamic Cast** | Required | Not needed |
| **Const Cast** | Required | Not needed |
| **Ownership** | Unclear (pointer) | Clear (reference) |
| **Performance** | Virtual call overhead | Direct access |
| **Type Safety** | Runtime checking | Compile-time |

## Conclusion

**Your observation is absolutely correct!** After implementing the Importer/Exporter pattern:

1. ✅ **SceneData no longer needs polymorphism** (no virtual destructor)
2. ✅ **No need for pointer passing** (use const reference instead)
3. ✅ **FbsSceneData can be deleted** (heavy data moves to Importer)
4. ✅ **Simpler, more efficient code** (no virtual calls, better optimization)
5. ✅ **Clearer semantics** (reference = non-owning, non-null access)

This is an excellent insight and further validates the benefits of the Importer/Exporter pattern!

---

**Document Version**: 1.0  
**Date**: 2026-01-06  
**Addresses**: Comment #3714596647 from @walliscode  
**Key Insight**: SceneData becomes a simple data struct (no polymorphism needed) with the Importer/Exporter pattern
