# AssetManager Migration Guide

This document provides a detailed, step-by-step guide for migrating the AssetManager to the proposed architecture, including code examples and specific migration points.

**Date:** 2025-12-24  
**Related:** ASSET_MANAGER_ANALYSIS.md, ASSET_MANAGER_WORKFLOWS.md

---

## Table of Contents

1. [Migration Overview](#migration-overview)
2. [Phase 1: Create Abstractions](#phase-1-create-abstractions)
3. [Phase 2: Implement Loaders](#phase-2-implement-loaders)
4. [Phase 3: Fix Encapsulation](#phase-3-fix-encapsulation)
5. [Phase 4: Refactor AssetManager](#phase-4-refactor-assetmanager)
6. [Phase 5: Complete Scene Loading](#phase-5-complete-scene-loading)
7. [Phase 6: Fix Error Handling](#phase-6-fix-error-handling)
8. [Code Migration Points](#code-migration-points)

---

## Migration Overview

### Goals

1. **Separate Concerns:** Split AssetManager into focused components
2. **Improve Encapsulation:** Remove direct map passing
3. **Enable Testability:** Use interfaces for dependencies
4. **Complete Features:** Implement scene asset loading
5. **Fix Bugs:** Consistent error handling

### Risk Assessment

| Phase | Risk Level | Breaking Changes | Testing Required |
|-------|-----------|------------------|------------------|
| Phase 1 | Low | None | Unit tests for new interfaces |
| Phase 2 | Low | None | Unit tests for loaders |
| Phase 3 | Medium | UIStyleDataProvider API | Update tests, check consumers |
| Phase 4 | High | AssetManager internals | Extensive testing |
| Phase 5 | Medium | Scene loading behavior | Scene transition tests |
| Phase 6 | Medium | GetDefaultUIStyle API | Update all callers |

### Estimated Effort

- **Phase 1:** 4-6 hours (design + implementation)
- **Phase 2:** 6-8 hours (two loaders + tests)
- **Phase 3:** 4-6 hours (refactor + update tests)
- **Phase 4:** 8-10 hours (refactor + extensive testing)
- **Phase 5:** 6-8 hours (implement + define semantics)
- **Phase 6:** 2-4 hours (simple refactor)

**Total:** 30-42 hours (approximately 1-2 weeks for one developer)

---

## Phase 1: Create Abstractions

### Goal
Create new interface abstractions without breaking existing code.

### Files to Create

#### 1. `src/interfaces/IAssetLoader.h`

```cpp
////////////////////////////////////////////////////////////
/// @file
/// @brief Generic asset loader interface template
////////////////////////////////////////////////////////////

#pragma once

#include "FailInfo.h"
#include <expected>
#include <vector>

namespace steamrot {

////////////////////////////////////////////////////////////
/// @brief Generic interface for loading assets of type T
///
/// @tparam T The type of asset to load (e.g., sf::Font, UIStyle)
////////////////////////////////////////////////////////////
template <typename T>
class IAssetLoader {
public:
  virtual ~IAssetLoader() = default;

  ////////////////////////////////////////////////////////////
  /// @brief Load default assets (common across all scenes)
  ///
  /// @return Vector of loaded assets or error
  ////////////////////////////////////////////////////////////
  virtual std::expected<std::vector<T>, FailInfo> LoadDefaultAssets() = 0;

  ////////////////////////////////////////////////////////////
  /// @brief Load scene-specific assets
  ///
  /// @param scene_type The type of scene to load assets for
  /// @return Vector of loaded assets or error
  ////////////////////////////////////////////////////////////
  virtual std::expected<std::vector<T>, FailInfo>
  LoadSceneAssets(SceneType scene_type) = 0;
};

} // namespace steamrot
```

#### 2. `src/interfaces/IFontLoader.h`

```cpp
////////////////////////////////////////////////////////////
/// @file
/// @brief Font loader interface
////////////////////////////////////////////////////////////

#pragma once

#include "IAssetLoader.h"
#include <SFML/Graphics/Font.hpp>
#include <memory>

namespace steamrot {

////////////////////////////////////////////////////////////
/// @brief Interface for loading font assets
////////////////////////////////////////////////////////////
class IFontLoader : public IAssetLoader<std::shared_ptr<sf::Font>> {
public:
  virtual ~IFontLoader() = default;

  ////////////////////////////////////////////////////////////
  /// @brief Load default fonts
  ///
  /// @return Vector of loaded fonts or error
  ////////////////////////////////////////////////////////////
  std::expected<std::vector<std::shared_ptr<sf::Font>>, FailInfo>
  LoadDefaultAssets() override = 0;

  ////////////////////////////////////////////////////////////
  /// @brief Load scene-specific fonts
  ///
  /// @param scene_type The type of scene to load fonts for
  /// @return Vector of loaded fonts or error
  ////////////////////////////////////////////////////////////
  std::expected<std::vector<std::shared_ptr<sf::Font>>, FailInfo>
  LoadSceneAssets(SceneType scene_type) override = 0;
};

} // namespace steamrot
```

#### 3. `src/interfaces/IStyleLoader.h`

```cpp
////////////////////////////////////////////////////////////
/// @file
/// @brief UI Style loader interface
////////////////////////////////////////////////////////////

#pragma once

#include "IAssetLoader.h"
#include "IFontProvider.h"
#include "UIStyle.h"

namespace steamrot {

////////////////////////////////////////////////////////////
/// @brief Interface for loading UI style assets
////////////////////////////////////////////////////////////
class IStyleLoader : public IAssetLoader<UIStyle> {
public:
  virtual ~IStyleLoader() = default;

  ////////////////////////////////////////////////////////////
  /// @brief Load default UI styles
  ///
  /// @param font_provider Provider for font resources
  /// @return Vector of loaded styles or error
  ////////////////////////////////////////////////////////////
  virtual std::expected<std::vector<UIStyle>, FailInfo>
  LoadDefaultAssets(const IFontProvider &font_provider) = 0;

  ////////////////////////////////////////////////////////////
  /// @brief Load scene-specific UI styles
  ///
  /// @param scene_type The type of scene to load styles for
  /// @param font_provider Provider for font resources
  /// @return Vector of loaded styles or error
  ////////////////////////////////////////////////////////////
  virtual std::expected<std::vector<UIStyle>, FailInfo>
  LoadSceneAssets(SceneType scene_type,
                  const IFontProvider &font_provider) = 0;
};

} // namespace steamrot
```

#### 4. `src/assets/AssetRegistry.h`

```cpp
////////////////////////////////////////////////////////////
/// @file
/// @brief Asset storage registry
////////////////////////////////////////////////////////////

#pragma once

#include "FailInfo.h"
#include "UIStyle.h"
#include <SFML/Graphics/Font.hpp>
#include <expected>
#include <memory>
#include <string>
#include <unordered_map>

namespace steamrot {

////////////////////////////////////////////////////////////
/// @brief Central registry for storing and accessing game assets
///
/// Pure storage class with no loading logic. Assets are added
/// via explicit methods and retrieved by name.
////////////////////////////////////////////////////////////
class AssetRegistry {
private:
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> m_fonts;
  std::unordered_map<std::string, UIStyle> m_ui_styles;

public:
  ////////////////////////////////////////////////////////////
  /// @brief Add a font to the registry
  ///
  /// @param name Name of the font
  /// @param font Font to store
  /// @return Success or error if name already exists
  ////////////////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  AddFont(const std::string &name, std::shared_ptr<const sf::Font> font);

  ////////////////////////////////////////////////////////////
  /// @brief Get a font by name
  ///
  /// @param name Name of the font
  /// @return Font pointer or error if not found
  ////////////////////////////////////////////////////////////
  std::expected<std::shared_ptr<const sf::Font>, FailInfo>
  GetFont(const std::string &name) const;

  ////////////////////////////////////////////////////////////
  /// @brief Add a UI style to the registry
  ///
  /// @param style UIStyle to store (uses style.name as key)
  /// @return Success or error if name already exists
  ////////////////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> AddUIStyle(const UIStyle &style);

  ////////////////////////////////////////////////////////////
  /// @brief Get a UI style by name
  ///
  /// @param name Name of the style
  /// @return Style reference or error if not found
  ////////////////////////////////////////////////////////////
  std::expected<const UIStyle &, FailInfo>
  GetUIStyle(const std::string &name) const;

  ////////////////////////////////////////////////////////////
  /// @brief Get all fonts
  ////////////////////////////////////////////////////////////
  const std::unordered_map<std::string, std::shared_ptr<const sf::Font>> &
  GetAllFonts() const;

  ////////////////////////////////////////////////////////////
  /// @brief Get all UI styles
  ////////////////////////////////////////////////////////////
  const std::unordered_map<std::string, UIStyle> &GetAllUIStyles() const;

  ////////////////////////////////////////////////////////////
  /// @brief Clear all fonts
  ////////////////////////////////////////////////////////////
  void ClearFonts();

  ////////////////////////////////////////////////////////////
  /// @brief Clear all UI styles
  ////////////////////////////////////////////////////////////
  void ClearUIStyles();
};

} // namespace steamrot
```

#### 5. `src/assets/AssetRegistry.cpp`

```cpp
////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of AssetRegistry
////////////////////////////////////////////////////////////

#include "AssetRegistry.h"
#include <format>

namespace steamrot {

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
AssetRegistry::AddFont(const std::string &name,
                       std::shared_ptr<const sf::Font> font) {
  auto [it, inserted] = m_fonts.insert({name, font});
  if (!inserted) {
    return std::unexpected(
        FailInfo{FailMode::NotAddedToMap,
                 std::format("Font already exists: {}", name)});
  }
  return std::monostate{};
}

////////////////////////////////////////////////////////////
std::expected<std::shared_ptr<const sf::Font>, FailInfo>
AssetRegistry::GetFont(const std::string &name) const {
  auto it = m_fonts.find(name);
  if (it != m_fonts.end()) {
    return it->second;
  }
  return std::unexpected(
      FailInfo{FailMode::FileNotFound, std::format("Font not found: {}", name)});
}

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
AssetRegistry::AddUIStyle(const UIStyle &style) {
  auto [it, inserted] = m_ui_styles.insert({style.name, style});
  if (!inserted) {
    return std::unexpected(
        FailInfo{FailMode::NotAddedToMap,
                 std::format("UIStyle already exists: {}", style.name)});
  }
  return std::monostate{};
}

////////////////////////////////////////////////////////////
std::expected<const UIStyle &, FailInfo>
AssetRegistry::GetUIStyle(const std::string &name) const {
  auto it = m_ui_styles.find(name);
  if (it != m_ui_styles.end()) {
    return it->second;
  }
  return std::unexpected(
      FailInfo{FailMode::FileNotFound,
               std::format("UIStyle not found: {}", name)});
}

////////////////////////////////////////////////////////////
const std::unordered_map<std::string, std::shared_ptr<const sf::Font>> &
AssetRegistry::GetAllFonts() const {
  return m_fonts;
}

////////////////////////////////////////////////////////////
const std::unordered_map<std::string, UIStyle> &
AssetRegistry::GetAllUIStyles() const {
  return m_ui_styles;
}

////////////////////////////////////////////////////////////
void AssetRegistry::ClearFonts() { m_fonts.clear(); }

////////////////////////////////////////////////////////////
void AssetRegistry::ClearUIStyles() { m_ui_styles.clear(); }

} // namespace steamrot
```

### Testing Phase 1

Create tests for AssetRegistry:

```cpp
TEST_CASE("AssetRegistry stores and retrieves fonts", "[unit][AssetRegistry]") {
  steamrot::AssetRegistry registry;
  
  auto font = std::make_shared<sf::Font>();
  auto add_result = registry.AddFont("test_font", font);
  REQUIRE(add_result.has_value());
  
  auto get_result = registry.GetFont("test_font");
  REQUIRE(get_result.has_value());
  REQUIRE(get_result.value() == font);
}

TEST_CASE("AssetRegistry prevents duplicate fonts", "[unit][AssetRegistry]") {
  steamrot::AssetRegistry registry;
  
  auto font = std::make_shared<sf::Font>();
  registry.AddFont("test_font", font);
  
  auto duplicate_result = registry.AddFont("test_font", font);
  REQUIRE(!duplicate_result.has_value());
  REQUIRE(duplicate_result.error().mode == steamrot::FailMode::NotAddedToMap);
}
```

---

## Phase 2: Implement Loaders

### Goal
Create concrete implementations of loader interfaces.

### Files to Create

#### 1. `src/loaders/FontLoader.h`

```cpp
////////////////////////////////////////////////////////////
/// @file
/// @brief Concrete font loader implementation
////////////////////////////////////////////////////////////

#pragma once

#include "IFontLoader.h"
#include "IAssetDataProvider.h"

namespace steamrot {

////////////////////////////////////////////////////////////
/// @brief Loads fonts from filesystem
////////////////////////////////////////////////////////////
class FontLoader : public IFontLoader {
private:
  IAssetDataProvider &m_asset_data_provider;

  ////////////////////////////////////////////////////////////
  /// @brief Load a single font from filesystem
  ///
  /// @param font_name Name of the font (without extension)
  /// @return Loaded font or error
  ////////////////////////////////////////////////////////////
  std::expected<std::shared_ptr<sf::Font>, FailInfo>
  LoadFont(const std::string &font_name) const;

public:
  FontLoader(IAssetDataProvider &asset_data_provider);

  std::expected<std::vector<std::shared_ptr<sf::Font>>, FailInfo>
  LoadDefaultAssets() override;

  std::expected<std::vector<std::shared_ptr<sf::Font>>, FailInfo>
  LoadSceneAssets(SceneType scene_type) override;
};

} // namespace steamrot
```

#### 2. `src/loaders/FontLoader.cpp`

```cpp
////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of FontLoader
////////////////////////////////////////////////////////////

#include "FontLoader.h"
#include "paths.h"
#include <filesystem>
#include <format>

namespace steamrot {

////////////////////////////////////////////////////////////
FontLoader::FontLoader(IAssetDataProvider &asset_data_provider)
    : m_asset_data_provider(asset_data_provider) {}

////////////////////////////////////////////////////////////
std::expected<std::shared_ptr<sf::Font>, FailInfo>
FontLoader::LoadFont(const std::string &font_name) const {
  // Get font directory
  std::filesystem::path font_dir = paths::GetFontsDirectory();

  // Generate full font file name
  std::string font_file_name = font_name + ".ttf";
  std::filesystem::path font_path = font_dir / font_file_name;

  // Check if file exists
  if (!std::filesystem::exists(font_path)) {
    return std::unexpected(
        FailInfo{FailMode::FileNotFound,
                 std::format("Font file not found: {}", font_path.string())});
  }

  // Create and load font
  auto font = std::make_shared<sf::Font>();
  if (!font->openFromFile(font_path)) {
    return std::unexpected(FailInfo{
        FailMode::FileNotFound,
        std::format("Failed to load font from file: {}", font_path.string())});
  }

  // Configure font
  font->setSmooth(false);

  return font;
}

////////////////////////////////////////////////////////////
std::expected<std::vector<std::shared_ptr<sf::Font>>, FailInfo>
FontLoader::LoadDefaultAssets() {
  // Get asset configuration
  auto asset_data_result = m_asset_data_provider.LoadAssetData();
  if (!asset_data_result.has_value()) {
    return std::unexpected(asset_data_result.error());
  }

  AssetData &asset_data = asset_data_result.value();

  // Load each font
  std::vector<std::shared_ptr<sf::Font>> fonts;
  for (const auto &font_data : asset_data.fonts) {
    auto load_result = LoadFont(font_data.name);
    if (!load_result.has_value()) {
      return std::unexpected(load_result.error());
    }
    fonts.push_back(load_result.value());
  }

  return fonts;
}

////////////////////////////////////////////////////////////
std::expected<std::vector<std::shared_ptr<sf::Font>>, FailInfo>
FontLoader::LoadSceneAssets(SceneType scene_type) {
  // Get scene asset configuration
  auto asset_data_result = m_asset_data_provider.LoadSceneAssetData(scene_type);
  if (!asset_data_result.has_value()) {
    return std::unexpected(asset_data_result.error());
  }

  AssetData &asset_data = asset_data_result.value();

  // Load each scene-specific font
  std::vector<std::shared_ptr<sf::Font>> fonts;
  for (const auto &font_data : asset_data.fonts) {
    auto load_result = LoadFont(font_data.name);
    if (!load_result.has_value()) {
      return std::unexpected(load_result.error());
    }
    fonts.push_back(load_result.value());
  }

  return fonts;
}

} // namespace steamrot
```

#### 3. `src/loaders/FlatbuffersStyleLoader.h`

Similar structure for style loader...

### Update DataAccessFactory

Add methods to `src/data_providers/DataAccessFactory.h`:

```cpp
////////////////////////////////////////////////////////////
/// @brief Returns a raw pointer to the Font Loader
////////////////////////////////////////////////////////////
std::expected<IFontLoader *, FailInfo> GetFontLoader();

////////////////////////////////////////////////////////////
/// @brief Returns a raw pointer to the Style Loader
////////////////////////////////////////////////////////////
std::expected<IStyleLoader *, FailInfo> GetStyleLoader();
```

Add members:

```cpp
private:
  std::unique_ptr<IFontLoader> m_font_loader{nullptr};
  std::unique_ptr<IStyleLoader> m_style_loader{nullptr};
```

---

## Phase 3: Fix Encapsulation

### Goal
Remove direct map passing from UIStyleDataProvider.

### Migration Point 1: Update IUIStyleDataProvider

**File:** `src/interfaces/IUIStyleDataProvider.h`

**Before:**
```cpp
class IUIStyleDataProvider {
protected:
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> &fonts_map;

public:
  IUIStyleDataProvider(
      std::unordered_map<std::string, std::shared_ptr<const sf::Font>>
          &fonts_map)
      : fonts_map(fonts_map) {};
```

**After:**
```cpp
class IUIStyleDataProvider {
protected:
  const IFontProvider &m_font_provider;

public:
  IUIStyleDataProvider(const IFontProvider &font_provider)
      : m_font_provider(font_provider) {}
```

### Migration Point 2: Update FlatbuffersUIStyleDataProvider

**File:** `src/data_providers/FlatbuffersUIStyleDataProvider.h`

**Before:**
```cpp
FlatbuffersUIStyleDataProvider(
    std::unordered_map<std::string, std::shared_ptr<const sf::Font>> &fonts_map);
```

**After:**
```cpp
FlatbuffersUIStyleDataProvider(const IFontProvider &font_provider);
```

**File:** `src/data_providers/FlatbuffersUIStyleDataProvider.cpp`

Replace all direct map access with interface calls:

**Before:**
```cpp
auto it = fonts_map.find(button_fb->font()->str());
if (it == fonts_map.end())
  return std::unexpected(/* error */);
button_style.font = it->second;
```

**After:**
```cpp
auto font_result = m_font_provider.GetFont(button_fb->font()->str());
if (!font_result.has_value())
  return std::unexpected(font_result.error());
button_style.font = font_result.value();
```

### Migration Point 3: Update AssetManager

**File:** `src/assets/AssetManager.cpp`

**Before:**
```cpp
FlatbuffersUIStyleDataProvider ui_style_provider(m_fonts);
```

**After:**
```cpp
FlatbuffersUIStyleDataProvider ui_style_provider(*this);  // AssetManager is IFontProvider
```

---

## Phase 4: Refactor AssetManager

### Goal
Use new loaders and registry in AssetManager.

### Migration Point 4: Add AssetRegistry Member

**File:** `src/assets/AssetManager.h`

Add includes and member:
```cpp
#include "AssetRegistry.h"

private:
  AssetRegistry m_asset_registry;
```

### Migration Point 5: Refactor LoadDefaultAssets

**File:** `src/assets/AssetManager.cpp`

**Before:**
```cpp
std::expected<std::monostate, FailInfo> AssetManager::LoadDefaultAssets() {
  // ... lots of logic ...
  for (const auto &font_data : asset_data.fonts) {
    auto add_font_result = AddFont(font_data.name);
    // ...
  }
  // ...
}
```

**After:**
```cpp
std::expected<std::monostate, FailInfo> AssetManager::LoadDefaultAssets() {
  // Load fonts via loader
  auto font_loader_result = m_data_access_factory.GetFontLoader();
  if (!font_loader_result.has_value())
    return std::unexpected(font_loader_result.error());

  auto fonts_result = font_loader_result.value()->LoadDefaultAssets();
  if (!fonts_result.has_value())
    return std::unexpected(fonts_result.error());

  // Store fonts in registry
  for (const auto &font : fonts_result.value()) {
    // Extract name from font (need to track names)
    auto add_result = m_asset_registry.AddFont(name, font);
    if (!add_result.has_value())
      return std::unexpected(add_result.error());
  }

  // Load styles via loader
  auto style_loader_result = m_data_access_factory.GetStyleLoader();
  if (!style_loader_result.has_value())
    return std::unexpected(style_loader_result.error());

  auto styles_result = style_loader_result.value()->LoadDefaultAssets(*this);
  if (!styles_result.has_value())
    return std::unexpected(styles_result.error());

  // Store styles in registry
  for (const auto &style : styles_result.value()) {
    auto add_result = m_asset_registry.AddUIStyle(style);
    if (!add_result.has_value())
      return std::unexpected(add_result.error());
  }

  return std::monostate{};
}
```

### Migration Point 6: Delegate GetFont to Registry

**Before:**
```cpp
std::expected<std::shared_ptr<const sf::Font>, FailInfo>
AssetManager::GetFont(const std::string &font_name) const {
  auto it = m_fonts.find(font_name);
  if (it != m_fonts.end()) {
    return it->second;
  }
  return std::unexpected(/* error */);
}
```

**After:**
```cpp
std::expected<std::shared_ptr<const sf::Font>, FailInfo>
AssetManager::GetFont(const std::string &font_name) const {
  return m_asset_registry.GetFont(font_name);
}
```

---

## Phase 5: Complete Scene Loading

### Goal
Implement actual scene asset loading functionality.

### Migration Point 7: Implement LoadSceneAssets

**File:** `src/assets/AssetManager.cpp`

**Before:**
```cpp
std::expected<std::monostate, FailInfo>
AssetManager::LoadSceneAssets(const SceneType &scene_type) {
  auto asset_data_result = asset_provider.LoadSceneAssetData(scene_type);
  if (!asset_data_result.has_value())
    return std::unexpected(asset_data_result.error());

  // Scene assets are loaded on demand, return success
  return std::monostate();  // Does nothing!
}
```

**After:**
```cpp
std::expected<std::monostate, FailInfo>
AssetManager::LoadSceneAssets(const SceneType &scene_type) {
  // Load scene-specific fonts (if any)
  auto font_loader_result = m_data_access_factory.GetFontLoader();
  if (!font_loader_result.has_value())
    return std::unexpected(font_loader_result.error());

  auto fonts_result = font_loader_result.value()->LoadSceneAssets(scene_type);
  if (!fonts_result.has_value())
    return std::unexpected(fonts_result.error());

  // Store/merge scene fonts with defaults
  for (const auto &font : fonts_result.value()) {
    // Merge strategy: Replace if exists, add if new
    auto add_result = m_asset_registry.AddFont(name, font);
    // Note: May need AddOrReplace method
  }

  // Load scene-specific styles (if any)
  auto style_loader_result = m_data_access_factory.GetStyleLoader();
  if (!style_loader_result.has_value())
    return std::unexpected(style_loader_result.error());

  auto styles_result = 
      style_loader_result.value()->LoadSceneAssets(scene_type, *this);
  if (!styles_result.has_value())
    return std::unexpected(styles_result.error());

  // Store/merge scene styles with defaults
  for (const auto &style : styles_result.value()) {
    auto add_result = m_asset_registry.AddUIStyle(style);
    // Note: May need AddOrReplace method
  }

  return std::monostate{};
}
```

---

## Phase 6: Fix Error Handling

### Goal
Make error handling consistent across all methods.

### Migration Point 8: Update GetDefaultUIStyle

**File:** `src/assets/AssetManager.h`

**Before:**
```cpp
const UIStyle &GetDefaultUIStyle() const;
```

**After:**
```cpp
std::expected<const UIStyle &, FailInfo> GetDefaultUIStyle() const;
```

**File:** `src/assets/AssetManager.cpp`

**Before:**
```cpp
const UIStyle &AssetManager::GetDefaultUIStyle() const {
  auto it = m_ui_styles.find("default");
  if (it != m_ui_styles.end()) {
    return it->second;
  } else {
    throw std::runtime_error("Default UIStyle not found");
  }
}
```

**After:**
```cpp
std::expected<const UIStyle &, FailInfo>
AssetManager::GetDefaultUIStyle() const {
  return m_asset_registry.GetUIStyle("default");
}
```

### Migration Point 9: Update All Callers

Find all usages of `GetDefaultUIStyle()`:

```bash
grep -r "GetDefaultUIStyle" --include="*.cpp" --include="*.h"
```

Update from:
```cpp
const UIStyle &style = asset_manager.GetDefaultUIStyle();
```

To:
```cpp
auto style_result = asset_manager.GetDefaultUIStyle();
if (!style_result.has_value()) {
  // Handle error
}
const UIStyle &style = style_result.value();
```

---

## Code Migration Points

### Summary of All Migration Points

| Point | File | Type | Risk |
|-------|------|------|------|
| MP1 | IUIStyleDataProvider.h | Interface change | Medium |
| MP2 | FlatbuffersUIStyleDataProvider.h/cpp | Implementation change | Medium |
| MP3 | AssetManager.cpp | Call site change | Low |
| MP4 | AssetManager.h | Add member | Low |
| MP5 | AssetManager.cpp | Refactor method | High |
| MP6 | AssetManager.cpp | Delegate to registry | Low |
| MP7 | AssetManager.cpp | Implement feature | Medium |
| MP8 | AssetManager.h | Change signature | Medium |
| MP9 | All callers | Update error handling | Medium |

### Testing Strategy for Each Point

**MP1-3 (Phase 3):**
- Unit test IUIStyleDataProvider with mock IFontProvider
- Integration test with AssetManager

**MP4-6 (Phase 4):**
- Unit test AssetRegistry
- Integration test AssetManager with loaders
- Regression test existing functionality

**MP7 (Phase 5):**
- Unit test scene asset loading
- Integration test scene transitions

**MP8-9 (Phase 6):**
- Update all unit tests
- Check all call sites compile
- Integration test error paths

---

## Rollback Strategy

### If Issues Arise

**Phase 1-2:** Simply don't use new code (no changes to existing)  
**Phase 3:** Revert 3 files (IUIStyleDataProvider changes)  
**Phase 4:** Revert AssetManager changes (most risky)  
**Phase 5:** Disable scene asset loading (feature flag)  
**Phase 6:** Revert error handling change

### Feature Flags

Consider adding feature flags:
```cpp
#define USE_ASSET_LOADERS 1  // Enable Phase 4 changes
#define USE_SCENE_ASSETS 1   // Enable Phase 5 changes
```

---

## Verification Checklist

After each phase:

- [ ] All unit tests pass
- [ ] All integration tests pass
- [ ] No compiler warnings
- [ ] Memory leaks checked (valgrind)
- [ ] Code review completed
- [ ] Documentation updated

After full migration:

- [ ] All assets load correctly
- [ ] Scene transitions work
- [ ] UI styles render properly
- [ ] Fonts display correctly
- [ ] Performance is acceptable
- [ ] Memory usage is reasonable

---

## Document Metadata

**Created:** 2025-12-24  
**Author:** GitHub Copilot Agent  
**Purpose:** Detailed migration guide for AssetManager refactoring  
**Status:** Complete  
**Review:** Pending
