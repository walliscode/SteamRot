/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for getting fonts for testing
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "test_fonts.h"
#include "paths.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
std::expected<sf::Font, steamrot::FailInfo> ProvideDefaultFont() {
  sf::Font font;
  std::filesystem::path font_path =
      steamrot::paths::GetDataDirectory() /
      std::filesystem::path{"assets/fonts/Roboto-Regular.ttf"};
  if (!font.openFromFile(font_path.string())) {
    return std::unexpected(
        steamrot::FailInfo{steamrot::FailMode::FileNotFound,
                           "Failed to load font from " + font_path.string()});
  }
  return font;
}
} // namespace steamrot::tests
