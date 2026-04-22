/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for analysing a
/// MachinaFormScaffold as a PartGraph.
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
bool is_fragment(const PartNode &node) {
  return std::holds_alternative<FragmentInstance>(*node.instance);
}

/////////////////////////////////////////////////
bool is_joint(const PartNode &node) {
  return std::holds_alternative<JointInstance>(*node.instance);
}

/////////////////////////////////////////////////
bool is_isolated(const PartNode &node, const PartGraph &graph) {
  for (const auto &edge : graph.edges) {
    if (edge.part_id_a == node.id || edge.part_id_b == node.id)
      return false;
  }
  return true;
}

/////////////////////////////////////////////////
size_t edge_count(const PartNode &node, const PartGraph &graph) {
  size_t count = 0;
  for (const auto &edge : graph.edges) {
    if (edge.part_id_a == node.id)
      ++count;
    if (edge.part_id_b == node.id)
      ++count;
  }
  return count;
}

/////////////////////////////////////////////////
size_t socket_count(const PartNode &node) {
  return std::visit(
      [](const auto &instance) -> size_t { return instance.sockets.size(); },
      *node.instance);
}

/////////////////////////////////////////////////
bool has_available_socket(const PartNode &node) {
  return std::visit(
      [](const auto &instance) -> bool {
        for (const auto &socket : instance.sockets) {
          if (socket.state == SocketState::Available)
            return true;
        }
        return false;
      },
      *node.instance);
}

} // namespace steamrot::logic::analysis::grimoire_machina
