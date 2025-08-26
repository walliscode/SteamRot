/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the StylesConfigurator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once
#include "FailInfo.h"
#include "UIStyle.h"
#include "ui_style_generated.h"
#include <expected>
#include <string>
#include <vector>
namespace steamrot {

/////////////////////////////////////////////////
/// @class StylesConfigurator
/// @brief Responsible for providing UI styles configured from external data
///
/////////////////////////////////////////////////
class StylesConfigurator {

private:
public:
  StylesConfigurator() = default;

  /////////////////////////////////////////////////
  /// @brief Gather all available style names from the styles directory
  /////////////////////////////////////////////////
  std::expected<std::vector<std::string>, FailInfo> GetAllStyleNames();

  /////////////////////////////////////////////////
  /// @brief Configure a UIStyle object based on the provided flatbuffer
  ///
  /// @param style_data Flatbuffer data containing style configuration
  /////////////////////////////////////////////////
  std::expected<UIStyle, FailInfo>
  ConfigureStyle(const UIStyleData &style_data);
};
} // namespace steamrot
