/////////////////////////////////////////////////
/// @file
/// @brief Indentation helpers for formatted test output
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <sstream>
#include <string>

namespace steamrot::tests::formatting {

/////////////////////////////////////////////////
/// @brief Generate indentation string for given level
///
/// Creates a string of spaces for the specified indentation level.
/// Each level adds spaces_per_level spaces to the output.
///
/// @param level Indentation level (0 = no indent, 1 = one level, etc.)
/// @param spaces_per_level Number of spaces per indentation level (default: 2)
/// @return String containing appropriate number of spaces
///
/// @example
/// std::string indent = Indent(0);  // "" (no spaces)
/// std::string indent = Indent(1);  // "  " (2 spaces)
/// std::string indent = Indent(2);  // "    " (4 spaces)
/////////////////////////////////////////////////
inline std::string Indent(size_t level, size_t spaces_per_level = 2) {
  return std::string(level * spaces_per_level, ' ');
}

/////////////////////////////////////////////////
/// @brief Format text with indentation
///
/// Prepends the appropriate indentation to the given text based on
/// the indentation level.
///
/// @param text Text to indent
/// @param level Indentation level
/// @param spaces_per_level Number of spaces per indentation level (default: 2)
/// @return Indented text
///
/// @example
/// std::string line = IndentedLine("Root level", 0);      // "Root level"
/// std::string line = IndentedLine("First level", 1);     // "  First level"
/// std::string line = IndentedLine("Second level", 2);    // "    Second level"
/////////////////////////////////////////////////
inline std::string IndentedLine(const std::string &text, size_t level,
                                size_t spaces_per_level = 2) {
  return Indent(level, spaces_per_level) + text;
}

/////////////////////////////////////////////////
/// @brief Format key-value pair with indentation
///
/// Creates a formatted "key: value" string with appropriate indentation.
/// The value is aligned after the colon with extra spacing for readability.
/// Boolean values are formatted as "true"/"false" rather than 1/0.
///
/// @param key Key name
/// @param value Value (will be converted to string via ostringstream)
/// @param level Indentation level
/// @param spaces_per_level Number of spaces per indentation level (default: 2)
/// @return Formatted "key:   value" with indentation
///
/// @example
/// std::string kv = IndentedKeyValue("actual", false, 2);
/// // Output: "    actual:   false"
/// std::string kv = IndentedKeyValue("expected", true, 2);
/// // Output: "    expected: true"
/////////////////////////////////////////////////
template <typename T>
std::string IndentedKeyValue(const std::string &key, const T &value,
                              size_t level, size_t spaces_per_level = 2) {
  std::ostringstream oss;
  oss << std::boolalpha;  // Format booleans as true/false
  oss << Indent(level, spaces_per_level) << key << ":   " << value;
  return oss.str();
}

} // namespace steamrot::tests::formatting
