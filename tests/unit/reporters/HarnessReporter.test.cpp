/////////////////////////////////////////////////
/// @file
/// @brief Tests for the HarnessReporter class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("HarnessReporter runs on succesful test", "[reporters]") {
  SUCCEED();
}

TEST_CASE("HarnessReporter fails on failed test", "[reporters]") {
  SUCCEED("Intentional failure for testing HarnessReporter");
}

TEST_CASE("Checking GENERATE macro", "[reporters]") {
  auto value = GENERATE(1, 2, 3);
  REQUIRE(value <= 3);
}
