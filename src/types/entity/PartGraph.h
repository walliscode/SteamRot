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
  /// @brief Construct a PartNode from a PartMap entry.
  ///
  /// @param id       Stable ID of the instance.
  /// @param variant  Const reference to the PartMap value; the address is
  ///                 stored. The PartMap must outlive this node.
  /////////////////////////////////////////////////
  explicit PartNode(uint32_t id,
                    const std::variant<JointInstance, FragmentInstance> &variant)
      : id{id}, instance{&variant} {}
};

/////////////////////////////////////////////////
/// @struct PartEdge
/// @brief Non-owning view of a single Connection in a MachinaFormScaffold.
///
/// Mirrors the endpoint fields of @c Connection for quick access, and
/// stores a raw pointer back to the original Connection for full detail.
/// The source @c std::vector<Connection> must outlive this edge.
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

  /////////////////////////////////////////////////
  /// @brief Non-owning pointer to the original Connection.
  /////////////////////////////////////////////////
  const Connection *connection{nullptr};

  /////////////////////////////////////////////////
  /// @brief Construct a PartEdge from a Connection.
  ///
  /// @param c Connection to view. The connections vector must outlive this
  ///          edge.
  /////////////////////////////////////////////////
  explicit PartEdge(const Connection &c)
      : part_id_a{c.socket_a.part_id}, socket_index_a{c.socket_a.socket_index},
        part_id_b{c.socket_b.part_id}, socket_index_b{c.socket_b.socket_index},
        connection{&c} {}
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
};

} // namespace steamrot
