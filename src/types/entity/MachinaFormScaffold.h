/////////////////////////////////////////////////
/// @file
/// @brief Declarartion of the MachinFormScaffold struct. No implementation is
/// needed
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
#include "ViewDirection.h"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>

namespace steamrot {

/////////////////////////////////////////////////
/// @class Socket
/// @brief Representation of the socket stored on the Fragment or Joint
///
/// Allows us to store further information about the socket; used, available,
/// etc.
/////////////////////////////////////////////////
struct Socket {
  enum class State {
    Available, // Socket exists and can connect
    Connected, // Socket is currently connected
    Blocked    // Socket exists but can't be used (e.g., edge of canvas)
  };

  /////////////////////////////////////////////////
  /// @brief Construct a Socket at the given local position on the
  /// Fragment or Joint.
  ///
  /// Stores the local position for data reference. The socket circle is
  /// positioned in world space by the owning FragmentInstance or
  /// JointInstance constructor.
  ///
  /// @param local_position Local position of the socket on its parent
  /// Fragment or Joint.
  /////////////////////////////////////////////////
  explicit Socket(sf::Vector2f local_position)
      : position{local_position} {}

  State state{State::Available};

  /////////////////////////////////////////////////
  /// @brief Local position of this socket on its parent Fragment or Joint.
  /////////////////////////////////////////////////
  sf::Vector2f position{};

  /////////////////////////////////////////////////
  /// @brief For visual and collision purposes.
  /////////////////////////////////////////////////
  sf::CircleShape circle{5.f, 30};

  /////////////////////////////////////////////////
  /// @brief Toggle whether the mouse is currently over the socket
  /////////////////////////////////////////////////
  bool is_mouse_over{false};

  /////////////////////////////////////////////////
  /// @brief Color of the socket when not hovered
  /////////////////////////////////////////////////
  sf::Color base_color{sf::Color::White};

  /////////////////////////////////////////////////
  /// @brief Color of the socket when hovered
  /////////////////////////////////////////////////
  sf::Color hover_color{sf::Color::Yellow};
};

struct JointInstance {
  /////////////////////////////////////////////////
  /// @brief Construct a JointInstance from a Joint and an initial transform.
  ///
  /// Populates the sockets vector from the Joint's local socket positions.
  /// Each socket's circle is positioned in world space by applying
  /// @p initial_transform to the local socket position.
  ///
  /// @param joint_ref      Joint to reference for this instance.
  /// @param initial_transform  Transform placing this instance in space.
  /////////////////////////////////////////////////
  JointInstance(Joint &joint_ref,
                sf::Transform initial_transform = sf::Transform::Identity)
      : joint{joint_ref}, transform{initial_transform} {
    sockets.reserve(joint_ref.sockets.size());
    for (const sf::Vector2f &local_pos : joint_ref.sockets) {
      Socket socket{local_pos};
      socket.circle.setPosition(initial_transform.transformPoint(local_pos));
      sockets.push_back(std::move(socket));
    }
  }

  ////////////////////////////////////////////////
  /// @brief Joint being referenced for this instance
  /// //////////////////////////////////////////////
  Joint &joint;

  /////////////////////////////////////////////////
  /// @brief Transformation of the Fragment on the canvas
  /////////////////////////////////////////////////
  sf::Transform transform{sf::Transform::Identity};

  /////////////////////////////////////////////////
  /// @brief All sockets on the Joint, with their state (available, used,
  /// connected)
  ///
  /// This should match the number of sockets on the Joint, and the socket_id
  /// should match the index of the socket on the Joint's vector of sockets.
  /////////////////////////////////////////////////
  std::vector<Socket> sockets;
};
struct FragmentInstance {
  /////////////////////////////////////////////////
  /// @brief Construct a FragmentInstance from a Fragment and an initial
  /// transform.
  ///
  /// Populates the sockets vector from the Fragment's local socket positions.
  /// Each socket's circle is positioned in world space by applying
  /// @p initial_transform to the local socket position.
  ///
  /// @param fragment_ref     Fragment to reference for this instance.
  /// @param initial_transform  Transform placing this instance in space.
  /////////////////////////////////////////////////
  FragmentInstance(
      Fragment &fragment_ref,
      sf::Transform initial_transform = sf::Transform::Identity)
      : fragment{fragment_ref}, transform{initial_transform} {
    sockets.reserve(fragment_ref.sockets.size());
    for (const sf::Vector2f &local_pos : fragment_ref.sockets) {
      Socket socket{local_pos};
      socket.circle.setPosition(initial_transform.transformPoint(local_pos));
      sockets.push_back(std::move(socket));
    }
  }

  /////////////////////////////////////////////////
  /// @brief Fragment being referenced for this instance
  /////////////////////////////////////////////////
  Fragment &fragment;

  /////////////////////////////////////////////////
  /// @brief Transformation of the Fragment on the canvas
  /////////////////////////////////////////////////
  sf::Transform transform{sf::Transform::Identity};

  /////////////////////////////////////////////////
  /// @brief All sockets on the Fragment, with their state (available, used,
  /// connected)
  ///
  /// This should match the number of sockets on the Fragment, and the socket_id
  /// should match the index of the socket on the Fragment's vector of sockets.
  /////////////////////////////////////////////////
  std::vector<Socket> sockets;
};

/////////////////////////////////////////////////
/// @class Connection
/// @brief representation of a connection between two sockets on two
/// FragmentInstances or JointInstances
///
/////////////////////////////////////////////////

struct Connection {
  enum class PartType { Fragment, Joint };

  struct Endpoint {
    PartType part_type;  // Is this a fragment or joint?
    size_t part_index;   // Index into fragments[] or joints[]
    size_t socket_index; // Index into that part's sockets[]
  };

  Endpoint socket_a;
  Endpoint socket_b;
};
;

struct GrowthPoint {

  /////////////////////////////////////////////////
  /// @brief The "growth" point or origin of the MachinaFormScaffold
  /////////////////////////////////////////////////
  sf::CircleShape origin{10.f, 30};

  /////////////////////////////////////////////////
  /// @brief toggle whether the mouse is currently over the growth point
  /////////////////////////////////////////////////
  bool is_mouse_over{false};

  /////////////////////////////////////////////////
  /// @brief Color of the growth point when not hovered
  /////////////////////////////////////////////////
  sf::Color base_color{sf::Color::Blue};

  /////////////////////////////////////////////////
  /// @brief Color of the growth point when hovered
  /////////////////////////////////////////////////
  sf::Color hover_color{sf::Color::Cyan};
};

/////////////////////////////////////////////////
/// @class MachinaFormScaffold
/// @brief Contains all data necessary to create a MachinaForm.
///
/// This is designed to be an "unbaked" version of the MachinaForm, this could
/// be heavy on data but only one will be run at a time. When commited to a
/// MachinaForm, certain data will be "baked" away into more efficient formats.
/// For example, the Fragment data will be baked into a single vertex array for
/// each view direction and progression of movement.
///
/////////////////////////////////////////////////
struct MachinaFormScaffold {

  /////////////////////////////////////////////////
  /// @brief Name of the MachinaFormScaffold, will be transferred to the
  /// MachinaForm when commited
  /////////////////////////////////////////////////
  std::string machina_form_name{""};

  /////////////////////////////////////////////////
  /// @brief Final "baked" vertex arrays for each view direction and 8
  /// progressions for movement
  /////////////////////////////////////////////////
  CompositeViews baked_views;

  /////////////////////////////////////////////////
  /// @brief Growth point of the MachinaFormScaffold
  /////////////////////////////////////////////////
  GrowthPoint growth_point;

  /////////////////////////////////////////////////
  /// @brief All JointInstances on the MachinaFormScaffold
  /////////////////////////////////////////////////
  std::vector<JointInstance> joints;

  /////////////////////////////////////////////////
  /// @brief All FragmentInstances on the MachinaFormScaffold
  /////////////////////////////////////////////////
  std::vector<FragmentInstance> fragments;

  /////////////////////////////////////////////////
  /// @brief All connections between sockets on the MachinaFormScaffold
  /////////////////////////////////////////////////
  std::vector<Connection> connections;
};
} // namespace steamrot
