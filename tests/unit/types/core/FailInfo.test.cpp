/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FailInfo type
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FailInfo: Constructor adds data to members", "[types]") {
  // Arrange
  steamrot::FailMode test_mode = steamrot::FailMode::FileNotFound;
  std::string test_message = "Test failure message";
  // Act
  steamrot::FailInfo fail_info(test_mode, test_message);
  // Assert
  REQUIRE(fail_info.mode == test_mode);
  REQUIRE(fail_info.message == test_message);
}
