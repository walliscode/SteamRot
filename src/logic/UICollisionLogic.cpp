#include "UICollisionLogic.h"
#include "CUserInterface.h"
#include "archetypes.h"
#include "collision_mouse.h"
#include "entity_memory.h"
#include <SFML/Window/Mouse.hpp>
#include <array>
#include <set>

namespace steamrot::logic {
/////////////////////////////////////////////////
UICollisionLogic::UICollisionLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void UICollisionLogic::ProcessLogic() {

  std::set<size_t> entity_index_set =
      archetypes::GenerateEntityIndexesFromComponents<CUserInterface>(
          m_scene_context.archetypes, true);
  std::vector<size_t> entity_indexes(entity_index_set.begin(),
                                     entity_index_set.end());

  // Clear all hover state to remove stale state from previous frame.
  for (size_t entity_id : entity_indexes) {
    CUserInterface &ui_component = entity::memory::GetComponent<CUserInterface>(
        entity_id, m_scene_context.scene_entities);
    collision::mouse::ClearMouseOver(*ui_component.m_root_element);
  }
  m_scene_context.scene_state.is_mouse_over_ui_layer = false;

  static constexpr std::array k_collision_pass_order{
      UIPriorityTier::Modal, UIPriorityTier::Elevated, UIPriorityTier::Normal,
      UIPriorityTier::Background};

  bool higher_tier_claimed_mouse = false;
  for (const UIPriorityTier tier : k_collision_pass_order) {
    collision::mouse::CheckMouseOverAllCUserInterfaceComponentsInTier(
        entity_indexes, m_scene_context.scene_entities,
        m_scene_context.mouse_position, tier, higher_tier_claimed_mouse,
        m_scene_context.scene_state.is_mouse_over_ui_layer);
  }
}

} // namespace steamrot::logic
