/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersDataLoader
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersDataLoader.h"
#include <catch2/catch_test_macros.hpp>

// create FlatbuffersDataLoader object
steamrot::FlatbuffersDataLoader flatbuffersDataLoader;

TEST_CASE("fails to find binary file for fragment data",
          "[FlatbuffersDataLoader]") {

  auto result = flatbuffersDataLoader.ProvideFragment("non_existent_fragment");
  REQUIRE(result.error() == steamrot::DataFailMode::FileNotFound);
}
