/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the PartGraphBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "MachinaFormScaffold.h"
#include <unordered_map>
namespace steamrot::tests {

enum class FragmentNames { NoSocket, OneSocket, TwoSockets, ThreeSockets };

enum class JointNames { NoSocket, OneSocket, TwoSockets, ThreeSockets };

/////////////////////////////////////////////////
/// @class PartGraphPackage
/// @brief All data necessary to test predicates against a PargGraph and
/// generate a trace
/////////////////////////////////////////////////
struct PartGraphPackage {
  /////////////////////////////////////////////////
  /// @brief stable ID generator for new parts added to the graph. Incremented
  /// by the builder for each new part added, ensuring unique IDs across both
  /// joints
  /////////////////////////////////////////////////
  uint32_t next_id{0};

  /////////////////////////////////////////////////
  /// @brief PartGraph containing Part Instances and their connections.
  /////////////////////////////////////////////////
  PartGraph part_graph;

  /////////////////////////////////////////////////
  /// @brief Mapping from user-friendly string IDs to stable uint32_t IDs in the
  /// PartGraph.
  /// used by the trace generator to translate the user-friendly IDs used in the
  /// test definitions
  /////////////////////////////////////////////////
  std::unordered_map<std::string, uint32_t> id_to_part_graph_id{};
};

class PartGraphBuilder {

private:
  PartGraphPackage m_package{};

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor. Initializes an empty PartGraphBuilder.
  /////////////////////////////////////////////////
  PartGraphBuilder() = default;

  /////////////////////////////////////////////////
  /// @brief Creates a FragmentInstance based on the provided FragmentNames enum
  /// value.
  ///
  /// @param name Enum value specifying the type of FragmentInstance to create.
  /// @return A FragmentInstance corresponding to the specified FragmentNames.
  /////////////////////////////////////////////////
  FragmentInstance MakeFragmentInstance(const FragmentNames name);

  /////////////////////////////////////////////////
  /// @brief Creates a JointInstance based on the provided JointNames enum
  /// value.
  ///
  /// @param name Enum value specifying the type of JointInstance to create.
  /// @return A JointInstance corresponding to the specified JointNames.
  /////////////////////////////////////////////////
  JointInstance MakeJointInstance(const JointNames name);

  PartGraphBuilder &AddFragmentInstance(const FragmentNames name,
                                        const std::string id);

  PartGraphBuilder &AddFragmentInstance(const Fragment &fragment,
                                        const std::string id);

  PartGraphBuilder &AddJointInstance(const JointNames name,
                                     const std::string id);

  PartGraphBuilder &AddJointInstance(const Joint &joint, const std::string id);

  PartGraphBuilder &Connect(const std::string &from_id,
                            const uint32_t from_socket_id,
                            const std::string &to_id,
                            const uint32_t to_socket_id);

  /////////////////////////////////////////////////
  /// @brief Connects two parts without enforcing the Fragment↔Joint type
  ///        restriction. Use for test topologies that deliberately wire
  ///        same-type pairs (e.g. joint↔joint ring cycles or
  ///        fragment↔fragment isolated pairs).
  ///
  /// @param from_id      User-friendly alias of the first part.
  /// @param from_socket_id Socket ID on the first part.
  /// @param to_id        User-friendly alias of the second part.
  /// @param to_socket_id Socket ID on the second part.
  /////////////////////////////////////////////////
  PartGraphBuilder &ConnectUnchecked(const std::string &from_id,
                                     const uint32_t from_socket_id,
                                     const std::string &to_id,
                                     const uint32_t to_socket_id);

  PartGraphPackage Build();
};

} // namespace steamrot::tests
