/////////////////////////////////////////////////
/// @file
/// @brief Declaration of TestDataLoader class for loading test data configurations
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "DataLoader.h"
#include "FailInfo.h"
#include "test_data_generated.h"
#include <expected>
#include <filesystem>
#include <string>
#include <vector>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class TestDataLoader
/// @brief Loads test data configurations from FlatBuffers binary files
///
/// This class provides utilities for loading test data configurations
/// defined in .test_data.json files (compiled to .test_data.bin).
/// It follows the same patterns as FlatbuffersDataLoader for consistency.
/////////////////////////////////////////////////
class TestDataLoader : public steamrot::DataLoader {

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor for TestDataLoader
  /////////////////////////////////////////////////
  TestDataLoader() = default;

  /////////////////////////////////////////////////
  /// @brief Load a test data configuration by name
  ///
  /// Loads a test data configuration from a .test_data.bin file.
  /// The file should be located in the appropriate test data directory.
  ///
  /// @param test_name Name of the test (without .test_data.bin extension)
  /// @param subdirectory Optional subdirectory within tests/data (e.g., "components", "logic")
  /// @return TestDataConfig pointer or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<const TestDataConfig *, FailInfo>
  LoadTestData(const std::string &test_name,
               const std::string &subdirectory = "") const;

  /////////////////////////////////////////////////
  /// @brief Find all test data files in a directory
  ///
  /// Scans a directory for files matching the .test_data.bin pattern.
  /// Useful for discovering available test configurations.
  ///
  /// @param subdirectory Subdirectory within tests/data to search
  /// @return Vector of test names (without extension) or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::vector<std::string>, FailInfo>
  DiscoverTestDataFiles(const std::string &subdirectory = "") const;

  /////////////////////////////////////////////////
  /// @brief Load multiple test data configurations
  ///
  /// Loads all test data configurations from a list of test names.
  ///
  /// @param test_names Vector of test names to load
  /// @param subdirectory Optional subdirectory within tests/data
  /// @return Vector of TestDataConfig pointers or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::vector<const TestDataConfig *>, FailInfo>
  LoadMultipleTestData(const std::vector<std::string> &test_names,
                       const std::string &subdirectory = "") const;

private:
  /////////////////////////////////////////////////
  /// @brief Construct path to test data file
  ///
  /// @param test_name Name of the test
  /// @param subdirectory Optional subdirectory
  /// @return Full filesystem path to the test data file
  /////////////////////////////////////////////////
  std::filesystem::path
  GetTestDataPath(const std::string &test_name,
                  const std::string &subdirectory) const;
};

} // namespace steamrot::tests
