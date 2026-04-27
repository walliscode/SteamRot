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
/// ConnectionSpec describes one connection between two parts using
/// insertion-order indices (fragments first, then joints) so callers never
/// need to map-walk for stable IDs.
///
/// ScaffoldResult bundles a MachinaFormScaffold with the insertion-order ID
/// list, making it easy to wire additional connections programmatically.
///
/// PartLibraryBuilder wraps a TestPartLibrary reference and provides
/// convenience methods to create FragmentInstances, JointInstances, PartMaps,
/// and fully-connected scaffolds backed by the library's storage.
///
/// ⚠ Instances produced by PartLibraryBuilder hold raw pointers into the
/// TestPartLibrary they were built from. The library must outlive any
/// instances, PartMaps, or ScaffoldResults it produces. This applies to the
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
#include <SFML/Graphics/Transform.hpp>
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
/// @struct ConnectionSpec
/// @brief Describes a single connection between two socket endpoints using
///        insertion-order part indices.
///
/// Part indices are 0-based positions in the combined insertion order used
/// by PartLibraryBuilder: fragments first (in argument order), then joints
/// (in argument order). Using indices instead of raw IDs keeps test specs
/// resilient to the builder's internal ID counter value.
///
/// Example — connect fragment[0].socket[1] to joint[0].socket[0]
/// (assuming two fragments before the joint):
/// @code
/// ConnectionSpec spec{0, 1, 2, 0};
/// @endcode
/////////////////////////////////////////////////
struct ConnectionSpec {
  /////////////////////////////////////////////////
  /// @brief Insertion-order index of the first part.
  /////////////////////////////////////////////////
  size_t part_index_a{0};

  /////////////////////////////////////////////////
  /// @brief Index into the first part's sockets vector.
  /////////////////////////////////////////////////
  size_t socket_index_a{0};

  /////////////////////////////////////////////////
  /// @brief Insertion-order index of the second part.
  /////////////////////////////////////////////////
  size_t part_index_b{0};

  /////////////////////////////////////////////////
  /// @brief Index into the second part's sockets vector.
  /////////////////////////////////////////////////
  size_t socket_index_b{0};
};

/////////////////////////////////////////////////
/// @struct ScaffoldResult
/// @brief Bundles a MachinaFormScaffold with the insertion-order ID list.
///
/// @c part_ids holds the stable IDs assigned to each part in the order they
/// were inserted (fragments first, then joints). Use it to wire additional
/// Connections beyond what PartLibraryBuilder::MakeConnectedScaffold provides.
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
  /////////////////////////////////////////////////
  std::vector<uint32_t> part_ids;
};

/////////////////////////////////////////////////
/// @class PartLibraryBuilder
/// @brief Creates instances and PartMaps from a TestPartLibrary.
///
/// Wraps a TestPartLibrary reference and exposes convenience methods for
/// constructing FragmentInstances, JointInstances, PartMaps, and
/// fully-connected scaffolds in tests. Each builder maintains its own ID
/// counter so IDs are unique within a single builder's lifetime.
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
/// // Build a PartMap from a named subset
/// PartMap parts = builder.MakePartMap({"fragment_no_socket"},
///                                     {"joint_one_socket"});
///
/// // Build a scaffold with connections (no ID hunting, no socket patching)
/// ScaffoldResult result = builder.MakeConnectedScaffold(
///     {"fragment_two_sockets", "fragment_two_sockets"},
///     {"joint_two_sockets"},
///     {{0, 1, 2, 0},   // fragment[0].socket[1] -> joint[0].socket[0]
///      {2, 1, 1, 0}}); // joint[0].socket[1]    -> fragment[1].socket[0]
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
  /// @brief Monotonically increasing counter for assigning instance IDs.
  /////////////////////////////////////////////////
  uint32_t m_next_id{0};

  /////////////////////////////////////////////////
  /// @brief Build a scaffold and record insertion-order part IDs.
  ///
  /// Shared implementation used by MakeScaffoldWithParts and
  /// MakeConnectedScaffold.
  ///
  /// @param fragment_names Names of Fragments to include, in order.
  /// @param joint_names    Names of Joints to include, in order.
  /// @return ScaffoldResult with no connections and IDs in insertion order.
  /////////////////////////////////////////////////
  ScaffoldResult
  BuildScaffoldWithIds(const std::vector<std::string> &fragment_names,
                       const std::vector<std::string> &joint_names);

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
  /// @brief Build a PartMap from named Fragments and Joints in the library.
  ///
  /// Fragment instances are inserted first (in order), then Joint instances.
  /// Each entry is assigned a monotonically increasing ID from this builder's
  /// internal counter.
  ///
  /// All raw pointers in the returned instances point into the library's
  /// storage — the library must outlive the PartMap.
  ///
  /// @param fragment_names Names of Fragments to include, in order.
  /// @param joint_names    Names of Joints to include, in order.
  /// @return PartMap keyed by stable instance IDs.
  /////////////////////////////////////////////////
  PartMap MakePartMap(const std::vector<std::string> &fragment_names,
                      const std::vector<std::string> &joint_names);

  /////////////////////////////////////////////////
  /// @brief Build a MachinaFormScaffold pre-populated with instances but no
  /// connections.
  ///
  /// Fragment instances are added first (in order), then Joint instances.
  /// Each instance is assigned a monotonically increasing ID from this
  /// builder's internal counter and the scaffold's @c next_id is advanced to
  /// match.
  ///
  /// All raw pointers in the instances point into the library's storage — the
  /// library must outlive the returned scaffold.
  ///
  /// Typical usage: call MakeScaffoldWithParts to obtain a scaffold, then push
  /// Connection values into @c scaffold.connections to build connected or ring
  /// scenarios. For scenarios that need connections, prefer
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
  /// Fragment instances are added first (in order), then Joint instances —
  /// assigning part indices 0, 1, … in that combined order. Each
  /// ConnectionSpec references parts by their insertion-order index, so no
  /// map-walking or ID hunting is needed in tests.
  ///
  /// For every connection in @p connections the method:
  ///   - Appends a Connection to the scaffold's @c connections vector.
  ///   - Marks both endpoint sockets as @c SocketState::Connected.
  ///
  /// Fails the test (via FAIL) if any part index or socket index in a
  /// ConnectionSpec is out of range.
  ///
  /// All raw pointers in the result point into the library's storage — the
  /// library must outlive the returned ScaffoldResult.
  ///
  /// @param fragment_names Names of Fragments to include, in order.
  /// @param joint_names    Names of Joints to include, in order.
  /// @param connections    Connections to wire, expressed as index-based specs.
  /// @return ScaffoldResult containing the wired scaffold and insertion-order
  ///         part IDs.
  /////////////////////////////////////////////////
  ScaffoldResult
  MakeConnectedScaffold(const std::vector<std::string> &fragment_names,
                        const std::vector<std::string> &joint_names,
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
