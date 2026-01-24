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
#include <vector>

namespace steamrot {

// Forward declaration
class IFontProvider;

////////////////////////////////////////////////////////////
/// @class IUIStyleDataProvider
/// @brief Interface for loading and configuring UI style data from various
/// sources
///
/// Providers load raw style data and configure UIStyle objects using the
/// provided IFontProvider for font resources.
////////////////////////////////////////////////////////////
class IUIStyleDataProvider {
protected:
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> &fonts_map;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for IUIStyleDataProvider taking in font map
  ///
  /// @param fonts_map Font map to use for loading styles
  /////////////////////////////////////////////////
  IUIStyleDataProvider(
      std::unordered_map<std::string, std::shared_ptr<const sf::Font>>
          &fonts_map)
      : fonts_map(fonts_map) {};

  ////////////////////////////////////////////////////////////
  /// @brief Virtual destructor
  ////////////////////////////////////////////////////////////
  virtual ~IUIStyleDataProvider() = default;

  ////////////////////////////////////////////////////////////
  /// @brief Create and provide configured UI styles
  ///
  /// @return Vector of style data wrappers, or error
  ////////////////////////////////////////////////////////////
  virtual std::expected<std::vector<UIStyle>, FailInfo> CreateUIStyles() = 0;

  ////////////////////////////////////////////////////////////
  /// @brief Configure the provided UI styles vector
  ///
  /// @param ui_styles Vector of UIStyle objects to configure
  /// @return monostate on success, FailInfo on error
  ////////////////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureUIStyles(std::vector<UIStyle> &ui_styles) = 0;

  ////////////////////////////////////////////////////////////
  /// @brief Load and configure multiple UI styles by name (deprecated - use
  /// CreateUIStyles)
  ///
  /// @return Vector of style data wrappers, or error
  ////////////////////////////////////////////////////////////
  virtual std::expected<std::vector<UIStyle>, FailInfo> ProvideUIStyles() = 0;
};

} // namespace steamrot
