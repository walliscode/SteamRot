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
#include "FailInfo.h"
#include "PanelElement.h"
#include "UIElement.h"
#include "user_interface_generated.h"
#include <expected>
#include <memory>
#include <variant>

namespace steamrot {

class UIElementFactory {
private:
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
};

std::expected<std::unique_ptr<UIElement>, FailInfo>
CreateUIElement(const UIElementDataUnion &data_type, const void *data);

/////////////////////////////////////////////////
/// @brief Configure the base properties of a UIElement from the provided
/// flatbuffers data
///
/// @param element UIElement to configure
/// @param data Flatbuffers data to configure from
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureBaseUIElement(UIElement &element, const UIElementData &data);

/////////////////////////////////////////////////
/// @brief Configure a Panel UIElement from the provided flatbuffers data
///
/// @param element PanelElement to configure
/// @param data Flatbuffers data to configure from
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigurePanelElement(PanelElement &panel_element, const PanelData &data);

} // namespace steamrot
