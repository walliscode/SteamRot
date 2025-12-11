/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for CMeta class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CMeta.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CMeta has correct default values", "[unit][Components][CMeta]") {
  steamrot::CMeta meta;

  // Test pre configuration state
  REQUIRE(meta.m_active == false);
  REQUIRE(meta.m_entity_alive == false);
}
