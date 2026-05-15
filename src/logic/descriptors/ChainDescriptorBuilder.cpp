/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ChainDescriptorBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ChainDescriptorBuilder.h"
#include "DescriptorResult.h"
#include <string>
#include <unordered_set>
#include <vector>

namespace steamrot::logic::descriptors {
namespace {

/////////////////////////////////////////////////
/// @class DFSContext
/// @brief Mutable state threaded through the depth-first search.
///
/// Tracks visited nodes (cycle guard), the current candidate path
/// (current_chain), the DFS nesting depth used to stamp trace events, and
/// the accumulated AnalysisTrace that records every evaluation step.
/////////////////////////////////////////////////
struct DFSContext {
  /////////////////////////////////////////////////
  /// @brief Part IDs already on the current path (cycle guard).
  /////////////////////////////////////////////////
  std::unordered_set<uint32_t> visited;

  /////////////////////////////////////////////////
  /// @brief Ordered part IDs forming the current candidate subgraph path.
  /////////////////////////////////////////////////
  std::vector<uint32_t> current_chain;

  /////////////////////////////////////////////////
  /// @brief Whether at least one node has been consumed by a WhileIsTrue step.
  /////////////////////////////////////////////////
  bool at_least_one_while_loop_consumed{false};

  /////////////////////////////////////////////////
  /// @brief Current nesting depth for trace event stamping.
  ///
  /// Starts at 1 (anchor node level). Incremented before recursing into a
  /// neighbour and decremented after the recursive call returns.
  /////////////////////////////////////////////////
  uint32_t depth{1};

  /////////////////////////////////////////////////
  /// @brief Accumulated trace events for the whole chain evaluation.
  /////////////////////////////////////////////////
  AnalysisTrace trace;
};

/////////////////////////////////////////////////
/// @brief Recursive depth-first search over a PartGraph, matching an ordered
///        sequence of ChainStep predicates.
///
/// Emits NodeEval/NodeResult events (via NodeDescriptor::operator()),
/// MovingToNeighbour events before recursing, and Backtracking events after
/// each recursive call returns. All events land in @p context.trace.
///
/// @param steps_it    Iterator to the current step in the walk pattern.
/// @param steps_end   Past-the-end iterator for the steps sequence.
/// @param context     Mutable DFS state (visited set, chain, depth, trace).
/// @param current_id  Stable part ID of the node being evaluated.
/// @param parts       The PartGraph being traversed.
/// @param result      Accumulates matched and rejected subgraph ID lists.
/////////////////////////////////////////////////
void depth_first_search(std::vector<ChainStep>::const_iterator steps_it,
                        std::vector<ChainStep>::const_iterator steps_end,
                        DFSContext &context, uint32_t current_id,
                        const PartGraph &parts, ChainDescriptorResult &result) {

  /////////////////////////////////////////////////
  /// CHECKING END CONDITIONS
  /////////////////////////////////////////////////

  if (steps_it == steps_end) {
    // All N step predicates have been satisfied by the preceding N nodes.
    // current_id is the first node encountered AFTER all steps are matched;
    // it is not evaluated against any predicate and is not part of the chain.
    // valid_subgraphs records only the N nodes that passed the predicates.
    result.valid_subgraphs.push_back(context.current_chain);

    // emit ValidSubgraphIsolated event
    AnalysisEvent valid_event{};
    valid_event.kind = TraceEventKind::ValidSubgraphIsolated;
    valid_event.depth = context.depth;
    context.trace.push_back(std::move(valid_event));

    return;
  }

  if (context.visited.count(current_id))
    return;

  /////////////////////////////////////////////////
  /// EVALUATING CURRENT NODE
  /////////////////////////////////////////////////

  const NodeDescriptor &current_predicate = steps_it->predicate;

  // Evaluate the predicate; this stamps NodeEval + NodeResult into the result.
  auto pred_result = current_predicate(parts, current_id, context.depth);

  // pass the predicates trace as an R value, the context takes ownership of it
  // and merges it into the overall trace
  Merge(context.trace, std::move(pred_result.m_trace));

  if (!pred_result) {
    switch (steps_it->kind) {
    case ChainStepKind::Sequence: {
      result.invalid_subgraphs.push_back(context.current_chain);
      return;
    }
    case ChainStepKind::WhileIsTrue: {
      if (!context.at_least_one_while_loop_consumed) {
        result.invalid_subgraphs.push_back(context.current_chain);
        return;
      }
      // Reset the flag so that a subsequent WhileIsTrue step on this path
      // independently requires at least one consumption of its own.
      // Note: this flag is shared across all recursive calls for the current
      // DFS path — paths explored after backtracking start with the flag
      // already modified. A per-step design would be cleaner but requires
      // more invasive changes.
      context.at_least_one_while_loop_consumed = false;
      depth_first_search(std::next(steps_it), steps_end, context, current_id,
                         parts, result);
      return;
    }
    }
  }
  if (steps_it->kind == ChainStepKind::WhileIsTrue) {
    context.at_least_one_while_loop_consumed = true;
  }

  // Mark current node as visited and record it on the candidate path.
  context.visited.insert(current_id);
  context.current_chain.push_back(current_id);

  // Iterate connected neighbours.
  const auto part_it = parts.find(current_id);
  if (part_it == parts.end()) {
    result.invalid_subgraphs.push_back(context.current_chain);
    return;
  }
  const SocketMap &sockets = std::visit(
      [](const auto &inst) -> const SocketMap & { return inst.sockets; },
      part_it->second);

  for (const auto &[socket_id, socket] : sockets) {
    if (!socket.connected_to.has_value())
      continue;

    const uint32_t neighbour_id = socket.connected_to->peer_part_id;
    if (context.visited.count(neighbour_id))
      continue;

    // Emit MovingToNeighbour before descending.
    AnalysisEvent move_event{};
    move_event.kind = TraceEventKind::MovingToNeighbour;
    move_event.depth = context.depth;
    move_event.from_id = current_id;
    move_event.to_id = neighbour_id;
    move_event.socket_id = socket_id;
    context.trace.push_back(std::move(move_event));

    // Advance or stay on the current step, then recurse at increased depth.
    auto effective_steps_it = steps_it;
    switch (steps_it->kind) {
    case ChainStepKind::Sequence:
      effective_steps_it = std::next(steps_it);
      break;
    case ChainStepKind::WhileIsTrue:
      break;
    }

    ++context.depth;
    depth_first_search(effective_steps_it, steps_end, context, neighbour_id,
                       parts, result);
    --context.depth;

    // Emit Backtracking at the parent depth after the recursive call returns.
    AnalysisEvent back_event{};
    back_event.kind = TraceEventKind::Backtracking;
    back_event.depth = context.depth;
    back_event.from_id = neighbour_id;
    context.trace.push_back(std::move(back_event));
  }

  // Unmark and remove from candidate path before backtracking.
  context.current_chain.pop_back();
  context.visited.erase(current_id);
}
} // namespace

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
ChainDescriptor ChainDescriptorBuilder::Build(std::string name) {

  std::vector<ChainStep> steps = m_steps;

  return ChainDescriptor{
      name,
      [steps = std::move(steps), chain_name = std::move(name)](
          const PartGraph &parts, uint32_t start_id) -> ChainDescriptorResult {
        ChainDescriptorResult result{false};
        DFSContext context;

        // Check for empty PartGraph
        if (parts.empty()) {

          // this is automatically a failed result
          result.m_result = false;

          // set up EmtpyGraph event in the trace
          AnalysisEvent empty_graph_event{};
          empty_graph_event.kind = TraceEventKind::EmtpyPartGraph;
          context.trace.push_back(std::move(empty_graph_event));
          result.m_trace = std::move(context.trace);

          // return early since there's no graph to traverse
          return result;
        }

        // Check for empty steps
        if (steps.empty()) {

          // this is automatically a failed result since a chain with no steps
          // can't be satisfied
          result.m_result = false;

          // set up EmptyChainSteps event in the trace to explain why the result
          // is false
          AnalysisEvent empty_steps_event{};
          empty_steps_event.kind = TraceEventKind::EmtpyChainSteps;
          context.trace.push_back(std::move(empty_steps_event));
          result.m_trace = std::move(context.trace);

          // return early since there's no steps to evaluate
          return result;
        }

        AnalysisEvent scope_begin{};
        scope_begin.kind = TraceEventKind::ScopeBegin;
        scope_begin.depth = 0;
        scope_begin.scope_name = chain_name;
        scope_begin.scope_kind = ScopeKind::Chain;
        scope_begin.anchor_id = start_id;
        context.trace.push_back(std::move(scope_begin));

        depth_first_search(steps.cbegin(), steps.cend(), context, start_id,
                           parts, result);

        if (!result.valid_subgraphs.empty()) {
          result.m_result = true;
        }

        // ScopeEnd
        AnalysisEvent scope_end{};
        scope_end.kind = TraceEventKind::ScopeEnd;
        scope_end.depth = 0;
        scope_end.scope_name = chain_name;
        scope_end.scope_kind = ScopeKind::Chain;
        scope_end.result = result.m_result;
        context.trace.push_back(std::move(scope_end));

        result.m_trace = std::move(context.trace);
        return result;
      }};
}
} // namespace steamrot::logic::descriptors
