/////////////////////////////////////////////////
/// @file
/// @brief Generic combinators, lifting utilities, and ChainDescriptorBuilder
///        for the four-level descriptor hierarchy.
///
/// Descriptor hierarchy (narrowest to broadest scope):
///   NodeDescriptor             bool(const PartNode&)
///   ContextualNodeDescriptor   bool(const PartGraph&, const PartNode&)
///   ChainDescriptor            bool(const PartGraph&, const PartNode& start)
///   GraphDescriptor            bool(const PartGraph&)
///
/// Use lift() to widen a NodeDescriptor into a ContextualNodeDescriptor or
/// ChainDescriptor without changing its logic.
/// Use any_node_satisfies() / all_nodes_satisfy() to derive a GraphDescriptor
/// from a ChainDescriptor or ContextualNodeDescriptor.
/// Use ChainDescriptorBuilder to build multi-hop ChainDescriptors.
/// Use and_(), or_(), not_() as generic combinators across all descriptor
/// levels.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "PartGraph.h"
#include <utility>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
/// Lifting utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// @brief Lift a NodeDescriptor to a ContextualNodeDescriptor.
///
/// The PartGraph argument is ignored; the wrapped descriptor is applied to
/// the node alone. Use when a function requires a ContextualNodeDescriptor
/// but you only need to examine the node's own data.
///
/// @param nd NodeDescriptor to lift.
/// @return ContextualNodeDescriptor that delegates to @p nd.
/////////////////////////////////////////////////
inline ContextualNodeDescriptor lift(NodeDescriptor nd) {
  return [nd = std::move(nd)](const PartGraph & /*graph*/,
                               const PartNode &node) -> bool {
    return nd(node);
  };
}

/////////////////////////////////////////////////
/// @brief Lift a NodeDescriptor to a ChainDescriptor.
///
/// Equivalent to lifting to ContextualNodeDescriptor — the start node is
/// evaluated against @p nd and no walk is performed. Use when a function
/// requires a ChainDescriptor but your predicate only examines the anchor.
///
/// @param nd NodeDescriptor to lift.
/// @return ChainDescriptor that delegates to @p nd.
/////////////////////////////////////////////////
inline ChainDescriptor lift_to_chain(NodeDescriptor nd) {
  return [nd = std::move(nd)](const PartGraph & /*graph*/,
                               const PartNode &node) -> bool {
    return nd(node);
  };
}

/////////////////////////////////////////////////
/// Graph-level query builders
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// @brief Build a GraphDescriptor that returns true when at least one node
///        in the graph satisfies @p cd.
///
/// @param cd ChainDescriptor (or ContextualNodeDescriptor) to evaluate.
/// @return GraphDescriptor returning true if any node satisfies @p cd.
/////////////////////////////////////////////////
inline GraphDescriptor any_node_satisfies(ChainDescriptor cd) {
  return [cd = std::move(cd)](const PartGraph &graph) -> bool {
    for (const auto &node : graph.nodes)
      if (cd(graph, node))
        return true;
    return false;
  };
}

/////////////////////////////////////////////////
/// @brief Build a GraphDescriptor that returns true when every node
///        in the graph satisfies @p cd.
///
/// @param cd ChainDescriptor (or ContextualNodeDescriptor) to evaluate.
/// @return GraphDescriptor returning true if all nodes satisfy @p cd.
/////////////////////////////////////////////////
inline GraphDescriptor all_nodes_satisfy(ChainDescriptor cd) {
  return [cd = std::move(cd)](const PartGraph &graph) -> bool {
    for (const auto &node : graph.nodes)
      if (!cd(graph, node))
        return false;
    return true;
  };
}

/////////////////////////////////////////////////
/// Generic combinators
///
/// Work uniformly with NodeDescriptor, ContextualNodeDescriptor,
/// ChainDescriptor, and GraphDescriptor. Template deduction ensures that
/// both arguments must be the same descriptor level; mixing levels is a
/// compile error.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// @brief Return a descriptor that is true when both @p a and @p b are true.
///
/// @tparam Desc Any descriptor type.
/// @param a First descriptor.
/// @param b Second descriptor.
/// @return Combined descriptor returning a(...) && b(...).
/////////////////////////////////////////////////
template <typename Desc> Desc and_(Desc a, Desc b) {
  return [a = std::move(a), b = std::move(b)](auto &&...args) -> bool {
    return a(std::forward<decltype(args)>(args)...) &&
           b(std::forward<decltype(args)>(args)...);
  };
}

/////////////////////////////////////////////////
/// @brief Return a descriptor that is true when either @p a or @p b is true.
///
/// @tparam Desc Any descriptor type.
/// @param a First descriptor.
/// @param b Second descriptor.
/// @return Combined descriptor returning a(...) || b(...).
/////////////////////////////////////////////////
template <typename Desc> Desc or_(Desc a, Desc b) {
  return [a = std::move(a), b = std::move(b)](auto &&...args) -> bool {
    return a(std::forward<decltype(args)>(args)...) ||
           b(std::forward<decltype(args)>(args)...);
  };
}

/////////////////////////////////////////////////
/// @brief Return a descriptor that negates @p a.
///
/// @tparam Desc Any descriptor type.
/// @param a Descriptor to negate.
/// @return Descriptor returning !a(...).
/////////////////////////////////////////////////
template <typename Desc> Desc not_(Desc a) {
  return [a = std::move(a)](auto &&...args) -> bool {
    return !a(std::forward<decltype(args)>(args)...);
  };
}

/////////////////////////////////////////////////
/// @class ChainDescriptorBuilder
/// @brief Builds a ChainDescriptor from an ordered list of NodeDescriptor
///        steps matched against a DFS walk through the PartGraph.
///
/// Usage:
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

} // namespace steamrot
