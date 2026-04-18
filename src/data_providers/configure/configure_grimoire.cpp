/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions to configure grimoire data
/// structures (Fragment and Joint)
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_grimoire.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Vertex.hpp>

namespace steamrot::data::configure {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureFragment(Fragment &fragment, const FragmentFbs *fragment_fbs) {

  // Check for null pointer
  if (fragment_fbs == nullptr) {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "FragmentFbs pointer is null"});
  }

  // Configure name
  if (fragment_fbs->name() && !(fragment_fbs->name()->str().empty())) {
    fragment.name = fragment_fbs->name()->str();
  } else {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "Fragment name is missing"});
  }

  // Configure sockets
  if (!fragment_fbs->sockets()->empty()) {
    const auto *sockets_fbs = fragment_fbs->sockets();
    fragment.sockets.clear();
    fragment.sockets.reserve(sockets_fbs->size());

    for (const auto *socket_fbs : *sockets_fbs) {
      if (socket_fbs) {
        fragment.sockets.emplace_back(socket_fbs->x(), socket_fbs->y());
      } else {
        return std::unexpected(
            FailInfo{FailMode::FlatbuffersDataNotFound, "Socket data is null"});
      }
    }
  } else {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "Fragment sockets are missing"});
  }

  // Configure movement_views
  if (!fragment_fbs->movement_views()->empty()) {
    const auto *views_fbs = fragment_fbs->movement_views();
    fragment.movement_views.clear();

    for (const auto *view_fbs : *views_fbs) {
      if (!view_fbs) {
        return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                        "ViewFbs data is null"});
      }

      // Convert ViewDirectionFbs to ViewDirection
      ViewDirection direction;
      switch (view_fbs->direction()) {
      case ViewDirectionFbs_FRONT:
        direction = ViewDirection::Front;
        break;
      case ViewDirectionFbs_NONE:
        // NONE is used as a default/uninitialized value in FlatBuffers
        // Map to Back as the fallback direction
        direction = ViewDirection::Back;
        break;
      default:
        return std::unexpected(
            FailInfo{FailMode::FlatbuffersDataNotFound,
                     "Unknown view direction in Fragment movement_views"});
      }

      // Process triangles for this view
      if (!view_fbs->triangles()) {
        return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                        "ViewFbs triangles are missing"});
      }

      const auto *triangles_fbs = view_fbs->triangles();

      // Create a single VertexArray for this direction
      sf::VertexArray vertex_array;
      vertex_array.setPrimitiveType(sf::PrimitiveType::Triangles);

      for (const auto *triangle_fbs : *triangles_fbs) {
        if (!triangle_fbs) {
          return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                          "Triangle data is null"});
        }

        if (!triangle_fbs->vertices()) {
          return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                          "Triangle vertices are missing"});
        }

        const auto *vertices_fbs = triangle_fbs->vertices();
        if (vertices_fbs->size() != 3) {
          return std::unexpected(
              FailInfo{FailMode::FlatbuffersDataNotFound,
                       "Triangle must have exactly 3 vertices"});
        }

        for (const auto *vertex_fbs : *vertices_fbs) {
          if (!vertex_fbs) {
            return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                            "Vertex data is null"});
          }

          if (!vertex_fbs->position()) {
            return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                            "Vertex position is missing"});
          }

          if (!vertex_fbs->color()) {
            return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                            "Vertex color is missing"});
          }

          const auto *pos = vertex_fbs->position();
          const auto *col = vertex_fbs->color();

          sf::Vertex sfml_vertex;
          sfml_vertex.position = sf::Vector2f(pos->x(), pos->y());
          sfml_vertex.color = sf::Color(col->r(), col->g(), col->b(), col->a());

          vertex_array.append(sfml_vertex);
        }
      }

      fragment.movement_views.insert_or_assign(direction,
                                               std::move(vertex_array));
    }
  } else {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "Fragment movement_views are missing"});
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureJoint(Joint &joint, const JointFbs *joint_fbs) {

  // Check for null pointer
  if (joint_fbs == nullptr) {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "JointFbs pointer is null"});
  }
  // Configure name
  if (joint_fbs->name() && !(joint_fbs->name()->str().empty())) {
    joint.name = joint_fbs->name()->str();

  } else {

    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound, "Joint name is missing"});
  }

  // configure origin
  if (joint_fbs->origin()) {
    joint.origin =
        sf::Vector2f(joint_fbs->origin()->x(), joint_fbs->origin()->y());
  } else {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound, "Joint origin is missing"});
  }

  // Configure socket_config
  if (!joint_fbs->socket_config()) {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "Joint socket_config is missing"});
  }

  const auto *sc = joint_fbs->socket_config();

  if (sc->socket_count() <= 0) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "Joint socket_config socket_count must be positive"});
  }

  joint.socket_config.socket_count = sc->socket_count();
  joint.socket_config.radius = sc->radius();
  joint.socket_config.min_gap = sc->min_gap();
  joint.socket_config.fixed_socket_angle = sc->fixed_socket_angle();
  joint.socket_config.has_fixed_socket = sc->has_fixed_socket();
  joint.socket_config.rotation_arc_min = sc->rotation_arc_min();
  joint.socket_config.rotation_arc_max = sc->rotation_arc_max();

  // Configure movement_views
  if (!joint_fbs->movement_views()->empty()) {
    const auto *views_fbs = joint_fbs->movement_views();
    joint.movement_views.clear();

    for (const auto *view_fbs : *views_fbs) {
      if (!view_fbs) {
        return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                        "ViewFbs data is null"});
      }

      // Convert ViewDirectionFbs to ViewDirection
      ViewDirection direction;
      switch (view_fbs->direction()) {
      case ViewDirectionFbs_FRONT:
        direction = ViewDirection::Front;
        break;
      case ViewDirectionFbs_NONE:
        // NONE is used as a default/uninitialized value in FlatBuffers
        // Map to Back as the fallback direction
        direction = ViewDirection::Back;
        break;
      default:
        return std::unexpected(
            FailInfo{FailMode::FlatbuffersDataNotFound,
                     "Unknown view direction in Joint movement_views"});
      }

      // Process triangles for this view
      if (!view_fbs->triangles()) {
        return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                        "ViewFbs triangles are missing"});
      }

      const auto *triangles_fbs = view_fbs->triangles();

      // Create a single VertexArray for this direction
      sf::VertexArray vertex_array;
      vertex_array.setPrimitiveType(sf::PrimitiveType::Triangles);

      for (const auto *triangle_fbs : *triangles_fbs) {
        if (!triangle_fbs) {
          return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                          "Triangle data is null"});
        }

        if (!triangle_fbs->vertices()) {
          return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                          "Triangle vertices are missing"});
        }

        const auto *vertices_fbs = triangle_fbs->vertices();
        if (vertices_fbs->size() != 3) {
          return std::unexpected(
              FailInfo{FailMode::FlatbuffersDataNotFound,
                       "Triangle must have exactly 3 vertices"});
        }

        for (const auto *vertex_fbs : *vertices_fbs) {
          if (!vertex_fbs) {
            return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                            "Vertex data is null"});
          }

          if (!vertex_fbs->position()) {
            return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                            "Vertex position is missing"});
          }

          if (!vertex_fbs->color()) {
            return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                            "Vertex color is missing"});
          }

          const auto *pos = vertex_fbs->position();
          const auto *col = vertex_fbs->color();

          sf::Vertex sfml_vertex;
          sfml_vertex.position = sf::Vector2f(pos->x(), pos->y());
          sfml_vertex.color = sf::Color(col->r(), col->g(), col->b(), col->a());

          vertex_array.append(sfml_vertex);
        }
      }

      joint.movement_views.insert_or_assign(direction, std::move(vertex_array));
    }
  } else {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "Joint movement_views are missing"});
  }

  return std::monostate{};
}

} // namespace steamrot::data::configure
