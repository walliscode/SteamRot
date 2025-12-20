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

TEST_CASE("FlatbuffersUIElementConfigurator error handling",
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

TEST_CASE("FlatbuffersUIElementConfigurator configures elements correctly",
          "[FlatbuffersUIElementConfigurator]") {
  // set up the test fixture
  steamrot::tests::TestFixture fixture;
  // Load UI element test data
  auto [data, ui_element_data] = LoadUIElementTestData();
  REQUIRE(ui_element_data != nullptr);

  // Initialize configurator
  steamrot::FlatbuffersUIElementConfigurator configurator(
      fixture.GetGameContext().event_handler, *ui_element_data);

  // check fbs data and extract
  REQUIRE(ui_element_data->root_ui_element()->base_data()->children()->size() ==
          2);

  auto children = ui_element_data->root_ui_element()->base_data()->children();

  SECTION("Configure PanelElement") {
    // Create PanelElement from FlatBuffers data
    auto panel_data = ui_element_data->root_ui_element();
    REQUIRE(panel_data != nullptr);
    steamrot::PanelElement panel_element;
    auto result =
        configurator.ConfigurePanelElement(panel_element, *panel_data);
    REQUIRE(result.has_value());

    // add PanelElement specific checks here when needed
  }

  SECTION("Configure ButtonElement") {
    // Get ButtonData from FlatBuffers data and check validity
    auto button_fb = children->Get(0);
    REQUIRE(button_fb != nullptr);
    REQUIRE(button_fb->element_type() ==
            steamrot::UIElementDataUnion::UIElementDataUnion_ButtonData);
    auto button_data =
        static_cast<const steamrot::ButtonData *>(button_fb->element());
    REQUIRE(button_data != nullptr);

    // create ButtonElement and configure using flatbuffers data
    steamrot::ButtonElement button_element;
    auto result =
        configurator.ConfigureButtonElement(button_element, *button_data);
    REQUIRE(result.has_value());

    // ButtnonElement specific checks
    REQUIRE(button_element.label == "Test Tab");
  }

  SECTION("Configure DropDownListElement") {
    // Get DropDownListData from FlatBuffers data and check validity
    auto dropdown_fb = children->Get(1);
    REQUIRE(dropdown_fb != nullptr);
    REQUIRE(dropdown_fb->element_type() ==
            steamrot::UIElementDataUnion::UIElementDataUnion_DropDownListData);
    auto dropdown_data =
        static_cast<const steamrot::DropDownListData *>(dropdown_fb->element());
    REQUIRE(dropdown_data != nullptr);
    // create DropDownListElement and configure using flatbuffers data
    steamrot::DropDownListElement dropdown_element;
    auto result = configurator.ConfigureDropDownListElement(dropdown_element,
                                                            *dropdown_data);
    REQUIRE(result.has_value());
    // DropDownListElement specific checks
    REQUIRE(dropdown_element.unexpanded_label == "Select Option");
    REQUIRE(dropdown_element.expanded_label == "Options:");
    REQUIRE(dropdown_element.data_populate_function ==
            steamrot::DataPopulateFunction::
                DataPopulateFunction_PopulateWithFragmentData);
  }
}
