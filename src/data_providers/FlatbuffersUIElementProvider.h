////////////////////////////////////////////////////////////
/// @file
/// @brief Declaration of FlatbuffersUIElementProvider class
////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include "ButtonElement.h"
#include "DropDownButtonElement.h"
#include "DropDownContainerElement.h"
#include "DropDownItemElement.h"
#include "DropDownListElement.h"
#include "IUIElementProvider.h"
#include "PanelElement.h"
#include "user_interface_generated.h"

namespace steamrot {

////////////////////////////////////////////////////////////
/// @class FlatbuffersUIElementProvider
/// @brief Provides and configures UIElement instances from FlatBuffers data
////////////////////////////////////////////////////////////
class FlatbuffersUIElementProvider : public IUIElementProvider {

private:
  ////////////////////////////////////////////////////////////
  /// @brief Reference to the UserInterfaceData FlatBuffers data
  ////////////////////////////////////////////////////////////
  const UserInterfaceFbs &m_ui_data;

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

public:
  ////////////////////////////////////////////////////////////
  /// @brief Constructor
  ///
  /// @param event_handler Reference to the EventHandler
  /// @param ui_data Reference to the UserInterfaceData to configure from
  ////////////////////////////////////////////////////////////
  FlatbuffersUIElementProvider(EventHandler &event_handler,
                               const UserInterfaceFbs &ui_data);

  ////////////////////////////////////////////////////////////
  /// @brief Create a root UIElement from FlatBuffers data
  ///
  /// @return A std::expected containing a unique_ptr to a UIElement, or
  /// FailInfo on error
  ////////////////////////////////////////////////////////////
  std::expected<std::unique_ptr<UIElement>, FailInfo>
  CreateRootUIElement() override;

  ////////////////////////////////////////////////////////////
  /// @brief Configure a root UIElement from FlatBuffers data
  ///
  /// @param root_element Root UIElement to configure
  /// @return std::expected with monostate on success, or FailInfo on error
  ////////////////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureRootUIElement(UIElement &root_element) override;
};

} // namespace steamrot
