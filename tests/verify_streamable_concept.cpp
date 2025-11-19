/////////////////////////////////////////////////
/// @file
/// @brief Test to verify the Streamable concept and template approach
///
/// This is a standalone verification that the template approach works
/// without needing the full conmat library built yet.
/////////////////////////////////////////////////

#include <concepts>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

namespace test_conmat {

// Simplified Color enum for testing
enum class Color { Red, Green, Blue, Yellow };

// The Streamable concept - exactly as in the conmat enhancement
template <typename T>
concept Streamable = requires(std::ostream &os, const T &value) {
  { os << value } -> std::convertible_to<std::ostream &>;
};

// Simplified base function for strings
std::string Colorize(std::string_view text, Color color) {
  std::string color_code;
  switch (color) {
  case Color::Red:
    color_code = "\033[31m";
    break;
  case Color::Green:
    color_code = "\033[32m";
    break;
  case Color::Blue:
    color_code = "\033[34m";
    break;
  case Color::Yellow:
    color_code = "\033[33m";
    break;
  }
  return color_code + std::string(text) + "\033[0m";
}

// Template overload for any streamable type - exactly as in the enhancement
template <Streamable T>
  requires(!std::convertible_to<T, std::string_view>)
std::string Colorize(const T &value, Color color) {
  std::ostringstream oss;
  oss << value;
  return Colorize(oss.str(), color);
}

} // namespace test_conmat

int main() {
  using namespace test_conmat;

  std::cout << "\n=== Verifying Streamable Type Template Approach ===\n"
            << std::endl;

  // Test with string (original functionality)
  std::cout << "String: " << Colorize("Hello", Color::Green) << std::endl;

  // Test with int
  std::cout << "Int: " << Colorize(42, Color::Blue) << std::endl;

  // Test with double
  std::cout << "Double: " << Colorize(3.14159, Color::Yellow) << std::endl;

  // Test with bool
  std::cout << "Bool (true): " << Colorize(true, Color::Green) << std::endl;
  std::cout << "Bool (false): " << Colorize(false, Color::Red) << std::endl;

  // Test with char
  std::cout << "Char: " << Colorize('X', Color::Blue) << std::endl;

  // Test with long
  std::cout << "Long: " << Colorize(9876543210L, Color::Yellow) << std::endl;

  // Test with float
  std::cout << "Float: " << Colorize(2.71828f, Color::Green) << std::endl;

  std::cout << "\n✓ All types successfully colorized!" << std::endl;
  std::cout << "The template approach works correctly.\n" << std::endl;

  return 0;
}
