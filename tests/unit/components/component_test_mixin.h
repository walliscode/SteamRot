/////////////////////////////////////////////////
/// @file
/// @brief Declaration of ComponentTestMixin template for reusable component tests
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "Component.h"
#include "containers.h"
#include <catch2/catch_test_macros.hpp>
#include <type_traits>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class ComponentTestMixin
/// @brief Template providing reusable test methods for Component types
///
/// This mixin provides standard tests that all Component types should pass.
/// Usage:
/// @code
/// TEST_CASE("MyComponent follows Component contract", "[unit][MyComponent]") {
///   ComponentTestMixin<MyComponent>::TestDefaultConstruction();
///   ComponentTestMixin<MyComponent>::TestComponentRegisterIndex();
/// }
/// @endcode
///
/// @tparam TComponent The Component type to test
/////////////////////////////////////////////////
template <typename TComponent>
class ComponentTestMixin {
  static_assert(std::is_base_of<Component, TComponent>::value,
                "TComponent must derive from Component");

public:
  /////////////////////////////////////////////////
  /// @brief Test that component is default-constructible
  ///
  /// Verifies that:
  /// - Component can be default-constructed without errors
  /// - m_active is initialized to false
  /// - All member variables have sensible default values
  /////////////////////////////////////////////////
  static void TestDefaultConstruction() {
    TComponent component;
    
    // All components should start inactive
    REQUIRE_FALSE(component.m_active);
    
    // Verify default construction succeeded
    SUCCEED("Component default-constructed successfully");
  }

  /////////////////////////////////////////////////
  /// @brief Test that GetComponentRegisterIndex returns correct value
  ///
  /// Verifies that:
  /// - GetComponentRegisterIndex() returns a valid index
  /// - Index is less than the size of ComponentRegister
  /// - Index matches the component's position in ComponentRegister
  /////////////////////////////////////////////////
  static void TestComponentRegisterIndex() {
    TComponent component;
    
    size_t index = component.GetComponentRegisterIndex();
    
    // Index should be within valid range
    REQUIRE(index < std::tuple_size<ComponentRegister>::value);
    
    // Verify index matches actual position using TupleTypeIndex
    size_t expected_index = TupleTypeIndex<TComponent, ComponentRegister>;
    REQUIRE(index == expected_index);
  }

  /////////////////////////////////////////////////
  /// @brief Test that component can be activated and deactivated
  ///
  /// Verifies that:
  /// - m_active can be set to true
  /// - m_active can be set back to false
  /// - State changes persist
  /////////////////////////////////////////////////
  static void TestActivation() {
    TComponent component;
    
    // Start inactive
    REQUIRE_FALSE(component.m_active);
    
    // Activate
    component.m_active = true;
    REQUIRE(component.m_active);
    
    // Deactivate
    component.m_active = false;
    REQUIRE_FALSE(component.m_active);
  }

  /////////////////////////////////////////////////
  /// @brief Test that component can be copy-constructed
  ///
  /// Verifies that:
  /// - Component is copyable
  /// - Copy has same values as original
  /////////////////////////////////////////////////
  static void TestCopyConstruction() {
    TComponent original;
    original.m_active = true;
    
    TComponent copy = original;
    
    REQUIRE(copy.m_active == original.m_active);
  }

  /////////////////////////////////////////////////
  /// @brief Test that component can be copy-assigned
  ///
  /// Verifies that:
  /// - Component is copy-assignable
  /// - Copy assignment preserves values
  /////////////////////////////////////////////////
  static void TestCopyAssignment() {
    TComponent original;
    original.m_active = true;
    
    TComponent copy;
    copy = original;
    
    REQUIRE(copy.m_active == original.m_active);
  }

  /////////////////////////////////////////////////
  /// @brief Run all standard component tests
  ///
  /// Convenience method to run all tests in one call.
  /////////////////////////////////////////////////
  static void RunAllTests() {
    TestDefaultConstruction();
    TestComponentRegisterIndex();
    TestActivation();
    TestCopyConstruction();
    TestCopyAssignment();
  }
};

} // namespace steamrot::tests
