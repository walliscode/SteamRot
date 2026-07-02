/////////////////////////////////////////////////
/// @file
/// @brief Declartion of the JointBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Joint.h"
#include "views_library.h"

namespace steamrot::parts {

/////////////////////////////////////////////////
/// @class JointBuilder
/// @brief Provides a fluent interface for constructing JointInstance objects
/// with specific configurations and properties.
/////////////////////////////////////////////////
class JointBuilder {

private:
  /////////////////////////////////////////////////
  /// @brief Joint to modify and return
  /////////////////////////////////////////////////
  Joint m_joint;

public:
  /////////////////////////////////////////////////
  /// @brief Initializes a new JointBuilder
  /////////////////////////////////////////////////
  JointBuilder() = default;

  /////////////////////////////////////////////////
  /// @brief Adds a set of premade views to the Joint being built.
  ///
  /// @param view enum value representing the premade view configuration to add
  /// to the Joint.
  /// @return Reference to the current JointBuilder instance for method
  /// chaining.
  /////////////////////////////////////////////////
  JointBuilder &AddViews(const PremadeViews view);

  /////////////////////////////////////////////////
  /// @brief Sets the local origin of the Joint being built.
  ///
  /// @param origin The local origin of the Joint in its own coordinate space.
  /// @return Reference to the current JointBuilder instance for method
  /// chaining.
  /////////////////////////////////////////////////
  JointBuilder &SetOrigin(const sf::Vector2f &origin);

  /////////////////////////////////////////////////
  /// @brief [TODO:description]
  ///
  /// @param socket_count [TODO:parameter]
  /// @param radius [TODO:parameter]
  /// @param min_gap [TODO:parameter]
  /// @param fixed_socket_angle [TODO:parameter]
  /// @param has_fixed_socket [TODO:parameter]
  /// @param rotation_arc_min [TODO:parameter]
  /// @param rotation_arc_max [TODO:parameter]
  /// @return [TODO:return]
  /////////////////////////////////////////////////
  JointBuilder &SetSocketConfig(const uint8_t socket_count, const float radius,
                                const float min_gap,
                                const float fixed_socket_angle,
                                const bool has_fixed_socket,
                                const float rotation_arc_min,
                                const float rotation_arc_max);
  /////////////////////////////////////////////////
  /// @brief Finalizes the construction of the Joint and returns the constructed
  /// Joint object.
  /////////////////////////////////////////////////
  /////////////////////////////////////////////////
  Joint Build(const std::string &name);
};
} // namespace steamrot::parts
