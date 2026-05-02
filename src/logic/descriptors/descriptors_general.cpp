/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the free functions for general-purpose
/// descriptors/helpers
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "descriptors_general.h"

namespace steamrot::logic::descriptors {
/////////////////////////////////////////////////
PartGraph build_part_graph(const MachinaFormScaffold &scaffold) {

  // PartGraph to build and return.
  PartGraph graph;
  // Reserve space for all nodes to avoid reallocations.
  graph.nodes.reserve(scaffold.parts.size());
  graph.node_index_by_id.reserve(scaffold.parts.size());

  // Create one node per part, populating node_index_by_id.
  for (const auto &[id, variant] : scaffold.parts) {
    graph.node_index_by_id[id] = graph.nodes.size();
    graph.nodes.emplace_back(id, variant);
  }

  // Derive edges from socket connected_to fields.
  // Emit each edge exactly once by only processing the side with the lower
  // part ID, then push the edge index into both endpoint nodes' edge_indices.
  for (const auto &[id, variant] : scaffold.parts) {
    const std::vector<SocketData> *sockets{nullptr};
    std::visit([&sockets](const auto &instance) { sockets = &instance.sockets; },
               variant);

    for (size_t socket_idx = 0; socket_idx < sockets->size(); ++socket_idx) {
      const SocketData &socket = (*sockets)[socket_idx];
      if (!socket.connected_to.has_value())
        continue;

      const uint32_t peer_id = socket.connected_to->peer_part_id;
      if (id >= peer_id)
        continue; // emit each edge only from the lower-id endpoint

      const size_t edge_idx = graph.edges.size();
      graph.edges.push_back(
          {id, socket_idx, peer_id, socket.connected_to->peer_socket_index});

      auto it_a = graph.node_index_by_id.find(id);
      if (it_a != graph.node_index_by_id.end())
        graph.nodes[it_a->second].edge_indices.push_back(edge_idx);

      auto it_b = graph.node_index_by_id.find(peer_id);
      if (it_b != graph.node_index_by_id.end())
        graph.nodes[it_b->second].edge_indices.push_back(edge_idx);
    }
  }

  return graph;
}
} // namespace steamrot::logic::descriptors
