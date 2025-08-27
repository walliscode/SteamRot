/////////////////////////////////////////////////
/// @file
/// @brief Provides factory methods to create test UIElementData flatbuffers
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
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
    if (!children.o) {
      children = builder.CreateVector<flatbuffers::Offset<child>>({});
    }
    return CreateUIElementData(builder, position, size, children_active,
                               children, layout, spacing);
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

  // Create DropDownContainerData, finish buffer, return pointer
  static const DropDownContainerData *CreateTestDropDownContainerData(
      flatbuffers::FlatBufferBuilder &builder,
      flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<child>>>
          children = 0) {
    auto base = CreateTestUIElementData(builder, 10, 20, 100, 200, true,
                                        SpacingAndSizingType_DropDownList,
                                        LayoutType_DropDown, children);
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
};

} // namespace steamrot::tests
