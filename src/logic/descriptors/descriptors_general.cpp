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
  // Reserve space for all nodes and edges to avoid reallocations that would
  // invalidate pointers into the scaffold.
  graph.nodes.reserve(scaffold.parts.size());
  graph.node_index_by_id.reserve(scaffold.parts.size());
  graph.edges.reserve(scaffold.connections.size());

  // cycle the scaffold's PartMap to create one node per part, populating
  // node_index_by_id
  for (const auto &[id, variant] : scaffold.parts) {

    // .size() grows as we emplace nodes, so it always points to the next index
    // to insert at. .rserve() does not effect the return value of .size(), so
    // we can safely call it before the loop.
    graph.node_index_by_id[id] = graph.nodes.size();
    graph.nodes.emplace_back(id, variant);
  }
  // cycle the scaffold's connections to create one edge per connection, pushing
  // // the edge index into both endpoint nodes' edge_indices
  for (const auto &c : scaffold.connections) {
    const size_t edge_idx = graph.edges.size();

    // create a new PartEdge and push it into the graph's edges vector.
    graph.edges.push_back({c.socket_a.part_id, c.socket_a.socket_index,
                           c.socket_b.part_id, c.socket_b.socket_index});

    // get the node id socket A and push the edge index into its edge_indices
    // vector.
    auto it_a = graph.node_index_by_id.find(c.socket_a.part_id);
    if (it_a != graph.node_index_by_id.end())
      graph.nodes[it_a->second].edge_indices.push_back(edge_idx);

    // get the node id socket B and push the edge index into its edge_indices
    // vector.
    auto it_b = graph.node_index_by_id.find(c.socket_b.part_id);
    if (it_b != graph.node_index_by_id.end())
      graph.nodes[it_b->second].edge_indices.push_back(edge_idx);
  }

  return graph;
}
} // namespace steamrot::logic::descriptors
