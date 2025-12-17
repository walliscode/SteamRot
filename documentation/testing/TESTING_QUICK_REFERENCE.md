# Unit Testing Quick Reference

Quick reference guide for implementing unit tests in SteamRot. For comprehensive details, see [UNIT_TESTING_GUIDE.md](UNIT_TESTING_GUIDE.md).

## Test File Template

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for ClassName
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ClassName.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ClassName does something", "[unit][ClassName]") {
  // Arrange
  steamrot::ClassName object;
  
  // Act
  auto result = object.Method();
  
  // Assert
  REQUIRE(result == expected);
}
```

## Testing Checklist by Layer

### Layer 0: Core Types
- [ ] Default construction
- [ ] Field initialization
- [ ] Data integrity

### Layer 1: Components
- [ ] Default construction
- [ ] Default values
- [ ] `GetComponentRegisterIndex()`
- [ ] `m_active` flag
- [ ] Copy construction

### Layer 2: Data Providers & Configurators
- [ ] Successful data loading
- [ ] Error handling (invalid paths)
- [ ] Null safety (FlatBuffers fields)
- [ ] Data validation
- [ ] Component activation

### Layer 3: Managers
- [ ] Construction
- [ ] Resource operations (add, get, remove)
- [ ] State management
- [ ] Error conditions
- [ ] Memory safety

### Layer 4: Logic Classes
- [ ] Construction with SceneContext
- [ ] `RunLogic()` execution
- [ ] Entity processing by archetype
- [ ] State changes
- [ ] Subscriber handling

### Layer 5: Scenes
- [ ] Construction with GameContext
- [ ] Scene type initialization
- [ ] Render texture validity
- [ ] System method execution (`sRender`, `sAction`, etc.)
- [ ] Entity creation and configuration
- [ ] Active state changes

### Layer 6: Engine
- [ ] Construction
- [ ] Subsystem initialization
- [ ] Configuration loading
- [ ] Scene creation
- [ ] Resource initialization
- [ ] Error handling

## Common Patterns

### Testing std::expected

```cpp
// Success case
auto result = function();
REQUIRE(result.has_value());
REQUIRE(result.value() == expected);

// Error case
auto result = function();
REQUIRE_FALSE(result.has_value());
REQUIRE(result.error().fail_mode == FailMode::Expected);
```

### Testing with TestFixture

```cpp
TEST_CASE("Test with fixture", "[unit][Class]") {
  steamrot::tests::TestFixture fixture;
  
  auto& entity_manager = fixture.GetEntityManager();
  auto scene_context = fixture.GetSceneContext();
  
  // Use in test...
}
```

### Testing Entity Processing

```cpp
TEST_CASE("Logic processes entities", "[unit][Logic]") {
  steamrot::tests::TestFixture fixture;
  auto& pool = fixture.GetEntityManager().GetEntityMemoryPool();
  
  // Activate components
  auto& comp = steamrot::GetComponent<Component>(0, pool);
  comp.m_active = true;
  
  // Update archetypes
  fixture.GetEntityManager().UpdateArchetypes();
  
  // Run logic
  Logic logic(fixture.GetSceneContext());
  REQUIRE_NOTHROW(logic.RunLogic());
}
```

### Testing FlatBuffers Null Safety

```cpp
TEST_CASE("Configurator handles null fields", "[unit][Configurator]") {
  // Create data with null fields
  flatbuffers::FlatBufferBuilder builder;
  auto data = CreateData(builder, nullptr);
  
  auto result = configurator.Configure(data);
  
  // Should handle gracefully
  REQUIRE(result.has_value());
}
```

### Testing Resource Loading

```cpp
TEST_CASE("Manager loads resources", "[unit][Manager]") {
  Manager manager;
  
  auto result = manager.LoadResources();
  REQUIRE(result.has_value());
  
  // Verify loaded
  REQUIRE(manager.GetResourceCount() > 0);
  auto resource = manager.GetResource("name");
  REQUIRE(resource != nullptr);
}
```

## Test Organization

### Directory Structure
```
tests/unit/
├── assets/           # AssetManager tests
├── components/       # Component tests
├── data_providers/   # Data provider tests
├── display/          # DisplayManager tests
├── engine/           # GameEngine tests
├── entity/           # EntityManager, configurator tests
├── events/           # EventHandler tests
├── logic/            # Logic class tests
├── scenes/           # Scene tests
└── user_interface/   # UI element tests
```

### File Naming
- Test file: `ClassName.test.cpp`
- One test file per class
- Mirror `src/` directory structure

### Test Tags
- Always include: `[unit]`
- Always include: `[ClassName]`
- Optional: Additional descriptive tags

Example: `TEST_CASE("...", "[unit][AssetManager]")`

## Catch2 Assertions

### Basic Assertions
```cpp
REQUIRE(condition);              // Must be true
REQUIRE_FALSE(condition);        // Must be false
REQUIRE_NOTHROW(expression);     // Must not throw
REQUIRE_THROWS(expression);      // Must throw
REQUIRE_THROWS_AS(expr, Type);   // Must throw specific type
```

### Comparison Assertions
```cpp
REQUIRE(a == b);
REQUIRE(a != b);
REQUIRE(a < b);
REQUIRE(a <= b);
REQUIRE(a > b);
REQUIRE(a >= b);
```

### Container Assertions
```cpp
REQUIRE(container.empty());
REQUIRE(container.size() == n);
REQUIRE(container.contains(key));
```

### std::expected Assertions
```cpp
REQUIRE(result.has_value());      // Success
REQUIRE_FALSE(result.has_value()); // Error
REQUIRE(result.value() == x);     // Check value
REQUIRE(result.error().fail_mode == mode); // Check error
```

## Test Infrastructure

### TestFixture
**Location**: `tests/context/TestFixture.h`

Provides:
- EntityManager
- AssetManager
- SceneContext
- GameContext

```cpp
TestFixture();                          // Default (TEST scene)
TestFixture(SceneType scene_type);      // Specific scene

// Getters
GetEntityManager();
GetAssetManager();
GetSceneContext();
GetGameContext();
```

### Test Helpers
**Location**: `tests/unit/*/` subdirectories

- `asset_test_helpers.h` - Asset validation
- `logic_test_helpers.h` - Logic validation
- `TestUIElementDataProvider.h` - UI test data

### Test Matchers
**Location**: `tests/matchers/`

- `EntityMemoryPoolEqualsMatcher` - Pool comparison
- `CUserInterfaceEqualsMatcher` - UI comparison
- `EventBusEqualsMatcher` - Event bus comparison

## Best Practices

### DO ✅
- Test concrete implementations
- Use descriptive test names
- Test error conditions
- Use TestFixture for setup
- Check std::expected results
- Mirror src/ structure

### DON'T ❌
- Test abstract interfaces
- Test private methods directly
- Rely on test execution order
- Share state between tests
- Ignore return values
- Test generated code

## Quick Start Workflow

1. **Identify the layer** of the class to test
2. **Create test file** in matching `tests/unit/` subdirectory
3. **Add to CMakeLists.txt** in that directory
4. **Write tests** following the layer's checklist
5. **Use patterns** from this guide
6. **Follow style** from existing tests

## Coverage Priorities

Current gaps (high priority):
1. **Layer 5: Scenes** - TitleScene, CraftingScene, SceneFactory, SceneManager
2. **Layer 3: Managers** - Display utilities (Tile, ReTile, Session)
3. **Layer 6: Engine** - Expand GameEngine tests
4. **Layer 0: Types** - UI Element types, core config types

## Resources

- **Full Guide**: [UNIT_TESTING_GUIDE.md](UNIT_TESTING_GUIDE.md)
- **Catch2 Docs**: https://github.com/catchorg/Catch2/tree/devel/docs
- **Style Guide**: `documentation/style/STYLE_GUIDE.md`
- **Copilot Instructions**: `.github/copilot-instructions.md`

---

**Last Updated**: 2025-12-17
