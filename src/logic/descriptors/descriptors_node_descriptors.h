/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the free functions for Node Descriptors
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "DescriptorResult.h"
#include "MachinaFormScaffold.h"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <type_traits>

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
/// @class NodeDescriptor
/// @brief Named, callable predicate for single-node queries on a PartGraph.
///
/// Wraps a @c std::function with the signature
/// @c NodeDescriptorResult(const PartGraph&, uint32_t part_id)
/// and a stable string name used in analysis traces.
///
/// @c operator() calls the underlying function and records @c NodeEval and
/// @c NodeResult trace events in the returned result.
///
/// Example:
/// @code
/// NodeDescriptorResult result = descriptors::is_fragment(scaffold.parts, id);
/// if (result) { /* node is a fragment */ }
/// @endcode
/////////////////////////////////////////////////
class NodeDescriptor {
public:
  /////////////////////////////////////////////////
  /// @brief Underlying callable type.
  /////////////////////////////////////////////////
  using FnType = std::function<NodeDescriptorResult(const PartGraph &part_graph,
                                                    uint32_t part_id)>;

  NodeDescriptor() = default;

  /////////////////////////////////////////////////
  /// @brief Construct a named descriptor.
  ///
  /// @param name Human-readable predicate name used in trace events.
  /// @param fn   Callable implementing the predicate.
  /////////////////////////////////////////////////
  NodeDescriptor(std::string name, FnType fn);

  /////////////////////////////////////////////////
  /// @brief Construct an unnamed descriptor from any compatible callable.
  ///
  /// Enabled only for types other than @c NodeDescriptor itself so as not
  /// to shadow the copy/move constructors.
  /////////////////////////////////////////////////
  template <typename F, typename = std::enable_if_t<
                            !std::is_same_v<std::decay_t<F>, NodeDescriptor>>>
  NodeDescriptor(F &&fn) // NOLINT(google-explicit-constructor)
      : m_fn(std::forward<F>(fn)) {}

  /////////////////////////////////////////////////
  /// @brief Evaluate the predicate and record trace events.
  ///
  /// Emits a @c NodeEval event before and a @c NodeResult event after
  /// calling the underlying function. Both events carry @p depth so that
  /// callers (e.g. the DFS in ChainDescriptorBuilder) can control nesting.
  ///
  /// @param parts The PartGraph to query.
  /// @param id    Stable part ID of the node to evaluate.
  /// @param depth Nesting depth written into the emitted trace events.
  /// @return NodeDescriptorResult with m_trace populated.
  /////////////////////////////////////////////////
  NodeDescriptorResult operator()(const PartGraph &parts, uint32_t id,
                                  uint32_t depth = 0) const;

  /////////////////////////////////////////////////
  /// @brief Return the predicate's stable name.
  /////////////////////////////////////////////////
  const std::string &GetName() const noexcept { return m_name; }

  /////////////////////////////////////////////////
  /// @brief Return the underlying std::function.
  /////////////////////////////////////////////////
  const FnType &GetFn() const noexcept { return m_fn; }

  /////////////////////////////////////////////////
  /// @brief Return the type_info of the stored callable.
  ///
  /// Delegates to @c std::function::target_type() for test compatibility.
  /////////////////////////////////////////////////
  const std::type_info &target_type() const noexcept {
    return m_fn.target_type();
  }

  /////////////////////////////////////////////////
  /// @brief True when the descriptor holds a callable.
  /////////////////////////////////////////////////
  explicit operator bool() const noexcept { return static_cast<bool>(m_fn); }

private:
  std::string m_name{};
  FnType m_fn{};
};

/////////////////////////////////////////////////
/// @brief Predicate for a single node with full access to the PartGraph.
///
/// Semantically distinct from @c NodeDescriptor: use
/// @c ContextualNodeDescriptor when the predicate examines neighbouring
/// nodes via socket traversal, and @c NodeDescriptor when it examines only
/// the named node's own data.
///
/// Both types share the same underlying class; the distinction is enforced
/// by naming convention and the @c lift() conversion function.
/////////////////////////////////////////////////
using ContextualNodeDescriptor = NodeDescriptor;

/////////////////////////////////////////////////
/// @brief Lift a NodeDescriptor to a ContextualNodeDescriptor.
///
/// Both types share the same class; this is an identity conversion used to
/// make the semantic promotion explicit at call sites.
///
/// @param nd NodeDescriptor to lift.
/// @return ContextualNodeDescriptor wrapping @p nd.
/////////////////////////////////////////////////
inline ContextualNodeDescriptor lift(NodeDescriptor nd) { return nd; }

/////////////////////////////////////////////////
/// @brief NodeDescriptor that returns true when the node holds a
/// FragmentInstance.
/////////////////////////////////////////////////
extern const NodeDescriptor is_fragment;

/////////////////////////////////////////////////
/// @brief NodeDescriptor that returns true when the node holds a
/// JointInstance.
/////////////////////////////////////////////////
extern const NodeDescriptor is_joint;

/////////////////////////////////////////////////
/// @brief Returns a NodeDescriptor that returns true when the node has exactly
/// @p n connections (i.e., exactly @p n connected sockets).
///
/// @param n Number of connections.
/// @param name Human-readable name for the descriptor, used in trace events.
/// @return NodeDescriptor returning true when connection_count == n.
/////////////////////////////////////////////////
NodeDescriptor has_exactly_n_edges(size_t n, std::string name);

/////////////////////////////////////////////////
/// @brief Returns a NodeDescriptor that returns true when the node has exactly
/// 2 connections.
///
/// serial is used to indicate part of a serial chain
/////////////////////////////////////////////////
extern const NodeDescriptor is_serial;

/////////////////////////////////////////////////
/// @brief Returns a NodeDescriptor that returns true when the node has at least
/// @p n connections (i.e., at least @p n connected sockets).
///
/// @param n Minimum number of connections.
/// @return NodeDescriptor returning true when connection_count >= n.
/////////////////////////////////////////////////
NodeDescriptor has_minimum_n_edges(size_t n);

/////////////////////////////////////////////////
/// @brief A NodeDescriptor that returns true when the node has at least 3
/// connections (i.e., at least 3 connected sockets), indicating a "branching"
/// point
/////////////////////////////////////////////////
extern const NodeDescriptor is_branched;

/////////////////////////////////////////////////
/// @brief Returns a NodeDescriptor that returns true when the node has at
/// most @p n connections (i.e., at most @p n connected sockets).
///
/// @param n Maximum number of connections.
/// @return NodeDescriptor returning true when connection_count <= n.
/////////////////////////////////////////////////
NodeDescriptor has_maximum_n_edges(size_t n);

/////////////////////////////////////////////////
/// @brief NodeDescriptor that returns true for nodes with at most 1 connected
/// socket (i.e., at the "end" of a chain of connections).
/////////////////////////////////////////////////
extern const NodeDescriptor is_terminal;
} // namespace steamrot::logic::descriptors
