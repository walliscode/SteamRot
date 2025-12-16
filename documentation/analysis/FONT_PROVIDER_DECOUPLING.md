# Font Provider Interface Decoupling

## Overview

This document describes the architectural improvement made to decouple the UI style configurator from the AssetManager by introducing the IFontProvider interface.

**Date:** 2025-12-16  
**Issue:** Circular dependency between IUIStyleConfigurator and AssetManager  
**Solution:** Introduce IFontProvider interface to invert the dependency  

---

## Problem Statement

### Original Design Issue

The `IUIStyleConfigurator` interface had a direct dependency on `AssetManager`:

```cpp
class IUIStyleConfigurator {
protected:
  const AssetManager &m_asset_manager;  // ← Direct dependency

public:
  IUIStyleConfigurator(const AssetManager &asset_manager)
      : m_asset_manager(asset_manager) {}
};
```

**Problems:**

1. **Circular Dependency Concern:** AssetManager calls the configurator, but the configurator also depends on AssetManager
2. **Tight Coupling:** Configurator cannot work without AssetManager, even if fonts could come from other sources
3. **Poor Testability:** Cannot easily mock font access for testing
4. **Violates Dependency Inversion Principle:** High-level configurator depends on low-level AssetManager

### Usage Pattern

The AssetManager was calling StylesConfigurator like this:

```cpp
// In AssetManager::LoadUIStyles()
StylesConfigurator styles_configurator;
auto ui_styles_map_result =
    styles_configurator.ProvideUIStylesMap(*this, style_names);
    //                                     ^^^^^ Passing itself
```

While the configurator needed AssetManager only to get fonts:

```cpp
// In StylesConfigurator::ConfigureStyle()
auto get_font_result = asset_manager.GetFont(button_fb->font()->str());
//                     ^^^^^^^^^^^^^ Only using GetFont() method
```

---

## Solution: IFontProvider Interface

### Design

Introduce an abstract `IFontProvider` interface that provides font access without coupling to AssetManager:

```cpp
class IFontProvider {
public:
  virtual ~IFontProvider() = default;
  
  virtual std::expected<std::shared_ptr<const sf::Font>, FailInfo>
  GetFont(const std::string &font_name) const = 0;
};
```

### Implementation

**1. AssetManager implements IFontProvider:**

```cpp
class AssetManager : public IFontProvider {
  // ... existing code ...
  
  std::expected<std::shared_ptr<const sf::Font>, FailInfo>
  GetFont(const std::string &font_name) const override;
};
```

**2. IUIStyleConfigurator depends on IFontProvider:**

```cpp
class IUIStyleConfigurator {
protected:
  const IFontProvider &m_font_provider;  // ← Abstract interface

public:
  IUIStyleConfigurator(const IFontProvider &font_provider)
      : m_font_provider(font_provider) {}
};
```

**3. StylesConfigurator uses IFontProvider:**

```cpp
std::expected<UIStyle, FailInfo>
StylesConfigurator::ConfigureStyle(const UIStyleData &style_data,
                                   const IFontProvider &font_provider) {
  // ...
  auto get_font_result = font_provider.GetFont(button_fb->font()->str());
  //                     ^^^^^^^^^^^^^ Uses interface
}
```

---

## Benefits

### 1. Proper Dependency Direction

**Before:**
```
AssetManager ←→ IUIStyleConfigurator
    (circular dependency concern)
```

**After:**
```
AssetManager → IUIStyleConfigurator → IFontProvider
               ↓                      ↑
          (implements)           (depends on)
```

The configurator now depends on an abstraction (IFontProvider), and AssetManager implements that abstraction. This follows the Dependency Inversion Principle.

### 2. Loose Coupling

The configurator no longer knows about AssetManager at all. It only knows about the IFontProvider interface, which could be implemented by:
- AssetManager (current implementation)
- TestFontProvider (for unit testing)
- CachedFontProvider (for performance)
- NetworkFontProvider (for remote fonts)
- Any other font source

### 3. Better Testability

Unit tests can now provide a mock IFontProvider without needing to create a full AssetManager:

```cpp
class MockFontProvider : public IFontProvider {
  std::expected<std::shared_ptr<const sf::Font>, FailInfo>
  GetFont(const std::string &font_name) const override {
    // Return test fonts without file I/O
  }
};

TEST_CASE("StylesConfigurator with mock fonts") {
  MockFontProvider mock_provider;
  StylesConfigurator configurator;
  auto result = configurator.ConfigureStyle(style_data, mock_provider);
  // Test without AssetManager dependency
}
```

### 4. Single Responsibility

- **IFontProvider:** Responsible for providing fonts
- **IUIStyleConfigurator:** Responsible for configuring UI styles
- **AssetManager:** Responsible for managing all game assets (implements IFontProvider)

Each component has a clear, single responsibility.

### 5. Flexibility

Future implementations can easily add alternative font sources:
- Load fonts from a custom format
- Cache fonts in memory
- Load fonts over network
- Use system fonts

All without changing the configurator code.

---

## Files Changed

### New Files (1)

**`src/interfaces/IFontProvider.h`**
- Pure abstract interface for font access
- Single method: `GetFont(const std::string &font_name)`
- Returns `std::expected<std::shared_ptr<const sf::Font>, FailInfo>`

### Modified Files (4)

**1. `src/assets/AssetManager.h`**
- Added: `#include "IFontProvider.h"`
- Changed: `class AssetManager : public IFontProvider`
- Changed: `GetFont()` method now has `override` specifier

**2. `src/configuration/IUIStyleConfigurator.h`**
- Removed: `#include "AssetManager.h"`
- Added: `#include "IFontProvider.h"`
- Changed: `const AssetManager &m_asset_manager` → `const IFontProvider &m_font_provider`
- Changed: Constructor parameter from `AssetManager` to `IFontProvider`

**3. `src/user_interface/StylesConfigurator.h`**
- Removed: `#include "AssetManager.h"`
- Added: `#include "IFontProvider.h"`
- Changed: `ConfigureStyle()` parameter from `AssetManager` to `IFontProvider`
- Changed: `ProvideUIStylesMap()` parameter from `AssetManager` to `IFontProvider`

**4. `src/user_interface/StylesConfigurator.cpp`**
- Added: `#include "IFontProvider.h"`
- Changed: All method signatures updated to use `IFontProvider`
- Changed: All `asset_manager.GetFont()` calls → `font_provider.GetFont()`

---

## Architecture Diagram

### Before

```
┌─────────────────────────────────────┐
│  AssetManager                       │
│  - LoadUIStyles()                   │
│    ├─ Creates StylesConfigurator    │
│    └─ Passes *this                  │
└─────────────────────────────────────┘
              │
              │ passes self
              ▼
┌─────────────────────────────────────┐
│  IUIStyleConfigurator               │
│  - const AssetManager &m_asset_mgr  │ ◄── Circular dependency
│    └─ Uses asset_mgr.GetFont()     │     concern
└─────────────────────────────────────┘
```

### After

```
┌─────────────────────────────────────┐
│  IFontProvider (Interface)          │
│  + GetFont(name) → Font             │
└─────────────────────────────────────┘
              ▲                ▲
              │                │
    implements│                │depends on
              │                │
┌─────────────┴─────┐   ┌─────┴──────────────────────┐
│  AssetManager     │   │  IUIStyleConfigurator      │
│  - LoadUIStyles() │   │  - m_font_provider         │
│  - GetFont()      │   │  - ConfigureStyle()        │
│    (override)     │   │    Uses m_font_provider    │
└───────────────────┘   └────────────────────────────┘
        │                         ▲
        │ creates & calls         │
        └─────────────────────────┘
```

**Key Points:**
- AssetManager depends on IUIStyleConfigurator (calls it)
- IUIStyleConfigurator depends on IFontProvider (uses it)
- AssetManager implements IFontProvider (satisfies dependency)
- No circular dependency: dependencies flow in one direction

---

## Impact on Future Work

### Phase 3 Implementation (FlatbuffersUIStyleConfigurator)

When implementing Phase 3, the FlatBuffers configurator will inherit from IUIStyleConfigurator:

```cpp
class FlatbuffersUIStyleConfigurator : public IUIStyleConfigurator {
public:
  FlatbuffersUIStyleConfigurator(const IFontProvider &font_provider)
      : IUIStyleConfigurator(font_provider) {}
  
  std::expected<UIStyle, FailInfo>
  ConfigureStyle(const std::string &style_name) override {
    // Load FlatBuffers data
    // Use m_font_provider.GetFont() to get fonts
    // Configure UIStyle
  }
};
```

**Usage:**
```cpp
// In AssetManager::LoadUIStyles()
FlatbuffersUIStyleConfigurator configurator(*this);  // AssetManager is IFontProvider
auto result = configurator.ConfigureStyle("default_style");
```

### Testing Benefits

**Unit Test Example:**
```cpp
class TestFontProvider : public IFontProvider {
  std::expected<std::shared_ptr<const sf::Font>, FailInfo>
  GetFont(const std::string &font_name) const override {
    // Return pre-loaded test font
    static auto test_font = std::make_shared<sf::Font>();
    return test_font;
  }
};

TEST_CASE("FlatbuffersUIStyleConfigurator") {
  TestFontProvider font_provider;
  FlatbuffersUIStyleConfigurator configurator(font_provider);
  
  auto result = configurator.ConfigureStyle("test_style");
  REQUIRE(result.has_value());
  // No need for AssetManager, file system, or data files
}
```

---

## Design Principles Applied

### 1. Dependency Inversion Principle (DIP)

**Definition:** High-level modules should not depend on low-level modules. Both should depend on abstractions.

**Application:**
- High-level: IUIStyleConfigurator (configuration logic)
- Low-level: AssetManager (asset management)
- Abstraction: IFontProvider (font access)
- Result: Configurator depends on IFontProvider (abstraction), not AssetManager (concrete)

### 2. Interface Segregation Principle (ISP)

**Definition:** Clients should not be forced to depend on interfaces they don't use.

**Application:**
- StylesConfigurator only needs font access, not all of AssetManager
- IFontProvider provides only font access
- Result: Minimal interface with only what's needed

### 3. Single Responsibility Principle (SRP)

**Definition:** A class should have only one reason to change.

**Application:**
- IFontProvider: Changes only if font access needs change
- IUIStyleConfigurator: Changes only if style configuration needs change
- AssetManager: Changes only if asset management needs change
- Result: Clear separation of concerns

### 4. Open/Closed Principle (OCP)

**Definition:** Software entities should be open for extension, closed for modification.

**Application:**
- New IFontProvider implementations can be added without modifying existing code
- Configurators work with any IFontProvider implementation
- Result: Extensible without modification

---

## Comparison with Other Patterns

### Similar Pattern: IEntityConfigurator

The IUIStyleConfigurator → IFontProvider pattern mirrors the existing IEntityConfigurator pattern:

**IEntityConfigurator Pattern:**
```cpp
class IEntityConfigurator {
protected:
  EventHandler &m_event_handler;  // Depends on abstract EventHandler
  
public:
  IEntityConfigurator(EventHandler &event_handler);
};
```

**IUIStyleConfigurator Pattern (New):**
```cpp
class IUIStyleConfigurator {
protected:
  const IFontProvider &m_font_provider;  // Depends on abstract IFontProvider
  
public:
  IUIStyleConfigurator(const IFontProvider &font_provider);
};
```

**Consistency:** Both configurators depend on interfaces, not concrete implementations.

---

## Backward Compatibility

### No Breaking Changes

The change is **100% backward compatible** for existing code:

**AssetManager usage remains identical:**
```cpp
// Before and After - No change needed
AssetManager asset_manager;
asset_manager.LoadDefaultAssets();
auto font = asset_manager.GetFont("arial");
```

**The only change is internal to StylesConfigurator:**
- AssetManager now passes itself as IFontProvider
- StylesConfigurator receives IFontProvider instead of AssetManager
- Everything else works exactly the same

**No changes required in:**
- Entity configuration
- Scene management
- Game loop
- Asset loading
- Any code that uses AssetManager

---

## Testing Strategy

### Unit Tests (Recommended)

**1. Test IFontProvider interface:**
```cpp
TEST_CASE("IFontProvider contract") {
  // Test that AssetManager implements IFontProvider correctly
}
```

**2. Test with mock provider:**
```cpp
TEST_CASE("StylesConfigurator with mock fonts") {
  MockFontProvider mock_provider;
  // Test configurator without AssetManager dependency
}
```

**3. Test font loading:**
```cpp
TEST_CASE("AssetManager as IFontProvider") {
  AssetManager asset_manager;
  asset_manager.LoadDefaultAssets();
  
  IFontProvider &provider = asset_manager;  // Upcast to interface
  auto font_result = provider.GetFont("arial");
  REQUIRE(font_result.has_value());
}
```

### Integration Tests (Recommended)

**1. Test style configuration with real AssetManager:**
```cpp
TEST_CASE("StylesConfigurator with AssetManager") {
  AssetManager asset_manager;
  asset_manager.LoadDefaultAssets();
  
  StylesConfigurator configurator;
  auto styles = configurator.ProvideUIStylesMap(asset_manager, {"default"});
  REQUIRE(styles.has_value());
}
```

---

## Summary

**Problem:** IUIStyleConfigurator had circular dependency concern with AssetManager

**Solution:** Introduced IFontProvider interface to invert the dependency

**Benefits:**
- ✅ Proper dependency direction (no circular dependencies)
- ✅ Loose coupling (configurator independent of AssetManager)
- ✅ Better testability (can mock font access)
- ✅ Follows SOLID principles (DIP, ISP, SRP, OCP)
- ✅ Consistent with existing patterns (IEntityConfigurator)
- ✅ Zero breaking changes (100% backward compatible)

**Files Changed:**
- 1 new file (IFontProvider.h)
- 4 modified files (AssetManager.h, IUIStyleConfigurator.h, StylesConfigurator.h/cpp)

**Impact:** Improved architecture quality, better testability, more flexible design

---

## Document Metadata

**Created:** 2025-12-16  
**Author:** GitHub Copilot Agent  
**Related Documents:**
- `USER_INTERFACE_DECOUPLING_ANALYSIS.md` - Original decoupling analysis
- `USER_INTERFACE_PHASES_1_2_IMPLEMENTATION.md` - Phases 1 & 2 implementation
- `LAYERING_AND_ARCHITECTURE_ANALYSIS.md` - Three-layer architecture

**Status:** Complete  
**Review:** Pending
