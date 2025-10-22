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
#include "FailInfo.h"
#include "PathProvider.h"
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
/// It uses PathProvider to locate test data files and provides methods
/// for loading individual configurations or discovering all available tests.
/////////////////////////////////////////////////
class TestDataLoader {

private:
  /////////////////////////////////////////////////
  /// @brief PathProvider for locating test data files
  /////////////////////////////////////////////////
  PathProvider m_path_provider;

  /////////////////////////////////////////////////
  /// @brief Load binary data from file
  ///
  /// @param file_path Path to the binary file
  /// @return Pointer to loaded data (caller must manage memory)
  /////////////////////////////////////////////////
  char *LoadBinaryData(const std::filesystem::path &file_path) const;

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

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor for TestDataLoader
  ///
  /// Initializes with Test environment type for PathProvider
  /////////////////////////////////////////////////
  TestDataLoader();

  /////////////////////////////////////////////////
  /// @brief Load a test data configuration by name
  ///
  /// Loads a test data configuration from a .test_data.bin file.
  /// In Test environment, files are located in tests/data/<subdirectory>.
  /// Uses PathProvider to determine the correct base path.
  ///
  /// @param test_name Name of the test (without .test_data.bin extension)
  /// @param subdirectory Optional subdirectory within data directory (e.g., "components", "logic")
  /// @return TestDataConfig pointer or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<const TestDataConfig *, FailInfo>
  LoadTestData(const std::string &test_name,
               const std::string &subdirectory = "") const;

  /////////////////////////////////////////////////
  /// @brief Find all test data files in a directory
  ///
  /// Scans a directory for files matching the .test_data.bin pattern.
  /// Uses PathProvider to determine base directory (tests/data in Test environment).
  /// Useful for discovering available test configurations.
  ///
  /// @param subdirectory Subdirectory within data directory to search
  /// @return Vector of test names (without extension) or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::vector<std::string>, FailInfo>
  DiscoverTestDataFiles(const std::string &subdirectory = "") const;

  /////////////////////////////////////////////////
  /// @brief Load multiple test data configurations
  ///
  /// Loads all test data configurations from a list of test names.
  /// Uses PathProvider to determine base directory.
  ///
  /// @param test_names Vector of test names to load
  /// @param subdirectory Optional subdirectory within data directory
  /// @return Vector of TestDataConfig pointers or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::vector<const TestDataConfig *>, FailInfo>
  LoadMultipleTestData(const std::vector<std::string> &test_names,
                       const std::string &subdirectory = "") const;
};

} // namespace steamrot::tests
