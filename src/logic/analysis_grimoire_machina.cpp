/////////////////////////////////////////////////
/// @file
/// @brief Implementation of analysis utilities for a MachinaFormScaffold
/// as a PartGraph, including free functions and NodeDescriptor predicates.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "analysis_grimoire_machina.h"
#include <variant>

namespace steamrot::logic::analysis::grimoire_machina {

/////////////////////////////////////////////////
PartGraph build_part_graph(const MachinaFormScaffold &scaffold) {
  PartGraph graph;
  graph.nodes.reserve(scaffold.parts.size());
  graph.edges.reserve(scaffold.connections.size());

  for (const auto &[id, variant] : scaffold.parts)
    graph.nodes.emplace_back(id, variant);

  for (const auto &connection : scaffold.connections)
    graph.edges.emplace_back(connection);

  return graph;
}

/////////////////////////////////////////////////
const NodeDescriptor is_fragment = [](const PartNode &node) -> bool {
  return std::holds_alternative<FragmentInstance>(*node.instance);
};

/////////////////////////////////////////////////
const NodeDescriptor is_joint = [](const PartNode &node) -> bool {
  return std::holds_alternative<JointInstance>(*node.instance);
};

/////////////////////////////////////////////////
const NodeDescriptor has_available_socket = [](const PartNode &node) -> bool {
  return std::visit(
      [](const auto &instance) -> bool {
        for (const auto &socket : instance.sockets) {
          if (socket.state == SocketState::Available)
            return true;
        }
        return false;
      },
      *node.instance);
};

/////////////////////////////////////////////////
const NodeDescriptor has_maximum_n_connected_sockets(size_t n) {

  // take in the PartNode parameter, this is is necesary to match the return
  // type of NodeDescriptor
  return [n](const PartNode &node) -> bool {
    // compile time dispatch on the instance variant which will check for
    // .sockets member
    return std::visit(
        [n](const auto &instance) -> bool {
          // iterate over the sockets and count how many are connected, then
          // compare to n
          size_t connected_count = 0;
          for (const auto &socket : instance.sockets) {
            if (socket.state == SocketState::Connected)
              connected_count++;
          }
          // return true if the number of connected sockets is less than or
          // equal to n
          return connected_count <= n;
        },
        *node.instance);
  };
};

/////////////////////////////////////////////////
const NodeDescriptor is_terminal = has_maximum_n_connected_sockets(1);

} // namespace steamrot::logic::analysis::grimoire_machina
