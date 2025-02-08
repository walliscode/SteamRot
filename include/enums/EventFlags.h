#pragma once
#include <cstdint>
#include <magic_enum/magic_enum.hpp>

enum class EventFlags : std::uint64_t {
  A = 1ULL << 0,
  B = 1ULL << 1,
  C = 1ULL << 2,
  D = 1ULL << 3,
  E = 1ULL << 4,
  F = 1ULL << 5,
  G = 1ULL << 6,
  H = 1ULL << 7,
  I = 1ULL << 8,
  J = 1ULL << 9,
  K = 1ULL << 10,
  L = 1ULL << 11,
  M = 1ULL << 12,
  N = 1ULL << 13,
  O = 1ULL << 14,
  P = 1ULL << 15,
  Q = 1ULL << 16,
  R = 1ULL << 17,
  S = 1ULL << 18,
  T = 1ULL << 19,
  U = 1ULL << 20,
  V = 1ULL << 21,
  W = 1ULL << 22,
  X = 1ULL << 23,
  Y = 1ULL << 24,
  Z = 1ULL << 25,
  CONTROL = 1ULL << 26,
  ALT = 1ULL << 27,
  LEFT = 1ULL << 28,
  RIGHT = 1ULL << 29,
  UP = 1ULL << 30,
  DOWN = 1ULL << 31,
};

template <> struct magic_enum::customize::enum_range<EventFlags> {
  static constexpr bool is_flags = true;
};
