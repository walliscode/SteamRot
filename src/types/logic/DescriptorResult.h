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
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
/// @brief An ordered list of part IDs produced by a successful DFS walk.
///
/// Aliases @c std::vector<uint32_t> so that archetype result structs can use
/// a named, self-documenting type for their fields.
/////////////////////////////////////////////////
using SubGraph = std::vector<uint32_t>;

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
/// @c valid_subgraph records the first matching part-ID path and
/// @c invalid_subgraphs records rejected part-ID paths explored by the DFS.
/// The @c m_trace (inherited from DescriptorResult) contains the full
/// structured event sequence and is the primary source for analysis output.
/////////////////////////////////////////////////
struct ChainDescriptorResult : DescriptorResult {

  /////////////////////////////////////////////////
  /// @brief First part-ID path that satisfies the chain pattern from the
  /// anchor node.
  /////////////////////////////////////////////////
  std::optional<std::vector<uint32_t>> valid_subgraph{};

  /////////////////////////////////////////////////
  /// @brief Ordered sets of part IDs that partially satisfy the chain pattern
  /// but fail at some step.
  /////////////////////////////////////////////////
  std::vector<std::vector<uint32_t>> invalid_subgraphs{};

  ChainDescriptorResult() = default;
  explicit ChainDescriptorResult(bool result) : DescriptorResult(result) {}
};

struct TestArchetypeResult {
  SubGraph chain1;
  SubGraph chain2;
  std::vector<SubGraph> chains;
};
/////////////////////////////////////////////////
/// @class GrabResult
/// @brief Store the anchor and arms of a successful Grab MachinaArchetype
/////////////////////////////////////////////////
struct GrabResult {
  /////////////////////////////////////////////////
  /// @brief Anchor node from which the arms extend
  /////////////////////////////////////////////////
  SubGraph anchor;

  /////////////////////////////////////////////////
  /// @brief Collection of grabbing arms
  /////////////////////////////////////////////////
  std::vector<SubGraph> arms;
};

using ArchetypeVariant = std::variant<TestArchetypeResult, GrabResult>;

/////////////////////////////////////////////////
/// @class MachinaArchetypeResult
/// @brief Result type for MachinaArchetype evaluations.
/////////////////////////////////////////////////
struct MachinaArchetypeResult : DescriptorResult {

  /////////////////////////////////////////////////
  /// @brief default constructor for MachinaArchetypeResult.
  /////////////////////////////////////////////////
  MachinaArchetypeResult() = default;

  /////////////////////////////////////////////////
  /// @brief Construct a MachinaArchetypeResult with a boolean result.
  ///
  /// @param result
  /////////////////////////////////////////////////
  explicit MachinaArchetypeResult(bool result, ArchetypeVariant variant)
      : DescriptorResult(result), result_sub_graphs(variant) {}

  /////////////////////////////////////////////////
  /// @brief Variant to store all possible subgraph results from
  /// MachinaArchetype evaluation.
  /////////////////////////////////////////////////
  ArchetypeVariant result_sub_graphs;
};
} // namespace steamrot::logic::descriptors
