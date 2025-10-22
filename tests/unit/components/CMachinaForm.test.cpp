/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for CMachinaForm class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CMachinaForm.h"
#include "component_test_mixin.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CMachinaForm follows Component contract",
          "[unit][Components][CMachinaForm]") {
  steamrot::tests::ComponentTestMixin<steamrot::CMachinaForm>::RunAllTests();
}

TEST_CASE("CMachinaForm has correct default values",
          "[unit][Components][CMachinaForm]") {
  steamrot::CMachinaForm form;
  
  // Test pre configuration state
  REQUIRE(form.m_active == false);
  REQUIRE(form.m_fragments.empty());
  REQUIRE(form.m_joints.empty());
}
