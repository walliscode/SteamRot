/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the NodeDescriptor class
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
#include <functional>

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

} // namespace steamrot::logic::descriptors
