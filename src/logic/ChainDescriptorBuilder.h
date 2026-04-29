/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the ChainDescriptorBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "PartGraph.h"
#include <vector>

namespace steamrot::logic {

/////////////////////////////////////////////////
/// @class ChainDescriptorBuilder
/// @brief Builds a ChainDescriptor from an ordered list of NodeDescriptor
///        steps matched against a DFS walk through the PartGraph.
///
/// Usage (from outside the `steamrot::logic` namespace):
/// @code
/// ChainDescriptor linear_3 =
///     steamrot::logic::ChainDescriptorBuilder{}
///         .StartWith(is_terminal)
///         .Then(is_serial)
///         .End(is_terminal);
/// @endcode
///
/// Usage (from within `steamrot::logic` or via a `using` declaration):
/// @code
/// ChainDescriptor linear_3 =
///     ChainDescriptorBuilder{}
///         .StartWith(is_terminal)
///         .Then(is_serial)
///         .End(is_terminal);
/// @endcode
///
/// @note Multi-hop DFS traversal is not yet implemented. The built descriptor
///       always returns false until the TODO below is resolved.
/////////////////////////////////////////////////
class ChainDescriptorBuilder {
public:
  /////////////////////////////////////////////////
  /// @brief Set the predicate for the start node of the chain.
  ///
  /// @param nd NodeDescriptor to apply to the first node.
  /// @return *this for method chaining.
  /////////////////////////////////////////////////
  ChainDescriptorBuilder &StartWith(NodeDescriptor nd) {
    m_steps.push_back(std::move(nd));
    return *this;
  }

  /////////////////////////////////////////////////
  /// @brief Append a predicate for the next node in the walk.
  ///
  /// May be called zero or more times between StartWith() and End().
  ///
  /// @param nd NodeDescriptor to apply to the next node in the walk.
  /// @return *this for method chaining.
  /////////////////////////////////////////////////
  ChainDescriptorBuilder &Then(NodeDescriptor nd) {
    m_steps.push_back(std::move(nd));
    return *this;
  }

  /////////////////////////////////////////////////
  /// @brief Append the predicate for the final node and build the descriptor.
  ///
  /// @param nd NodeDescriptor to apply to the last node.
  /// @return ChainDescriptor representing the full walk pattern.
  /////////////////////////////////////////////////
  ChainDescriptor End(NodeDescriptor nd) {
    m_steps.push_back(std::move(nd));
    // TODO: implement DFS traversal over the PartGraph using m_steps.
    // Walk starting from the anchor node, following edges, and verify each
    // visited node against the corresponding step predicate in order.
    // Return true only when a complete path matching all steps is found.
    return [steps = std::move(m_steps)](const PartGraph & /*graph*/,
                                        const PartNode & /*start*/) -> bool {
      (void)steps;
      return false;
    };
  }

private:
  /////////////////////////////////////////////////
  /// @brief Ordered node predicates from StartWith() through End().
  /////////////////////////////////////////////////
  std::vector<NodeDescriptor> m_steps{};
};

} // namespace steamrot::logic
