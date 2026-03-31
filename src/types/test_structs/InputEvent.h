/////////////////////////////////////////////////
/// @file
/// @brief Declaration of InputSequence struct and related types
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <string>
#include <variant>

namespace steamrot::tests {

enum class InputType {
  None = 0,
  MouseMove = 1,
  MouseClick = 2,
  MouseRelease = 3,
  KeyPress = 4,
  KeyRelease = 5,
};

struct MousePositionData {
  float x{0.0f};
  float y{0.0f};
};
struct MouseInputData {
  float x{0.0f};
  float y{0.0f};
  int button{0}; // 0 = left, 1 = right
};

struct KeyInputData {
  int key_code{0}; // sf::Keyboard::Key value
};

using InputData = std::variant<MouseInputData, KeyInputData>;

struct InputEvent {
  InputType input_type{InputType::None};
  InputData input_data{MouseInputData{}};
  std::string description{};
};

} // namespace steamrot::tests
