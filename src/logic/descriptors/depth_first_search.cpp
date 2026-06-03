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
#include "descriptors_analysis_event_helpers.h"

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
Transition resolve_transition(const ChainStep &step,
                              const bool predicate_result,
                              StepProgress progress) {
  switch (step.kind) {

  /////////////////////////////////////////////////
  /// Sequence: consume exactly one matching node and advance to the next step.
  /////////////////////////////////////////////////
  case ChainStepKind::Sequence: {
    return predicate_result
               ? Transition{TransitionKind::ConsumeNodeAndAdvanceStep}
               : Transition{TransitionKind::Reject};
  }

  /////////////////////////////////////////////////
  /// WhileIsTrue / WhileIsTrueForMinimumN: consume every consecutive matching
  /// node, then hand the first non-matching node to the next step.
  ///
  /// Both kinds share the same logic; the only difference is the minimum
  /// number of nodes required before the step is considered satisfied.
  /// WhileIsTrue uses the default min_repetitions of 1.
  /////////////////////////////////////////////////
  case ChainStepKind::WhileIsTrue:
  case ChainStepKind::WhileIsTrueForMinimumN: {
    if (predicate_result) {
      // Increment the count and carry it forward in the returned transition so
      // the cursor's progress is updated for the next recursive layer.
      progress.match_count++;
      return Transition{TransitionKind::ConsumeNodeAndHoldStep, progress};
    }
    // Predicate failed: advance only if the minimum count has been reached.
    return progress.match_count >= step.min_repetitions
               ? Transition{TransitionKind::HoldNodeAndAdvanceStep}
               : Transition{TransitionKind::Reject};
  }
  default:
    return Transition{TransitionKind::Reject};
  }
}

/////////////////////////////////////////////////
void depth_first_search(Cursor cursor, DFSContext &context,
                        const PartGraph &parts, ChainDescriptorResult &result) {
  if (result.valid_subgraph.has_value())
    return;

  /////////////////////////////////////////////////
  /// SECTION: Validate that the current node exists in the graph
  /////////////////////////////////////////////////
  const auto current_node = parts.find(cursor.current_id);
  if (current_node == parts.end()) {
    result.invalid_subgraphs.push_back(context.current_chain);
    add_invalid_subgraph_isolated_event(context, cursor.depth);
    return;
  }

  /////////////////////////////////////////////////
  /// SECTION: Check if all steps have been satisfied (base case)
  ///
  /// This fires when HoldNodeAndAdvanceStep re-enters with steps_end already
  /// set — the current node is not evaluated or consumed, only the chain
  /// accumulated so far is recorded as valid.
  /////////////////////////////////////////////////
  if (cursor.steps_it == context.steps_end) {
    if (!result.valid_subgraph.has_value())
      result.valid_subgraph = context.current_chain;
    return;
  }

  /////////////////////////////////////////////////
  /// SECTION: Cycle guard — skip nodes already on the current path
  /////////////////////////////////////////////////
  if (context.visited.count(cursor.current_id))
    return;

  /////////////////////////////////////////////////
  /// SECTION: Evaluate the current node against the current step's predicate
  /////////////////////////////////////////////////
  const ChainStep &current_step = *cursor.steps_it;
  const NodeDescriptor &current_predicate = current_step.predicate;
  auto predicate_result =
      current_predicate(parts, cursor.current_id, cursor.depth);
  Merge(context.trace, std::move(predicate_result.m_trace));

  Transition transition = resolve_transition(
      current_step, static_cast<bool>(predicate_result), cursor.progress);

  /////////////////////////////////////////////////
  /// SECTION: Act on the transition
  /////////////////////////////////////////////////
  switch (transition.kind) {

  case TransitionKind::Reject: {
    // Record the rejected path and return; the NodeResult trace event already
    // explains why the predicate failed.
    result.invalid_subgraphs.push_back(context.current_chain);
    return;
  }

  case TransitionKind::HoldNodeAndAdvanceStep: {
    // The node did not match the current step but the step is satisfied (e.g.
    // end of a WhileIsTrue run). Re-evaluate the same node against the next
    // step without consuming it onto the path.
    Cursor next_cursor = cursor;
    next_cursor.steps_it = std::next(next_cursor.steps_it);
    next_cursor.progress = {};
    depth_first_search(next_cursor, context, parts, result);
    return;
  }

  case TransitionKind::ConsumeNodeAndHoldStep: {
    // Node matched the current repeating step; consume it and keep the same
    // step active for the next neighbour. Propagate the updated match count so
    // subsequent layers know how many nodes have been consumed so far.
    context.visited.insert(cursor.current_id);
    context.current_chain.push_back(cursor.current_id);
    cursor.progress = transition.progress;
    break;
  }

  case TransitionKind::ConsumeNodeAndAdvanceStep: {
    // Node matched a Sequence step; consume it and advance to the next step.
    context.visited.insert(cursor.current_id);
    context.current_chain.push_back(cursor.current_id);
    cursor.steps_it = std::next(cursor.steps_it);
    cursor.progress = {};
    break;
  }
  }

  /////////////////////////////////////////////////
  /// SECTION: Record valid subgraph if all steps are now satisfied
  ///
  /// This fires when ConsumeNodeAndAdvanceStep consumed the last Sequence step,
  /// advancing steps_it to steps_end. The subgraph is recorded immediately and
  /// we backtrack without iterating neighbours, since there is no remaining
  /// step to match against them.
  /////////////////////////////////////////////////
  if (cursor.steps_it == context.steps_end) {
    if (!result.valid_subgraph.has_value())
      result.valid_subgraph = context.current_chain;
    context.visited.erase(cursor.current_id);
    context.current_chain.pop_back();
    return;
  }

  /////////////////////////////////////////////////
  /// SECTION: Traverse neighbours
  ///
  /// cursor.steps_it and cursor.progress have been updated by the switch above
  /// and are forwarded to each child cursor so every neighbour continues from
  /// the correct step.
  /////////////////////////////////////////////////
  const SocketMap &sockets = std::visit(
      [](const auto &instance) -> const SocketMap & {
        return instance.sockets;
      },
      current_node->second);

  for (const auto &[socket_id, socket_data] : sockets) {
    if (!socket_data.connected_to)
      continue;

    const uint32_t neighbour_id = socket_data.connected_to->peer_part_id;
    if (context.visited.count(neighbour_id))
      continue;

    append_event(context,
                 make_moving_to_neighbour_event(
                     cursor.depth, cursor.current_id, socket_id, neighbour_id,
                     socket_data.connected_to->peer_socket_id, parts));

    Cursor child{};
    child.current_id = neighbour_id;
    child.steps_it = cursor.steps_it;
    child.progress = cursor.progress;
    child.depth = cursor.depth + 1;

    depth_first_search(child, context, parts, result);

    append_event(context, make_backtracking_event(
                              cursor.depth, neighbour_id,
                              socket_data.connected_to->peer_socket_id,
                              cursor.current_id, socket_id, parts));

    if (result.valid_subgraph.has_value())
      break;
  }

  /////////////////////////////////////////////////
  /// SECTION: Backtrack — unmark the current node before returning
  /////////////////////////////////////////////////
  context.visited.erase(cursor.current_id);
  context.current_chain.pop_back();
}
} // namespace steamrot::logic::descriptors
