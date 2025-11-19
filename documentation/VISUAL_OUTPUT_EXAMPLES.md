# Visual Output Examples

This document shows examples of the enhanced Colorize function output.

## Test Output

When running the verification test (`tests/verify_streamable_concept.cpp`), the output shows correctly colorized values of different types:

```
=== Verifying Streamable Type Template Approach ===

String: Hello         (displays in GREEN)
Int: 42              (displays in BLUE)
Double: 3.14159      (displays in YELLOW)
Bool (true): 1       (displays in GREEN)
Bool (false): 0      (displays in RED)
Char: X              (displays in BLUE)
Long: 9876543210     (displays in YELLOW)
Float: 2.71828       (displays in GREEN)

✓ All types successfully colorized!
The template approach works correctly.
```

## ANSI Code Verification

The raw output with visible ANSI codes (using `cat -A`):

```
String: ^[[32mHello^[[0m        (^[[32m = green color code)
Int: ^[[34m42^[[0m             (^[[34m = blue color code)
Double: ^[[33m3.14159^[[0m     (^[[33m = yellow color code)
Bool (true): ^[[32m1^[[0m      (^[[32m = green color code)
Bool (false): ^[[31m0^[[0m     (^[[31m = red color code)
Char: ^[[34mX^[[0m             (^[[34m = blue color code)
Long: ^[[33m9876543210^[[0m    (^[[33m = yellow color code)
Float: ^[[32m2.71828^[[0m      (^[[32m = green color code)
```

All ANSI codes are correctly applied:
- `\033[31m` - Red
- `\033[32m` - Green
- `\033[33m` - Yellow
- `\033[34m` - Blue
- `\033[0m` - Reset

## Usage in Real Code

### Before Enhancement (strings only):
```cpp
std::cout << conmat::Colorize("The answer is ", conmat::Color::Blue);
std::cout << conmat::Colorize(std::to_string(42), conmat::Color::Green);  // Manual conversion required
std::cout << "\n";
```

### After Enhancement (any streamable type):
```cpp
std::cout << conmat::Colorize("The answer is ", conmat::Color::Blue);
std::cout << conmat::Colorize(42, conmat::Color::Green);  // Works directly!
std::cout << "\n";
```

### More Examples:
```cpp
// Colorize test results
bool passed = true;
std::cout << "Test result: " << conmat::Colorize(passed, conmat::Color::Green) << "\n";

// Colorize numeric values
double pi = 3.14159;
std::cout << "Pi = " << conmat::Colorize(pi, conmat::Color::Yellow) << "\n";

// Colorize with formatting
conmat::FormatOptions opts(conmat::Color::Red, conmat::Style::Bold);
int error_count = 5;
std::cout << "Errors: " << conmat::Format(error_count, opts) << "\n";
```

## Terminal Output Preview

When displayed in a terminal with ANSI support, the output would look like this:

```
String: Hello         (text appears in green)
Int: 42              (number appears in blue)
Double: 3.14159      (number appears in yellow)
Bool (true): 1       (number appears in green)
Bool (false): 0      (number appears in red)
Char: X              (character appears in blue)
Long: 9876543210     (number appears in yellow)
Float: 2.71828       (number appears in green)
```

## Compilation Success

The verification test compiles cleanly with C++23:

```bash
$ clang++ -std=c++23 tests/verify_streamable_concept.cpp -o verify_streamable
$ echo $?
0
```

No warnings, no errors - clean compilation! ✅

## Test Execution Success

```bash
$ ./verify_streamable
=== Verifying Streamable Type Template Approach ===

[... colorized output ...]

✓ All types successfully colorized!
The template approach works correctly.

$ echo $?
0
```

Exit code 0 indicates successful execution! ✅
