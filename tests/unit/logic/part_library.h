/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the PartLibrary test helper.
///
/// PartLibrary is a test-only in-memory catalog of named Fragment and Joint
/// definitions. Tests can build a library with AddFragment / AddJoint, look up
/// parts by name, and use the builder methods to create FragmentInstances,
/// JointInstances, and PartMaps for use in unit tests.
///
/// ⚠ Instances created by this class hold raw pointers into the library's
/// internal storage.  The PartLibrary must therefore outlive any instances or
/// PartMaps it produces.
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
/// @class PartLibrary
/// @brief Test-only catalog of named Fragment and Joint definitions.
///
/// Usage example:
/// @code
/// PartLibrary lib;
/// lib.AddFragment("arm", MakeFragmentWithFrontView())
///    .AddJoint("shoulder", MakeJointWithSocketConfig(2, 10.f));
///
/// // Single lookup
/// const Fragment& arm = lib.GetFragment("arm");
///
/// // Create a FragmentInstance (pointer into lib storage — lib must stay alive)
/// FragmentInstance inst = lib.MakeFragmentInstance("arm");
///
/// // Create a PartMap from a subset of the catalog
/// PartMap parts = lib.MakePartMap({"arm"}, {"shoulder"});
/// @endcode
/////////////////////////////////////////////////
class PartLibrary {
public:
  /////////////////////////////////////////////////
  /// @brief Default constructor — creates an empty library.
  /////////////////////////////////////////////////
  PartLibrary() = default;

  /////////////////////////////////////////////////
  /// @brief Add (or replace) a named Fragment in the library.
  ///
  /// Returns *this to allow fluent chaining.
  ///
  /// @param name     Key used for later lookup.
  /// @param fragment Fragment definition to store.
  /// @return Reference to this library.
  /////////////////////////////////////////////////
  PartLibrary &AddFragment(std::string name, Fragment fragment);

  /////////////////////////////////////////////////
  /// @brief Add (or replace) a named Joint in the library.
  ///
  /// Returns *this to allow fluent chaining.
  ///
  /// @param name  Key used for later lookup.
  /// @param joint Joint definition to store.
  /// @return Reference to this library.
  /////////////////////////////////////////////////
  PartLibrary &AddJoint(std::string name, Joint joint);

  /////////////////////////////////////////////////
  /// @brief Retrieve a Fragment by name.
  ///
  /// Terminates the test (via REQUIRE) if the name is not found.
  ///
  /// @param name Key to look up.
  /// @return Reference to the stored Fragment.
  /////////////////////////////////////////////////
  Fragment &GetFragment(const std::string &name);

  /////////////////////////////////////////////////
  /// @brief Retrieve a Fragment by name (const overload).
  ///
  /// @param name Key to look up.
  /// @return Const reference to the stored Fragment.
  /////////////////////////////////////////////////
  const Fragment &GetFragment(const std::string &name) const;

  /////////////////////////////////////////////////
  /// @brief Retrieve a Joint by name.
  ///
  /// Terminates the test (via REQUIRE) if the name is not found.
  ///
  /// @param name Key to look up.
  /// @return Reference to the stored Joint.
  /////////////////////////////////////////////////
  Joint &GetJoint(const std::string &name);

  /////////////////////////////////////////////////
  /// @brief Retrieve a Joint by name (const overload).
  ///
  /// @param name Key to look up.
  /// @return Const reference to the stored Joint.
  /////////////////////////////////////////////////
  const Joint &GetJoint(const std::string &name) const;

  /////////////////////////////////////////////////
  /// @brief Retrieve multiple Fragments by name.
  ///
  /// Each name must exist in the library (tested via REQUIRE).
  /// The returned pointers are stable as long as the library is alive and no
  /// AddFragment call replaces the same key.
  ///
  /// @param names Ordered list of Fragment keys.
  /// @return Vector of non-owning pointers, one per name, in the same order.
  /////////////////////////////////////////////////
  std::vector<Fragment *> GetFragments(const std::vector<std::string> &names);

  /////////////////////////////////////////////////
  /// @brief Retrieve multiple Joints by name.
  ///
  /// @param names Ordered list of Joint keys.
  /// @return Vector of non-owning pointers, one per name, in the same order.
  /////////////////////////////////////////////////
  std::vector<Joint *> GetJoints(const std::vector<std::string> &names);

  /////////////////////////////////////////////////
  /// @brief Create a FragmentInstance backed by a stored Fragment definition.
  ///
  /// The returned instance holds a raw pointer into this library's storage.
  /// The library must outlive the instance.
  ///
  /// @param name             Key of the Fragment to instantiate.
  /// @param initial_transform World-space transform for the instance.
  /// @return A new FragmentInstance referencing the stored Fragment.
  /////////////////////////////////////////////////
  FragmentInstance
  MakeFragmentInstance(const std::string &name,
                       sf::Transform initial_transform = sf::Transform::Identity);

  /////////////////////////////////////////////////
  /// @brief Create a JointInstance backed by a stored Joint definition.
  ///
  /// The returned instance holds a raw pointer into this library's storage.
  /// The library must outlive the instance.
  ///
  /// @param name             Key of the Joint to instantiate.
  /// @param initial_transform World-space transform for the instance.
  /// @return A new JointInstance referencing the stored Joint.
  /////////////////////////////////////////////////
  JointInstance
  MakeJointInstance(const std::string &name,
                    sf::Transform initial_transform = sf::Transform::Identity);

  /////////////////////////////////////////////////
  /// @brief Build a PartMap from named Fragments and Joints in the library.
  ///
  /// Each entry in the returned map is assigned a monotonically increasing ID
  /// from an internal counter.  Fragment names are inserted first (in order),
  /// then Joint names.
  ///
  /// All raw pointers in the returned instances point into this library's
  /// storage — the library must outlive the PartMap.
  ///
  /// @param fragment_names Names of Fragments to include, in order.
  /// @param joint_names    Names of Joints to include, in order.
  /// @return PartMap keyed by stable instance IDs.
  /////////////////////////////////////////////////
  PartMap MakePartMap(const std::vector<std::string> &fragment_names,
                      const std::vector<std::string> &joint_names);

  /////////////////////////////////////////////////
  /// @brief Returns the names of all stored Fragments.
  ///
  /// @return Vector of Fragment keys in map iteration order.
  /////////////////////////////////////////////////
  std::vector<std::string> FragmentNames() const;

  /////////////////////////////////////////////////
  /// @brief Returns the names of all stored Joints.
  ///
  /// @return Vector of Joint keys in map iteration order.
  /////////////////////////////////////////////////
  std::vector<std::string> JointNames() const;

private:
  /////////////////////////////////////////////////
  /// @brief Named Fragment definitions.
  /////////////////////////////////////////////////
  std::map<std::string, Fragment> m_fragments;

  /////////////////////////////////////////////////
  /// @brief Named Joint definitions.
  /////////////////////////////////////////////////
  std::map<std::string, Joint> m_joints;

  /////////////////////////////////////////////////
  /// @brief Counter used to generate unique IDs for instances created by
  /// MakeFragmentInstance, MakeJointInstance, and MakePartMap.
  /////////////////////////////////////////////////
  uint32_t m_next_id{0};
};

} // namespace steamrot::tests
