/////////////////////////////////////////////////
/// @file
/// @brief provides overloads for std::visit to allow lambdas to be used with
/// std::variant
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

namespace steamrot {
template <class... Ts> struct overload : Ts... {
  using Ts::operator()...;

  // Prevent implicit type conversions
  consteval void operator()(auto) const {
    static_assert(false, "Unsupported type");
  }
};
} // namespace steamrot
