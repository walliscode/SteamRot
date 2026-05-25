/////////////////////////////////////////////////
/// @file
/// @brief Result types returned by Descriptor evaluations.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AnalysisEvent.h"
#include <cstdint>
#include <string>
#include <vector>

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
/// @struct DescriptorResult
/// @brief Base result type for all descriptor evaluations.
///
/// Stores the boolean outcome of a descriptor evaluation and an
/// @c AnalysisTrace recording the structured events produced during
/// evaluation. Provides bool conversion so result objects can be used
/// directly in conditional expressions and assertions.
/////////////////////////////////////////////////
struct DescriptorResult {
  /////////////////////////////////////////////////
  /// @brief The outcome of the descriptor evaluation.
  /////////////////////////////////////////////////
  bool m_result{false};

  /////////////////////////////////////////////////
  /// @brief Structured events produced during this evaluation.
  /////////////////////////////////////////////////
  AnalysisTrace m_trace{};

  DescriptorResult() = default;
  explicit DescriptorResult(bool result) : m_result(result) {}

  /////////////////////////////////////////////////
  /// @brief Convert to bool for use in conditional expressions.
  /////////////////////////////////////////////////
  explicit operator bool() const noexcept { return m_result; }

  /////////////////////////////////////////////////
  /// @brief Equality comparison with another DescriptorResult.
  /////////////////////////////////////////////////
  bool operator==(const DescriptorResult &other) const noexcept {
    return m_result == other.m_result;
  }

  /////////////////////////////////////////////////
  /// @brief Equality comparison with a plain bool.
  /////////////////////////////////////////////////
  bool operator==(bool other) const noexcept { return m_result == other; }
};

/////////////////////////////////////////////////
/// @struct NodeDescriptorResult
/// @brief Result type for NodeDescriptor evaluations.
///
/// @c m_reason carries a human-readable explanation of the predicate
/// outcome (e.g. "connection_count=2, expected<=1").
/////////////////////////////////////////////////
struct NodeDescriptorResult : DescriptorResult {
  /////////////////////////////////////////////////
  /// @brief Human-readable explanation of the predicate outcome.
  /////////////////////////////////////////////////
  std::string m_reason{};

  NodeDescriptorResult() = default;
  explicit NodeDescriptorResult(bool result, std::string reason = {})
      : DescriptorResult(result), m_reason(std::move(reason)) {}
};

/////////////////////////////////////////////////
/// @struct ChainDescriptorResult
/// @brief Result type for ChainDescriptor evaluations.
///
/// @c valid_subgraphs and @c invalid_subgraphs record the part-ID paths
/// explored by the DFS. The @c m_trace (inherited from DescriptorResult)
/// contains the full structured event sequence and is the primary source
/// for analysis output.
/////////////////////////////////////////////////
struct ChainDescriptorResult : DescriptorResult {

  /////////////////////////////////////////////////
  /// @brief Ordered sets of part IDs (one per match) that satisfy the chain
  /// pattern from the anchor node.
  /////////////////////////////////////////////////
  std::vector<std::vector<uint32_t>> valid_subgraphs;

  /////////////////////////////////////////////////
  /// @brief Ordered sets of part IDs that partially satisfy the chain pattern
  /// but fail at some step.
  /////////////////////////////////////////////////
  std::vector<std::vector<uint32_t>> invalid_subgraphs;

  ChainDescriptorResult() = default;
  explicit ChainDescriptorResult(bool result) : DescriptorResult(result) {}
};

} // namespace steamrot::logic::descriptors
