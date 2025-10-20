/////////////////////////////////////////////////
/// @file
/// @brief Declaration of LogicTestBase template for reusable Logic tests
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "Logic.h"
#include "LogicContext.h"
#include "TestContext.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <type_traits>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class LogicTestBase
/// @brief Template providing reusable test infrastructure for Logic classes
///
/// This base class reduces boilerplate in Logic tests by providing:
/// - Standard test setup and teardown
/// - Common test scenarios (empty archetype, single entity, multiple entities)
/// - Helper methods for verification
///
/// Usage:
/// @code
/// class MyLogicTest : public LogicTestBase<MyLogic> {
/// protected:
///   void SetUp() override {
///     LogicTestBase<MyLogic>::SetUp();
///     // Additional setup
///   }
/// };
///
/// TEST_CASE_METHOD(MyLogicTest, "MyLogic processes correctly", "[unit][MyLogic]") {
///   TestConstruction();
///   TestProcessWithSingleEntity();
/// }
/// @endcode
///
/// @tparam TLogic The Logic class type to test
/////////////////////////////////////////////////
template <typename TLogic>
class LogicTestBase {
  static_assert(std::is_base_of<Logic, TLogic>::value,
                "TLogic must derive from Logic");

protected:
  /////////////////////////////////////////////////
  /// @brief Test context providing mock dependencies
  /////////////////////////////////////////////////
  std::unique_ptr<TestContext> test_context;

  /////////////////////////////////////////////////
  /// @brief The Logic instance under test
  /////////////////////////////////////////////////
  std::unique_ptr<TLogic> logic;

  /////////////////////////////////////////////////
  /// @brief Scene type for the test
  /////////////////////////////////////////////////
  SceneType scene_type;

  /////////////////////////////////////////////////
  /// @brief Setup test environment
  ///
  /// Override this method to add custom setup logic.
  /// Always call the base class SetUp() first.
  ///
  /// @param test_scene_type Scene type to use for testing
  /////////////////////////////////////////////////
  virtual void SetUp(SceneType test_scene_type = SceneType::SceneType_TEST) {
    scene_type = test_scene_type;
    test_context = std::make_unique<TestContext>(scene_type);
  }

  /////////////////////////////////////////////////
  /// @brief Create Logic instance with TestContext
  ///
  /// @return Unique pointer to Logic instance
  /////////////////////////////////////////////////
  std::unique_ptr<TLogic> CreateLogic() {
    return std::make_unique<TLogic>(
        test_context->GetLogicContextForTestScene());
  }

  /////////////////////////////////////////////////
  /// @brief Create Logic instance with custom LogicContext
  ///
  /// @param context Custom LogicContext to use
  /// @return Unique pointer to Logic instance
  /////////////////////////////////////////////////
  std::unique_ptr<TLogic> CreateLogicWithContext(const LogicContext &context) {
    return std::make_unique<TLogic>(context);
  }

public:
  /////////////////////////////////////////////////
  /// @brief Test that Logic can be constructed without errors
  /////////////////////////////////////////////////
  void TestConstruction() {
    SetUp();
    logic = CreateLogic();
    REQUIRE(logic != nullptr);
    SUCCEED("Logic constructed successfully");
  }

  /////////////////////////////////////////////////
  /// @brief Test Logic processing with empty archetype
  ///
  /// Verifies that Logic handles empty archetypes gracefully
  /// (i.e., no crashes, no errors).
  /////////////////////////////////////////////////
  void TestProcessWithEmptyArchetype() {
    SetUp();
    logic = CreateLogic();
    
    // Run logic with no entities
    REQUIRE_NOTHROW(logic->RunLogic());
    SUCCEED("Logic processed empty archetype without errors");
  }

  /////////////////////////////////////////////////
  /// @brief Test Logic processing with a single entity
  ///
  /// Override this method to provide entity setup for single-entity tests.
  /////////////////////////////////////////////////
  virtual void TestProcessWithSingleEntity() {
    SetUp();
    logic = CreateLogic();
    
    // Subclasses should override to add entity setup
    REQUIRE_NOTHROW(logic->RunLogic());
  }

  /////////////////////////////////////////////////
  /// @brief Test Logic processing with multiple entities
  ///
  /// Override this method to provide entity setup for multi-entity tests.
  ///
  /// @param entity_count Number of entities to create
  /////////////////////////////////////////////////
  virtual void TestProcessWithMultipleEntities(size_t entity_count = 10) {
    SetUp();
    logic = CreateLogic();
    
    // Subclasses should override to add entity setup
    (void)entity_count; // Suppress unused parameter warning
    REQUIRE_NOTHROW(logic->RunLogic());
  }

  /////////////////////////////////////////////////
  /// @brief Run all standard Logic tests
  ///
  /// Convenience method to run common tests.
  /////////////////////////////////////////////////
  void RunStandardTests() {
    TestConstruction();
    TestProcessWithEmptyArchetype();
    TestProcessWithSingleEntity();
    TestProcessWithMultipleEntities();
  }
};

} // namespace steamrot::tests
