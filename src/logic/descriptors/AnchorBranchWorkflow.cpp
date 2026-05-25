/////////////////////////////////////////////////
/// @file
/// @brief Implementation for AnchorBranchWorkflow.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AnchorBranchWorkflow.h"
#include <algorithm>
#include <utility>

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
AnchorBranchWorkflowResult
AnchorBranchWorkflow::Execute(const PartGraph &parts, uint32_t anchor_id) const {
  AnchorBranchWorkflowResult result{};
  result.m_anchor_id = anchor_id;
  result.m_anchor_result = m_definition.m_anchor_rule(parts, anchor_id);

  if (!result.m_anchor_result) {
    result.m_overall_pass = false;
    return result;
  }

  const auto anchor_it = parts.find(anchor_id);
  if (anchor_it == parts.end()) {
    result.m_overall_pass = false;
    return result;
  }

  std::vector<std::vector<uint32_t>> passing_paths{};
  const SocketMap &anchor_sockets = std::visit(
      [](const auto &instance) -> const SocketMap & { return instance.sockets; },
      anchor_it->second);

  for (const auto &[socket_id, socket_data] : anchor_sockets) {
    if (!socket_data.connected_to)
      continue;
    (void)socket_id;

    const uint32_t neighbour_id = socket_data.connected_to->peer_part_id;
    BranchEvaluation branch{};
    branch.m_start_id = neighbour_id;

    const ChainDescriptorResult branch_result =
        m_definition.m_branch_rule(parts, neighbour_id);
    branch.m_passed = static_cast<bool>(branch_result);

    if (!branch_result.valid_subgraphs.empty())
      branch.m_matched_path = branch_result.valid_subgraphs.front();

    if (!branch.m_passed)
      branch.m_reason = "branch_rule_failed";

    if (branch.m_passed && !branch.m_matched_path.empty())
      passing_paths.push_back(branch.m_matched_path);

    result.m_branch_evaluations.push_back(std::move(branch));
  }

  const size_t passing_count = passing_paths.size();
  const bool cardinality_passed =
      passing_count >= m_definition.m_minimum_valid_branches;
  result.m_overall_pass = cardinality_passed;

  if (!cardinality_passed)
    return result;

  size_t assignment_count = m_definition.m_assignment_count;
  if (assignment_count == 0)
    assignment_count = passing_count;
  assignment_count = std::min(assignment_count, passing_count);

  result.m_assigned_branches.assign(passing_paths.begin(),
                                    passing_paths.begin() + assignment_count);
  return result;
}

} // namespace steamrot::logic::descriptors
