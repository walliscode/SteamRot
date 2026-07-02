/////////////////////////////////////////////////
/// @file
/// @brief Declartion of the FragmentBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Fragment.h"
#include "views_library.h"

namespace steamrot::parts {

/////////////////////////////////////////////////
/// @class FragmentBuilder
/// @brief Provides a fluent interface for constructing FragmentInstance objects
/// with specific configurations and properties.
/////////////////////////////////////////////////
class FragmentBuilder {

private:
  /////////////////////////////////////////////////
  /// @brief Fragment to modify and return
  /////////////////////////////////////////////////
  Fragment m_fragment;

public:
  /////////////////////////////////////////////////
  /// @brief Initializes a new FragmentBuilder
  /////////////////////////////////////////////////
  FragmentBuilder() = default;

  /////////////////////////////////////////////////
  /// @brief Adds a set of premade views to the Fragment being built.
  ///
  /// @param view enum value representing the premade view configuration to add
  /// to the Fragment.
  /// @return Reference to the current FragmentBuilder instance for method
  /// chaining.
  /////////////////////////////////////////////////
  FragmentBuilder &AddViews(const PremadeViews view);

  /////////////////////////////////////////////////
  /// @brief Add a single socket to the Fragment being built, specifying its
  /// local position and alignment vector.
  ///
  /// @param local_pos Vector2f representing the local position of the socket in
  /// the part's own coordinate space.
  /// @param align_vec Vector2f representing the alignment vector/axis in the
  /// part's own coordinate space. It will be normalized.
  /// @return Reference to the current FragmentBuilder instance for method
  /// chaining.
  /////////////////////////////////////////////////
  FragmentBuilder &AddSocket(const sf::Vector2f &local_pos,
                             const sf::Vector2f &align_vec);

  /////////////////////////////////////////////////
  /// @brief Returns, by copy, the Fragment constructed by the builder. The
  /// builder can be reused after this call.
  ///
  /// @return Fragment The constructed Fragment object.
  /////////////////////////////////////////////////
  Fragment Build(const std::string &name);
};
} // namespace steamrot::parts
