/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the MachinaArchetypeBuilder class.
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
#include <cstdint>
#include <variant>
#include <vector>

namespace steamrot::logic::descriptors {

using SubGraph = std::vector<uint32_t>;

enum class ArchetypeStepKind { Sequence };

template <typename T> class MachinaArchetypeBuilder {

  struct ArchetypeStep {

    ChainDescriptor descriptor;

    ArchetypeStepKind kind;

    std::variant<SubGraph T::*, std::vector<SubGraph> T::*> result_storage;
  };

private:
  std::vector<ArchetypeStep> m_steps{};

public:
  MachinaArchetypeBuilder() = default;

  /////////////////////////////////////////////////
  /// @brief Return an immutable reference to the steps added so far.
  ///
  /// @return a const reference to the vector of ArchetypeSteps added to this
  /// builder.
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

  MachinaArchetypeResult Build(std::string name);
};
} // namespace steamrot::logic::descriptors
