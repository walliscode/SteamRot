#pragma once
#include <magic_enum/magic_enum.hpp>

enum class EventFlags : std::uint64_t {
  A = 1 << 0,
  B = 1 << 1,
  C = 1 << 2,
  D = 1 << 3,
  E = 1 << 4,
  F = 1 << 5,
  G = 1 << 6,
  H = 1 << 7,
  I = 1 << 8,
  J = 1 << 9,
  K = 1 << 10,
  L = 1 << 11,
  M = 1 << 12,
  N = 1 << 13,
  O = 1 << 14,
  P = 1 << 15,
  Q = 1 << 16,
  R = 1 << 17,
  S = 1 << 18,
  T = 1 << 19,
  U = 1 << 20,
  V = 1 << 21,
  W = 1 << 22,
  X = 1 << 23,
  Y = 1 << 24,
  Z = 1 << 25,
  CONTROL = 1 << 26,
  ALT = 1 << 27,
};

template <> struct magic_enum::customize::enum_range<EventFlags> {
  static constexpr bool is_flags = true;
};
