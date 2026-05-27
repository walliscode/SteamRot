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

  MachinaArchetypeBuilder &Then(ChainDescriptor cd, SubGraph T::*ptr);

  MachinaArchetypeResult Build(std::string name);
};
} // namespace steamrot::logic::descriptors
