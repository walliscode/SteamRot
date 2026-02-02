/////////////////////////////////////////////////
/// @file
/// @brief This file aggregates and runs all simulation tests.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "harness_runner.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Run all simulation tests", "[simulation][integration]") {

  auto simulation_result = steamrot::tests::RunHarnessTests(__FILE__);
  if (!simulation_result) {
    FAIL(simulation_result.error().message);
  }
}
