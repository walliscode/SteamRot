/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the TestPartLibrary struct and PartLibraryBuilder
///        class for test infrastructure.
///
/// TestPartLibrary is a value-type struct that holds a small, fixed set of
/// pre-defined Fragment and Joint objects keyed by name. Instantiate it via
/// TestPartLibrary::Create() to get the standard catalog.
///
/// PartLibraryBuilder wraps a TestPartLibrary reference and provides
/// convenience methods to create FragmentInstances, JointInstances, and
/// PartMaps backed by the library's storage.
///
/// ⚠ Instances produced by PartLibraryBuilder hold raw pointers into the
/// TestPartLibrary they were built from. The library must outlive any
/// instances or PartMaps it produces.
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
/// @struct TestPartLibrary
/// @brief Fixed catalog of named Fragment and Joint test definitions.
///
/// The catalog is pre-populated by TestPartLibrary::Create() with the
/// following named parts:
///
/// Fragments:
///   "fragment_no_socket"     — green triangle in Front view, no sockets
///   "fragment_one_socket"    — green origin triangle + one socket at (5, 5)
///   "fragment_two_sockets"   — white 20×20 square + sockets at (0, 10) and (20, 10)
///   "fragment_three_sockets" — white 20×20 square + sockets at (0, 10), (10, 10) and (20, 10)
///
/// Joints:
///   "joint_no_socket"      — blue triangle in Front view, no sockets
///   "joint_one_socket"     — 1 socket at radius 10, full rotation arc
///   "joint_two_sockets"    — 2 sockets at radius 15, full rotation arc
///   "joint_three_sockets"  — 3 sockets at radius 15, full rotation arc
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
  /// @brief Create a TestPartLibrary pre-populated with the standard catalog.
  ///
  /// @return TestPartLibrary containing the predefined Fragments and Joints.
  /////////////////////////////////////////////////
  static TestPartLibrary Create();
};

/////////////////////////////////////////////////
/// @class PartLibraryBuilder
/// @brief Creates instances and PartMaps from a TestPartLibrary.
///
/// Wraps a TestPartLibrary reference and exposes convenience methods for
/// constructing FragmentInstances, JointInstances, and PartMaps in tests.
/// Each builder maintains its own ID counter so IDs are unique within a
/// single builder's lifetime.
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
/// @endcode
/////////////////////////////////////////////////
class PartLibraryBuilder {
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
  FragmentInstance
  MakeFragmentInstance(const std::string &name,
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
  /// scenarios.
  ///
  /// @param fragment_names Names of Fragments to include, in order.
  /// @param joint_names    Names of Joints to include, in order.
  /// @return MachinaFormScaffold containing the requested instances.
  /////////////////////////////////////////////////
  MachinaFormScaffold
  MakeScaffoldWithParts(const std::vector<std::string> &fragment_names,
                        const std::vector<std::string> &joint_names);

private:
  /////////////////////////////////////////////////
  /// @brief Reference to the library this builder draws from.
  /////////////////////////////////////////////////
  TestPartLibrary &m_library;

  /////////////////////////////////////////////////
  /// @brief Monotonically increasing counter for assigning instance IDs.
  /////////////////////////////////////////////////
  uint32_t m_next_id{0};
};

} // namespace steamrot::tests
