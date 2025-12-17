/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the StylesConfigurator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "FailInfo.h"
#include "IUIStyleDataProvider.h"
#include "UIStyle.h"
#include <expected>
#include <vector>
namespace steamrot {

/////////////////////////////////////////////////
/// @class StylesConfigurator
/// @brief Responsible for providing UI styles configured from external data
///
/////////////////////////////////////////////////
class FlatbuffersUIStyleDataProvider : public IUIStyleDataProvider {

public:
  FlatbuffersUIStyleDataProvider(
      const std::unordered_map<std::string, std::shared_ptr<const sf::Font>>
          &fonts_map);

  std::expected<std::vector<UIStyle>, FailInfo> ProvideUIStyles() override;
};
} // namespace steamrot
