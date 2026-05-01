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

} // namespace steamrot::logic::descriptors
