/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersUIElementConfigurator
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersUIElementConfigurator.h"
#include "EventPacket.h"
#include "Subscriber.h"
#include "TestFixture.h"
#include "UserInputBitset.h"
#include "user_interface_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <flatbuffers/flatbuffers.h>
#include <fstream>
#include <variant>

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
          5);

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

  SECTION("Configure DropDownContainerElement") {
    // Get DropDownContainerData from FlatBuffers data and check validity
    auto container_fb = children->Get(2);
    REQUIRE(container_fb != nullptr);
    REQUIRE(
        container_fb->element_type() ==
        steamrot::UIElementDataUnion::UIElementDataUnion_DropDownContainerData);
    auto container_data = static_cast<const steamrot::DropDownContainerData *>(
        container_fb->element());
    REQUIRE(container_data != nullptr);

    // create DropDownContainerElement and configure using flatbuffers data
    steamrot::DropDownContainerElement container_element;
    auto result = configurator.ConfigureDropDownContainerElement(
        container_element, *container_data);
    REQUIRE(result.has_value());

    // DropDownContainerElement specific checks
    // The configurator validates that it has exactly 2 children
    REQUIRE(container_data->base_data()->children()->size() == 2);
  }

  SECTION("Configure DropDownItemElement") {
    // Get DropDownItemData from FlatBuffers data and check validity
    auto item_fb = children->Get(3);
    REQUIRE(item_fb != nullptr);
    REQUIRE(item_fb->element_type() ==
            steamrot::UIElementDataUnion::UIElementDataUnion_DropDownItemData);
    auto item_data =
        static_cast<const steamrot::DropDownItemData *>(item_fb->element());
    REQUIRE(item_data != nullptr);

    // create DropDownItemElement and configure using flatbuffers data
    steamrot::DropDownItemElement item_element;
    auto result =
        configurator.ConfigureDropDownItemElement(item_element, *item_data);
    REQUIRE(result.has_value());

    // DropDownItemElement specific checks
    REQUIRE(item_element.label == "Item Label");
  }

  SECTION("Configure DropDownButtonElement") {
    // Get DropDownButtonData from FlatBuffers data and check validity
    auto button_fb = children->Get(4);
    REQUIRE(button_fb != nullptr);
    REQUIRE(
        button_fb->element_type() ==
        steamrot::UIElementDataUnion::UIElementDataUnion_DropDownButtonData);
    auto button_data =
        static_cast<const steamrot::DropDownButtonData *>(button_fb->element());
    REQUIRE(button_data != nullptr);

    // create DropDownButtonElement and configure using flatbuffers data
    steamrot::DropDownButtonElement button_element;
    auto result = configurator.ConfigureDropDownButtonElement(button_element,
                                                              *button_data);
    REQUIRE(result.has_value());

    // DropDownButtonElement specific checks
    REQUIRE(button_element.is_expanded == true);
  }
}

TEST_CASE("FlatbuffersUIElementConfigurator CreateUIElement creates correct "
          "element types",
          "[FlatbuffersUIElementConfigurator]") {
  // set up the test fixture
  steamrot::tests::TestFixture fixture;
  // Load UI element test data
  auto [data, ui_element_data] = LoadUIElementTestData();
  REQUIRE(ui_element_data != nullptr);

  // Initialize configurator
  steamrot::FlatbuffersUIElementConfigurator configurator(
      fixture.GetGameContext().event_handler, *ui_element_data);

  REQUIRE(ui_element_data->root_ui_element()->base_data()->children()->size() ==
          5);
  auto children = ui_element_data->root_ui_element()->base_data()->children();

  SECTION("CreateUIElement creates PanelElement") {
    auto panel_fb = ui_element_data->root_ui_element();
    REQUIRE(panel_fb != nullptr);

    auto result = configurator.CreateUIElement(
        steamrot::UIElementDataUnion::UIElementDataUnion_PanelData, panel_fb);
    REQUIRE(result.has_value());

    auto element = std::move(result.value());
    REQUIRE(element != nullptr);

    // Check correct type was created
    auto panel_element = dynamic_cast<steamrot::PanelElement *>(element.get());
    REQUIRE(panel_element != nullptr);

    // Check base data configured correctly
    REQUIRE(element->position.x == 10.0f);
    REQUIRE(element->position.y == 20.0f);
    REQUIRE(element->size.x == 100.0f);
    REQUIRE(element->size.y == 50.0f);
    REQUIRE(element->children_active == false);
    REQUIRE(element->layout == steamrot::Layout::Horizontal);
    REQUIRE(element->spacing_strategy == steamrot::SpacingAndSizing::None);
  }

  SECTION("CreateUIElement creates ButtonElement") {
    auto button_fb = children->Get(0);
    REQUIRE(button_fb != nullptr);
    REQUIRE(button_fb->element_type() ==
            steamrot::UIElementDataUnion::UIElementDataUnion_ButtonData);

    auto result = configurator.CreateUIElement(button_fb->element_type(),
                                               button_fb->element());

    if (!result.has_value()) {
      auto error = result.error();
      FAIL(error.message);
    }

    auto element = std::move(result.value());
    REQUIRE(element != nullptr);

    // Check correct type was created
    auto button_element =
        dynamic_cast<steamrot::ButtonElement *>(element.get());
    REQUIRE(button_element != nullptr);

    // Check base data configured correctly
    REQUIRE(element->position.x == 0.0f);
    REQUIRE(element->position.y == 0.0f);
    REQUIRE(element->size.x == 0.0f);
    REQUIRE(element->size.y == 0.0f);
    REQUIRE(element->children_active == false);
    REQUIRE(element->layout == steamrot::Layout::Horizontal);
    REQUIRE(element->spacing_strategy == steamrot::SpacingAndSizing::None);

    // check response event
    REQUIRE(element->response_event.has_value());
    const steamrot::EventPacket &event_packet = element->response_event.value();
    REQUIRE(event_packet.event_type ==
            steamrot::EventType::EventType_EVENT_CHANGE_SCENE);
    REQUIRE(std::holds_alternative<steamrot::SceneChangePacket>(
        event_packet.event_data));
    const auto &scene_change =
        std::get<steamrot::SceneChangePacket>(event_packet.event_data);
    REQUIRE(scene_change.second == steamrot::SceneType::SceneType_TEST);

    // check Subscriber
    REQUIRE(element->subscription != nullptr);
    const steamrot::Subscriber &subscriber = *element->subscription;
    REQUIRE(subscriber.m_trigger_event_type ==
            steamrot::EventType::EventType_EVENT_USER_INPUT);

    REQUIRE(subscriber.m_trigger_event_data.has_value());
    const steamrot::EventData &event_data =
        subscriber.m_trigger_event_data.value();
    REQUIRE(std::holds_alternative<steamrot::UserInputBitset>(event_data));

    // check its been registered with the event handler
    auto &event_handler = fixture.GetGameContext().event_handler;
    auto &subscriber_register = event_handler.GetSubcriberRegister();
    auto it =
        subscriber_register.find(element->subscription->m_trigger_event_type);
    REQUIRE(it != subscriber_register.end());
    auto &subscribers = it->second;
    auto found = std::any_of(
        subscribers.begin(), subscribers.end(),

        // pass in element to lambda capture
        [&element](const std::weak_ptr<steamrot::Subscriber> &sub_ptr) {
          // check if weak_ptr can be locked and matches element's subscription
          auto shared_ptr = sub_ptr.lock();
          return shared_ptr && (shared_ptr == element->subscription);
        });

    // check that we found the subscriber
    REQUIRE(found);

    // Check element-specific data
    REQUIRE(button_element->label == "Test Tab");
  }

  SECTION("CreateUIElement creates DropDownListElement") {
    auto dropdown_fb = children->Get(1);
    REQUIRE(dropdown_fb != nullptr);
    REQUIRE(dropdown_fb->element_type() ==
            steamrot::UIElementDataUnion::UIElementDataUnion_DropDownListData);

    auto result = configurator.CreateUIElement(dropdown_fb->element_type(),
                                               dropdown_fb->element());
    REQUIRE(result.has_value());

    auto element = std::move(result.value());
    REQUIRE(element != nullptr);

    // Check correct type was created
    auto dropdown_element =
        dynamic_cast<steamrot::DropDownListElement *>(element.get());
    REQUIRE(dropdown_element != nullptr);

    // Check base data configured correctly
    REQUIRE(element->position.x == 0.0f);
    REQUIRE(element->position.y == 0.0f);
    REQUIRE(element->size.x == 0.0f);
    REQUIRE(element->size.y == 0.0f);
    REQUIRE(element->children_active == false);
    REQUIRE(element->layout == steamrot::Layout::DropDown);
    REQUIRE(element->spacing_strategy ==
            steamrot::SpacingAndSizing::DropDownList);

    // Check element-specific data
    REQUIRE(dropdown_element->unexpanded_label == "Select Option");
    REQUIRE(dropdown_element->expanded_label == "Options:");
  }

  SECTION("CreateUIElement creates DropDownContainerElement") {
    auto container_fb = children->Get(2);
    REQUIRE(container_fb != nullptr);
    REQUIRE(
        container_fb->element_type() ==
        steamrot::UIElementDataUnion::UIElementDataUnion_DropDownContainerData);

    auto result = configurator.CreateUIElement(container_fb->element_type(),
                                               container_fb->element());
    REQUIRE(result.has_value());

    auto element = std::move(result.value());
    REQUIRE(element != nullptr);

    // Check correct type was created
    auto container_element =
        dynamic_cast<steamrot::DropDownContainerElement *>(element.get());
    REQUIRE(container_element != nullptr);

    // Check base data configured correctly
    REQUIRE(element->position.x == 10.0f);
    REQUIRE(element->position.y == 10.0f);
    REQUIRE(element->size.x == 200.0f);
    REQUIRE(element->size.y == 30.0f);
    REQUIRE(element->children_active == true);
    REQUIRE(element->layout == steamrot::Layout::Horizontal);
    REQUIRE(element->spacing_strategy == steamrot::SpacingAndSizing::None);

    // Check that children were created (should be 2: list and button)
    REQUIRE(element->child_elements.size() == 2);
  }

  SECTION("CreateUIElement creates DropDownItemElement") {
    auto item_fb = children->Get(3);
    REQUIRE(item_fb != nullptr);
    REQUIRE(item_fb->element_type() ==
            steamrot::UIElementDataUnion::UIElementDataUnion_DropDownItemData);

    auto result = configurator.CreateUIElement(item_fb->element_type(),
                                               item_fb->element());
    REQUIRE(result.has_value());

    auto element = std::move(result.value());
    REQUIRE(element != nullptr);

    // Check correct type was created
    auto item_element =
        dynamic_cast<steamrot::DropDownItemElement *>(element.get());
    REQUIRE(item_element != nullptr);

    // Check base data configured correctly
    REQUIRE(element->position.x == 5.0f);
    REQUIRE(element->position.y == 5.0f);
    REQUIRE(element->size.x == 100.0f);
    REQUIRE(element->size.y == 20.0f);
    REQUIRE(element->children_active == false);
    REQUIRE(element->layout == steamrot::Layout::None);
    REQUIRE(element->spacing_strategy == steamrot::SpacingAndSizing::None);

    // Check element-specific data
    REQUIRE(item_element->label == "Item Label");
  }

  SECTION("CreateUIElement creates DropDownButtonElement") {
    auto button_fb = children->Get(4);
    REQUIRE(button_fb != nullptr);
    REQUIRE(
        button_fb->element_type() ==
        steamrot::UIElementDataUnion::UIElementDataUnion_DropDownButtonData);

    auto result = configurator.CreateUIElement(button_fb->element_type(),
                                               button_fb->element());
    REQUIRE(result.has_value());

    auto element = std::move(result.value());
    REQUIRE(element != nullptr);

    // Check correct type was created
    auto button_element =
        dynamic_cast<steamrot::DropDownButtonElement *>(element.get());
    REQUIRE(button_element != nullptr);

    // Check base data configured correctly
    REQUIRE(element->position.x == 15.0f);
    REQUIRE(element->position.y == 15.0f);
    REQUIRE(element->size.x == 50.0f);
    REQUIRE(element->size.y == 50.0f);
    REQUIRE(element->children_active == false);
    REQUIRE(element->layout == steamrot::Layout::None);
    REQUIRE(element->spacing_strategy == steamrot::SpacingAndSizing::None);

    // Check element-specific data
    REQUIRE(button_element->is_expanded == true);
  }
}

TEST_CASE("FlatbuffersUIElementConfigurator CreateRootUIElement creates root "
          "element correctly",
          "[FlatbuffersUIElementConfigurator]") {
  // set up the test fixture
  steamrot::tests::TestFixture fixture;
  // Load UI element test data
  auto [data, ui_element_data] = LoadUIElementTestData();
  REQUIRE(ui_element_data != nullptr);

  // Initialize configurator
  steamrot::FlatbuffersUIElementConfigurator configurator(
      fixture.GetGameContext().event_handler, *ui_element_data);

  SECTION("CreateRootUIElement creates root element successfully") {
    auto result = configurator.CreateRootUIElement();
    REQUIRE(result.has_value());

    auto root_element = std::move(result.value());
    REQUIRE(root_element != nullptr);

    // Check that it's a PanelElement
    auto panel_element =
        dynamic_cast<steamrot::PanelElement *>(root_element.get());
    REQUIRE(panel_element != nullptr);

    // Check base data is configured correctly
    REQUIRE(root_element->position.x == 10.0f);
    REQUIRE(root_element->position.y == 20.0f);
    REQUIRE(root_element->size.x == 100.0f);
    REQUIRE(root_element->size.y == 50.0f);
    REQUIRE(root_element->children_active == false);
    REQUIRE(root_element->layout == steamrot::Layout::Horizontal);
    REQUIRE(root_element->spacing_strategy == steamrot::SpacingAndSizing::None);

    // Check that children were created
    REQUIRE(root_element->child_elements.size() == 5);
  }

  SECTION("CreateRootUIElement creates children with correct types") {
    auto result = configurator.CreateRootUIElement();
    REQUIRE(result.has_value());

    auto root_element = std::move(result.value());
    REQUIRE(root_element != nullptr);
    REQUIRE(root_element->child_elements.size() == 5);

    // Check first child is ButtonElement
    auto button =
        dynamic_cast<steamrot::ButtonElement *>(root_element->child_elements[0].get());
    REQUIRE(button != nullptr);

    // Check second child is DropDownListElement
    auto dropdown_list = dynamic_cast<steamrot::DropDownListElement *>(
        root_element->child_elements[1].get());
    REQUIRE(dropdown_list != nullptr);

    // Check third child is DropDownContainerElement
    auto dropdown_container = dynamic_cast<steamrot::DropDownContainerElement *>(
        root_element->child_elements[2].get());
    REQUIRE(dropdown_container != nullptr);

    // Check fourth child is DropDownItemElement
    auto dropdown_item = dynamic_cast<steamrot::DropDownItemElement *>(
        root_element->child_elements[3].get());
    REQUIRE(dropdown_item != nullptr);

    // Check fifth child is DropDownButtonElement
    auto dropdown_button = dynamic_cast<steamrot::DropDownButtonElement *>(
        root_element->child_elements[4].get());
    REQUIRE(dropdown_button != nullptr);
  }

  SECTION("CreateRootUIElement creates nested children (grandchildren)") {
    auto result = configurator.CreateRootUIElement();
    REQUIRE(result.has_value());

    auto root_element = std::move(result.value());
    REQUIRE(root_element != nullptr);
    REQUIRE(root_element->child_elements.size() == 5);

    // The DropDownContainerElement (child index 2) should have 2 children
    auto dropdown_container = dynamic_cast<steamrot::DropDownContainerElement *>(
        root_element->child_elements[2].get());
    REQUIRE(dropdown_container != nullptr);
    REQUIRE(dropdown_container->child_elements.size() == 2);

    // Check first grandchild is DropDownListElement
    auto grandchild_list = dynamic_cast<steamrot::DropDownListElement *>(
        dropdown_container->child_elements[0].get());
    REQUIRE(grandchild_list != nullptr);
    REQUIRE(grandchild_list->unexpanded_label == "Container List");
    REQUIRE(grandchild_list->expanded_label == "Container Options:");

    // Check second grandchild is DropDownButtonElement
    auto grandchild_button = dynamic_cast<steamrot::DropDownButtonElement *>(
        dropdown_container->child_elements[1].get());
    REQUIRE(grandchild_button != nullptr);
    REQUIRE(grandchild_button->is_expanded == false);
  }
}

TEST_CASE("FlatbuffersUIElementConfigurator error handling tests",
          "[FlatbuffersUIElementConfigurator]") {
  // set up the test fixture
  steamrot::tests::TestFixture fixture;

  SECTION("CreateRootUIElement fails with missing root_ui_element") {
    // Create minimal FlatBuffers data without root_ui_element
    flatbuffers::FlatBufferBuilder builder(1024);
    auto ui_name = builder.CreateString("test_ui");
    
    steamrot::UserInterfaceFbsBuilder ui_builder(builder);
    ui_builder.add_ui_name(ui_name);
    ui_builder.add_is_visible(true);
    // Note: not adding root_ui_element
    
    auto ui_data_offset = ui_builder.Finish();
    builder.Finish(ui_data_offset);
    
    const steamrot::UserInterfaceFbs *ui_data =
        steamrot::GetUserInterfaceFbs(builder.GetBufferPointer());
    REQUIRE(ui_data != nullptr);

    steamrot::FlatbuffersUIElementConfigurator configurator(
        fixture.GetGameContext().event_handler, *ui_data);

    auto result = configurator.CreateRootUIElement();
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().fail_mode ==
            steamrot::FailMode::FlatbuffersDataNotFound);
  }

  SECTION("CreateUIElement fails with unsupported element type") {
    // Load valid test data
    auto [data, ui_element_data] = LoadUIElementTestData();
    REQUIRE(ui_element_data != nullptr);

    steamrot::FlatbuffersUIElementConfigurator configurator(
        fixture.GetGameContext().event_handler, *ui_element_data);

    // Use NONE type which is unsupported
    auto result = configurator.CreateUIElement(
        steamrot::UIElementDataUnion::UIElementDataUnion_NONE, nullptr);
    
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().fail_mode ==
            steamrot::FailMode::NonExistentEnumValue);
  }

  SECTION("ConfigureDropDownContainerElement fails with no children") {
    // Create DropDownContainerData without children
    flatbuffers::FlatBufferBuilder builder(1024);
    
    // Create base_data with no children
    auto position = steamrot::CreateVec2f(builder, 0.0f, 0.0f);
    auto size = steamrot::CreateVec2f(builder, 100.0f, 50.0f);
    
    steamrot::UIElementDataBuilder base_builder(builder);
    base_builder.add_position(position);
    base_builder.add_size(size);
    base_builder.add_children_active(true);
    base_builder.add_layout(steamrot::LayoutFbs_Horizontal);
    base_builder.add_spacing_strategy(steamrot::SpacingAndSizingFbs_None);
    // Note: not adding children vector
    
    auto base_data_offset = base_builder.Finish();
    
    steamrot::DropDownContainerDataBuilder container_builder(builder);
    container_builder.add_base_data(base_data_offset);
    auto container_data_offset = container_builder.Finish();
    
    builder.Finish(container_data_offset);
    
    const steamrot::DropDownContainerData *container_data =
        flatbuffers::GetRoot<steamrot::DropDownContainerData>(
            builder.GetBufferPointer());
    REQUIRE(container_data != nullptr);

    // Load valid test data for the configurator
    auto [data, ui_element_data] = LoadUIElementTestData();
    REQUIRE(ui_element_data != nullptr);

    steamrot::FlatbuffersUIElementConfigurator configurator(
        fixture.GetGameContext().event_handler, *ui_element_data);

    steamrot::DropDownContainerElement container_element;
    auto result = configurator.ConfigureDropDownContainerElement(
        container_element, *container_data);
    
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().fail_mode ==
            steamrot::FailMode::FlatbuffersDataNotFound);
  }

  SECTION("ConfigureDropDownContainerElement fails with wrong number of "
          "children") {
    // Create DropDownContainerData with 1 child instead of 2
    flatbuffers::FlatBufferBuilder builder(1024);
    
    // Create a single child (ButtonData)
    auto child_position = steamrot::CreateVec2f(builder, 0.0f, 0.0f);
    auto child_size = steamrot::CreateVec2f(builder, 50.0f, 50.0f);
    
    steamrot::UIElementDataBuilder child_base_builder(builder);
    child_base_builder.add_position(child_position);
    child_base_builder.add_size(child_size);
    child_base_builder.add_children_active(false);
    child_base_builder.add_layout(steamrot::LayoutFbs_None);
    child_base_builder.add_spacing_strategy(steamrot::SpacingAndSizingFbs_None);
    auto child_base_data = child_base_builder.Finish();
    
    auto button_label = builder.CreateString("Button");
    steamrot::ButtonDataBuilder button_builder(builder);
    button_builder.add_base_data(child_base_data);
    button_builder.add_label(button_label);
    auto button_data = button_builder.Finish();
    
    steamrot::UIElementDataWrapperBuilder wrapper_builder(builder);
    wrapper_builder.add_element_type(
        steamrot::UIElementDataUnion::UIElementDataUnion_ButtonData);
    wrapper_builder.add_element(button_data.Union());
    auto wrapper = wrapper_builder.Finish();
    
    std::vector<flatbuffers::Offset<steamrot::UIElementDataWrapper>> children_vec;
    children_vec.push_back(wrapper);
    auto children = builder.CreateVector(children_vec);
    
    // Create base_data with 1 child
    auto position = steamrot::CreateVec2f(builder, 0.0f, 0.0f);
    auto size = steamrot::CreateVec2f(builder, 100.0f, 50.0f);
    
    steamrot::UIElementDataBuilder base_builder(builder);
    base_builder.add_position(position);
    base_builder.add_size(size);
    base_builder.add_children_active(true);
    base_builder.add_children(children);
    base_builder.add_layout(steamrot::LayoutFbs_Horizontal);
    base_builder.add_spacing_strategy(steamrot::SpacingAndSizingFbs_None);
    auto base_data_offset = base_builder.Finish();
    
    steamrot::DropDownContainerDataBuilder container_builder(builder);
    container_builder.add_base_data(base_data_offset);
    auto container_data_offset = container_builder.Finish();
    
    builder.Finish(container_data_offset);
    
    const steamrot::DropDownContainerData *container_data =
        flatbuffers::GetRoot<steamrot::DropDownContainerData>(
            builder.GetBufferPointer());
    REQUIRE(container_data != nullptr);

    // Load valid test data for the configurator
    auto [data, ui_element_data] = LoadUIElementTestData();
    REQUIRE(ui_element_data != nullptr);

    steamrot::FlatbuffersUIElementConfigurator configurator(
        fixture.GetGameContext().event_handler, *ui_element_data);

    steamrot::DropDownContainerElement container_element;
    auto result = configurator.ConfigureDropDownContainerElement(
        container_element, *container_data);
    
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().fail_mode ==
            steamrot::FailMode::FlatbuffersDataNotFound);
    REQUIRE(result.error().message.find("2 children") != std::string::npos);
  }

  SECTION("ConfigureDropDownContainerElement fails with wrong first child "
          "type") {
    // Create DropDownContainerData with 2 children but first is not
    // DropDownListData
    flatbuffers::FlatBufferBuilder builder(1024);
    
    // Create first child (ButtonData instead of DropDownListData)
    auto child1_position = steamrot::CreateVec2f(builder, 0.0f, 0.0f);
    auto child1_size = steamrot::CreateVec2f(builder, 50.0f, 50.0f);
    
    steamrot::UIElementDataBuilder child1_base_builder(builder);
    child1_base_builder.add_position(child1_position);
    child1_base_builder.add_size(child1_size);
    child1_base_builder.add_children_active(false);
    child1_base_builder.add_layout(steamrot::LayoutFbs_None);
    child1_base_builder.add_spacing_strategy(steamrot::SpacingAndSizingFbs_None);
    auto child1_base_data = child1_base_builder.Finish();
    
    auto button_label = builder.CreateString("Button");
    steamrot::ButtonDataBuilder button_builder(builder);
    button_builder.add_base_data(child1_base_data);
    button_builder.add_label(button_label);
    auto button_data = button_builder.Finish();
    
    steamrot::UIElementDataWrapperBuilder wrapper1_builder(builder);
    wrapper1_builder.add_element_type(
        steamrot::UIElementDataUnion::UIElementDataUnion_ButtonData);
    wrapper1_builder.add_element(button_data.Union());
    auto wrapper1 = wrapper1_builder.Finish();
    
    // Create second child (DropDownButtonData)
    auto child2_position = steamrot::CreateVec2f(builder, 0.0f, 0.0f);
    auto child2_size = steamrot::CreateVec2f(builder, 50.0f, 50.0f);
    
    steamrot::UIElementDataBuilder child2_base_builder(builder);
    child2_base_builder.add_position(child2_position);
    child2_base_builder.add_size(child2_size);
    child2_base_builder.add_children_active(false);
    child2_base_builder.add_layout(steamrot::LayoutFbs_None);
    child2_base_builder.add_spacing_strategy(steamrot::SpacingAndSizingFbs_None);
    auto child2_base_data = child2_base_builder.Finish();
    
    steamrot::DropDownButtonDataBuilder ddbtn_builder(builder);
    ddbtn_builder.add_base_data(child2_base_data);
    ddbtn_builder.add_is_expanded(false);
    auto ddbtn_data = ddbtn_builder.Finish();
    
    steamrot::UIElementDataWrapperBuilder wrapper2_builder(builder);
    wrapper2_builder.add_element_type(
        steamrot::UIElementDataUnion::UIElementDataUnion_DropDownButtonData);
    wrapper2_builder.add_element(ddbtn_data.Union());
    auto wrapper2 = wrapper2_builder.Finish();
    
    std::vector<flatbuffers::Offset<steamrot::UIElementDataWrapper>> children_vec;
    children_vec.push_back(wrapper1);
    children_vec.push_back(wrapper2);
    auto children = builder.CreateVector(children_vec);
    
    // Create base_data with 2 children
    auto position = steamrot::CreateVec2f(builder, 0.0f, 0.0f);
    auto size = steamrot::CreateVec2f(builder, 100.0f, 50.0f);
    
    steamrot::UIElementDataBuilder base_builder(builder);
    base_builder.add_position(position);
    base_builder.add_size(size);
    base_builder.add_children_active(true);
    base_builder.add_children(children);
    base_builder.add_layout(steamrot::LayoutFbs_Horizontal);
    base_builder.add_spacing_strategy(steamrot::SpacingAndSizingFbs_None);
    auto base_data_offset = base_builder.Finish();
    
    steamrot::DropDownContainerDataBuilder container_builder(builder);
    container_builder.add_base_data(base_data_offset);
    auto container_data_offset = container_builder.Finish();
    
    builder.Finish(container_data_offset);
    
    const steamrot::DropDownContainerData *container_data =
        flatbuffers::GetRoot<steamrot::DropDownContainerData>(
            builder.GetBufferPointer());
    REQUIRE(container_data != nullptr);

    // Load valid test data for the configurator
    auto [data, ui_element_data] = LoadUIElementTestData();
    REQUIRE(ui_element_data != nullptr);

    steamrot::FlatbuffersUIElementConfigurator configurator(
        fixture.GetGameContext().event_handler, *ui_element_data);

    steamrot::DropDownContainerElement container_element;
    auto result = configurator.ConfigureDropDownContainerElement(
        container_element, *container_data);
    
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().fail_mode ==
            steamrot::FailMode::FlatbuffersDataNotFound);
    REQUIRE(result.error().message.find("first child") != std::string::npos);
  }

  SECTION("ConfigureDropDownContainerElement fails with wrong second child "
          "type") {
    // Create DropDownContainerData with 2 children but second is not
    // DropDownButtonData
    flatbuffers::FlatBufferBuilder builder(1024);
    
    // Create first child (DropDownListData - correct)
    auto child1_position = steamrot::CreateVec2f(builder, 0.0f, 0.0f);
    auto child1_size = steamrot::CreateVec2f(builder, 50.0f, 50.0f);
    
    steamrot::UIElementDataBuilder child1_base_builder(builder);
    child1_base_builder.add_position(child1_position);
    child1_base_builder.add_size(child1_size);
    child1_base_builder.add_children_active(false);
    child1_base_builder.add_layout(steamrot::LayoutFbs_DropDown);
    child1_base_builder.add_spacing_strategy(
        steamrot::SpacingAndSizingFbs_DropDownList);
    auto child1_base_data = child1_base_builder.Finish();
    
    auto list_label = builder.CreateString("List");
    auto expanded_label = builder.CreateString("Expanded");
    steamrot::DropDownListDataBuilder list_builder(builder);
    list_builder.add_base_data(child1_base_data);
    list_builder.add_label(list_label);
    list_builder.add_expanded_label(expanded_label);
    list_builder.add_data_populate_function(
        steamrot::DataPopulateFunction::DataPopulateFunction_PopulateWithFragmentData);
    auto list_data = list_builder.Finish();
    
    steamrot::UIElementDataWrapperBuilder wrapper1_builder(builder);
    wrapper1_builder.add_element_type(
        steamrot::UIElementDataUnion::UIElementDataUnion_DropDownListData);
    wrapper1_builder.add_element(list_data.Union());
    auto wrapper1 = wrapper1_builder.Finish();
    
    // Create second child (ButtonData instead of DropDownButtonData)
    auto child2_position = steamrot::CreateVec2f(builder, 0.0f, 0.0f);
    auto child2_size = steamrot::CreateVec2f(builder, 50.0f, 50.0f);
    
    steamrot::UIElementDataBuilder child2_base_builder(builder);
    child2_base_builder.add_position(child2_position);
    child2_base_builder.add_size(child2_size);
    child2_base_builder.add_children_active(false);
    child2_base_builder.add_layout(steamrot::LayoutFbs_None);
    child2_base_builder.add_spacing_strategy(steamrot::SpacingAndSizingFbs_None);
    auto child2_base_data = child2_base_builder.Finish();
    
    auto button_label = builder.CreateString("Button");
    steamrot::ButtonDataBuilder button_builder(builder);
    button_builder.add_base_data(child2_base_data);
    button_builder.add_label(button_label);
    auto button_data = button_builder.Finish();
    
    steamrot::UIElementDataWrapperBuilder wrapper2_builder(builder);
    wrapper2_builder.add_element_type(
        steamrot::UIElementDataUnion::UIElementDataUnion_ButtonData);
    wrapper2_builder.add_element(button_data.Union());
    auto wrapper2 = wrapper2_builder.Finish();
    
    std::vector<flatbuffers::Offset<steamrot::UIElementDataWrapper>> children_vec;
    children_vec.push_back(wrapper1);
    children_vec.push_back(wrapper2);
    auto children = builder.CreateVector(children_vec);
    
    // Create base_data with 2 children
    auto position = steamrot::CreateVec2f(builder, 0.0f, 0.0f);
    auto size = steamrot::CreateVec2f(builder, 100.0f, 50.0f);
    
    steamrot::UIElementDataBuilder base_builder(builder);
    base_builder.add_position(position);
    base_builder.add_size(size);
    base_builder.add_children_active(true);
    base_builder.add_children(children);
    base_builder.add_layout(steamrot::LayoutFbs_Horizontal);
    base_builder.add_spacing_strategy(steamrot::SpacingAndSizingFbs_None);
    auto base_data_offset = base_builder.Finish();
    
    steamrot::DropDownContainerDataBuilder container_builder(builder);
    container_builder.add_base_data(base_data_offset);
    auto container_data_offset = container_builder.Finish();
    
    builder.Finish(container_data_offset);
    
    const steamrot::DropDownContainerData *container_data =
        flatbuffers::GetRoot<steamrot::DropDownContainerData>(
            builder.GetBufferPointer());
    REQUIRE(container_data != nullptr);

    // Load valid test data for the configurator
    auto [data, ui_element_data] = LoadUIElementTestData();
    REQUIRE(ui_element_data != nullptr);

    steamrot::FlatbuffersUIElementConfigurator configurator(
        fixture.GetGameContext().event_handler, *ui_element_data);

    steamrot::DropDownContainerElement container_element;
    auto result = configurator.ConfigureDropDownContainerElement(
        container_element, *container_data);
    
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().fail_mode ==
            steamrot::FailMode::FlatbuffersDataNotFound);
    REQUIRE(result.error().message.find("second child") != std::string::npos);
  }
}
