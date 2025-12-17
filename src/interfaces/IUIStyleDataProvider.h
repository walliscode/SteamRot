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
<<<<<<< HEAD
#include <memory>
#include <string>
#include <unordered_map>
=======
>>>>>>> a7787f7 (builds successfully)
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
protected:
  const std::unordered_map<std::string, std::shared_ptr<const sf::Font>>
      &m_fonts_map;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for IUIStyleDataProvider taking in font map
  ///
  /// @param fonts_map Font map to use for loading styles
  /////////////////////////////////////////////////
  IUIStyleDataProvider(
      const std::unordered_map<std::string, std::shared_ptr<const sf::Font>>
          &fonts_map)
      : m_fonts_map(fonts_map) {};

  ////////////////////////////////////////////////////////////
  /// @brief Virtual destructor
  ////////////////////////////////////////////////////////////
  virtual ~IUIStyleDataProvider() = default;

  ////////////////////////////////////////////////////////////
  /// @brief Load and configure multiple UI styles by name
  ///
<<<<<<< HEAD
  /// @param font_provider Reference to IFontProvider for loading fonts
  /// @param style_names Vector of style names to load
  /// @return Map of style name to configured UIStyle, or error
  ////////////////////////////////////////////////////////////
  virtual std::expected<std::unordered_map<std::string, UIStyle>, FailInfo>
  ProvideUIStylesMap(const IFontProvider &font_provider,
                     const std::vector<std::string> &style_names) = 0;
=======
  /// @return Vector of style data wrappers, or error
  ////////////////////////////////////////////////////////////
  virtual std::expected<std::vector<UIStyle>, FailInfo> ProvideUIStyles() = 0;
>>>>>>> a7787f7 (builds successfully)
};

} // namespace steamrot
