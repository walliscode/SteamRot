/////////////////////////////////////////////////
/// @file
/// @brief Example Logic class implementation demonstrating best practices
///
/// This file shows recommended patterns for Logic implementation:
/// - Archetype-based entity processing
/// - Component access and modification
/// - Safe handling of missing archetypes
/// - Interaction with game systems
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ExampleLogic.h"
#include "ArchetypeHelpers.h"
#include "ArchetypeManager.h"
#include "CYourComponent.h"
#include "emp_helpers.h"

namespace steamrot {

/////////////////////////////////////////////////
ExampleLogic::ExampleLogic(const LogicContext logic_context)
    : Logic(logic_context) {
  // Constructor typically just calls base class constructor
  // All logic should be in ProcessLogic()
}

/////////////////////////////////////////////////
void ExampleLogic::ProcessLogic() {

  // PATTERN 1: Single Component Archetype
  // ======================================
  // Generate archetype ID for entities with specific component(s)
  ArchetypeID archetype_id = GenerateArchetypeIDfromTypes<CYourComponent>();

  // Find the archetype in the context
  const auto it = m_logic_context.archetypes.find(archetype_id);
  
  // Always check if archetype exists before processing
  // Not all scenes will have all archetypes
  if (it != m_logic_context.archetypes.end()) {

    // Get the archetype from the map
    const Archetype &archetype = it->second;

    // Process each entity in the archetype
    for (size_t entity_id : archetype) {

      // Get the component for this entity
      CYourComponent &component = emp_helpers::GetComponent<CYourComponent>(
          entity_id, m_logic_context.scene_entities);

      // Example: Modify component state
      if (component.m_active) {
        component.m_some_value += 1;
        component.m_some_flag = true;
      }
    }
  }

  // PATTERN 2: Multiple Component Archetype
  // ========================================
  // For entities that need multiple components
  ArchetypeID multi_archetype_id = 
      GenerateArchetypeIDfromTypes<CComponent1, CComponent2>();

  const auto multi_it = m_logic_context.archetypes.find(multi_archetype_id);
  
  if (multi_it != m_logic_context.archetypes.end()) {
    const Archetype &multi_archetype = multi_it->second;

    for (size_t entity_id : multi_archetype) {
      // Get both components
      CComponent1 &comp1 = emp_helpers::GetComponent<CComponent1>(
          entity_id, m_logic_context.scene_entities);
      CComponent2 &comp2 = emp_helpers::GetComponent<CComponent2>(
          entity_id, m_logic_context.scene_entities);

      // Use both components in logic
      comp1.m_position.x += comp2.m_velocity.x;
      comp1.m_position.y += comp2.m_velocity.y;
    }
  }

  // PATTERN 3: Rendering Logic
  // ===========================
  // Drawing to the scene render texture
  ArchetypeID render_archetype_id = 
      GenerateArchetypeIDfromTypes<CRenderComponent>();

  const auto render_it = m_logic_context.archetypes.find(render_archetype_id);
  
  if (render_it != m_logic_context.archetypes.end()) {
    const Archetype &render_archetype = render_it->second;

    for (size_t entity_id : render_archetype) {
      CRenderComponent &render_comp = emp_helpers::GetComponent<CRenderComponent>(
          entity_id, m_logic_context.scene_entities);

      if (render_comp.m_visible) {
        // Access render texture from context
        // sf::Sprite sprite = ...;
        // sprite.setPosition(render_comp.m_position);
        // m_logic_context.scene_texture.draw(sprite);
      }
    }
  }

  // PATTERN 4: Collision/Mouse Interaction Logic
  // =============================================
  // Using mouse position for collision detection
  ArchetypeID ui_archetype_id = 
      GenerateArchetypeIDfromTypes<CUserInterface>();

  const auto ui_it = m_logic_context.archetypes.find(ui_archetype_id);
  
  if (ui_it != m_logic_context.archetypes.end()) {
    const Archetype &ui_archetype = ui_it->second;

    for (size_t entity_id : ui_archetype) {
      CUserInterface &ui_comp = emp_helpers::GetComponent<CUserInterface>(
          entity_id, m_logic_context.scene_entities);

      // Access mouse position from context
      sf::Vector2i mouse_pos = m_logic_context.mouse_position;

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
  }

  // PATTERN 5: Event-Based Logic
  // =============================
  // Triggering events based on game state
  ArchetypeID action_archetype_id = 
      GenerateArchetypeIDfromTypes<CActionComponent>();

  const auto action_it = m_logic_context.archetypes.find(action_archetype_id);
  
  if (action_it != m_logic_context.archetypes.end()) {
    const Archetype &action_archetype = action_it->second;

    for (size_t entity_id : action_archetype) {
      CActionComponent &action_comp = emp_helpers::GetComponent<CActionComponent>(
          entity_id, m_logic_context.scene_entities);

      // Check if action should be triggered
      if (action_comp.m_should_trigger) {
        // Create and add event to event handler
        EventPacket event_packet{EventType_EVENT_YOUR_EVENT, 
                                std::monostate()};
        m_logic_context.event_handler.AddEvent(event_packet);
        
        // Reset trigger flag
        action_comp.m_should_trigger = false;
      }
    }
  }

  // PATTERN 6: Asset Manager Access
  // ================================
  // Using assets from the asset manager
  ArchetypeID sprite_archetype_id = 
      GenerateArchetypeIDfromTypes<CSpriteComponent>();

  const auto sprite_it = m_logic_context.archetypes.find(sprite_archetype_id);
  
  if (sprite_it != m_logic_context.archetypes.end()) {
    const Archetype &sprite_archetype = sprite_it->second;

    for (size_t entity_id : sprite_archetype) {
      CSpriteComponent &sprite_comp = emp_helpers::GetComponent<CSpriteComponent>(
          entity_id, m_logic_context.scene_entities);

      // Access textures from asset manager
      // const sf::Texture* texture = 
      //     m_logic_context.asset_manager.GetTexture(sprite_comp.m_texture_id);
      // if (texture) {
      //   sprite_comp.m_sprite.setTexture(*texture);
      // }
    }
  }

  // PATTERN 7: Conditional Processing
  // ==================================
  // Only process entities that meet certain criteria
  ArchetypeID conditional_archetype_id = 
      GenerateArchetypeIDfromTypes<CConditionalComponent>();

  const auto cond_it = m_logic_context.archetypes.find(conditional_archetype_id);
  
  if (cond_it != m_logic_context.archetypes.end()) {
    const Archetype &conditional_archetype = cond_it->second;

    for (size_t entity_id : conditional_archetype) {
      CConditionalComponent &cond_comp = 
          emp_helpers::GetComponent<CConditionalComponent>(
              entity_id, m_logic_context.scene_entities);

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
  }
}

} // namespace steamrot
