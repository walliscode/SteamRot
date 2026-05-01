/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ChainDescriptorBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ChainDescriptorBuilder.h"

namespace steamrot::logic::descriptors {
/////////////////////////////////////////////////
ChainDescriptorBuilder &ChainDescriptorBuilder::StartWith(NodeDescriptor nd) {
  m_steps.push_back(std::move(nd));
  return *this;
}

/////////////////////////////////////////////////
ChainDescriptorBuilder &ChainDescriptorBuilder::Then(NodeDescriptor nd) {
  m_steps.push_back(std::move(nd));
  return *this;
}

/////////////////////////////////////////////////
ChainDescriptor ChainDescriptorBuilder::End(NodeDescriptor nd) {
  m_steps.push_back(std::move(nd));
  // TODO: implement DFS traversal over the PartGraph using m_steps.
  // Walk starting from the anchor node, following edges, and verify each
  // visited node against the corresponding step predicate in order.
  // Return true only when a complete path matching all steps is found.
  return [steps = std::move(m_steps)](const PartGraph & /*graph*/,
                                      const PartNode & /*start*/) -> ChainDescriptorResult {
    (void)steps;
    return ChainDescriptorResult{false};
  };
}
} // namespace steamrot::logic::descriptors
