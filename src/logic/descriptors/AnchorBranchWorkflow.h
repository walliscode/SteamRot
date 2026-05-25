/////////////////////////////////////////////////
/// @file
/// @brief Workflow classes for anchor/branch/cardinality descriptor matching.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "descriptors_chain_descriptors.h"
#include "descriptors_node_descriptors.h"
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
/// @struct BranchEvaluation
/// @brief Evaluation outcome for one branch starting from one anchor neighbour.
/////////////////////////////////////////////////
struct BranchEvaluation {
  uint32_t m_start_id{0};
  bool m_passed{false};
  std::vector<uint32_t> m_matched_path{};
  std::string m_reason{};
};

/////////////////////////////////////////////////
/// @struct AnchorBranchWorkflowDefinition
/// @brief Definition of an anchor/branch/cardinality workflow.
/////////////////////////////////////////////////
struct AnchorBranchWorkflowDefinition {
  NodeDescriptor m_anchor_rule{};
  ChainDescriptor m_branch_rule{};
  size_t m_minimum_valid_branches{1};
  size_t m_assignment_count{0};
};

/////////////////////////////////////////////////
/// @struct AnchorBranchWorkflowResult
/// @brief Outcome and assignments produced by AnchorBranchWorkflow::Execute.
/////////////////////////////////////////////////
struct AnchorBranchWorkflowResult {
  bool m_overall_pass{false};
  NodeDescriptorResult m_anchor_result{};
  uint32_t m_anchor_id{0};
  std::vector<std::vector<uint32_t>> m_assigned_branches{};
  std::vector<BranchEvaluation> m_branch_evaluations{};

  explicit operator bool() const noexcept { return m_overall_pass; }
};

/////////////////////////////////////////////////
/// @class AnchorBranchWorkflow
/// @brief Runs an anchor predicate, evaluates branch predicates, applies
/// cardinality, and assigns the first N passing branches.
/////////////////////////////////////////////////
class AnchorBranchWorkflow {
public:
  /////////////////////////////////////////////////
  /// @brief Construct a workflow from the provided definition.
  ///
  /// @param definition Workflow definition.
  /////////////////////////////////////////////////
  explicit AnchorBranchWorkflow(AnchorBranchWorkflowDefinition definition)
      : m_definition(std::move(definition)) {}

  /////////////////////////////////////////////////
  /// @brief Returns the workflow definition.
  /////////////////////////////////////////////////
  const AnchorBranchWorkflowDefinition &GetDefinition() const noexcept {
    return m_definition;
  }

  /////////////////////////////////////////////////
  /// @brief Execute the workflow from @p anchor_id.
  ///
  /// @param parts PartGraph to query.
  /// @param anchor_id Stable part ID for the anchor node.
  /// @return AnchorBranchWorkflowResult containing pass/fail and assignments.
  /////////////////////////////////////////////////
  AnchorBranchWorkflowResult Execute(const PartGraph &parts,
                                     uint32_t anchor_id) const;

private:
  AnchorBranchWorkflowDefinition m_definition{};
};

} // namespace steamrot::logic::descriptors
