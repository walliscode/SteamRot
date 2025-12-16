////////////////////////////////////////////////////////////
/// @file
/// @brief Declaration of FlatbuffersUIElementConfigurator class
////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include "IUIElementConfigurator.h"
#include "user_interface_generated.h"

namespace steamrot {

////////////////////////////////////////////////////////////
/// @class FlatbuffersUIElementConfigurator
/// @brief Configures UIElement instances from FlatBuffers data
////////////////////////////////////////////////////////////
class FlatbuffersUIElementConfigurator : public IUIElementConfigurator {
private:
  ////////////////////////////////////////////////////////////
  /// @brief Reference to the UserInterfaceData FlatBuffers data
  ////////////////////////////////////////////////////////////
  const UserInterfaceData &m_ui_data;

  ////////////////////////////////////////////////////////////
  /// @brief Create a UIElement from FlatBuffers data
  ///
  /// @param data_type The FlatBuffers union type for the element
  /// @param data Pointer to the root FlatBuffers table of the element
  /// @return A std::expected containing a unique_ptr to a UIElement, or
  /// FailInfo on error
  ////////////////////////////////////////////////////////////
  std::expected<std::unique_ptr<UIElement>, FailInfo>
  CreateUIElement(const UIElementDataUnion &data_type, const void *data);

  ////////////////////////////////////////////////////////////
  /// @brief Configure the base properties of a UIElement
  ///
  /// @param element UIElement to configure
  /// @param data FlatBuffers data to configure from
  ////////////////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureBaseUIElement(UIElement &element, const UIElementData &data);

  ////////////////////////////////////////////////////////////
  /// @brief Configure a Panel UIElement from FlatBuffers data
  ////////////////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigurePanelElement(PanelElement &panel_element, const PanelData &data);

  ////////////////////////////////////////////////////////////
  /// @brief Configure a Button UIElement from FlatBuffers data
  ////////////////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureButtonElement(ButtonElement &button_element, const ButtonData &data);

  ////////////////////////////////////////////////////////////
  /// @brief Configure a DropDownList UIElement from FlatBuffers data
  ////////////////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> ConfigureDropDownListElement(
      DropDownListElement &dropdown_list_element,
      const DropDownListData &data);

  ////////////////////////////////////////////////////////////
  /// @brief Configure a DropDownContainer UIElement from FlatBuffers data
  ////////////////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> ConfigureDropDownContainerElement(
      DropDownContainerElement &dropdown_container_element,
      const DropDownContainerData &data);

  ////////////////////////////////////////////////////////////
  /// @brief Configure a DropDownItem UIElement from FlatBuffers data
  ////////////////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> ConfigureDropDownItemElement(
      DropDownItemElement &dropdown_item_element,
      const DropDownItemData &data);

  ////////////////////////////////////////////////////////////
  /// @brief Configure a DropDownButton UIElement from FlatBuffers data
  ////////////////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> ConfigureDropDownButtonElement(
      DropDownButtonElement &dropdown_button_element,
      const DropDownButtonData &data);

  ////////////////////////////////////////////////////////////
  /// @brief Convert FlatBuffers LayoutFbs to native Layout
  ////////////////////////////////////////////////////////////
  static Layout ConvertLayout(int8_t fbs_layout);

  ////////////////////////////////////////////////////////////
  /// @brief Convert FlatBuffers SpacingAndSizingFbs to native SpacingAndSizing
  ////////////////////////////////////////////////////////////
  static SpacingAndSizing ConvertSpacingAndSizing(int8_t fbs_spacing);

public:
  ////////////////////////////////////////////////////////////
  /// @brief Constructor
  ///
  /// @param event_handler Reference to the EventHandler
  /// @param ui_data Reference to the UserInterfaceData to configure from
  ////////////////////////////////////////////////////////////
  FlatbuffersUIElementConfigurator(EventHandler &event_handler,
                                   const UserInterfaceData &ui_data);

  ////////////////////////////////////////////////////////////
  /// @brief Create a root UIElement from FlatBuffers data
  ////////////////////////////////////////////////////////////
  std::expected<std::unique_ptr<UIElement>, FailInfo>
  CreateRootUIElement() override;
};

} // namespace steamrot
