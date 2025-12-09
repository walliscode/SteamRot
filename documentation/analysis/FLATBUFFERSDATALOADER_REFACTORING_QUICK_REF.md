# FlatbuffersDataLoader Refactoring - Quick Reference

**Date**: December 2025  
**Status**: Ready for Implementation  
**Quick Answer**: Create 3 missing providers to complete the pattern

---

## TL;DR

**Problem**: FlatbuffersDataLoader becoming god object (13+ methods, 8 domains)

**Solution**: Continue existing provider pattern (75% already done!)

**Action**: Create 3 missing providers:
1. `IContextDataProvider` (HIGH priority)
2. `IUIStyleProvider` (MEDIUM priority)
3. `ILogicDataProvider` (MEDIUM priority)

**Timeline**: 3-4 weeks

---

## Current State

### What's Already Done ✅

```cpp
// These providers already exist and wrap FlatbuffersDataLoader:
✅ IEngineDataProvider          → FlatbuffersEngineDataProvider
✅ ISceneDataProvider           → FlatbuffersSceneDataProvider
✅ IAssetDataProvider           → FlatbuffersAssetDataProvider
✅ IFragmentDataProvider        → FlatbuffersFragmentDataProvider
✅ ISceneManagerDataProvider    → FlatbuffersSceneManagerDataProvider
✅ IUserPreferencesProvider     → FlatbuffersUserPreferencesProvider
✅ ISaveDataProvider            → FlatbuffersSaveDataProvider
```

**Coverage**: ~75% of FlatbuffersDataLoader functionality

### What's Missing ❌

```cpp
// These need to be created:
❌ IContextDataProvider         → (wraps ProvideContextData)
❌ IUIStyleProvider             → (wraps ProvideUIStylesData)
❌ ILogicDataProvider           → (wraps ProvideLogicCollectionData)
```

---

## Provider Pattern Template

### Step 1: Define Interface

```cpp
// src/data_providers/IXDataProvider.h

namespace steamrot {

/////////////////////////////////////////////////
/// @struct XData
/// @brief Native C++ struct for X configuration.
/////////////////////////////////////////////////
struct XData {
  // Native C++ members (no FlatBuffers types!)
  std::string name;
  uint32_t value{0};
};

/////////////////////////////////////////////////
/// @class IXDataProvider
/// @brief Interface for loading X data.
/////////////////////////////////////////////////
class IXDataProvider {
public:
  virtual ~IXDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Load X data.
  ///
  /// @return X data or failure information
  /////////////////////////////////////////////////
  virtual std::expected<XData, FailInfo>
  LoadXData() const = 0;
};

} // namespace steamrot
```

### Step 2: Implement FlatBuffers Version

```cpp
// src/data_providers/FlatbuffersXDataProvider.h

#pragma once

#include "IXDataProvider.h"
#include "FlatbuffersDataLoader.h"

namespace steamrot {

class FlatbuffersXDataProvider : public IXDataProvider {
private:
  FlatbuffersDataLoader m_loader;

public:
  FlatbuffersXDataProvider() = default;

  std::expected<XData, FailInfo>
  LoadXData() const override;
};

} // namespace steamrot
```

```cpp
// src/data_providers/FlatbuffersXDataProvider.cpp

#include "FlatbuffersXDataProvider.h"

namespace steamrot {

std::expected<XData, FailInfo>
FlatbuffersXDataProvider::LoadXData() const {
  // 1. Call FlatbuffersDataLoader method
  auto fb_result = m_loader.ProvideXDataFbs();
  if (!fb_result.has_value()) {
    return std::unexpected(fb_result.error());
  }

  const auto *fb_data = fb_result.value();

  // 2. Convert FlatBuffers → native struct
  XData native_data;
  if (fb_data->name()) {
    native_data.name = fb_data->name()->str();
  }
  native_data.value = fb_data->value();

  // 3. Return native struct
  return native_data;
}

} // namespace steamrot
```

### Step 3: Update Consumer

```cpp
// Before: Direct usage of FlatbuffersDataLoader
void OldCode() {
  FlatbuffersDataLoader loader;
  auto fb_result = loader.ProvideXDataFbs();  // Returns FlatBuffers type!
  const XDataFbs *fb_data = fb_result.value();
  // Work with FlatBuffers type...
}

// After: Use provider interface
void NewCode() {
  FlatbuffersXDataProvider provider;
  auto result = provider.LoadXData();  // Returns native struct!
  const XData& data = result.value();
  // Work with native C++ struct!
}

// Even better: Depend on interface
void BestCode(const IXDataProvider& provider) {
  auto result = provider.LoadXData();
  // Can work with ANY implementation (FlatBuffers, JSON, XML, etc.)
}
```

---

## Implementation Checklist

### Provider 1: IContextDataProvider (HIGH Priority)

**Files to Create**:
- [ ] `src/data_providers/IContextDataProvider.h`
- [ ] `src/data_providers/FlatbuffersContextDataProvider.h`
- [ ] `src/data_providers/FlatbuffersContextDataProvider.cpp`

**Method to Wrap**:
```cpp
std::expected<const ContextData *, FailInfo> ProvideContextData() const;
```

**Native Struct**:
```cpp
struct GameContextData {
  uint32_t window_width{800};
  uint32_t window_height{600};
  std::string window_title{"SteamRot"};
  uint32_t framerate_limit{60};
  std::map<SceneType, SceneContextData> scene_contexts;
};

struct SceneContextData {
  uint32_t entity_pool_size{100};
  uint32_t render_texture_width{800};
  uint32_t render_texture_height{600};
};
```

**Interface Method**:
```cpp
virtual std::expected<GameContextData, FailInfo>
LoadContextData() const = 0;

virtual std::expected<SceneContextData, FailInfo>
LoadSceneContext(SceneType scene_type) const = 0;
```

**Consumers to Update**:
- Context configuration system
- Scene setup code

**Test Cases**:
- [ ] Test loading valid context data
- [ ] Test loading scene-specific context
- [ ] Test error handling (missing file)
- [ ] Test FlatBuffers → native conversion

---

### Provider 2: IUIStyleProvider (MEDIUM Priority)

**Files to Create**:
- [ ] `src/data_providers/IUIStyleProvider.h`
- [ ] `src/data_providers/FlatbuffersUIStyleProvider.h`
- [ ] `src/data_providers/FlatbuffersUIStyleProvider.cpp`

**Method to Wrap**:
```cpp
std::expected<const UIStyleData *, FailInfo>
ProvideUIStylesData(const std::string &style_name) const;
```

**Native Struct**:
```cpp
struct UIStyleConfig {
  std::string style_name;
  // Font properties
  std::string font_family;
  uint32_t font_size{16};
  // Colors
  sf::Color text_color{255, 255, 255};
  sf::Color background_color{0, 0, 0};
  // Spacing
  float padding{10.0f};
  float margin{5.0f};
};
```

**Interface Method**:
```cpp
virtual std::expected<UIStyleConfig, FailInfo>
LoadUIStyle(const std::string &style_name) const = 0;
```

**Consumers to Update**:
- `StylesConfigurator` (currently uses FlatbuffersDataLoader directly)

**Test Cases**:
- [ ] Test loading style by name
- [ ] Test style not found
- [ ] Test conversion to native struct

---

### Provider 3: ILogicDataProvider (MEDIUM Priority)

**Files to Create**:
- [ ] `src/data_providers/ILogicDataProvider.h`
- [ ] `src/data_providers/FlatbuffersLogicDataProvider.h`
- [ ] `src/data_providers/FlatbuffersLogicDataProvider.cpp`

**Method to Wrap**:
```cpp
std::expected<const LogicCollectionData *, FailInfo>
ProvideLogicCollectionData(const SceneType scene_type) const;
```

**Native Struct**:
```cpp
struct LogicConfiguration {
  SceneType scene_type{SceneType::SceneType_UNKNOWN};
  std::vector<LogicSystemConfig> systems;
};

struct LogicSystemConfig {
  std::string system_name;
  bool enabled{true};
  int32_t execution_order{0};
};
```

**Interface Method**:
```cpp
virtual std::expected<LogicConfiguration, FailInfo>
LoadLogicConfiguration(SceneType scene_type) const = 0;
```

**Consumers to Update**:
- Logic factory system
- Scene logic setup

**Test Cases**:
- [ ] Test loading logic config for scene
- [ ] Test scene type not found
- [ ] Test conversion to native struct

---

## Timeline

| Week | Task | Deliverable |
|------|------|-------------|
| 1 | Create IContextDataProvider | Interface + Implementation + Tests |
| 2 | Create IUIStyleProvider | Interface + Implementation + Tests |
| 2 | Create ILogicDataProvider | Interface + Implementation + Tests |
| 3 | Update consumers | Migration complete |
| 3-4 | Deprecate methods | Warnings added |
| 4 | Documentation | Complete |

**Total**: 3-4 weeks

---

## Key Principles

### DO ✅

- **Follow existing pattern** (see `IEngineDataProvider`, `ISaveDataProvider`)
- **Return native C++ structs** (no FlatBuffers types in interfaces)
- **Wrap FlatbuffersDataLoader** (don't duplicate loading logic)
- **Write tests** (unit tests for each provider)
- **Update consumers** (migrate to use providers)
- **Use std::expected** (consistent error handling)

### DON'T ❌

- **Don't expose FlatBuffers types** in interfaces
- **Don't duplicate file loading logic** (use FlatbuffersDataLoader)
- **Don't break existing code** (gradual migration)
- **Don't skip tests** (providers are critical)
- **Don't add to FlatbuffersDataLoader** (use providers instead)

---

## Benefits Summary

### Immediate (Week 1-4)
- ✅ Stop god object growth
- ✅ Better organization
- ✅ Easier testing

### Medium-term (Month 2-3)
- ✅ Easier to add new data types
- ✅ Clear separation of concerns
- ✅ Less coupling

### Long-term (Month 6+)
- ✅ Format independence (can add JSON/XML/Lua)
- ✅ Modding support ready
- ✅ Network loading ready
- ✅ Maintainable codebase

---

## Common Questions

### Q: Why not just keep FlatbuffersDataLoader as-is?
**A**: It will keep growing. In 6 months, it'll have 25+ methods and be unmaintainable.

### Q: Isn't this overengineering?
**A**: You're 75% done already! Just need 3 more providers to complete the pattern.

### Q: Can we do this incrementally?
**A**: Yes! Create one provider at a time, migrate consumers gradually.

### Q: What if we need to add a new data type?
**A**: Create a new provider. Don't add to FlatbuffersDataLoader.

### Q: Do we need to remove FlatbuffersDataLoader?
**A**: No! It stays as an internal utility. Just don't expose it to game code.

---

## Success Metrics

**Definition of Done**:
- [ ] 3 new providers created
- [ ] All consumers migrated
- [ ] Tests passing
- [ ] Methods deprecated in FlatbuffersDataLoader
- [ ] Documentation updated

**Success Indicators**:
- ✅ FlatbuffersDataLoader not used directly in game code
- ✅ All data access goes through provider interfaces
- ✅ New data type added using provider pattern (not FlatbuffersDataLoader)
- ✅ Team adopts pattern for future work

---

## Example: Good vs Bad

### ❌ Bad (Current State)

```cpp
// Game code directly uses FlatbuffersDataLoader
void SetupGame() {
  FlatbuffersDataLoader loader;  // Tight coupling!
  
  auto context = loader.ProvideContextData();  // FlatBuffers type!
  auto styles = loader.ProvideUIStylesData("default");
  auto logic = loader.ProvideLogicCollectionData(scene_type);
  
  // Work with FlatBuffers types...
}
```

### ✅ Good (After Refactoring)

```cpp
// Game code uses provider interfaces
void SetupGame(
    const IContextDataProvider& context_provider,
    const IUIStyleProvider& style_provider,
    const ILogicDataProvider& logic_provider) {
  
  auto context = context_provider.LoadContextData();  // Native struct!
  auto styles = style_provider.LoadUIStyle("default");
  auto logic = logic_provider.LoadLogicConfiguration(scene_type);
  
  // Work with native C++ structs!
  // Can swap implementations (JSON, XML, etc.) without changing code!
}
```

---

## Related Documentation

- **Full Analysis**: `FLATBUFFERSDATALOADER_GOD_OBJECT_ANALYSIS.md`
- **Implementation Checklist**: `FLATBUFFERSDATALOADER_REFACTORING_CHECKLIST.md` (next)
- **Existing Providers**: `src/data_providers/`
- **Good Example**: `ISaveDataProvider.h` / `FlatbuffersSaveDataProvider.h`

---

## Contact / Questions

For questions about this refactoring:
1. Read full analysis: `FLATBUFFERSDATALOADER_GOD_OBJECT_ANALYSIS.md`
2. Check existing providers: `src/data_providers/I*Provider.h`
3. Review pattern: `ISaveDataProvider.h` (best example)
