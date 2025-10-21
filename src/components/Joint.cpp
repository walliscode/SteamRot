/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the Joint struct
/////////////////////////////////////////////////

#include "Joint.h"

namespace steamrot {

////////////////////////////////////////////////////////////
bool Joint::operator==(const Joint &other) const {
  // Compare joint name
  if (m_joint_name != other.m_joint_name)
    return false;

  // Compare number of connections
  if (m_number_of_connections != other.m_number_of_connections)
    return false;

  // Compare global position
  if (m_global_position != other.m_global_position)
    return false;

  // Compare connected fragments
  if (m_connected_fragments != other.m_connected_fragments)
    return false;

  // Compare transforms (matrix elements)
  const auto *this_matrix = m_transform.getMatrix();
  const auto *other_matrix = other.m_transform.getMatrix();
  for (size_t i = 0; i < 16; ++i) {
    if (this_matrix[i] != other_matrix[i])
      return false;
  }

  // Compare render overlays
  if (m_render_overlay.getVertexCount() != other.m_render_overlay.getVertexCount())
    return false;

  if (m_render_overlay.getPrimitiveType() != other.m_render_overlay.getPrimitiveType())
    return false;

  for (size_t i = 0; i < m_render_overlay.getVertexCount(); ++i) {
    const auto &v1 = m_render_overlay[i];
    const auto &v2 = other.m_render_overlay[i];
    if (v1.position != v2.position || v1.color != v2.color ||
        v1.texCoords != v2.texCoords)
      return false;
  }

  return true;
}

} // namespace steamrot
