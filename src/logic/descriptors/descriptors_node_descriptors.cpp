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
const NodeDescriptor is_fragment = [](const PartNode &node) -> bool {
  return std::holds_alternative<FragmentInstance>(*node.instance);
};

/////////////////////////////////////////////////
const NodeDescriptor is_joint = [](const PartNode &node) -> bool {
  return std::holds_alternative<JointInstance>(*node.instance);
};

/////////////////////////////////////////////////
NodeDescriptor has_exactly_n_edges(size_t n) {
  return [n](const PartNode &node) -> bool {
    return node.edge_indices.size() == n;
  };
}

/////////////////////////////////////////////////
const NodeDescriptor is_serial = has_exactly_n_edges(2);

/////////////////////////////////////////////////
NodeDescriptor has_minimum_n_edges(size_t n) {
  return [n](const PartNode &node) -> bool {
    return node.edge_indices.size() >= n;
  };
}

/////////////////////////////////////////////////
const NodeDescriptor is_branched = has_minimum_n_edges(3);

/////////////////////////////////////////////////
NodeDescriptor has_maximum_n_edges(size_t n) {
  return [n](const PartNode &node) -> bool {
    return node.edge_indices.size() <= n;
  };
}

/////////////////////////////////////////////////
const NodeDescriptor is_terminal = has_maximum_n_edges(1);
} // namespace steamrot::logic::descriptors
