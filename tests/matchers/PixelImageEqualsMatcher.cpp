/////////////////////////////////////////////////
/// @file
/// @brief Implementation of PixelImageEqualsMatcher.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "PixelImageEqualsMatcher.h"
#include <filesystem>
#include <format>
#include <sstream>

namespace steamrot::tests {

namespace {

/////////////////////////////////////////////////
/// @brief Per-channel absolute difference.
/////////////////////////////////////////////////
uint8_t ChannelDiff(uint8_t lhs, uint8_t rhs) {
  return static_cast<uint8_t>(lhs > rhs ? lhs - rhs : rhs - lhs);
}

/////////////////////////////////////////////////
/// @brief Write a diff-highlighted PNG to @p path.
///
/// Differing pixels are highlighted in magenta; matching pixels are reproduced
/// at half brightness.
///
/// @param actual    Actual image.
/// @param expected  Expected / baseline image.
/// @param path      Destination file path.
/// @param tolerance Per-channel tolerance used when highlighting.
/////////////////////////////////////////////////
void WriteDiffPng(const sf::Image &actual, const sf::Image &expected,
                  const std::filesystem::path &path, uint8_t tolerance) {
  const sf::Vector2u size = actual.getSize();
  sf::Image diff_image({size.x, size.y});

  for (unsigned int y = 0; y < size.y; ++y) {
    for (unsigned int x = 0; x < size.x; ++x) {
      sf::Color a = actual.getPixel({x, y});
      sf::Color e =
          (x < expected.getSize().x && y < expected.getSize().y)
              ? expected.getPixel({x, y})
              : sf::Color::Transparent;

      bool differs = ChannelDiff(a.r, e.r) > tolerance ||
                     ChannelDiff(a.g, e.g) > tolerance ||
                     ChannelDiff(a.b, e.b) > tolerance ||
                     ChannelDiff(a.a, e.a) > tolerance;

      if (differs) {
        diff_image.setPixel({x, y}, sf::Color::Magenta);
      } else {
        diff_image.setPixel(
            {x, y},
            sf::Color{static_cast<uint8_t>(a.r / 2u),
                      static_cast<uint8_t>(a.g / 2u),
                      static_cast<uint8_t>(a.b / 2u), a.a});
      }
    }
  }

  std::filesystem::create_directories(path.parent_path());
  diff_image.saveToFile(path.string());
}

} // anonymous namespace

/////////////////////////////////////////////////
PixelImageEqualsMatcher::PixelImageEqualsMatcher(
    const sf::Image &expected, uint8_t tolerance, std::string test_name,
    std::optional<std::filesystem::path> diff_output)
    : m_expected(expected), m_tolerance(tolerance),
      m_test_name(std::move(test_name)),
      m_diff_output_dir(std::move(diff_output)) {}

/////////////////////////////////////////////////
bool PixelImageEqualsMatcher::match(const sf::Image &actual) const {
  const sf::Vector2u actual_size = actual.getSize();
  const sf::Vector2u expected_size = m_expected.getSize();

  if (actual_size != expected_size) {
    m_mismatch_description = std::format(
        "image size mismatch: actual ({}x{}) != expected ({}x{})",
        actual_size.x, actual_size.y, expected_size.x, expected_size.y);
    return false;
  }

  const size_t total_pixels =
      static_cast<size_t>(actual_size.x) * static_cast<size_t>(actual_size.y);
  size_t differing = 0;
  bool found_first = false;
  sf::Vector2u first_pos;
  sf::Color first_actual;
  sf::Color first_expected;

  for (unsigned int y = 0; y < actual_size.y; ++y) {
    for (unsigned int x = 0; x < actual_size.x; ++x) {
      sf::Color a = actual.getPixel({x, y});
      sf::Color e = m_expected.getPixel({x, y});

      bool differs = ChannelDiff(a.r, e.r) > m_tolerance ||
                     ChannelDiff(a.g, e.g) > m_tolerance ||
                     ChannelDiff(a.b, e.b) > m_tolerance ||
                     ChannelDiff(a.a, e.a) > m_tolerance;

      if (differs) {
        ++differing;
        if (!found_first) {
          found_first = true;
          first_pos = {x, y};
          first_actual = a;
          first_expected = e;
        }
      }
    }
  }

  if (differing == 0) {
    return true;
  }

  double pct =
      (static_cast<double>(differing) / static_cast<double>(total_pixels)) *
      100.0;

  std::ostringstream oss;
  oss << std::format("{}/{} pixels differ ({:.3f}%)", differing, total_pixels,
                     pct)
      << std::format("; first diff at ({},{})", first_pos.x, first_pos.y)
      << std::format(" actual=rgba({},{},{},{})", static_cast<int>(first_actual.r),
                     static_cast<int>(first_actual.g),
                     static_cast<int>(first_actual.b),
                     static_cast<int>(first_actual.a))
      << std::format(" expected=rgba({},{},{},{})",
                     static_cast<int>(first_expected.r),
                     static_cast<int>(first_expected.g),
                     static_cast<int>(first_expected.b),
                     static_cast<int>(first_expected.a));

  // optionally write diff PNG for diagnostics
  if (m_diff_output_dir.has_value()) {
    std::string diff_name =
        m_test_name.empty() ? "diff" : (m_test_name + "_diff");
    std::filesystem::path diff_path =
        m_diff_output_dir.value() / (diff_name + ".png");
    WriteDiffPng(actual, m_expected, diff_path, m_tolerance);
    oss << "; diff PNG written to '" << diff_path.string() << "'";
  }

  m_mismatch_description = oss.str();
  return false;
}

/////////////////////////////////////////////////
std::string PixelImageEqualsMatcher::describe() const {
  if (!m_mismatch_description.empty()) {
    return "pixel images to be equal — " + m_mismatch_description;
  }
  if (m_tolerance == 0) {
    return "pixel images to be exactly equal";
  }
  return std::format("pixel images to be equal within per-channel tolerance {}",
                     static_cast<int>(m_tolerance));
}

} // namespace steamrot::tests
