#pragma once

#include "user_interface_generated.h"
#include <flatbuffers/flatbuffers.h>
#include <string>

namespace steamrot::tests {

class TestUIElementDataFactory {
public:
  // Create base UIElementData - does NOT finish buffer
  static flatbuffers::Offset<UIElementData> CreateTestUIElementData(
      flatbuffers::FlatBufferBuilder &builder, float x = 10, float y = 20,
      float w = 100, float h = 200, bool children_active = true,
      SpacingAndSizingType spacing = SpacingAndSizingType_DropDownList,
      LayoutType layout = LayoutType_Grid) {
    auto position = CreateVector2fData(builder, x, y);
    auto size = CreateVector2fData(builder, w, h);
    auto children = builder.CreateVector<flatbuffers::Offset<child>>({});
    return CreateUIElementData(builder, position, size, children_active,
                               children, layout, spacing);
  }

  // Create PanelData, finish buffer, return pointer
  static const PanelData *
  CreateTestPanelData(flatbuffers::FlatBufferBuilder &builder) {
    auto base = CreateTestUIElementData(builder);
    auto panel_offset = CreatePanelData(builder, base);
    builder.Finish(panel_offset);
    return flatbuffers::GetRoot<PanelData>(builder.GetBufferPointer());
  }

  // Create ButtonData, finish buffer, return pointer
  static const ButtonData *
  CreateTestButtonData(flatbuffers::FlatBufferBuilder &builder,
                       const std::string &label = "Button") {
    auto base = CreateTestUIElementData(builder, 10, 20, 100, 40, false,
                                        SpacingAndSizingType_None,
                                        LayoutType_Horizontal);
    auto label_str = builder.CreateString(label);
    auto button_offset = CreateButtonData(builder, base, label_str);
    builder.Finish(button_offset);
    return flatbuffers::GetRoot<ButtonData>(builder.GetBufferPointer());
  }
};

} // namespace steamrot::tests
