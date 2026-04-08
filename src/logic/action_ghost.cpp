/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for processing actions related to
/// MrGhost.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_ghost.h"

namespace steamrot::logic::action::ghost {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
AttachGhostPayload(MrGhost &mr_ghost, const GhostPayload &payload) {

  switch (payload.action) {
  case GhostPayload::GhostAction::SELECT:
    mr_ghost.m_selection = payload.m_selection;
    break;
  case GhostPayload::GhostAction::CLEAR:
  case GhostPayload::GhostAction::NONE:
    mr_ghost.m_selection = std::monostate{};
    break;
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
void ClearGhostSelection(MrGhost &mr_ghost) {
  mr_ghost.m_selection = std::monostate{};
}

} // namespace steamrot::logic::action::ghost
