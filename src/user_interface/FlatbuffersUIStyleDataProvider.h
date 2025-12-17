/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the StylesConfigurator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "FailInfo.h"
#include "IFontProvider.h"
#include "IUIStyleDataProvider.h"
#include "UIStyle.h"
#include "ui_style_generated.h"
#include <expected>
#include <string>
#include <unordered_map>
#include <vector>
namespace steamrot {

/////////////////////////////////////////////////
/// @class StylesConfigurator
/// @brief Responsible for providing UI styles configured from external data
///
/////////////////////////////////////////////////
class FlatbuffersUIStyleDataProvider : public IUIStyleDataProvider {

private:
  /////////////////////////////////////////////////
  /// @brief Configure a UIStyle object based on the provided flatbuffer
  ///
  /// @param style_data Flatbuffer data containing style configuration
  /// @param font_provider Reference to IFontProvider for font loading
  /////////////////////////////////////////////////
  std::expected<UIStyle, FailInfo>
  ConfigureStyle(const UIStyleData &style_data,
                 const IFontProvider &font_provider);

public:
  FlatbuffersUIStyleDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Provide a map of all available UIStyles (implements interface)
  ///
  /// @param font_provider Reference to IFontProvider for font loading
  /// @param style_names Vector of style names to load
  /// @return Map of style name to configured UIStyle, or error
  /////////////////////////////////////////////////
  std::expected<std::unordered_map<std::string, UIStyle>, FailInfo>
  ProvideUIStylesMap(const IFontProvider &font_provider,
                     const std::vector<std::string> &style_names) override;
};
} // namespace steamrot
