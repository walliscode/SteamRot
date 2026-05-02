/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ChainDescriptorBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ChainDescriptorBuilder.h"
#include "DescriptorResult.h"
#include <expected>
#include <vector>

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
ChainDescriptorBuilder &ChainDescriptorBuilder::Then(NodeDescriptor nd) {
  m_steps.push_back({std::move(nd), ChainStepKind::Sequence});
  return *this;
}

/////////////////////////////////////////////////
ChainDescriptorBuilder &ChainDescriptorBuilder::WhileIsTrue(NodeDescriptor nd) {
  m_steps.push_back({std::move(nd), ChainStepKind::WhileIsTrue});
  return *this;
}

/////////////////////////////////////////////////
std::string ChainDescriptorBuilder::Validate() const {

  // return string
  std::string error_message;

  // if m_build_finalised is true, then Build() has already been called, so we
  // can't modify the builder
  if (m_build_finalised) {
    error_message += "Cannot modify builder after Build() has been called. ";
  }

  return error_message;
}

/////////////////////////////////////////////////
std::expected<ChainDescriptor, std::string> ChainDescriptorBuilder::Build() {

  // validate the builder state before building the descriptor
  std::string validation_error = Validate();
  if (!validation_error.empty()) {
    return std::unexpected(validation_error);
  }

  // mark the builder as finalised to prevent further modification
  m_build_finalised = true;

  // stand in implementation: just return a descriptor that always returns false
  // for now
  return [](const PartGraph &graph,
            const PartNode &start_node) -> ChainDescriptorResult {
    return ChainDescriptorResult{false};
  };
}
/////////////////////////////////////////////////
void ChainDescriptorBuilder::dfs(
    std::vector<ChainStep>::const_iterator steps_it,
    std::vector<ChainStep>::const_iterator steps_end,
    const PartNode &current_node, std::vector<bool> &visited,
    const PartGraph &main_graph, PartGraph &current_chain,
    ChainDescriptorResult &result) {

  /////////////////////////////////////////////////
  /// CHECKING END CONDITIONS
  /////////////////////////////////////////////////

  // if we've reached the end of the steps, return true to indicate a successful
  // match. This ignores any current nodes as we've satisfied all the predicates
  // in the chain, so we can store the result
  if (steps_it == steps_end) {
    // store the current chain as a valid subgraph in the result
    result.valid_subgraphs.push_back(current_chain);
    return;
  }
  // if current node has already been visited, return false to avoid cycles
  // we may need to think about if we are tying to identify a cycle in the
  // graph, as this could be a valid match for some descriptors (maybe
  // is_visited). this may just be testing specific predicates for now
  if (visited[current_node.id])
    return;

  /////////////////////////////////////////////////
  /// EVALUATING CURRENT NODE
  /////////////////////////////////////////////////

  // get the current step's predicate
  NodeDescriptor current_predicate = steps_it->predicate;

  // match current step predicate against current node, if it fails, return
  // false
  if (!current_predicate(current_node)) {

    // switch on the step kind to determine how to handle failure of the current
    // step
    switch (steps_it->kind) {
    case ChainStepKind::Sequence: {
      result.invalid_subgraphs.push_back(current_chain);
      // for a Sequence step, we simply return as this chain has failed
      return;
    }
    case ChainStepKind::WhileIsTrue: {
      dfs(std::next(steps_it), steps_end, current_node, visited, main_graph,
          current_chain, result);
      return;
    }
    }
  }

  // mark current node as visited
  visited[current_node.id] = true;

  // recursively visit adjacent nodes for the next step in the chain
  for (const auto &edge_index : current_node.edge_indices) {

    // get the adjacent node from the main graph using the adjacent node ID
    const PartEdge &edge = main_graph.edges[edge_index];
    const uint32_t neighbour_id =
        (edge.part_id_a == current_node.id) ? edge.part_id_b : edge.part_id_a;
    const PartNode &adjacent_node =
        main_graph.nodes[main_graph.node_index_by_id.at(neighbour_id)];

    // switch on the step kind as this determines how we progress certain
    // steps in the chain
    switch (steps_it->kind) {
    case ChainStepKind::Sequence: {
      // for a Sequence step, we simply move to the next ChainStep as this
      // one is satisfied
      steps_it = std::next(steps_it);
      break;
    }
    case ChainStepKind::WhileIsTrue: {

      // for a WhileIsTrue step, we stay on the current ChainStep
      break;
    }
    }

    // call the dfs function recursively
    dfs(steps_it, steps_end, adjacent_node, visited, main_graph, current_chain,
        result);
  }

  // unset current node as visited before backtracking
  visited[current_node.id] = false;

  return;
}

} // namespace steamrot::logic::descriptors
