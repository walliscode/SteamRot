/////////////////////////////////////////////////
/// @file
/// @brief Provides factory methods to create test UIElementData flatbuffers
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "mock_fb_subscriber_data.h"
#include "user_interface_generated.h"
#include <flatbuffers/flatbuffers.h>
#include <string>
#include <vector>

namespace steamrot::tests {

class TestUIElementDataFactory {
public:
  // Create base UIElementData - does NOT finish buffer
  static flatbuffers::Offset<UIElementData> CreateTestUIElementData(
      flatbuffers::FlatBufferBuilder &builder, float x = 10, float y = 20,
      float w = 100, float h = 200, bool children_active = true,
      SpacingAndSizingType spacing = SpacingAndSizingType_DropDownList,
      LayoutType layout = LayoutType_Grid,
      flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<child>>>
          children = 0) {

    auto position = CreateVector2fData(builder, x, y);
    auto size = CreateVector2fData(builder, w, h);
    auto subscriber_data = CreateTestUserInputSubscriberData(builder);

    if (!children.o) {
      children = builder.CreateVector<flatbuffers::Offset<child>>({});
    }
    return CreateUIElementData(builder, position, size, subscriber_data,
                               children_active, children, layout, spacing);
  }

  // Create child wrapper for a UIElementDataUnion
  static flatbuffers::Offset<child>
  CreateTestChild(flatbuffers::FlatBufferBuilder &builder,
                  UIElementDataUnion type, flatbuffers::Offset<void> data) {
    return Createchild(builder, type, data);
  }

  // Create PanelData, finish buffer, return pointer
  static const PanelData *CreateTestPanelData(
      flatbuffers::FlatBufferBuilder &builder,
      flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<child>>>
          children = 0) {
    auto base = CreateTestUIElementData(builder, 10, 20, 100, 200, true,
                                        SpacingAndSizingType_DropDownList,
                                        LayoutType_Grid, children);
    auto panel_offset = CreatePanelData(builder, base);
    builder.Finish(panel_offset);
    return flatbuffers::GetRoot<PanelData>(builder.GetBufferPointer());
  }

  // Create ButtonData, finish buffer, return pointer
  static const ButtonData *CreateTestButtonData(
      flatbuffers::FlatBufferBuilder &builder,
      const std::string &label = "Button",
      flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<child>>>
          children = 0) {
    auto base = CreateTestUIElementData(builder, 10, 20, 100, 40, false,
                                        SpacingAndSizingType_None,
                                        LayoutType_Horizontal, children);
    auto label_str = builder.CreateString(label);
    auto button_offset = CreateButtonData(builder, base, label_str);
    builder.Finish(button_offset);
    return flatbuffers::GetRoot<ButtonData>(builder.GetBufferPointer());
  }

  // Create DropDownContainerData with DropDownListData and DropDownButtonData
  // as children, finish buffer, return pointer
  static const DropDownContainerData *CreateTestDropDownContainerData(
      flatbuffers::FlatBufferBuilder &builder,
      // Optionally allow custom DropDownListData and DropDownButtonData args
      const std::string &list_label = "List",
      const std::string &list_expanded_label = "Expanded",
      DataPopulateFunction data_populate_function = DataPopulateFunction_None,
      bool button_is_expanded = false) {
    // --- Create DropDownList child ---
    auto ddl_base =
        CreateTestUIElementData(builder, 10, 20, 100, 40, false,
                                SpacingAndSizingType_None, LayoutType_DropDown);
    auto ddl_label_offset = builder.CreateString(list_label);
    auto ddl_expanded_label_offset = builder.CreateString(list_expanded_label);
    auto ddl_offset = CreateDropDownListData(
        builder, ddl_base, ddl_label_offset, ddl_expanded_label_offset,
        data_populate_function);

    // --- Create DropDownButton child ---
    auto ddb_base =
        CreateTestUIElementData(builder, 10, 20, 100, 40, false,
                                SpacingAndSizingType_None, LayoutType_DropDown);
    auto ddb_offset =
        CreateDropDownButtonData(builder, ddb_base, button_is_expanded);

    // --- Wrap both children ---
    std::vector<std::pair<UIElementDataUnion, flatbuffers::Offset<void>>>
        children_data{
            {UIElementDataUnion_DropDownListData, ddl_offset.Union()},
            {UIElementDataUnion_DropDownButtonData, ddb_offset.Union()}};
    auto children_vec = CreateChildrenVector(builder, children_data);

    // --- Create DropDownContainer with these children ---
    auto base = CreateTestUIElementData(builder, 10, 20, 100, 200, true,
                                        SpacingAndSizingType_DropDownList,
                                        LayoutType_DropDown, children_vec);
    auto offset = CreateDropDownContainerData(builder, base);
    builder.Finish(offset);
    return flatbuffers::GetRoot<DropDownContainerData>(
        builder.GetBufferPointer());
  }

  // Create DropDownListData, finish buffer, return pointer
  static const DropDownListData *CreateTestDropDownListData(
      flatbuffers::FlatBufferBuilder &builder,
      const std::string &label = "List",
      const std::string &expanded_label = "Expanded",
      DataPopulateFunction data_populate_function = DataPopulateFunction_None,
      flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<child>>>
          children = 0) {
    auto base = CreateTestUIElementData(builder, 10, 20, 100, 40, false,
                                        SpacingAndSizingType_None,
                                        LayoutType_DropDown, children);
    auto label_offset = builder.CreateString(label);
    auto expanded_label_offset = builder.CreateString(expanded_label);
    auto offset =
        CreateDropDownListData(builder, base, label_offset,
                               expanded_label_offset, data_populate_function);
    builder.Finish(offset);
    return flatbuffers::GetRoot<DropDownListData>(builder.GetBufferPointer());
  }

  // Create DropDownItemData, finish buffer, return pointer
  static const DropDownItemData *CreateTestDropDownItemData(
      flatbuffers::FlatBufferBuilder &builder,
      const std::string &label = "Item",
      flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<child>>>
          children = 0) {
    auto base = CreateTestUIElementData(builder, 10, 20, 100, 40, false,
                                        SpacingAndSizingType_None,
                                        LayoutType_DropDown, children);
    auto label_offset = builder.CreateString(label);
    auto offset = CreateDropDownItemData(builder, base, label_offset);
    builder.Finish(offset);
    return flatbuffers::GetRoot<DropDownItemData>(builder.GetBufferPointer());
  }

  // Create DropDownButtonData, finish buffer, return pointer
  static const DropDownButtonData *CreateTestDropDownButtonData(
      flatbuffers::FlatBufferBuilder &builder, bool is_expanded = false,
      flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<child>>>
          children = 0) {
    auto base = CreateTestUIElementData(builder, 10, 20, 100, 40, false,
                                        SpacingAndSizingType_None,
                                        LayoutType_DropDown, children);
    auto offset = CreateDropDownButtonData(builder, base, is_expanded);
    builder.Finish(offset);
    return flatbuffers::GetRoot<DropDownButtonData>(builder.GetBufferPointer());
  }

  // --- Utility to create a vector of children from UIElementDataUnions ---
  static flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<child>>>
  CreateChildrenVector(
      flatbuffers::FlatBufferBuilder &builder,
      const std::vector<std::pair<UIElementDataUnion,
                                  flatbuffers::Offset<void>>> &children_data) {
    std::vector<flatbuffers::Offset<child>> children_vec;
    for (const auto &[type, data] : children_data) {
      children_vec.push_back(steamrot::Createchild(builder, type, data));
    }

    return builder.CreateVector(children_vec);
  }

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Create a 4-level nested UIElementData structure for testing.
  /// Example structure (Panel):
  /// - Panel
  ///   - DropDownContainer
  ///     - DropDownList
  ///       - Button
  ///       - DropDownItem
  ///     - DropDownButton
  //////////////////////////////////////////////////////////////////////////////
  static const PanelData *
  CreateDeeplyNestedTestPanel(flatbuffers::FlatBufferBuilder &builder) {
    // Level 3: Children of DropDownList
    auto button_base = CreateTestUIElementData(builder, 5, 5, 20, 10, false,
                                               SpacingAndSizingType_None,
                                               LayoutType_Horizontal);
    auto button_label = builder.CreateString("NestedButton");
    auto button_offset = CreateButtonData(builder, button_base, button_label);

    auto item_base =
        CreateTestUIElementData(builder, 6, 6, 18, 8, false,
                                SpacingAndSizingType_None, LayoutType_DropDown);
    auto item_label = builder.CreateString("NestedItem");
    auto item_offset = CreateDropDownItemData(builder, item_base, item_label);

    std::vector<std::pair<UIElementDataUnion, flatbuffers::Offset<void>>>
        ddl_children{
            {UIElementDataUnion_ButtonData, button_offset.Union()},
            {UIElementDataUnion_DropDownItemData, item_offset.Union()}};
    auto ddl_children_vec = CreateChildrenVector(builder, ddl_children);

    // Level 2: DropDownList, DropDownButton
    auto ddl_base = CreateTestUIElementData(
        builder, 7, 7, 60, 20, false, SpacingAndSizingType_None,
        LayoutType_DropDown, ddl_children_vec);
    auto ddl_label = builder.CreateString("ListLevel2");
    auto ddl_expanded_label = builder.CreateString("ExpandedLevel2");
    auto ddl_offset =
        CreateDropDownListData(builder, ddl_base, ddl_label, ddl_expanded_label,
                               DataPopulateFunction_None);

    auto ddb_base =
        CreateTestUIElementData(builder, 8, 8, 20, 10, false,
                                SpacingAndSizingType_None, LayoutType_DropDown);
    auto ddb_offset = CreateDropDownButtonData(builder, ddb_base, false);

    std::vector<std::pair<UIElementDataUnion, flatbuffers::Offset<void>>>
        ddcontainer_children{
            {UIElementDataUnion_DropDownListData, ddl_offset.Union()},
            {UIElementDataUnion_DropDownButtonData, ddb_offset.Union()}};
    auto ddcontainer_children_vec =
        CreateChildrenVector(builder, ddcontainer_children);

    // Level 1: DropDownContainer as Panel's child
    auto ddcontainer_base = CreateTestUIElementData(
        builder, 9, 9, 100, 40, true, SpacingAndSizingType_DropDownList,
        LayoutType_DropDown, ddcontainer_children_vec);
    auto ddcontainer_offset =
        CreateDropDownContainerData(builder, ddcontainer_base);

    // Wrap DropDownContainer as a child for the Panel root
    std::vector<std::pair<UIElementDataUnion, flatbuffers::Offset<void>>>
        panel_children{{UIElementDataUnion_DropDownContainerData,
                        ddcontainer_offset.Union()}};
    auto panel_children_vec = CreateChildrenVector(builder, panel_children);

    // Create the Panel as the root element
    auto panel_base = CreateTestUIElementData(
        builder, 10, 20, 100, 200, true, SpacingAndSizingType_DropDownList,
        LayoutType_Grid, panel_children_vec);
    auto panel_offset = CreatePanelData(builder, panel_base);

    builder.Finish(panel_offset);
    return flatbuffers::GetRoot<PanelData>(builder.GetBufferPointer());
  }
};

} // namespace steamrot::tests
