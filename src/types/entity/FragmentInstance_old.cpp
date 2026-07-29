/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the FragmentInstance class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FragmentInstance.h"
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <stdexcept>

namespace steamrot {

/////////////////////////////////////////////////
FragmentInstance::FragmentInstance(const Fragment *fragment_ptr,
                                   sf::Transform initial_transform)
    : PartInstance{initial_transform}, fragment{fragment_ptr} {
  if (fragment_ptr) {
    for (uint32_t i = 0;
         i < static_cast<uint32_t>(fragment_ptr->sockets.size()); ++i)
      sockets.emplace(
          i, FragmentSocketState{fragment_ptr->sockets[i].local_position,
                                 fragment_ptr->sockets[i].alignment_vector});
  }
}

/////////////////////////////////////////////////
sf::Vector2f FragmentInstance::CalculateAlignmentVectorWorld(
    const uint32_t socket_id) const {
  // place holder
  return sf::Vector2f{0.f, 0.f};
}

/////////////////////////////////////////////////
void FragmentInstance::SetConnection(const SocketConnection &connection,
                                     const uint32_t socket_id) {
  auto it = sockets.find(socket_id);
  if (it != sockets.end()) {
    it->second.SetConnection(connection);
  }
}

/////////////////////////////////////////////////
void FragmentInstance::CheckMouseOver(const sf::Vector2f &world_mouse) {
  for (auto &[socket_id, socket] : sockets) {
    const sf::Vector2f world_pos =
        transform.transformPoint(socket.GetLocalPosition());
    socket.CheckMouseOver(world_mouse, world_pos);
  }
}

/////////////////////////////////////////////////
const FragmentSocketState &
FragmentInstance::GetSocket(const uint32_t socket_id) const {
  auto it = sockets.find(socket_id);
  if (it != sockets.end()) {
    return it->second;
  }
  throw std::out_of_range("Socket ID not found in FragmentInstance.");
}

/////////////////////////////////////////////////
FragmentSocketState &FragmentInstance::GetSocket(const uint32_t socket_id) {
  auto it = sockets.find(socket_id);
  if (it != sockets.end()) {
    return it->second;
  }
  throw std::out_of_range("Socket ID not found in FragmentInstance.");
}
/////////////////////////////////////////////////
const sf::Vector2f
FragmentInstance::GetSocketWorldPosition(const uint32_t socket_id) const {
  auto it = sockets.find(socket_id);
  if (it != sockets.end()) {
    return transform.transformPoint(it->second.GetLocalPosition());
  }
  return sf::Vector2f{0.f, 0.f};
}

/////////////////////////////////////////////////
const sf::Vector2f FragmentInstance::GetSocketWorldAlignmentVector(
    const uint32_t socket_id) const {

  // place holder
  return sf::Vector2f{0.f, 0.f};
}
} // namespace steamrot
