/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the PartNode, PartEdge, and PartGraph structs.
///
/// These types provide a non-owning, read-only view of a
/// MachinaFormScaffold as a graph. All pointers reference storage owned
/// by the scaffold; the scaffold must outlive any PartGraph derived from it.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "MachinaFormScaffold.h"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <variant>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
/// @struct PartNode
/// @brief Non-owning view of a single entry in a PartMap.
///
/// @c instance points directly at the
/// @c std::variant<JointInstance, FragmentInstance> stored in the PartMap.
/// Use @c std::holds_alternative or @c std::visit on @c *instance to
/// distinguish type without an additional cast layer.
///
/// The PartMap that owns the referenced variant must outlive this node.
/////////////////////////////////////////////////
struct PartNode {
  /////////////////////////////////////////////////
  /// @brief Stable ID of the referenced instance.
  /////////////////////////////////////////////////
  uint32_t id{0};

  /////////////////////////////////////////////////
  /// @brief Non-owning pointer to the variant entry in the source PartMap.
  /////////////////////////////////////////////////
  const std::variant<JointInstance, FragmentInstance> *instance{nullptr};

  /////////////////////////////////////////////////
  /// @brief Indices into @c PartGraph::edges for every edge incident to this
  ///        node. Populated by @c build_part_graph; provides O(degree)
  ///        neighbour access without scanning the full edge list.
  /////////////////////////////////////////////////
  std::vector<size_t> edge_indices;

  /////////////////////////////////////////////////
  /// @brief Construct a PartNode from a PartMap entry.
  ///
  /// @param id       Stable ID of the instance.
  /// @param variant  Const reference to the PartMap value; the address is
  ///                 stored. The PartMap must outlive this node.
  /////////////////////////////////////////////////
  explicit PartNode(
      uint32_t id, const std::variant<JointInstance, FragmentInstance> &variant)
      : id{id}, instance{&variant} {}
};

/////////////////////////////////////////////////
/// @struct PartEdge
/// @brief Non-owning view of a single Connection in a MachinaFormScaffold.
///
/// Mirrors the endpoint fields of @c Connection for direct access.
/// The source @c MachinaFormScaffold must outlive this edge.
/////////////////////////////////////////////////
struct PartEdge {
  /////////////////////////////////////////////////
  /// @brief Stable ID of the first endpoint's part instance.
  /////////////////////////////////////////////////
  uint32_t part_id_a{0};

  /////////////////////////////////////////////////
  /// @brief Socket index within the first endpoint's instance.
  /////////////////////////////////////////////////
  size_t socket_index_a{0};

  /////////////////////////////////////////////////
  /// @brief Stable ID of the second endpoint's part instance.
  /////////////////////////////////////////////////
  uint32_t part_id_b{0};

  /////////////////////////////////////////////////
  /// @brief Socket index within the second endpoint's instance.
  /////////////////////////////////////////////////
  size_t socket_index_b{0};
};

/////////////////////////////////////////////////
/// @struct PartGraph
/// @brief Lightweight non-owning snapshot of a MachinaFormScaffold as a
/// graph.
///
/// All pointers in nodes and edges refer into the scaffold that was passed
/// to @c build_part_graph. The scaffold must outlive the PartGraph.
/////////////////////////////////////////////////
struct PartGraph {
  /////////////////////////////////////////////////
  /// @brief One node per entry in the scaffold's PartMap.
  /////////////////////////////////////////////////
  std::vector<PartNode> nodes;

  /////////////////////////////////////////////////
  /// @brief One edge per Connection in the scaffold.
  /////////////////////////////////////////////////
  std::vector<PartEdge> edges;

  /////////////////////////////////////////////////
  /// @brief Maps each part ID to its index in @c nodes.
  ///
  /// Populated by @c build_part_graph. Enables O(1) node lookup by ID
  /// without scanning the full @c nodes vector.
  /////////////////////////////////////////////////
  std::unordered_map<uint32_t, size_t> node_index_by_id;
};

/////////////////////////////////////////////////
/// @brief Predicate type for single-node queries on a PartGraph.
///
/// Any callable with signature @c bool(const PartNode&) qualifies.
/// @c is_fragment, @c is_joint, and @c has_available_socket in
/// @c steamrot::logic::analysis::grimoire_machina are declared as
/// @c const @c NodeDescriptor variables and can be used directly or
/// assigned to other @c NodeDescriptor instances.
///
/// Example:
/// @code
/// NodeDescriptor predicate = agm::is_fragment;
/// bool result = predicate(node);
/// @endcode
/////////////////////////////////////////////////
using NodeDescriptor = std::function<bool(const PartNode &)>;

/////////////////////////////////////////////////
/// @brief Predicate type for single-edge queries on a PartGraph.
///
/// Any callable with signature @c bool(const PartEdge&) qualifies.
/// Use @c connects_fragments, @c connects_joints, or
/// @c connects_fragment_to_joint factory functions to obtain ready-made
/// descriptors for common edge-type queries.
/////////////////////////////////////////////////
using EdgeDescriptor = std::function<bool(const PartEdge &)>;

/////////////////////////////////////////////////
/// @brief Predicate for a single node with access to the whole graph.
///
/// Any callable with signature
/// @c bool(const PartGraph&, const PartNode&) qualifies.
/// Use when the predicate needs to examine neighbouring nodes via the graph
/// but does not need to walk further than one hop from the anchor.
///
/// Obtain instances from the modifier free functions in
/// @c steamrot::descriptors (see @c descriptor_ops.h in @c src/logic/).
/////////////////////////////////////////////////
using ContextualNodeDescriptor =
    std::function<bool(const PartGraph &, const PartNode &)>;

/////////////////////////////////////////////////
/// @brief Predicate for a multi-hop walk starting from one anchor node.
///
/// Shares the underlying signature with @c ContextualNodeDescriptor.
/// The distinction is semantic: a @c ChainDescriptor answers a structural
/// question about a subgraph reachable from the start node (e.g. "does a
/// 3-node linear chain begin here?") rather than a question about the
/// start node alone.
///
/// Build instances with @c steamrot::logic::ChainDescriptorBuilder
/// (see @c ChainDescriptorBuilder.h in @c src/logic/).
/////////////////////////////////////////////////
using ChainDescriptor =
    std::function<bool(const PartGraph &, const PartNode &)>;

/////////////////////////////////////////////////
/// @brief Predicate for the whole graph with no anchor node.
///
/// Any callable with signature @c bool(const PartGraph&) qualifies.
/// Derive instances via @c steamrot::descriptors::any_node_satisfies() or
/// @c steamrot::descriptors::all_nodes_satisfy() in
/// @c src/logic/descriptor_ops.h.
///
/// @c GraphDescriptor is the terminal type in the hierarchy — it is only
/// consumed by Logic/Action classes and is never passed back into a modifier.
/////////////////////////////////////////////////
using GraphDescriptor = std::function<bool(const PartGraph &)>;

} // namespace steamrot
