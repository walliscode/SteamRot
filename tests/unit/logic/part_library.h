/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the TestPartLibrary struct and PartLibraryBuilder
///        class for test infrastructure.
///
/// TestPartLibrary is a value-type struct that holds a small, fixed set of
/// pre-defined Fragment and Joint objects keyed by name. Instantiate it via
/// TestPartLibrary::Create() to get the standard catalog. The catalog also
/// exposes a scaffold_scenarios map (keyed by ScaffoldScenario) of pre-wired
/// MachinaFormScaffold topologies for use in NodeDescriptor tests.
///
/// ConnectionSpec describes one connection between two named part aliases,
/// allowing tests to stay readable while IDs remain deterministic.
///
/// ScaffoldResult bundles a MachinaFormScaffold with alias↔ID mappings and
/// insertion-order lists for robust test lookups.
///
/// ScaffoldScenarioExpectations holds one std::array<bool, N> per
/// ScaffoldScenario (N equals the node count for that scenario) so callers can
/// express exact per-node expected results in a compact, type-safe way.
/// Node order within each array matches insertion order (fragments first, then
/// joints) as documented on each field.
///
/// CheckNodeDescriptorForAllScenarios runs a NodeDescriptor against every
/// ScaffoldScenario in the library and CHECKs each node's result against the
/// corresponding entry in a ScaffoldScenarioExpectations value.
///
/// PartLibraryBuilder wraps a TestPartLibrary reference and provides
/// convenience methods to create FragmentInstances, JointInstances, PartGraphs,
/// and fully-connected scaffolds backed by the library's storage.
///
/// ⚠ Instances produced by PartLibraryBuilder hold raw pointers into the
/// TestPartLibrary they were built from. The library must outlive any
/// instances, PartGraphs, or ScaffoldResults it produces. This applies to the
/// pre-built scaffold_scenarios as well — do not extract a scenario from the
/// library and use it after the library is destroyed, as scenarios hold raw
/// pointers into the library's part storage.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Fragment.h"
#include "Joint.h"
#include "MachinaFormScaffold.h"
#include "descriptors_node_descriptors.h"
#include <SFML/Graphics/Transform.hpp>
#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @enum ScaffoldScenario
/// @brief Identifies a pre-wired MachinaFormScaffold topology stored in
///        TestPartLibrary::scaffold_scenarios.
///
/// Use as the key to TestPartLibrary::scaffold_scenarios:
/// @code
/// const MachinaFormScaffold& chain =
///     lib.scaffold_scenarios.at(ScaffoldScenario::LinearChain);
/// @endcode
/////////////////////////////////////////////////
enum class ScaffoldScenario {
  /////////////////////////////////////////////////
  /// fragment_two_sockets ─ joint_two_sockets ─ fragment_two_sockets;
  /// both connections wired, all involved sockets Connected.
  /////////////////////////////////////////////////
  LinearChain,

  /////////////////////////////////////////////////
  /// Three joint_two_sockets in a cycle; all sockets Connected.
  /////////////////////////////////////////////////
  Ring,

  /////////////////////////////////////////////////
  /// Two fragment_one_sockets fully connected to each other.
  /////////////////////////////////////////////////
  IsolatedPair,

  /////////////////////////////////////////////////
  /// One Joint connected to one socket each of three fragments, creating a
  /// branching point
  /////////////////////////////////////////////////
  SimpleBranch

};

/////////////////////////////////////////////////
/// @struct TestPartLibrary
/// @brief Fixed catalog of named Fragment and Joint test definitions, plus
///        pre-wired scaffold topologies.
///
/// The catalog is pre-populated by TestPartLibrary::Create() with the
/// following named parts:
///
/// Fragments:
///   "fragment_no_socket"     — green triangle in Front view, no sockets
///   "fragment_one_socket"    — green origin triangle + one socket at (5, 5)
///   "fragment_two_sockets"   — white 20×20 square + sockets at (0, 10) and
///   (20, 10) "fragment_three_sockets" — white 20×20 square + sockets at (0,
///   10), (10, 10) and (20, 10)
///
/// Joints:
///   "joint_no_socket"      — blue triangle in Front view, no sockets
///   "joint_one_socket"     — 1 socket at radius 10, full rotation arc
///   "joint_two_sockets"    — 2 sockets at radius 15, full rotation arc
///   "joint_three_sockets"  — 3 sockets at radius 15, full rotation arc
///
/// Scaffold scenarios (pre-wired topologies for NodeDescriptor tests):
///   ScaffoldScenario::LinearChain  — fragment_two_sockets ─ joint_two_sockets
///   ─ fragment_two_sockets;
///                                    both connections wired, all involved
///                                    sockets Connected
///   ScaffoldScenario::Ring         — three joint_two_sockets in a cycle, all
///   sockets Connected ScaffoldScenario::IsolatedPair — two
///   fragment_one_sockets connected to each other ScaffoldScenario::Partial —
///   fragment_three_sockets with sockets 0 and 2 connected to
///                                    two joint_one_sockets; socket 1 remains
///                                    Available
/////////////////////////////////////////////////
struct TestPartLibrary {
  /////////////////////////////////////////////////
  /// @brief Named Fragment definitions.
  /////////////////////////////////////////////////
  std::map<std::string, Fragment> fragments;

  /////////////////////////////////////////////////
  /// @brief Named Joint definitions.
  /////////////////////////////////////////////////
  std::map<std::string, Joint> joints;

  /////////////////////////////////////////////////
  /// @brief Pre-wired MachinaFormScaffold topologies keyed by ScaffoldScenario.
  ///
  /// All instances inside each scenario hold raw pointers into @c fragments
  /// and @c joints above. The scenarios are only valid while this
  /// TestPartLibrary object is alive and unmoved.
  /////////////////////////////////////////////////
  std::map<ScaffoldScenario, MachinaFormScaffold> scaffold_scenarios;

  /////////////////////////////////////////////////
  /// @brief Create a TestPartLibrary pre-populated with the standard catalog
  ///        and scaffold scenarios.
  ///
  /// @return TestPartLibrary containing the predefined Fragments, Joints,
  ///         and pre-wired scaffold scenarios.
  /////////////////////////////////////////////////
  static TestPartLibrary Create();
};

/////////////////////////////////////////////////
/// @struct NamedPartSpec
/// @brief Declares one scaffold instance using an explicit alias and a library
///        part name.
///
/// @c alias identifies this concrete instance within a scaffold build and must
/// be unique across both fragment and joint declarations for that build.
///
/// @c part_name must match an entry in TestPartLibrary::fragments or
/// TestPartLibrary::joints depending on which declaration list it appears in.
/////////////////////////////////////////////////
struct NamedPartSpec {
  /////////////////////////////////////////////////
  /// @brief Unique instance alias used by connections and trace builders.
  /////////////////////////////////////////////////
  std::string alias{};

  /////////////////////////////////////////////////
  /// @brief Source part name in the test library catalog.
  /////////////////////////////////////////////////
  std::string part_name{};
};

///////////////////////////////////////////////
/// @struct ConnectionSpec
/// @brief Describes a single connection between two named part aliases.
///////////////////////////////////////////////
struct ConnectionSpec {
  /////////////////////////////////////////////////
  /// @brief Alias of the first endpoint part.
  /////////////////////////////////////////////////
  std::string from_alias{};

  /////////////////////////////////////////////////
  /// @brief Stable socket ID on the first part's sockets map.
  /////////////////////////////////////////////////
  uint32_t socket_id_a{0};

  /////////////////////////////////////////////////
  /// @brief Alias of the second endpoint part.
  /////////////////////////////////////////////////
  std::string to_alias{};

  /////////////////////////////////////////////////
  /// @brief Stable socket ID on the second part's sockets map.
  /////////////////////////////////////////////////
  uint32_t socket_id_b{0};
};

/////////////////////////////////////////////////
/// @struct ScaffoldResult
/// @brief Bundles a MachinaFormScaffold with alias/ID mapping artifacts.
///
/// All raw pointers in @c scaffold point into the TestPartLibrary used to
/// build it. The library must outlive the ScaffoldResult.
/////////////////////////////////////////////////
struct ScaffoldResult {
  /////////////////////////////////////////////////
  /// @brief The built scaffold, optionally pre-wired with connections.
  /////////////////////////////////////////////////
  MachinaFormScaffold scaffold;

  /////////////////////////////////////////////////
  /// @brief Stable part IDs in insertion order (fragments first, then joints).
  ///
  /// Maintained for compatibility with legacy tests.
  /////////////////////////////////////////////////
  std::vector<uint32_t> part_ids;

  /////////////////////////////////////////////////
  /// @brief Part aliases in insertion order (fragments first, then joints).
  /////////////////////////////////////////////////
  std::vector<std::string> ordered_aliases;

  /////////////////////////////////////////////////
  /// @brief Map from instance alias to stable part ID.
  /////////////////////////////////////////////////
  std::map<std::string, uint32_t> alias_to_id;

  /////////////////////////////////////////////////
  /// @brief Reverse map from stable part ID to instance alias.
  /////////////////////////////////////////////////
  std::map<uint32_t, std::string> id_to_alias;
};

/////////////////////////////////////////////////
/// @struct ScaffoldScenarioExpectations
/// @brief Holds exact per-node expected results for every ScaffoldScenario.
///
/// Each field is a fixed-size array of booleans whose length equals the node
/// count of the corresponding scenario.  Array indices follow insertion order
/// (fragments first, then joints) as built by TestPartLibrary::Create():
///
///   linear_chain[0] — frag0 (fragment_two_sockets, socket[1] connected)
///   linear_chain[1] — frag1 (fragment_two_sockets, socket[0] connected)
///   linear_chain[2] — joint0 (joint_two_sockets, both sockets connected)
///
///   ring[0] — joint0 (joint_two_sockets, sockets[0] and [1] connected)
///   ring[1] — joint1 (joint_two_sockets, sockets[0] and [1] connected)
///   ring[2] — joint2 (joint_two_sockets, sockets[0] and [1] connected)
///
///   isolated_pair[0] — frag0 (fragment_one_socket, socket[0] connected)
///   isolated_pair[1] — frag1 (fragment_one_socket, socket[0] connected)
///
/// Pass a value of this type to CheckNodeDescriptorForAllScenarios together
/// with the NodeDescriptor under test.
/////////////////////////////////////////////////
struct ScaffoldScenarioExpectations {
  /////////////////////////////////////////////////
  /// @brief Expected results for ScaffoldScenario::LinearChain.
  ///
  /// Indices: [0]=frag0, [1]=frag1, [2]=joint0.
  /////////////////////////////////////////////////
  std::array<bool, 3> linear_chain{};

  /////////////////////////////////////////////////
  /// @brief Expected results for ScaffoldScenario::Ring.
  ///
  /// Indices: [0]=joint0, [1]=joint1, [2]=joint2.
  /////////////////////////////////////////////////
  std::array<bool, 3> ring{};

  /////////////////////////////////////////////////
  /// @brief Expected results for ScaffoldScenario::IsolatedPair.
  ///
  /// Indices: [0]=frag0, [1]=frag1.
  /////////////////////////////////////////////////
  std::array<bool, 2> isolated_pair{};

  /////////////////////////////////////////////////
  /// @brief Expected results for ScaffoldScenario::SimpleBranch.
  ///
  /// Indices: [0]=frag0, [1]=frag1, [2]=frag2, [3]=joint0.
  /////////////////////////////////////////////////
  std::array<bool, 4> simple_branch{};
};

/////////////////////////////////////////////////
/// @brief Run a NodeDescriptor against every ScaffoldScenario and assert
///        exact per-node results.
///
/// Iterates each pre-wired scenario in @p lib, applies @p descriptor to every
/// part, and CHECKs the result against the corresponding entry in @p expected.
/// A REQUIRE guards that each scaffold's part count matches the expected array
/// size before the per-node CHECKs run, so a mismatch is reported immediately
/// rather than causing an out-of-bounds access.
///
/// Must be called from within a Catch2 TEST_CASE (or SECTION) because it
/// uses the CHECK and REQUIRE macros.
///
/// @param descriptor NodeDescriptor predicate to exercise.
/// @param expected   Exact expected result for every node in every scenario.
/// @param lib        TestPartLibrary whose scaffold_scenarios are used.
/////////////////////////////////////////////////
void CheckNodeDescriptorForAllScenarios(
    const steamrot::logic::descriptors::NodeDescriptor &descriptor,
    const ScaffoldScenarioExpectations &expected, const TestPartLibrary &lib);

/////////////////////////////////////////////////
/// @class PartLibraryBuilder
/// @brief Creates instances and PartGraphs from a TestPartLibrary.
///
/// Wraps a TestPartLibrary reference and exposes convenience methods for
/// constructing FragmentInstances, JointInstances, PartGraphs, and
/// fully-connected scaffolds in tests. IDs are reset to 0 at the start of each
/// scaffold/graph build call so every new scaffold starts from part ID 0.
///
/// Usage example:
/// @code
/// TestPartLibrary lib = TestPartLibrary::Create();
/// PartLibraryBuilder builder{lib};
///
/// // Look up individual parts
/// const Fragment& f = lib.fragments.at("fragment_one_socket");
///
/// // Create instances (raw pointers into lib — lib must outlive instances)
/// FragmentInstance fi = builder.MakeFragmentInstance("fragment_one_socket");
/// JointInstance    ji = builder.MakeJointInstance("joint_two_sockets");
///
/// // Build a PartGraph from a named subset
/// PartGraph parts = builder.MakePartGraph({"fragment_no_socket"},
///                                     {"joint_one_socket"});
///
/// // Build a scaffold with connections (no ID hunting, no socket patching)
/// ScaffoldResult result = builder.MakeConnectedScaffold(
///     {{"frag0", "fragment_two_sockets"}, {"frag1", "fragment_two_sockets"}},
///     {{"joint0", "joint_two_sockets"}},
///     {{"frag0", 1, "joint0", 0}, {"joint0", 1, "frag1", 0}});
///
/// // Or use a pre-built scenario
/// const MachinaFormScaffold& chain =
///     lib.scaffold_scenarios.at(ScaffoldScenario::LinearChain);
/// @endcode
/////////////////////////////////////////////////
class PartLibraryBuilder {

private:
  /////////////////////////////////////////////////
  /// @brief Reference to the library this builder draws from.
  /////////////////////////////////////////////////
  TestPartLibrary &m_library;

  /////////////////////////////////////////////////
  /// @brief Counter for assigning instance IDs during the current build.
  /////////////////////////////////////////////////
  uint32_t m_next_id{0};

  /////////////////////////////////////////////////
  /// @brief Build a scaffold and record insertion-order part IDs.
  ///
  /// Shared implementation used by MakeScaffoldWithParts and
  /// MakeConnectedScaffold.
  ///
  /// @param fragment_specs Named Fragment instance declarations.
  /// @param joint_specs    Named Joint instance declarations.
  /// @return ScaffoldResult with no connections and IDs in insertion order.
  /////////////////////////////////////////////////
  ScaffoldResult
  BuildScaffoldWithIds(const std::vector<NamedPartSpec> &fragment_specs,
                       const std::vector<NamedPartSpec> &joint_specs);

public:
  /////////////////////////////////////////////////
  /// @brief Construct a builder that draws from the given library.
  ///
  /// @param library TestPartLibrary to use as the part source.
  /////////////////////////////////////////////////
  explicit PartLibraryBuilder(TestPartLibrary &library);

  /////////////////////////////////////////////////
  /// @brief Create a FragmentInstance backed by the named Fragment.
  ///
  /// The returned instance holds a raw pointer into the library's storage.
  /// The library must outlive the instance.
  ///
  /// @param name              Key of the Fragment to instantiate.
  /// @param initial_transform World-space transform for the instance.
  /// @return A new FragmentInstance referencing the stored Fragment.
  /////////////////////////////////////////////////
  FragmentInstance MakeFragmentInstance(
      const std::string &name,
      sf::Transform initial_transform = sf::Transform::Identity);

  /////////////////////////////////////////////////
  /// @brief Create a JointInstance backed by the named Joint.
  ///
  /// The returned instance holds a raw pointer into the library's storage.
  /// The library must outlive the instance.
  ///
  /// @param name              Key of the Joint to instantiate.
  /// @param initial_transform World-space transform for the instance.
  /// @return A new JointInstance referencing the stored Joint.
  /////////////////////////////////////////////////
  JointInstance
  MakeJointInstance(const std::string &name,
                    sf::Transform initial_transform = sf::Transform::Identity);

  /////////////////////////////////////////////////
  /// @brief Build a PartGraph from named Fragments and Joints in the library.
  ///
  /// Fragment instances are inserted first (in order), then Joint instances.
  /// IDs are reset to 0 at the start of each call, then assigned in insertion
  /// order within that call.
  ///
  /// All raw pointers in the returned instances point into the library's
  /// storage — the library must outlive the PartGraph.
  ///
  /// @param fragment_names Names of Fragments to include, in order.
  /// @param joint_names    Names of Joints to include, in order.
  /// @return PartGraph keyed by stable instance IDs.
  /////////////////////////////////////////////////
  PartGraph MakePartGraph(const std::vector<std::string> &fragment_names,
                      const std::vector<std::string> &joint_names);

  /////////////////////////////////////////////////
  /// @brief Build a MachinaFormScaffold pre-populated with instances but no
  /// connections.
  ///
  /// Fragment instances are added first (in order), then Joint instances.
  /// IDs are reset to 0 at the start of each call, then assigned in insertion
  /// order. The scaffold's @c next_id is advanced to match.
  ///
  /// All raw pointers in the instances point into the library's storage — the
  /// library must outlive the returned scaffold.
  ///
  /// Typical usage: call MakeScaffoldWithParts to obtain a scaffold, then set
  /// @c SocketData::connected_to on individual sockets to build connected or
  /// ring scenarios. For scenarios that need connections, prefer
  /// MakeConnectedScaffold which handles all wiring automatically.
  ///
  /// @param fragment_names Names of Fragments to include, in order.
  /// @param joint_names    Names of Joints to include, in order.
  /// @return MachinaFormScaffold containing the requested instances.
  /////////////////////////////////////////////////
  MachinaFormScaffold
  MakeScaffoldWithParts(const std::vector<std::string> &fragment_names,
                        const std::vector<std::string> &joint_names);

  /////////////////////////////////////////////////
  /// @brief Build a MachinaFormScaffold with instances and connections wired.
  ///
  /// Fragment instances are added first (in order), then Joint instances.
  /// Each instance is declared with an explicit alias, and each ConnectionSpec
  /// resolves endpoint aliases through the build's alias-to-ID map.
  ///
  /// For every connection in @p connections the method:
  ///   - Sets @c SocketData::connected_to on both endpoint sockets
  ///     (reciprocally) and marks them as @c SocketState::Connected.
  ///
  /// Fails the test (via FAIL) if aliases are duplicate/unknown, if a
  /// self-connection is requested, or if any socket ID is invalid.
  ///
  /// All raw pointers in the result point into the library's storage — the
  /// library must outlive the returned ScaffoldResult.
  ///
  /// @param fragment_specs Fragment instance declarations with aliases.
  /// @param joint_specs    Joint instance declarations with aliases.
  /// @param connections    Connections to wire, expressed as alias-based specs.
  /// @return ScaffoldResult containing the wired scaffold and insertion-order
  ///         alias/ID mappings.
  /////////////////////////////////////////////////
  ScaffoldResult
  MakeConnectedScaffold(const std::vector<NamedPartSpec> &fragment_specs,
                        const std::vector<NamedPartSpec> &joint_specs,
                        const std::vector<ConnectionSpec> &connections);

  /////////////////////////////////////////////////
  /// @brief Return a const reference to a pre-wired MachinaFormScaffold
  /// scenario from the library.
  ///
  ///  This is designed for analysis only, no modification required
  ///
  /// @param scenario Enum key for the desired scaffold scenario in the
  /// library's
  /// @return const reference to the requested MachinaFormScaffold scenario.
  /////////////////////////////////////////////////
  const MachinaFormScaffold &
  GetScenarioForAnalysis(ScaffoldScenario scenario) const;

  /////////////////////////////////////////////////
  /// @brief Return a reference to a pre-wired MachinaFormScaffold scenario from
  /// the library.
  ///
  /// @param scenario Enum key for the desired scaffold scenario in the
  /// library's
  /// @return reference to the requested MachinaFormScaffold scenario, which can
  /// be modified by the caller.
  /////////////////////////////////////////////////
  MachinaFormScaffold &GetScenarioForModification(ScaffoldScenario scenario);
};

} // namespace steamrot::tests
