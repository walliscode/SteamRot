/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ChainDescriptorBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ChainDescriptorBuilder.h"
#include <vector>

namespace {

/////////////////////////////////////////////////
/// @brief Attempt to match @p steps[step_idx..] starting from the node at
///        @p node_idx in @p graph.
///
/// Performs a depth-first walk, marking consumed nodes in @p visited to avoid
/// revisiting them. Backtracks when no path satisfies the remaining steps.
///
/// @param graph     Graph to walk.
/// @param node_idx  Index into @c graph.nodes for the current candidate node.
/// @param visited   Per-node visited flags; must have @c graph.nodes.size()
///                  entries.
/// @param step_idx  Index of the step currently being matched.
/// @param steps     Ordered list of steps to match.
/// @return @c true when a path satisfying all remaining steps is found.
/////////////////////////////////////////////////
bool TryMatch(
    const steamrot::PartGraph &graph,
    size_t node_idx,
    std::vector<bool> &visited,
    size_t step_idx,
    const std::vector<steamrot::logic::descriptors::ChainStep> &steps) {

  if (step_idx >= steps.size())
    return true;

  if (node_idx >= graph.nodes.size() || visited[node_idx])
    return false;

  const steamrot::PartNode &node = graph.nodes[node_idx];
  const steamrot::logic::descriptors::ChainStep &step = steps[step_idx];
  const bool predicate_holds = static_cast<bool>(step.predicate(node));

  switch (step.kind) {

  case steamrot::logic::descriptors::ChainStepKind::Sequence: {
    if (!predicate_holds)
      return false;

    if (step_idx + 1 >= steps.size())
      return true;

    visited[node_idx] = true;
    for (const size_t edge_idx : node.edge_indices) {
      const steamrot::PartEdge &edge = graph.edges[edge_idx];
      const uint32_t neighbour_id =
          (edge.part_id_a == node.id) ? edge.part_id_b : edge.part_id_a;
      const auto it = graph.node_index_by_id.find(neighbour_id);
      if (it == graph.node_index_by_id.end())
        continue;
      const size_t neighbour_idx = it->second;
      if (!visited[neighbour_idx] &&
          TryMatch(graph, neighbour_idx, visited, step_idx + 1, steps))
        return true;
    }
    visited[node_idx] = false;
    return false;
  }

  case steamrot::logic::descriptors::ChainStepKind::WhileIsTrue: {
    if (predicate_holds) {
      visited[node_idx] = true;

      if (step_idx + 1 >= steps.size())
        return true;

      for (const size_t edge_idx : node.edge_indices) {
        const steamrot::PartEdge &edge = graph.edges[edge_idx];
        const uint32_t neighbour_id =
            (edge.part_id_a == node.id) ? edge.part_id_b : edge.part_id_a;
        const auto it = graph.node_index_by_id.find(neighbour_id);
        if (it == graph.node_index_by_id.end())
          continue;
        const size_t neighbour_idx = it->second;
        if (visited[neighbour_idx])
          continue;
        // Continue consuming (same step) or exit the loop (next step).
        if (TryMatch(graph, neighbour_idx, visited, step_idx, steps))
          return true;
        if (TryMatch(graph, neighbour_idx, visited, step_idx + 1, steps))
          return true;
      }
      visited[node_idx] = false;
      return false;
    } else {
      // Predicate fails: exit the loop consuming zero nodes; pass the current
      // node unchanged to the next step.
      return TryMatch(graph, node_idx, visited, step_idx + 1, steps);
    }
  }
  }

  return false;
}

} // anonymous namespace

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
ChainDescriptorBuilder &ChainDescriptorBuilder::StartWith(NodeDescriptor nd) {
  m_steps.push_back({std::move(nd), ChainStepKind::Sequence});
  return *this;
}

/////////////////////////////////////////////////
ChainDescriptorBuilder &ChainDescriptorBuilder::Then(NodeDescriptor nd) {
  m_steps.push_back({std::move(nd), ChainStepKind::Sequence});
  return *this;
}

/////////////////////////////////////////////////
ChainDescriptorBuilder &
ChainDescriptorBuilder::WhileIsTrue(NodeDescriptor nd) {
  m_steps.push_back({std::move(nd), ChainStepKind::WhileIsTrue});
  return *this;
}

/////////////////////////////////////////////////
ChainDescriptor ChainDescriptorBuilder::End(NodeDescriptor nd) {
  m_steps.push_back({std::move(nd), ChainStepKind::Sequence});
  return [steps = std::move(m_steps)](const PartGraph &graph,
                                      const PartNode &start) -> ChainDescriptorResult {
    if (steps.empty())
      return ChainDescriptorResult{false};

    const auto it = graph.node_index_by_id.find(start.id);
    if (it == graph.node_index_by_id.end())
      return ChainDescriptorResult{false};

    std::vector<bool> visited(graph.nodes.size(), false);
    return ChainDescriptorResult{
        TryMatch(graph, it->second, visited, 0, steps)};
  };
}

} // namespace steamrot::logic::descriptors
