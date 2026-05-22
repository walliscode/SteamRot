/////////////////////////////////////////////////
/// @file
/// @brief Implentation of free functions related to depth-first searches
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "depth_first_search.h"
#include "AnalysisEvent.h"
#include "MachinaFormScaffold.h"

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
Transition resolve_transition(const ChainStep &step,
                              const bool predicate_result,
                              StepProgress progress) {
  switch (step.kind) {
  case ChainStepKind::Sequence: {

    return predicate_result
               ? Transition{TransitionKind::ConsumeNodeAndAdvanceStep}
               : Transition{TransitionKind::Reject};
    ;
  }
  case ChainStepKind::WhileIsTrue: {

    if (predicate_result) {
      // increate count of how many nodes have satisfied the predicate for this
      // step
      progress.match_count++;
      // consume the node and stay on the current step
      return Transition{TransitionKind::ConsumeNodeAndHoldStep};
    }
    // if the predicate fails, we only advance if we've satisfied the predicate
    // at least once, otherwise this chain is invalid and we reject
    return progress.match_count >= 1
               ? Transition{TransitionKind::HoldNodeAndAdvanceStep}
               : Transition{TransitionKind::Reject};
  }
  case ChainStepKind::WhileIsTrueForN: {
    if (predicate_result) {
      // increate count of how many nodes have satisfied the predicate for this
      // step
      progress.match_count++;
      // consume the node and stay on the current step
      return Transition{TransitionKind::ConsumeNodeAndHoldStep};
    }
    // if the predicate fails, we only advance if we've satisfied the predicate
    // at least min_repetitions times, otherwise this chain is invalid and we
    // reject
    return progress.match_count >= step.min_repetitions
               ? Transition{TransitionKind::HoldNodeAndAdvanceStep}
               : Transition{TransitionKind::Reject};
  }
  }
}

/////////////////////////////////////////////////
void depth_first_search(Cursor cursor, DFSContext &context,
                        const PartGraph &parts, ChainDescriptorResult &result) {

  // set some local variables for readability
  const auto &current_id = cursor.current_id;
  const auto &current_node = parts.find(current_id);
  if (current_node == parts.end()) {
    // if the current node doesn't exist in the graph, this is an invalid chain
    result.invalid_subgraphs.push_back(context.current_chain);
    AnalysisEvent invalid_event{};
    invalid_event.kind = TraceEventKind::InvalidSubgraphIsolated;
    invalid_event.depth = cursor.depth;
    context.trace.push_back(std::move(invalid_event));
    return;
  }
  // All steps satisfied
  if (cursor.steps_it == context.steps_end) {

    // we've created a valid subgraph! record it in the result
    result.valid_subgraphs.push_back(context.current_chain);

    // emit ValidSubgraphIsolated event
    AnalysisEvent valid_event{};
    valid_event.kind = TraceEventKind::ValidSubgraphIsolated;
    valid_event.depth = cursor.depth;
    context.trace.push_back(std::move(valid_event));

    return;
  }

  // if the node has already been visited we have a cycle
  // current behaviour is to return, however we may want cycles in the future
  if (context.visited.count(cursor.current_id))
    return;

  // Evaluate the current node against the current step's predicate
  const ChainStep &current_step = *cursor.steps_it;
  const NodeDescriptor &current_predicate = current_step.predicate;
  auto predicate_result = current_predicate(parts, cursor.current_id);
  Merge(context.trace, std::move(predicate_result.m_trace));

  // Ask what should be done with the result
  Transition transition = resolve_transition(
      current_step, static_cast<bool>(predicate_result), cursor.progress);

  // handle non consuming outcomes
  switch (transition.kind) {

    // simplest case, this route no longer works
  case TransitionKind::Reject: {
    // we've found a invalid chain, record the failture and emit trace
    result.invalid_subgraphs.push_back(context.current_chain);
    AnalysisEvent invalid_event{};
    invalid_event.kind = TraceEventKind::InvalidSubgraphIsolated;
    invalid_event.depth = cursor.depth;

    return;
  }
    // used for when we are ending a while loop and want to reevalute the same
    // node with the next step
  case TransitionKind::HoldNodeAndAdvanceStep: {
    // create a new Cursor object as a copy of the current one
    Cursor next_cursor = cursor;
    // advance variables
    next_cursor.steps_it = std::next(next_cursor.steps_it);
    next_cursor.progress = transition.progress;
    // perform dfs with the new cursor
    depth_first_search(next_cursor, context, parts, result);
  }

  case TransitionKind::ConsumeNodeAndHoldStep: {
    // mark the current node as visited and add it to the current chain
    context.visited.insert(cursor.current_id);
    context.current_chain.push_back(cursor.current_id);
  }

  case TransitionKind::ConsumeNodeAndAdvanceStep: {
    // // mark the current node as visited and add it to the current chain
    // context.visited.insert(cursor.current_id);
    // context.current_chain.push_back(cursor.current_id);
    // // create a new Cursor object as a copy of the current one
    // Cursor next_cursor = cursor;
    // // advance variables
    // next_cursor.steps_it = std::next(next_cursor.steps_it);
    // next_cursor.progress = transition.progress;
    // // perform dfs with the new cursor
    // depth_first_search(next_cursor, context, parts, result);
  }
  }

  // now we move onto the next nodes in the graph, anything that has a held node

  // use std::visit so we guarantee that the sockets is type safe
  const SocketMap &sockets = std::visit(
      [](const auto &instance) -> const SocketMap & {
        return instance.sockets;
      },
      current_node->second);

  // cycle through the neighbours of the current node
  for (const auto &[socket_id, socket_data] : sockets) {

    // if this socket isn't connected to anything, skip it
    if (!socket_data.connected_to)
      continue;

    // get the neighbour id from the socket connection
    const uint32_t neighbour_id = socket_data.connected_to->peer_part_id;
    if (context.visited.count(neighbour_id))
      // potentially add some kind of event here in the future to indicate that
      // we're skipping a visited node
      continue;

    // emit a moving to neighbour event
    AnalysisEvent move_event{};
    move_event.kind = TraceEventKind::MovingToNeighbour;
    move_event.depth = cursor.depth;
    move_event.from_id = cursor.current_id;
    move_event.to_id = neighbour_id;
    move_event.socket_id = socket_id;
    context.trace.push_back(std::move(move_event));

    // set up a new cursor for the recursive call
    Cursor child{};
    child.current_id = neighbour_id;
    child.steps_it = cursor.steps_it;
    child.progress = cursor.progress;
    child.depth = cursor.depth + 1;

    // call dfs recursively with the new cursor
    depth_first_search(child, context, parts, result);

    // if we are here then we have backtracked from the neighbour, emit a
    // backtracking event
    AnalysisEvent backtrack_event{};
    backtrack_event.kind = TraceEventKind::Backtracking;
    backtrack_event.depth = cursor.depth;
    backtrack_event.from_id = neighbour_id;
    backtrack_event.to_id = cursor.current_id;
    context.trace.push_back(std::move(backtrack_event));
  }

  // we've now visited all the neighbours of the current node, we need to unmark
  // it as we return
  context.visited.erase(cursor.current_id);
  context.current_chain.pop_back();

  // not strictly necessary, but to indiciate that we are unwinding
  return;
}
} // namespace steamrot::logic::descriptors
