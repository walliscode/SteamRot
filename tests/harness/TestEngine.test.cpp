/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for TestEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestEngine.h"
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

TEST_CASE("TestEngine construction", "[unit][TestEngine]") {
  SECTION("Can be constructed with null config") {
    TestEngine engine(nullptr);
    // Verify construction succeeded by accessing the data bank
    const auto &data_bank = engine.GetDataBank();
    REQUIRE(data_bank.empty());
  }
}

TEST_CASE("TestEngine GetDataBank", "[unit][TestEngine]") {
  SECTION("Returns empty data bank for null config") {
    TestEngine engine(nullptr);
    const auto &data_bank = engine.GetDataBank();
    REQUIRE(data_bank.empty());
  }
}

} // namespace steamrot::tests
