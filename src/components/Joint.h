/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the Joint struct. No implementation is needed
/////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
struct Joint {
  uint8_t m_number_of_connections;

  /////////////////////////////////////////////////
  /// @brief Connects joint to fragments
  ///
  /// The index is the connection point in the joint and the value is the index
  /////////////////////////////////////////////////
  std::vector<size_t> m_connected_fragments;
};
