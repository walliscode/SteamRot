#include "conmat.h"
#include <iostream>
#include <cassert>
#include <string>

void test_color_formatting() {
  using namespace conmat;
  
  std::string red = Colorize("test", Color::Red);
  assert(red.find("\033[31m") != std::string::npos);
  assert(red.find("test") != std::string::npos);
  assert(red.find("\033[0m") != std::string::npos);
  
  std::cout << "✓ Color formatting test passed" << std::endl;
}

void test_style_formatting() {
  using namespace conmat;
  
  std::string bold = Stylize("test", Style::Bold);
  assert(bold.find("\033[1m") != std::string::npos);
  assert(bold.find("test") != std::string::npos);
  
  std::cout << "✓ Style formatting test passed" << std::endl;
}

void test_combined_formatting() {
  using namespace conmat;
  
  FormatOptions opts(Color::Green, Style::Bold);
  std::string result = Format("test", opts);
  assert(result.find("\033[1m") != std::string::npos);
  assert(result.find("\033[32m") != std::string::npos);
  assert(result.find("test") != std::string::npos);
  
  std::cout << "✓ Combined formatting test passed" << std::endl;
}

void test_divider() {
  using namespace conmat;
  
  std::string div = Divider("=", 10);
  assert(div == "==========");
  
  std::string div2 = Divider("-", 5);
  assert(div2 == "-----");
  
  std::string div3 = Divider("abc", 10);
  assert(div3 == "abcabcabca");
  
  std::cout << "✓ Divider test passed" << std::endl;
}

void test_sanitize() {
  using namespace conmat;
  
  // Test that control characters are removed
  std::string input = "hello\x1b[31mworld";
  std::string output = Sanitize(input);
  assert(output.find('\x1b') == std::string::npos);
  assert(output.find("hello") != std::string::npos);
  assert(output.find("world") != std::string::npos);
  
  // Test that normal text is preserved
  std::string normal = "normal text";
  assert(Sanitize(normal) == normal);
  
  // Test that whitespace is preserved
  std::string with_ws = "hello\nworld\ttab";
  std::string sanitized_ws = Sanitize(with_ws);
  assert(sanitized_ws.find('\n') != std::string::npos);
  assert(sanitized_ws.find('\t') != std::string::npos);
  
  std::cout << "✓ Sanitize test passed" << std::endl;
}

void test_strip_ansi() {
  using namespace conmat;
  
  std::string with_ansi = "\033[31mRed Text\033[0m";
  std::string stripped = StripAnsi(with_ansi);
  assert(stripped == "Red Text");
  
  std::string no_ansi = "Plain text";
  assert(StripAnsi(no_ansi) == no_ansi);
  
  std::cout << "✓ Strip ANSI test passed" << std::endl;
}

void test_background_color() {
  using namespace conmat;
  
  FormatOptions opts(Color::White, Color::Red);
  std::string result = Format("test", opts);
  assert(result.find("\033[37m") != std::string::npos); // White foreground
  assert(result.find("\033[41m") != std::string::npos); // Red background
  
  std::cout << "✓ Background color test passed" << std::endl;
}

void test_no_reset() {
  using namespace conmat;
  
  FormatOptions opts;
  opts.foreground = Color::Red;
  opts.reset_after = false;
  std::string result = Format("test", opts);
  assert(result.find("\033[0m") == std::string::npos);
  assert(result.find("\033[31m") != std::string::npos);
  
  std::cout << "✓ No reset test passed" << std::endl;
}

void test_empty_divider() {
  using namespace conmat;
  
  std::string empty1 = Divider("", 10);
  assert(empty1.empty());
  
  std::string empty2 = Divider("=", 0);
  assert(empty2.empty());
  
  std::cout << "✓ Empty divider test passed" << std::endl;
}

void test_default_divider() {
  using namespace conmat;
  
  // Test that default divider has correct length (80)
  // The symbol is configurable via CMake, so we don't check for a specific symbol
  std::string div = Divider();
  assert(div.length() == 80);
  
  // Test with a specific width
  std::string div_40 = Divider(40);
  assert(div_40.length() == 40);
  
  std::cout << "✓ Default divider test passed" << std::endl;
}

void test_divider_runtime_override() {
  using namespace conmat;
  
  // Test that explicit symbol overrides the default
  std::string custom = Divider("-", 10);
  assert(custom == "----------");
  
  // Test with different symbols
  std::string stars = Divider("*", 5);
  assert(stars == "*****");
  
  // Test that parameterless call uses default (configured via CMake)
  std::string default_div = Divider(10);
  assert(default_div.length() == 10);
  
  std::cout << "✓ Divider runtime override test passed" << std::endl;
}

void test_streamable_types() {
  using namespace conmat;
  
  // Test with int
  std::string int_result = Colorize(42, Color::Green);
  assert(int_result.find("42") != std::string::npos);
  assert(int_result.find("\033[32m") != std::string::npos);
  assert(int_result.find("\033[0m") != std::string::npos);
  
  // Test with bool
  std::string bool_result = Colorize(true, Color::Blue);
  assert(bool_result.find("1") != std::string::npos || bool_result.find("true") != std::string::npos);
  assert(bool_result.find("\033[34m") != std::string::npos);
  
  // Test with double
  std::string double_result = Colorize(3.14159, Color::Yellow);
  assert(double_result.find("3.14159") != std::string::npos);
  assert(double_result.find("\033[33m") != std::string::npos);
  
  // Test with float
  std::string float_result = Stylize(2.71828f, Style::Bold);
  assert(float_result.find("2.71828") != std::string::npos);
  assert(float_result.find("\033[1m") != std::string::npos);
  
  // Test with char
  std::string char_result = Colorize('X', Color::Magenta);
  assert(char_result.find("X") != std::string::npos);
  assert(char_result.find("\033[35m") != std::string::npos);
  
  // Test with long
  std::string long_result = Colorize(1234567890L, Color::Cyan);
  assert(long_result.find("1234567890") != std::string::npos);
  assert(long_result.find("\033[36m") != std::string::npos);
  
  // Test Format with int and FormatOptions
  FormatOptions opts(Color::Red, Style::Bold);
  std::string format_result = Format(999, opts);
  assert(format_result.find("999") != std::string::npos);
  assert(format_result.find("\033[31m") != std::string::npos);
  assert(format_result.find("\033[1m") != std::string::npos);
  
  std::cout << "✓ Streamable types test passed" << std::endl;
}

int main() {
  std::cout << "Running conmat tests..." << std::endl << std::endl;
  
  test_color_formatting();
  test_style_formatting();
  test_combined_formatting();
  test_divider();
  test_sanitize();
  test_strip_ansi();
  test_background_color();
  test_no_reset();
  test_empty_divider();
  test_default_divider();
  test_divider_runtime_override();
  test_streamable_types();
  
  std::cout << std::endl << "All tests passed! ✓" << std::endl;
  
  return 0;
}
