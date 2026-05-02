/////////////////////////////////////////////////
/// @file
/// @brief Result types returned by Descriptor evaluations.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "PartGraph.h"
namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
/// @struct DescriptorResult
/// @brief Base result type for all descriptor evaluations.
///
/// Stores the boolean outcome of a descriptor evaluation and provides
/// bool conversion so result objects can be used directly in conditional
/// expressions and assertions.
///
/// Derived types (NodeDescriptorResult, ChainDescriptorResult,
/// GraphDescriptorResult) are reserved for future extension with
/// level-specific analysis data.
/////////////////////////////////////////////////
struct DescriptorResult {
  /////////////////////////////////////////////////
  /// @brief The outcome of the descriptor evaluation.
  /////////////////////////////////////////////////
  bool m_result{false};

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
/// @brief Result type for NodeDescriptor and ContextualNodeDescriptor
///        evaluations.
///
/// Inherits DescriptorResult's bool conversion and equality operators.
/// Reserved for future extension with node-specific analysis data.
/////////////////////////////////////////////////
struct NodeDescriptorResult : DescriptorResult {};

/////////////////////////////////////////////////
/// @struct ChainDescriptorResult
/// @brief Result type for ChainDescriptor evaluations.
///
/// Inherits DescriptorResult's bool conversion and equality operators.
/// Reserved for future extension with chain-specific analysis data.
/////////////////////////////////////////////////
struct ChainDescriptorResult : DescriptorResult {

  /////////////////////////////////////////////////
  /// @brief An isograph of the main graph matching the chain pattern
  /////////////////////////////////////////////////
  std::vector<PartGraph> valid_subgraphs;

  /////////////////////////////////////////////////
  /// @brief an isograph of the main graph that partially matches the chain
  /// pattern but fails at some point
  /////////////////////////////////////////////////
  std::vector<PartGraph> invalid_subgraphs;
};

/////////////////////////////////////////////////
/// @struct GraphDescriptorResult
/// @brief Result type for GraphDescriptor evaluations.
///
/// Inherits DescriptorResult's bool conversion and equality operators.
/// Reserved for future extension with graph-specific analysis data.
/////////////////////////////////////////////////
struct GraphDescriptorResult : DescriptorResult {};

} // namespace steamrot::logic::descriptors
