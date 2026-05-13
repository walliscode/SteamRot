/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the free functions for Graph Descriptors
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
#include <string>
#include <type_traits>

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
/// @class GraphDescriptor
/// @brief Named, callable predicate for the whole PartGraph with no anchor.
///
/// Wraps a @c std::function with the signature
/// @c GraphDescriptorResult(const PartGraph&).
/// Derive instances via @c any_node_satisfies() or @c all_nodes_satisfy().
/// @c GraphDescriptor is the terminal type in the hierarchy — it is only
/// consumed by Logic/Action classes and is never passed back into a modifier
/// or combinator.
/////////////////////////////////////////////////
class GraphDescriptor {
public:
  /////////////////////////////////////////////////
  /// @brief Underlying callable type.
  /////////////////////////////////////////////////
  using FnType = std::function<GraphDescriptorResult(const PartGraph &)>;

  GraphDescriptor() = default;

  /////////////////////////////////////////////////
  /// @brief Construct a named descriptor.
  ///
  /// @param name Human-readable descriptor name used in trace events.
  /// @param fn   Callable implementing the graph-level predicate.
  /////////////////////////////////////////////////
  GraphDescriptor(std::string name, FnType fn)
      : m_name(std::move(name)), m_fn(std::move(fn)) {}

  /////////////////////////////////////////////////
  /// @brief Construct an unnamed descriptor from any compatible callable.
  /////////////////////////////////////////////////
  template <typename F,
            typename = std::enable_if_t<
                !std::is_same_v<std::decay_t<F>, GraphDescriptor>>>
  GraphDescriptor(F &&fn) // NOLINT(google-explicit-constructor)
      : m_fn(std::forward<F>(fn)) {}

  /////////////////////////////////////////////////
  /// @brief Evaluate the descriptor against the entire @p parts graph.
  ///
  /// @param parts The PartGraph to query.
  /////////////////////////////////////////////////
  GraphDescriptorResult operator()(const PartGraph &parts) const {
    return m_fn(parts);
  }

  /////////////////////////////////////////////////
  /// @brief Return the descriptor's stable name.
  /////////////////////////////////////////////////
  const std::string &GetName() const noexcept { return m_name; }

  /////////////////////////////////////////////////
  /// @brief Return the underlying std::function.
  /////////////////////////////////////////////////
  const FnType &GetFn() const noexcept { return m_fn; }

  /////////////////////////////////////////////////
  /// @brief True when the descriptor holds a callable.
  /////////////////////////////////////////////////
  explicit operator bool() const noexcept { return static_cast<bool>(m_fn); }

private:
  std::string m_name{};
  FnType m_fn{};
};

} // namespace steamrot::logic::descriptors
