/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersEntityConfigurator
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersEntityConfigurator.h"
#include "ButtonElement.h"
#include "EventHandler.h"
#include "PanelElement.h"
#include "SpacingAndSizing.h"
#include "Subscriber.h"
#include "containers.h"
#include "entities_generated.h"
#include "entity_memory.h"
#include "user_interface_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <memory>

/////////////////////////////////////////////////
/// Constructor Tests
/////////////////////////////////////////////////

TEST_CASE("FlatbuffersEntityConfigurator::Constructor",
          "[unit][FlatbuffersEntityConfigurator]") {

  // set up resources
  steamrot::EventHandler event_handler;
  steamrot::EntityMemoryPool emp;

  // Load entity test data
  // Get the path to the data directory adjacent to this test file
  std::filesystem::path test_file_path = __FILE__;
  std::filesystem::path data_dir = test_file_path.parent_path() / "data";
  std::filesystem::path bin_file_path = data_dir / "entity_test_data.bin";

  // Open and read the binary file
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

  // Get the root and verify
  const steamrot::EntityCollectionFbs *entity_collection =
      steamrot::GetEntityCollectionFbs(data.get());

  // pre tests
  REQUIRE(steamrot::entity::memory::GetMemoryPoolSize(emp) == 0);

  // pointer is not null
  REQUIRE(entity_collection != nullptr);
  REQUIRE(entity_collection->entity_memory_pool_size() == 100);

  // set up configurator
  steamrot::FlatbuffersEntityConfigurator configurator(event_handler,
                                                       *entity_collection);

  // run configuration of emp
  auto config_result = configurator.ConfigureEntityMemoryPool(emp);

  // tests

  // emp size is set
  REQUIRE(steamrot::entity::memory::GetMemoryPoolSize(emp) == 100);

  // check configuration result

  ///// CUserInterface component of entity 1 /////
  const steamrot::CUserInterface &c_ui_component =
      steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(1, emp);
  REQUIRE(c_ui_component.m_name == "test_ui");
  REQUIRE(c_ui_component.m_visible == true);
  REQUIRE(c_ui_component.m_root_element != nullptr);
  // check that root element is PanelElement
  REQUIRE(dynamic_cast<steamrot::PanelElement *>(
      c_ui_component.m_root_element.get()));

  const steamrot::PanelElement &root_panel =
      static_cast<const steamrot::PanelElement &>(
          *c_ui_component.m_root_element);

  REQUIRE(root_panel.size == sf::Vector2f(300.0f, 400.0f));
  REQUIRE(root_panel.position == sf::Vector2f(100.0f, 200.0f));
  REQUIRE(root_panel.layout == steamrot::Layout::Horizontal);
  REQUIRE(root_panel.spacing_strategy == steamrot::SpacingAndSizing::Even);
  REQUIRE(root_panel.child_elements.size() == 1);

  // check thet child element is ButtonElement and assign it
  REQUIRE(dynamic_cast<steamrot::ButtonElement *>(
      root_panel.child_elements[0].get()));
  const steamrot::ButtonElement &child_button =
      static_cast<const steamrot::ButtonElement &>(
          *root_panel.child_elements[0]);

  REQUIRE(child_button.size == sf::Vector2f(0.f, 0.f));
  REQUIRE(child_button.position == sf::Vector2f(0.f, 0.f));
  REQUIRE(child_button.label == "Start Game");
  REQUIRE(child_button.subscription != nullptr);
  const steamrot::Subscriber &button_sub = *child_button.subscription;

  REQUIRE(button_sub.m_trigger_event_data.has_value());

  /////
}
