/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the ITestDataProvider interface.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "TestData.h"
#include <expected>
#include <filesystem>

class ITestDataProvider {

protected:
  const std::filesystem::path object_directory_path;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor taking object directory path
  ///
  /// @param obj_dir_path Object directory path
  /////////////////////////////////////////////////
  ITestDataProvider(std::filesystem::path &obj_dir_path)
      : object_directory_path(std::move(obj_dir_path)) {}

  /////////////////////////////////////////////////
  /// @brief Virtual destructor.
  /////////////////////////////////////////////////
  virtual ~ITestDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Provide all instances of TestData struct
  /////////////////////////////////////////////////
  virtual std::expected<std::vector<steamrot::TestData>, steamrot::FailInfo>
  ProviderAllTestData() const = 0;

  const std::filesystem::path &GetObjectDirectoryPath() const {
    return object_directory_path;
  }
};
