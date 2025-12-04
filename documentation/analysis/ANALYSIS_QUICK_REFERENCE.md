# Codebase Analysis - Quick Reference

**Date**: December 3, 2025  
**For**: Quick lookup of analysis findings

> 📘 For full details, see [COMPREHENSIVE_CODEBASE_ANALYSIS.md](./COMPREHENSIVE_CODEBASE_ANALYSIS.md)  
> 📊 For architecture diagrams, see [ARCHITECTURE_DIAGRAMS.md](./ARCHITECTURE_DIAGRAMS.md)

---

## At a Glance

| Metric | Count | Status |
|--------|-------|--------|
| Source Files | 169 | ✓ |
| Test Files | 118 | ⚠️ |
| Documentation Files | 47 | ⚠️ |
| **Missing Logic Tests** | **5** | **❌** |
| **Outdated Docs** | **4** | **❌** |

---

## 🔴 Critical Issues

### 1. Outdated Documentation (HIGH PRIORITY)

All reference deprecated `TestContext` instead of current `TestFixture`:

- `documentation/workflows/ADDING_LOGIC.md` ← PRIMARY WORKFLOW DOC
- `documentation/testing/TEST_DATA_CONFIGURATION.md`
- `documentation/testing/TESTING_OVERVIEW.md`
- `documentation/examples/README.md`

**Impact**: Developers following docs will write broken code  
**Fix Time**: ~9 hours total (Week 1 task)

### 2. Missing Logic Class Unit Tests (HIGH PRIORITY)

These classes exist but have NO dedicated unit tests:

- `src/logic/UIActionLogic.cpp` → ❌ No test
- `src/logic/UICollisionLogic.cpp` → ❌ No test
- `src/logic/UIRenderLogic.cpp` → ❌ No test
- `src/logic/CraftingRenderLogic.cpp` → ❌ No test
- `src/logic/UIStateLogic.cpp` → ❌ No test

**Impact**: Cannot rely on data-driven testing without foundation  
**Fix Time**: ~15 hours total (Week 2 task)

---

## ⚠️ Medium Priority Issues

### 3. Partial Free Function Test Coverage

These have SOME tests but need expansion:

- `src/logic/logic_action.cpp` → tests/unit/logic/draw_ui_elements.test.cpp (partial)
- `src/logic/logic_collision.cpp` → tests/unit/logic/collision.test.cpp (partial)
- `src/logic/logic_ui.cpp` → tests/unit/logic/ui_helpers.test.cpp (partial)

**Fix Time**: ~11 hours (Week 3 task)

### 4. Namespace Inconsistencies

- `namespace event` vs `namespace events` - Similar but different
- `draw_ui_elements` namespace should be `steamrot::logic::render`
- No consistent pattern documented

**Fix Time**: ~11 hours (Week 5 task)

### 5. TestEngine Underutilized

- TestEngine exists and works
- Needs foundational unit tests first
- Usage documentation incomplete

**Fix Time**: ~11 hours (Week 6-7 tasks)

---

## 📋 Quick Action Items

### This Week (Documentation)

```bash
# 1. Fix ADDING_LOGIC.md (2 hours)
#    - Replace TestContext → TestFixture
#    - Update all examples

# 2. Fix TEST_DATA_CONFIGURATION.md (1 hour)
#    - Update test infrastructure references

# 3. Fix TESTING_OVERVIEW.md (1 hour)
#    - Document TestFixture API
#    - Document TestEngine usage

# 4. Create TESTING_LOGIC_CLASSES.md (3 hours)
#    - Patterns for testing each Logic type
#    - Free function testing patterns

# 5. Create FREE_FUNCTION_GUIDELINES.md (2 hours)
#    - When to use free functions vs classes
#    - Namespace conventions
```

### Next 2 Weeks (Unit Tests)

```bash
# Week 2: Create Logic class tests
tests/unit/logic/UIActionLogic.test.cpp      # 4 hours
tests/unit/logic/UICollisionLogic.test.cpp   # 3 hours
tests/unit/logic/UIRenderLogic.test.cpp      # 3 hours
tests/unit/logic/CraftingRenderLogic.test.cpp # 2 hours
tests/unit/logic/UIStateLogic.test.cpp       # 3 hours

# Week 3: Expand free function tests
# Expand logic_action tests     # 4 hours
# Expand logic_collision tests  # 3 hours
# Expand logic_ui tests         # 3 hours
```

---

## 📊 Test Coverage Matrix

| Source File | Test File | Status | Priority |
|-------------|-----------|--------|----------|
| Logic.cpp | Logic.test.cpp | ✅ Exists | - |
| LogicFactory.cpp | LogicFactory.test.cpp | ✅ Exists | - |
| **UIActionLogic.cpp** | ❌ **Missing** | **Must create** | **HIGH** |
| **UICollisionLogic.cpp** | ❌ **Missing** | **Must create** | **HIGH** |
| **UIRenderLogic.cpp** | ❌ **Missing** | **Must create** | **HIGH** |
| **CraftingRenderLogic.cpp** | ❌ **Missing** | **Must create** | **HIGH** |
| **UIStateLogic.cpp** | ❌ **Missing** | **Must create** | **HIGH** |
| logic_action.cpp | ⚠️ Partial | Expand | MEDIUM |
| logic_collision.cpp | collision.test.cpp ⚠️ Partial | Expand | MEDIUM |
| logic_ui.cpp | ui_helpers.test.cpp ⚠️ Partial | Expand | MEDIUM |

---

## 🏗️ Architecture Patterns

### ✅ Good Patterns (Keep Using)

1. **Logic Classes + Free Functions**
   ```cpp
   class UIActionLogic : public Logic {
     void ProcessLogic() override {
       logic::action::ProcessNestedUIActionsAndEvents(...);
     }
   };
   ```
   - Free functions testable independently
   - Logic classes handle context management
   - Good for reusable logic

2. **Context System**
   ```
   GameCore → GameContext (refs)
   SceneCore + EntityManager → SceneContext (refs)
   ```
   - Clear ownership
   - Lightweight reference wrappers
   - No dangling references

3. **Entity-Component-Archetype**
   - Efficient iteration
   - Type-safe component access
   - Cache-friendly

### ⚠️ Patterns to Improve

1. **Namespace Organization**
   - CURRENT: Mixed patterns (event vs events, draw_ui_elements)
   - SHOULD BE: Consistent `steamrot::logic::*` pattern

2. **Code Reuse**
   - CURRENT: Some duplication in entity iteration
   - SHOULD BE: Extract common patterns to utilities

---

## 🧪 Test Infrastructure

### Current Status

| Component | Status | Usage |
|-----------|--------|-------|
| **TestFixture** | ✅ Complete | Unit tests - Use this! |
| **TestEngine** | ✅ Complete | Integration/Data-driven tests |
| **Component Matchers** | ✅ Complete | All components covered |
| **EntityMemoryPoolMatcher** | ✅ Complete | Full pool comparison |
| LogicTestBase | ❌ Missing | Need to create |
| ComponentTestMixin | ❌ Missing | Need to create |
| TestScenarioBuilder | ❌ Missing | Need to create |

### When to Use What

```cpp
// UNIT TESTS → Use TestFixture
TEST_CASE("Test Logic class", "[unit]") {
  TestFixture fixture;
  fixture.Initialize();
  auto& context = fixture.GetSceneContext();
  MyLogic logic(context);
  logic.RunLogic();
  REQUIRE(expected);
}

// DATA-DRIVEN TESTS → Use TestEngine
TEST_CASE("Test simulation", "[integration]") {
  auto configs = load_test_data_configs();
  TestEngine engine(configs[0]);
  engine.RunGame();
  auto& results = engine.GetDataBank();
  // Compare results...
}
```

---

## 📚 Missing Documentation

Need to create:

1. **TESTING_LOGIC_CLASSES.md** (Week 1)
   - How to test each Logic type
   - Patterns and examples
   - Free function testing

2. **FREE_FUNCTION_GUIDELINES.md** (Week 1)
   - When to use free functions
   - Namespace conventions
   - Organization patterns

3. **TEST_ENGINE_USAGE.md** (Week 6)
   - TestEngine vs TestFixture
   - Creating test data
   - Interpreting results

4. **UTILITY_FUNCTION_LIBRARY.md** (Week 5)
   - Common patterns extracted
   - Reusable utilities
   - API documentation

---

## 🎯 Success Criteria

### Week 1 Goal
- [ ] Zero TestContext references in docs
- [ ] All examples compile
- [ ] New testing guides created

### Week 2-3 Goal
- [ ] All 5 Logic classes have tests
- [ ] Free function tests expanded
- [ ] Test coverage > 80% for logic layer

### Month 1 Goal
- [ ] All documentation up to date
- [ ] All tests passing
- [ ] Test infrastructure complete
- [ ] Clear data-driven testing path

---

## 🔗 Quick Links

- **Full Analysis**: [COMPREHENSIVE_CODEBASE_ANALYSIS.md](./COMPREHENSIVE_CODEBASE_ANALYSIS.md)
- **Diagrams**: [ARCHITECTURE_DIAGRAMS.md](./ARCHITECTURE_DIAGRAMS.md)
- **8-Week Plan**: See Implementation Plan section in full analysis
- **Existing Workflows**: `documentation/workflows/`
- **Test Examples**: `tests/unit/logic/`

---

## 💡 Pro Tips

### For New Developers

1. **Read this first**: This document
2. **Then read**: ARCHITECTURE_DIAGRAMS.md for visual understanding
3. **Then read**: Specific workflow docs as needed
4. **Ignore references to**: TestContext (it's deprecated)
5. **Use instead**: TestFixture for unit tests

### For Testing

1. **Unit test a Logic class?** → Use TestFixture
2. **Need test data?** → Create test_data.json, use TestEngine
3. **Compare components?** → Use EntityMemoryPoolEqualsMatcher
4. **Need mock context?** → TestFixture creates everything

### For Code Review

1. **Logic class changes?** → Verify unit test exists
2. **Free function added?** → Verify test coverage
3. **New namespace?** → Check consistency with patterns
4. **Documentation updated?** → Verify no TestContext references

---

## 📞 Questions?

- See full analysis for detailed explanations
- Check architecture diagrams for visual reference
- Review existing test files for patterns
- Consult style guide for coding standards

**Remember**: This is a planning document. Implementation happens in phases over 8 weeks. The analysis identifies what needs to be done, not what has been done yet.
