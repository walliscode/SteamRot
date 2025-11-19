#include "conmat.h"
#include "conmat_config.h"
#include <regex>
#include <sstream>

namespace conmat {

namespace {

// ANSI escape codes
constexpr const char *RESET = "\033[0m";

// Get ANSI code for foreground color
std::string get_fg_color_code(Color color) {
  switch (color) {
  case Color::Default:
    return "";
  case Color::Black:
    return "\033[30m";
  case Color::Red:
    return "\033[31m";
  case Color::Green:
    return "\033[32m";
  case Color::Yellow:
    return "\033[33m";
  case Color::Blue:
    return "\033[34m";
  case Color::Magenta:
    return "\033[35m";
  case Color::Cyan:
    return "\033[36m";
  case Color::White:
    return "\033[37m";
  case Color::BrightBlack:
    return "\033[90m";
  case Color::BrightRed:
    return "\033[91m";
  case Color::BrightGreen:
    return "\033[92m";
  case Color::BrightYellow:
    return "\033[93m";
  case Color::BrightBlue:
    return "\033[94m";
  case Color::BrightMagenta:
    return "\033[95m";
  case Color::BrightCyan:
    return "\033[96m";
  case Color::BrightWhite:
    return "\033[97m";
  }
  return "";
}

// Get ANSI code for background color
std::string get_bg_color_code(Color color) {
  switch (color) {
  case Color::Default:
    return "";
  case Color::Black:
    return "\033[40m";
  case Color::Red:
    return "\033[41m";
  case Color::Green:
    return "\033[42m";
  case Color::Yellow:
    return "\033[43m";
  case Color::Blue:
    return "\033[44m";
  case Color::Magenta:
    return "\033[45m";
  case Color::Cyan:
    return "\033[46m";
  case Color::White:
    return "\033[47m";
  case Color::BrightBlack:
    return "\033[100m";
  case Color::BrightRed:
    return "\033[101m";
  case Color::BrightGreen:
    return "\033[102m";
  case Color::BrightYellow:
    return "\033[103m";
  case Color::BrightBlue:
    return "\033[104m";
  case Color::BrightMagenta:
    return "\033[105m";
  case Color::BrightCyan:
    return "\033[106m";
  case Color::BrightWhite:
    return "\033[107m";
  }
  return "";
}

// Get ANSI code for text style
std::string get_style_code(Style style) {
  switch (style) {
  case Style::Default:
    return "";
  case Style::Bold:
    return "\033[1m";
  case Style::Dim:
    return "\033[2m";
  case Style::Italic:
    return "\033[3m";
  case Style::Underline:
    return "\033[4m";
  case Style::Blink:
    return "\033[5m";
  case Style::Reverse:
    return "\033[7m";
  case Style::Hidden:
    return "\033[8m";
  case Style::Strikethrough:
    return "\033[9m";
  }
  return "";
}

} // anonymous namespace

std::string Format(std::string_view text, const FormatOptions &options) {
  // Sanitize input first
  std::string safe_text = Sanitize(text);

  std::ostringstream result;

  // Apply style
  result << get_style_code(options.style);

  // Apply foreground color
  result << get_fg_color_code(options.foreground);

  // Apply background color
  result << get_bg_color_code(options.background);

  // Add the text
  result << safe_text;

  // Reset if requested
  if (options.reset_after) {
    result << RESET;
  }

  return result.str();
}

std::string Colorize(std::string_view text, Color color) {
  FormatOptions options;
  options.foreground = color;
  return Format(text, options);
}

std::string Stylize(std::string_view text, Style style) {
  FormatOptions options;
  options.style = style;
  return Format(text, options);
}

std::string Divider(std::string_view symbol, size_t width,
                    const FormatOptions &options) {
  if (symbol.empty() || width == 0) {
    return "";
  }

  // Sanitize the symbol to prevent injection
  std::string safe_symbol = Sanitize(symbol);

  // Build the divider by repeating the symbol
  std::ostringstream result;
  size_t symbol_length = safe_symbol.length();
  size_t full_repeats = width / symbol_length;
  size_t remaining = width % symbol_length;

  for (size_t i = 0; i < full_repeats; ++i) {
    result << safe_symbol;
  }

  // Add remaining characters if needed
  if (remaining > 0) {
    result << safe_symbol.substr(0, remaining);
  }

  // Apply formatting if any
  if (options.foreground != Color::Default ||
      options.background != Color::Default || options.style != Style::Default) {
    return Format(result.str(), options);
  }

  return result.str();
}

std::string Divider(size_t width, const FormatOptions &options) {
  // Use the compile-time configured default symbol
  return Divider(CONMAT_DEFAULT_DIVIDER_SYMBOL, width, options);
}

std::string Sanitize(std::string_view text) {
  std::string result;
  result.reserve(text.length());

  for (char c : text) {
    // Filter out control characters except common whitespace
    if (c >= 32 && c <= 126) {
      // Printable ASCII
      result += c;
    } else if (c == '\n' || c == '\t' || c == '\r') {
      // Allow common whitespace
      result += c;
    } else if (static_cast<unsigned char>(c) >= 128) {
      // Allow UTF-8 continuation bytes and high characters
      result += c;
    }
    // Skip other control characters (including potential ANSI escape sequences)
  }

  return result;
}

std::string StripAnsi(std::string_view text) {
  // Remove ANSI escape sequences using regex
  // Pattern matches: ESC [ ... m (and other ANSI codes)
  std::string str(text);
  std::regex ansi_regex("\033\\[[0-9;]*[A-Za-z]");
  return std::regex_replace(str, ansi_regex, "");
}

std::string TestInProgress() { return Colorize("[...]", Color::Yellow); }

std::string TestPassed() { return Colorize("[✓]", Color::Green); }

std::string TestFailed() { return Colorize("[✗]", Color::Red); }
} // namespace conmat
