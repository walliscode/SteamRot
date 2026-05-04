/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the free functions for Node Descriptors
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "descriptors_node_descriptors.h"

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
const NodeDescriptor is_fragment =
    [](const PartGraph &parts, uint32_t id) -> NodeDescriptorResult {
  return NodeDescriptorResult{
      std::holds_alternative<FragmentInstance>(parts.at(id))};
};

/////////////////////////////////////////////////
const NodeDescriptor is_joint =
    [](const PartGraph &parts, uint32_t id) -> NodeDescriptorResult {
  return NodeDescriptorResult{
      std::holds_alternative<JointInstance>(parts.at(id))};
};

/////////////////////////////////////////////////
NodeDescriptor has_exactly_n_edges(size_t n) {
  return [n](const PartGraph &parts,
             uint32_t id) -> NodeDescriptorResult {
    const size_t count = std::visit(
        [](const auto &inst) -> size_t { return inst.connection_count; },
        parts.at(id));
    return NodeDescriptorResult{count == n};
  };
}

/////////////////////////////////////////////////
const NodeDescriptor is_serial = has_exactly_n_edges(2);

/////////////////////////////////////////////////
NodeDescriptor has_minimum_n_edges(size_t n) {
  return [n](const PartGraph &parts,
             uint32_t id) -> NodeDescriptorResult {
    const size_t count = std::visit(
        [](const auto &inst) -> size_t { return inst.connection_count; },
        parts.at(id));
    return NodeDescriptorResult{count >= n};
  };
}

/////////////////////////////////////////////////
const NodeDescriptor is_branched = has_minimum_n_edges(3);

/////////////////////////////////////////////////
NodeDescriptor has_maximum_n_edges(size_t n) {
  return [n](const PartGraph &parts,
             uint32_t id) -> NodeDescriptorResult {
    const size_t count = std::visit(
        [](const auto &inst) -> size_t { return inst.connection_count; },
        parts.at(id));
    return NodeDescriptorResult{count <= n};
  };
}

/////////////////////////////////////////////////
const NodeDescriptor is_terminal = has_maximum_n_edges(1);
} // namespace steamrot::logic::descriptors
