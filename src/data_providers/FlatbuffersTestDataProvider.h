/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the FlatbuffersTestDataProvider class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersTestDataLoader.h"
#include "ITestDataProvider.h"
#include "TestData.h"
#include "test_data_generated.h"
#include <expected>
#include <filesystem>

namespace steamrot {

// Forward declare EventHandler to avoid circular dependencies
class EventHandler;

/////////////////////////////////////////////////
/// @class FlatbuffersTestDataProvider
/// @brief FlatBuffers implementation of ITestDataProvider.
///
/// Loads test data from FlatBuffers binary files and converts
/// to native C++ TestData structs.
/////////////////////////////////////////////////
class FlatbuffersTestDataProvider : public ITestDataProvider {

private:
  /////////////////////////////////////////////////
  /// @brief Path to the directory containing test data files
  /////////////////////////////////////////////////
  std::filesystem::path m_object_directory_path;

  /////////////////////////////////////////////////
  /// @brief Reference to EventHandler for entity configuration
  /////////////////////////////////////////////////
  EventHandler &m_event_handler;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor.
  ///
  /// @param obj_dir_path Path to the directory containing test data files.
  /// @param event_handler Reference to EventHandler for entity configuration.
  /////////////////////////////////////////////////
  FlatbuffersTestDataProvider(std::filesystem::path obj_dir_path,
                               EventHandler &event_handler);

  /////////////////////////////////////////////////
  /// @brief Creates and returns all TestData instances loaded from FlatBuffers
  /// binary files.
  /////////////////////////////////////////////////
  std::expected<std::vector<TestData>, FailInfo>
  CreateAllTestData() const override;

  /////////////////////////////////////////////////
  /// @brief Configures a TestData instance from FlatBuffers data.
  ///
  /// @param test_data TestData instance to configure.
  /// @param fbs_test_data FlatBuffers TestDataFbs instance.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureTestData(TestData &test_data,
                    const TestDataFbs *fbs_test_data) const;

  /////////////////////////////////////////////////
  /// @brief Returns the object directory path.
  /////////////////////////////////////////////////
  const std::filesystem::path &GetObjectDirectoryPath() const;
};

} // namespace steamrot
