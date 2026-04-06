/////////////////////////////////////////////////
/// @file
/// @brief Implementation of pixel capture and comparison utilities.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "pixel_capture.h"
#include "paths.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
std::filesystem::path GetBaselineDirectory() {
  return steamrot::paths::GetSourceDirectory() / "tests" / "data" /
         "ui_baselines";
}

/////////////////////////////////////////////////
sf::Image CaptureImage(const sf::RenderTexture &render_texture) {
  return render_texture.getTexture().copyToImage();
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SaveBaseline(const sf::Image &image, const std::string &name) {
  std::filesystem::path path = GetBaselineDirectory() / (name + ".png");
  return SaveImage(image, path);
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SaveImage(const sf::Image &image, const std::filesystem::path &path) {
  // ensure the directory exists
  std::error_code ec;
  std::filesystem::create_directories(path.parent_path(), ec);
  if (ec) {
    return std::unexpected(
        FailInfo{FailMode::FileNotFound,
                 std::format("pixel_capture::SaveImage: could not create "
                             "directory '{}': {}",
                             path.parent_path().string(), ec.message())});
  }

  if (!image.saveToFile(path.string())) {
    return std::unexpected(
        FailInfo{FailMode::FileNotFound,
                 std::format("pixel_capture::SaveImage: failed to save image "
                             "to '{}'",
                             path.string())});
  }
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<sf::Image, FailInfo>
LoadBaseline(const std::string &name) {
  std::filesystem::path path = GetBaselineDirectory() / (name + ".png");

  if (!std::filesystem::exists(path)) {
    return std::unexpected(
        FailInfo{FailMode::FileNotFound,
                 std::format("pixel_capture::LoadBaseline: baseline '{}' not "
                             "found at '{}'",
                             name, path.string())});
  }

  sf::Image image;
  if (!image.loadFromFile(path.string())) {
    return std::unexpected(
        FailInfo{FailMode::FileNotFound,
                 std::format("pixel_capture::LoadBaseline: failed to load "
                             "baseline '{}' from '{}'",
                             name, path.string())});
  }
  return image;
}

/////////////////////////////////////////////////
bool IsRecordMode() {
  const char *env = std::getenv("STEAMROT_RECORD_BASELINES");
  return env != nullptr && std::string(env) == "1";
}

/////////////////////////////////////////////////
PixelDiff CompareImages(const sf::Image &actual, const sf::Image &expected,
                        uint8_t tolerance) {
  PixelDiff result;

  const sf::Vector2u actual_size = actual.getSize();
  const sf::Vector2u expected_size = expected.getSize();

  result.total_pixels =
      static_cast<size_t>(actual_size.x) * static_cast<size_t>(actual_size.y);

  // different sizes → all pixels differ
  if (actual_size != expected_size) {
    result.equal = false;
    result.differing_pixel_count = result.total_pixels;
    return result;
  }

  bool found_first_diff = false;

  for (unsigned int y = 0; y < actual_size.y; ++y) {
    for (unsigned int x = 0; x < actual_size.x; ++x) {
      sf::Color a = actual.getPixel({x, y});
      sf::Color e = expected.getPixel({x, y});

      auto diff_channel = [](uint8_t lhs, uint8_t rhs) -> uint8_t {
        return static_cast<uint8_t>(
            lhs > rhs ? lhs - rhs : rhs - lhs);
      };

      bool pixel_differs =
          diff_channel(a.r, e.r) > tolerance ||
          diff_channel(a.g, e.g) > tolerance ||
          diff_channel(a.b, e.b) > tolerance ||
          diff_channel(a.a, e.a) > tolerance;

      if (pixel_differs) {
        ++result.differing_pixel_count;
        if (!found_first_diff) {
          found_first_diff = true;
          result.first_diff_position = {x, y};
          result.first_diff_actual = a;
          result.first_diff_expected = e;
        }
      }
    }
  }

  result.equal = (result.differing_pixel_count == 0);
  return result;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SaveDiffImage(const sf::Image &actual, const sf::Image &expected,
              const std::filesystem::path &path, uint8_t tolerance) {
  const sf::Vector2u actual_size = actual.getSize();

  sf::Image diff_image({actual_size.x, actual_size.y});

  for (unsigned int y = 0; y < actual_size.y; ++y) {
    for (unsigned int x = 0; x < actual_size.x; ++x) {
      // default to transparent if sizes differ for expected
      sf::Color a = actual.getPixel({x, y});
      sf::Color e =
          (x < expected.getSize().x && y < expected.getSize().y)
              ? expected.getPixel({x, y})
              : sf::Color::Transparent;

      auto diff_channel = [](uint8_t lhs, uint8_t rhs) -> uint8_t {
        return static_cast<uint8_t>(lhs > rhs ? lhs - rhs : rhs - lhs);
      };

      bool pixel_differs =
          diff_channel(a.r, e.r) > tolerance ||
          diff_channel(a.g, e.g) > tolerance ||
          diff_channel(a.b, e.b) > tolerance ||
          diff_channel(a.a, e.a) > tolerance;

      if (pixel_differs) {
        // highlight differing pixels in magenta
        diff_image.setPixel({x, y}, sf::Color::Magenta);
      } else {
        // show matching pixels at half brightness
        diff_image.setPixel(
            {x, y},
            sf::Color{static_cast<uint8_t>(a.r / 2),
                      static_cast<uint8_t>(a.g / 2),
                      static_cast<uint8_t>(a.b / 2), a.a});
      }
    }
  }

  return SaveImage(diff_image, path);
}

} // namespace steamrot::tests
