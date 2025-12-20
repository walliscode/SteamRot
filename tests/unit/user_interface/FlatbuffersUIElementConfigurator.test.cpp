/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersUIElementConfigurator
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersUIElementConfigurator.h"
#include "TestFixture.h"
#include "user_interface_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <fstream>

std::pair<std::unique_ptr<char[]>, const steamrot::UserInterfaceFbs *>
LoadUIElementTestData() {
  std::filesystem::path test_file_path = __FILE__;
  std::filesystem::path data_dir = test_file_path.parent_path() / "data";
  std::filesystem::path bin_file_path = data_dir / "ui_element_test_data.bin";

  std::ifstream infile(bin_file_path, std::ios::binary | std::ios::in);
  if (!infile.is_open()) {
    throw std::runtime_error("Failed to open file: " + bin_file_path.string());
  }

  infile.seekg(0, std::ios::end);
  auto length = infile.tellg();
  if (length <= 0) {
    throw std::runtime_error("Empty or invalid file: " +
                             bin_file_path.string());
  }

  infile.seekg(0, std::ios::beg);
  auto data = std::make_unique<char[]>(static_cast<size_t>(length));
  infile.read(data.get(), length);
  infile.close();

  const steamrot::UserInterfaceFbs *ui_element_data =
      steamrot::GetUserInterfaceFbs(data.get());

  return {std::move(data), ui_element_data};
}

TEST_CASE("FlatbuffersUIElementConfigurator initialization",
          "[FlatbuffersUIElementConfigurator]") {
  // set up the test fixture
  steamrot::tests::TestFixture fixture;

  // Load UI element test data
  auto [data, ui_element_data] = LoadUIElementTestData();
  REQUIRE(ui_element_data != nullptr);

  // Initialize configurator
  steamrot::FlatbuffersUIElementConfigurator configurator(
      fixture.GetGameContext().event_handler, *ui_element_data);
}
