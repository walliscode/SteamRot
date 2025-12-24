# AssetManager Analysis and Refactoring Plan

## Overview

This document provides a comprehensive analysis of the current AssetManager implementation, identifies responsibilities and concerns, proposes clearer separation of concerns, and provides workflow diagrams for the current and proposed architecture.

**Date:** 2025-12-24  
**Purpose:** Analyze AssetManager state and provide refactoring recommendations  
**Status:** Analysis Complete

---

## Table of Contents

1. [Current State Analysis](#current-state-analysis)
2. [Responsibility Analysis](#responsibility-analysis)
3. [Current Workflows (Diagrams)](#current-workflows-diagrams)
4. [Identified Issues](#identified-issues)
5. [Proposed Architecture](#proposed-architecture)
6. [Proposed Workflows (Diagrams)](#proposed-workflows-diagrams)
7. [Migration Strategy](#migration-strategy)
8. [Recommendations](#recommendations)

---

## Current State Analysis

### Class Overview

**Location:** `src/assets/AssetManager.h`, `src/assets/AssetManager.cpp`

**Current Responsibilities:**
1. **Font Management:** Loading, storing, and providing access to fonts
2. **UI Style Management:** Loading, storing, and providing access to UI styles
3. **Asset Data Loading:** Coordinating with data providers to load asset configurations
4. **Scene Asset Management:** Loading scene-specific assets
5. **Font Provider Interface:** Implements `IFontProvider` for font access
6. **UIStyle Configuration:** Creating and configuring UI styles via `FlatbuffersUIStyleDataProvider`

### Dependencies

**Direct Dependencies:**
- `DataAccessFactory` - Factory for obtaining data providers
- `IAssetDataProvider` - Interface for loading asset configuration data
- `FlatbuffersUIStyleDataProvider` - Concrete provider for UI style configuration
- `IFontProvider` - Interface that AssetManager implements
- SFML `sf::Font` - Font resource type
- `UIStyle` - UI style data structure

**Dependents (Who Uses AssetManager):**
- `EngineResources` - Owns AssetManager instance
- `GameContext` - Provides reference to AssetManager
- `SceneContext` - Provides reference to AssetManager
- `SceneManager` - Calls `LoadSceneAssets()`
- All Logic classes and systems that need fonts or UI styles

### Current Data Flow

```
DataAccessFactory
        ↓
   IAssetDataProvider (FlatbuffersAssetDataProvider)
        ↓
    AssetData (fonts, ui_styles list)
        ↓
   AssetManager
        ↓
   ├─ Font Loading (from filesystem)
   ├─ Font Storage (m_fonts map)
   ├─ UIStyle Configuration (FlatbuffersUIStyleDataProvider)
   └─ UIStyle Storage (m_ui_styles map)
```

### Current Methods

| Method | Purpose | Visibility |
|--------|---------|------------|
| `AssetManager(DataAccessFactory&)` | Constructor | Public |
| `LoadDefaultAssets()` | Load default fonts and UI styles | Public |
| `LoadSceneAssets(SceneType)` | Load scene-specific assets | Public |
| `LoadUIStyles()` | Load all UI styles | Public |
| `AddFont(string)` | Load and add a single font | Private |
| `GetFont(string)` | Get font by name (IFontProvider) | Public |
| `GetAllFonts()` | Get entire font map | Public |
| `GetDefaultUIStyle()` | Get default UI style | Public |
| `GetAllUIStyles()` | Get entire UI styles map | Public |

---

## Responsibility Analysis

### Current Responsibilities (Too Many!)

AssetManager currently handles **6 distinct responsibilities**:

1. **Asset Registry Management**
   - Maintains maps of fonts (`m_fonts`)
   - Maintains maps of UI styles (`m_ui_styles`)
   - Provides access to these resources

2. **Font Loading**
   - Reads font files from filesystem
   - Creates `sf::Font` objects
   - Configures font properties (e.g., `setSmooth(false)`)

3. **Asset Configuration Loading**
   - Coordinates with `DataAccessFactory`
   - Obtains `IAssetDataProvider`
   - Loads `AssetData` configuration

4. **UI Style Configuration**
   - Creates `FlatbuffersUIStyleDataProvider`
   - Passes font map to the provider
   - Receives configured `UIStyle` objects
   - Stores styles in map

5. **Font Provider Interface**
   - Implements `IFontProvider` for other systems
   - Provides font lookup by name

6. **Scene-Specific Asset Management**
   - Loads scene-specific asset configurations
   - (Currently placeholder - does not load actual scene assets)

### Single Responsibility Principle Violations

**Problem:** AssetManager violates SRP by combining multiple concerns:
- **Registry/Storage** (data structure management)
- **Loading** (I/O operations)
- **Configuration** (coordinating with providers)
- **Access** (providing interface to consumers)

**Impact:**
- Hard to test individual concerns
- Changes to one aspect affect others
- Difficult to swap implementations
- Complex dependencies

---

## Current Workflows (Diagrams)

### Workflow 1: Startup Asset Loading

```
┌─────────────────────────────────────────────────────────────────┐
│  Engine Startup                                                 │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────────────┐
│  EngineResources Construction                                   │
│  - Creates DataAccessFactory                                    │
│  - Creates AssetManager(data_access_factory)                    │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────────────┐
│  AssetManager::LoadDefaultAssets()                              │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 ├─────────────────────────────────────────────────┐
                 │                                                 │
                 ▼                                                 ▼
┌────────────────────────────────┐    ┌────────────────────────────────┐
│  Load Font Configuration       │    │  Load UI Style Configuration   │
│                                │    │                                │
│  1. Get IAssetDataProvider     │    │  1. Call LoadUIStyles()        │
│  2. LoadAssetData()            │    │  2. Create UIStyleProvider     │
│  3. For each FontData:         │    │  3. Pass m_fonts to provider   │
│     - AddFont(font_name)       │    │  4. ProvideUIStyles()          │
│     - Load from filesystem     │    │  5. Store in m_ui_styles       │
│     - Store in m_fonts         │    │                                │
└────────────────────────────────┘    └────────────────────────────────┘
```

### Workflow 2: Scene Asset Loading

```
┌─────────────────────────────────────────────────────────────────┐
│  SceneManager::ChangeScene(scene_type)                          │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────────────┐
│  AssetManager::LoadSceneAssets(scene_type)                      │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────────────┐
│  1. Get IAssetDataProvider from DataAccessFactory               │
│  2. LoadSceneAssetData(scene_type)                              │
│  3. Returns AssetData (fonts, ui_styles)                        │
│  4. Currently: No additional loading (placeholder)              │
│     - Does NOT load scene-specific fonts                        │
│     - Does NOT load scene-specific textures                     │
│     - Does NOT load scene-specific sounds                       │
└─────────────────────────────────────────────────────────────────┘
```

### Workflow 3: Font Access by Consumers

```
┌─────────────────────────────────────────────────────────────────┐
│  Consumer (Logic, System, Configurator)                         │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────────────┐
│  IFontProvider::GetFont(font_name)                              │
│  (via AssetManager reference)                                   │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────────────┐
│  AssetManager::GetFont(font_name)                               │
│  1. Search m_fonts map                                          │
│  2. Return shared_ptr<Font> if found                            │
│  3. Return FailInfo if not found                                │
└─────────────────────────────────────────────────────────────────┘
```

### Workflow 4: UI Style Configuration

```
┌─────────────────────────────────────────────────────────────────┐
│  AssetManager::LoadUIStyles()                                   │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────────────┐
│  Create FlatbuffersUIStyleDataProvider(m_fonts)                 │
│  - Passes AssetManager's font map                               │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────────────┐
│  FlatbuffersUIStyleDataProvider::ProvideUIStyles()              │
│  1. Create FlatbuffersDataLoader                                │
│  2. Load UIStyleData from FlatBuffers                           │
│  3. For each style:                                             │
│     a. ConfigureBaseStyle (colors, sizes, margins)              │
│     b. ConfigurePanelStyle                                      │
│     c. ConfigureButtonStyle (with font lookup)                  │
│     d. ConfigureDropDownStyles (with font lookups)              │
│  4. Return vector<UIStyle>                                      │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────────────┐
│  AssetManager stores UIStyles in m_ui_styles map                │
└─────────────────────────────────────────────────────────────────┘
```

---

## Identified Issues

### Issue 1: Tight Coupling with FlatbuffersUIStyleDataProvider

**Problem:**
- AssetManager directly creates `FlatbuffersUIStyleDataProvider`
- Hard-coded to FlatBuffers implementation
- No abstraction for UI style loading

**Code:**
```cpp
std::expected<std::monostate, FailInfo> AssetManager::LoadUIStyles() {
  // Direct instantiation - hard-coded dependency
  FlatbuffersUIStyleDataProvider ui_style_provider(m_fonts);
  auto ui_style_data_result = ui_style_provider.ProvideUIStyles();
  // ...
}
```

**Impact:**
- Cannot swap UI style loading implementation
- Violates Dependency Inversion Principle
- Inconsistent with other providers (IAssetDataProvider pattern)

**Inconsistency:**
- Fonts use `IAssetDataProvider` → `FlatbuffersAssetDataProvider`
- UI Styles directly use `FlatbuffersUIStyleDataProvider` (no abstraction)

### Issue 2: Direct Font Map Passing

**Problem:**
- AssetManager passes its internal `m_fonts` map to `FlatbuffersUIStyleDataProvider`
- Breaks encapsulation
- UIStyleProvider directly accesses AssetManager internals

**Code:**
```cpp
// AssetManager.cpp
FlatbuffersUIStyleDataProvider ui_style_provider(m_fonts);

// FlatbuffersUIStyleDataProvider.h
class FlatbuffersUIStyleDataProvider : public IUIStyleDataProvider {
protected:
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> &fonts_map;
  // Direct reference to AssetManager's private member!
};
```

**Impact:**
- UIStyleProvider can modify AssetManager's font map
- Tight coupling between AssetManager and UIStyleProvider
- Violates encapsulation

**Better Alternative:**
- Pass `IFontProvider` interface instead
- UIStyleProvider uses `GetFont()` method
- AssetManager internals remain private

### Issue 3: Mixed Responsibilities in LoadDefaultAssets

**Problem:**
- Single method handles both font loading AND UI style loading
- Different concerns mixed together
- Hard to test independently

**Code:**
```cpp
std::expected<std::monostate, FailInfo> AssetManager::LoadDefaultAssets() {
  // Font loading logic
  for (const auto &font_data : asset_data.fonts) {
    auto add_font_result = AddFont(font_data.name);
    // ...
  }
  
  // UI style loading logic
  auto load_ui_style_result = LoadUIStyles();
  // ...
}
```

**Impact:**
- Cannot load fonts without loading UI styles
- Cannot test font loading in isolation
- Unclear separation of concerns

### Issue 4: Scene Asset Loading is Incomplete

**Problem:**
- `LoadSceneAssets()` does not actually load scene-specific resources
- Only loads configuration data, does nothing with it
- Misleading method name

**Code:**
```cpp
std::expected<std::monostate, FailInfo>
AssetManager::LoadSceneAssets(const SceneType &scene_type) {
  auto asset_data_result = asset_provider.LoadSceneAssetData(scene_type);
  if (!asset_data_result.has_value())
    return std::unexpected<FailInfo>(asset_data_result.error());

  // Scene assets are loaded on demand, return success
  return std::monostate();  // Does nothing with the data!
}
```

**Questions:**
- Should scene-specific fonts be loaded here?
- Should scene-specific UI styles be loaded here?
- Should textures/sounds be loaded here?
- What does "scene asset" mean?

### Issue 5: GetDefaultUIStyle() Throws Exception

**Problem:**
- Inconsistent error handling
- Most methods return `std::expected<T, FailInfo>`
- `GetDefaultUIStyle()` throws `std::runtime_error`

**Code:**
```cpp
const UIStyle &AssetManager::GetDefaultUIStyle() const {
  auto it = m_ui_styles.find("default");
  if (it != m_ui_styles.end()) {
    return it->second;
  } else {
    throw std::runtime_error("Default UIStyle not found");  // Inconsistent!
  }
}
```

**Impact:**
- Violates error handling conventions
- Callers must use try/catch instead of checking expected
- Inconsistent with rest of codebase

### Issue 6: AddFont() Performs I/O

**Problem:**
- `AddFont()` directly reads from filesystem
- Mixes I/O logic with asset management
- Hard to test without filesystem
- Hard-coded file extension (`.ttf`)

**Code:**
```cpp
std::expected<std::monostate, FailInfo>
AssetManager::AddFont(const std::string &font_name) {
  std::filesystem::path font_dir = paths::GetFontsDirectory();
  std::string font_file_name = font_name + ".ttf";  // Hard-coded!
  std::filesystem::path font_path = font_dir / font_file_name;
  
  if (!std::filesystem::exists(font_path)) { /* error */ }
  
  sf::Font font;
  bool success = font.openFromFile(font_path);  // I/O operation
  // ...
}
```

**Impact:**
- Cannot test font management without files
- Cannot support other font formats easily
- Mixes concerns (registry + I/O)

---

## Proposed Architecture

### Architectural Principles

**1. Separation of Concerns**
- **Registry:** Store and provide access to loaded assets
- **Loading:** Read assets from sources (files, network, etc.)
- **Configuration:** Transform raw data to usable assets
- **Provision:** Provide interfaces for asset access

**2. Dependency Inversion**
- AssetManager depends on abstractions, not concrete implementations
- Loaders and configurators depend on interfaces
- Factory pattern for creating concrete loaders

**3. Single Responsibility**
- Each class has one clear purpose
- Classes are easy to test in isolation
- Changes to one concern don't affect others

### Proposed Class Structure

```
┌─────────────────────────────────────────────────────────────────┐
│  AssetManager (Registry & Access)                               │
│  - Stores asset maps (fonts, styles, textures, sounds)          │
│  - Provides access via interfaces (IFontProvider, etc.)         │
│  - Delegates loading to specialized loaders                     │
│  - Delegates configuration to specialized configurators         │
└─────────────────────────────────────────────────────────────────┘
                         │
         ┌───────────────┼───────────────┐
         │               │               │
         ▼               ▼               ▼
┌──────────────┐  ┌──────────────┐  ┌──────────────┐
│ IFontLoader  │  │ IStyleLoader │  │ITextureLoader│
│ (Interface)  │  │ (Interface)  │  │ (Interface)  │
└──────────────┘  └──────────────┘  └──────────────┘
         │               │               │
         ▼               ▼               ▼
┌──────────────┐  ┌──────────────┐  ┌──────────────┐
│ Flatbuffers  │  │ Flatbuffers  │  │  SFML        │
│ FontLoader   │  │ StyleLoader  │  │TextureLoader │
└──────────────┘  └──────────────┘  └──────────────┘
```

### Proposed Components

**1. AssetRegistry (New)**
- Pure storage of asset maps
- No loading logic
- No I/O operations
- Simple add/get/remove methods

**2. IAssetLoader (New Interface)**
- Generic interface for loading assets
- Implementations: FontLoader, StyleLoader, TextureLoader
- Returns loaded assets or error

**3. AssetManager (Refactored)**
- Owns AssetRegistry
- Coordinates loaders
- Implements provider interfaces
- No direct I/O or configuration

**4. FontLoader (New)**
- Loads fonts from filesystem
- Handles font file formats
- Configures font properties
- Returns loaded Font objects

**5. StyleLoader (New)**
- Loads UI styles from data source
- Uses IFontProvider for font references
- Configures style objects
- Returns loaded UIStyle objects

### Responsibility Distribution

| Component | Responsibilities |
|-----------|-----------------|
| **AssetRegistry** | Store assets, provide access |
| **AssetManager** | Coordinate loading, implement interfaces |
| **IAssetLoader** | Load assets from sources |
| **FontLoader** | Font file I/O, font configuration |
| **StyleLoader** | Style configuration, font references |
| **DataAccessFactory** | Provide concrete loaders |

---

## Proposed Workflows (Diagrams)

### Proposed Workflow 1: Startup Asset Loading

```
┌─────────────────────────────────────────────────────────────────┐
│  Engine Startup                                                 │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────────────┐
│  EngineResources Construction                                   │
│  - Creates DataAccessFactory                                    │
│  - Creates AssetManager(data_access_factory)                    │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────────────┐
│  AssetManager::LoadDefaultAssets()                              │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 ├──────────────────────────────┬──────────────────┐
                 │                              │                  │
                 ▼                              ▼                  ▼
┌────────────────────────┐  ┌────────────────────────┐  ┌─────────────────┐
│  Load Default Fonts    │  │  Load Default Styles   │  │  Load Default   │
│                        │  │                        │  │  Textures       │
│  1. Get IFontLoader    │  │  1. Get IStyleLoader   │  │  (Future)       │
│  2. LoadDefaultFonts() │  │  2. Pass IFontProvider │  │                 │
│  3. Store in registry  │  │  3. LoadDefaultStyles()│  │                 │
│                        │  │  4. Store in registry  │  │                 │
└────────────────────────┘  └────────────────────────┘  └─────────────────┘
         │                              │
         ▼                              ▼
┌────────────────────────┐  ┌────────────────────────────────────┐
│  FontLoader            │  │  StyleLoader                       │
│  1. Get IAssetData     │  │  1. Get IStyleDataProvider         │
│  2. For each font:     │  │  2. ProvideUIStyles()              │
│     - Read from file   │  │  3. Use IFontProvider.GetFont()    │
│     - Configure Font   │  │  4. Return configured UIStyles     │
│     - Return Font      │  │                                    │
└────────────────────────┘  └────────────────────────────────────┘
```

### Proposed Workflow 2: Scene Asset Loading

```
┌─────────────────────────────────────────────────────────────────┐
│  SceneManager::ChangeScene(scene_type)                          │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────────────┐
│  AssetManager::LoadSceneAssets(scene_type)                      │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 ├──────────────────────┬─────────────────────────┐
                 │                      │                         │
                 ▼                      ▼                         ▼
┌────────────────────────┐  ┌─────────────────────┐  ┌──────────────────┐
│  Load Scene Fonts      │  │  Load Scene Styles  │  │  Load Scene      │
│  (if any)              │  │  (if any)           │  │  Textures        │
│                        │  │                     │  │  (Future)        │
│  1. Get IFontLoader    │  │  1. Get IStyle      │  │                  │
│  2. LoadSceneFonts()   │  │     Loader          │  │  1. Get ITexture │
│  3. Store in registry  │  │  2. LoadScene       │  │     Loader       │
│  4. Merge with default │  │     Styles()        │  │  2. LoadScene    │
│                        │  │  3. Store in reg    │  │     Textures()   │
└────────────────────────┘  └─────────────────────┘  └──────────────────┘
```

### Proposed Workflow 3: Font Access (Unchanged)

```
┌─────────────────────────────────────────────────────────────────┐
│  Consumer (Logic, System, Configurator)                         │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────────────┐
│  IFontProvider::GetFont(font_name)                              │
│  (via AssetManager reference)                                   │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────────────┐
│  AssetManager::GetFont(font_name)                               │
│  1. Delegate to AssetRegistry                                   │
│  2. Return shared_ptr<Font> if found                            │
│  3. Return FailInfo if not found                                │
└─────────────────────────────────────────────────────────────────┘
```

### Proposed Workflow 4: UI Style Configuration

```
┌─────────────────────────────────────────────────────────────────┐
│  AssetManager::LoadUIStyles()                                   │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────────────┐
│  Get IStyleLoader from DataAccessFactory                        │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────────────┐
│  IStyleLoader::LoadDefaultStyles(IFontProvider)                 │
│  - Passes AssetManager as IFontProvider                         │
│  - Does NOT pass internal font map                              │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────────────┐
│  FlatbuffersStyleLoader (Concrete Implementation)               │
│  1. Load UIStyleData from FlatBuffers                           │
│  2. For each style:                                             │
│     a. ConfigureBaseStyle                                       │
│     b. ConfigureButtonStyle                                     │
│        - Call font_provider.GetFont() (interface method)        │
│        - No direct map access                                   │
│  3. Return vector<UIStyle>                                      │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────────────┐
│  AssetManager stores UIStyles in registry                       │
└─────────────────────────────────────────────────────────────────┘
```

---

## Migration Strategy

### Phase 1: Create New Abstractions (Non-Breaking)

**Goal:** Add new interfaces without breaking existing code

**Tasks:**
1. Create `IAssetLoader<T>` interface template
2. Create `IFontLoader : IAssetLoader<sf::Font>` interface
3. Create `IStyleLoader : IAssetLoader<UIStyle>` interface
4. Create `AssetRegistry` class for storage

**Files to Create:**
- `src/interfaces/IAssetLoader.h`
- `src/interfaces/IFontLoader.h`
- `src/interfaces/IStyleLoader.h`
- `src/assets/AssetRegistry.h`
- `src/assets/AssetRegistry.cpp`

**Impact:** Zero - new files, no changes to existing code

### Phase 2: Implement Concrete Loaders (Non-Breaking)

**Goal:** Create concrete loader implementations

**Tasks:**
1. Implement `FontLoader` (moves logic from `AssetManager::AddFont`)
2. Implement `FlatbuffersStyleLoader` (replaces direct `FlatbuffersUIStyleDataProvider` usage)
3. Update `DataAccessFactory` to provide loaders

**Files to Create:**
- `src/loaders/FontLoader.h`
- `src/loaders/FontLoader.cpp`
- `src/loaders/FlatbuffersStyleLoader.h`
- `src/loaders/FlatbuffersStyleLoader.cpp`

**Files to Modify:**
- `src/data_providers/DataAccessFactory.h` (add GetFontLoader, GetStyleLoader)
- `src/data_providers/DataAccessFactory.cpp` (implement methods)

**Impact:** Low - new functionality, existing code unchanged

### Phase 3: Refactor FlatbuffersUIStyleDataProvider (Breaking)

**Goal:** Make UIStyleDataProvider use IFontProvider instead of font map

**Tasks:**
1. Change constructor: `FlatbuffersUIStyleDataProvider(IFontProvider&)` instead of font map
2. Update all font lookups to use `GetFont()` method
3. Update tests

**Files to Modify:**
- `src/data_providers/FlatbuffersUIStyleDataProvider.h`
- `src/data_providers/FlatbuffersUIStyleDataProvider.cpp`
- `src/interfaces/IUIStyleDataProvider.h`
- `tests/unit/data_providers/FlatbuffersUIStyleDataProvider.test.cpp`

**Impact:** Medium - breaks direct coupling, improves encapsulation

### Phase 4: Refactor AssetManager (Breaking)

**Goal:** Use new loaders instead of direct implementation

**Tasks:**
1. Add `AssetRegistry` member to AssetManager
2. Update `LoadDefaultAssets()` to use `IFontLoader` and `IStyleLoader`
3. Update `LoadUIStyles()` to use `IStyleLoader`
4. Remove or deprecate `AddFont()` (now in FontLoader)
5. Update tests

**Files to Modify:**
- `src/assets/AssetManager.h`
- `src/assets/AssetManager.cpp`
- `tests/unit/assets/AssetManager.test.cpp`

**Impact:** High - internal refactoring, but public API remains mostly same

### Phase 5: Implement Scene Asset Loading (New Feature)

**Goal:** Complete `LoadSceneAssets()` functionality

**Tasks:**
1. Define what "scene assets" means (fonts? styles? textures?)
2. Implement scene-specific font loading (if needed)
3. Implement scene-specific style loading (if needed)
4. Add proper registry management (merge vs. replace)

**Files to Modify:**
- `src/assets/AssetManager.cpp` (complete LoadSceneAssets)

**Impact:** Medium - adds new functionality, defines scene asset semantics

### Phase 6: Fix Error Handling Inconsistencies (Bug Fix)

**Goal:** Make error handling consistent

**Tasks:**
1. Change `GetDefaultUIStyle()` to return `std::expected<const UIStyle&, FailInfo>`
2. Update all callers to handle expected
3. Update tests

**Files to Modify:**
- `src/assets/AssetManager.h`
- `src/assets/AssetManager.cpp`
- All files calling `GetDefaultUIStyle()`

**Impact:** Medium - breaking change, but improves consistency

---

## Recommendations

### High Priority (Address Soon)

**1. Fix Encapsulation Violation in UIStyleDataProvider**
- **Issue:** Direct font map passing breaks encapsulation
- **Fix:** Pass `IFontProvider` interface instead
- **Benefit:** Better encapsulation, clearer dependencies
- **Effort:** Medium (Phase 3)

**2. Complete Scene Asset Loading**
- **Issue:** `LoadSceneAssets()` is incomplete and misleading
- **Fix:** Define and implement scene asset loading semantics
- **Benefit:** Feature completeness, clear purpose
- **Effort:** Medium (Phase 5)

**3. Fix Error Handling in GetDefaultUIStyle**
- **Issue:** Throws exception instead of returning expected
- **Fix:** Return `std::expected<const UIStyle&, FailInfo>`
- **Benefit:** Consistency with codebase conventions
- **Effort:** Low (Phase 6)

### Medium Priority (Plan for Future)

**4. Separate Font Loading from AssetManager**
- **Issue:** `AddFont()` mixes I/O with registry
- **Fix:** Create `FontLoader` class
- **Benefit:** Better separation of concerns, easier testing
- **Effort:** Medium (Phase 2)

**5. Create Loader Abstractions**
- **Issue:** No abstraction for asset loaders
- **Fix:** Create `IAssetLoader<T>` interface and implementations
- **Benefit:** Extensibility, testability, consistency
- **Effort:** High (Phase 1-2)

**6. Introduce AssetRegistry**
- **Issue:** AssetManager mixes storage with coordination
- **Fix:** Extract storage to `AssetRegistry` class
- **Benefit:** Single responsibility, clearer architecture
- **Effort:** High (Phase 4)

### Low Priority (Future Enhancement)

**7. Support Multiple Asset Types**
- **Current:** Only fonts and UI styles
- **Future:** Textures, sounds, animations, shaders
- **Approach:** Use loader pattern for each type
- **Benefit:** Scalability for future asset needs
- **Effort:** High (depends on asset types)

**8. Asset Caching and Management**
- **Current:** All assets loaded and kept in memory
- **Future:** LRU cache, lazy loading, unloading unused assets
- **Approach:** Add caching layer in AssetRegistry
- **Benefit:** Memory efficiency for large games
- **Effort:** High (requires profiling and metrics)

---

## Summary

### Current State Issues

1. ✗ Too many responsibilities in AssetManager
2. ✗ Direct coupling with `FlatbuffersUIStyleDataProvider`
3. ✗ Font map passed directly (breaks encapsulation)
4. ✗ I/O mixed with registry management
5. ✗ Scene asset loading incomplete
6. ✗ Inconsistent error handling

### Proposed Improvements

1. ✓ Separate concerns: Registry, Loading, Configuration
2. ✓ Abstract loaders: `IFontLoader`, `IStyleLoader`
3. ✓ Pass interfaces: `IFontProvider` instead of font map
4. ✓ Extract I/O: Move to `FontLoader` class
5. ✓ Complete features: Define and implement scene assets
6. ✓ Consistent errors: Use `std::expected` everywhere

### Benefits of Proposed Architecture

- **Testability:** Each component tested in isolation
- **Flexibility:** Easy to add new asset types
- **Maintainability:** Clear responsibilities, easy to understand
- **Extensibility:** New loaders without changing AssetManager
- **Consistency:** Follows existing patterns in codebase

### Migration Risk Level

- **Phase 1-2:** Low risk (additive changes)
- **Phase 3-4:** Medium risk (refactoring existing code)
- **Phase 5-6:** Low risk (completing features, fixing bugs)

### Recommendation Priority

**Immediate:** Fix encapsulation (Phase 3), Complete scene loading (Phase 5)  
**Near-term:** Create loaders (Phase 1-2), Refactor AssetManager (Phase 4)  
**Long-term:** Support new asset types, Implement caching

---

## Document Metadata

**Created:** 2025-12-24  
**Author:** GitHub Copilot Agent  
**Related Documents:**
- `FONT_PROVIDER_DECOUPLING.md` - IFontProvider interface introduction
- `USER_INTERFACE_PHASES_1_2_IMPLEMENTATION.md` - UI style configuration
- GitHub Copilot Instructions - AssetManager guidelines

**Status:** Analysis Complete  
**Review:** Pending  
**Next Steps:** Discuss with team, prioritize phases, implement changes
