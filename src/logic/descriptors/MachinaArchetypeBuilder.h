/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the MachinaArchetypeBuilder class.
///
/// @note All method definitions are provided inline in this header because
/// @c MachinaArchetypeBuilder is a class template. C++ requires template
/// definitions to be visible at every instantiation site; placing them here
/// ensures the compiler can generate specialisations for any @p T.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AnalysisEvent.h"
#include "ChainDescriptor.h"
#include "DescriptorResult.h"
#include "MachinaArchetype.h"
#include "descriptors_analysis_event_helpers.h"
#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
/// @enum ArchetypeStepKind
/// @brief Classifies how a step in a @c MachinaArchetypeBuilder is matched.
///
/// Currently only @c Sequence is supported, meaning each step must match
/// exactly once in declaration order.
/////////////////////////////////////////////////
enum class ArchetypeStepKind {
  /////////////////////////////////////////////////
  /// @brief The step's ChainDescriptor must match exactly once, in sequence.
  /////////////////////////////////////////////////
  Sequence,

  /////////////////////////////////////////////////
  /// @brief At leatst N neighbours from the current node match the step's
  /// ChainDescriptor
  /////////////////////////////////////////////////
  AtLeastNOf,

};

struct ArchetypeAnalysisContext {
  AnalysisTrace trace;
};

/////////////////////////////////////////////////
/// @class MachinaArchetypeBuilder
/// @brief Fluent builder that assembles a @c MachinaArchetype from an ordered
///        list of @c ChainDescriptor steps.
///
/// @tparam T  User-defined result struct whose member fields receive the
///            @c SubGraph produced by each step. Each @c Then() call binds
///            one @c ChainDescriptor to one @c SubGraph member of @p T.
///
/// Typical usage:
/// @code
/// struct GrabArchetype { SubGraph arm; SubGraph grip; };
///
/// MachinaArchetype archetype =
///     MachinaArchetypeBuilder<GrabArchetype>{}
///         .Then(is_serial_arm, &GrabArchetype::arm)
///         .Then(is_grip,       &GrabArchetype::grip)
///         .Build("grab");
/// @endcode
/////////////////////////////////////////////////
template <typename T> class MachinaArchetypeBuilder {

  /////////////////////////////////////////////////
  /// @struct ArchetypeStep
  /// @brief Internal representation of a single builder step.
  ///
  /// Bundles the @c ChainDescriptor to evaluate, the @c ArchetypeStepKind
  /// that controls how it is applied, and a pointer-to-member that identifies
  /// the @c T field where the matching @c SubGraph should be stored.
  /////////////////////////////////////////////////
  struct ArchetypeStep {

    /////////////////////////////////////////////////
    /// @brief Descriptor evaluated at this step.
    /////////////////////////////////////////////////
    ChainDescriptor descriptor;

    /////////////////////////////////////////////////
    /// @brief How the descriptor is matched during evaluation.
    /////////////////////////////////////////////////
    ArchetypeStepKind kind;

    /////////////////////////////////////////////////
    /// @brief Destination field in the result struct @c T.
    ///
    /// A @c SubGraph @c T::* stores a single matching path; a
    /// @c std::vector<SubGraph> @c T::* stores multiple paths (reserved for
    /// future step kinds).
    /////////////////////////////////////////////////
    std::variant<SubGraph T::*, std::vector<SubGraph> T::*> result_storage;

    /////////////////////////////////////////////////
    /// @brief For use with step kinds that match multiple subgraphs, the
    /// minimum number of matches required to satisfy the step.
    /////////////////////////////////////////////////
    size_t min_repetitions{};

    explicit ArchetypeStep(
        ChainDescriptor descriptor, ArchetypeStepKind kind,
        std::variant<SubGraph T::*, std::vector<SubGraph> T::*> result_storage,
        size_t min_repetitions = 0)
        : descriptor(std::move(descriptor)), kind(kind),
          result_storage(std::move(result_storage)),
          min_repetitions(min_repetitions) {};
  };

private:
  static T &GetTypedResult(MachinaArchetypeResult &result) {
    return std::get<T>(result.result_sub_graphs);
  }

  static bool EvaluateSequenceStep(const ArchetypeStep &step,
                                   const PartGraph &parts,
                                   uint32_t graph_cursor, uint32_t depth,
                                   ArchetypeAnalysisContext &context,
                                   T &typed_result) {

    // guard statement for invalid pointer type; should never happen because
    // Then() only accepts SubGraph T::*, but added defensively to fail safely
    // if that invariant is ever broken.
    if (!std::holds_alternative<SubGraph T::*>(step.result_storage)) {
      return false;
    }
    SubGraph &result_field =
        typed_result.*std::get<SubGraph T::*>(step.result_storage);

    // analyise node with descriptor
    ChainDescriptorResult step_result =
        step.descriptor(parts, graph_cursor, depth + 1);

    // merge trace to parent context
    Merge(context.trace, std::move(step_result.m_trace));

    // assign valid subgraph to result field if it exists
    if (step_result && step_result.valid_subgraph.has_value())
      result_field = step_result.valid_subgraph.value();

    // add event showing whether result assignment happened
    add_machina_part_result_event(context, step.descriptor.GetName(),
                                  static_cast<bool>(step_result), depth);

    return static_cast<bool>(step_result);
  }

  static bool EvaluateAtLeastNOfStep(const ArchetypeStep &step,
                                     const PartGraph &parts,
                                     const SocketMap &sockets, uint32_t depth,
                                     ArchetypeAnalysisContext &context,
                                     T &typed_result) {
    // This should never happen because AtLeastNOf always stores a vector
    // pointer; keep this defensive guard to fail safely if that invariant
    // is ever broken.
    if (!std::holds_alternative<std::vector<SubGraph> T::*>(
            step.result_storage))
      return false;

    size_t matches_found = 0;
    std::vector<SubGraph> &result_vector =
        typed_result.*std::get<std::vector<SubGraph> T::*>(step.result_storage);

    for (const auto &[socket_id, socket_data] : sockets) {
      (void)socket_id;
      if (!socket_data.connected_to.has_value())
        continue;

      const uint32_t neighbour_id = socket_data.connected_to->peer_part_id;
      ChainDescriptorResult step_result =
          step.descriptor(parts, neighbour_id, depth + 1);
      Merge(context.trace, std::move(step_result.m_trace));

      if (step_result && step_result.valid_subgraph.has_value()) {
        result_vector.push_back(*step_result.valid_subgraph);
        matches_found++;
      }
    }

    const bool step_succeeded = matches_found >= step.min_repetitions;
    add_machina_part_result_event(context, step.descriptor.GetName(),
                                  step_succeeded, depth);
    return step_succeeded;
  }

  static MachinaArchetypeResult
  Evaluate(const std::vector<ArchetypeStep> &steps,
           const std::string &archetype_name, const PartGraph &parts,
           uint32_t start_id, uint32_t depth) {
    MachinaArchetypeResult result{false, T{}};
    ArchetypeAnalysisContext context{};

    if (parts.empty()) {
      add_empty_part_graph_event(context);
      result.m_trace = std::move(context.trace);
      return result;
    }

    if (steps.empty()) {
      add_empty_chain_steps_event(context);
      result.m_trace = std::move(context.trace);
      return result;
    }

    add_scope_begin_event(context, archetype_name, ScopeKind::MachinaArchetype,
                          parts, depth, start_id);

    const auto start_node_it = parts.find(start_id);
    if (start_node_it == parts.end()) {
      add_scope_end_event(context, archetype_name, ScopeKind::MachinaArchetype,
                          false, depth);
      result.m_trace = std::move(context.trace);
      return result;
    }

    const SocketMap &start_sockets = std::visit(
        [](const auto &instance) -> const SocketMap & {
          return instance.sockets;
        },
        start_node_it->second);

    result.m_result = true;
    T &typed_result = GetTypedResult(result);

    for (const ArchetypeStep &step : steps) {
      bool step_succeeded = false;
      switch (step.kind) {
      case ArchetypeStepKind::Sequence:
        step_succeeded = EvaluateSequenceStep(step, parts, start_id, depth,
                                              context, typed_result);
        break;
      case ArchetypeStepKind::AtLeastNOf:
        step_succeeded = EvaluateAtLeastNOfStep(step, parts, start_sockets,
                                                depth, context, typed_result);
        break;
      }

      result.m_result = result.m_result && step_succeeded;
    }

    add_scope_end_event(context, archetype_name, ScopeKind::MachinaArchetype,
                        result.m_result, depth);
    result.m_trace = std::move(context.trace);
    return result;
  }

  /////////////////////////////////////////////////
  /// @brief Ordered steps appended via @c Then().
  /////////////////////////////////////////////////
  std::vector<ArchetypeStep> m_steps{};

public:
  MachinaArchetypeBuilder() = default;

  /////////////////////////////////////////////////
  /// @brief Return an immutable reference to the steps added so far.
  ///
  /// Primarily used in unit tests to inspect builder state.
  ///
  /// @return Const reference to the internal step vector.
  /////////////////////////////////////////////////
  const std::vector<ArchetypeStep> &GetSteps() const { return m_steps; }

  /////////////////////////////////////////////////
  /// @brief Append a ChainDescriptor whose valid subgraph result will be
  /// stored in the member field pointed to by @p ptr.
  ///
  /// Adds an @c ArchetypeStepKind::Sequence step. May be called one or more
  /// times before @c Build().
  ///
  /// @param cd  ChainDescriptor to evaluate at this step.
  /// @param ptr Pointer-to-member of @c T where the matching SubGraph is
  ///            stored.
  /// @return *this for method chaining.
  /////////////////////////////////////////////////
  MachinaArchetypeBuilder &Then(ChainDescriptor cd, SubGraph T::*ptr) {
    m_steps.push_back(
        ArchetypeStep{std::move(cd), ArchetypeStepKind::Sequence, ptr});
    return *this;
  }

  /////////////////////////////////////////////////
  /// @brief Append a ChainDescriptor that must match at least @p
  /// min_repetitions times
  ///
  /// @param cd ChainDescriptor to evaluate at this step.
  /// @param min_repetitions Minimum number of matches required to satisfy this
  /// step.
  /// @return *this for method chaining.
  /////////////////////////////////////////////////
  MachinaArchetypeBuilder &AtLeastNOf(ChainDescriptor cd,
                                      size_t min_repetitions,
                                      std::vector<SubGraph> T::*ptr) {
    m_steps.push_back(ArchetypeStep{
        std::move(cd), ArchetypeStepKind::AtLeastNOf, ptr, min_repetitions});
    return *this;
  }

  /////////////////////////////////////////////////
  /// @brief Consume the builder and produce a named @c MachinaArchetype.
  ///
  /// The returned @c MachinaArchetype wraps a lambda that, when called with a
  /// @c (PartGraph, part_id) pair, evaluates every accumulated step in order.
  /// The overall result is @c true only when all steps succeed.
  ///
  /// @param name Human-readable name stamped on the archetype and used in
  ///             trace events.
  /// @return A fully configured @c MachinaArchetype ready for evaluation.
  /////////////////////////////////////////////////
  MachinaArchetype Build(std::string archetype_name) {
    auto steps = m_steps;
    return MachinaArchetype{
        archetype_name,
        [steps = std::move(steps), archetype_name = archetype_name](
            const PartGraph &parts, uint32_t start_id,
            uint32_t depth) -> MachinaArchetypeResult {
          return Evaluate(steps, archetype_name, parts, start_id, depth);
        },
        m_steps.size()};
  }
};
} // namespace steamrot::logic::descriptors
