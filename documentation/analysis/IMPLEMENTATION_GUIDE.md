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

### Step 2.1: Create Target Directory Structure

```bash
# Create new branch
git checkout -b phase2-header-only-components

# Create directory for components in types
mkdir -p src/types/components

# Verify types/CMakeLists.txt includes this directory
# Should already be set to include subdirectories
```

### Step 2.2: Update Component Base Class

**File:** `src/components/Component.h`

**Before:**
```cpp
struct Component {
  virtual ~Component();  // Declared in .h, defined in .cpp
  virtual size_t GetComponentRegisterIndex() const = 0;
  bool m_active{false};
};
```

**After:**
```cpp
struct Component {
  virtual ~Component() = default;  // Inline, no .cpp needed
  virtual constexpr size_t GetComponentRegisterIndex() const = 0;
  bool m_active{false};
};
```

**Action:**
```bash
# Edit Component.h
vi src/components/Component.h

# Delete Component.cpp (no longer needed)
rm src/components/Component.cpp
```

### Step 2.3: Convert Each Component to Header-Only

For **each component** (CMeta, CUserInterface, CMachinaForm, CGrimoireMachina, CUIState):

#### Example: CMeta

**File:** `src/components/CMeta.h`

**Before:**
```cpp
struct CMeta : public Component {
  CMeta() = default;
  bool m_entity_alive = false;
  size_t GetComponentRegisterIndex() const override;  // Declared here
};
```

**After:**
```cpp
#include "containers.h"  // Need this for TupleTypeIndex

struct CMeta : public Component {
  CMeta() = default;
  bool m_entity_alive = false;
  
  constexpr size_t GetComponentRegisterIndex() const override {
    return TupleTypeIndex<CMeta, ComponentRegister>;
  }
};
```

**Action for CMeta:**
```bash
# Edit CMeta.h - add inline implementation
vi src/components/CMeta.h

# Delete CMeta.cpp
rm src/components/CMeta.cpp
```

**Repeat for remaining components:**
- `CUserInterface.h` / `CUserInterface.cpp`
- `CMachinaForm.h` / `CMachinaForm.cpp`
- `CGrimoireMachina.h` / `CGrimoireMachina.cpp`
- `CUIState.h` / `CUIState.cpp`

### Step 2.4: Move containers.h

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

### Step 2.5: Move All Component Headers

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

### Step 2.6: Update types/CMakeLists.txt

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

### Step 2.7: Remove components Library

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

### Step 2.8: Update All Component Includes

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

### Step 2.9: Update Dependent Libraries

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

### Step 2.10: Build and Test

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
- Made GetComponentRegisterIndex() constexpr inline in each component
- Removed all component .cpp files
- Moved all component headers to types/components/
- Moved containers.h to types/components/
- Removed components library from build system
- Updated all dependent libraries to use types instead of components
- Expected ~20% build time improvement"
```

### Step 2.11: Verify Build Time Improvement

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

**Goal:** Make asset loading UI-agnostic

**Risk:** HIGH  
**Impact:** Better modularity, cleaner separation of concerns

**Note:** This phase is optional and can be deferred until asset/UI systems need refactoring for other reasons.

### Step 4.1: Analyze Current Dependencies

```bash
# Create new branch
git checkout -b phase4-break-circular-dependency

# Find why assets depends on user_interface
grep -r "user_interface" src/assets/ --include="*.cpp" --include="*.h"

# Find what UI needs from assets
grep -r "AssetManager" src/user_interface/ --include="*.cpp" --include="*.h"
```

### Step 4.2: Refactor AssetManager

**Current issue:** AssetManager has UI-specific methods.

**Goal:** Make AssetManager generic, move UI-specific logic to user_interface layer.

**Example refactoring:**

**Before (in AssetManager):**
```cpp
class AssetManager {
  void LoadUIAssets();  // UI-specific method
  void LoadFonts();     // UI-specific
};
```

**After:**
```cpp
// AssetManager becomes generic
class AssetManager {
  void LoadAsset(const AssetConfig& config);  // Generic
};

// In user_interface layer
class UIAssetLoader {
  UIAssetLoader(AssetManager& assets) : m_assets(assets) {}
  void LoadUIAssets() {
    // UI-specific logic using generic AssetManager
  }
private:
  AssetManager& m_assets;
};
```

### Step 4.3: Update CMakeLists.txt

**File:** `src/assets/CMakeLists.txt`

**Remove user_interface dependency:**

```cmake
target_link_libraries(assets
  PUBLIC
  SFML::Graphics
  types
  # user_interface  # REMOVE THIS LINE
  data_providers
)
```

### Step 4.4: Move UI-Specific Asset Code

Create new file in user_interface:
- `src/user_interface/UIAssetHelpers.cpp`
- `src/user_interface/UIAssetHelpers.h`

Move UI-specific asset loading logic here.

### Step 4.5: Build and Test

```bash
# Build
cmake --build --preset Debug

# Test
ctest --preset Debug

# Commit if successful
git add -A
git commit -m "Phase 4: Break assets/user_interface circular dependency

- Made AssetManager generic (removed UI-specific methods)
- Created UIAssetHelpers in user_interface for UI-specific asset logic
- Removed user_interface dependency from assets
- Now assets → types, user_interface → assets (unidirectional)"
```

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

**Symptom:** Compile errors about incomplete types

**Solution:**
```bash
# Use forward declarations where possible
# Break circular includes by moving implementations to .cpp (if any remain)
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
