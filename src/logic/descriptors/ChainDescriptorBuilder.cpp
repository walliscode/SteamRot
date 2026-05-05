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
#include <unordered_set>
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

  // capture steps by value so the descriptor is self-contained
  auto steps = m_steps;

  return [steps = std::move(steps)](const PartGraph &parts,
                                    uint32_t start_id) -> ChainDescriptorResult {
    ChainDescriptorResult result;
    std::unordered_set<uint32_t> visited;
    std::vector<uint32_t> current_chain;

    ChainDescriptorBuilder::dfs(steps.cbegin(), steps.cend(), start_id,
                                visited, parts, current_chain, result, false);

    result.m_result = !result.valid_subgraphs.empty();
    return result;
  };
}

/////////////////////////////////////////////////
void ChainDescriptorBuilder::dfs(
    std::vector<ChainStep>::const_iterator steps_it,
    std::vector<ChainStep>::const_iterator steps_end, uint32_t current_id,
    std::unordered_set<uint32_t> &visited, const PartGraph &parts,
    std::vector<uint32_t> &current_chain, ChainDescriptorResult &result,
    bool while_consumed) {

  /////////////////////////////////////////////////
  /// CHECKING END CONDITIONS
  /////////////////////////////////////////////////

  // if we've reached the end of the steps, return true to indicate a successful
  // match. This ignores any current nodes as we've satisfied all the predicates
  // in the chain, so we can store the result
  if (steps_it == steps_end) {
    // store the current chain part IDs as a valid subgraph in the result
    result.valid_subgraphs.push_back(current_chain);
    return;
  }
  // if current node has already been visited, return false to avoid cycles
  // we may need to think about if we are tying to identify a cycle in the
  // graph, as this could be a valid match for some descriptors (maybe
  // is_visited). this may just be testing specific predicates for now
  if (visited.count(current_id))
    return;

  /////////////////////////////////////////////////
  /// EVALUATING CURRENT NODE
  /////////////////////////////////////////////////

  // get the current step's predicate
  NodeDescriptor current_predicate = steps_it->predicate;

  // match current step predicate against current node, if it fails, return
  // false
  if (!current_predicate(parts, current_id)) {

    // switch on the step kind to determine how to handle failure of the current
    // step
    switch (steps_it->kind) {
    case ChainStepKind::Sequence: {
      result.invalid_subgraphs.push_back(current_chain);
      // for a Sequence step, we simply return as this chain has failed
      return;
    }
    case ChainStepKind::WhileIsTrue: {
      // WhileIsTrue requires at least one node to have been consumed before it
      // can exit. Reject the path if nothing was consumed yet.
      if (!while_consumed) {
        result.invalid_subgraphs.push_back(current_chain);
        return;
      }
      // Pass the failing node to the next step (exit the while loop).
      dfs(std::next(steps_it), steps_end, current_id, visited, parts,
          current_chain, result, false);
      return;
    }
    }
  }

  // mark current node as visited
  visited.insert(current_id);

  // find neighbours by iterating the part's sockets
  const SocketMap &sockets = std::visit(
      [](const auto &inst) -> const SocketMap & { return inst.sockets; },
      parts.at(current_id));

  bool any_unvisited_neighbour{false};
  for (const auto &[socket_id, socket] : sockets) {
    if (!socket.connected_to.has_value())
      continue;

    const uint32_t neighbour_id = socket.connected_to->peer_part_id;

    // skip already-visited neighbours to avoid redundant cycles and to allow
    // accurate dead-end detection below
    if (visited.count(neighbour_id))
      continue;

    any_unvisited_neighbour = true;

    // effective_steps_it is the step iterator to pass to the recursive call;
    // it advances for Sequence steps (one node consumed) but stays on the
    // current step for WhileIsTrue steps (one-or-more consumption).
    auto effective_steps_it = steps_it;
    switch (steps_it->kind) {
    case ChainStepKind::Sequence: {
      effective_steps_it = std::next(steps_it);
      break;
    }
    case ChainStepKind::WhileIsTrue: {
      // stay on the current step
      break;
    }
    }

    // mark that at least one node has now been consumed by this WhileIsTrue
    // step before the recursive call
    const bool next_while_consumed =
        (steps_it->kind == ChainStepKind::WhileIsTrue);

    // call the dfs function recursively
    dfs(effective_steps_it, steps_end, neighbour_id, visited, parts,
        current_chain, result, next_while_consumed);
  }

  // WhileIsTrue dead-end: the current node satisfied the predicate but there
  // are no unvisited neighbours left to continue the walk. Succeed only when
  // no more steps remain; fail otherwise (the pattern requires more nodes).
  if (!any_unvisited_neighbour &&
      steps_it->kind == ChainStepKind::WhileIsTrue) {
    if (std::next(steps_it) == steps_end) {
      result.valid_subgraphs.push_back(current_chain);
    } else {
      result.invalid_subgraphs.push_back(current_chain);
    }
  }

  // unmark current node before backtracking
  visited.erase(current_id);

  return;
}

} // namespace steamrot::logic::descriptors
