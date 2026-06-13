/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersGrimoireMachinaProvider class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersGrimoireMachinaProvider.h"
#include "FlatbuffersDataLoader.h"
#include "configure_grimoire.h"

namespace steamrot {
/////////////////////////////////////////////////
std::expected<GrimoireMachina, FailInfo>
FlatbuffersGrimoireMachinaProvider::CreateGrimoireMachina() const {
  GrimoireMachina grimoire_machina;
  return grimoire_machina;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersGrimoireMachinaProvider::ConfigureGrimoireMachina(
    GrimoireMachina &grimoire_machina) const {

  // instantiate the flatbuffers data loader
  FlatbuffersDataLoader data_loader;

  // load all the Fragments from the flatbuffers data source
  auto fragments_result = data_loader.ProvideAllFragmentData();
  if (!fragments_result.has_value()) {
    return std::unexpected(fragments_result.error());
  }

  // for each Fragment, configure a Fragment data structure and add it to the
  // GrimoireMachina
  for (const auto &fragment_fbs : fragments_result.value()) {
    Fragment fragment;
    auto configure_result =
        data::configure::ConfigureFragment(fragment, fragment_fbs);
    if (!configure_result.has_value()) {
      return std::unexpected(configure_result.error());
    }
    grimoire_machina.m_all_fragments.insert({fragment.name, fragment});
  }

  // load all the Joints from the flatbuffers data source
  auto joints_result = data_loader.ProvideAllJointData();
  if (!joints_result.has_value()) {
    return std::unexpected(joints_result.error());
  }
  // for each Joint, configure a Joint data structure and add it to the
  // GrimoireMachina
  for (const auto &joint_fbs : joints_result.value()) {
    Joint joint;
    auto configure_result = data::configure::ConfigureJoint(joint, joint_fbs);
    if (!configure_result.has_value()) {
      return std::unexpected(configure_result.error());
    }
    grimoire_machina.m_all_joints[joint.name] = joint;
  }

  return std::monostate{};
}

} // namespace steamrot
