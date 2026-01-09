# Architecture Refactoring Implementation Guide

**Purpose:** Step-by-step guide for implementing the architecture improvements identified in the analysis.

**Prerequisites:** Read [RECOMMENDATIONS_SUMMARY.md](./RECOMMENDATIONS_SUMMARY.md) first for context.

---

## ⚠️ Before You Start

### Preparation Checklist

- [ ] **Backup**: Create a branch from current state: `git checkout -b backup-before-refactor`
- [ ] **Clean Build**: Ensure project builds successfully: `cmake --build --preset Debug`
- [ ] **Clean Tests**: Ensure all tests pass: `ctest --preset Debug`
- [ ] **No Uncommitted Changes**: `git status` should show clean working directory
- [ ] **Review Analysis**: Read RECOMMENDATIONS_SUMMARY.md and ARCHITECTURE_LAYER_ANALYSIS.md

### Implementation Strategy

**Incremental approach:** Each phase is independent and can be committed/tested separately.

**Rollback plan:** If any phase fails, revert with: `git reset --hard HEAD~1`

**Time estimate:** 13-20 hours total (split across multiple sessions)

---

## Phase 1: Data Structure Relocation (1-2 hours)

**Goal:** Move data structures from `interfaces/` to `types/core/`

**Risk:** LOW  
**Impact:** Better organization, clearer separation of concerns

### Step 1.1: Move SceneLoadData.h

```bash
# Create backup branch point
git checkout -b phase1-data-structure-relocation

# Move the file
mkdir -p src/types/core
mv src/interfaces/SceneLoadData.h src/types/core/

# Update CMakeLists.txt if needed (types is INTERFACE, just include directory)
# No changes needed since types already includes core/
```

### Step 1.2: Update All Includes

Find all files that include SceneLoadData.h:

```bash
# Find all occurrences
grep -r "SceneLoadData.h" src/ tests/ --include="*.cpp" --include="*.h"
```

Update each file:

**Before:**
```cpp
#include "SceneLoadData.h"
```

**After:**
```cpp
#include "SceneLoadData.h"  // Path unchanged if using include directories correctly
```

**Files to update** (based on analysis):
- `src/interfaces/ISceneLoadDataProvider.h`
- `src/data_providers/FlatbuffersSceneLoadDataProvider.h`
- `src/data_providers/FlatbuffersSceneLoadDataProvider.cpp`
- `src/scenes/SceneFactory.h`
- `src/scenes/SceneFactory.cpp`
- Any test files

**Script to help:**
```bash
# Find and list all files
find src tests -type f \( -name "*.cpp" -o -name "*.h" \) -exec grep -l "SceneLoadData.h" {} \;

# For each file, verify the include is correct
# The include path may not need to change if using proper include directories
```

### Step 1.3: Move SceneInfoProvider.h

```bash
# Move the file
mv src/interfaces/SceneInfoProvider.h src/types/core/

# Find all occurrences
grep -r "SceneInfoProvider.h" src/ tests/ --include="*.cpp" --include="*.h"
```

Update includes in:
- `src/scenes/SceneManager.h`
- `src/scenes/SceneManager.cpp`
- Any test files

### Step 1.4: Build and Test

```bash
# Clean build directory
rm -rf build/

# Reconfigure
cmake --preset Debug

# Build
cmake --build --preset Debug

# If build succeeds, run tests
ctest --preset Debug

# If all tests pass, commit
git add -A
git commit -m "Phase 1: Move SceneLoadData and SceneInfoProvider to types/core

- Moved SceneLoadData.h from interfaces/ to types/core/
- Moved SceneInfoProvider.h from interfaces/ to types/core/
- Updated all includes to reflect new location
- All tests passing"
```

### Step 1.5: Verify Changes

```bash
# Check file locations
ls -la src/types/core/SceneLoadData.h
ls -la src/types/core/SceneInfoProvider.h

# Verify old locations removed
ls src/interfaces/SceneLoadData.h 2>&1 | grep "No such file"
ls src/interfaces/SceneInfoProvider.h 2>&1 | grep "No such file"

# Run targeted tests
ctest --preset Debug -R "Scene|Data" --verbose
```

---

## Phase 2: Header-Only Components (4-6 hours) ⭐

**Goal:** Convert components library to header-only and move to types/components/

**Risk:** MEDIUM  
**Impact:** ~20% build time improvement, clearer architecture

**Important Note on Circular Dependencies:**

The original recommendation to make `GetComponentRegisterIndex()` inline in headers creates a circular dependency:
- Component headers would need `containers.h` for `TupleTypeIndex`  
- But `containers.h` includes all component headers for `ComponentRegister`

**Solution:** Remove `GetComponentRegisterIndex()` entirely and use `TupleTypeIndex` directly. This is cleaner, avoids circular dependencies, and is entirely compile-time (no virtual call overhead).

### Step 2.1: Create Target Directory Structure

```bash
# Create new branch
git checkout -b phase2-header-only-components

# Create directory for components in types
mkdir -p src/types/components

# Verify types/CMakeLists.txt includes this directory
# Should already be set to include subdirectories
```

### Step 2.2: Remove GetComponentRegisterIndex() to Avoid Circular Dependencies

**Problem:** Making `GetComponentRegisterIndex()` inline in headers creates circular dependencies:
- Component headers would need `containers.h` for `TupleTypeIndex`
- But `containers.h` includes all component headers for `ComponentRegister`

**Solution:** Eliminate `GetComponentRegisterIndex()` entirely and use `TupleTypeIndex` directly where needed.

**File:** `src/components/Component.h`

**Before:**
```cpp
struct Component {
  virtual ~Component();
  virtual size_t GetComponentRegisterIndex() const = 0;
  bool m_active{false};
};
```

**After:**
```cpp
struct Component {
  virtual ~Component() = default;  // Inline, no .cpp needed
  // GetComponentRegisterIndex() removed - use TupleTypeIndex directly
  bool m_active{false};
};
```

**Action:**
```bash
# Edit Component.h - remove virtual method, inline destructor
vi src/components/Component.h

# Delete Component.cpp (no longer needed)
rm src/components/Component.cpp
```

### Step 2.3: Remove GetComponentRegisterIndex() from All Components

For **each component**, simply remove the method declaration and its implementation:

#### Example: CMeta

**File:** `src/components/CMeta.h`

**Before:**
```cpp
struct CMeta : public Component {
  CMeta() = default;
  bool m_entity_alive = false;
  size_t GetComponentRegisterIndex() const override;
};
```

**After:**
```cpp
struct CMeta : public Component {
  CMeta() = default;
  bool m_entity_alive = false;
  // GetComponentRegisterIndex() removed
};
```

**Action:**
```bash
# Edit CMeta.h - remove method declaration
vi src/components/CMeta.h

# Delete CMeta.cpp (entire file no longer needed)
rm src/components/CMeta.cpp
```

**Repeat for all components:**
- `CUserInterface.h` / `CUserInterface.cpp` - remove method, delete .cpp
- `CMachinaForm.h` / `CMachinaForm.cpp` - remove method, delete .cpp
- `CGrimoireMachina.h` / `CGrimoireMachina.cpp` - remove method, delete .cpp
- `CUIState.h` / `CUIState.cpp` - remove method, delete .cpp

### Step 2.4: Update ArchetypeManager to Use TupleTypeIndex Directly

Since we removed `GetComponentRegisterIndex()`, we need to update the one place it's used in `ArchetypeManager.cpp`.

**File:** `src/entity/ArchetypeManager.cpp`

**Find this code (around line 29-38):**
```cpp
std::apply(
    [&](const auto &...component_vector) {
      // for each component vector, check if the entity has that component
      ((archetypeID.set(
           component_vector[entity_index].GetComponentRegisterIndex(),
           component_vector[entity_index].m_active)),
       ...);
    },
    m_entity_memory_pool);
```

**Replace with:**
```cpp
std::apply(
    [&]<typename... ComponentTypes>(
        const std::vector<ComponentTypes> &...component_vector) {
      // for each component vector, check if the entity has that component
      // Use TupleTypeIndex directly instead of GetComponentRegisterIndex()
      ((archetypeID.set(
           TupleTypeIndex<ComponentTypes, ComponentRegister>,
           component_vector[entity_index].m_active)),
       ...);
    },
    m_entity_memory_pool);
```

**Explanation:**
- Changed lambda to use C++20 template lambda syntax: `[&]<typename... ComponentTypes>`
- This captures the actual component type for each vector
- Use `TupleTypeIndex<ComponentTypes, ComponentRegister>` directly to get the index
- No need for virtual method call - everything is compile-time!

**Action:**
```bash
# Edit ArchetypeManager.cpp
vi src/entity/ArchetypeManager.cpp
```

### Step 2.5: Move containers.h

```bash
# Move containers.h (it's already header-only)
# No changes needed to content
mv src/components/containers.h src/types/components/

# Update include in containers.h to use relative paths for components
vi src/types/components/containers.h
```

**Edit containers.h includes:**

**Before:**
```cpp
#include "CGrimoireMachina.h"
#include "CMachinaForm.h"
#include "CMeta.h"
#include "CUIState.h"
#include "CUserInterface.h"
```

**After:**
```cpp
// Include paths remain the same if using proper include directories
// Or use relative paths if needed:
#include "CGrimoireMachina.h"
#include "CMachinaForm.h"
#include "CMeta.h"
#include "CUIState.h"
#include "CUserInterface.h"
```

### Step 2.6: Move All Component Headers

```bash
# Move all component headers to types/components/
mv src/components/Component.h src/types/components/
mv src/components/CMeta.h src/types/components/
mv src/components/CUserInterface.h src/types/components/
mv src/components/CMachinaForm.h src/types/components/
mv src/components/CGrimoireMachina.h src/types/components/
mv src/components/CUIState.h src/types/components/

# Verify components/ only has CMakeLists.txt left
ls src/components/
```

### Step 2.7: Update types/CMakeLists.txt

**File:** `src/types/CMakeLists.txt`

**Add include directory for components:**

```cmake
target_include_directories(types
  INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/user_interface/
  ${CMAKE_CURRENT_SOURCE_DIR}/user_interface/styles/
  ${CMAKE_CURRENT_SOURCE_DIR}/core/
  ${CMAKE_CURRENT_SOURCE_DIR}/components/      # ADD THIS LINE
  ${CMAKE_CURRENT_SOURCE_DIR}/events/
  ${CMAKE_CURRENT_SOURCE_DIR}/assets/
  ${CMAKE_CURRENT_SOURCE_DIR}/test_structs/
  ${CMAKE_CURRENT_SOURCE_DIR}/flatbuffers/generated/
)
```

### Step 2.8: Remove components Library

**File:** `src/components/CMakeLists.txt`

**Delete or comment out entirely:**

```cmake
# Components are now header-only in types/components/
# This library is no longer needed
```

**File:** `src/CMakeLists.txt`

**Remove components subdirectory:**

```cmake
add_subdirectory(assets)
# add_subdirectory(components)  # REMOVE THIS LINE
add_subdirectory(configuration)
# ... rest of file
```

### Step 2.9: Update All Component Includes

Find all files that include component headers:

```bash
# Find all component includes
grep -r "#include \"C.*\.h\"" src/ tests/ --include="*.cpp" --include="*.h" | grep -E "(CMeta|CUserInterface|CMachinaForm|CGrimoireMachina|CUIState)" > component_includes.txt

# Review the file
cat component_includes.txt
```

**Most includes won't need to change** if include directories are set correctly, but verify:

```bash
# Test compile a few files that include components
cmake --build --preset Debug --target <some-target-that-uses-components>
```

### Step 2.10: Update Dependent Libraries

Remove `components` from target_link_libraries in these CMakeLists.txt:

**Files to update:**
- `src/entity/CMakeLists.txt` - Remove `components` from target_link_libraries
- `src/context/CMakeLists.txt` - Remove `components` from target_link_libraries
- `src/logic/CMakeLists.txt` - Remove `components` from target_link_libraries
- `src/scenes/CMakeLists.txt` - Remove `components` from target_link_libraries

**Before:**
```cmake
target_link_libraries(entity
  PUBLIC
  SFML::Graphics
  components      # REMOVE THIS
  types
  interfaces
  events
)
```

**After:**
```cmake
target_link_libraries(entity
  PUBLIC
  SFML::Graphics
  types           # Components now included via types
  interfaces
  events
)
```

### Step 2.11: Build and Test

```bash
# Clean build
rm -rf build/

# Reconfigure
cmake --preset Debug

# Build
cmake --build --preset Debug

# Should see significantly fewer compilation units
# Components are no longer compiled as separate .cpp files

# Run all tests
ctest --preset Debug

# If all pass, commit
git add -A
git commit -m "Phase 2: Convert components to header-only in types/components

- Made Component destructor inline (= default)
- Removed GetComponentRegisterIndex() virtual method (eliminates circular dependency)
- Updated ArchetypeManager to use TupleTypeIndex directly with template lambda
- Removed all component .cpp files
- Moved all component headers to types/components/
- Moved containers.h to types/components/
- Removed components library from build system
- Updated all dependent libraries to use types instead of components
- Expected ~20% build time improvement"
```

### Step 2.12: Verify Build Time Improvement

```bash
# Time a clean build before (from backup branch)
git checkout backup-before-refactor
rm -rf build/
time cmake --preset Debug && time cmake --build --preset Debug

# Time a clean build after (from new branch)
git checkout phase2-header-only-components
rm -rf build/
time cmake --preset Debug && time cmake --build --preset Debug

# Compare times
```

---

## Phase 3: Foundation Consolidation (2-4 hours)

**Goal:** Merge types, interfaces, configuration, resources into single types library

**Risk:** MEDIUM  
**Impact:** Eliminates circular dependencies, simpler architecture

### Step 3.1: Create Unified Directory Structure

```bash
# Create new branch
git checkout -b phase3-foundation-consolidation

# Create interfaces subdirectory in types
mkdir -p src/types/interfaces

# Create configuration subdirectory in types (or reuse interfaces)
# We'll put configuration interfaces in types/interfaces/
```

### Step 3.2: Move Interface Files

```bash
# Move all interface files from interfaces/ to types/interfaces/
mv src/interfaces/*.h src/types/interfaces/

# Verify
ls src/types/interfaces/
```

### Step 3.3: Move Configuration Files

```bash
# Move configuration interface files to types/interfaces/
mv src/configuration/IUIElementConfigurator.h src/types/interfaces/
mv src/configuration/IUIStyleConfigurator.h src/types/interfaces/

# Verify
ls src/types/interfaces/
```

### Step 3.4: Move Resources Files

```bash
# Move EngineResources.h to types/core/
mv src/resources/EngineResources.h src/types/core/

# Verify
ls src/types/core/EngineResources.h
```

### Step 3.5: Update types/CMakeLists.txt

```cmake
target_include_directories(types
  INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/user_interface/
  ${CMAKE_CURRENT_SOURCE_DIR}/user_interface/styles/
  ${CMAKE_CURRENT_SOURCE_DIR}/core/
  ${CMAKE_CURRENT_SOURCE_DIR}/components/
  ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/      # ADD THIS LINE
  ${CMAKE_CURRENT_SOURCE_DIR}/events/
  ${CMAKE_CURRENT_SOURCE_DIR}/assets/
  ${CMAKE_CURRENT_SOURCE_DIR}/test_structs/
  ${CMAKE_CURRENT_SOURCE_DIR}/flatbuffers/generated/
)

# Keep existing dependencies
target_link_libraries(types
  INTERFACE
  SFML::Graphics
  stduuid
  # interfaces library removed - now part of types
  flatbuffers
  flatbuffers_headers
)
```

### Step 3.6: Remove Old Libraries

**Remove these directories:**
```bash
# Remove interfaces library
rm -rf src/interfaces/

# Remove configuration library
rm -rf src/configuration/

# Remove resources library
rm -rf src/resources/
```

**Update src/CMakeLists.txt:**

Remove these lines:
```cmake
# add_subdirectory(interfaces)      # REMOVE
# add_subdirectory(configuration)   # REMOVE
# add_subdirectory(resources)       # REMOVE
```

### Step 3.7: Update Dependent Libraries

**Files to update:**
- `src/entity/CMakeLists.txt`
- `src/user_interface/CMakeLists.txt`
- `src/data_providers/CMakeLists.txt`
- `src/assets/CMakeLists.txt`
- `src/display/CMakeLists.txt`
- `src/engine/CMakeLists.txt`
- `src/scenes/CMakeLists.txt`

**For each file, replace:**
```cmake
# Before
target_link_libraries(some_library
  types
  interfaces       # REMOVE
  configuration    # REMOVE
  resources        # REMOVE
)

# After
target_link_libraries(some_library
  types            # Now includes everything
)
```

### Step 3.8: Update Include Statements

**Most includes shouldn't need changes** if using include directories correctly.

Verify by searching for problematic includes:

```bash
# Check for any broken includes
grep -r "#include \"I.*\.h\"" src/ --include="*.cpp" --include="*.h"
grep -r "#include \"EngineResources\.h\"" src/ --include="*.cpp" --include="*.h"
```

### Step 3.9: Build and Test

```bash
# Clean build
rm -rf build/

# Reconfigure
cmake --preset Debug

# Build
cmake --build --preset Debug

# Run tests
ctest --preset Debug

# Commit if successful
git add -A
git commit -m "Phase 3: Consolidate foundation layer into types

- Moved all interface files to types/interfaces/
- Moved configuration files to types/interfaces/
- Moved EngineResources.h to types/core/
- Removed interfaces, configuration, resources libraries
- Updated all dependent libraries to use types only
- Eliminated circular dependency between types and interfaces
- Simplified CMake structure"
```

---

## Phase 4: Break Asset/UI Circular Dependency (6-8 hours)

**Goal:** Make asset loading UI-agnostic by introducing UIAssetLoader

**Risk:** HIGH  
**Impact:** Better modularity, cleaner separation of concerns, enables independent evolution of asset and UI systems

**Note:** This phase is optional and can be deferred until asset/UI systems need refactoring for other reasons.

### Understanding the Problem

**Current circular dependency:**
```
assets → user_interface (depends on UIStyle type)
user_interface → assets (depends on AssetManager for fonts/styles)
```

**The issue:**
- AssetManager stores `std::unordered_map<std::string, UIStyle>` (UI-specific type)
- AssetManager has methods like `LoadUIStyles()` and `GetDefaultUIStyle()` (UI-specific)
- This couples the generic asset system to UI concerns

**The solution:**
- Make AssetManager generic and UI-agnostic
- Create UIAssetLoader in user_interface layer to handle UI-specific asset operations
- Result: `assets → types`, `user_interface → assets` (unidirectional)

### Step 4.1: Analyze Current Dependencies

```bash
# Create new branch
git checkout -b phase4-break-circular-dependency

# Understand the circular dependency
cat src/assets/CMakeLists.txt  # See user_interface dependency
cat src/assets/AssetManager.h  # See UIStyle usage

# Find all UI-specific code in AssetManager
grep -n "UIStyle" src/assets/AssetManager.h
grep -n "UIStyle" src/assets/AssetManager.cpp
```

**Key findings:**
- AssetManager implements `IFontProvider` interface (good - generic)
- AssetManager stores `m_ui_styles` map (bad - UI-specific)
- Methods: `LoadUIStyles()`, `GetDefaultUIStyle()`, `GetAllUIStyles()` (bad - UI-specific)

### Step 4.2: Design UIAssetLoader

**Architecture decision:**

Create a **facade pattern** where UIAssetLoader wraps AssetManager and provides UI-specific functionality:

```
┌─────────────────────┐
│  user_interface/    │
│  UIElements, etc.   │
└──────────┬──────────┘
           │
           ↓
┌─────────────────────┐
│  UIAssetLoader      │  ← New class in user_interface/
│  - LoadUIStyles()   │
│  - GetUIStyle()     │
└──────────┬──────────┘
           │
           ↓
┌─────────────────────┐
│  AssetManager       │  ← Generic asset loading
│  - LoadAsset()      │
│  - GetFont()        │
└─────────────────────┘
```

### Step 4.3: Create UIAssetLoader (Header)

**File:** `src/user_interface/UIAssetLoader.h`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the UIAssetLoader class.
/////////////////////////////////////////////////

#pragma once

#include "AssetManager.h"
#include "FailInfo.h"
#include "UIStyle.h"
#include <expected>
#include <string>
#include <unordered_map>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Handles UI-specific asset loading and management.
///
/// UIAssetLoader wraps AssetManager to provide UI-specific
/// functionality without coupling the generic asset system
/// to UI concerns.
/////////////////////////////////////////////////
class UIAssetLoader {
private:
  /////////////////////////////////////////////////
  /// @brief Reference to the generic AssetManager
  /////////////////////////////////////////////////
  AssetManager& m_asset_manager;

  /////////////////////////////////////////////////
  /// @brief Storage for UI styles (moved from AssetManager)
  /////////////////////////////////////////////////
  std::unordered_map<std::string, UIStyle> m_ui_styles;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor taking reference to AssetManager
  ///
  /// @param asset_manager Reference to the generic asset manager
  /////////////////////////////////////////////////
  explicit UIAssetLoader(AssetManager& asset_manager);

  /////////////////////////////////////////////////
  /// @brief Load all UI styles from data providers
  ///
  /// Uses the AssetManager to load fonts, then configures
  /// UI styles based on loaded font data.
  ///
  /// @return Success or error information
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> LoadUIStyles();

  /////////////////////////////////////////////////
  /// @brief Get a specific UI style by name
  ///
  /// @param style_name Name of the style to retrieve
  /// @return UIStyle or error if not found
  /////////////////////////////////////////////////
  std::expected<const UIStyle&, FailInfo>
  GetUIStyle(const std::string& style_name) const;

  /////////////////////////////////////////////////
  /// @brief Convenience function to get the default UIStyle
  ///
  /// @return Reference to the default UIStyle
  /////////////////////////////////////////////////
  const UIStyle& GetDefaultUIStyle() const;

  /////////////////////////////////////////////////
  /// @brief Get all loaded UI styles
  ///
  /// @return Const reference to all UI styles
  /////////////////////////////////////////////////
  const std::unordered_map<std::string, UIStyle>& GetAllUIStyles() const;

  /////////////////////////////////////////////////
  /// @brief Access to underlying AssetManager (for fonts)
  ///
  /// Allows UI code to access fonts through IFontProvider interface
  ///
  /// @return Reference to AssetManager as IFontProvider
  /////////////////////////////////////////////////
  IFontProvider& GetFontProvider();
};

} // namespace steamrot
```

### Step 4.4: Implement UIAssetLoader

**File:** `src/user_interface/UIAssetLoader.cpp`

```cpp
#include "UIAssetLoader.h"
#include "DataAccessFactory.h"
#include "IUIStyleDataProvider.h"

namespace steamrot {

/////////////////////////////////////////////////
UIAssetLoader::UIAssetLoader(AssetManager& asset_manager)
    : m_asset_manager(asset_manager) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> UIAssetLoader::LoadUIStyles() {
  // Get UI style data from data access factory
  auto& data_factory = m_asset_manager.GetDataAccessFactory();
  auto style_provider = data_factory.GetUIStyleDataProvider();
  
  auto styles_result = style_provider->ProvideUIStyleData();
  if (!styles_result.has_value()) {
    return std::unexpected(styles_result.error());
  }

  const auto* style_data = styles_result.value();
  
  // Load each style using fonts from AssetManager
  for (const auto* style_config : *style_data->styles()) {
    UIStyle style;
    
    // Configure style with fonts from AssetManager
    if (style_config->font_name()) {
      auto font_result = m_asset_manager.GetFont(
          style_config->font_name()->str());
      if (font_result.has_value()) {
        style.m_font = font_result.value();
      }
    }
    
    // Configure other style properties
    if (style_config->font_size()) {
      style.m_font_size = style_config->font_size();
    }
    
    // Store the style
    m_ui_styles[style_config->name()->str()] = style;
  }
  
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<const UIStyle&, FailInfo>
UIAssetLoader::GetUIStyle(const std::string& style_name) const {
  auto it = m_ui_styles.find(style_name);
  if (it == m_ui_styles.end()) {
    return std::unexpected(FailInfo{
        FailMode::InvalidArgument,
        "UI style not found: " + style_name});
  }
  return it->second;
}

/////////////////////////////////////////////////
const UIStyle& UIAssetLoader::GetDefaultUIStyle() const {
  // Return first style or create default if none exist
  if (m_ui_styles.empty()) {
    static UIStyle default_style;
    return default_style;
  }
  return m_ui_styles.begin()->second;
}

/////////////////////////////////////////////////
const std::unordered_map<std::string, UIStyle>&
UIAssetLoader::GetAllUIStyles() const {
  return m_ui_styles;
}

/////////////////////////////////////////////////
IFontProvider& UIAssetLoader::GetFontProvider() {
  return m_asset_manager;
}

} // namespace steamrot
```

### Step 4.5: Refactor AssetManager (Remove UI Dependencies)

**File:** `src/assets/AssetManager.h`

**Remove these:**
```cpp
// REMOVE: #include "UIStyle.h"
// REMOVE: std::unordered_map<std::string, UIStyle> m_ui_styles;
// REMOVE: std::expected<std::monostate, FailInfo> LoadUIStyles();
// REMOVE: const UIStyle& GetDefaultUIStyle() const;
// REMOVE: const std::unordered_map<std::string, UIStyle>& GetAllUIStyles() const;
```

**Keep these (generic asset functionality):**
```cpp
class AssetManager : public IFontProvider {
private:
  DataAccessFactory& m_data_access_factory;
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> m_fonts;
  std::expected<std::monostate, FailInfo> AddFont(const std::string& font_name);

public:
  AssetManager(DataAccessFactory& data_access_factory);
  
  std::expected<std::monostate, FailInfo>
  LoadAssets(const AssetConfig asset_config);
  
  // IFontProvider interface
  std::expected<std::shared_ptr<const sf::Font>, FailInfo>
  GetFont(const std::string& font_name) const override;
  
  const std::unordered_map<std::string, std::shared_ptr<const sf::Font>>&
  GetAllFonts() const;
  
  // Accessor for data factory (needed by UIAssetLoader)
  DataAccessFactory& GetDataAccessFactory() { return m_data_access_factory; }
};
```

**File:** `src/assets/AssetManager.cpp`

Remove all UI-specific implementation code related to:
- `LoadUIStyles()` implementation
- `GetDefaultUIStyle()` implementation  
- `GetAllUIStyles()` implementation

### Step 4.6: Update CMakeLists.txt

**File:** `src/assets/CMakeLists.txt`

**Remove user_interface dependency:**

```cmake
target_link_libraries(assets
  PUBLIC
    SFML::Graphics
    types
    interfaces
    # user_interface  # REMOVE THIS LINE - breaks circular dependency
    data_providers
)
```

**File:** `src/user_interface/CMakeLists.txt`

**Add UIAssetLoader to build:**

```cmake
add_library(user_interface
  # ... existing files ...
  UIAssetLoader.cpp  # ADD THIS
)

# Already has assets dependency - no change needed
target_link_libraries(user_interface
  PUBLIC
    SFML::Graphics
    types
    assets  # Already present - now unidirectional
    interfaces
)
```

### Step 4.7: Update Code That Uses AssetManager

**Find all usages:**
```bash
grep -r "GetDefaultUIStyle\|LoadUIStyles\|GetAllUIStyles" src/ --include="*.cpp"
```

**Update pattern:**

**Before:**
```cpp
// Direct AssetManager usage
AssetManager& assets = engine_resources.asset_manager;
assets.LoadUIStyles();
const UIStyle& style = assets.GetDefaultUIStyle();
```

**After:**
```cpp
// Use UIAssetLoader
UIAssetLoader ui_assets(engine_resources.asset_manager);
ui_assets.LoadUIStyles();
const UIStyle& style = ui_assets.GetDefaultUIStyle();

// For fonts, can still use AssetManager directly (IFontProvider)
auto font = engine_resources.asset_manager.GetFont("arial");
// Or through UIAssetLoader
auto font2 = ui_assets.GetFontProvider().GetFont("arial");
```

**Common locations to update:**
- Scene initialization code
- UI factory classes
- Configuration loading code

### Step 4.8: Update EngineResources (if needed)

If EngineResources needs UI asset access, add UIAssetLoader:

**File:** `src/resources/EngineResources.h` (or wherever it ends up)

```cpp
struct EngineResources {
  // ... existing members ...
  AssetManager asset_manager;
  
  // Optional: Add UIAssetLoader if it's a core engine resource
  // UIAssetLoader ui_asset_loader{asset_manager};  // Initialized with asset_manager
};
```

**Or** create UIAssetLoader where needed (in scenes, UI systems, etc.)

### Step 4.9: Build and Test

```bash
# Clean build to catch any dependency issues
rm -rf build/

# Reconfigure
cmake --preset Debug

# Build - should succeed without circular dependency
cmake --build --preset Debug

# Run all tests
ctest --preset Debug

# Verify no user_interface dependency in assets
grep -r "user_interface" src/assets/CMakeLists.txt
# Should return nothing

# Commit if successful
git add -A
git commit -m "Phase 4: Break assets/user_interface circular dependency

- Created UIAssetLoader class in user_interface layer
- Moved UI style storage and management to UIAssetLoader
- Removed UIStyle dependencies from AssetManager (now generic)
- Removed user_interface dependency from assets CMakeLists
- AssetManager provides generic IFontProvider interface
- Updated all code using UI styles to use UIAssetLoader
- Now assets → types, user_interface → assets (unidirectional)
- Enables independent evolution of asset and UI systems"
```

### Step 4.10: Verify Dependency Graph

```bash
# Visualize the new dependency structure
# assets should NOT depend on user_interface anymore

# Check assets dependencies
grep "target_link_libraries(assets" src/assets/CMakeLists.txt

# Expected output:
# target_link_libraries(assets
#   PUBLIC
#     SFML::Graphics
#     types
#     interfaces
#     data_providers
# )

# Check user_interface dependencies  
grep "target_link_libraries(user_interface" src/user_interface/CMakeLists.txt

# Expected to include:
# - assets (unidirectional dependency)
```

### Benefits Achieved

**Architectural improvements:**
- ✅ Eliminated circular dependency between assets and user_interface
- ✅ AssetManager is now generic and UI-agnostic
- ✅ Clear separation: assets for generic loading, UIAssetLoader for UI-specific logic
- ✅ Unidirectional dependency flow: `user_interface → assets → types`

**Extensibility gains:**
- ✅ Can add other domain-specific loaders (AudioAssetLoader, GameAssetLoader, etc.)
- ✅ AssetManager can be reused in non-UI contexts
- ✅ UI system can evolve independently
- ✅ Asset system can evolve independently

**Maintainability:**
- ✅ Clearer responsibility boundaries
- ✅ Easier to test AssetManager in isolation
- ✅ UI-specific logic localized in user_interface layer

### Alternatives Considered

**Alternative 1: Generic Asset Types**
- Make AssetManager use `std::variant` or template-based generic storage
- Pro: More flexible for different asset types
- Con: More complex, loses type safety
- **Decision:** UIAssetLoader pattern is simpler and clearer

**Alternative 2: Asset Registry Pattern**
- Central registry for all asset types with plugins
- Pro: Maximum flexibility
- Con: Over-engineered for current needs
- **Decision:** Deferred until more asset types needed

**Alternative 3: Move UIStyle to types/**
- Keep UIStyle storage in AssetManager but move type to foundation
- Pro: Minimal code changes
- Con: Doesn't solve circular dependency, just moves it
- **Decision:** UIAssetLoader provides cleaner separation

---

## Final Verification

### Complete System Test

```bash
# Clean build from scratch
rm -rf build/

# Configure
cmake --preset Debug

# Full build
cmake --build --preset Debug

# Full test suite
ctest --preset Debug

# Check for warnings
cmake --build --preset Debug 2>&1 | grep -i warning
```

### Measure Improvements

```bash
# Build time comparison (vs backup branch)
# Record initial build time
# Record after-refactor build time
# Calculate improvement percentage

# Binary size comparison
du -sh build/steamrot/SteamRot
du -sh build/src/*.a

# Verify no functionality lost
ctest --preset Debug --verbose
```

### Documentation Updates

Update these files to reflect new structure:
- `README.md` - Update architecture overview
- `.github/COPILOT_INSTRUCTIONS.md` - Update file locations
- Any other docs referencing component locations

---

## Troubleshooting

### Issue: Include Path Errors

**Symptom:** `fatal error: 'CMeta.h' file not found`

**Solution:**
```bash
# Check include directories in CMakeLists.txt
# Ensure types includes components subdirectory
# Update include paths if needed to use correct relative paths
```

### Issue: Linker Errors

**Symptom:** `undefined reference to Component::~Component()`

**Solution:**
```bash
# Component destructor must be inline or have default implementation
# Check Component.h has: virtual ~Component() = default;
```

### Issue: Template Errors

**Symptom:** `TupleTypeIndex is not defined`

**Solution:**
```bash
# Ensure containers.h is included in component headers
# Check include order - containers.h must come before component definitions use it
```

### Issue: Circular Include Dependencies

**Symptom:** Compile errors about incomplete types, or circular dependency when trying to inline `GetComponentRegisterIndex()`

**Problem:** Component headers need `containers.h` for `TupleTypeIndex`, but `containers.h` includes all component headers for `ComponentRegister`.

**Solution:**
```bash
# Remove GetComponentRegisterIndex() entirely (as described in Phase 2 Step 2.2-2.4)
# Use TupleTypeIndex directly where needed with template lambdas
# This is the recommended approach to avoid circular dependencies
```

**Example - ArchetypeManager.cpp:**
```cpp
// Instead of: component.GetComponentRegisterIndex()
// Use: TupleTypeIndex<ComponentType, ComponentRegister>

std::apply(
    [&]<typename... ComponentTypes>(
        const std::vector<ComponentTypes> &...component_vector) {
      ((archetypeID.set(
           TupleTypeIndex<ComponentTypes, ComponentRegister>,
           component_vector[entity_index].m_active)),
       ...);
    },
    m_entity_memory_pool);
```

### Issue: Tests Failing

**Symptom:** Tests that passed before now fail

**Solution:**
```bash
# Check test includes are updated
# Verify test data paths are correct
# Ensure test CMakeLists.txt updated to link to types instead of components
```

---

## Rollback Procedures

### Rollback Single Phase

```bash
# If phase N fails, rollback to before it started
git log --oneline  # Find commit before phase N
git reset --hard <commit-hash-before-phase-N>
```

### Rollback All Changes

```bash
# Return to original state
git checkout backup-before-refactor
git branch -D phase1-data-structure-relocation
git branch -D phase2-header-only-components
git branch -D phase3-foundation-consolidation
git branch -D phase4-break-circular-dependency
```

---

## Success Criteria

### Phase 1 Complete
- [ ] SceneLoadData.h in types/core/
- [ ] SceneInfoProvider.h in types/core/
- [ ] All includes updated
- [ ] Clean build
- [ ] All tests pass

### Phase 2 Complete
- [ ] No .cpp files in components/
- [ ] All components in types/components/
- [ ] containers.h in types/components/
- [ ] components library removed from build
- [ ] Build time improved ~20%
- [ ] All tests pass

### Phase 3 Complete
- [ ] interfaces/ directory removed
- [ ] configuration/ directory removed
- [ ] resources/ directory removed
- [ ] All files in types/
- [ ] No circular dependencies in build graph
- [ ] All tests pass

### Phase 4 Complete (Optional)
- [ ] assets doesn't depend on user_interface
- [ ] Asset loading is generic
- [ ] UI-specific logic in user_interface layer
- [ ] All tests pass

---

## Post-Implementation

### Update Documentation

Create a new document: `ARCHITECTURE_MIGRATION_COMPLETE.md`

Document:
- What changed
- New file locations
- New include patterns
- Build time improvements
- Any lessons learned

### Update Team

- Share new architecture diagram
- Update onboarding docs
- Add to PR template: "Check for correct include paths"
- Update CI/CD if needed

### Monitor

- Watch build times over next few commits
- Monitor for any new issues
- Gather team feedback on new structure

---

## Quick Reference

### New File Locations

| Old Location | New Location |
|--------------|--------------|
| `src/components/Component.h` | `src/types/components/Component.h` |
| `src/components/CMeta.h` | `src/types/components/CMeta.h` |
| `src/components/containers.h` | `src/types/components/containers.h` |
| `src/interfaces/SceneLoadData.h` | `src/types/core/SceneLoadData.h` |
| `src/interfaces/I*.h` | `src/types/interfaces/I*.h` |
| `src/configuration/I*.h` | `src/types/interfaces/I*.h` |
| `src/resources/EngineResources.h` | `src/types/core/EngineResources.h` |

### Libraries Removed

- `components` - Now header-only in types
- `interfaces` - Merged into types
- `configuration` - Merged into types
- `resources` - Merged into types

### Include Patterns

**Before:**
```cpp
#include "CMeta.h"              // From components library
#include "SceneLoadData.h"       // From interfaces library
#include "IEntityImporter.h"     // From interfaces library
```

**After:**
```cpp
#include "CMeta.h"              // From types (paths may be same via include dirs)
#include "SceneLoadData.h"       // From types/core
#include "IEntityImporter.h"     // From types/interfaces
```

### Linking Pattern

**Before:**
```cmake
target_link_libraries(my_library
  types
  interfaces
  components
  configuration
  resources
)
```

**After:**
```cmake
target_link_libraries(my_library
  types  # Everything now in types
)
```

---

## Help and Support

**Questions?** Refer back to:
- [RECOMMENDATIONS_SUMMARY.md](./RECOMMENDATIONS_SUMMARY.md) - Why these changes
- [ARCHITECTURE_LAYER_ANALYSIS.md](./ARCHITECTURE_LAYER_ANALYSIS.md) - Detailed analysis
- [ARCHITECTURE_DIAGRAMS.md](./ARCHITECTURE_DIAGRAMS.md) - Visual guides

**Issues?** See Troubleshooting section above or rollback and reassess.

**Success?** Document your experience and update this guide with any lessons learned!
