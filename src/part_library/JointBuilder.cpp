/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the JointBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "JointBuilder.h"

namespace steamrot::parts {
/////////////////////////////////////////////////
JointBuilder &JointBuilder::AddViews(const PremadeViews view) {
  switch (view) {
  case PremadeViews::WhiteOblong:
    m_joint.positioning_views = get_white_oblong_Views();
    break;
  case PremadeViews::WhiteSquare:
    m_joint.positioning_views = get_white_square_Views();
    break;
  default:
    // Handle unknown view type if necessary
    break;
  }
  return *this;
}

/////////////////////////////////////////////////
Joint JointBuilder::Build(const std::string &name) {
  m_joint.name = name;
  Joint joint_copy = m_joint;
  m_joint = Joint(); // Reset to a new Joint for future use
  // Return the constructed Joint object
  return joint_copy;
}
} // namespace steamrot::parts
