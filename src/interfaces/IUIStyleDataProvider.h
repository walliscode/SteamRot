////////////////////////////////////////////////////////////
/// @file
/// @brief Declaration of IUIStyleDataProvider interface
////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include "FailInfo.h"
#include "UIStyle.h"
#include <expected>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace steamrot {

// Forward declaration
class IFontProvider;

////////////////////////////////////////////////////////////
/// @class IUIStyleDataProvider
/// @brief Interface for loading and configuring UI style data from various sources
///
/// Providers load raw style data and configure UIStyle objects using the
/// provided IFontProvider for font resources.
////////////////////////////////////////////////////////////
class IUIStyleDataProvider {
public:
  ////////////////////////////////////////////////////////////
  /// @brief Virtual destructor
  ////////////////////////////////////////////////////////////
  virtual ~IUIStyleDataProvider() = default;

  ////////////////////////////////////////////////////////////
  /// @brief Load and configure multiple UI styles by name
  ///
  /// @param font_provider Reference to IFontProvider for loading fonts
  /// @param style_names Vector of style names to load
  /// @return Map of style name to configured UIStyle, or error
  ////////////////////////////////////////////////////////////
  virtual std::expected<std::unordered_map<std::string, UIStyle>, FailInfo>
  ProvideUIStylesMap(const IFontProvider &font_provider,
                     const std::vector<std::string> &style_names) = 0;
};

} // namespace steamrot
