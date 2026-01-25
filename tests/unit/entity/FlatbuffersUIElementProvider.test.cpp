/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersUIElementProvider
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersUIElementProvider.h"
#include "ButtonElement.h"
#include "DropDownButtonElement.h"
#include "DropDownContainerElement.h"
#include "DropDownItemElement.h"
#include "DropDownListElement.h"
#include "PanelElement.h"
#include "TestFixture.h"
#include "user_interface_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <fstream>

std::pair<std::unique_ptr<char[]>, const steamrot::UserInterfaceFbs *>
LoadTestData(const std::string &filename) {
  std::filesystem::path test_file_path = __FILE__;
  std::filesystem::path data_dir = test_file_path.parent_path() / "data";
  std::filesystem::path bin_file_path = data_dir / filename;

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

std::pair<std::unique_ptr<char[]>, const steamrot::UserInterfaceFbs *>
LoadUIElementTestData() {
  return LoadTestData("ui_element_test_data.bin");
}

TEST_CASE("FlatbuffersUIElementProvider error handling",
          "[unit][FlatbuffersUIElementProvider]") {
  // set up the test fixture
  steamrot::tests::TestFixture fixture;

  // Load UI element test data
  auto [data, ui_element_data] = LoadUIElementTestData();
  REQUIRE(ui_element_data != nullptr);

  // Initialize provider
  steamrot::FlatbuffersUIElementProvider provider(
      fixture.GetGameContext().event_handler, *ui_element_data);
}

TEST_CASE(
    "FlatbuffersUIElementProvider creates and configures elements correctly",
    "[unit][FlatbuffersUIElementProvider]") {
  // set up the test fixture
  steamrot::tests::TestFixture fixture;
  // Load UI element test data
  auto [data, ui_element_data] = LoadUIElementTestData();
  REQUIRE(ui_element_data != nullptr);

  // Initialize provider
  steamrot::FlatbuffersUIElementProvider provider(
      fixture.GetGameContext().event_handler, *ui_element_data);

  // check fbs data and extract
  REQUIRE(ui_element_data->root_ui_element()->base_data()->children()->size() ==
          5);

  SECTION("CreateRootUIElement returns valid element") {
    auto root_result = provider.CreateRootUIElement();
    REQUIRE(root_result.has_value());
    auto &root_element = root_result.value();
    REQUIRE(root_element != nullptr);

    // Check that the element has children
    REQUIRE(root_element->child_elements.size() == 5);
  }

  SECTION("ConfigureRootUIElement configures element correctly") {
    // Create a panel element to configure
    steamrot::PanelElement panel_element;

    auto config_result = provider.ConfigureRootUIElement(panel_element);
    REQUIRE(config_result.has_value());

    // Check that the element was configured with children
    REQUIRE(panel_element.child_elements.size() == 5);
  }
}

TEST_CASE("FlatbuffersUIElementProvider handles nested elements",
          "[unit][FlatbuffersUIElementProvider]") {
  steamrot::tests::TestFixture fixture;
  auto [data, ui_element_data] = LoadUIElementTestData();
  REQUIRE(ui_element_data != nullptr);

  steamrot::FlatbuffersUIElementProvider provider(
      fixture.GetGameContext().event_handler, *ui_element_data);

  auto root_result = provider.CreateRootUIElement();
  REQUIRE(root_result.has_value());
  auto &root_element = root_result.value();

  // Verify structure
  REQUIRE(root_element->child_elements.size() == 5);

  // Check that different element types are created
  auto &children = root_element->child_elements;

  // First child should be a ButtonElement
  REQUIRE(dynamic_cast<steamrot::ButtonElement *>(children[0].get()) !=
          nullptr);
  auto *button = dynamic_cast<steamrot::ButtonElement *>(children[0].get());
  REQUIRE(button->label == "Test Tab");

  // Second child should be a DropDownListElement
  REQUIRE(dynamic_cast<steamrot::DropDownListElement *>(children[1].get()) !=
          nullptr);
  auto *ddlist =
      dynamic_cast<steamrot::DropDownListElement *>(children[1].get());
  REQUIRE(ddlist->unexpanded_label == "Select Option");

  // Third child should be a DropDownContainerElement
  REQUIRE(dynamic_cast<steamrot::DropDownContainerElement *>(
              children[2].get()) != nullptr);
  auto *ddcont =
      dynamic_cast<steamrot::DropDownContainerElement *>(children[2].get());
  // DropDownContainer should have 2 children
  REQUIRE(ddcont->child_elements.size() == 2);

  // Fourth child should be a DropDownItemElement
  REQUIRE(dynamic_cast<steamrot::DropDownItemElement *>(children[3].get()) !=
          nullptr);
  auto *dditem =
      dynamic_cast<steamrot::DropDownItemElement *>(children[3].get());
  REQUIRE(dditem->label == "Item Label");

  // Fifth child should be a DropDownButtonElement
  REQUIRE(dynamic_cast<steamrot::DropDownButtonElement *>(children[4].get()) !=
          nullptr);
  auto *ddbtn =
      dynamic_cast<steamrot::DropDownButtonElement *>(children[4].get());
  REQUIRE(ddbtn->is_expanded == true);
}

TEST_CASE("FlatbuffersUIElementProvider handles element properties",
          "[unit][FlatbuffersUIElementProvider]") {
  steamrot::tests::TestFixture fixture;
  auto [data, ui_element_data] = LoadUIElementTestData();
  REQUIRE(ui_element_data != nullptr);

  steamrot::FlatbuffersUIElementProvider provider(
      fixture.GetGameContext().event_handler, *ui_element_data);

  auto root_result = provider.CreateRootUIElement();
  REQUIRE(root_result.has_value());
  auto &root_element = root_result.value();

  // Check root element basic properties
  REQUIRE(root_element->position.x == 10.0f);
  REQUIRE(root_element->position.y == 20.0f);
  REQUIRE(root_element->size.x == 100.0f);
  REQUIRE(root_element->size.y == 50.0f);
  REQUIRE(root_element->layout == steamrot::Layout::Horizontal);

  // Check a child element's properties (first button)
  auto &button = root_element->child_elements[0];
  REQUIRE(button->position.x == 0.0f);
  REQUIRE(button->position.y == 0.0f);
  REQUIRE(button->size.x == 0.0f);
  REQUIRE(button->size.y == 0.0f);
}
