/////////////////////////////////////////////////
/// @file
/// @brief General matcher helper functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include <ostream>
#include <vector>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Helper to print std::vector to output stream
///
/// @tparam T [TODO:tparam]
/// @param os [TODO:parameter]
/// @param vec [TODO:parameter]
/// @return [TODO:return]
/////////////////////////////////////////////////
template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vec) {
  os << "[";
  for (size_t i = 0; i < vec.size(); ++i) {
    os << vec[i];
    if (i + 1 < vec.size())
      os << ", ";
  }
  os << "]";
  return os;
}
} // namespace steamrot::tests
