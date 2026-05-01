/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the ChainDescriptorBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "PartGraph.h"
#include "descriptors_chain_descriptors.h"
#include "descriptors_node_descriptors.h"
#include <vector>

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
/// @enum ChainStepKind
/// @brief Identifies the flow-control behaviour of a single ChainStep.
///
/// Add new enumerators here to introduce additional flow-control strategies.
/// The builder's @c End() method dispatches on this value during DFS
/// traversal.
/////////////////////////////////////////////////
enum class ChainStepKind {
  /////////////////////////////////////////////////
  /// Consume exactly one node that satisfies the predicate.
  /// Used by @c StartWith(), @c Then(), and @c End().
  /////////////////////////////////////////////////
  Sequence,

  /////////////////////////////////////////////////
  /// Consume zero or more consecutive nodes that satisfy the predicate,
  /// then pass the first non-matching node to the next step.
  /// Used by @c WhileIsTrue().
  /////////////////////////////////////////////////
  WhileIsTrue,
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
};

/////////////////////////////////////////////////
/// @class ChainDescriptorBuilder
/// @brief Builds a ChainDescriptor from an ordered list of ChainSteps matched
///        against a DFS walk through the PartGraph.
///
/// Each step is added via @c StartWith(), @c Then(), @c WhileIsTrue(), and
/// finally @c End(), which finalises the step list and returns the descriptor.
///
/// Usage (from outside the `steamrot::logic` namespace):
/// @code
/// ChainDescriptor linear_3 =
///     steamrot::logic::descriptors::ChainDescriptorBuilder{}
///         .StartWith(is_terminal)
///         .Then(is_serial)
///         .End(is_terminal);
/// @endcode
///
/// Usage (from within `steamrot::logic::descriptors` or via a `using`
/// declaration):
/// @code
/// ChainDescriptor linear_3 =
///     ChainDescriptorBuilder{}
///         .StartWith(is_terminal)
///         .Then(is_serial)
///         .End(is_terminal);
///
/// ChainDescriptor variable_middle =
///     ChainDescriptorBuilder{}
///         .StartWith(is_terminal)
///         .WhileIsTrue(is_serial)
///         .End(is_terminal);
/// @endcode
/////////////////////////////////////////////////
class ChainDescriptorBuilder {

private:
  /////////////////////////////////////////////////
  /// @brief Ordered steps from StartWith() through End().
  /////////////////////////////////////////////////
  std::vector<ChainStep> m_steps{};

public:
  /////////////////////////////////////////////////
  /// @brief Set the predicate for the start node of the chain.
  ///
  /// Adds a @c ChainStepKind::Sequence step that must match exactly once.
  ///
  /// @param nd NodeDescriptor to apply to the first node.
  /// @return *this for method chaining.
  /////////////////////////////////////////////////
  ChainDescriptorBuilder &StartWith(NodeDescriptor nd);

  /////////////////////////////////////////////////
  /// @brief Append a predicate for the next node in the walk.
  ///
  /// Adds a @c ChainStepKind::Sequence step. May be called zero or more times
  /// between @c StartWith() and @c End().
  ///
  /// @param nd NodeDescriptor to apply to the next node in the walk.
  /// @return *this for method chaining.
  /////////////////////////////////////////////////
  ChainDescriptorBuilder &Then(NodeDescriptor nd);

  /////////////////////////////////////////////////
  /// @brief Append a repeating predicate that matches zero or more consecutive
  ///        nodes while @p nd holds.
  ///
  /// Adds a @c ChainStepKind::WhileIsTrue step. The DFS walk consumes nodes
  /// satisfying @p nd until the first node that does not satisfy it, then
  /// passes that node to the subsequent step.
  ///
  /// @param nd NodeDescriptor evaluated repeatedly during the walk.
  /// @return *this for method chaining.
  /////////////////////////////////////////////////
  ChainDescriptorBuilder &WhileIsTrue(NodeDescriptor nd);

  /////////////////////////////////////////////////
  /// @brief Append the predicate for the final node and build the descriptor.
  ///
  /// Adds a @c ChainStepKind::Sequence step and performs a DFS from the anchor
  /// node, matching each step in order. Returns a @c ChainDescriptor that
  /// evaluates to @c true when a path satisfying all steps is found.
  ///
  /// @param nd NodeDescriptor to apply to the last node.
  /// @return ChainDescriptor representing the full walk pattern.
  /////////////////////////////////////////////////
  ChainDescriptor End(NodeDescriptor nd);
};
} // namespace steamrot::logic::descriptors
