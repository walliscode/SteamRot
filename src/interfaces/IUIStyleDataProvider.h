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
  /// @brief Load and configure multiple UI styles by name from files
  ///
  /// @return Vector of style data wrappers, or error
  ////////////////////////////////////////////////////////////
  virtual std::expected<std::vector<UIStyle>, FailInfo> ProvideUIStyles() = 0;

  ////////////////////////////////////////////////////////////
  /// @brief Convert FlatBuffers UIStyleData to native UIStyle objects.
  ///
  /// This method allows passing pre-loaded FlatBuffers style data directly
  /// for conversion without file I/O.
  ///
  /// @param fb_styles Vector of pointers to FlatBuffers UIStyleData
  /// @return Vector of converted UIStyle objects, or error
  ////////////////////////////////////////////////////////////
  virtual std::expected<std::vector<UIStyle>, FailInfo>
  ConvertUIStyles(const std::vector<const UIStyleData *> &fb_styles) = 0;
};

} // namespace steamrot
