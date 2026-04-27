/////////////////////////////////////////////////
/// @file
/// @brief Implementation of analysis utilities for a MachinaFormScaffold
/// as a PartGraph, including free functions and NodeDescriptor predicates.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "analysis_grimoire_machina.h"
#include <queue>
#include <unordered_set>
#include <variant>

namespace steamrot::logic::analysis::grimoire_machina {

/////////////////////////////////////////////////
PartGraph build_part_graph(const MachinaFormScaffold &scaffold) {
  PartGraph graph;
  graph.nodes.reserve(scaffold.parts.size());
  graph.node_index_by_id.reserve(scaffold.parts.size());
  graph.edges.reserve(scaffold.connections.size());

  for (const auto &[id, variant] : scaffold.parts) {
    graph.node_index_by_id[id] = graph.nodes.size();
    graph.nodes.emplace_back(id, variant);
  }

  for (const auto &c : scaffold.connections) {
    const size_t edge_idx = graph.edges.size();
    graph.edges.push_back({c.socket_a.part_id, c.socket_a.socket_index,
                           c.socket_b.part_id, c.socket_b.socket_index});

    auto it_a = graph.node_index_by_id.find(c.socket_a.part_id);
    if (it_a != graph.node_index_by_id.end())
      graph.nodes[it_a->second].edge_indices.push_back(edge_idx);

    auto it_b = graph.node_index_by_id.find(c.socket_b.part_id);
    if (it_b != graph.node_index_by_id.end())
      graph.nodes[it_b->second].edge_indices.push_back(edge_idx);
  }

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
NodeDescriptor has_maximum_n_connected_sockets(size_t n) {
  return [n](const PartNode &node) -> bool {
    return std::visit(
        [n](const auto &instance) -> bool {
          size_t connected_count = 0;
          for (const auto &socket : instance.sockets) {
            if (socket.state == SocketState::Connected)
              connected_count++;
          }
          return connected_count <= n;
        },
        *node.instance);
  };
}

/////////////////////////////////////////////////
const NodeDescriptor is_terminal = has_maximum_n_connected_sockets(1);

/////////////////////////////////////////////////
EdgeDescriptor connects_fragments(const PartGraph &graph) {
  return [&graph](const PartEdge &edge) -> bool {
    const PartNode *a = find_node(graph, edge.part_id_a);
    const PartNode *b = find_node(graph, edge.part_id_b);
    return a && b && is_fragment(*a) && is_fragment(*b);
  };
}

/////////////////////////////////////////////////
EdgeDescriptor connects_joints(const PartGraph &graph) {
  return [&graph](const PartEdge &edge) -> bool {
    const PartNode *a = find_node(graph, edge.part_id_a);
    const PartNode *b = find_node(graph, edge.part_id_b);
    return a && b && is_joint(*a) && is_joint(*b);
  };
}

/////////////////////////////////////////////////
EdgeDescriptor connects_fragment_to_joint(const PartGraph &graph) {
  return [&graph](const PartEdge &edge) -> bool {
    const PartNode *a = find_node(graph, edge.part_id_a);
    const PartNode *b = find_node(graph, edge.part_id_b);
    if (!a || !b)
      return false;
    return (is_fragment(*a) && is_joint(*b)) ||
           (is_joint(*a) && is_fragment(*b));
  };
}

/////////////////////////////////////////////////
const PartNode *find_node(const PartGraph &graph, uint32_t id) {
  const auto it = graph.node_index_by_id.find(id);
  if (it == graph.node_index_by_id.end())
    return nullptr;
  return &graph.nodes[it->second];
}

/////////////////////////////////////////////////
std::vector<const PartEdge *> get_adjacent_edges(const PartGraph &graph,
                                                  const PartNode &node) {
  std::vector<const PartEdge *> result;
  result.reserve(node.edge_indices.size());
  for (const size_t idx : node.edge_indices)
    result.push_back(&graph.edges[idx]);
  return result;
}

/////////////////////////////////////////////////
std::vector<uint32_t> get_neighbor_ids(const PartGraph &graph,
                                        const PartNode &node) {
  std::vector<uint32_t> ids;
  ids.reserve(node.edge_indices.size());
  for (const size_t idx : node.edge_indices) {
    const PartEdge &edge = graph.edges[idx];
    ids.push_back(edge.part_id_a == node.id ? edge.part_id_b : edge.part_id_a);
  }
  return ids;
}

/////////////////////////////////////////////////
std::vector<const PartNode *> get_neighbors(const PartGraph &graph,
                                             const PartNode &node) {
  std::vector<const PartNode *> result;
  for (const uint32_t neighbor_id : get_neighbor_ids(graph, node)) {
    const PartNode *neighbor = find_node(graph, neighbor_id);
    if (neighbor)
      result.push_back(neighbor);
  }
  return result;
}

/////////////////////////////////////////////////
std::vector<const PartNode *>
find_nodes_matching(const PartGraph &graph, const NodeDescriptor &predicate) {
  std::vector<const PartNode *> result;
  for (const auto &node : graph.nodes)
    if (predicate(node))
      result.push_back(&node);
  return result;
}

/////////////////////////////////////////////////
size_t count_nodes_matching(const PartGraph &graph,
                             const NodeDescriptor &predicate) {
  size_t count = 0;
  for (const auto &node : graph.nodes)
    if (predicate(node))
      ++count;
  return count;
}

/////////////////////////////////////////////////
bool is_connected(const PartGraph &graph) {
  if (graph.nodes.empty())
    return true;

  std::unordered_set<uint32_t> visited;
  std::queue<uint32_t> to_visit;

  visited.insert(graph.nodes[0].id);
  to_visit.push(graph.nodes[0].id);

  while (!to_visit.empty()) {
    const uint32_t current_id = to_visit.front();
    to_visit.pop();

    const PartNode *current = find_node(graph, current_id);
    if (!current)
      continue;

    for (const uint32_t neighbor_id : get_neighbor_ids(graph, *current)) {
      if (visited.insert(neighbor_id).second)
        to_visit.push(neighbor_id);
    }
  }

  return visited.size() == graph.nodes.size();
}

/////////////////////////////////////////////////
void bfs(const PartGraph &graph, uint32_t start_id,
         std::function<void(const PartNode &)> visitor) {
  const PartNode *start = find_node(graph, start_id);
  if (!start)
    return;

  std::unordered_set<uint32_t> visited;
  std::queue<uint32_t> to_visit;

  visited.insert(start_id);
  to_visit.push(start_id);

  while (!to_visit.empty()) {
    const uint32_t current_id = to_visit.front();
    to_visit.pop();

    const PartNode *current = find_node(graph, current_id);
    if (!current)
      continue;

    visitor(*current);

    for (const uint32_t neighbor_id : get_neighbor_ids(graph, *current)) {
      if (visited.insert(neighbor_id).second)
        to_visit.push(neighbor_id);
    }
  }
}

} // namespace steamrot::logic::analysis::grimoire_machina
