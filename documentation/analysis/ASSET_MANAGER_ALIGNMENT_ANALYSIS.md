# AssetManager Alignment Analysis

## Executive Summary

This document analyzes the current state of the AssetManager class and identifies steps needed to bring it in line with the rest of the SteamRot codebase's architecture patterns. The analysis focuses on two main areas: **implementing abstract interfaces for data loading** and **improving testing infrastructure for data provision**.

**Key Findings:**
- AssetManager partially uses provider patterns but has inconsistent implementation
- Font loading is done directly in AssetManager, bypassing provider abstraction
- IFontProvider interface exists but AssetManager doesn't implement it
- Testing infrastructure exists but lacks mock provider support
- Opportunities exist to align with established patterns (IAssetDataProvider, provider factory)

**Recommended Priority:**
- **High Priority**: Implement IFontProvider interface (enables testing, follows existing pattern)
- **Medium Priority**: Extract font loading logic to dedicated provider
- **Medium Priority**: Improve test coverage with mock providers
- **Low Priority**: Consider consolidating asset loading patterns

---

## Table of Contents

1. [Current State Analysis](#current-state-analysis)
2. [Architecture Patterns in SteamRot](#architecture-patterns-in-steamrot)
3. [Gap Analysis](#gap-analysis)
4. [Proposed Improvements](#proposed-improvements)
5. [Testing Infrastructure](#testing-infrastructure)
6. [Implementation Roadmap](#implementation-roadmap)
7. [Dependencies and Risks](#dependencies-and-risks)

---

## Current State Analysis

### AssetManager Structure

**File: `src/assets/AssetManager.h`**

```cpp
class AssetManager {
private:
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> m_fonts;
  std::unordered_map<std::string, UIStyle> m_ui_styles;
  
  std::expected<std::monostate, FailInfo> AddFont(const std::string &font_name);

public:
  std::expected<std::monostate, FailInfo> LoadDefaultAssets();
  std::expected<std::monostate, FailInfo> LoadSceneAssets(const SceneType &scene_type);
  std::expected<std::monostate, FailInfo> LoadUIStyles();
  
  const std::unordered_map<std::string, std::shared_ptr<const sf::Font>> &GetAllFonts() const;
  const UIStyle &GetDefaultUIStyle() const;
  const std::unordered_map<std::string, UIStyle> &GetAllUIStyles() const;
};
```

### Current Data Flow

**For Asset Configuration Data:**
```
AssetManager::LoadDefaultAssets()
  └─> GetAssetDataProvider()                    // Uses factory
      └─> IAssetDataProvider::LoadAssetData()   // Interface abstraction
          └─> FlatbuffersAssetDataProvider       // Concrete implementation
              └─> FlatbuffersDataLoader          // Data loading
                  └─> File I/O                   // Actual data access
```

**For Font Loading:**
```
AssetManager::LoadDefaultAssets()
  └─> AssetManager::AddFont()                   // Direct implementation
      └─> paths::GetFontsDirectory()            // Direct file system access
          └─> sf::Font::openFromFile()          // Direct SFML call
```

**For UI Styles:**
```
AssetManager::LoadUIStyles()
  └─> FlatbuffersUIStyleDataProvider(m_fonts)   // Direct instantiation
      └─> IUIStyleDataProvider::ProvideUIStyles() // Interface, but no factory
          └─> FlatbuffersDataLoader              // Data loading
              └─> File I/O                       // Actual data access
```

### Problems Identified

1. **Inconsistent Provider Pattern Usage**
   - Asset data: Uses IAssetDataProvider + factory ✅
   - Font loading: Direct implementation, no provider abstraction ❌
   - UI styles: Uses interface but no factory, direct instantiation ❌

2. **IFontProvider Not Implemented**
   - `IFontProvider` interface exists (from FONT_PROVIDER_DECOUPLING analysis)
   - AssetManager should implement it but currently doesn't
   - Missing `GetFont(const std::string&)` method

3. **Font Loading Responsibilities**
   - AssetManager knows about file system paths (via `paths::GetFontsDirectory()`)
   - AssetManager knows about file extensions (hardcoded ".ttf")
   - AssetManager handles SFML font loading directly
   - **Violates Single Responsibility Principle**

4. **Limited Testing Support**
   - No mock providers for font loading
   - Testing requires actual file system and font files
   - Cannot easily test failure scenarios
   - `asset_test_helpers` depends on FlatbuffersDataLoader directly

5. **No Provider Factory for Fonts**
   - Asset data has `GetAssetDataProvider()` factory
   - Engine data has `GetEngineDataProvider()` factory
   - Scene data has `GetSceneDataProvider()` factory
   - **Fonts have no equivalent factory**

---

## Architecture Patterns in SteamRot

### Provider Pattern (Established)

The codebase has established a clear three-layer architecture:

**Layer 1 - Interfaces (`src/interfaces/`):**
```cpp
class IAssetDataProvider {
public:
  virtual std::expected<AssetData, FailInfo> LoadAssetData() const = 0;
  virtual std::expected<AssetData, FailInfo> 
    LoadSceneAssetData(SceneType scene_type) const = 0;
};
```

**Layer 2 - Implementations (`src/data_providers/`):**
```cpp
class FlatbuffersAssetDataProvider : public IAssetDataProvider {
private:
  FlatbuffersDataLoader m_loader;
public:
  std::expected<AssetData, FailInfo> LoadAssetData() const override;
  std::expected<AssetData, FailInfo> 
    LoadSceneAssetData(SceneType scene_type) const override;
};
```

**Layer 3 - Factory (`src/data_providers/provider_factory.h`):**
```cpp
IAssetDataProvider &GetAssetDataProvider();  // Returns static instance
```

**Usage Pattern:**
```cpp
// Consumers use factory to get provider
IAssetDataProvider &provider = GetAssetDataProvider();
auto result = provider.LoadAssetData();
```

### Configurator Pattern (Established)

For converting data to runtime objects:

```cpp
// Interface defines configuration contract
class IUIStyleDataProvider {
public:
  virtual std::expected<std::vector<UIStyle>, FailInfo> ProvideUIStyles() = 0;
};

// Implementation converts FlatBuffers to UIStyle
class FlatbuffersUIStyleDataProvider : public IUIStyleDataProvider {
  std::expected<std::vector<UIStyle>, FailInfo> ProvideUIStyles() override;
};
```

### Font Provider Pattern (Partially Implemented)

From `FONT_PROVIDER_DECOUPLING.md`:

```cpp
// Interface exists
class IFontProvider {
public:
  virtual std::expected<std::shared_ptr<const sf::Font>, FailInfo>
    GetFont(const std::string &font_name) const = 0;
};

// AssetManager SHOULD implement this but currently doesn't
```

**Current Status:**
- ✅ Interface defined in `src/interfaces/IFontProvider.h`
- ✅ IUIStyleConfigurator uses IFontProvider
- ✅ FlatbuffersUIStyleDataProvider uses IFontProvider
- ❌ AssetManager doesn't implement IFontProvider
- ❌ No factory function for IFontProvider
- ❌ No alternative implementations (mock, test, cached)

---

## Gap Analysis

### Gap 1: AssetManager Doesn't Implement IFontProvider

**Current State:**
```cpp
class AssetManager {
  // No IFontProvider inheritance
  const std::unordered_map<std::string, std::shared_ptr<const sf::Font>> &
    GetAllFonts() const;  // Returns entire map
};
```

**Expected State:**
```cpp
class AssetManager : public IFontProvider {
  std::expected<std::shared_ptr<const sf::Font>, FailInfo>
    GetFont(const std::string &font_name) const override;  // Single font lookup
    
  const std::unordered_map<std::string, std::shared_ptr<const sf::Font>> &
    GetAllFonts() const;  // Keep for bulk access
};
```

**Impact:**
- Cannot use AssetManager where IFontProvider is required
- Cannot substitute with mock IFontProvider for testing
- Inconsistent with FONT_PROVIDER_DECOUPLING recommendations

**Effort:** Low (1-2 hours)
**Risk:** Low (additive change)

---

### Gap 2: Font Loading Logic in AssetManager

**Current State:**
```cpp
std::expected<std::monostate, FailInfo>
AssetManager::AddFont(const std::string &font_name) {
  std::filesystem::path font_dir = paths::GetFontsDirectory();  // Direct access
  std::string font_file_name = font_name + ".ttf";              // Hardcoded
  std::filesystem::path font_path = font_dir / font_file_name;
  
  if (!std::filesystem::exists(font_path)) {
    return std::unexpected<FailInfo>({FailMode::FileNotFound, ...});
  }
  
  sf::Font font;
  bool success = font.openFromFile(font_path);                  // Direct SFML
  // ...
}
```

**Problems:**
1. **Mixed Concerns**: AssetManager both manages assets AND loads them from disk
2. **Hard to Test**: Requires actual file system and font files
3. **Inflexible**: Cannot easily add alternative sources (memory, network, cached)
4. **Hardcoded Extensions**: ".ttf" is baked in

**Expected Pattern:**
```cpp
// Interface for font data provision
class IFontDataProvider {
public:
  virtual std::expected<sf::Font, FailInfo>
    LoadFontFromSource(const std::string &font_name) const = 0;
};

// File system implementation
class FileSystemFontDataProvider : public IFontDataProvider {
  std::expected<sf::Font, FailInfo>
    LoadFontFromSource(const std::string &font_name) const override;
};

// AssetManager uses provider
class AssetManager : public IFontProvider {
private:
  IFontDataProvider &m_font_data_provider;
  
  std::expected<std::monostate, FailInfo>
    AddFont(const std::string &font_name) {
      auto font_result = m_font_data_provider.LoadFontFromSource(font_name);
      if (font_result.has_value()) {
        m_fonts[font_name] = std::make_shared<sf::Font>(font_result.value());
      }
      return ...;
    }
};
```

**Benefits:**
- ✅ Testable with mock providers
- ✅ Flexible (can add network, memory, cached sources)
- ✅ Follows Single Responsibility Principle
- ✅ Consistent with IAssetDataProvider pattern

**Effort:** Medium (4-6 hours)
**Risk:** Medium (requires testing all font loading scenarios)

---

### Gap 3: No Factory for Font Provider

**Current State:**
```cpp
// provider_factory.h
IAssetDataProvider &GetAssetDataProvider();          // ✅ Exists
IEngineDataProvider &GetEngineDataProvider();        // ✅ Exists
ISceneDataProvider &GetSceneDataProvider();          // ✅ Exists
// ❌ No GetFontProvider() or GetFontDataProvider()
```

**Expected State:**
```cpp
// provider_factory.h
IFontProvider &GetFontProvider();                    // Returns AssetManager singleton
IFontDataProvider &GetFontDataProvider();            // Returns file system provider
```

**Usage:**
```cpp
// Code that needs fonts
IFontProvider &font_provider = GetFontProvider();
auto font = font_provider.GetFont("arial");

// Code that needs to load new fonts
IFontDataProvider &data_provider = GetFontDataProvider();
auto font_data = data_provider.LoadFontFromSource("arial");
```

**Benefits:**
- ✅ Consistent with existing factory pattern
- ✅ Decouples consumers from AssetManager
- ✅ Enables future provider swapping
- ✅ Testable (can return mock providers)

**Effort:** Low (2-3 hours)
**Risk:** Low (follows established pattern)

---

### Gap 4: Direct Instantiation of UI Style Provider

**Current State:**
```cpp
std::expected<std::monostate, FailInfo> AssetManager::LoadUIStyles() {
  // Direct instantiation - no factory
  FlatbuffersUIStyleDataProvider ui_style_provider(m_fonts);
  
  auto ui_style_data_result = ui_style_provider.ProvideUIStyles();
  // ...
}
```

**Expected State:**
```cpp
// Option A: Use factory
std::expected<std::monostate, FailInfo> AssetManager::LoadUIStyles() {
  IUIStyleDataProvider &provider = GetUIStyleDataProvider(*this);  // Pass IFontProvider
  auto ui_style_data_result = provider.ProvideUIStyles();
  // ...
}

// Option B: Inject in constructor (Dependency Injection)
class AssetManager : public IFontProvider {
private:
  IUIStyleDataProvider &m_ui_style_provider;
  
public:
  AssetManager(IUIStyleDataProvider &ui_style_provider)
    : m_ui_style_provider(ui_style_provider) {}
};
```

**Trade-offs:**

**Option A (Factory):**
- ✅ Consistent with other factories
- ✅ Easy to swap implementations
- ❌ Adds another factory function

**Option B (Dependency Injection):**
- ✅ Explicit dependencies
- ✅ Very testable
- ❌ Breaks existing AssetManager construction
- ❌ Requires updating all call sites

**Recommendation:** Option A (Factory) for consistency

**Effort:** Low (1-2 hours)
**Risk:** Low (purely refactoring)

---

### Gap 5: Testing Infrastructure

**Current State:**

**File: `tests/unit/assets/asset_test_helpers.cpp`**
```cpp
void CheckFontConfiguration(const AssetCollection &asset_collection,
                            const AssetManager &asset_manager) {
  for (const auto &font : *asset_collection.fonts()) {
    const std::string font_name = font->name()->str();
    auto font_result = asset_manager.GetFont(font_name);  // ❌ Doesn't exist
    // ...
  }
}
```

**Problems:**
1. Calls `asset_manager.GetFont()` which doesn't exist (should be `GetAllFonts()`)
2. No mock IFontProvider for unit testing
3. No mock IFontDataProvider for testing font loading failures
4. Tests depend on actual FlatBuffers data files
5. Cannot easily test edge cases (missing fonts, corrupted files, etc.)

**Expected Improvements:**

**1. Mock Font Provider:**
```cpp
// tests/unit/assets/MockFontProvider.h
class MockFontProvider : public IFontProvider {
private:
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> m_test_fonts;
  
public:
  void AddTestFont(const std::string &name, std::shared_ptr<const sf::Font> font);
  
  std::expected<std::shared_ptr<const sf::Font>, FailInfo>
    GetFont(const std::string &font_name) const override;
};
```

**2. Mock Font Data Provider:**
```cpp
// tests/unit/assets/MockFontDataProvider.h
class MockFontDataProvider : public IFontDataProvider {
private:
  std::unordered_map<std::string, sf::Font> m_font_data;
  std::vector<std::string> m_fonts_to_fail;
  
public:
  void AddFontData(const std::string &name, const sf::Font &font);
  void SetFontToFail(const std::string &name);
  
  std::expected<sf::Font, FailInfo>
    LoadFontFromSource(const std::string &font_name) const override;
};
```

**3. Test Examples:**
```cpp
TEST_CASE("AssetManager with mock font provider", "[unit][AssetManager]") {
  MockFontDataProvider mock_provider;
  sf::Font test_font;
  mock_provider.AddFontData("test_font", test_font);
  
  AssetManager asset_manager(mock_provider);  // Inject provider
  auto result = asset_manager.LoadFont("test_font");
  REQUIRE(result.has_value());
}

TEST_CASE("AssetManager handles font loading failure", "[unit][AssetManager]") {
  MockFontDataProvider mock_provider;
  mock_provider.SetFontToFail("missing_font");
  
  AssetManager asset_manager(mock_provider);
  auto result = asset_manager.LoadFont("missing_font");
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == FailMode::FileNotFound);
}
```

**Benefits:**
- ✅ Tests don't depend on file system
- ✅ Fast test execution (no disk I/O)
- ✅ Easy to test failure scenarios
- ✅ Can test edge cases without complex setup

**Effort:** Medium (3-4 hours for mock classes + updated tests)
**Risk:** Low (purely additive for testing)

---

## Proposed Improvements

### Improvement 1: Implement IFontProvider in AssetManager

**Priority:** HIGH  
**Effort:** Low (1-2 hours)  
**Risk:** Low

**Changes Required:**

**File: `src/assets/AssetManager.h`**
```cpp
#include "IFontProvider.h"

class AssetManager : public IFontProvider {
  // ... existing members ...
  
public:
  // ... existing methods ...
  
  ////////////////////////////////////////////////////////////
  /// @brief Get a font by name (IFontProvider implementation)
  ///
  /// @param font_name Name of the font to retrieve
  /// @return Shared pointer to the font, or error if not found
  ////////////////////////////////////////////////////////////
  std::expected<std::shared_ptr<const sf::Font>, FailInfo>
    GetFont(const std::string &font_name) const override;
};
```

**File: `src/assets/AssetManager.cpp`**
```cpp
////////////////////////////////////////////////////////////
std::expected<std::shared_ptr<const sf::Font>, FailInfo>
AssetManager::GetFont(const std::string &font_name) const {
  auto it = m_fonts.find(font_name);
  if (it == m_fonts.end()) {
    return std::unexpected<FailInfo>({
      FailMode::NotFoundInMap,
      std::format("Font not found: {}", font_name)
    });
  }
  return it->second;
}
```

**Testing:**
```cpp
TEST_CASE("AssetManager implements IFontProvider", "[unit][AssetManager]") {
  AssetManager asset_manager;
  asset_manager.LoadDefaultAssets();
  
  IFontProvider &provider = asset_manager;  // Upcast to interface
  auto font = provider.GetFont("DaddyTimeMonoNerdFont-Regular");
  REQUIRE(font.has_value());
}

TEST_CASE("AssetManager::GetFont returns error for missing font", "[unit][AssetManager]") {
  AssetManager asset_manager;
  auto font = asset_manager.GetFont("NonExistentFont");
  REQUIRE(!font.has_value());
  REQUIRE(font.error().mode == FailMode::NotFoundInMap);
}
```

**Benefits:**
- ✅ Enables IFontProvider polymorphism
- ✅ Consistent with FONT_PROVIDER_DECOUPLING
- ✅ Zero breaking changes (adds method, doesn't remove)
- ✅ Testable immediately

**Dependencies:**
- None (IFontProvider already exists)

---

### Improvement 2: Extract Font Loading to IFontDataProvider

**Priority:** MEDIUM  
**Effort:** Medium (4-6 hours)  
**Risk:** Medium

**New Interface:**

**File: `src/interfaces/IFontDataProvider.h`**
```cpp
////////////////////////////////////////////////////////////
/// @file
/// @brief Declaration of IFontDataProvider interface
////////////////////////////////////////////////////////////

#pragma once

#include "FailInfo.h"
#include <SFML/Graphics/Font.hpp>
#include <expected>
#include <string>

namespace steamrot {

////////////////////////////////////////////////////////////
/// @class IFontDataProvider
/// @brief Interface for loading font data from various sources
///
/// Implementations handle the actual font loading mechanism
/// (file system, memory, network, cache, etc.)
////////////////////////////////////////////////////////////
class IFontDataProvider {
public:
  virtual ~IFontDataProvider() = default;

  ////////////////////////////////////////////////////////////
  /// @brief Load font data from source
  ///
  /// @param font_name Name of the font to load
  /// @return Font object or error
  ////////////////////////////////////////////////////////////
  virtual std::expected<sf::Font, FailInfo>
    LoadFontFromSource(const std::string &font_name) const = 0;
};

} // namespace steamrot
```

**File System Implementation:**

**File: `src/data_providers/FileSystemFontDataProvider.h`**
```cpp
////////////////////////////////////////////////////////////
/// @file
/// @brief Declaration of FileSystemFontDataProvider class
////////////////////////////////////////////////////////////

#pragma once

#include "IFontDataProvider.h"

namespace steamrot {

////////////////////////////////////////////////////////////
/// @class FileSystemFontDataProvider
/// @brief Loads fonts from the file system
////////////////////////////////////////////////////////////
class FileSystemFontDataProvider : public IFontDataProvider {
public:
  FileSystemFontDataProvider() = default;

  std::expected<sf::Font, FailInfo>
    LoadFontFromSource(const std::string &font_name) const override;
};

} // namespace steamrot
```

**File: `src/data_providers/FileSystemFontDataProvider.cpp`**
```cpp
#include "FileSystemFontDataProvider.h"
#include "paths.h"
#include <filesystem>
#include <format>

namespace steamrot {

////////////////////////////////////////////////////////////
std::expected<sf::Font, FailInfo>
FileSystemFontDataProvider::LoadFontFromSource(
    const std::string &font_name) const {
  
  // Get font directory
  std::filesystem::path font_dir = paths::GetFontsDirectory();
  
  // Generate font file path (assume .ttf)
  std::string font_file_name = font_name + ".ttf";
  std::filesystem::path font_path = font_dir / font_file_name;
  
  // Check if file exists
  if (!std::filesystem::exists(font_path)) {
    return std::unexpected<FailInfo>({
      FailMode::FileNotFound,
      std::format("Font file not found: {}", font_path.string())
    });
  }
  
  // Load font
  sf::Font font;
  bool success = font.openFromFile(font_path);
  if (!success) {
    return std::unexpected<FailInfo>({
      FailMode::FileLoadFailed,
      std::format("Failed to load font from file: {}", font_path.string())
    });
  }
  
  // Unsmooth font (preserve pixel art style)
  font.setSmooth(false);
  
  return font;
}

} // namespace steamrot
```

**Update AssetManager:**

**File: `src/assets/AssetManager.h`**
```cpp
#include "IFontDataProvider.h"

class AssetManager : public IFontProvider {
private:
  IFontDataProvider &m_font_data_provider;
  
  std::expected<std::monostate, FailInfo>
    AddFont(const std::string &font_name);

public:
  // Constructor with provider injection
  AssetManager(IFontDataProvider &font_data_provider);
  
  // ... existing methods ...
};
```

**File: `src/assets/AssetManager.cpp`**
```cpp
////////////////////////////////////////////////////////////
AssetManager::AssetManager(IFontDataProvider &font_data_provider)
    : m_font_data_provider(font_data_provider) {}

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
AssetManager::AddFont(const std::string &font_name) {
  // Use provider instead of direct loading
  auto font_result = m_font_data_provider.LoadFontFromSource(font_name);
  
  if (!font_result.has_value()) {
    return std::unexpected<FailInfo>(font_result.error());
  }
  
  // Store font in map
  m_fonts.insert({font_name, std::make_shared<sf::Font>(font_result.value())});
  
  return std::monostate{};
}
```

**Factory Function:**

**File: `src/data_providers/provider_factory.h`**
```cpp
IFontDataProvider &GetFontDataProvider();
```

**File: `src/data_providers/provider_factory.cpp`**
```cpp
#include "FileSystemFontDataProvider.h"

IFontDataProvider &GetFontDataProvider() {
  static FileSystemFontDataProvider provider;
  return provider;
}
```

**Benefits:**
- ✅ Separation of concerns (loading vs managing)
- ✅ Testable with mock providers
- ✅ Flexible (can add network, memory sources)
- ✅ Consistent with IAssetDataProvider pattern

**Breaking Changes:**
- AssetManager constructor signature changes
- All AssetManager instantiations need updating

**Migration:**
```cpp
// Before:
AssetManager asset_manager;

// After:
AssetManager asset_manager(GetFontDataProvider());
```

---

### Improvement 3: Add Provider Factory Functions

**Priority:** MEDIUM  
**Effort:** Low (1-2 hours)  
**Risk:** Low

**Add to `provider_factory.h`:**
```cpp
////////////////////////////////////////////////////////////
/// @brief Get the font provider instance.
///
/// Returns a reference to the global AssetManager as IFontProvider.
/// Note: AssetManager must be initialized before calling this.
////////////////////////////////////////////////////////////
IFontProvider &GetFontProvider();

////////////////////////////////////////////////////////////
/// @brief Get the font data provider instance.
///
/// Returns a static instance of FileSystemFontDataProvider.
////////////////////////////////////////////////////////////
IFontDataProvider &GetFontDataProvider();
```

**Add to `provider_factory.cpp`:**
```cpp
#include "AssetManager.h"
#include "FileSystemFontDataProvider.h"

// Note: This assumes AssetManager is a singleton or globally accessible
// May need adjustment based on actual AssetManager lifecycle
IFontProvider &GetFontProvider() {
  // Option 1: Return AssetManager singleton
  static AssetManager &asset_manager = GetAssetManager();
  return asset_manager;
  
  // Option 2: If AssetManager is not singleton, this needs different approach
}

IFontDataProvider &GetFontDataProvider() {
  static FileSystemFontDataProvider provider;
  return provider;
}
```

**Note on AssetManager Singleton:**
- Currently AssetManager is not a singleton
- May need to make it one, or pass it through context
- See `GameContext` and `SceneContext` for context-based approach

**Alternative: Context-Based Approach (Recommended)**
```cpp
// In GameContext or SceneContext
class GameContext {
private:
  AssetManager m_asset_manager;
  
public:
  IFontProvider &GetFontProvider() { return m_asset_manager; }
};

// Usage
IFontProvider &font_provider = game_context.GetFontProvider();
```

---

### Improvement 4: Add Factory for UI Style Provider

**Priority:** LOW  
**Effort:** Low (1-2 hours)  
**Risk:** Low

**Add to `provider_factory.h`:**
```cpp
////////////////////////////////////////////////////////////
/// @brief Get the UI style data provider instance.
///
/// @param font_provider Reference to IFontProvider for font access
/// @return Reference to IUIStyleDataProvider
////////////////////////////////////////////////////////////
IUIStyleDataProvider &GetUIStyleDataProvider(IFontProvider &font_provider);
```

**Add to `provider_factory.cpp`:**
```cpp
#include "FlatbuffersUIStyleDataProvider.h"

IUIStyleDataProvider &GetUIStyleDataProvider(IFontProvider &font_provider) {
  // Note: Cannot use static here because font_provider may change
  // Consider alternatives:
  
  // Option 1: Dynamic allocation (memory leak unless managed)
  static IUIStyleDataProvider *provider = nullptr;
  if (!provider) {
    provider = new FlatbuffersUIStyleDataProvider(font_provider);
  }
  return *provider;
  
  // Option 2: Thread-local storage
  // Option 3: Managed by context object
}
```

**Better Alternative: Context-Based**
```cpp
// In AssetManager (or GameContext)
IUIStyleDataProvider &AssetManager::GetUIStyleDataProvider() {
  // Create on-demand
  if (!m_ui_style_provider) {
    m_ui_style_provider = 
      std::make_unique<FlatbuffersUIStyleDataProvider>(*this);
  }
  return *m_ui_style_provider;
}
```

**Update AssetManager::LoadUIStyles:**
```cpp
std::expected<std::monostate, FailInfo> AssetManager::LoadUIStyles() {
  IUIStyleDataProvider &provider = GetUIStyleDataProvider();
  auto ui_style_data_result = provider.ProvideUIStyles();
  // ... rest of implementation
}
```

---

## Testing Infrastructure

### Current Testing Gaps

1. **No Mock Font Provider**
   - Tests require real AssetManager with real files
   - Cannot test IFontProvider interface independently

2. **No Mock Font Data Provider**
   - Cannot test font loading failures
   - Cannot test alternative sources

3. **Limited Edge Case Testing**
   - Missing fonts
   - Corrupted font files
   - Permission errors
   - Network failures (for future network provider)

4. **`asset_test_helpers` Issues**
   - Calls `GetFont()` which doesn't exist
   - Tightly coupled to FlatbuffersDataLoader
   - Cannot use with alternative providers

### Proposed Test Infrastructure

**1. Mock Font Provider**

**File: `tests/unit/assets/MockFontProvider.h`**
```cpp
////////////////////////////////////////////////////////////
/// @file
/// @brief Mock implementation of IFontProvider for testing
////////////////////////////////////////////////////////////

#pragma once

#include "IFontProvider.h"
#include <unordered_map>

namespace steamrot::tests {

class MockFontProvider : public IFontProvider {
private:
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> m_fonts;

public:
  ////////////////////////////////////////////////////////////
  /// @brief Add a test font to the mock provider
  ////////////////////////////////////////////////////////////
  void AddFont(const std::string &name, std::shared_ptr<const sf::Font> font);

  ////////////////////////////////////////////////////////////
  /// @brief Check if a font exists in the mock provider
  ////////////////////////////////////////////////////////////
  bool HasFont(const std::string &name) const;

  ////////////////////////////////////////////////////////////
  /// @brief Clear all fonts from the mock provider
  ////////////////////////////////////////////////////////////
  void Clear();

  ////////////////////////////////////////////////////////////
  /// @brief Get font (IFontProvider implementation)
  ////////////////////////////////////////////////////////////
  std::expected<std::shared_ptr<const sf::Font>, FailInfo>
    GetFont(const std::string &font_name) const override;
};

} // namespace steamrot::tests
```

**2. Mock Font Data Provider**

**File: `tests/unit/assets/MockFontDataProvider.h`**
```cpp
////////////////////////////////////////////////////////////
/// @file
/// @brief Mock implementation of IFontDataProvider for testing
////////////////////////////////////////////////////////////

#pragma once

#include "IFontDataProvider.h"
#include <unordered_map>
#include <set>

namespace steamrot::tests {

class MockFontDataProvider : public IFontDataProvider {
private:
  std::unordered_map<std::string, sf::Font> m_font_data;
  std::set<std::string> m_fonts_to_fail;

public:
  ////////////////////////////////////////////////////////////
  /// @brief Add font data that can be loaded
  ////////////////////////////////////////////////////////////
  void AddFontData(const std::string &name, const sf::Font &font);

  ////////////////////////////////////////////////////////////
  /// @brief Configure a font name to fail when loaded
  ////////////////////////////////////////////////////////////
  void SetFontToFail(const std::string &name);

  ////////////////////////////////////////////////////////////
  /// @brief Clear all configured fonts and failures
  ////////////////////////////////////////////////////////////
  void Clear();

  ////////////////////////////////////////////////////////////
  /// @brief Load font from mock source (IFontDataProvider implementation)
  ////////////////////////////////////////////////////////////
  std::expected<sf::Font, FailInfo>
    LoadFontFromSource(const std::string &font_name) const override;
};

} // namespace steamrot::tests
```

**3. Test Examples**

**File: `tests/unit/assets/AssetManager.test.cpp`** (additions)
```cpp
#include "MockFontProvider.h"
#include "MockFontDataProvider.h"

TEST_CASE("AssetManager implements IFontProvider correctly", 
          "[unit][AssetManager][IFontProvider]") {
  // Test with real AssetManager
  AssetManager asset_manager(GetFontDataProvider());
  asset_manager.LoadDefaultAssets();
  
  IFontProvider &provider = asset_manager;
  auto font = provider.GetFont("DaddyTimeMonoNerdFont-Regular");
  REQUIRE(font.has_value());
}

TEST_CASE("AssetManager with MockFontDataProvider", 
          "[unit][AssetManager][mock]") {
  MockFontDataProvider mock_provider;
  
  // Create a test font
  sf::Font test_font;
  mock_provider.AddFontData("test_font", test_font);
  
  // Create AssetManager with mock provider
  AssetManager asset_manager(mock_provider);
  
  // Load font (bypasses file system)
  // Note: Need to add method to load specific font
  auto result = asset_manager.AddFont("test_font");
  REQUIRE(result.has_value());
  
  // Verify font is accessible
  auto font = asset_manager.GetFont("test_font");
  REQUIRE(font.has_value());
}

TEST_CASE("AssetManager handles font loading failure", 
          "[unit][AssetManager][error]") {
  MockFontDataProvider mock_provider;
  mock_provider.SetFontToFail("missing_font");
  
  AssetManager asset_manager(mock_provider);
  
  // Attempt to load font that will fail
  auto result = asset_manager.AddFont("missing_font");
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == FailMode::FileNotFound);
}

TEST_CASE("IFontProvider interface can be mocked", 
          "[unit][IFontProvider][mock]") {
  MockFontProvider mock_provider;
  
  sf::Font test_font;
  mock_provider.AddFont("test", std::make_shared<sf::Font>(test_font));
  
  IFontProvider &provider = mock_provider;
  auto font = provider.GetFont("test");
  REQUIRE(font.has_value());
  
  auto missing = provider.GetFont("missing");
  REQUIRE(!missing.has_value());
}
```

**4. Updated asset_test_helpers**

**File: `tests/unit/assets/asset_test_helpers.cpp`**
```cpp
void CheckFontConfiguration(const AssetCollection &asset_collection,
                            const AssetManager &asset_manager) {
  if (!asset_collection.fonts()) {
    return;
  }
  
  // Cast to IFontProvider to use GetFont method
  const IFontProvider &font_provider = asset_manager;
  
  for (const auto &font : *asset_collection.fonts()) {
    const std::string font_name = font->name()->str();
    
    // Use IFontProvider interface
    auto font_result = font_provider.GetFont(font_name);
    
    if (!font_result.has_value()) {
      FAIL(font_result.error().message);
    }
  }
}
```

### Testing Benefits

With these improvements:

✅ **Unit Tests Don't Need File System**
- Mock providers return test data from memory
- No dependency on data files
- Fast test execution

✅ **Easy Failure Testing**
- Set fonts to fail on-demand
- Test error handling thoroughly
- Verify error messages

✅ **Flexible Test Scenarios**
- Test with different font sets
- Test with empty providers
- Test provider swapping

✅ **Interface Contract Testing**
- Verify IFontProvider behavior
- Test polymorphism
- Ensure mock and real implementations match

---

## Implementation Roadmap

### Phase 1: Implement IFontProvider (High Priority)

**Effort:** 1-2 hours  
**Risk:** Low  
**Dependencies:** None

**Tasks:**
1. Update `AssetManager.h` to inherit from `IFontProvider`
2. Implement `GetFont()` method in `AssetManager.cpp`
3. Add unit tests for `GetFont()`
4. Update `asset_test_helpers.cpp` to use `GetFont()`
5. Run existing tests to verify no regressions

**Success Criteria:**
- ✅ AssetManager implements IFontProvider
- ✅ GetFont() returns fonts correctly
- ✅ GetFont() returns error for missing fonts
- ✅ All existing tests pass
- ✅ asset_test_helpers works with GetFont()

**Validation:**
```bash
# Build and test
cmake --build --preset Debug
ctest --preset Debug -R AssetManager
```

---

### Phase 2: Create Test Infrastructure (High Priority)

**Effort:** 3-4 hours  
**Risk:** Low  
**Dependencies:** Phase 1 complete

**Tasks:**
1. Create `MockFontProvider.h/cpp` in `tests/unit/assets/`
2. Create `MockFontDataProvider.h/cpp` in `tests/unit/assets/`
3. Add unit tests using mock providers
4. Add tests for error scenarios
5. Add tests for IFontProvider interface contract

**Success Criteria:**
- ✅ Mock providers implemented
- ✅ Tests pass with mock providers
- ✅ Tests verify error handling
- ✅ Tests verify IFontProvider contract

**Validation:**
```bash
ctest --preset Debug -R "AssetManager.*mock"
ctest --preset Debug -R "IFontProvider"
```

---

### Phase 3: Extract Font Loading Logic (Medium Priority)

**Effort:** 4-6 hours  
**Risk:** Medium  
**Dependencies:** Phases 1-2 complete

**Tasks:**
1. Create `IFontDataProvider` interface in `src/interfaces/`
2. Create `FileSystemFontDataProvider` in `src/data_providers/`
3. Move font loading logic from AssetManager to provider
4. Update AssetManager to use IFontDataProvider
5. Add factory function `GetFontDataProvider()`
6. Update all AssetManager construction sites
7. Add unit tests for FileSystemFontDataProvider
8. Run full test suite

**Success Criteria:**
- ✅ IFontDataProvider interface defined
- ✅ FileSystemFontDataProvider implemented
- ✅ AssetManager uses provider
- ✅ Factory function works
- ✅ All tests pass
- ✅ No regressions in font loading

**Breaking Changes:**
- AssetManager constructor changes
- Requires provider parameter

**Migration Path:**
```cpp
// Update all construction sites:
// Before:
AssetManager asset_manager;

// After:
AssetManager asset_manager(GetFontDataProvider());

// Or via context:
GameContext context;
AssetManager asset_manager(context.GetFontDataProvider());
```

**Validation:**
```bash
# Full test suite
ctest --preset Debug

# Verify font loading
ctest --preset Debug -R "AssetManager.*LoadDefaultAssets"
```

---

### Phase 4: Add Provider Factories (Low Priority)

**Effort:** 2-3 hours  
**Risk:** Low  
**Dependencies:** Phase 3 complete

**Tasks:**
1. Add `GetFontProvider()` to `provider_factory.h/cpp`
2. Add `GetFontDataProvider()` to `provider_factory.h/cpp`
3. Consider context-based approach vs singleton
4. Update documentation
5. Add examples of factory usage

**Success Criteria:**
- ✅ Factory functions implemented
- ✅ Factories return correct providers
- ✅ Documentation updated
- ✅ Examples added

**Notes:**
- May not need factory for IFontProvider if context-based
- IFontDataProvider factory is straightforward
- Consider AssetManager lifecycle

---

### Phase 5: UI Style Provider Factory (Optional, Low Priority)

**Effort:** 1-2 hours  
**Risk:** Low  
**Dependencies:** None

**Tasks:**
1. Consider factory vs context-based approach
2. If factory: Add `GetUIStyleDataProvider()` with IFontProvider parameter
3. If context: Add method to AssetManager or context class
4. Update `AssetManager::LoadUIStyles()` to use factory
5. Add tests

**Success Criteria:**
- ✅ UI style provider access standardized
- ✅ Consistent with other providers
- ✅ Tests pass

**Recommendation:**
- Context-based approach preferred (simpler lifecycle)

---

## Dependencies and Risks

### Technical Dependencies

**Phase 1 Dependencies:**
- IFontProvider interface (exists)
- AssetManager class (exists)
- **No external dependencies**

**Phase 2 Dependencies:**
- Phase 1 complete
- Catch2 testing framework (exists)
- **No external dependencies**

**Phase 3 Dependencies:**
- Phase 1-2 complete
- SFML Font class (exists)
- paths utility (exists)
- **Breaking change: AssetManager constructor**

**Phase 4 Dependencies:**
- Phase 3 complete
- Provider factory pattern (exists)
- Decision on singleton vs context approach

### Risk Assessment

**Phase 1 (Implement IFontProvider):**
- **Risk Level:** LOW
- **Mitigation:** Additive change only, no removal
- **Rollback:** Easy (remove method and inheritance)

**Phase 2 (Test Infrastructure):**
- **Risk Level:** LOW
- **Mitigation:** Test-only code, doesn't affect production
- **Rollback:** Easy (delete test files)

**Phase 3 (Extract Font Loading):**
- **Risk Level:** MEDIUM
- **Mitigation:**
  - Comprehensive testing before and after
  - Gradual migration (keep old code initially)
  - Test with real font files
- **Rollback:** Medium difficulty (revert constructor changes)
- **Breaking Change:** AssetManager constructor signature

**Phase 4 (Provider Factories):**
- **Risk Level:** LOW
- **Mitigation:** Optional pattern, doesn't break existing code
- **Rollback:** Easy (remove factory functions)

### Impact on Existing Code

**Files That Need Updates:**

**Phase 1:**
- `src/assets/AssetManager.h` (add interface)
- `src/assets/AssetManager.cpp` (add method)
- `tests/unit/assets/asset_test_helpers.cpp` (use GetFont)

**Phase 2:**
- Only test files (additive)

**Phase 3:**
- All files that construct AssetManager:
  - `src/main.cpp` (or wherever game is initialized)
  - `src/context/GameContext.cpp` (if AssetManager is there)
  - Test files that create AssetManager
  - Estimate: 5-10 files

**Phase 4:**
- `src/data_providers/provider_factory.h/cpp` (add functions)
- Documentation files

### Testing Strategy

**Unit Tests:**
- Test IFontProvider implementation
- Test mock providers
- Test font loading with FileSystemFontDataProvider
- Test error scenarios

**Integration Tests:**
- Test AssetManager with real font files
- Test full asset loading pipeline
- Test with different scene types

**Regression Tests:**
- Run full test suite after each phase
- Verify font loading in actual game
- Check UI rendering with loaded fonts

---

## Appendix A: Code Examples

### Example 1: Using IFontProvider

**Before:**
```cpp
const auto &fonts = asset_manager.GetAllFonts();
auto it = fonts.find("arial");
if (it != fonts.end()) {
  const sf::Font &font = *it->second;
  // Use font
}
```

**After:**
```cpp
IFontProvider &provider = asset_manager;
auto font_result = provider.GetFont("arial");
if (font_result.has_value()) {
  const sf::Font &font = *font_result.value();
  // Use font
}
```

### Example 2: Mock Testing

**Before (required file system):**
```cpp
TEST_CASE("Test font loading") {
  AssetManager asset_manager;
  asset_manager.LoadDefaultAssets();  // Loads from disk
  // Test...
}
```

**After (no file system):**
```cpp
TEST_CASE("Test font loading") {
  MockFontDataProvider mock_provider;
  sf::Font test_font;
  mock_provider.AddFontData("test", test_font);
  
  AssetManager asset_manager(mock_provider);  // No disk access
  // Test...
}
```

### Example 3: Factory Usage

**Before:**
```cpp
AssetManager asset_manager;
```

**After:**
```cpp
// Option 1: Direct factory
AssetManager asset_manager(GetFontDataProvider());

// Option 2: Via context
GameContext context;
AssetManager asset_manager(context.GetFontDataProvider());
```

---

## Document Metadata

**Author:** GitHub Copilot Analysis Agent  
**Date:** 2025-12-22  
**Version:** 1.0  
**Status:** Draft - For Review

**Related Documents:**
- `FONT_PROVIDER_DECOUPLING.md` - IFontProvider interface pattern
- `USER_INTERFACE_DECOUPLING_ANALYSIS.md` - Provider pattern examples
- `.github/copilot-instructions.md` - Coding standards and patterns

**Next Steps:**
1. Review analysis with team
2. Prioritize phases based on project needs
3. Create implementation tasks
4. Begin Phase 1 (Implement IFontProvider)

---

## Questions for Review

1. **Priority:** Do we agree with the proposed phase priorities?
2. **Constructor:** Is dependency injection via constructor acceptable for AssetManager?
3. **Singleton:** Should AssetManager be a singleton, or context-based?
4. **Factory:** Do we want factories for all providers, or prefer context-based access?
5. **Timeline:** What is the desired timeline for completion?
6. **Breaking Changes:** Are breaking changes acceptable in Phase 3?

---

**End of Analysis**
