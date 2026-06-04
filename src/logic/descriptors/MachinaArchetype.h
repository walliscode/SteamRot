/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the MachinaArchetype class.
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
/// @class MachinaArchetype
/// @brief coordinates ChainDescriptors and assigns results to custom structs to
/// produce some form of "archetype" or "functionality" such as grab
///
/////////////////////////////////////////////////
class MachinaArchetype {

  /////////////////////////////////////////////////
  /// @brief Underlying callable type.
  /////////////////////////////////////////////////
  using FnType = std::function<MachinaArchetypeResult(const PartGraph &,
                                                      uint32_t, uint32_t)>;

private:
  /////////////////////////////////////////////////
  /// @brief name of the archetype, used in trace events and set on construction
  /////////////////////////////////////////////////
  const std::string m_name;

  /////////////////////////////////////////////////
  /// @brief Underlaying lambda that implements the archetype logic, set on
  /// construction
  /////////////////////////////////////////////////
  const FnType m_fn;

  /////////////////////////////////////////////////
  /// @brief Number of steps in this archetype, used for trace formatting
  /////////////////////////////////////////////////
  const size_t number_of_steps{0};

public:
  MachinaArchetype() = default;
  /////////////////////////////////////////////////
  /// @brief Construct a named descriptor.
  ///
  /// @param name Human-readable descriptor name used in trace events.
  /// @param fn   Callable implementing the chain walk.
  /////////////////////////////////////////////////
  MachinaArchetype(std::string name, FnType fn, const size_t num_steps)
      : m_name(std::move(name)), m_fn(std::move(fn)),
        number_of_steps(num_steps) {}

  /////////////////////////////////////////////////
  /// @brief Evaluate the chain descriptor from @p id as anchor.
  ///
  /// The returned @c MachinaArchetypeResult already has its @c m_trace
  /// populated by the DFS (via ScopeBegin/ScopeEnd and per-node events).
  ///
  /// @param parts The PartGraph to query.
  /// @param id    Stable part ID of the anchor node.
  /// @param depth Nesting depth used for archetype-scope trace events.
  /////////////////////////////////////////////////
  MachinaArchetypeResult operator()(const PartGraph &parts, uint32_t id,
                                    uint32_t depth = 0) const {
    return m_fn(parts, id, depth);
  }

  /////////////////////////////////////////////////
  /// @brief Return the descriptor's stable name.
  /////////////////////////////////////////////////
  const std::string &GetName() const noexcept { return m_name; }

  /////////////////////////////////////////////////
  /// @brief Return the number of steps in this archetype, used for trace
  /// formatting and debugging
  ///
  /// @return [TODO:return]
  /////////////////////////////////////////////////
  const size_t GetNumberOfSteps() const noexcept { return number_of_steps; }
  /////////////////////////////////////////////////
  /// @brief Return the underlying std::function.
  /////////////////////////////////////////////////
  const FnType &GetFn() const noexcept { return m_fn; }

  /////////////////////////////////////////////////
  /// @brief True when the descriptor holds a callable. Propogates the state of
  /// the predicate
  /////////////////////////////////////////////////
  explicit operator bool() const noexcept { return static_cast<bool>(m_fn); }
};
} // namespace steamrot::logic::descriptors
