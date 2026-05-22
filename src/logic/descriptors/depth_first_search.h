/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions related to depth-first searches
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "descriptors_node_descriptors.h"
#include <unordered_set>

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
/// @enum ChainStepKind
/// @brief Identifies the flow-control behaviour of a single ChainStep.
///
/// Add new enumerators here to introduce additional flow-control strategies.
/// traversal.
/////////////////////////////////////////////////
enum class ChainStepKind {
  /////////////////////////////////////////////////
  /// Consume exactly one node that satisfies the predicate.
  /////////////////////////////////////////////////
  Sequence,

  /////////////////////////////////////////////////
  /// Consume at least one node that satisfies the predicate, and continue
  /// consuming as long as the predicate is satisfied. Once the predicate fails,
  /// stop consuming
  /// then pass the first non-matching node to the next step.
  /// Used by @c WhileIsTrue().
  /////////////////////////////////////////////////
  WhileIsTrue,

  //////////////////////////////////////////////////
  /// Consume at least n nodes that satisfy the predicate, and continue
  /// consuming as long as the
  /// predicate is satisfied. Once the predicate fails, stop consuming then pass
  /// the first non-matching node to the next step.
  /// Used by @c WhileIsTrueForN().
  ////////////////////////////////////////////////////
  WhileIsTrueForN
};

/////////////////////////////////////////////////
/// @struct ChainStep
/// @brief One step in the ordered walk pattern built by ChainDescriptorBuilder.
///
/// Pairs a node predicate with the flow-control strategy that governs how many
/// graph nodes it consumes during a DFS walk.
/////////////////////////////////////////////////
struct ChainStep {
  /////////////////////////////////////////////////
  /// @brief Predicate evaluated against each candidate node.
  /////////////////////////////////////////////////
  NodeDescriptor predicate;

  /////////////////////////////////////////////////
  /// @brief Flow-control behaviour for this step.
  /////////////////////////////////////////////////
  ChainStepKind kind{ChainStepKind::Sequence};

  /////////////////////////////////////////////////
  /// @brief used by While types to specify the minimum number of nodes that
  /// must satisfy the predicate
  /////////////////////////////////////////////////
  size_t min_repetitions{1};
};

struct StepProgress {
  size_t match_count{0};
};

/////////////////////////////////////////////////
/// enum that holds human readable flow control. This could be achieved with
/// booleans but this is more extensible and readable and prevents unwanted
/// behaviour
/////////////////////////////////////////////////
enum class TransitionKind {
  Reject,
  ConsumeNodeAndAdvanceStep,
  ConsumeNodeAndHoldStep,
  HoldNodeAndAdvanceStep
};

struct Transition {
  TransitionKind kind{TransitionKind::Reject};
  StepProgress progress{};
};

/////////////////////////////////////////////////
/// @class Cursor
/// @brief Designed to be created per DFS layer and hold state for that layer
/////////////////////////////////////////////////
struct Cursor {
  /////////////////////////////////////////////////
  /// @brief Current stable part ID being evaluated at this layer of the DFS.
  /////////////////////////////////////////////////
  uint32_t current_id{0};

  /////////////////////////////////////////////////
  /// @brief Iterator to the current step in the walk pattern
  /////////////////////////////////////////////////
  std::vector<ChainStep>::const_iterator steps_it;

  /////////////////////////////////////////////////
  /// @brief Progress towards satisfying the current step's predicate, used for
  /// steps that require multiple matches (e.g., WhileIsTrueForN).
  /////////////////////////////////////////////////
  StepProgress progress{};

  /////////////////////////////////////////////////
  /// @brief Depth of the current layer in the DFS, used for trace event
  /// stamping.
  /////////////////////////////////////////////////
  uint32_t depth{1};
};

/////////////////////////////////////////////////
/// @class DFSContext
/// @brief Mutable state threaded through the depth-first search.
///
/// Tracks visited nodes (cycle guard), the current candidate path
/// (current_chain), the DFS nesting depth used to stamp trace events, and
/// the accumulated AnalysisTrace that records every evaluation step.
/////////////////////////////////////////////////
struct DFSContext {

  DFSContext(std::vector<ChainStep> steps)
      : steps(std::move(steps)), steps_end(this->steps.cend()) {}

  /////////////////////////////////////////////////
  /// @brief Steps generated from the builder, governing the walk pattern of the
  /// DFS.
  /////////////////////////////////////////////////
  const std::vector<ChainStep> steps;

  /////////////////////////////////////////////////
  /// @brief A const iterator to the current step in the walk pattern.
  /////////////////////////////////////////////////
  const std::vector<ChainStep>::const_iterator steps_end;

  /////////////////////////////////////////////////
  /// @brief Part IDs already on the current path (cycle guard).
  /////////////////////////////////////////////////
  std::unordered_set<uint32_t> visited;

  /////////////////////////////////////////////////
  /// @brief Ordered part IDs forming the current candidate subgraph path.
  /////////////////////////////////////////////////
  std::vector<uint32_t> current_chain;

  /////////////////////////////////////////////////
  /// @brief Accumulated trace events for the whole chain evaluation.
  /////////////////////////////////////////////////
  AnalysisTrace trace;
};

/////////////////////////////////////////////////
/// @brief Resolves the transition to take in the DFS walk based on the current
/// step kind and other state
///
/// @param step_kind Enum identifying the flow-control strategy for the current
/// step in the walk.
/// @param predicate_result The result of evaluating the step's predicate
/// against the current node.
/// @param progress Any progress made towards satisfying the current step's
/// predicate, used for steps that require multiple matches (e.g.,
/// WhileIsTrueForN).
/// @return A transition struct indicating the next action for the DFS walk
/////////////////////////////////////////////////
Transition resolve_transition(const ChainStep &step,
                              const bool predicate_result,
                              StepProgress progress);
/////////////////////////////////////////////////
/// @brief Depth-first search over a PartGraph, matching an ordered sequence of
/// ChainSteps
///
/// @param cursor Cursor struct created for this layer of tstate for the layer
/// @param context Global DFS state (visited set, chain, depth, trace).
/// @param parts PartGraph being traversed.
/// @param result ChainDescriptorResult accumulating matched and rejected
/// subgraph ID lists.
/////////////////////////////////////////////////
void depth_first_search(Cursor cursor, DFSContext &context,
                        const PartGraph &parts, ChainDescriptorResult &result);

} // namespace steamrot::logic::descriptors
