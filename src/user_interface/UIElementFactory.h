/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the UIElementFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "DropDown.h"
#include "FlatbuffersConfigurator.h"
#include "Panel.h"
#include "UIElement.h"
#include "user_interface_generated.h"
#include <expected>

namespace steamrot {

class UIElementFactory {
private:
  /////////////////////////////////////////////////
  /// @brief Recursively builds a UIElement from the provided data.
  ///
  /// @param element_data Provided flatbuffer data for the UI element
  /// @return Nested UIElement containing the element type and its children.
  /////////////////////////////////////////////////
  std::expected<UIElement, FailInfo>
  RecursivlyBuildUIElement(const UIElementData &element_data);

  /////////////////////////////////////////////////
  /// @brief Provides the UIElement with shared properties
  ///
  /// @param element Flatbuffer data for the UI element
  /// @return Generally configured UIElement
  /////////////////////////////////////////////////
  std::expected<UIElement, FailInfo>
  ConfigureGeneralUIElement(const UIElementData &element);

  /////////////////////////////////////////////////
  /// @brief Provides a Panel struct for UIElement.element_type
  ///
  /// @param element Panel specific flatbuffers data
  /// @return A configured Panel struct
  /////////////////////////////////////////////////
  std::expected<Panel, FailInfo> ConfigurePanel(const PanelData &panel_data);

  /////////////////////////////////////////////////
  /// @brief Provides a Button struct for UIElement.element_type
  ///
  /// @param button_data Button specific flatbuffers data
  /// @return A configured Button struct
  /////////////////////////////////////////////////
  std::expected<Button, FailInfo>
  ConfigureButton(const ButtonData &button_data);

  /////////////////////////////////////////////////
  /// @brief Provides a DropDown struct for UIElement.element_type
  ///
  /// @param dropdown_data DrowDown specific flatbuffers data
  /// @return A configured DropDown struct
  /////////////////////////////////////////////////
  std::expected<DropDownContainer, FailInfo>
  ConfigureDropDownContainer(const DropDownContainerData &dropdown_data);

  /////////////////////////////////////////////////
  /// @brief Provides a DropDownList struct for UIElement.element_type
  ///
  /// @param dropdown_data DropDownList specific flatbuffers data
  /// @return A configured DropDownList struct
  /////////////////////////////////////////////////
  std::expected<DropDownList, FailInfo>
  ConfigureDropDownList(const DropDownListData &dropdown_data);

  /////////////////////////////////////////////////
  /// @brief Provides a DropDownItem struct for UIElement.element_type
  ///
  /// @param dropdown_data DropDownItem specific flatbuffers data
  /// @return A configured DropDownItem struct
  /////////////////////////////////////////////////
  std::expected<DropDownItem, FailInfo>
  ConfigureDropDownItem(const DropDownItemData &dropdown_data);

  /////////////////////////////////////////////////
  /// @brief Provides a DropDownButton struct for UIElement.element_type
  ///
  /// @param dropdown_data DropDownButton specific flatbuffers data
  /// @return A configured DropDownButton struct
  /////////////////////////////////////////////////
  std::expected<DropDownButton, FailInfo>
  ConfigureDropDownButton(const DropDownButtonData &dropdown_data);

  /////////////////////////////////////////////////
  /// @brief Configures Trigger Event Data for a UI element
  ///
  /// @param element_data UI element data
  /// @return Configured EventData or error
  /////////////////////////////////////////////////
  std::expected<EventData, FailInfo>
  ConfigureTriggerEventData(const UIElementData &element_data);

  /////////////////////////////////////////////////
  /// @brief Configures Response Event Data for a UI element
  ///
  /// @param element_data UI element data
  /// @return Configured EventData or error
  /////////////////////////////////////////////////
  std::expected<EventData, FailInfo>
  ConfigureResponseEventData(const UIElementData &element_data);

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor for the UIElementFactory
  /////////////////////////////////////////////////
  UIElementFactory() = default;

  /////////////////////////////////////////////////
  /// @brief Creates a (nested) UIElement structure from the provided data.
  ///
  /// @param element_data Flatbuffer data for the UI element
  /// @return A UIElement containing the element type and its children.
  /////////////////////////////////////////////////
  std::expected<UIElement, FailInfo>
  CreateUIStructure(const UIElementData &element_data);

  /////////////////////////////////////////////////
  /// @brief Creates a DropDownItem structure (example public test)
  ///
  /// @return A DropDownItem or an error
  /////////////////////////////////////////////////
  std::expected<UIElement, FailInfo> CreateDropDownItem();
};

} // namespace steamrot
