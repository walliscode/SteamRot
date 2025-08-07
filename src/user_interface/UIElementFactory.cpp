///////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the UIElementFactory class
/////////////////////////////////////////////////
#include "UIElementFactory.h"
#include "DropDown.h"
#include "FlatbuffersConfigurator.h"
#include "event_helpers.h"
#include "user_interface_generated.h"
#include "uuid.h"
#include <expected>
#include <iostream>

namespace steamrot {

/////////////////////////////////////////////////
std::expected<UIElement, FailInfo>
UIElementFactory::CreateUIStructure(const UIElementData &element_data) {

  const auto result = RecursivlyBuildUIElement(element_data);

  // propagate the error if the result is not valid
  if (!result.has_value()) {
    return std::unexpected(result.error());
  }
  return result.value();
};

/////////////////////////////////////////////////
std::expected<UIElement, FailInfo>
UIElementFactory::RecursivlyBuildUIElement(const UIElementData &element_data) {
  // create UIElement with general configuration
  auto result = ConfigureGeneralUIElement(element_data);

  // propagate the error if the result is not valid
  if (!result.has_value()) {
    return std::unexpected(result.error());
  }
  UIElement ui_element = result.value();

  // check type is present
  if (!element_data.type()) {
    FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                       "UIElement type not found"};
    return std::unexpected(fail_info);
  }
  // Configure the UIElement based on its type
  switch (element_data.type()) {

    // Panel
  case UIElementType::UIElementType_Panel: {
    auto panel_ptr = element_data.element_as_PanelData();
    if (!panel_ptr) {
      FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                         "UIElement Panel data not found"};
      return std::unexpected(fail_info);
    }
    auto panel_result = ConfigurePanel(*panel_ptr);

    // propagate the error if the result is not valid
    if (!panel_result.has_value()) {
      return std::unexpected(panel_result.error());
    }
    ui_element.element_type = panel_result.value();

    break;
  }
    // Button
  case UIElementType::UIElementType_Button: {
    std::cout << "Configuring Button UIElement" << std::endl;
    auto button_ptr = element_data.element_as_ButtonData();
    if (!button_ptr) {
      FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                         "UIElement Button data not found"};
      return std::unexpected(fail_info);
    }
    auto button_result = ConfigureButton(*button_ptr);
    if (!button_result.has_value()) {
      return std::unexpected(button_result.error());
    }
    ui_element.element_type = button_result.value();

    std::cout << "Button label: "
              << std::get<Button>(ui_element.element_type).label << std::endl;
    break;
  }
    // DropDownContainer
  case UIElementType::UIElementType_DropDownContainer: {
    std::cout << "Configuring DropDown UIElement" << std::endl;
    auto dd_ptr = element_data.element_as_DropDownContainerData();
    if (!dd_ptr) {
      FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                         "UIElement DropDownContainer data not found"};
      return std::unexpected(fail_info);
    }
    auto dd_result = ConfigureDropDownContainer(*dd_ptr);

    if (!dd_result.has_value()) {
      return std::unexpected(dd_result.error());
    }
    ui_element.element_type = dd_result.value();

    break;
  }

    // DropDownList
  case UIElementType::UIElementType_DropDownList: {
    std::cout << "Configuring DropDownList UIElement" << std::endl;
    auto ddl_ptr = element_data.element_as_DropDownListData();
    if (!ddl_ptr) {
      FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                         "UIElement DropDownList data not found"};
      return std::unexpected(fail_info);
    }
    auto ddl_result = ConfigureDropDownList(*ddl_ptr);
    if (!ddl_result.has_value()) {
      return std::unexpected(ddl_result.error());
    }
    ui_element.element_type = ddl_result.value();
    break;
  }

    // DropDownItem
  case UIElementType::UIElementType_DropDownItem: {
    std::cout << "Configuring DropDownItem UIElement" << std::endl;
    auto ddi_ptr = element_data.element_as_DropDownItemData();
    if (!ddi_ptr) {
      FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                         "UIElement DropDownItem data not found"};
      return std::unexpected(fail_info);
    }
    auto ddi_result = ConfigureDropDownItem(*ddi_ptr);
    if (!ddi_result.has_value()) {
      return std::unexpected(ddi_result.error());
    }
    ui_element.element_type = ddi_result.value();
    break;
  }

  // DropDownButton
  case UIElementType::UIElementType_DropDownButton: {
    std::cout << "Configuring DropDownButton UIElement" << std::endl;
    auto ddb_ptr = element_data.element_as_DropDownButtonData();
    if (!ddb_ptr) {
      FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                         "UIElement DropDownButton data not found"};
      return std::unexpected(fail_info);
    }
    auto ddb_result = ConfigureDropDownButton(*ddb_ptr);
    if (!ddb_result.has_value()) {
      return std::unexpected(ddb_result.error());
    }
    ui_element.element_type = ddb_result.value();
    break;
  }
  default: {
    // add an empty Panel element to the UIElement for the None type
    ui_element.element_type = Panel{};

    break;
  }
  }
  std::cout << "UIElement configured successfully." << std::endl;

  // Recursively build child elements if they exist
  if (element_data.children()) {
    std::cout << "Recursively building child elements" << std::endl;
    for (const auto &child_element : *element_data.children()) {
      // Recursively build the child UIElement
      auto child_result = RecursivlyBuildUIElement(*child_element);

      // Check if the child UIElement was created successfully
      if (!child_result.has_value()) {
        // If not, propagate the error
        return std::unexpected(child_result.error());
      }

      // Get the successfully created child UIElement
      UIElement child_ui_element = child_result.value();

      // Add the child UIElement to the parent UIElement
      ui_element.child_elements.push_back(child_ui_element);
    }
  }
  return ui_element;
};

std::expected<UIElement, FailInfo>
UIElementFactory::ConfigureGeneralUIElement(const UIElementData &element) {
  UIElement ui_element;

  if (element.name()) {
    ui_element.name = element.name()->str();
  }
  if (element.layout())
    ui_element.layout = element.layout();

  if (element.spacing_strategy()) {
    ui_element.spacing_strategy = element.spacing_strategy();
  }

  if (element.position()) {
    ui_element.position =
        sf::Vector2f(element.position()->x(), element.position()->y());
  }

  if (element.size()) {
    ui_element.size = sf::Vector2f(element.size()->x(), element.size()->y());
  }

  if (element.ratio()) {
    ui_element.ratio = element.ratio();
  }

  if (element.children_active()) {
    ui_element.children_active = element.children_active();
  }

  if (element.trigger_event()) {
    ui_element.trigger_event = element.trigger_event();
    auto trigger_event_data_result = ConfigureTriggerEventData(element);
    if (!trigger_event_data_result.has_value()) {
      return std::unexpected(trigger_event_data_result.error());
    }
    ui_element.trigger_event_data = trigger_event_data_result.value();
  }

  if (element.response_event()) {
    ui_element.response_event = element.response_event();
    auto response_event_data_result = ConfigureResponseEventData(element);
    if (!response_event_data_result.has_value()) {
      return std::unexpected(response_event_data_result.error());
    }
    ui_element.response_event_data = response_event_data_result.value();
  }

  return ui_element;
}

/////////////////////////////////////////////////
std::expected<Panel, FailInfo>
UIElementFactory::ConfigurePanel(const PanelData &panel_data) {
  Panel panel_element;
  // configure the panel element

  return panel_element;
}

/////////////////////////////////////////////////
std::expected<Button, FailInfo>
UIElementFactory::ConfigureButton(const ButtonData &button_data) {

  Button button_element;

  if (button_data.label()) {
    button_element.label = button_data.label()->str();
  }

  return button_element;
}

/////////////////////////////////////////////////
std::expected<DropDownContainer, FailInfo>
UIElementFactory::ConfigureDropDownContainer(
    const DropDownContainerData &dropdown_data) {
  DropDownContainer dropdown_element;
  // configure the dropdown_element

  return dropdown_element;
}

/////////////////////////////////////////////////
std::expected<DropDownList, FailInfo>
UIElementFactory::ConfigureDropDownList(const DropDownListData &dropdown_data) {
  DropDownList dropdown_list_element;
  std::cout << "Configuring DropDownList UIElement" << std::endl;
  if (dropdown_data.label()) {
    dropdown_list_element.label = dropdown_data.label()->str();
  }
  if (dropdown_data.expanded_label()) {
    dropdown_list_element.expanded_label =
        dropdown_data.expanded_label()->str();
  }
  dropdown_list_element.data_populate_function =
      dropdown_data.data_populate_function();
  return dropdown_list_element;
}

//////////////////////////////////////////////////
std::expected<DropDownItem, FailInfo>
UIElementFactory::ConfigureDropDownItem(const DropDownItemData &dropdown_data) {
  DropDownItem dropdown_item_element;
  if (dropdown_data.label()) {
    dropdown_item_element.label = dropdown_data.label()->str();
  }
  return dropdown_item_element;
}

///////////////////////////////////////////////////
std::expected<DropDownButton, FailInfo>
UIElementFactory::ConfigureDropDownButton(
    const DropDownButtonData &dropdown_data) {
  DropDownButton dropdown_button_element;
  dropdown_button_element.is_expanded = dropdown_data.is_expanded();
  return dropdown_button_element;
}

/////////////////////////////////////////////////
std::expected<UIElement, FailInfo> UIElementFactory::CreateDropDownItem() {
  UIElement dropdown_item_element;
  dropdown_item_element.element_type = DropDownItem{};
  return dropdown_item_element;
}

/////////////////////////////////////////////////
std::expected<EventData, FailInfo>
UIElementFactory::ConfigureTriggerEventData(const UIElementData &element_data) {
  switch (element_data.trigger_event_data_type()) {

  case EventDataType::EventDataType_UserInputBitsetData: {
    if (!element_data.trigger_event_data_as_UserInputBitsetData()) {
      FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                         "Trigger event UserInputBitsetData not found"};
      return std::unexpected(fail_info);
    }

    UserInputBitset user_input_data = ConvertActionKeysToEvent(
        *element_data.trigger_event_data_as_UserInputBitsetData());

    return user_input_data;
  }
  case EventDataType::EventDataType_SceneChangeData: {
    if (!element_data.trigger_event_data_as_SceneChangeData()) {
      FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                         "Trigger event SceneChangeData not found"};
      return std::unexpected(fail_info);
    }

    SceneChangeData scene_change_data;
    const auto &data = *element_data.trigger_event_data_as_SceneChangeData();
    if (data.scene_id()) {
      auto uuid_opt = uuids::uuid::from_string(data.scene_id()->str());
      if (!uuid_opt.has_value()) {
        FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                           "Invalid scene_id format in trigger event"};
        return std::unexpected(fail_info);
      }
      scene_change_data.first = uuid_opt.value();
    }
    scene_change_data.second = data.scene_type();
    return scene_change_data;
  }
  case EventDataType::EventDataType_UIElementName: {
    if (!element_data.trigger_event_data_as_UIElementName()) {
      FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                         "Trigger event UIElementName not found"};
      return std::unexpected(fail_info);
    }

    UIElementName ui_element_name =
        element_data.trigger_event_data_as_UIElementName()->str();
    return ui_element_name;
  }
  case EventDataType::EventDataType_NONE:
  default:
    return std::monostate{};
  }
}
/////////////////////////////////////////////////
std::expected<EventData, FailInfo> UIElementFactory::ConfigureResponseEventData(
    const UIElementData &element_data) {
  switch (element_data.response_event_data_type()) {
  case EventDataType::EventDataType_UserInputBitsetData: {
    if (!element_data.response_event_data_as_UserInputBitsetData()) {
      FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                         "Response event UserInputBitsetData not found"};
      return std::unexpected(fail_info);
    }
    UserInputBitset user_input_data = ConvertActionKeysToEvent(
        *element_data.response_event_data_as_UserInputBitsetData());
    return user_input_data;
  }
  case EventDataType::EventDataType_SceneChangeData: {
    if (!element_data.response_event_data_as_SceneChangeData()) {
      FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                         "Response event SceneChangeData not found"};
      return std::unexpected(fail_info);
    }

    SceneChangeData scene_change_data;
    const auto &data = *element_data.response_event_data_as_SceneChangeData();
    if (data.scene_id()) {
      auto uuid_opt = uuids::uuid::from_string(data.scene_id()->str());
      if (!uuid_opt.has_value()) {
        FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                           "Invalid scene_id format in response event"};
        return std::unexpected(fail_info);
      }
      scene_change_data.first = uuid_opt.value();
    }
    scene_change_data.second = data.scene_type();
    return scene_change_data;
  }
  case EventDataType::EventDataType_UIElementName: {
    if (!element_data.response_event_data_as_UIElementName()) {
      FailInfo fail_info{FailMode::FlatbuffersDataNotFound,
                         "Response event UIElementName not found"};
      return std::unexpected(fail_info);
    }
    UIElementName ui_element_name =
        element_data.response_event_data_as_UIElementName()->str();
    return ui_element_name;
  }
  case EventDataType::EventDataType_NONE:
  default:
    return std::monostate{};
  }
}
} // namespace steamrot
