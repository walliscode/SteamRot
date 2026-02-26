////////////////////////////////////////////////////////////
/// @file
/// @brief Tests for FlatbuffersInputActionConfigProvider.
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include "FlatbuffersInputActionConfigProvider.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FlatbuffersInputActionConfigProvider::CreateInputActionRegistry "
          "returns a non-empty registry for default config",
          "[unit][FlatbuffersInputActionConfigProvider]") {

  steamrot::FlatbuffersInputActionConfigProvider provider;
  auto result = provider.CreateInputActionRegistry();
  REQUIRE(result.has_value());
  REQUIRE_FALSE(result.value().empty());
}
