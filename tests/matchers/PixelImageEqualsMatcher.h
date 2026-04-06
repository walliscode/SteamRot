/////////////////////////////////////////////////
/// @file
/// @brief Catch2 custom matcher for pixel-level sf::Image comparison.
///
/// Compares two sf::Image objects pixel by pixel and reports a human-readable
/// failure message including the first differing pixel coordinates and RGBA
/// values.
///
/// Optionally writes a diff-highlighted PNG to a diagnostics directory when a
/// mismatch is detected.
///
/// Usage:
/// @code
///   sf::Image actual   = steamrot::tests::CaptureImage(render_texture);
///   sf::Image expected = *steamrot::tests::LoadBaseline("button_default");
///   REQUIRE_THAT(actual, EqualsImage(expected));
/// @endcode
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <filesystem>
#include <optional>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class PixelImageEqualsMatcher
/// @brief Catch2 matcher that compares sf::Image objects pixel by pixel.
/////////////////////////////////////////////////
class PixelImageEqualsMatcher
    : public Catch::Matchers::MatcherBase<sf::Image> {

private:
  /////////////////////////////////////////////////
  /// @brief The expected / baseline image.
  /////////////////////////////////////////////////
  const sf::Image &m_expected;

  /////////////////////////////////////////////////
  /// @brief Per-channel tolerance (0 = exact match).
  /////////////////////////////////////////////////
  uint8_t m_tolerance;

  /////////////////////////////////////////////////
  /// @brief Optional directory to save diff PNGs on failure.
  /////////////////////////////////////////////////
  std::optional<std::filesystem::path> m_diff_output_dir;

  /////////////////////////////////////////////////
  /// @brief Name used for the diff PNG filename.
  /////////////////////////////////////////////////
  std::string m_test_name;

  /////////////////////////////////////////////////
  /// @brief Description string populated on mismatch.
  /////////////////////////////////////////////////
  mutable std::string m_mismatch_description;

public:
  /////////////////////////////////////////////////
  /// @brief Construct with an expected image.
  ///
  /// @param expected     Reference baseline image.
  /// @param tolerance    Per-channel tolerance (default 0 = exact).
  /// @param test_name    Optional name for diff file naming.
  /// @param diff_output  Optional directory to write diff PNGs on failure.
  /////////////////////////////////////////////////
  explicit PixelImageEqualsMatcher(
      const sf::Image &expected, uint8_t tolerance = 0,
      std::string test_name = "",
      std::optional<std::filesystem::path> diff_output = std::nullopt);

  /////////////////////////////////////////////////
  /// @brief Perform the pixel comparison.
  ///
  /// @param actual The image produced by the code under test.
  /// @return true if the images are equal within tolerance.
  /////////////////////////////////////////////////
  bool match(const sf::Image &actual) const override;

  /////////////////////////////////////////////////
  /// @brief Return a human-readable description of the expected value.
  /////////////////////////////////////////////////
  std::string describe() const override;
};

/////////////////////////////////////////////////
/// @brief Convenience factory function for PixelImageEqualsMatcher.
///
/// @param expected    Baseline image to compare against.
/// @param tolerance   Per-channel tolerance (default 0 = exact).
/// @param test_name   Optional name for diff file naming.
/// @param diff_output Optional directory for diff PNG output on failure.
/////////////////////////////////////////////////
inline PixelImageEqualsMatcher
EqualsImage(const sf::Image &expected, uint8_t tolerance = 0,
            const std::string &test_name = "",
            std::optional<std::filesystem::path> diff_output = std::nullopt) {
  return PixelImageEqualsMatcher(expected, tolerance, test_name, diff_output);
}

} // namespace steamrot::tests
