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
  Sequence
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
  MachinaArchetype Build(std::string name) {
    auto steps = m_steps;
    return MachinaArchetype{
        std::move(name),
        [steps = std::move(steps)](const PartGraph &parts,
                                   uint32_t id) -> MachinaArchetypeResult {
          // create variant to hold the result struct, default-constructed
          std::variant<T> result_struct{};

          // evaluate each step in order, passing the same (parts, id) to each
          bool all_passed = true;
          for (const auto &step : steps) {
            ChainDescriptorResult chain_result = step.descriptor(parts, id);
            if (!chain_result) {
              all_passed = false;
            }
          }
          return MachinaArchetypeResult{all_passed, result_struct};
        }};
  }
};
} // namespace steamrot::logic::descriptors
