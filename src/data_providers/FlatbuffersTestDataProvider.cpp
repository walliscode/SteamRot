/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the FlatbuffersTestDataProvider class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersTestDataProvider.h"
#include "configure_test_data.h"

namespace steamrot {

/////////////////////////////////////////////////
FlatbuffersTestDataProvider::FlatbuffersTestDataProvider(
    std::filesystem::path obj_dir_path, EventHandler &event_handler)
    : m_object_directory_path(std::move(obj_dir_path)),
      m_event_handler(event_handler) {}

/////////////////////////////////////////////////
std::expected<std::vector<TestData>, FailInfo>
FlatbuffersTestDataProvider::CreateAllTestData() const {

  // Instantiate the FlatbuffersTestDataLoader with the object directory path
  FlatbuffersTestDataLoader data_loader(m_object_directory_path);

  // Create vector to hold TestData instances
  std::vector<TestData> test_data_vec;

  // Load all TestDataFbs
  auto fbs_test_data_result = data_loader.LoadTestDataFbs();
  if (!fbs_test_data_result) {
    return std::unexpected(fbs_test_data_result.error());
  }
  const auto &fbs_test_data_vec = fbs_test_data_result.value();

  // Convert each TestDataFbs to TestData
  for (const auto *fbs_test_data : fbs_test_data_vec) {
    TestData test_data;
    auto configure_result =
        ConfigureTestData(test_data, fbs_test_data);
    if (!configure_result) {
      return std::unexpected(configure_result.error());
    }
    test_data_vec.push_back(std::move(test_data));
  }

  return test_data_vec;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersTestDataProvider::ConfigureTestData(
    TestData &test_data, const TestDataFbs *fbs_test_data) const {

  return data::configure::ConfigureTestData(test_data, fbs_test_data,
                                            m_event_handler);
}

/////////////////////////////////////////////////
const std::filesystem::path &
FlatbuffersTestDataProvider::GetObjectDirectoryPath() const {
  return m_object_directory_path;
}

} // namespace steamrot
