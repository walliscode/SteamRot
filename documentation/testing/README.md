# Testing Documentation Overview

This directory contains comprehensive testing documentation for the SteamRot game engine.

## Documentation Index

### 📘 Guides

#### [Unit Testing Guide](UNIT_TESTING_GUIDE.md)
**Comprehensive layer-by-layer guide to implementing unit tests**

The complete guide for implementing unit tests across all architectural layers. Includes:
- Testing philosophy and principles
- Architecture overview
- Layer-by-layer testing approach (Layers 0-6)
- Test infrastructure documentation
- Best practices and common patterns
- Current coverage status and priorities

**Use this when**: You're implementing new tests or want to understand the testing strategy.

#### [Testing Quick Reference](TESTING_QUICK_REFERENCE.md)
**Quick reference for common testing patterns and checklists**

A concise reference guide with:
- Test file templates
- Testing checklists by layer
- Common testing patterns
- Quick start workflow
- Catch2 assertion reference

**Use this when**: You need a quick reminder of testing patterns or want a checklist.

#### [Testing Examples](TESTING_EXAMPLES.md)
**Complete real-world testing examples for each layer**

Full working examples demonstrating:
- Complete test files for each layer (0-6)
- Real code that can be adapted
- Edge cases and error handling
- CMakeLists.txt integration
- Common patterns in practice

**Use this when**: You want to see complete examples or need a template to adapt.

#### [Architecture Layers](ARCHITECTURE_LAYERS.md)
**Visual architecture diagram and layer dependencies**

Comprehensive architecture overview:
- Visual layer diagram with dependencies
- Detailed layer descriptions
- Directory mapping (src/ to tests/)
- Coverage status by layer
- Testing strategy and priorities

**Use this when**: You need to understand the architecture or identify which layer a class belongs to.

#### [Test Data Configuration](TEST_DATA_CONFIGURATION.md)
**Guide for data-driven testing with FlatBuffers**

Documentation for the test data system:
- Test data file structure
- JSON to FlatBuffers workflow
- Test harness usage
- Data-driven test patterns

**Use this when**: Writing data-driven tests or creating test_data.json files.

## Testing Philosophy

SteamRot follows **Test-Driven Development (TDD)** principles:

1. **Test concrete implementations only** - Focus on classes with actual implementations
2. **Layer-by-layer approach** - Test from foundation up to higher layers
3. **Isolated unit tests** - Use mocks and fixtures to isolate the class under test
4. **Clear organization** - Tests mirror the `src/` directory structure
5. **Descriptive naming** - Test cases clearly describe what is being tested

## Architecture Layers

Tests are organized by architectural dependency layers:

```
Layer 6: Engine (GameEngine)
         ↓
Layer 5: Scenes (TitleScene, CraftingScene)
         ↓
Layer 4: Logic (UIRenderLogic, UIActionLogic, UICollisionLogic)
         ↓
Layer 3: Managers (EntityManager, AssetManager, EventHandler)
         ↓
Layer 2: Data Providers & Configurators
         ↓
Layer 1: Components (CMeta, CUserInterface, CGrimoireMachina)
         ↓
Layer 0: Core Types (FailInfo, EventPacket, UIElement types)
```

**Key principle**: Test lower layers before higher layers that depend on them.

## Current Testing Coverage

| Layer | Coverage | Priority | Notes |
|-------|----------|----------|-------|
| Layer 0: Core Types | ⚠️ Partial | Medium | Events tested, UI types missing |
| Layer 1: Components | ✅ Good | Low | All components have basic tests |
| Layer 2: Data/Config | ✅ Good | Low | Most providers tested |
| Layer 3: Managers | ⚠️ Partial | High | Core managers tested, display utils missing |
| Layer 4: Logic | ✅ Good | Low | Base class and implementations tested |
| Layer 5: Scenes | ❌ Missing | **Critical** | No scene tests exist |
| Layer 6: Engine | ⚠️ Basic | High | Minimal coverage, needs expansion |

## Test Organization

### Directory Structure

```
tests/
├── unit/                    # Unit tests (organized by subsystem)
│   ├── assets/             # AssetManager tests
│   ├── components/         # Component tests
│   ├── data_providers/     # Data provider tests
│   ├── display/            # DisplayManager tests
│   ├── engine/             # GameEngine tests
│   ├── entity/             # Entity management tests
│   ├── events/             # EventHandler tests
│   ├── logic/              # Logic class tests
│   ├── scenes/             # Scene tests (TO BE CREATED)
│   └── user_interface/     # UI element tests
├── integration/            # Integration tests
├── context/                # Test utilities and fixtures
├── matchers/               # Custom Catch2 matchers
├── harness/                # Test harness infrastructure
└── data/                   # Test data files
```

### Test File Naming

- Unit tests: `ClassName.test.cpp`
- One test file per class
- Mirror `src/` directory structure under `tests/unit/`

### Test Tags

All tests must include appropriate Catch2 tags:
- `[unit]` - Unit tests (isolated with mocked dependencies)
- `[integration]` - Integration tests (multiple components)
- `[system]` - System tests (end-to-end)
- `[.visual]` - Visual confirmation tests (hidden, require user interaction)

Example:
```cpp
TEST_CASE("ClassName does something", "[unit][ClassName]") {
  // Test implementation
}
```

## Getting Started

### For New Contributors

1. **Read**: [Unit Testing Guide](UNIT_TESTING_GUIDE.md) - Start here for comprehensive overview
2. **Reference**: [Testing Quick Reference](TESTING_QUICK_REFERENCE.md) - Keep this handy
3. **Understand**: Architecture layers and dependencies
4. **Follow**: Existing test patterns in the codebase

### Writing Your First Test

1. Identify which layer your class belongs to
2. Create test file in matching `tests/unit/` subdirectory
3. Follow the template in [Testing Quick Reference](TESTING_QUICK_REFERENCE.md)
4. Use the layer-specific checklist from [Unit Testing Guide](UNIT_TESTING_GUIDE.md)
5. Run tests locally to verify

### Current Priorities

**High priority** (implement these first):
1. **Layer 5 tests** - Create tests for TitleScene, CraftingScene, SceneFactory, SceneManager
2. **Layer 3 expansion** - Add tests for display utilities (Tile, ReTile, Session)
3. **Layer 6 expansion** - Expand GameEngine test coverage
4. **Layer 0 completion** - Add tests for UI Element types and core config types

## Test Infrastructure

### TestFixture
**Location**: `tests/context/TestFixture.h`

Standard test setup providing:
- Pre-configured EntityManager
- Pre-configured AssetManager
- SceneContext for Logic testing
- GameContext for Scene testing

### Test Helpers
**Location**: `tests/unit/*/` subdirectories

Domain-specific test utilities:
- `asset_test_helpers.h` - Asset validation
- `logic_test_helpers.h` - Logic validation
- `TestUIElementDataProvider.h` - UI test data

### Custom Matchers
**Location**: `tests/matchers/`

Catch2 matchers for complex comparisons:
- `EntityMemoryPoolEqualsMatcher` - Entity pool comparison
- `CUserInterfaceEqualsMatcher` - UI component comparison
- `EventBusEqualsMatcher` - Event bus comparison

## Best Practices

### DO ✅
- Test concrete implementations (not abstract interfaces)
- Use descriptive test names
- Test error conditions and edge cases
- Use TestFixture for consistent setup
- Verify std::expected results
- Mirror src/ directory structure
- Tag tests appropriately

### DON'T ❌
- Test abstract interfaces
- Test private methods directly
- Rely on test execution order
- Share state between tests
- Ignore return values or error conditions
- Test generated code (FlatBuffers headers)
- Test third-party libraries

## Testing Workflow

1. **Write test first** (TDD approach)
2. **Implement minimal code** to pass test
3. **Run test locally** to verify
4. **Refactor** if needed
5. **Commit** with descriptive message

## Running Tests

Tests are run locally by developers. See main [README.md](../../README.md) for build instructions.

Quick reference:
```bash
# Configure (done once)
cmake --preset Debug

# Build
cmake --build --preset Debug

# Run all tests
ctest --preset Debug

# Run specific test type
ctest --preset Debug -L unit
```

## Resources

### Documentation
- [Unit Testing Guide](UNIT_TESTING_GUIDE.md) - Comprehensive testing guide
- [Testing Quick Reference](TESTING_QUICK_REFERENCE.md) - Quick patterns and checklists
- [Test Data Configuration](TEST_DATA_CONFIGURATION.md) - Data-driven testing
- [Copilot Instructions](../../.github/copilot-instructions.md) - Agent guidelines

### External Resources
- [Catch2 Documentation](https://github.com/catchorg/Catch2/tree/devel/docs)
- [Test-Driven Development](https://en.wikipedia.org/wiki/Test-driven_development)
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)

## Contributing

When adding new tests:
1. Follow the layer-by-layer approach
2. Use existing test patterns
3. Add to appropriate `tests/unit/` subdirectory
4. Update CMakeLists.txt in that directory
5. Document any new test helpers or patterns

## Questions?

- Check [Unit Testing Guide](UNIT_TESTING_GUIDE.md) for detailed explanations
- Look at existing tests for examples
- Refer to [Testing Quick Reference](TESTING_QUICK_REFERENCE.md) for patterns

---

**Last Updated**: 2025-12-17  
**Documentation Version**: 1.0
