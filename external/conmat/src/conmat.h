#pragma once

#include <concepts>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>

namespace conmat {

////////////////////////////////////////////////////////////
/// \brief ANSI color codes enum
///
////////////////////////////////////////////////////////////
enum class Color {
  Default,
  Black,
  Red,
  Green,
  Yellow,
  Blue,
  Magenta,
  Cyan,
  White,
  BrightBlack,
  BrightRed,
  BrightGreen,
  BrightYellow,
  BrightBlue,
  BrightMagenta,
  BrightCyan,
  BrightWhite
};

////////////////////////////////////////////////////////////
/// \brief ANSI text style codes enum
///
////////////////////////////////////////////////////////////
enum class Style {
  Default,
  Bold,
  Dim,
  Italic,
  Underline,
  Blink,
  Reverse,
  Hidden,
  Strikethrough
};

////////////////////////////////////////////////////////////
/// \brief Format options for text formatting
///
////////////////////////////////////////////////////////////
struct FormatOptions {
  Color foreground = Color::Default;
  Color background = Color::Default;
  Style style = Style::Default;
  bool reset_after = true; // Automatically append reset sequence

  FormatOptions() = default;
  FormatOptions(Color fg) : foreground(fg) {}
  FormatOptions(Color fg, Style s) : foreground(fg), style(s) {}
  FormatOptions(Color fg, Color bg) : foreground(fg), background(bg) {}
  FormatOptions(Color fg, Color bg, Style s)
      : foreground(fg), background(bg), style(s) {}
};

////////////////////////////////////////////////////////////
/// \brief Format a string with ANSI codes
/// \param text The text to format
/// \param options Format options (default: no formatting)
/// \return Formatted string with ANSI codes
///
////////////////////////////////////////////////////////////
std::string Format(std::string_view text, const FormatOptions &options = {});

////////////////////////////////////////////////////////////
/// \brief Format text with just a foreground color
/// \param text The text to format
/// \param color The foreground color
/// \return Formatted string with color
///
////////////////////////////////////////////////////////////
std::string Colorize(std::string_view text, Color color);

////////////////////////////////////////////////////////////
/// \brief Format text with a style
/// \param text The text to format
/// \param style The text style
/// \return Formatted string with style
///
////////////////////////////////////////////////////////////
std::string Stylize(std::string_view text, Style style);

////////////////////////////////////////////////////////////
/// \brief Create a divider line with explicit symbol
/// \param symbol The symbol to use
/// \param width The width of the divider (default: 80)
/// \param options Format options for the divider
/// \return Formatted divider string
///
////////////////////////////////////////////////////////////
std::string Divider(std::string_view symbol, size_t width = 80,
                    const FormatOptions &options = {});

////////////////////////////////////////////////////////////
/// \brief Create a divider line with default symbol from CMake configuration
/// \param width The width of the divider (default: 80)
/// \param options Format options for the divider
/// \return Formatted divider string
///
////////////////////////////////////////////////////////////
std::string Divider(size_t width = 80, const FormatOptions &options = {});

////////////////////////////////////////////////////////////
/// \brief Escape and sanitize input string to prevent injection
/// \param text The text to sanitize
/// \return Sanitized string safe for terminal output
///
////////////////////////////////////////////////////////////
std::string Sanitize(std::string_view text);

////////////////////////////////////////////////////////////
/// \brief Strip ANSI codes from a string
/// \param text The text to strip codes from
/// \return Plain text without ANSI codes
///
////////////////////////////////////////////////////////////
std::string StripAnsi(std::string_view text);

/////////////////////////////////////////////////
/// @brief Return a yellow progress dots
/////////////////////////////////////////////////
std::string TestInProgress();

/////////////////////////////////////////////////
/// @brief Return a green check mark symbol
/////////////////////////////////////////////////
std::string TestPassed();

/////////////////////////////////////////////////
/// @brief Return a red cross mark symbol
/////////////////////////////////////////////////
std::string TestFailed();

////////////////////////////////////////////////////////////
/// \brief Concept to check if a type is streamable to ostream
///
////////////////////////////////////////////////////////////
template <typename T>
concept Streamable = requires(std::ostream &os, const T &value) {
  { os << value } -> std::convertible_to<std::ostream &>;
};

////////////////////////////////////////////////////////////
/// \brief Format any streamable value with ANSI codes
/// \tparam T Any type that can be streamed to std::ostream
/// \param value The value to format
/// \param options Format options (default: no formatting)
/// \return Formatted string with ANSI codes
///
////////////////////////////////////////////////////////////
template <Streamable T>
  requires(!std::convertible_to<T, std::string_view>)
std::string Format(const T &value, const FormatOptions &options = {}) {
  std::ostringstream oss;
  oss << value;
  return Format(oss.str(), options);
}

////////////////////////////////////////////////////////////
/// \brief Format any streamable value with just a foreground color
/// \tparam T Any type that can be streamed to std::ostream
/// \param value The value to format
/// \param color The foreground color
/// \return Formatted string with color
///
////////////////////////////////////////////////////////////
template <Streamable T>
  requires(!std::convertible_to<T, std::string_view>)
std::string Colorize(const T &value, Color color) {
  std::ostringstream oss;
  oss << value;
  return Colorize(oss.str(), color);
}

////////////////////////////////////////////////////////////
/// \brief Format any streamable value with a style
/// \tparam T Any type that can be streamed to std::ostream
/// \param value The value to format
/// \param style The text style
/// \return Formatted string with style
///
////////////////////////////////////////////////////////////
template <Streamable T>
  requires(!std::convertible_to<T, std::string_view>)
std::string Stylize(const T &value, Style style) {
  std::ostringstream oss;
  oss << value;
  return Stylize(oss.str(), style);
}

} // namespace conmat
