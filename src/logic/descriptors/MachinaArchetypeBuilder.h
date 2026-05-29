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
#include "ChainDescriptor.h"
#include "DescriptorResult.h"
#include "MachinaArchetype.h"
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
            const PartGraph &parts,
            uint32_t start_id) -> MachinaArchetypeResult {
          // create MachinaArchetypeResult to accumulate the final result and
          // trace
          MachinaArchetypeResult result{false, T{}};

          // create context to hold the trace and any other state we want to
          // pass
          ArchetypeAnalysisContext context{};

          // Check for empty PartGraph
          if (parts.empty()) {

            // this is automatically a failed result
            result.m_result = false;

            // set up EmtpyGraph event in the trace
            AnalysisEvent empty_graph_event{};
            empty_graph_event.kind = TraceEventKind::EmtpyPartGraph;
            context.trace.push_back(std::move(empty_graph_event));
            result.m_trace = std::move(context.trace);

            // return early since there's no graph to traverse
            return result;
          }

          // Check for empty steps
          if (steps.empty()) {

            // this is automatically a failed result since a chain with no
            // steps can't be satisfied
            result.m_result = false;

            // set up EmptyChainSteps event in the trace to explain why the
            // result is false
            AnalysisEvent empty_steps_event{};
            empty_steps_event.kind = TraceEventKind::EmtpyChainSteps;
            context.trace.push_back(std::move(empty_steps_event));
            result.m_trace = std::move(context.trace);

            // return early since there's no steps to evaluate
            return result;
          }

          AnalysisEvent scope_begin{};
          scope_begin.kind = TraceEventKind::ScopeBegin;
          scope_begin.depth = 0;
          scope_begin.scope_name = archetype_name;
          scope_begin.scope_kind = ScopeKind::MachinaArchetype;
          scope_begin.anchor_id = start_id;
          if (const auto anchor_it = parts.find(start_id);
              anchor_it != parts.end()) {
            scope_begin.part_id_alias = std::visit(
                [](const auto &inst) -> const std::string & {
                  return inst.alias;
                },
                anchor_it->second);
          }
          context.trace.push_back(std::move(scope_begin));

          // create cursor
          uint32_t graph_cursor = start_id;
          // set starting result to true
          result.m_result = true;

          // cycle through the steps and evalute each one depending on its
          // kind
          for (const auto &step : steps) {
            switch (step.kind) {
            case ArchetypeStepKind::Sequence: {

              // evalute the current node with the step's descriptor
              ChainDescriptorResult step_result =
                  step.descriptor(parts, graph_cursor);

              // for a simple sequence step, if the result is false then the
              // whole archetype fails and we can break early; if it's true then
              // we can continue to the next step
              if (!step_result)
                result.m_result = false;

              break;
            }
            case ArchetypeStepKind::AtLeastNOf: {
              break;
            }
            }
            // if result is false then break early without evaluating further
            // steps
            if (!result.m_result)
              break;
          }
          // move trace and return result
          result.m_trace = std::move(context.trace);
          return result;
        }};
  }
};
} // namespace steamrot::logic::descriptors
