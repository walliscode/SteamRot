# Factory Pattern Implementation Guide

## Overview

This guide provides a step-by-step checklist for implementing the DataAccessFactory pattern described in `DATA_PROVIDER_CONFIGURATOR_FACTORY_PATTERN.md`. Follow these steps in order, validating at each checkpoint.

**Estimated Time**: 2-3 days  
**Prerequisites**: Read the analysis document first  
**Goal**: Replace hardcoded static providers/configurators with a configurable factory system

---

## Phase 1: Create Factory Infrastructure ⏱️ ~4 hours

### Step 1.1: Create DataFormatType Enum

- [ ] **File**: `src/types/core/DataFormatType.h`
- [ ] **Action**: Create new file with enum definition
- [ ] **Content**:
  ```cpp
  ////////////////////////////////////////////////////////////
  /// @file
  /// @brief Declaration of DataFormatType enum for data source selection
  ////////////////////////////////////////////////////////////
  
  #pragma once
  
  #include <cstdint>
  
  namespace steamrot {
  
  ////////////////////////////////////////////////////////////
  /// @brief Enum for supported data format types
  ///
  /// Determines which data provider/configurator implementations
  /// to use for loading and configuring game data.
  ////////////////////////////////////////////////////////////
  enum class DataFormatType : uint8_t {
    FlatBuffers = 0,  ///< FlatBuffers binary format (default)
    XML = 1,          ///< XML format (future support)
    JSON = 2,         ///< JSON format (future support)
    Test = 3,         ///< Test mock data (for unit/integration tests)
  };
  
  } // namespace steamrot
  ```
- [ ] **Update**: `src/types/CMakeLists.txt` - Add new header to sources
- [ ] **Validate**: File compiles without errors

**Checkpoint 1.1**: ✅ Enum exists and compiles

---

### Step 1.2: Create DataAccessFactory Header

- [ ] **File**: `src/engine/DataAccessFactory.h`
- [ ] **Action**: Create factory class interface
- [ ] **Content Structure**:
  - Include all provider/configurator interfaces
  - Include DataFormatType enum
  - Class with private members for cached instances
  - Public constructor taking DataFormatType
  - Public getter methods for each provider/configurator
  - Factory methods for stateful configurators
- [ ] **Key Methods**:
  ```cpp
  // Providers (cached, returned by reference)
  IEngineDataProvider& GetEngineDataProvider();
  ISceneDataProvider& GetSceneDataProvider();
  IAssetDataProvider& GetAssetDataProvider();
  ISceneManagerDataProvider& GetSceneManagerDataProvider();
  
  // Configurators (cached, returned by reference)
  ISceneConfigurator& GetSceneConfigurator();
  
  // Factory methods (created on-demand)
  std::unique_ptr<IEntityConfigurator> CreateEntityConfigurator(
      EventHandler& event_handler,
      const void* entity_collection_data);
  
  std::unique_ptr<IUIElementConfigurator> CreateUIElementConfigurator(
      EventHandler& event_handler,
      const void* ui_data);
  ```
- [ ] **Validate**: Header compiles, no syntax errors

**Checkpoint 1.2**: ✅ Factory header complete

---

### Step 1.3: Implement DataAccessFactory

- [ ] **File**: `src/engine/DataAccessFactory.cpp`
- [ ] **Action**: Implement factory logic
- [ ] **Constructor**:
  ```cpp
  DataAccessFactory::DataAccessFactory(DataFormatType format_type)
      : m_data_format_type(format_type) {
    InitializeProviders();
    InitializeConfigurators();
  }
  ```
- [ ] **InitializeProviders()**: Create provider instances based on format type
  ```cpp
  void DataAccessFactory::InitializeProviders() {
    switch (m_data_format_type) {
    case DataFormatType::FlatBuffers:
      m_engine_provider = std::make_unique<FlatbuffersEngineDataProvider>();
      m_scene_provider = std::make_unique<FlatbuffersSceneDataProvider>();
      m_asset_provider = std::make_unique<FlatbuffersAssetDataProvider>();
      m_scene_manager_provider = std::make_unique<FlatbuffersSceneManagerDataProvider>();
      break;
    case DataFormatType::Test:
      // For now, use FlatBuffers (test providers come later)
      m_engine_provider = std::make_unique<FlatbuffersEngineDataProvider>();
      // ... same for others
      break;
    default:
      // Log error or throw
      break;
    }
  }
  ```
- [ ] **InitializeConfigurators()**: Create configurator instances
- [ ] **Getter methods**: Return references to cached instances
- [ ] **Factory methods**: Create configurators with proper casting for data types
- [ ] **Validate**: Implementation compiles

**Checkpoint 1.3**: ✅ Factory implementation complete

---

### Step 1.4: Update CMakeLists

- [ ] **File**: `src/engine/CMakeLists.txt`
- [ ] **Action**: Add DataAccessFactory to target sources
- [ ] **Content**:
  ```cmake
  target_sources(engine
    PRIVATE
      Engine.cpp
      GameEngine.cpp
      engine_configuration.cpp
      DataAccessFactory.cpp  # ← Add this
    PUBLIC
      FILE_SET HEADERS
      FILES
        Engine.h
        GameEngine.h
        engine_configuration.h
        DataAccessFactory.h  # ← Add this
  )
  ```
- [ ] **Validate**: Project builds successfully

**Checkpoint 1.4**: ✅ Factory builds with project

---

### Step 1.5: Write Factory Unit Tests

- [ ] **File**: `tests/unit/engine/DataAccessFactory.test.cpp`
- [ ] **Action**: Create comprehensive unit tests
- [ ] **Test Cases**:
  ```cpp
  TEST_CASE("DataAccessFactory constructor with FlatBuffers", "[unit][DataAccessFactory]") {
    DataAccessFactory factory(DataFormatType::FlatBuffers);
    REQUIRE(factory.GetDataFormatType() == DataFormatType::FlatBuffers);
  }
  
  TEST_CASE("DataAccessFactory provides engine data provider", "[unit][DataAccessFactory]") {
    DataAccessFactory factory(DataFormatType::FlatBuffers);
    IEngineDataProvider& provider = factory.GetEngineDataProvider();
    // Verify it's FlatBuffersEngineDataProvider
    auto* fb_provider = dynamic_cast<FlatbuffersEngineDataProvider*>(&provider);
    REQUIRE(fb_provider != nullptr);
  }
  
  TEST_CASE("DataAccessFactory provides scene data provider", "[unit][DataAccessFactory]") {
    // Similar test for scene provider
  }
  
  TEST_CASE("DataAccessFactory provides scene configurator", "[unit][DataAccessFactory]") {
    // Similar test for configurator
  }
  
  TEST_CASE("DataAccessFactory creates entity configurator", "[unit][DataAccessFactory]") {
    // Test CreateEntityConfigurator factory method
  }
  ```
- [ ] **Update**: `tests/unit/engine/CMakeLists.txt` - Add test file
- [ ] **Validate**: Run tests with `ctest --preset Debug -R DataAccessFactory`

**Checkpoint 1.5**: ✅ Factory tests pass

---

## Phase 2: Integrate Factory into Engine ⏱️ ~3 hours

### Step 2.1: Add Factory Member to Engine

- [ ] **File**: `src/engine/Engine.h`
- [ ] **Action**: Add factory as member and update constructor
- [ ] **Changes**:
  ```cpp
  class Engine {
  protected:
    // ... existing members ...
    
    ////////////////////////////////////////////////////////////
    /// @brief Data access factory for providers and configurators
    ////////////////////////////////////////////////////////////
    DataAccessFactory m_data_access_factory;
  
  public:
    ////////////////////////////////////////////////////////////
    /// @brief Constructor with optional data format type
    ///
    /// @param format_type Data format to use (defaults to FlatBuffers)
    ////////////////////////////////////////////////////////////
    explicit Engine(DataFormatType format_type = DataFormatType::FlatBuffers);
    
    ////////////////////////////////////////////////////////////
    /// @brief Get the data access factory
    ////////////////////////////////////////////////////////////
    DataAccessFactory& GetDataAccessFactory() { return m_data_access_factory; }
  };
  ```
- [ ] **Validate**: Header compiles

**Checkpoint 2.1**: ✅ Engine header updated

---

### Step 2.2: Update Engine Constructor

- [ ] **File**: `src/engine/Engine.cpp`
- [ ] **Action**: Initialize factory in constructor
- [ ] **Changes**:
  ```cpp
  Engine::Engine(DataFormatType format_type)
      : m_data_access_factory(format_type),
        m_engine_resources(),
        m_game_context(m_engine_resources),
        m_scene_manager(m_game_context) {}
  ```
- [ ] **Validate**: Implementation compiles

**Checkpoint 2.2**: ✅ Engine constructor updated

---

### Step 2.3: Update Engine::StartUp()

- [ ] **File**: `src/engine/Engine.cpp`
- [ ] **Action**: Use factory instead of `GetEngineDataProvider()`
- [ ] **Changes**:
  ```cpp
  std::expected<std::monostate, FailInfo> Engine::StartUp() {
    // OLD: IEngineDataProvider &data_provider = GetEngineDataProvider();
    // NEW:
    IEngineDataProvider &data_provider = m_data_access_factory.GetEngineDataProvider();
    
    // Rest of the method stays the same
    auto resources_config_result = data_provider.LoadEngineResourcesConfig();
    // ...
  }
  ```
- [ ] **Validate**: Implementation compiles

**Checkpoint 2.3**: ✅ Engine uses factory for data access

---

### Step 2.4: Update GameEngine Constructor

- [ ] **File**: `src/engine/GameEngine.h`
- [ ] **Action**: Add format_type parameter to constructor
- [ ] **Changes**:
  ```cpp
  class GameEngine : public Engine {
  public:
    ////////////////////////////////////////////////////////////
    /// @brief Constructor with optional data format type
    ///
    /// @param format_type Data format to use (defaults to FlatBuffers)
    ////////////////////////////////////////////////////////////
    explicit GameEngine(DataFormatType format_type = DataFormatType::FlatBuffers);
  };
  ```
- [ ] **File**: `src/engine/GameEngine.cpp`
- [ ] **Action**: Pass format_type to base constructor
- [ ] **Changes**:
  ```cpp
  GameEngine::GameEngine(DataFormatType format_type)
      : Engine(format_type) {}
  ```
- [ ] **Validate**: GameEngine compiles

**Checkpoint 2.4**: ✅ GameEngine updated

---

### Step 2.5: Update Main Entry Point

- [ ] **File**: `steamrot/main.cpp` (or wherever game is launched)
- [ ] **Action**: Update GameEngine instantiation
- [ ] **Changes**:
  ```cpp
  int main() {
    // Can specify format type if needed
    // DataFormatType format = DataFormatType::FlatBuffers;
    // GameEngine engine(format);
    
    // Or use default (FlatBuffers)
    GameEngine engine;
    engine.RunGame();
    return 0;
  }
  ```
- [ ] **Validate**: Game runs successfully

**Checkpoint 2.5**: ✅ Main entry point updated

---

### Step 2.6: Write Engine Integration Tests

- [ ] **File**: `tests/integration/engine/EngineDataAccess.test.cpp`
- [ ] **Action**: Test engine with factory
- [ ] **Test Cases**:
  ```cpp
  TEST_CASE("Engine initializes with DataAccessFactory", "[integration][Engine]") {
    // Create test engine
    steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
    
    // Using FlatBuffers format
    TestEngine engine(DataFormatType::FlatBuffers);
    
    // Verify factory is accessible
    auto& factory = engine.GetDataAccessFactory();
    REQUIRE(factory.GetDataFormatType() == DataFormatType::FlatBuffers);
  }
  
  TEST_CASE("Engine StartUp uses factory providers", "[integration][Engine]") {
    // Test that StartUp successfully uses factory
  }
  ```
- [ ] **Update**: Integration test CMakeLists.txt
- [ ] **Validate**: Run integration tests

**Checkpoint 2.6**: ✅ Engine integration tests pass

---

## Phase 3: Update SceneFactory and Configurators ⏱️ ~4 hours

### Step 3.1: Add Factory to SceneFactory

- [ ] **File**: `src/scenes/SceneFactory.h`
- [ ] **Action**: Add factory reference member
- [ ] **Changes**:
  ```cpp
  class SceneFactory {
  protected:
    const GameContext &m_game_context;
    DataAccessFactory &m_data_access_factory;  // ← Add this
  
  public:
    ////////////////////////////////////////////////////////////
    /// @brief Constructor for SceneFactory
    ///
    /// @param game_context Reference to game context
    /// @param data_access_factory Reference to data access factory
    ////////////////////////////////////////////////////////////
    SceneFactory(const GameContext &game_context,
                 DataAccessFactory &data_access_factory);
  };
  ```
- [ ] **Validate**: Header compiles

**Checkpoint 3.1**: ✅ SceneFactory header updated

---

### Step 3.2: Update SceneFactory Implementation

- [ ] **File**: `src/scenes/SceneFactory.cpp`
- [ ] **Action**: Use factory instead of static function
- [ ] **Changes in constructor**:
  ```cpp
  SceneFactory::SceneFactory(const GameContext &game_context,
                             DataAccessFactory &data_access_factory)
      : m_game_context(game_context),
        m_data_access_factory(data_access_factory) {}
  ```
- [ ] **Changes in CreateSceneFromDefault()**:
  ```cpp
  std::expected<std::unique_ptr<Scene>, FailInfo>
  SceneFactory::CreateSceneFromDefault(SceneType type) {
    // OLD: ISceneDataProvider &provider = GetSceneDataProvider();
    // NEW:
    ISceneDataProvider &provider = m_data_access_factory.GetSceneDataProvider();
    
    // OLD: ISceneConfigurator &configurator = GetSceneConfigurator();
    // NEW:
    ISceneConfigurator &configurator = m_data_access_factory.GetSceneConfigurator();
    
    // Rest stays the same
  }
  ```
- [ ] **Remove**: `GetSceneConfigurator()` function (was static, no longer needed)
- [ ] **Validate**: Implementation compiles

**Checkpoint 3.2**: ✅ SceneFactory uses factory

---

### Step 3.3: Update SceneManager to Use Factory

- [ ] **File**: `src/scenes/SceneManager.h`
- [ ] **Action**: Check if SceneManager creates SceneFactory
- [ ] **If yes**: Add factory reference to SceneManager constructor
- [ ] **Update**: SceneManager to pass factory to SceneFactory
- [ ] **Validate**: SceneManager compiles

**Checkpoint 3.3**: ✅ SceneManager integration complete

---

### Step 3.4: Update FlatbuffersSceneConfigurator

- [ ] **File**: `src/scenes/FlatbuffersSceneConfigurator.h`
- [ ] **Action**: Add factory reference
- [ ] **Changes**:
  ```cpp
  class FlatbuffersSceneConfigurator : public ISceneConfigurator {
  private:
    DataAccessFactory &m_data_access_factory;  // ← Add this
  
  public:
    ////////////////////////////////////////////////////////////
    /// @brief Constructor with factory reference
    ////////////////////////////////////////////////////////////
    explicit FlatbuffersSceneConfigurator(DataAccessFactory &factory);
  };
  ```
- [ ] **Validate**: Header compiles

**Checkpoint 3.4**: ✅ Scene configurator header updated

---

### Step 3.5: Update ConfigureEntities Method

- [ ] **File**: `src/scenes/FlatbuffersSceneConfigurator.cpp`
- [ ] **Action**: Use factory to create entity configurator
- [ ] **Changes in ConfigureEntities()**:
  ```cpp
  std::expected<std::monostate, FailInfo>
  FlatbuffersSceneConfigurator::ConfigureEntities(Scene &scene, const SceneData *scene_data) {
    // ... existing validation code ...
    
    // OLD: Direct instantiation
    // FlatbuffersEntityConfigurator entity_configurator(
    //     scene.GetSceneContext().event_handler,
    //     *fbs_scene_data->scene_data_fbs->entity_collection());
    
    // NEW: Use factory
    auto entity_configurator = m_data_access_factory.CreateEntityConfigurator(
        scene.GetSceneContext().event_handler,
        fbs_scene_data->scene_data_fbs->entity_collection());
    
    // Update method call
    auto emp_config_result = entity_configurator->ConfigureEntityMemoryPool(
        scene.GetSceneContext().scene_entities);
    
    // ... rest of method
  }
  ```
- [ ] **Validate**: Implementation compiles

**Checkpoint 3.5**: ✅ Entity configurator uses factory

---

### Step 3.6: Update Entity Configurator for UI Elements

- [ ] **File**: `src/entity/FlatbuffersEntityConfigurator.h`
- [ ] **Action**: Add factory reference member
- [ ] **Changes**: Add `DataAccessFactory &m_data_access_factory;` member
- [ ] **Update constructor**: Accept factory reference
- [ ] **File**: `src/entity/FlatbuffersEntityConfigurator.cpp`
- [ ] **Action**: Use factory to create UI element configurator
- [ ] **Changes in ConfigureCUserInterface()**:
  ```cpp
  // OLD: Direct instantiation
  // FlatbuffersUIElementConfigurator ui_configurator(m_event_handler, *ui_data);
  
  // NEW: Use factory
  auto ui_configurator = m_data_access_factory.CreateUIElementConfigurator(
      m_event_handler, ui_data);
  
  auto root_result = ui_configurator->CreateRootUIElement();
  ```
- [ ] **Validate**: Implementation compiles

**Checkpoint 3.6**: ✅ UI element configurator uses factory

---

### Step 3.7: Write Scene Factory Tests

- [ ] **File**: `tests/unit/scenes/SceneFactory.test.cpp`
- [ ] **Action**: Update tests to use factory
- [ ] **Changes**: Pass factory to SceneFactory constructor in all tests
- [ ] **Validate**: Run scene factory tests

**Checkpoint 3.7**: ✅ Scene factory tests pass

---

## Phase 4: Remove Old Provider Factory System ⏱️ ~2 hours

### Step 4.1: Identify All Usages

- [ ] **Action**: Search for uses of old factory functions
- [ ] **Command**: `grep -r "GetEngineDataProvider\|GetSceneDataProvider\|GetAssetDataProvider" src/`
- [ ] **List all files**: Make note of every file using old factory functions
- [ ] **Plan updates**: For each file, plan how to get factory reference

**Checkpoint 4.1**: ✅ Usage inventory complete

---

### Step 4.2: Update Remaining Call Sites

- [ ] **For each file found**: 
  - Add factory parameter to function/constructor if needed
  - Replace `GetXxxDataProvider()` with `factory.GetXxxDataProvider()`
  - Update tests for that component
- [ ] **Common files to check**:
  - Display/rendering code
  - Asset loading code
  - Scene management code
- [ ] **Validate**: Each file compiles after update

**Checkpoint 4.2**: ✅ All call sites updated

---

### Step 4.3: Remove Old Factory Files

- [ ] **Delete**: `src/data_providers/provider_factory.h`
- [ ] **Delete**: `src/data_providers/provider_factory.cpp`
- [ ] **Update**: `src/data_providers/CMakeLists.txt` - remove factory files
- [ ] **Validate**: Project builds without errors

**Checkpoint 4.3**: ✅ Old factory removed

---

### Step 4.4: Update Include Statements

- [ ] **Action**: Search for includes of old factory
- [ ] **Command**: `grep -r "#include.*provider_factory" src/`
- [ ] **For each file**: Remove the include, add DataAccessFactory include if needed
- [ ] **Validate**: Project builds

**Checkpoint 4.4**: ✅ Includes cleaned up

---

## Phase 5: Testing and Validation ⏱️ ~4 hours

### Step 5.1: Run Full Unit Test Suite

- [ ] **Command**: `ctest --preset Debug -L unit`
- [ ] **Fix any failures**: Address test failures related to factory changes
- [ ] **Validate**: All unit tests pass

**Checkpoint 5.1**: ✅ Unit tests pass

---

### Step 5.2: Run Integration Tests

- [ ] **Command**: `ctest --preset Debug -L integration`
- [ ] **Fix any failures**: Address integration test failures
- [ ] **Validate**: All integration tests pass

**Checkpoint 5.2**: ✅ Integration tests pass

---

### Step 5.3: Run Full Game

- [ ] **Action**: Run the full game application
- [ ] **Test**: Launch game, navigate menus, test scenes
- [ ] **Verify**: No crashes, data loads correctly
- [ ] **Check**: All scenes work as expected

**Checkpoint 5.3**: ✅ Game runs successfully

---

### Step 5.4: Test with Different Format Types

- [ ] **Action**: Test factory with DataFormatType::Test
- [ ] **Create**: Simple test provider if needed
- [ ] **Verify**: Factory can switch between formats
- [ ] **Document**: Any limitations or future work needed

**Checkpoint 5.4**: ✅ Format switching works

---

### Step 5.5: Performance Validation

- [ ] **Action**: Compare performance before/after changes
- [ ] **Measure**: Engine startup time
- [ ] **Measure**: Scene creation time
- [ ] **Validate**: No significant performance regression

**Checkpoint 5.5**: ✅ Performance acceptable

---

### Step 5.6: Code Review Preparation

- [ ] **Action**: Review all changed files
- [ ] **Check**: Code follows style guide
- [ ] **Check**: All methods documented with Doxygen comments
- [ ] **Check**: No commented-out code or debug statements
- [ ] **Update**: Any affected documentation

**Checkpoint 5.6**: ✅ Code review ready

---

## Final Checklist

### Code Quality
- [ ] All new files follow Google C++ Style Guide
- [ ] All functions have Doxygen comments
- [ ] No compiler warnings
- [ ] No memory leaks (run with valgrind if needed)
- [ ] Consistent naming conventions used

### Testing
- [ ] All unit tests pass
- [ ] All integration tests pass
- [ ] Manual game testing successful
- [ ] Edge cases tested (invalid format types, null pointers)

### Documentation
- [ ] README.md updated if needed
- [ ] Architecture diagrams updated if needed
- [ ] Implementation guide (this document) checked off
- [ ] Comments added for complex logic

### Git
- [ ] Commits are well-organized with clear messages
- [ ] No unnecessary files committed (build artifacts, etc.)
- [ ] Branch is up to date with main
- [ ] Ready for pull request review

---

## Troubleshooting

### Common Issues

**Issue**: Compiler can't find DataAccessFactory
- **Solution**: Check CMakeLists.txt has factory files added
- **Solution**: Verify includes are correct

**Issue**: Factory returns wrong provider type
- **Solution**: Check switch statement in InitializeProviders()
- **Solution**: Verify format_type is set correctly

**Issue**: Tests fail after factory integration
- **Solution**: Update test fixtures to pass factory reference
- **Solution**: Create test-specific factory with Test format type

**Issue**: Circular dependencies
- **Solution**: Use forward declarations in headers
- **Solution**: Move includes to .cpp files where possible

**Issue**: Scene creation fails
- **Solution**: Ensure factory is passed through constructor chain
- **Solution**: Check SceneManager → SceneFactory → Configurators

---

## Time Estimates

| Phase | Tasks | Estimated Time |
|-------|-------|----------------|
| Phase 1 | Factory infrastructure | 4 hours |
| Phase 2 | Engine integration | 3 hours |
| Phase 3 | Scene/configurator updates | 4 hours |
| Phase 4 | Remove old system | 2 hours |
| Phase 5 | Testing and validation | 4 hours |
| **Total** | | **17 hours (~2-3 days)** |

---

## Notes

- This guide assumes familiarity with the analysis document
- Each checkpoint should be validated before moving forward
- Tests should be written alongside implementation, not after
- Commit frequently with meaningful messages
- If stuck, refer back to the analysis document for design decisions
- Keep old factory system working until Phase 4 (backward compatibility)

---

## Success Criteria

Implementation is complete when:

1. ✅ DataAccessFactory exists and can create all providers/configurators
2. ✅ Engine uses factory for all data access
3. ✅ Old provider_factory system is removed
4. ✅ All tests pass
5. ✅ Game runs without errors
6. ✅ Code is documented and follows style guide
7. ✅ Ready for code review

---

## Next Steps After Implementation

1. **Review**: Submit PR for code review
2. **Iterate**: Address review feedback
3. **Merge**: Merge to main branch
4. **Document**: Update user-facing documentation if needed
5. **Plan**: Consider implementing XML/JSON providers (future work)

---

**Reference**: See `DATA_PROVIDER_CONFIGURATOR_FACTORY_PATTERN.md` for detailed analysis and architecture decisions.
