/////////////////////////////////////////////////
/// @file
/// @brief Declarartion of the MachinFormScaffold struct. No implementation is
/// needed
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "DescriptorResult.h"
#include "FragmentInstance.h"
#include "JointInstance.h"
#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace steamrot {

/// using helpers ///
using PartInstanceVariant = std::variant<JointInstance, FragmentInstance>;

using PartGraph = std::map<uint32_t, PartInstanceVariant>;

using StructuralAnalysisResults =
    std::unordered_map<std::string, std::vector<MachinaArchetypeResult>>;

enum class StructuralAnalysisState {
  NotRun,       ///< No analysis has been run yet.
  NothingFound, ///< Analysis was run but no archetypes were found.
  Found         ///< Analysis was run and archetypes were found.
};
/////////////////////////////////////////////////
/// 3struct MachinaFormScaffold
/// @brief Contains all data necessary to create a MachinaForm.
///
/// This is designed to be an "unbaked" editor workspace. When committed to a
/// MachinaForm, the scaffold data will be processed into efficient runtime
/// formats by the MachinaForm's own systems.
/////////////////////////////////////////////////
struct MachinaFormScaffold {

  /////////////////////////////////////////////////
  /// @brief Name transferred to the MachinaForm on commit.
  /////////////////////////////////////////////////
  std::string machina_form_name{""};

  /////////////////////////////////////////////////
  /// @brief Monotonically increasing counter used to assign stable IDs to
  /// newly added FragmentInstances and JointInstances.
  /////////////////////////////////////////////////
  uint32_t next_id{0};

  /////////////////////////////////////////////////
  /// @brief All placed parts (JointInstances and FragmentInstances) keyed by
  /// their stable IDs.
  ///
  /// Using std::map gives node-stable storage: insertions and erasures never
  /// invalidate references or iterators to other elements.
  /////////////////////////////////////////////////
  PartGraph parts;

  // Boolean States //

  /////////////////////////////////////////////////
  /// @brief a boolean toggle used to determine whether to render sockets on the
  /// Joint and Fragments.
  /////////////////////////////////////////////////
  bool are_sockets_visible{false};

  // Analysis Results //
  /////////////////////////////////////////////////
  /// @brief Holds the current state of the structural analysis.
  /////////////////////////////////////////////////
  StructuralAnalysisState structural_analysis_state{
      StructuralAnalysisState::NotRun};

  /////////////////////////////////////////////////
  /// @brief Holds the results of an analysis run, keyed by archetype name. Each
  /// archetype may produce multiple results
  /////////////////////////////////////////////////
  StructuralAnalysisResults structural_analysis_results{};
};

} // namespace steamrot
