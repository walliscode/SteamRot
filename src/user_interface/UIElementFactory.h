#pragma once
/////////////////////////////////////////////////
/// @file
/// @brief Decelaration of the UIElementFactory class
/////////////////////////////////////////////////

#include "DropDown.h"
#include "UIElement.h"
#include "user_interface_generated.h"
namespace steamrot {

class UIElementFactory {
private:
  /////////////////////////////////////////////////
  /// @brief Recursively builds a UIElement from the provided data.
  ///
  /// @param element_data Provided flatbuffer data for the UI element
  /// @return Nested UIElement containing the element type and its children.
  /////////////////////////////////////////////////
  UIElement RecursivlyBuildUIElement(const UIElementData &element_data);

  /////////////////////////////////////////////////
  /// @brief Provides the UIElement with shared properties
  ///
  /// @param element Flatbuffer data for the UI element
  /// @return Generally configured UIElement
  /////////////////////////////////////////////////
  UIElement ConfigureGeneralUIElement(const UIElementData &element);

  /////////////////////////////////////////////////
  /// @brief Provides a Panel struct for UIElement.element_type
  ///
  /// @param element Panel specific flatbuffers data
  /// @return A configured Panel struct
  /////////////////////////////////////////////////
  Panel ConfigurePanel(const PanelData &panel_data);

  /////////////////////////////////////////////////
  /// @brief Provides a Button struct for UIElement.element_type
  ///
  /// @param button_data Button specific flatbuffers data
  /// @return A configured Button struct
  /////////////////////////////////////////////////
  Button ConfigureButton(const ButtonData &button_data);

  /////////////////////////////////////////////////
  /// @brief Provides a DropDown struct for UIElement.element_type
  ///
  /// @param dropdown_data DrowDown specific flatbuffers data
  /// @return A configured DropDown struct
  /////////////////////////////////////////////////
  DropDown ConfigureDropDown(const DropDownData &dropdown_data);

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
  UIElement CreateUIStructure(const UIElementData &element_data);
};
} // namespace steamrot
