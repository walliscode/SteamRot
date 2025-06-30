/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the UIElementFactory class
/////////////////////////////////////////////////
#include "UIElementFactory.h"
#include "DropDown.h"
#include "user_interface_generated.h"
#include <iostream>

namespace steamrot {

/////////////////////////////////////////////////
UIElement
UIElementFactory::CreateUIStructure(const UIElementData &element_data) {
  std::cout << "Creating UIElement structure from data" << std::endl;
  // Recursively build the UIElement structure
  UIElement ui_element = RecursivlyBuildUIElement(element_data);
  std::cout << "UIElement structure created successfully." << std::endl;
  return ui_element;
};

/////////////////////////////////////////////////
UIElement
UIElementFactory::RecursivlyBuildUIElement(const UIElementData &element_data) {
  // create UIElement with general configuration
  UIElement ui_element = ConfigureGeneralUIElement(element_data);

  std::cout << "Configured general UIElement properties" << std::endl;

  // Configure the UIElement based on its type
  switch (element_data.type()) {

    // Panel
  case UIElementType::UIElementType_Panel: {
    std::cout << "Configuring Panel UIElement" << std::endl;
    ui_element.element_type =
        ConfigurePanel(*element_data.element_as_PanelData());
    break;
  }
    // Button
  case UIElementType::UIElementType_Button: {
    std::cout << "Configuring Button UIElement" << std::endl;
    ui_element.element_type =
        ConfigureButton(*element_data.element_as_ButtonData());

    std::cout << "Button label: "
              << std::get<Button>(ui_element.element_type).label << std::endl;
    break;
  }
    // DropDownContainer
  case UIElementType::UIElementType_DropDownContainer: {
    std::cout << "Configuring DropDown UIElement" << std::endl;
    ui_element.element_type = ConfigureDropDownContainer(
        *element_data.element_as_DropDownContainerData());

    break;
  }

    // DropDownList
  case UIElementType::UIElementType_DropDownList: {
    std::cout << "Configuring DropDownList UIElement" << std::endl;
    ui_element.element_type =
        ConfigureDropDownList(*element_data.element_as_DropDownListData());
    break;
  }

    // DropDownItem
  case UIElementType::UIElementType_DropDownItem: {
    std::cout << "Configuring DropDownItem UIElement" << std::endl;
    ui_element.element_type =
        ConfigureDropDownItem(*element_data.element_as_DropDownItemData());
    break;
  }

  // DropDownButton
  case UIElementType::UIElementType_DropDownButton: {
    std::cout << "Configuring DropDownButton UIElement" << std::endl;
    ui_element.element_type =
        ConfigureDropDownButton(*element_data.element_as_DropDownButtonData());
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
      UIElement child_ui_element = RecursivlyBuildUIElement(*child_element);
      // Add the child UIElement to the parent UIElement
      ui_element.child_elements.push_back(child_ui_element);
    }
  }
  return ui_element;
};

UIElement
UIElementFactory::ConfigureGeneralUIElement(const UIElementData &element) {
  // create UIElement
  UIElement ui_element;
  // general configuration for the UIElement (only variables specificied in
  // the UIElement struct)
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

  if (element.action()) {
    ui_element.action = element.action()->action_name();

    // convert action keys to EventBitset
    ui_element.trigger_event = ConvertActionKeysToEvent(*element.action());
  }
  if (element.data()) {

    // give it a scene type enum based on string (this needs to be switched to
    // a fbs enum at some point)
    if (element.data()->scene_type()) {

      // Title scene type
      if (element.data()->scene_type()->str() == "TITLE") {
        ui_element.ui_data_package.scene_type = SceneType::title;
        // Crafting scene type
      } else if (element.data()->scene_type()->str() == "CRAFTING") {
        ui_element.ui_data_package.scene_type = SceneType::crafting;
      }
    }
  }
  return ui_element;
}

/////////////////////////////////////////////////
Panel UIElementFactory::ConfigurePanel(const PanelData &panel_data) {
  Panel panel_element;
  // configure the panel element

  return panel_element;
}

/////////////////////////////////////////////////
Button UIElementFactory::ConfigureButton(const ButtonData &button_data) {

  Button button_element;

  // configure the button element
  if (button_data.label()) {

    button_element.label = button_data.label()->str();
  }
  // add to the UIElement
  return button_element;
}

/////////////////////////////////////////////////
DropDownContainer UIElementFactory::ConfigureDropDownContainer(
    const DropDownContainerData &dropdown_data) {
  DropDownContainer dropdown_element;
  // configure the dropdown_element

  return dropdown_element;
}

/////////////////////////////////////////////////
DropDownList
UIElementFactory::ConfigureDropDownList(const DropDownListData &dropdown_data) {
  DropDownList dropdown_list_element;
  std::cout << "Configuring DropDownList UIElement" << std::endl;
  // configure the dropdown_list_element
  dropdown_list_element.label = dropdown_data.label()->str();
  dropdown_list_element.expanded_label = dropdown_data.expanded_label()->str();
  return dropdown_list_element;
}

//////////////////////////////////////////////////
DropDownItem
UIElementFactory::ConfigureDropDownItem(const DropDownItemData &dropdown_data) {
  DropDownItem dropdown_item_element;
  // configure the dropdown_item_element
  dropdown_item_element.label = dropdown_data.label()->str();

  return dropdown_item_element;
}

///////////////////////////////////////////////////
DropDownButton UIElementFactory::ConfigureDropDownButton(
    const DropDownButtonData &dropdown_data) {
  DropDownButton dropdown_button_element;
  // configure the dropdown_button_element
  dropdown_button_element.is_expanded = dropdown_data.is_expanded();
  return dropdown_button_element;
}

} // namespace steamrot
