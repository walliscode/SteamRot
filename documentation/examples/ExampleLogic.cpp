/////////////////////////////////////////////////
/// @file
/// @brief Example Logic class implementation demonstrating best practices
///
/// This file shows recommended patterns for Logic implementation:
/// - Archetype-based entity processing with exact and partial matching
/// - Component access and modification
/// - Safe handling of missing archetypes
/// - Interaction with game systems
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ExampleLogic.h"
#include "ArchetypeUtils.h"
#include "ArchetypeManager.h"
#include "CYourComponent.h"
#include "entity_memory.h"

namespace steamrot {

/////////////////////////////////////////////////
ExampleLogic::ExampleLogic(const SceneContext scene_context)
    : Logic(scene_context) {
  // Constructor typically just calls base class constructor
  // All logic should be in ProcessLogic()
}

/////////////////////////////////////////////////
void ExampleLogic::ProcessLogic() {

  // PATTERN 1: Exact Match - Single Component Archetype
  // ====================================================
  // Gather entities with EXACTLY the specified component(s)
  // Use exact_match=true (default) to get only entities with 
  // precisely the listed components and no others
  std::set<size_t> exact_entity_indices = 
      GatherEntityIndices<CYourComponent>(m_scene_context.archetypes, true);

  // Process each entity in the archetype
  for (size_t entity_id : exact_entity_indices) {

    // Get the component for this entity
    CYourComponent &component = entity::memory::GetComponent<CYourComponent>(
        entity_id, m_scene_context.scene_entities);

    // Example: Modify component state
    if (component.m_active) {
      component.m_some_value += 1;
      component.m_some_flag = true;
    }
  }

  // PATTERN 1b: Partial Match - Single Component Archetype
  // =======================================================
  // Gather entities with AT LEAST the specified component(s)
  // Use exact_match=false to get all entities that have the component,
  // regardless of what other components they may also have
  std::set<size_t> partial_entity_indices = 
      GatherEntityIndices<CYourComponent>(m_scene_context.archetypes, false);

  // This is useful when you want to process a component but don't care
  // about what other components the entity might have
  for (size_t entity_id : partial_entity_indices) {
    CYourComponent &component = entity::memory::GetComponent<CYourComponent>(
        entity_id, m_scene_context.scene_entities);

    // Process component regardless of entity's other components
    component.m_some_value += 1;
  }

  // PATTERN 2: Exact Match - Multiple Component Archetype
  // ======================================================
  // For entities that need multiple specific components
  // exact_match=true ensures entities have ONLY these components
  std::set<size_t> multi_exact_indices = 
      GatherEntityIndices<CComponent1, CComponent2>(m_scene_context.archetypes, true);

  for (size_t entity_id : multi_exact_indices) {
    // Get both components
    CComponent1 &comp1 = entity::memory::GetComponent<CComponent1>(
        entity_id, m_scene_context.scene_entities);
    CComponent2 &comp2 = entity::memory::GetComponent<CComponent2>(
        entity_id, m_scene_context.scene_entities);

    // Use both components in logic
    comp1.m_position.x += comp2.m_velocity.x;
    comp1.m_position.y += comp2.m_velocity.y;
  }

  // PATTERN 2b: Partial Match - Multiple Component Archetype
  // =========================================================
  // Get all entities that have AT LEAST these components
  // They may have additional components as well
  std::set<size_t> multi_partial_indices = 
      GatherEntityIndices<CComponent1, CComponent2>(m_scene_context.archetypes, false);

  for (size_t entity_id : multi_partial_indices) {
    CComponent1 &comp1 = entity::memory::GetComponent<CComponent1>(
        entity_id, m_scene_context.scene_entities);
    CComponent2 &comp2 = entity::memory::GetComponent<CComponent2>(
        entity_id, m_scene_context.scene_entities);

    // Process regardless of other components the entity might have
    comp1.m_position.x += comp2.m_velocity.x;
    comp1.m_position.y += comp2.m_velocity.y;
  }

  // PATTERN 3: Rendering Logic
  // ===========================
  // Drawing to the scene render texture
  // Use exact_match=true when you only want pure render entities
  std::set<size_t> render_indices = 
      GatherEntityIndices<CRenderComponent>(m_scene_context.archetypes, true);

  for (size_t entity_id : render_indices) {
    CRenderComponent &render_comp = entity::memory::GetComponent<CRenderComponent>(
        entity_id, m_scene_context.scene_entities);

    if (render_comp.m_visible) {
      // Access render texture from context
      // sf::Sprite sprite = ...;
      // sprite.setPosition(render_comp.m_position);
      // m_scene_context.scene_texture.draw(sprite);
    }
  }

  // PATTERN 4: Collision/Mouse Interaction Logic
  // =============================================
  // Using mouse position for collision detection
  // Use exact_match=false to process ALL UI elements regardless of other components
  std::set<size_t> ui_indices = 
      GatherEntityIndices<CUserInterface>(m_scene_context.archetypes, false);

  for (size_t entity_id : ui_indices) {
    CUserInterface &ui_comp = entity::memory::GetComponent<CUserInterface>(
        entity_id, m_scene_context.scene_entities);

    // Access mouse position from context
    sf::Vector2i mouse_pos = m_scene_context.mouse_position;

    // Check if mouse is over element
    if (ui_comp.m_root_element) {
      sf::FloatRect bounds(ui_comp.m_root_element->position, 
                          ui_comp.m_root_element->size);
      
      if (bounds.contains(static_cast<sf::Vector2f>(mouse_pos))) {
        ui_comp.m_root_element->is_mouse_over = true;
      } else {
        ui_comp.m_root_element->is_mouse_over = false;
      }
    }
  }

  // PATTERN 5: Event-Based Logic
  // =============================
  // Triggering events based on game state
  std::set<size_t> action_indices = 
      GatherEntityIndices<CActionComponent>(m_scene_context.archetypes, true);

  for (size_t entity_id : action_indices) {
    CActionComponent &action_comp = entity::memory::GetComponent<CActionComponent>(
        entity_id, m_scene_context.scene_entities);

    // Check if action should be triggered
    if (action_comp.m_should_trigger) {
      // Create and add event to event handler
      EventPacket event_packet{EventType_EVENT_YOUR_EVENT, 
                              std::monostate()};
      m_scene_context.event_handler.AddEvent(event_packet);
      
      // Reset trigger flag
      action_comp.m_should_trigger = false;
    }
  }

  // PATTERN 6: Asset Manager Access
  // ================================
  // Using assets from the asset manager
  std::set<size_t> sprite_indices = 
      GatherEntityIndices<CSpriteComponent>(m_scene_context.archetypes, true);

  for (size_t entity_id : sprite_indices) {
    CSpriteComponent &sprite_comp = entity::memory::GetComponent<CSpriteComponent>(
        entity_id, m_scene_context.scene_entities);

    // Access textures from asset manager
    // const sf::Texture* texture = 
    //     m_scene_context.asset_manager.GetTexture(sprite_comp.m_texture_id);
    // if (texture) {
    //   sprite_comp.m_sprite.setTexture(*texture);
    // }
  }

  // PATTERN 7: Conditional Processing
  // ==================================
  // Only process entities that meet certain criteria
  std::set<size_t> conditional_indices = 
      GatherEntityIndices<CConditionalComponent>(m_scene_context.archetypes, true);

  for (size_t entity_id : conditional_indices) {
    CConditionalComponent &cond_comp = 
        entity::memory::GetComponent<CConditionalComponent>(
            entity_id, m_scene_context.scene_entities);

    // Only process active components
    if (!cond_comp.m_active) {
      continue;
    }

    // Additional conditions
    if (cond_comp.m_value > 100) {
      cond_comp.m_state = ComponentState::Critical;
    } else if (cond_comp.m_value > 50) {
      cond_comp.m_state = ComponentState::Warning;
    } else {
      cond_comp.m_state = ComponentState::Normal;
    }
  }

  // PATTERN 8: Advanced Usage - Combining Results
  // ==============================================
  // You can use set operations to combine results from multiple queries
  std::set<size_t> renderable_ui = 
      GatherEntityIndices<CUserInterface, CRenderComponent>(
          m_scene_context.archetypes, false);

  // This gives all entities that have BOTH CUserInterface AND CRenderComponent
  // plus potentially other components
  for (size_t entity_id : renderable_ui) {
    // Process entities with both components
    CUserInterface &ui = entity::memory::GetComponent<CUserInterface>(
        entity_id, m_scene_context.scene_entities);
    CRenderComponent &render = entity::memory::GetComponent<CRenderComponent>(
        entity_id, m_scene_context.scene_entities);
    
    // Combine UI and rendering logic
    if (ui.m_visible && render.m_visible) {
      // Draw UI element with render component properties
    }
  }
}

} // namespace steamrot
