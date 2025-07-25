/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CGrimoireMachina component
/////////////////////////////////////////////////

#include "CGrimoireMachina.h"
#include "containers.h"
#include "fragments_generated.h"
#include "joints_generated.h"
#include "log_handler.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <print>
namespace steamrot {

/////////////////////////////////////////////////
const size_t CGrimoireMachina::GetComponentRegisterIndex() const {

  // Get the index of this component in the component register
  static constexpr size_t index = components::containers::TupleTypeIndex<
      CGrimoireMachina, components::containers::ComponentRegister>;
  return index;
}

/////////////////////////////////////////////////
const std::string &CGrimoireMachina::Name() {
  static const std::string name = "CGrimoireMachina";
  return name;
}
/////////////////////////////////////////////////
void CGrimoireMachina::Configure(const GrimoireMachina *grimoire_data) {

  // Set component level data
  m_active = true;

  // Load all fragments from the provided data
  for (const auto *fragment_data : *grimoire_data->fragments()) {
    if (fragment_data) {
      // Configure fragments with the provided data
      ConfigureFragment(fragment_data);
      std::println("Configured fragment: {}", fragment_data->name()->str());
    }
  }
  // Load all joints from the provided data
  for (const auto *joint_data : *grimoire_data->joints()) {
    if (joint_data) {
      // Configure joints with the provided data
      ConfigureJoint(joint_data);
      std::println("Configured joint: {}", joint_data->name()->str());
    }
  }
}

/////////////////////////////////////////////////
void CGrimoireMachina::ConfigureFragment(const FragmentData *fragment_data) {
  // Guard statement to ensure data is not null
  if (!fragment_data) {
    log_handler::ProcessLog(spdlog::level::info, log_handler::LogCode::kNoCode,
                            "Configuration data is null for CGrimoireMachina");
    return;
  }
  Fragment new_fragment;

  // deal with sockets
  for (const auto *socket : *fragment_data->socket_data()->vertices()) {
    sf::Vertex vertex;
    vertex.position = sf::Vector2f(socket->x(), socket->y());
    new_fragment.m_sockets.append(vertex);
  }

  // deal with render overlay

  for (const auto view : *fragment_data->render_overlay_data()->views()) {
  }
}

/////////////////////////////////////////////////
void CGrimoireMachina::ConfigureJoint(const JointData *joint_data) {
  // Guard statement to ensure data is not null
  if (!joint_data) {
    log_handler::ProcessLog(spdlog::level::info, log_handler::LogCode::kNoCode,
                            "Configuration data is null for CGrimoireMachina");
    return;
  }
  Joint new_joint;
  new_joint.m_joint_name = joint_data->name()->str();
  new_joint.m_number_of_connections = joint_data->number_of_connections();
}
} // namespace steamrot
