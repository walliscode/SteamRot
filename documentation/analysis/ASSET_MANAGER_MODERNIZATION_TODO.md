# AssetManager Modernization To-Do List

## Overview

This document provides an actionable, prioritized to-do list for bringing the AssetManager in line with the rest of the SteamRot codebase. Tasks are organized by priority and include estimated effort, dependencies, and validation steps.

**Related Document:** See `ASSET_MANAGER_ALIGNMENT_ANALYSIS.md` for detailed analysis and rationale.

---

## Priority Levels

- **P0 (Critical):** Must be done to fix broken functionality
- **P1 (High):** Should be done soon, enables testing and follows established patterns
- **P2 (Medium):** Important for architecture consistency
- **P3 (Low):** Nice to have, improves maintainability

---

## Quick Start Checklist

For developers who want to jump in immediately:

- [ ] **Quick Win:** Implement IFontProvider in AssetManager (2 hours, see Task 1.1)
- [ ] **Quick Win:** Fix asset_test_helpers.cpp (30 min, see Task 1.2)
- [ ] **Quick Win:** Add GetFont() tests (1 hour, see Task 1.3)

These three tasks can be completed in one sitting and provide immediate value.

---

## Phase 1: Implement IFontProvider (P1 - High Priority)

**Goal:** Make AssetManager implement IFontProvider interface  
**Total Effort:** 3-4 hours  
**Risk:** Low  
**Dependencies:** None

### Task 1.1: Add IFontProvider Implementation

**Effort:** 1.5 hours  
**Assignee:** TBD

**Steps:**

1. **Update AssetManager.h**
   ```cpp
   // Add include
   #include "IFontProvider.h"
   
   // Update class declaration
   class AssetManager : public IFontProvider {
     // ... existing code ...
   public:
     // Add new method (pure virtual override)
     std::expected<std::shared_ptr<const sf::Font>, FailInfo>
       GetFont(const std::string &font_name) const override;
   };
   ```

2. **Implement GetFont() in AssetManager.cpp**
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

3. **Add documentation comments** (Doxygen style)

**Validation:**
```bash
# Compile
cmake --build --preset Debug

# Verify no compilation errors
# Verify IFontProvider is properly implemented
```

**Acceptance Criteria:**
- [ ] AssetManager inherits from IFontProvider
- [ ] GetFont() method implemented
- [ ] Code compiles without errors
- [ ] Doxygen comments added

---

### Task 1.2: Fix asset_test_helpers

**Effort:** 30 minutes  
**Assignee:** TBD  
**Dependencies:** Task 1.1 complete

**Steps:**

1. **Update CheckFontConfiguration in asset_test_helpers.cpp**
   ```cpp
   void CheckFontConfiguration(const AssetCollection &asset_collection,
                               const AssetManager &asset_manager) {
     if (!asset_collection.fonts()) {
       return;
     }
     
     // Cast to IFontProvider
     const IFontProvider &font_provider = asset_manager;
     
     for (const auto &font : *asset_collection.fonts()) {
       const std::string font_name = font->name()->str();
       
       // Use GetFont() instead of GetAllFonts()
       auto font_result = font_provider.GetFont(font_name);
       
       if (!font_result.has_value()) {
         FAIL(font_result.error().message);
       }
       
       // Verify font is not null
       REQUIRE(font_result.value() != nullptr);
     }
   }
   ```

**Validation:**
```bash
# Run asset tests
ctest --preset Debug -R AssetManager
```

**Acceptance Criteria:**
- [ ] CheckFontConfiguration uses GetFont()
- [ ] Tests compile and run
- [ ] All existing asset tests pass

---

### Task 1.3: Add GetFont() Unit Tests

**Effort:** 1 hour  
**Assignee:** TBD  
**Dependencies:** Task 1.1 complete

**Steps:**

1. **Add tests to AssetManager.test.cpp**
   ```cpp
   TEST_CASE("AssetManager implements IFontProvider", "[unit][AssetManager]") {
     AssetManager asset_manager;
     asset_manager.LoadDefaultAssets();
     
     // Upcast to interface
     IFontProvider &provider = asset_manager;
     auto font = provider.GetFont("DaddyTimeMonoNerdFont-Regular");
     
     REQUIRE(font.has_value());
     REQUIRE(font.value() != nullptr);
   }
   
   TEST_CASE("AssetManager::GetFont returns error for missing font", 
             "[unit][AssetManager]") {
     AssetManager asset_manager;
     
     auto font = asset_manager.GetFont("NonExistentFont");
     
     REQUIRE(!font.has_value());
     REQUIRE(font.error().mode == FailMode::NotFoundInMap);
     REQUIRE(font.error().message.find("NonExistentFont") != std::string::npos);
   }
   
   TEST_CASE("AssetManager::GetFont works after LoadDefaultAssets", 
             "[unit][AssetManager]") {
     AssetManager asset_manager;
     
     // Before loading
     auto before = asset_manager.GetFont("DaddyTimeMonoNerdFont-Regular");
     REQUIRE(!before.has_value());
     
     // Load assets
     auto load_result = asset_manager.LoadDefaultAssets();
     REQUIRE(load_result.has_value());
     
     // After loading
     auto after = asset_manager.GetFont("DaddyTimeMonoNerdFont-Regular");
     REQUIRE(after.has_value());
   }
   ```

**Validation:**
```bash
ctest --preset Debug -R "AssetManager.*GetFont"
```

**Acceptance Criteria:**
- [ ] Test for successful font retrieval
- [ ] Test for missing font error
- [ ] Test for before/after loading
- [ ] All tests pass

---

### Task 1.4: Update Documentation

**Effort:** 30 minutes  
**Assignee:** TBD  
**Dependencies:** Tasks 1.1-1.3 complete

**Steps:**

1. **Update .github/copilot-instructions.md**
   - Add note that AssetManager implements IFontProvider
   - Document GetFont() usage pattern

2. **Update README.md** (if applicable)
   - Note AssetManager as IFontProvider

3. **Add inline code comments** where GetFont() is used

**Acceptance Criteria:**
- [ ] Documentation updated
- [ ] Examples added
- [ ] Pattern documented

---

**Phase 1 Complete When:**
- [x] All tasks 1.1-1.4 complete
- [x] All tests pass
- [x] Code reviewed and approved
- [x] Documentation updated

---

## Phase 2: Create Test Infrastructure (P1 - High Priority)

**Goal:** Add mock providers for testing  
**Total Effort:** 4-5 hours  
**Risk:** Low  
**Dependencies:** Phase 1 complete

### Task 2.1: Create MockFontProvider

**Effort:** 1.5 hours  
**Assignee:** TBD

**Steps:**

1. **Create MockFontProvider.h**
   - Location: `tests/unit/assets/MockFontProvider.h`
   - Inherit from IFontProvider
   - Add methods: AddFont(), HasFont(), Clear()

2. **Create MockFontProvider.cpp**
   - Implement GetFont() to return from internal map
   - Return error if font not found

3. **Add Doxygen documentation**

**Reference Code:**
```cpp
class MockFontProvider : public IFontProvider {
private:
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> m_fonts;

public:
  void AddFont(const std::string &name, std::shared_ptr<const sf::Font> font);
  bool HasFont(const std::string &name) const;
  void Clear();
  
  std::expected<std::shared_ptr<const sf::Font>, FailInfo>
    GetFont(const std::string &font_name) const override;
};
```

**Validation:**
- [ ] Compiles without errors
- [ ] Doxygen documentation complete

---

### Task 2.2: Create MockFontDataProvider

**Effort:** 2 hours  
**Assignee:** TBD

**Steps:**

1. **Create IFontDataProvider interface first** (if doing Phase 3 later, skip this)
   - Location: `src/interfaces/IFontDataProvider.h`
   - Single method: LoadFontFromSource()

2. **Create MockFontDataProvider.h**
   - Location: `tests/unit/assets/MockFontDataProvider.h`
   - Inherit from IFontDataProvider
   - Add methods: AddFontData(), SetFontToFail(), Clear()

3. **Create MockFontDataProvider.cpp**
   - Implement LoadFontFromSource()
   - Return error if font set to fail
   - Return font from internal map

**Reference Code:**
```cpp
class MockFontDataProvider : public IFontDataProvider {
private:
  std::unordered_map<std::string, sf::Font> m_font_data;
  std::set<std::string> m_fonts_to_fail;

public:
  void AddFontData(const std::string &name, const sf::Font &font);
  void SetFontToFail(const std::string &name);
  void Clear();
  
  std::expected<sf::Font, FailInfo>
    LoadFontFromSource(const std::string &font_name) const override;
};
```

**Note:** This task can be deferred to Phase 3 if IFontDataProvider doesn't exist yet.

**Validation:**
- [ ] Compiles without errors
- [ ] Doxygen documentation complete

---

### Task 2.3: Add Tests Using Mock Providers

**Effort:** 1.5 hours  
**Assignee:** TBD  
**Dependencies:** Tasks 2.1, 2.2 complete

**Steps:**

1. **Add MockFontProvider tests**
   ```cpp
   TEST_CASE("MockFontProvider returns added fonts", "[unit][MockFontProvider]") {
     MockFontProvider mock;
     auto test_font = std::make_shared<sf::Font>();
     mock.AddFont("test", test_font);
     
     auto result = mock.GetFont("test");
     REQUIRE(result.has_value());
     REQUIRE(result.value() == test_font);
   }
   
   TEST_CASE("MockFontProvider returns error for missing fonts", 
             "[unit][MockFontProvider]") {
     MockFontProvider mock;
     
     auto result = mock.GetFont("missing");
     REQUIRE(!result.has_value());
   }
   ```

2. **Add IFontProvider contract tests**
   ```cpp
   TEST_CASE("IFontProvider contract - AssetManager", 
             "[unit][IFontProvider][AssetManager]") {
     AssetManager asset_manager;
     asset_manager.LoadDefaultAssets();
     
     IFontProvider &provider = asset_manager;
     // Test IFontProvider interface
   }
   
   TEST_CASE("IFontProvider contract - MockFontProvider", 
             "[unit][IFontProvider][mock]") {
     MockFontProvider mock;
     mock.AddFont("test", std::make_shared<sf::Font>());
     
     IFontProvider &provider = mock;
     // Test IFontProvider interface
   }
   ```

**Validation:**
```bash
ctest --preset Debug -R "MockFontProvider"
ctest --preset Debug -R "IFontProvider"
```

**Acceptance Criteria:**
- [ ] Mock provider tests pass
- [ ] Contract tests verify behavior
- [ ] Both AssetManager and mock pass contract tests

---

### Task 2.4: Add CMakeLists.txt Updates

**Effort:** 15 minutes  
**Assignee:** TBD  
**Dependencies:** Tasks 2.1-2.3 complete

**Steps:**

1. **Update tests/unit/assets/CMakeLists.txt**
   ```cmake
   add_executable(test_assets
     AssetManager.test.cpp
     MockFontProvider.cpp        # Add this
     MockFontDataProvider.cpp    # Add this (if Phase 3 done)
     asset_test_helpers.cpp
   )
   ```

**Validation:**
```bash
cmake --build --preset Debug
ctest --preset Debug -R test_assets
```

**Acceptance Criteria:**
- [ ] CMakeLists.txt updated
- [ ] Project builds successfully
- [ ] All tests run

---

**Phase 2 Complete When:**
- [x] All tasks 2.1-2.4 complete
- [x] Mock providers implemented
- [x] Tests using mocks pass
- [x] Documentation updated

---

## Phase 3: Extract Font Loading Logic (P2 - Medium Priority)

**Goal:** Move font loading to dedicated provider  
**Total Effort:** 6-8 hours  
**Risk:** Medium (breaking changes)  
**Dependencies:** Phase 2 complete

### Task 3.1: Create IFontDataProvider Interface

**Effort:** 30 minutes  
**Assignee:** TBD

**Steps:**

1. **Create IFontDataProvider.h**
   - Location: `src/interfaces/IFontDataProvider.h`
   - Single pure virtual method: LoadFontFromSource()
   - Document interface purpose and usage

**Reference Code:** See `ASSET_MANAGER_ALIGNMENT_ANALYSIS.md` Section "Improvement 2"

**Validation:**
- [ ] Interface compiles
- [ ] Doxygen documentation complete

---

### Task 3.2: Create FileSystemFontDataProvider

**Effort:** 2 hours  
**Assignee:** TBD  
**Dependencies:** Task 3.1 complete

**Steps:**

1. **Create FileSystemFontDataProvider.h**
   - Location: `src/data_providers/FileSystemFontDataProvider.h`
   - Inherit from IFontDataProvider
   - Override LoadFontFromSource()

2. **Create FileSystemFontDataProvider.cpp**
   - Move logic from AssetManager::AddFont()
   - Use paths::GetFontsDirectory()
   - Handle file existence check
   - Load font with sf::Font::openFromFile()
   - Apply font smoothing settings

3. **Add unit tests**
   ```cpp
   TEST_CASE("FileSystemFontDataProvider loads valid fonts", 
             "[unit][FileSystemFontDataProvider]") {
     FileSystemFontDataProvider provider;
     auto result = provider.LoadFontFromSource("DaddyTimeMonoNerdFont-Regular");
     REQUIRE(result.has_value());
   }
   
   TEST_CASE("FileSystemFontDataProvider returns error for missing fonts", 
             "[unit][FileSystemFontDataProvider]") {
     FileSystemFontDataProvider provider;
     auto result = provider.LoadFontFromSource("NonExistentFont");
     REQUIRE(!result.has_value());
     REQUIRE(result.error().mode == FailMode::FileNotFound);
   }
   ```

**Validation:**
```bash
ctest --preset Debug -R FileSystemFontDataProvider
```

**Acceptance Criteria:**
- [ ] Provider implemented
- [ ] Loads fonts from file system
- [ ] Returns errors appropriately
- [ ] Tests pass

---

### Task 3.3: Update AssetManager to Use Provider

**Effort:** 2 hours  
**Assignee:** TBD  
**Dependencies:** Task 3.2 complete

**Steps:**

1. **Update AssetManager.h**
   ```cpp
   #include "IFontDataProvider.h"
   
   class AssetManager : public IFontProvider {
   private:
     IFontDataProvider &m_font_data_provider;
     
   public:
     // Add constructor
     AssetManager(IFontDataProvider &font_data_provider);
     
     // ... existing methods ...
   };
   ```

2. **Update AssetManager.cpp**
   - Add constructor implementation
   - Update AddFont() to use m_font_data_provider
   - Remove direct file system access

3. **Update all AssetManager construction sites**
   - Find all places that create AssetManager
   - Add GetFontDataProvider() parameter
   - Estimate: 5-10 locations

**Breaking Change Alert:** This changes AssetManager constructor signature.

**Validation:**
```bash
# Search for AssetManager construction
grep -r "AssetManager asset_manager" src/ tests/

# Update each location
# Build and test
cmake --build --preset Debug
ctest --preset Debug
```

**Acceptance Criteria:**
- [ ] AssetManager constructor updated
- [ ] AddFont() uses provider
- [ ] All construction sites updated
- [ ] Tests pass

---

### Task 3.4: Add Provider Factory Function

**Effort:** 1 hour  
**Assignee:** TBD  
**Dependencies:** Task 3.3 complete

**Steps:**

1. **Update provider_factory.h**
   ```cpp
   ////////////////////////////////////////////////////////////
   /// @brief Get the font data provider instance.
   ///
   /// Returns a static instance of FileSystemFontDataProvider.
   ////////////////////////////////////////////////////////////
   IFontDataProvider &GetFontDataProvider();
   ```

2. **Update provider_factory.cpp**
   ```cpp
   #include "FileSystemFontDataProvider.h"
   
   IFontDataProvider &GetFontDataProvider() {
     static FileSystemFontDataProvider provider;
     return provider;
   }
   ```

3. **Update AssetManager construction to use factory**
   ```cpp
   // Before:
   FileSystemFontDataProvider provider;
   AssetManager asset_manager(provider);
   
   // After:
   AssetManager asset_manager(GetFontDataProvider());
   ```

**Validation:**
```bash
cmake --build --preset Debug
ctest --preset Debug
```

**Acceptance Criteria:**
- [ ] Factory function implemented
- [ ] Returns singleton provider
- [ ] Used in AssetManager construction
- [ ] Tests pass

---

### Task 3.5: Update CMakeLists.txt

**Effort:** 15 minutes  
**Assignee:** TBD  
**Dependencies:** Tasks 3.1-3.4 complete

**Steps:**

1. **Update src/interfaces/CMakeLists.txt**
   ```cmake
   # Add IFontDataProvider.h to interface headers
   ```

2. **Update src/data_providers/CMakeLists.txt**
   ```cmake
   add_library(data_providers
     # ... existing files ...
     FileSystemFontDataProvider.cpp
   )
   ```

3. **Update src/assets/CMakeLists.txt**
   ```cmake
   target_link_libraries(assets
     PRIVATE
     data_providers  # Ensure this is included
     # ...
   )
   ```

**Validation:**
```bash
cmake --build --preset Debug
```

**Acceptance Criteria:**
- [ ] All CMakeLists.txt updated
- [ ] Project builds successfully
- [ ] No linking errors

---

### Task 3.6: Integration Testing

**Effort:** 1 hour  
**Assignee:** TBD  
**Dependencies:** Tasks 3.1-3.5 complete

**Steps:**

1. **Run full test suite**
   ```bash
   ctest --preset Debug
   ```

2. **Test AssetManager with both providers**
   ```cpp
   TEST_CASE("AssetManager with FileSystemFontDataProvider", 
             "[integration][AssetManager]") {
     AssetManager asset_manager(GetFontDataProvider());
     auto result = asset_manager.LoadDefaultAssets();
     REQUIRE(result.has_value());
   }
   
   TEST_CASE("AssetManager with MockFontDataProvider", 
             "[integration][AssetManager]") {
     MockFontDataProvider mock;
     mock.AddFontData("test", sf::Font());
     
     AssetManager asset_manager(mock);
     // Test with mock provider
   }
   ```

3. **Visual/manual testing** (if needed)
   - Run actual game
   - Verify fonts load correctly
   - Check UI rendering

**Acceptance Criteria:**
- [ ] All unit tests pass
- [ ] Integration tests pass
- [ ] Manual testing confirms fonts work
- [ ] No regressions

---

**Phase 3 Complete When:**
- [x] All tasks 3.1-3.6 complete
- [x] AssetManager uses IFontDataProvider
- [x] Factory function works
- [x] All tests pass
- [x] No regressions in font loading

---

## Phase 4: Add Provider Factories (P3 - Low Priority)

**Goal:** Add factory functions for font providers  
**Total Effort:** 2-3 hours  
**Risk:** Low  
**Dependencies:** Phase 3 complete

### Task 4.1: Add GetFontProvider Factory

**Effort:** 1 hour  
**Assignee:** TBD

**Steps:**

1. **Decide on approach**
   - Option A: Singleton AssetManager
   - Option B: Context-based (recommended)

2. **If context-based, update GameContext**
   ```cpp
   class GameContext {
   private:
     AssetManager m_asset_manager;
     
   public:
     IFontProvider &GetFontProvider() { return m_asset_manager; }
   };
   ```

3. **If singleton, create factory**
   ```cpp
   // provider_factory.h
   IFontProvider &GetFontProvider();
   
   // provider_factory.cpp
   IFontProvider &GetFontProvider() {
     static AssetManager &asset_manager = GetAssetManager();
     return asset_manager;
   }
   ```

**Note:** Context-based approach is recommended (simpler lifecycle management).

**Validation:**
- [ ] Approach decided
- [ ] Factory implemented (if applicable)
- [ ] Tests pass

---

### Task 4.2: Document Factory Pattern

**Effort:** 1 hour  
**Assignee:** TBD  
**Dependencies:** Task 4.1 complete

**Steps:**

1. **Update .github/copilot-instructions.md**
   - Document factory usage
   - Add examples

2. **Update provider_factory.h**
   - Add comprehensive documentation
   - Explain when to use each factory

3. **Add usage examples**

**Acceptance Criteria:**
- [ ] Documentation complete
- [ ] Examples provided
- [ ] Pattern explained

---

**Phase 4 Complete When:**
- [x] All tasks 4.1-4.2 complete
- [x] Factory approach decided and implemented
- [x] Documentation complete

---

## Optional: UI Style Provider Factory (P3 - Low Priority)

**Goal:** Add factory for UI style provider  
**Total Effort:** 1-2 hours  
**Risk:** Low  
**Dependencies:** None

### Task 5.1: Add UI Style Provider Factory

**Effort:** 1 hour  
**Assignee:** TBD

**Steps:**

1. **Consider context-based approach** (recommended)
   ```cpp
   class AssetManager : public IFontProvider {
   private:
     std::unique_ptr<IUIStyleDataProvider> m_ui_style_provider;
     
   public:
     IUIStyleDataProvider &GetUIStyleDataProvider() {
       if (!m_ui_style_provider) {
         m_ui_style_provider = 
           std::make_unique<FlatbuffersUIStyleDataProvider>(*this);
       }
       return *m_ui_style_provider;
     }
   };
   ```

2. **Update LoadUIStyles() to use method**
   ```cpp
   std::expected<std::monostate, FailInfo> AssetManager::LoadUIStyles() {
     IUIStyleDataProvider &provider = GetUIStyleDataProvider();
     auto result = provider.ProvideUIStyles();
     // ...
   }
   ```

**Validation:**
```bash
cmake --build --preset Debug
ctest --preset Debug -R AssetManager
```

**Acceptance Criteria:**
- [ ] Method implemented
- [ ] LoadUIStyles() updated
- [ ] Tests pass

---

## Testing Checklist

After completing each phase, verify:

**Phase 1:**
- [ ] `ctest --preset Debug -R AssetManager` passes
- [ ] IFontProvider upcast works
- [ ] GetFont() returns expected results

**Phase 2:**
- [ ] Mock providers compile and link
- [ ] Tests using mocks pass
- [ ] Contract tests pass for both implementations

**Phase 3:**
- [ ] Full test suite passes
- [ ] Font loading works with new provider
- [ ] No regressions in asset loading
- [ ] Manual testing confirms fonts display correctly

**Phase 4:**
- [ ] Factory functions work
- [ ] Documentation is clear
- [ ] Examples are provided

---

## Common Issues and Solutions

### Issue: Compilation Error - IFontProvider Not Found

**Solution:**
```cpp
// Make sure to include the interface
#include "IFontProvider.h"

// Check CMakeLists.txt includes interfaces library
target_link_libraries(assets
  PUBLIC interfaces
)
```

### Issue: Linker Error - GetFont() Undefined

**Solution:**
- Verify GetFont() is implemented in AssetManager.cpp
- Check that `override` keyword is present
- Rebuild project fully: `cmake --build --preset Debug --clean-first`

### Issue: Test Failure - Font Not Found

**Solution:**
- Check that LoadDefaultAssets() was called
- Verify font exists in GetAllFonts() map
- Check font name spelling (case-sensitive)

### Issue: Breaking Change - AssetManager Constructor

**Solution:**
- Find all AssetManager construction sites:
  ```bash
  grep -r "AssetManager asset_manager" src/ tests/
  ```
- Update each to pass provider:
  ```cpp
  AssetManager asset_manager(GetFontDataProvider());
  ```

---

## Success Metrics

**Phase 1 Success:**
- ✅ AssetManager implements IFontProvider
- ✅ GetFont() method works
- ✅ asset_test_helpers uses GetFont()
- ✅ All existing tests pass

**Phase 2 Success:**
- ✅ Mock providers implemented
- ✅ Tests work with mocks (no file system)
- ✅ Contract tests verify interface behavior

**Phase 3 Success:**
- ✅ Font loading extracted to provider
- ✅ AssetManager uses IFontDataProvider
- ✅ Factory function works
- ✅ No regressions

**Phase 4 Success:**
- ✅ Factory pattern standardized
- ✅ Documentation complete
- ✅ Consistent with other providers

---

## Timeline Estimates

**Aggressive (Full-time, experienced developer):**
- Phase 1: 1 day
- Phase 2: 1 day
- Phase 3: 1.5 days
- Phase 4: 0.5 days
- **Total: 4 days**

**Moderate (Part-time or new to codebase):**
- Phase 1: 2 days
- Phase 2: 2 days
- Phase 3: 3 days
- Phase 4: 1 day
- **Total: 8 days (2 weeks part-time)**

**Conservative (Learning as you go):**
- Phase 1: 3 days
- Phase 2: 3 days
- Phase 3: 5 days
- Phase 4: 1 day
- **Total: 12 days (3 weeks part-time)**

---

## Getting Help

**Questions about:**
- **Interfaces:** See existing IAssetDataProvider pattern
- **Testing:** See tests/unit/assets/ for examples
- **Providers:** See src/data_providers/ for patterns
- **Factories:** See provider_factory.h/cpp

**Stuck on:**
- **Compilation:** Check CMakeLists.txt dependencies
- **Tests:** Run with `--verbose` flag for details
- **Patterns:** Review ASSET_MANAGER_ALIGNMENT_ANALYSIS.md

---

## Document Metadata

**Author:** GitHub Copilot Agent  
**Date:** 2025-12-22  
**Version:** 1.0  
**Status:** Ready for Implementation

**Related Documents:**
- `ASSET_MANAGER_ALIGNMENT_ANALYSIS.md` - Detailed analysis
- `FONT_PROVIDER_DECOUPLING.md` - IFontProvider pattern
- `.github/copilot-instructions.md` - Coding standards

---

**Ready to Start? Begin with Phase 1, Task 1.1! 🚀**
