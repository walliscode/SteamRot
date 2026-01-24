/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the StylesConfigurator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "FailInfo.h"
#include "FlatbuffersDataLoader.h"
#include "IUIStyleDataProvider.h"
#include "UIStyle.h"
#include <expected>
#include <vector>
namespace steamrot {

/////////////////////////////////////////////////
/// @class FlatbuffersUIStyleDataProvider
/// @brief FlatBuffers implementation of IUIStyleDataProvider
///
/// Loads UI style data from FlatBuffers binary files and configures
/// native C++ style structs.
/////////////////////////////////////////////////
class FlatbuffersUIStyleDataProvider : public IUIStyleDataProvider {

private:
  /////////////////////////////////////////////////
  /// @brief Instance of FlatbuffersDataLoader for loading data.
  /////////////////////////////////////////////////
  FlatbuffersDataLoader m_loader;

public:
  FlatbuffersUIStyleDataProvider(
      std::unordered_map<std::string, std::shared_ptr<const sf::Font>>
          &fonts_map);

  /////////////////////////////////////////////////
  /// @brief Create and provide configured UI styles
  ///
  /// @return Vector of style data wrappers, or error
  /////////////////////////////////////////////////
  std::expected<std::vector<UIStyle>, FailInfo> CreateUIStyles() override;

  /////////////////////////////////////////////////
  /// @brief Configure the provided UI styles vector
  ///
  /// @param ui_styles Vector of UIStyle objects to configure
  /// @return monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureUIStyles(std::vector<UIStyle> &ui_styles) override;

  /////////////////////////////////////////////////
  /// @brief Load and configure multiple UI styles by name (deprecated - use
  /// CreateUIStyles)
  ///
  /// @return Vector of style data wrappers, or error
  /////////////////////////////////////////////////
  std::expected<std::vector<UIStyle>, FailInfo> ProvideUIStyles() override;
};
} // namespace steamrot
