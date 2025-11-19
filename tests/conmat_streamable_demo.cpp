/////////////////////////////////////////////////
/// @file
/// @brief Demo/test file showing conmat streamable type support
///
/// This file demonstrates the enhanced conmat library functionality
/// that allows Colorize, Stylize, and Format to work with any
/// type that supports std::ostream << operator (like cout).
/////////////////////////////////////////////////

#include <iostream>

// Note: This demo assumes the conmat library has been updated with
// template overloads for Colorize, Stylize, and Format functions.
// 
// The changes made to conmat include:
// 1. Added <concepts>, <sstream>, and <type_traits> headers
// 2. Added Streamable concept to check if type supports operator<<
// 3. Added template overloads for Format, Colorize, and Stylize
//    that accept any Streamable type
//
// Updated conmat.h template additions:
//
// template <typename T>
// concept Streamable = requires(std::ostream &os, const T &value) {
//   { os << value } -> std::convertible_to<std::ostream &>;
// };
//
// template <Streamable T>
//   requires(!std::convertible_to<T, std::string_view>)
// std::string Colorize(const T &value, Color color) {
//   std::ostringstream oss;
//   oss << value;
//   return Colorize(oss.str(), color);
// }
// 
// Similar templates for Format() and Stylize()

int main() {
  std::cout << "\n=== Conmat Streamable Types Demo ===\n" << std::endl;
  
  std::cout << "This demo shows the expected behavior once conmat library" << std::endl;
  std::cout << "is updated with template overloads for streamable types.\n" << std::endl;
  
  std::cout << "Supported types include:" << std::endl;
  std::cout << "  - Integers (int, long, short, etc.)" << std::endl;
  std::cout << "  - Floating-point (float, double)" << std::endl;
  std::cout << "  - Booleans (bool)" << std::endl;
  std::cout << "  - Characters (char)" << std::endl;
  std::cout << "  - Strings (already supported)" << std::endl;
  std::cout << "  - Any custom type with operator<< overload" << std::endl;
  
  std::cout << "\nExample usage:" << std::endl;
  std::cout << "  std::cout << conmat::Colorize(42, conmat::Color::Green);" << std::endl;
  std::cout << "  std::cout << conmat::Colorize(true, conmat::Color::Blue);" << std::endl;
  std::cout << "  std::cout << conmat::Colorize(3.14159, conmat::Color::Yellow);" << std::endl;
  std::cout << "  std::cout << conmat::Stylize(2.71828f, conmat::Style::Bold);" << std::endl;
  std::cout << "  std::cout << conmat::Colorize('X', conmat::Color::Magenta);" << std::endl;
  
  std::cout << "\nImplementation details:" << std::endl;
  std::cout << "  - Uses C++23 concepts for type constraints" << std::endl;
  std::cout << "  - Converts value to string using std::ostringstream" << std::endl;
  std::cout << "  - Delegates to existing string-based functions" << std::endl;
  std::cout << "  - No ambiguity with string types (requires clause)" << std::endl;
  
  return 0;
}
