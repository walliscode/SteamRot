/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the Fragment struct
/////////////////////////////////////////////////

#include "Fragment.h"

namespace steamrot {

////////////////////////////////////////////////////////////
bool Fragment::operator==(const Fragment &other) const {
  // Compare name
  if (m_name != other.m_name)
    return false;

  // Compare sockets
  if (m_sockets != other.m_sockets)
    return false;

  // Compare transforms (matrix elements)
  const auto *this_matrix = m_transform.getMatrix();
  const auto *other_matrix = other.m_transform.getMatrix();
  for (size_t i = 0; i < 16; ++i) {
    if (this_matrix[i] != other_matrix[i])
      return false;
  }

  // Compare overlays (size and keys)
  if (m_overlays.size() != other.m_overlays.size())
    return false;

  for (const auto &[key, value] : m_overlays) {
    auto it = other.m_overlays.find(key);
    if (it == other.m_overlays.end())
      return false;

    // Compare VertexArrays
    const auto &this_va = value;
    const auto &other_va = it->second;

    if (this_va.getVertexCount() != other_va.getVertexCount())
      return false;

    if (this_va.getPrimitiveType() != other_va.getPrimitiveType())
      return false;

    for (size_t i = 0; i < this_va.getVertexCount(); ++i) {
      const auto &v1 = this_va[i];
      const auto &v2 = other_va[i];
      if (v1.position != v2.position || v1.color != v2.color ||
          v1.texCoords != v2.texCoords)
        return false;
    }
  }

  return true;
}

} // namespace steamrot
