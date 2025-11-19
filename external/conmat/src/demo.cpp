#include "conmat.h"
#include <iostream>

int main() {
  using namespace conmat;

  std::cout << "=== Conmat Library Demo ===" << std::endl << std::endl;

  // Test basic colors
  std::cout << "Basic Colors:" << std::endl;
  std::cout << Colorize("Red text", Color::Red) << std::endl;
  std::cout << Colorize("Green text", Color::Green) << std::endl;
  std::cout << Colorize("Blue text", Color::Blue) << std::endl;
  std::cout << Colorize("Yellow text", Color::Yellow) << std::endl;
  std::cout << Colorize("Magenta text", Color::Magenta) << std::endl;
  std::cout << Colorize("Cyan text", Color::Cyan) << std::endl;
  std::cout << std::endl;

  // Test bright colors
  std::cout << "Bright Colors:" << std::endl;
  std::cout << Colorize("Bright Red", Color::BrightRed) << std::endl;
  std::cout << Colorize("Bright Green", Color::BrightGreen) << std::endl;
  std::cout << Colorize("Bright Blue", Color::BrightBlue) << std::endl;
  std::cout << std::endl;

  // Test styles
  std::cout << "Text Styles:" << std::endl;
  std::cout << Stylize("Bold text", Style::Bold) << std::endl;
  std::cout << Stylize("Italic text", Style::Italic) << std::endl;
  std::cout << Stylize("Underline text", Style::Underline) << std::endl;
  std::cout << Stylize("Strikethrough text", Style::Strikethrough) << std::endl;
  std::cout << std::endl;

  // Test combined formatting
  std::cout << "Combined Formatting:" << std::endl;
  FormatOptions bold_red(Color::Red, Style::Bold);
  std::cout << Format("Bold Red Text", bold_red) << std::endl;

  FormatOptions green_on_black(Color::Green, Color::Black);
  std::cout << Format("Green on Black", green_on_black) << std::endl;

  FormatOptions bold_cyan_on_blue(Color::BrightCyan, Color::Blue, Style::Bold);
  std::cout << Format("Bold Cyan on Blue", bold_cyan_on_blue) << std::endl;
  std::cout << std::endl;

  // Test dividers
  std::cout << "Dividers:" << std::endl;
  std::cout << Divider() << std::endl;
  std::cout << Divider("-", 40) << std::endl;
  std::cout << Divider("*", 50) << std::endl;
  std::cout << Divider("=-", 60) << std::endl;
  std::cout << Divider("-", 70) << std::endl;
  std::cout << Divider("=", 80) << std::endl;
  std::cout << std::endl;

  // Test colored dividers
  std::cout << "Colored Dividers:" << std::endl;
  FormatOptions cyan_divider(Color::Cyan);
  std::cout << Divider("=", 80, cyan_divider) << std::endl;

  FormatOptions bold_green_divider(Color::Green, Style::Bold);
  std::cout << Divider("-", 80, bold_green_divider) << std::endl;
  std::cout << std::endl;

  // Test sanitization
  std::cout << "String Sanitization:" << std::endl;
  std::string unsafe = "Safe text\033[31mInjected\033[0m";
  std::cout << "Original: " << unsafe << std::endl;
  std::cout << "Sanitized: " << Sanitize(unsafe) << std::endl;
  std::cout << std::endl;

  // Test ANSI stripping
  std::cout << "ANSI Stripping:" << std::endl;
  std::string with_ansi = Colorize("Colored text", Color::Red);
  std::cout << "With ANSI: " << with_ansi << std::endl;
  std::cout << "Stripped: " << StripAnsi(with_ansi) << std::endl;
  std::cout << std::endl;

  std::cout << Format("Demo completed successfully!",
                      FormatOptions(Color::Green, Style::Bold))
            << std::endl;

  // state of test symbols
  std::cout << TestInProgress() << std::endl;
  std::cout << TestPassed() << std::endl;
  std::cout << TestFailed() << std::endl;
  return 0;
}
