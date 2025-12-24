# AssetManager Implementation TODO List

This document provides a detailed, prioritized task list for implementing the AssetManager refactoring, including test requirements and addressing future-proofing for save data.

**Date:** 2025-12-24  
**Related:** ASSET_MANAGER_MIGRATION_GUIDE.md, ASSET_MANAGER_ANALYSIS.md  
**Status:** Implementation Roadmap

---

## Table of Contents

1. [Future-Proofing for Save Data](#future-proofing-for-save-data)
2. [Loader Responsibilities](#loader-responsibilities)
3. [Staged Asset Loading](#staged-asset-loading)
4. [Complete TODO List](#complete-todo-list)

---

## Future-Proofing for Save Data

### Current Pattern in Codebase

The codebase already has an established pattern for handling default vs user save data:

**Example: EngineConfig (from `FlatbuffersDataLoader.cpp`)**
```cpp
std::expected<const EngineConfigFbs *, FailInfo>
FlatbuffersDataLoader::ProvideEngineConfigFbs() const {
  // 1. First check for user-specific config
  std::filesystem::path user_config_path = 
      paths::GetUserDirectory() / "engine" / "default.engine_config.bin";

  if (std::filesystem::exists(user_config_path)) {
    auto config_data = GetEngineConfigFbs(LoadBinaryData(user_config_path));
    if (config_data) {
      return config_data;  // Return user config if valid
    }
    // Fall through to defaults if user config invalid
  }

  // 2. Load default config as fallback
  std::filesystem::path default_config_path = 
      paths::GetDefaultEngineDirectory() / "default.engine_config.bin";
  
  return GetEngineConfigFbs(LoadBinaryData(default_config_path));
}
```

**Path Structure:**
```
data/
├── defaults/           # Read-only default data
│   ├── engine/
│   ├── asset_manager/
│   ├── scenes/
│   └── preferences/
└── user/              # Read-write user data
    ├── engine/        # User overrides for engine config
    ├── saves/         # Save game data
    └── preferences/   # User preferences
```

### Applying Pattern to AssetManager

**Strategy: Reuse Existing Pattern**

The loader abstraction naturally supports this pattern. Here's how to future-proof:

#### 1. Define Asset Data Locations

**Default Assets (Read-Only):**
- `data/defaults/asset_manager/asset_manager.bin`
- `data/defaults/scenes/{scene_name}.scene_data.bin` (with assets)
- `data/assets/fonts/*.ttf` (actual font files)

**User Assets (Read-Write, Future):**
- `data/user/asset_manager/asset_manager.bin` (overrides)
- `data/user/saves/{save_slot}/asset_manager/asset_manager.bin` (save-specific)
- `data/user/assets/fonts/*.ttf` (custom fonts, future)

#### 2. Implement Fallback Chain in Loaders

**Example: FontLoader with User Data Support**

```cpp
class FontLoader : public IFontLoader {
private:
  ////////////////////////////////////////////////////////////
  /// @brief Load font with fallback chain
  ///
  /// 1. Check user directory first
  /// 2. Fall back to defaults
  /// 3. Return error if not found
  ////////////////////////////////////////////////////////////
  std::expected<std::shared_ptr<sf::Font>, FailInfo>
  LoadFont(const std::string &font_name) const {
    
    // Try user fonts first (future-proofing)
    std::filesystem::path user_font_dir = 
        paths::GetUserDirectory() / "assets" / "fonts";
    std::filesystem::path user_font_path = 
        user_font_dir / (font_name + ".ttf");
    
    if (std::filesystem::exists(user_font_path)) {
      auto font_result = LoadFontFromFile(user_font_path);
      if (font_result.has_value()) {
        return font_result;  // Return user font
      }
      // Log warning: user font exists but failed to load
      // Fall through to defaults
    }
    
    // Load from defaults
    std::filesystem::path default_font_dir = paths::GetFontsDirectory();
    std::filesystem::path default_font_path = 
        default_font_dir / (font_name + ".ttf");
    
    if (!std::filesystem::exists(default_font_path)) {
      return std::unexpected(FailInfo{
          FailMode::FileNotFound,
          std::format("Font not found: {}", font_name)});
    }
    
    return LoadFontFromFile(default_font_path);
  }
  
  ////////////////////////////////////////////////////////////
  /// @brief Helper to load font from a specific path
  ////////////////////////////////////////////////////////////
  std::expected<std::shared_ptr<sf::Font>, FailInfo>
  LoadFontFromFile(const std::filesystem::path &path) const {
    auto font = std::make_shared<sf::Font>();
    if (!font->openFromFile(path)) {
      return std::unexpected(FailInfo{
          FailMode::FileNotFound,
          std::format("Failed to load font: {}", path.string())});
    }
    font->setSmooth(false);
    return font;
  }
};
```

#### 3. Code Reuse Between Routes

**Shared Components:**

```cpp
// Reusable helper for fallback pattern
namespace steamrot::loaders {

////////////////////////////////////////////////////////////
/// @brief Generic fallback loader helper
///
/// Tries user path first, falls back to default path
////////////////////////////////////////////////////////////
template <typename T>
std::expected<T, FailInfo>
LoadWithFallback(
    const std::filesystem::path &user_path,
    const std::filesystem::path &default_path,
    std::function<std::expected<T, FailInfo>(const std::filesystem::path&)> loader) {
  
  // Try user path first
  if (std::filesystem::exists(user_path)) {
    auto result = loader(user_path);
    if (result.has_value()) {
      return result;
    }
    // Log: User asset exists but failed to load, falling back
  }
  
  // Try default path
  if (!std::filesystem::exists(default_path)) {
    return std::unexpected(FailInfo{
        FailMode::FileNotFound,
        std::format("Asset not found at user or default path")});
  }
  
  return loader(default_path);
}

} // namespace steamrot::loaders
```

**Usage in FontLoader:**
```cpp
std::expected<std::shared_ptr<sf::Font>, FailInfo>
FontLoader::LoadFont(const std::string &font_name) const {
  auto user_path = paths::GetUserDirectory() / "assets" / "fonts" / (font_name + ".ttf");
  auto default_path = paths::GetFontsDirectory() / (font_name + ".ttf");
  
  return loaders::LoadWithFallback<std::shared_ptr<sf::Font>>(
      user_path, 
      default_path, 
      [this](const auto &path) { return LoadFontFromFile(path); });
}
```

### Benefits of This Approach

1. **Consistent with Codebase:** Uses same pattern as EngineConfig
2. **Future-Proof:** User assets work when added, no refactoring needed
3. **Code Reuse:** `LoadWithFallback` helper works for all asset types
4. **Graceful Degradation:** Falls back to defaults if user assets invalid
5. **No Breaking Changes:** Defaults work now, user assets work later

---

## Loader Responsibilities

### Primary Responsibilities

Loaders have **three core responsibilities**:

#### 1. Data Acquisition
- **What:** Read raw data from sources (files, network, etc.)
- **Where:** Filesystem, database, network endpoints
- **How:** Use platform-specific I/O (std::filesystem, SFML, etc.)

**Example:**
```cpp
// FontLoader reads font files from disk
auto font_data = font->openFromFile(font_path);
```

#### 2. Object Construction
- **What:** Create and configure game objects from raw data
- **Where:** Instantiate SFML objects, game types
- **How:** Call constructors, setters, configuration methods

**Example:**
```cpp
// FontLoader creates sf::Font objects
auto font = std::make_shared<sf::Font>();
font->openFromFile(path);
font->setSmooth(false);  // Configuration
```

#### 3. Data Transformation
- **What:** Convert from storage format to runtime format
- **Where:** FlatBuffers → Native types, JSON → Objects
- **How:** Parse, validate, transform

**Example:**
```cpp
// StyleLoader converts FlatBuffers → UIStyle
const UIStyleData *fb_style = GetUIStyleData(binary_data);
UIStyle native_style;
native_style.name = fb_style->name()->str();  // Transform
native_style.panel_style = ConfigurePanelStyle(fb_style->panel_style());
```

### What Loaders Do NOT Do

**Not Responsible For:**
- ✗ Storage/Registry management (that's AssetRegistry)
- ✗ Lifecycle management (that's AssetManager)
- ✗ Access control (that's IFontProvider interfaces)
- ✗ Caching strategies (that's AssetRegistry, future)
- ✗ Business logic (that's game systems)

### Loader Types and Their Responsibilities

| Loader | Acquisition | Construction | Transformation |
|--------|-------------|--------------|----------------|
| **FontLoader** | Read .ttf files | Create sf::Font | Apply smoothing settings |
| **StyleLoader** | Read .fbs files | Create UIStyle | FlatBuffers → Native types |
| **TextureLoader** | Read image files | Create sf::Texture | Apply texture settings |
| **SoundLoader** | Read audio files | Create sf::SoundBuffer | Apply audio settings |

### Interaction with Other Components

```
┌─────────────────────────────────────────────┐
│  AssetManager                               │
│  - Coordinates loading                      │
│  - Manages lifecycle                        │
│  - Provides access                          │
└──────────────┬──────────────────────────────┘
               │
               │ "Load fonts"
               ▼
┌─────────────────────────────────────────────┐
│  FontLoader (IFontLoader)                   │
│  1. Read font files (Acquisition)           │
│  2. Create sf::Font objects (Construction)  │
│  3. Configure font properties (Transform)   │
└──────────────┬──────────────────────────────┘
               │
               │ Returns vector<Font>
               ▼
┌─────────────────────────────────────────────┐
│  AssetRegistry                              │
│  - Stores fonts in map                      │
│  - Provides retrieval by name               │
└─────────────────────────────────────────────┘
```

### Design Principle: Separation of Concerns

**Why separate loading from storage?**

1. **Testing:** Can test loading without registry
2. **Flexibility:** Swap loaders (file vs network)
3. **Reusability:** Same registry for all loaders
4. **Clarity:** Clear "load" vs "store" phases

**Example:**
```cpp
// Clear separation
auto fonts = font_loader->LoadDefaultAssets();  // Loader's job
for (auto &font : fonts.value()) {
  registry.AddFont(name, font);  // Registry's job
}
```

---

## Staged Asset Loading

### Motivation

Some assets depend on others. Loading order matters:

**Dependencies:**
- UI Styles **depend on** Fonts (need font pointers)
- Textures **depend on** nothing (independent)
- Sounds **depend on** nothing (independent)
- Materials **might depend on** Textures (future)

**Problem without stages:**
```cpp
// Bad: UI styles fail because fonts not loaded yet
LoadUIStyles();  // Tries to get fonts
LoadFonts();     // Fonts loaded second - TOO LATE!
```

### Proposed Staged Approach

#### Stage Definitions

**Stage 0: Independent Assets**
- Assets with **no dependencies**
- Can load in any order
- Examples: Fonts, Textures (base), Sounds

**Stage 1: First-Order Dependencies**
- Assets that depend on **Stage 0** assets
- Examples: UI Styles (need Fonts), Materials (need Textures)

**Stage 2: Second-Order Dependencies**
- Assets that depend on **Stage 1** assets
- Examples: Complex UI (needs Styles), Advanced Materials

**Stage N: Nth-Order Dependencies**
- Continue as needed for complex dependency graphs

### Implementation: Explicit Stage Loading

**Option A: Explicit Stages (Recommended)**

```cpp
class AssetManager {
public:
  ////////////////////////////////////////////////////////////
  /// @brief Load all default assets in dependency order
  ////////////////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> LoadDefaultAssets() {
    // Stage 0: Independent assets
    auto stage0_result = LoadStage0Assets();
    if (!stage0_result.has_value())
      return std::unexpected(stage0_result.error());
    
    // Stage 1: First-order dependencies
    auto stage1_result = LoadStage1Assets();
    if (!stage1_result.has_value())
      return std::unexpected(stage1_result.error());
    
    // Future stages...
    
    return std::monostate{};
  }

private:
  ////////////////////////////////////////////////////////////
  /// @brief Stage 0: Load independent assets
  ///
  /// Assets loaded:
  /// - Fonts (no dependencies)
  /// - Base textures (no dependencies)
  /// - Sounds (no dependencies)
  ////////////////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> LoadStage0Assets() {
    // Load fonts
    auto font_loader = m_data_access_factory.GetFontLoader();
    if (!font_loader.has_value())
      return std::unexpected(font_loader.error());
    
    auto fonts = font_loader.value()->LoadDefaultAssets();
    if (!fonts.has_value())
      return std::unexpected(fonts.error());
    
    // Store fonts in registry
    for (const auto &[name, font] : fonts.value()) {
      auto result = m_asset_registry.AddFont(name, font);
      if (!result.has_value())
        return std::unexpected(result.error());
    }
    
    // Load textures (future)
    // Load sounds (future)
    
    return std::monostate{};
  }
  
  ////////////////////////////////////////////////////////////
  /// @brief Stage 1: Load assets with Stage 0 dependencies
  ///
  /// Assets loaded:
  /// - UI Styles (depends on Fonts)
  ////////////////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> LoadStage1Assets() {
    // Load UI styles (needs fonts loaded first)
    auto style_loader = m_data_access_factory.GetStyleLoader();
    if (!style_loader.has_value())
      return std::unexpected(style_loader.error());
    
    // Pass IFontProvider so styles can reference fonts
    auto styles = style_loader.value()->LoadDefaultAssets(*this);
    if (!styles.has_value())
      return std::unexpected(styles.error());
    
    // Store styles in registry
    for (const auto &style : styles.value()) {
      auto result = m_asset_registry.AddUIStyle(style);
      if (!result.has_value())
        return std::unexpected(result.error());
    }
    
    return std::monostate{};
  }
};
```

**Benefits:**
- ✅ Clear dependency order
- ✅ Easy to understand and debug
- ✅ Explicit documentation of dependencies
- ✅ Easy to add new stages

**Drawbacks:**
- ⚠️ Manual ordering (must know dependencies)
- ⚠️ Not dynamic (can't reorder at runtime)

#### Alternative: Dependency Graph (Advanced, Future)

For complex dependency graphs, consider a dependency resolution system:

```cpp
// Future enhancement - not needed now
class AssetDependencyGraph {
  // Register dependencies
  AddDependency(AssetType::UIStyle, AssetType::Font);
  AddDependency(AssetType::Material, AssetType::Texture);
  
  // Resolve loading order
  auto load_order = ResolveLoadOrder();  // Topological sort
  
  // Load in order
  for (auto asset_type : load_order) {
    LoadAssetsOfType(asset_type);
  }
};
```

**When to use:**
- Only if dependency graph becomes complex (5+ stages)
- Only if dynamic ordering needed
- For now, explicit stages are sufficient

### Staged Loading for Scene Assets

Same principle applies:

```cpp
std::expected<std::monostate, FailInfo>
AssetManager::LoadSceneAssets(SceneType scene_type) {
  // Stage 0: Scene fonts (if any)
  auto fonts_result = LoadSceneFontsStage0(scene_type);
  if (!fonts_result.has_value())
    return std::unexpected(fonts_result.error());
  
  // Stage 1: Scene styles (depends on fonts)
  auto styles_result = LoadSceneStylesStage1(scene_type);
  if (!styles_result.has_value())
    return std::unexpected(styles_result.error());
  
  return std::monostate{};
}
```

### Documentation of Dependencies

**In each loader, document dependencies:**

```cpp
////////////////////////////////////////////////////////////
/// @class StyleLoader
/// @brief Loads UI style assets
///
/// **Dependencies:**
/// - Fonts (Stage 0) - Required for text styling
///
/// **Loading Stage:** 1 (depends on Stage 0 assets)
////////////////////////////////////////////////////////////
class StyleLoader : public IStyleLoader {
  // Requires IFontProvider in constructor
  std::expected<std::vector<UIStyle>, FailInfo>
  LoadDefaultAssets(const IFontProvider &font_provider) override;
};
```

### Summary: Staged Loading

**Current Assets:**
- **Stage 0:** Fonts (independent)
- **Stage 1:** UI Styles (depends on Fonts)

**Future Assets:**
- **Stage 0:** Fonts, Textures, Sounds
- **Stage 1:** UI Styles (→ Fonts), Materials (→ Textures)
- **Stage 2:** Complex UI (→ Styles), Advanced Materials (→ Materials)

---

## Complete TODO List

### Phase 1: Create Abstractions (Low Risk, 4-6 hours)

**Goal:** Add new interfaces without breaking existing code

#### 1.1 Create Generic Loader Interface
- [ ] **File:** `src/interfaces/IAssetLoader.h`
  - [ ] Define `IAssetLoader<T>` template interface
  - [ ] Method: `LoadDefaultAssets() → vector<T>`
  - [ ] Method: `LoadSceneAssets(SceneType) → vector<T>`
  - [ ] Add Doxygen documentation
- [ ] **Test:** `tests/unit/interfaces/IAssetLoader.test.cpp`
  - [ ] Test interface contract with mock implementation
  - [ ] Verify template works with different types

#### 1.2 Create Font Loader Interface
- [ ] **File:** `src/interfaces/IFontLoader.h`
  - [ ] Define `IFontLoader : IAssetLoader<pair<string, shared_ptr<sf::Font>>>`
  - [ ] Override methods with font-specific return types
  - [ ] Document Stage 0 loading (no dependencies)
- [ ] **Test:** `tests/unit/interfaces/IFontLoader.test.cpp`
  - [ ] Test interface with mock font loader
  - [ ] Verify return type correct

#### 1.3 Create Style Loader Interface
- [ ] **File:** `src/interfaces/IStyleLoader.h`
  - [ ] Define `IStyleLoader : IAssetLoader<UIStyle>`
  - [ ] Override: `LoadDefaultAssets(IFontProvider&)`
  - [ ] Override: `LoadSceneAssets(SceneType, IFontProvider&)`
  - [ ] Document Stage 1 loading (depends on fonts)
- [ ] **Test:** `tests/unit/interfaces/IStyleLoader.test.cpp`
  - [ ] Test interface with mock style loader
  - [ ] Verify IFontProvider parameter required

#### 1.4 Create Asset Registry
- [ ] **File:** `src/assets/AssetRegistry.h`
  - [ ] Define AssetRegistry class
  - [ ] Method: `AddFont(string, shared_ptr<Font>)`
  - [ ] Method: `GetFont(string) → expected<Font, FailInfo>`
  - [ ] Method: `AddUIStyle(UIStyle)`
  - [ ] Method: `GetUIStyle(string) → expected<UIStyle&, FailInfo>`
  - [ ] Method: `GetAllFonts()`, `GetAllUIStyles()`
  - [ ] Method: `ClearFonts()`, `ClearUIStyles()`
- [ ] **File:** `src/assets/AssetRegistry.cpp`
  - [ ] Implement all methods
  - [ ] Return appropriate errors
- [ ] **Test:** `tests/unit/assets/AssetRegistry.test.cpp`
  - [ ] Test adding fonts (success case)
  - [ ] Test adding duplicate fonts (error case)
  - [ ] Test getting existing font (success)
  - [ ] Test getting missing font (error)
  - [ ] Test adding UI styles (success case)
  - [ ] Test adding duplicate styles (error case)
  - [ ] Test getting existing style (success)
  - [ ] Test getting missing style (error)
  - [ ] Test GetAll methods return correct maps
  - [ ] Test Clear methods empty maps

#### 1.5 Create Loader Helpers
- [ ] **File:** `src/loaders/loader_helpers.h`
  - [ ] Define `LoadWithFallback<T>` template function
  - [ ] Takes user_path, default_path, loader function
  - [ ] Returns first successful load or error
  - [ ] Add logging for fallback events
- [ ] **Test:** `tests/unit/loaders/loader_helpers.test.cpp`
  - [ ] Test user path exists and valid (returns user)
  - [ ] Test user path exists but invalid (returns default)
  - [ ] Test only default exists (returns default)
  - [ ] Test neither exists (returns error)

---

### Phase 2: Implement Loaders (Low Risk, 6-8 hours)

**Goal:** Create concrete loader implementations

#### 2.1 Implement Font Loader
- [ ] **File:** `src/loaders/FontLoader.h`
  - [ ] Define `FontLoader : IFontLoader`
  - [ ] Constructor takes `IAssetDataProvider&`
  - [ ] Private: `LoadFont(string) → expected<Font, FailInfo>`
  - [ ] Private: `LoadFontFromFile(path) → expected<Font, FailInfo>`
  - [ ] Document Stage 0 classification
- [ ] **File:** `src/loaders/FontLoader.cpp`
  - [ ] Implement `LoadDefaultAssets()`
    - [ ] Get AssetData from provider
    - [ ] For each font name, call LoadFont
    - [ ] Return vector of (name, font) pairs
  - [ ] Implement `LoadSceneAssets(SceneType)`
    - [ ] Get scene AssetData from provider
    - [ ] Load scene-specific fonts
  - [ ] Implement `LoadFont(font_name)`
    - [ ] Use `LoadWithFallback` helper
    - [ ] Try user fonts directory first
    - [ ] Fall back to default fonts directory
    - [ ] Call LoadFontFromFile with path
  - [ ] Implement `LoadFontFromFile(path)`
    - [ ] Check file exists
    - [ ] Create sf::Font
    - [ ] Call openFromFile
    - [ ] Set smooth(false)
    - [ ] Return shared_ptr<Font>
- [ ] **Test:** `tests/unit/loaders/FontLoader.test.cpp`
  - [ ] Test LoadDefaultAssets with mock provider
  - [ ] Test LoadSceneAssets with mock provider
  - [ ] Test LoadFont with existing font (success)
  - [ ] Test LoadFont with missing font (error)
  - [ ] Test LoadFont with user override (returns user)
  - [ ] Test LoadFontFromFile with valid file
  - [ ] Test LoadFontFromFile with invalid file
  - [ ] Test font configuration (smooth = false)

#### 2.2 Implement Style Loader
- [ ] **File:** `src/loaders/FlatbuffersStyleLoader.h`
  - [ ] Define `FlatbuffersStyleLoader : IStyleLoader`
  - [ ] Constructor takes no parameters (creates own FlatbuffersDataLoader)
  - [ ] Private: `ConfigureStyle(UIStyleData*, IFontProvider&) → expected<UIStyle, FailInfo>`
  - [ ] Document Stage 1 classification (depends on fonts)
- [ ] **File:** `src/loaders/FlatbuffersStyleLoader.cpp`
  - [ ] Implement `LoadDefaultAssets(IFontProvider&)`
    - [ ] Create FlatbuffersDataLoader
    - [ ] Call ProvideUIStylesData()
    - [ ] For each UIStyleData, call ConfigureStyle
    - [ ] Return vector<UIStyle>
  - [ ] Implement `LoadSceneAssets(SceneType, IFontProvider&)`
    - [ ] Load scene-specific style data
    - [ ] Configure styles with font provider
  - [ ] Implement `ConfigureStyle(data, font_provider)`
    - [ ] Extract this logic from FlatbuffersUIStyleDataProvider
    - [ ] Use font_provider.GetFont() instead of direct map access
    - [ ] Configure all style types (Panel, Button, DropDown, etc.)
    - [ ] Return configured UIStyle
- [ ] **Test:** `tests/unit/loaders/FlatbuffersStyleLoader.test.cpp`
  - [ ] Test LoadDefaultAssets with mock font provider
  - [ ] Test style configuration uses GetFont() correctly
  - [ ] Test error handling when font not found
  - [ ] Test all style components configured
  - [ ] Test LoadSceneAssets with scene-specific styles

#### 2.3 Update DataAccessFactory
- [ ] **File:** `src/data_providers/DataAccessFactory.h`
  - [ ] Add member: `unique_ptr<IFontLoader> m_font_loader`
  - [ ] Add member: `unique_ptr<IStyleLoader> m_style_loader`
  - [ ] Add method: `GetFontLoader() → expected<IFontLoader*, FailInfo>`
  - [ ] Add method: `GetStyleLoader() → expected<IStyleLoader*, FailInfo>`
- [ ] **File:** `src/data_providers/DataAccessFactory.cpp`
  - [ ] Update `SetFlatbuffersDataProviders()`
    - [ ] Create `FontLoader` with asset data provider
    - [ ] Store in m_font_loader
    - [ ] Create `FlatbuffersStyleLoader`
    - [ ] Store in m_style_loader
  - [ ] Implement `GetFontLoader()`
    - [ ] Return pointer or error if null
  - [ ] Implement `GetStyleLoader()`
    - [ ] Return pointer or error if null
- [ ] **Test:** `tests/unit/data_providers/DataAccessFactory.test.cpp`
  - [ ] Test GetFontLoader returns valid pointer
  - [ ] Test GetStyleLoader returns valid pointer
  - [ ] Test loaders created with correct data type

---

### Phase 3: Fix Encapsulation (Medium Risk, 4-6 hours)

**Goal:** Remove direct map passing to UIStyleDataProvider

#### 3.1 Update IUIStyleDataProvider Interface
- [ ] **File:** `src/interfaces/IUIStyleDataProvider.h`
  - [ ] Change constructor parameter: `fonts_map&` → `IFontProvider&`
  - [ ] Update member: `fonts_map` → `m_font_provider`
  - [ ] Update documentation
- [ ] **Test:** Update all tests using IUIStyleDataProvider
  - [ ] Pass mock IFontProvider instead of map

#### 3.2 Update FlatbuffersUIStyleDataProvider
- [ ] **File:** `src/data_providers/FlatbuffersUIStyleDataProvider.h`
  - [ ] Update constructor: takes `IFontProvider&`
  - [ ] Update all method signatures
- [ ] **File:** `src/data_providers/FlatbuffersUIStyleDataProvider.cpp`
  - [ ] Update constructor implementation
  - [ ] Replace all `fonts_map.find()` with `m_font_provider.GetFont()`
  - [ ] In `ConfigureButtonStyle`:
    - [ ] Change: `auto it = fonts_map.find(name)`
    - [ ] To: `auto font_result = m_font_provider.GetFont(name)`
    - [ ] Handle expected result
  - [ ] In `ConfigureDropDownListStyle`: Same pattern
  - [ ] In `ConfigureDropDownItemStyle`: Same pattern
- [ ] **Test:** `tests/unit/data_providers/FlatbuffersUIStyleDataProvider.test.cpp`
  - [ ] Test with mock IFontProvider
  - [ ] Test font lookup via GetFont()
  - [ ] Test error handling when font not found
  - [ ] Verify no direct map access

#### 3.3 Update AssetManager LoadUIStyles
- [ ] **File:** `src/assets/AssetManager.cpp`
  - [ ] Update `LoadUIStyles()`:
    - [ ] Change: `FlatbuffersUIStyleDataProvider provider(m_fonts)`
    - [ ] To: `FlatbuffersUIStyleDataProvider provider(*this)`
  - [ ] Verify AssetManager is IFontProvider
- [ ] **Test:** `tests/unit/assets/AssetManager.test.cpp`
  - [ ] Test LoadUIStyles uses IFontProvider interface
  - [ ] Verify encapsulation not broken

---

### Phase 4: Refactor AssetManager (High Risk, 8-10 hours)

**Goal:** Use new loaders and registry in AssetManager

#### 4.1 Add AssetRegistry to AssetManager
- [ ] **File:** `src/assets/AssetManager.h`
  - [ ] Add include: `#include "AssetRegistry.h"`
  - [ ] Add member: `AssetRegistry m_asset_registry`
  - [ ] Keep existing m_fonts, m_ui_styles for now (migration period)
  - [ ] Document staged loading approach
- [ ] **Test:** Verify AssetManager still compiles

#### 4.2 Refactor LoadDefaultAssets (Staged Approach)
- [ ] **File:** `src/assets/AssetManager.cpp`
  - [ ] Create `LoadStage0Assets()` private method
    - [ ] Get IFontLoader from factory
    - [ ] Call LoadDefaultAssets()
    - [ ] Store fonts in registry AND m_fonts (dual write)
    - [ ] Return success/failure
  - [ ] Create `LoadStage1Assets()` private method
    - [ ] Get IStyleLoader from factory
    - [ ] Call LoadDefaultAssets(*this)
    - [ ] Store styles in registry AND m_ui_styles (dual write)
    - [ ] Return success/failure
  - [ ] Update `LoadDefaultAssets()` to call stages:
    - [ ] Call LoadStage0Assets()
    - [ ] Call LoadStage1Assets()
    - [ ] Return combined result
- [ ] **Test:** `tests/unit/assets/AssetManager.test.cpp`
  - [ ] Test LoadDefaultAssets succeeds
  - [ ] Test fonts loaded in Stage 0
  - [ ] Test styles loaded in Stage 1
  - [ ] Test styles can access fonts
  - [ ] Test error in Stage 0 prevents Stage 1
  - [ ] Test dual write: both registry and old maps populated

#### 4.3 Update GetFont to Use Registry
- [ ] **File:** `src/assets/AssetManager.cpp`
  - [ ] Update `GetFont()`:
    - [ ] Change: Search m_fonts map
    - [ ] To: `return m_asset_registry.GetFont(font_name)`
  - [ ] Add comment: Uses registry, m_fonts maintained for compatibility
- [ ] **Test:** `tests/unit/assets/AssetManager.test.cpp`
  - [ ] Test GetFont returns correct font
  - [ ] Test GetFont returns error for missing font
  - [ ] Test GetFont works after LoadDefaultAssets

#### 4.4 Update GetAllFonts to Use Registry
- [ ] **File:** `src/assets/AssetManager.cpp`
  - [ ] Update `GetAllFonts()`:
    - [ ] Return: `m_asset_registry.GetAllFonts()`
- [ ] **Test:** Test GetAllFonts returns correct map

#### 4.5 Update GetAllUIStyles to Use Registry
- [ ] **File:** `src/assets/AssetManager.cpp`
  - [ ] Update `GetAllUIStyles()`:
    - [ ] Return: `m_asset_registry.GetAllUIStyles()`
- [ ] **Test:** Test GetAllUIStyles returns correct map

#### 4.6 Deprecate Old Members (Future Cleanup)
- [ ] **File:** `src/assets/AssetManager.h`
  - [ ] Add comment to m_fonts: "Deprecated - use m_asset_registry"
  - [ ] Add comment to m_ui_styles: "Deprecated - use m_asset_registry"
  - [ ] Plan: Remove in future PR after migration complete

---

### Phase 5: Complete Scene Loading (Medium Risk, 6-8 hours)

**Goal:** Implement functional scene asset loading

#### 5.1 Define Scene Asset Semantics
- [ ] **Document:** Update ASSET_MANAGER_ANALYSIS.md
  - [ ] Define what "scene assets" means:
    - [ ] Scene-specific fonts (if any)
    - [ ] Scene-specific UI styles (if any)
    - [ ] Future: Scene-specific textures, sounds
  - [ ] Define merge strategy:
    - [ ] Option A: Merge (keep defaults + add scene)
    - [ ] Option B: Replace (discard defaults)
    - [ ] **Decision:** Merge with override (scene overrides defaults)

#### 5.2 Implement Scene Font Loading (Stage 0)
- [ ] **File:** `src/assets/AssetManager.cpp`
  - [ ] Create `LoadSceneFontsStage0(SceneType)` private method
    - [ ] Get IFontLoader from factory
    - [ ] Call LoadSceneAssets(scene_type)
    - [ ] For each font: AddOrReplace in registry
    - [ ] Return success/failure
- [ ] **Test:** `tests/unit/assets/AssetManager.test.cpp`
  - [ ] Test scene fonts loaded
  - [ ] Test scene font overrides default font (same name)
  - [ ] Test scene font added to defaults (different name)

#### 5.3 Implement Scene Style Loading (Stage 1)
- [ ] **File:** `src/assets/AssetManager.cpp`
  - [ ] Create `LoadSceneStylesStage1(SceneType)` private method
    - [ ] Get IStyleLoader from factory
    - [ ] Call LoadSceneAssets(scene_type, *this)
    - [ ] For each style: AddOrReplace in registry
    - [ ] Return success/failure
- [ ] **Test:** `tests/unit/assets/AssetManager.test.cpp`
  - [ ] Test scene styles loaded
  - [ ] Test scene style can access fonts (default + scene)
  - [ ] Test scene style overrides default style

#### 5.4 Refactor LoadSceneAssets
- [ ] **File:** `src/assets/AssetManager.cpp`
  - [ ] Update `LoadSceneAssets(SceneType)`:
    - [ ] Call LoadSceneFontsStage0(scene_type)
    - [ ] Call LoadSceneStylesStage1(scene_type)
    - [ ] Return combined result
  - [ ] Remove old placeholder code
- [ ] **Test:** `tests/unit/assets/AssetManager.test.cpp`
  - [ ] Test LoadSceneAssets completes successfully
  - [ ] Test staged loading order correct
  - [ ] Test merge strategy works correctly

#### 5.5 Add AddOrReplace to AssetRegistry
- [ ] **File:** `src/assets/AssetRegistry.h`
  - [ ] Add: `AddOrReplaceFont(string, Font)`
  - [ ] Add: `AddOrReplaceUIStyle(UIStyle)`
- [ ] **File:** `src/assets/AssetRegistry.cpp`
  - [ ] Implement AddOrReplaceFont:
    - [ ] If exists: replace value
    - [ ] If not exists: insert new
    - [ ] Always return success
  - [ ] Implement AddOrReplaceUIStyle: Same pattern
- [ ] **Test:** `tests/unit/assets/AssetRegistry.test.cpp`
  - [ ] Test AddOrReplace with new key (inserts)
  - [ ] Test AddOrReplace with existing key (replaces)
  - [ ] Test value actually replaced

#### 5.6 Integration Testing
- [ ] **Test:** `tests/integration/assets/scene_asset_loading.integration.test.cpp`
  - [ ] Test complete flow: LoadDefaultAssets → LoadSceneAssets
  - [ ] Test default font used by default style
  - [ ] Test scene font used by scene style
  - [ ] Test scene font overrides default font
  - [ ] Test GetFont returns scene font after scene load

---

### Phase 6: Fix Error Handling (Medium Risk, 2-4 hours)

**Goal:** Make error handling consistent with std::expected

#### 6.1 Update GetDefaultUIStyle
- [ ] **File:** `src/assets/AssetManager.h`
  - [ ] Change signature:
    - [ ] From: `const UIStyle& GetDefaultUIStyle() const`
    - [ ] To: `std::expected<const UIStyle&, FailInfo> GetDefaultUIStyle() const`
- [ ] **File:** `src/assets/AssetManager.cpp`
  - [ ] Update implementation:
    - [ ] Remove: `throw std::runtime_error(...)`
    - [ ] Use: `return m_asset_registry.GetUIStyle("default")`
- [ ] **Test:** `tests/unit/assets/AssetManager.test.cpp`
  - [ ] Test GetDefaultUIStyle with existing default (success)
  - [ ] Test GetDefaultUIStyle with missing default (error)

#### 6.2 Find All Callers of GetDefaultUIStyle
- [ ] **Search:** `grep -r "GetDefaultUIStyle" --include="*.cpp" --include="*.h"`
  - [ ] Make list of all files calling this method

#### 6.3 Update All Call Sites
For each file found:
- [ ] **File:** `{filename}`
  - [ ] Change: `const UIStyle &style = asset_manager.GetDefaultUIStyle()`
  - [ ] To:
    ```cpp
    auto style_result = asset_manager.GetDefaultUIStyle();
    if (!style_result.has_value()) {
      // Handle error appropriately
    }
    const UIStyle &style = style_result.value();
    ```
  - [ ] Update tests for this file

---

### Phase 7: Documentation and Cleanup

#### 7.1 Update Documentation
- [ ] **File:** `documentation/analysis/ASSET_MANAGER_ANALYSIS.md`
  - [ ] Add "Implementation Status" section
  - [ ] Mark completed phases
- [ ] **File:** GitHub Copilot instructions (if applicable)
  - [ ] Document new AssetRegistry pattern
  - [ ] Document staged loading approach
  - [ ] Add examples for future asset types

#### 7.2 Code Review
- [ ] Run full test suite
- [ ] Check for memory leaks (valgrind, if available)
- [ ] Review all new code for style consistency
- [ ] Verify all Doxygen comments complete

---

## Testing Summary

### Unit Tests (Per Phase)
- **Phase 1:** 5 test files, ~50 test cases
- **Phase 2:** 3 test files, ~40 test cases
- **Phase 3:** Update 3 files, ~20 test cases
- **Phase 4:** 1 test file, ~30 test cases
- **Phase 5:** 2 test files, ~40 test cases
- **Phase 6:** 1 test file + updates, ~15 test cases

**Total:** ~195 unit test cases

### Integration Tests
- [ ] Asset loading flow (defaults)
- [ ] Asset loading flow (scene-specific)
- [ ] Font provider integration
- [ ] Style configuration integration

**Total:** ~4 integration test files, ~20 test cases

### Regression Tests
- [ ] Existing AssetManager tests still pass
- [ ] Existing scene loading still works
- [ ] Existing UI rendering still works

---

## Verification Checklist

After each phase:
- [ ] All unit tests pass
- [ ] No new compiler warnings
- [ ] Code follows style guide
- [ ] Doxygen comments complete
- [ ] Changes documented in commit messages

After all phases:
- [ ] Full test suite passes
- [ ] Integration tests pass
- [ ] No memory leaks
- [ ] Performance acceptable
- [ ] Code reviewed and approved
- [ ] Documentation updated

---

## Estimated Total Effort

| Phase | Effort | Testing | Total |
|-------|--------|---------|-------|
| Phase 1 | 4 hrs | 2 hrs | 6 hrs |
| Phase 2 | 6 hrs | 3 hrs | 9 hrs |
| Phase 3 | 3 hrs | 2 hrs | 5 hrs |
| Phase 4 | 6 hrs | 3 hrs | 9 hrs |
| Phase 5 | 5 hrs | 3 hrs | 8 hrs |
| Phase 6 | 2 hrs | 1 hr | 3 hrs |
| Phase 7 | 2 hrs | - | 2 hrs |

**Total: 42 hours** (approximately 1 week for dedicated work)

---

## Document Metadata

**Created:** 2025-12-24  
**Author:** GitHub Copilot Agent  
**Purpose:** Detailed implementation TODO list  
**Status:** Ready for implementation  
**Related:**
- ASSET_MANAGER_ANALYSIS.md
- ASSET_MANAGER_MIGRATION_GUIDE.md
- ASSET_MANAGER_WORKFLOWS.md
