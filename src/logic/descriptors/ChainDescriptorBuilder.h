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
#include "descriptors_chain_descriptors.h"
#include "descriptors_node_descriptors.h"
#include <expected>
#include <vector>

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
/////////////////////////////////////////////////
class ChainDescriptorBuilder {

private:
  /////////////////////////////////////////////////
  /// @brief Ordered steps from StartWith() through End().
  /////////////////////////////////////////////////
  std::vector<ChainStep> m_steps{};

  /////////////////////////////////////////////////
  /// @brief Flag to prevent modification of the builder after End() is called.
  /////////////////////////////////////////////////
  bool m_build_finalised{false};

public:
  /////////////////////////////////////////////////
  /// @brief Return an immutable reference to the steps added so far
  ///
  /// @return a const reference to the vector of ChainSteps added to this
  /// builder.
  /////////////////////////////////////////////////
  const std::vector<ChainStep> &GetSteps() const { return m_steps; }

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
  /// @brief Validates the state of the builder, just before building the
  /// descriptor.
  /////////////////////////////////////////////////
  std::string Validate() const;

  /////////////////////////////////////////////////
  /// @brief Returns a ChainDescriptor containing the steps added to this
  /// builder.
  ///
  /// The optional @p name is stamped on the descriptor and appears in
  /// ScopeBegin/ScopeEnd trace events. If omitted the descriptor is unnamed.
  ///
  /// If an error occurs during validation of the steps, returns a string
  /// describing the error instead.
  /////////////////////////////////////////////////
  std::expected<ChainDescriptor, std::string> Build(std::string name = "");
};
} // namespace steamrot::logic::descriptors
