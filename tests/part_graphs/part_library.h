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
#include <map>

namespace steamrot::tests {

enum class FragmentInstanceNames {
  FragmentNoSocket,
  FragmentOneSocket,
  FragmentTwoSockets,
  FragmentThreeSockets
};

enum class JointInstanceNames {
  JointNoSocket,
  JointOneSocket,
  JointTwoSockets,
  JointThreeSockets
};
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
/// @brief Fixed catalog of named Fragment and Joint test definitions
/////////////////////////////////////////////////
struct TestPartLibrary {
  /////////////////////////////////////////////////
  /// @brief Named Fragment definitions.
  /////////////////////////////////////////////////
  std::map<FragmentInstanceNames, Fragment> fragments;

  /////////////////////////////////////////////////
  /// @brief Named Joint definitions.
  /////////////////////////////////////////////////
  std::map<JointInstanceNames, Joint> joints;

  /////////////////////////////////////////////////
  /// @brief Create a TestPartLibrary pre-populated with the standard catalog
  ///        and scaffold scenarios.
  ///
  /// @return TestPartLibrary containing the predefined Fragments, Joints,
  ///         and pre-wired scaffold scenarios.
  /////////////////////////////////////////////////
  static TestPartLibrary Create();
};

} // namespace steamrot::tests
