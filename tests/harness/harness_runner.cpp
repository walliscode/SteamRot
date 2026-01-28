/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for running the test harness.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "harness_runner.h"
#include "FailInfo.h"
#include "FlatbuffersTestDataProvider.h"
#include "TestEngine.h"
#include "add_uuids.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
RunHarnessTests(const std::filesystem::path current_location) {

  // check if the adjacent data directory exists
  std::filesystem::path data_dir = current_location / "data";
  if (!std::filesystem::exists(data_dir) |
      !std::filesystem::is_directory(data_dir)) {
    return std::unexpected(FailInfo{
        FailMode::DirectoryNotFound,
        std::format("No adjacent 'data' directory found at {}",
                    data_dir.string()),
    });
  }

  // instantiate the FlatbuffersTestDataProvider with the object directory path
  FlatbuffersTestDataProvider data_provider(current_location);

  // get all test data
  auto test_data_result = data_provider.ProvideAllTestData();
  if (!test_data_result) {
    return std::unexpected(test_data_result.error());
  }

  // check if any test data was found
  auto &test_data_vec = test_data_result.value();
  if (test_data_vec.empty()) {
    return std::unexpected(FailInfo{
        FailMode::None,
        "No test data found in the 'data' directory.",
    });
  }

  // modify data
  for (auto &test_data : test_data_vec) {

    auto add_uuids_result = add_uuids(test_data);
    if (!add_uuids_result) {
      return std::unexpected(add_uuids_result.error());
    }
  }
  // cycle through all test data and run tests
  for (const auto &test_data : test_data_vec) {

    // create TestEngine instance
    TestEngine test_engine{test_data};
  }

  return std::monostate{};
}
} // namespace steamrot::tests
