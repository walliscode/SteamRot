/////////////////////////////////////////////////
/// @file
/// @brief helper functions for UIElementFactory unit tests
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ButtonElement.h"
#include "DropDownElement.h"
#include "PanelElement.h"
#include "UIElement.h"
#include "user_interface_generated.h"
#include <catch2/catch_test_macros.hpp>

namespace steamrot {
namespace tests {

void TestUIELementProperites(const UIElement &element,
                             const UIElementData &data);

void TestPanelElementProperties(const PanelElement &element,
                                const PanelData &data);

void TestButtonElementProperties(const ButtonElement &element,
                                 const ButtonData &data);

void TestDropDownListElementProperties(const DropDownListElement &element,
                                       const DropDownListData &data);

void TestDropDownContainerElementProperties(
    const DropDownContainerElement &element, const DropDownContainerData &data);

void TestDropDownItemElementProperties(const DropDownItemElement &element,
                                       const DropDownItemData &data);

void TestDropDownButtonElementProperties(const DropDownButtonElement &element,
                                         const DropDownButtonData &data);

} // namespace tests
} // namespace steamrot
