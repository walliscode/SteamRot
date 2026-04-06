/////////////////////////////////////////////////
/// @file
/// @brief Utilities for capturing and comparing pixel data from SFML
/// render textures.
///
/// Provides helpers for:
/// - Extracting sf::Image snapshots from sf::RenderTexture
/// - Saving images to PNG files for baseline generation
/// - Loading baseline images from PNG files
/// - Pixel-level comparison with optional tolerance
///
/// Baseline generation workflow:
/// Set the environment variable STEAMROT_RECORD_BASELINES=1 before
/// running tests to write new baseline PNGs instead of comparing.
///
/// Usage:
/// @code
///   sf::RenderTexture rt;
///   // ... render something ...
///   rt.display();
///   sf::Image captured = steamrot::tests::CaptureImage(rt);
///
///   // In record mode: save to baseline
///   steamrot::tests::SaveBaseline(captured, "button_default");
///
///   // In compare mode: load and compare
///   auto baseline = steamrot::tests::LoadBaseline("button_default");
///   REQUIRE_THAT(captured, EqualsImage(*baseline));
/// @endcode
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include <SFML/Graphics.hpp>
#include <expected>
#include <filesystem>
#include <optional>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Default directory for baseline PNG images.
///
/// Relative to the project source root: tests/data/ui_baselines/
/////////////////////////////////////////////////
std::filesystem::path GetBaselineDirectory();

/////////////////////////////////////////////////
/// @brief Capture an sf::Image snapshot from a render texture.
///
/// Calls `render_texture.display()` before capturing if the texture was drawn to.
///
/// @param render_texture The render texture to capture.
/// @return Pixel snapshot as an sf::Image.
/////////////////////////////////////////////////
sf::Image CaptureImage(const sf::RenderTexture &render_texture);

/////////////////////////////////////////////////
/// @brief Save an sf::Image as a PNG file to the baseline directory.
///
/// The file will be saved at:
///   GetBaselineDirectory() / (name + ".png")
///
/// @param image  Image to save.
/// @param name   Baseline name without extension (e.g. "button_default").
/// @return std::monostate on success, FailInfo on failure.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SaveBaseline(const sf::Image &image, const std::string &name);

/////////////////////////////////////////////////
/// @brief Save an sf::Image as a PNG file to a custom path.
///
/// @param image Image to save.
/// @param path  Full path including filename and extension.
/// @return std::monostate on success, FailInfo on failure.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SaveImage(const sf::Image &image, const std::filesystem::path &path);

/////////////////////////////////////////////////
/// @brief Load a baseline PNG from the baseline directory.
///
/// The file is expected at:
///   GetBaselineDirectory() / (name + ".png")
///
/// @param name Baseline name without extension (e.g. "button_default").
/// @return The loaded sf::Image on success, FailInfo if the file does not
/// exist or cannot be loaded.
/////////////////////////////////////////////////
std::expected<sf::Image, FailInfo>
LoadBaseline(const std::string &name);

/////////////////////////////////////////////////
/// @brief Check whether the test is running in baseline-record mode.
///
/// Record mode is enabled when the environment variable
/// STEAMROT_RECORD_BASELINES is set to "1".
///
/// In record mode, tests should call SaveBaseline() instead of comparing.
///
/// @return true if STEAMROT_RECORD_BASELINES == "1".
/////////////////////////////////////////////////
bool IsRecordMode();

/////////////////////////////////////////////////
/// @brief Pixel comparison result holding mismatch details.
/////////////////////////////////////////////////
struct PixelDiff {
  /////////////////////////////////////////////////
  /// @brief Whether the images are considered equal.
  /////////////////////////////////////////////////
  bool equal{true};

  /////////////////////////////////////////////////
  /// @brief Number of differing pixels.
  /////////////////////////////////////////////////
  size_t differing_pixel_count{0};

  /////////////////////////////////////////////////
  /// @brief Total number of pixels compared.
  /////////////////////////////////////////////////
  size_t total_pixels{0};

  /////////////////////////////////////////////////
  /// @brief Coordinates of the first differing pixel.
  /////////////////////////////////////////////////
  sf::Vector2u first_diff_position{0u, 0u};

  /////////////////////////////////////////////////
  /// @brief Actual color of the first differing pixel.
  /////////////////////////////////////////////////
  sf::Color first_diff_actual{sf::Color::Transparent};

  /////////////////////////////////////////////////
  /// @brief Expected color of the first differing pixel.
  /////////////////////////////////////////////////
  sf::Color first_diff_expected{sf::Color::Transparent};
};

/////////////////////////////////////////////////
/// @brief Compare two sf::Images pixel by pixel.
///
/// Images of different sizes are always considered unequal (the PixelDiff
/// will report equal=false with differing_pixel_count set to total_pixels
/// of the actual image).
///
/// @param actual    Image produced by the code under test.
/// @param expected  Reference / baseline image.
/// @param tolerance Maximum allowed per-channel difference (0 = exact).
/// @return PixelDiff describing the comparison outcome.
/////////////////////////////////////////////////
PixelDiff CompareImages(const sf::Image &actual, const sf::Image &expected,
                        uint8_t tolerance = 0);

/////////////////////////////////////////////////
/// @brief Write a diff-highlighted PNG to help diagnose pixel failures.
///
/// Pixels that differ beyond tolerance are highlighted in magenta
/// (sf::Color::Magenta); matching pixels are reproduced from the actual
/// image at half brightness.
///
/// @param actual    Actual image.
/// @param expected  Expected / baseline image.
/// @param path      Destination path for the diff PNG.
/// @param tolerance Per-channel tolerance used when highlighting.
/// @return std::monostate on success, FailInfo on failure.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SaveDiffImage(const sf::Image &actual, const sf::Image &expected,
              const std::filesystem::path &path, uint8_t tolerance = 0);

} // namespace steamrot::tests
