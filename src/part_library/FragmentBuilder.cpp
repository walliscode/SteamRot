/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the FragmentBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FragmentBuilder.h"

namespace steamrot::parts {
/////////////////////////////////////////////////
FragmentBuilder &FragmentBuilder::AddViews(const PremadeViews view) {
  switch (view) {
  case PremadeViews::WhiteOblong:
    m_fragment.positioning_views = get_white_oblong_Views();
    break;
  default:
    // Handle unknown view type if necessary
    break;
  }
  return *this;
}

/////////////////////////////////////////////////
FragmentBuilder &FragmentBuilder::AddSocket(const sf::Vector2f &local_pos,
                                            const sf::Vector2f &align_vec) {
  m_fragment.sockets.emplace_back(local_pos, align_vec);
  return *this;
}
/////////////////////////////////////////////////
Fragment FragmentBuilder::Build(const std::string &name) {

  m_fragment.name = name;
  Fragment fragment_copy = m_fragment;
  m_fragment = Fragment(); // Reset to a new Fragment for future use
  return fragment_copy;
}
} // namespace steamrot::parts
