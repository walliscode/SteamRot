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
#include "depth_first_search.h"
#include "descriptors_chain_descriptors.h"
#include "descriptors_node_descriptors.h"
#include <vector>

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
/// @class ChainDescriptorBuilder
/// @brief Builds a ChainDescriptor from an ordered list of ChainSteps matched
///        against a DFS walk through the PartGraph.
/////////////////////////////////////////////////
class ChainDescriptorBuilder {

private:
  /////////////////////////////////////////////////
  /// @brief Ordered steps from builder
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
  /// @brief Returns a ChainDescriptor containing the steps added to this
  /// builder.
  ///
  /// The optional @p name is stamped on the descriptor and appears in
  /// ScopeBegin/ScopeEnd trace events. If omitted the descriptor is unnamed.
  ///
  /// If an error occurs during validation of the steps, returns a string
  /// describing the error instead.
  /////////////////////////////////////////////////
  ChainDescriptor Build(std::string name = "");
};
} // namespace steamrot::logic::descriptors
