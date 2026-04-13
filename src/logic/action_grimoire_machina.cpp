/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for process actions related to the
/// GrimoireMachina.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_grimoire_machina.h"
#include "CUserInterface.h"
#include "EventPayload.h"
#include "EventType.h"
#include "MachinaFormScaffold.h"
#include "ViewDirection.h"
#include "archetypes.h"
#include "collision_mouse.h"
#include "entity_memory.h"
#include <SFML/Graphics/Transform.hpp>
#include <string>
#include <vector>

namespace steamrot::logic::action::grimoire_machina {
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
InitialiseActiveMachinaFormScaffold(GrimoireMachina &grimoire_machina) {

  // clear the active form if it exists
  if (grimoire_machina.m_scaffold_form)
    grimoire_machina.m_scaffold_form.reset();
  // add a new MachinaForm to the active form
  grimoire_machina.m_scaffold_form = std::make_unique<MachinaFormScaffold>();

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ClearActiveMachinaFormScaffold(GrimoireMachina &grimoire_machina) {
  // clear the active form if it exists
  if (grimoire_machina.m_scaffold_form)
    grimoire_machina.m_scaffold_form = nullptr;

  return std::monostate{};
}

/////////////////////////////////////////////////
std::vector<std::string>
GetAllFragmentNames(GrimoireMachina &grimoire_machina) {

  std::vector<std::string> fragment_names;
  // cycle through all fragments in the GrimoireMachina and add their names to
  // the vector
  for (const auto &[name, fragment] : grimoire_machina.m_all_fragments) {
    fragment_names.push_back(name);
  }
  return fragment_names;
}

/////////////////////////////////////////////////
std::vector<std::string> GetAllJointNames(GrimoireMachina &grimoire_machina) {

  std::vector<std::string> joint_names;
  // cycle through all joints in the GrimoireMachina and add their names to
  // the vector
  for (const auto &[name, joint] : grimoire_machina.m_all_joints) {
    joint_names.push_back(name);
  }
  return joint_names;
}

/////////////////////////////////////////////////
void ProcessSubscriber(Subscriber &subscriber,
                       GrimoireMachina &grimoire_machina) {
  if (!subscriber.captured_payload.has_value())
    return;

  const LogicPayload *logic_payload =
      std::get_if<LogicPayload>(&subscriber.captured_payload.value());
  if (!logic_payload)
    return;

  if (logic_payload->toggle_name ==
      LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD) {
    // [TODO:] handle the result of this action and report failure if it fails.
    auto initialise_result =
        InitialiseActiveMachinaFormScaffold(grimoire_machina);
  } else if (logic_payload->toggle_name ==
             LogicPayload::LogicToggle::CLEAR_MACHINA_FORM_SCAFFOLD) {
    // [TODO:] handle the result of this action and report failure if it fails.
    auto clear_result = ClearActiveMachinaFormScaffold(grimoire_machina);
  }
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
PlaceFirstPiece(GrimoireMachina &grimoire_machina, const MrGhost &mr_ghost,
                sf::Vector2f world_pos) {

  MachinaFormScaffold *scaffold = grimoire_machina.m_scaffold_form.get();
  if (!scaffold)
    return std::unexpected(FailInfo{FailMode::NullPointer,
                                    "PlaceFirstPiece: no active scaffold"});

  if (!scaffold->fragments.empty() || !scaffold->joints.empty())
    return std::unexpected(
        FailInfo{FailMode::InvalidInput,
                 "PlaceFirstPiece: scaffold is not empty"});

  if (std::holds_alternative<FragmentTag>(mr_ghost.m_selection)) {
    const auto &tag = std::get<FragmentTag>(mr_ghost.m_selection);
    auto it = grimoire_machina.m_all_fragments.find(tag.key);
    if (it == grimoire_machina.m_all_fragments.end())
      return std::unexpected(
          FailInfo{FailMode::MissingData,
                   "PlaceFirstPiece: fragment key not found"});

    Fragment &fragment = it->second;
    const sf::FloatRect bounds =
        fragment.movement_views[ViewDirection::Front].getBounds();

    sf::Transform transform;
    transform.translate(world_pos - bounds.position - bounds.size / 2.f);

    FragmentInstance instance{fragment, transform};
    instance.id = scaffold->next_id++;
    scaffold->fragments.push_back(std::move(instance));

  } else if (std::holds_alternative<JointTag>(mr_ghost.m_selection)) {
    const auto &tag = std::get<JointTag>(mr_ghost.m_selection);
    auto it = grimoire_machina.m_all_joints.find(tag.key);
    if (it == grimoire_machina.m_all_joints.end())
      return std::unexpected(
          FailInfo{FailMode::MissingData,
                   "PlaceFirstPiece: joint key not found"});

    Joint &joint = it->second;
    const sf::FloatRect bounds =
        joint.movement_views[ViewDirection::Front].getBounds();

    sf::Transform transform;
    transform.translate(world_pos - bounds.position - bounds.size / 2.f);

    JointInstance instance{joint, transform};
    instance.id = scaffold->next_id++;
    scaffold->joints.push_back(std::move(instance));

  } else {
    return std::unexpected(
        FailInfo{FailMode::InvalidInput,
                 "PlaceFirstPiece: no ghost item selected"});
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
PlaceGhostOnScaffold(GrimoireMachina &grimoire_machina, const MrGhost &mr_ghost,
                     sf::Vector2f world_pos) {

  MachinaFormScaffold *scaffold = grimoire_machina.m_scaffold_form.get();
  if (!scaffold)
    return std::unexpected(FailInfo{
        FailMode::NullPointer, "PlaceGhostOnScaffold: no active scaffold"});

  if (scaffold->fragments.empty() && scaffold->joints.empty())
    return PlaceFirstPiece(grimoire_machina, mr_ghost, world_pos);

  // [TODO:] Socket-proximity collision detection will determine whether the
  // new piece can connect to an existing open socket on the scaffold.
  return std::unexpected(
      FailInfo{FailMode::InvalidInput,
               "PlaceGhostOnScaffold: no valid socket connection"});
}

/////////////////////////////////////////////////
void ProcessScaffoldSubscribers(
    const std::vector<std::shared_ptr<Subscriber>> &subscribers,
    GrimoireMachina &grimoire_machina) {
  for (const auto &subscriber : subscribers) {
    if (!subscriber->m_active)
      continue;
    ProcessSubscriber(*subscriber, grimoire_machina);
  }
}

/////////////////////////////////////////////////
void ProcessPlacementSubscribers(
    const std::vector<std::shared_ptr<Subscriber>> &subscribers,
    const SceneContext &scene_context, GrimoireMachina &grimoire_machina) {
  for (const auto &subscriber : subscribers) {
    if (!subscriber->m_active)
      continue;
    if (subscriber->event_type != EventType::USER_INPUT)
      continue;

    // Guard 1: a ghost item must be selected (not monostate).
    if (std::holds_alternative<std::monostate>(
            scene_context.mr_ghost.m_selection))
      continue;

    // Guard 2: the click must not land on any visible UI element.
    bool can_place = true;
    const std::vector<size_t> ui_ids =
        archetypes::GetEntitiesSortedByPriority<CUserInterface>(
            scene_context.archetypes, scene_context.scene_entities,
            /*ascending=*/false);
    for (size_t id : ui_ids) {
      const CUserInterface &ui = entity::memory::GetComponent<CUserInterface>(
          id, scene_context.scene_entities);
      if (ui.m_visible &&
          collision::mouse::AnyMouseOver(*ui.m_root_element)) {
        can_place = false;
        break;
      }
    }
    if (!can_place)
      continue;

    // Guard 3: the active scaffold must exist.
    if (!grimoire_machina.m_scaffold_form)
      continue;

    // Guard 4: the click must be inside the crafting canvas.
    if (!collision::mouse::IsMouseOverBounds(
            scene_context.mouse_position,
            grimoire_machina.m_crafting_helpers.crafting_canvas))
      continue;

    // First piece: snap to canvas centre; subsequent pieces: use cursor.
    MachinaFormScaffold *scaffold = grimoire_machina.m_scaffold_form.get();
    const bool is_first_piece =
        scaffold->fragments.empty() && scaffold->joints.empty();

    sf::Vector2f place_pos = scene_context.world_mouse_position;
    if (is_first_piece) {
      const sf::FloatRect &canvas =
          grimoire_machina.m_crafting_helpers.crafting_canvas;
      const sf::Vector2i canvas_center_pixel{
          static_cast<int>(canvas.position.x + canvas.size.x / 2.f),
          static_cast<int>(canvas.position.y + canvas.size.y / 2.f)};
      const sf::View world_view = scene_context.camera_state.GetWorldView(
          scene_context.scene_texture);
      place_pos = scene_context.scene_texture.mapPixelToCoords(
          canvas_center_pixel, world_view);
    }

    // [TODO:] handle the result and report failure if it fails.
    auto place_result =
        PlaceGhostOnScaffold(grimoire_machina, scene_context.mr_ghost, place_pos);
  }
}

/////////////////////////////////////////////////
void ProcessSocketVisibilitySubscribers(
    const std::vector<std::shared_ptr<Subscriber>> &subscribers,
    GrimoireMachina &grimoire_machina) {
  for (const auto &subscriber : subscribers) {
    if (!subscriber->m_active)
      continue;
    if (subscriber->event_type != EventType::USER_INPUT)
      continue;

    if (!subscriber->captured_payload.has_value())
      continue;

    const InputPayload *input_payload =
        std::get_if<InputPayload>(&subscriber->captured_payload.value());
    if (!input_payload)
      continue;
    if (input_payload->action != InputPayload::InputAction::TOGGLE_SOCKET_VISIBILITY)
      continue;

    MachinaFormScaffold *scaffold = grimoire_machina.m_scaffold_form.get();
    if (!scaffold)
      continue;

    scaffold->are_sockets_visible = !scaffold->are_sockets_visible;
  }
}

/////////////////////////////////////////////////
void ProcessSubscribers(
    const std::vector<std::shared_ptr<Subscriber>> &subscribers,
    const SceneContext &scene_context, GrimoireMachina &grimoire_machina) {
  for (const auto &subscriber : subscribers) {
    if (!subscriber->m_active)
      continue;

    if (subscriber->event_type == EventType::LOGIC) {
      ProcessSubscriber(*subscriber, grimoire_machina);
      continue;
    }

    if (subscriber->event_type != EventType::USER_INPUT)
      continue;

    if (!subscriber->captured_payload.has_value())
      continue;

    const InputPayload *input_payload =
        std::get_if<InputPayload>(&subscriber->captured_payload.value());
    if (!input_payload)
      continue;

    if (input_payload->action ==
        InputPayload::InputAction::TOGGLE_SOCKET_VISIBILITY) {
      MachinaFormScaffold *scaffold = grimoire_machina.m_scaffold_form.get();
      if (!scaffold)
        continue;
      scaffold->are_sockets_visible = !scaffold->are_sockets_visible;

    } else if (input_payload->action == InputPayload::InputAction::SELECT) {
      // Guard 1: a ghost item must be selected (not monostate).
      if (std::holds_alternative<std::monostate>(
              scene_context.mr_ghost.m_selection))
        continue;

      // Guard 2: the click must not land on any visible UI element.
      bool can_place = true;
      const std::vector<size_t> ui_ids =
          archetypes::GetEntitiesSortedByPriority<CUserInterface>(
              scene_context.archetypes, scene_context.scene_entities,
              /*ascending=*/false);
      for (size_t id : ui_ids) {
        const CUserInterface &ui = entity::memory::GetComponent<CUserInterface>(
            id, scene_context.scene_entities);
        if (ui.m_visible &&
            collision::mouse::AnyMouseOver(*ui.m_root_element)) {
          can_place = false;
          break;
        }
      }
      if (!can_place)
        continue;

      // Guard 3: the active scaffold must exist.
      if (!grimoire_machina.m_scaffold_form)
        continue;

      // Guard 4: the click must be inside the crafting canvas.
      if (!collision::mouse::IsMouseOverBounds(
              scene_context.mouse_position,
              grimoire_machina.m_crafting_helpers.crafting_canvas))
        continue;

      // First piece: snap to canvas centre; subsequent pieces: use cursor.
      MachinaFormScaffold *scaffold = grimoire_machina.m_scaffold_form.get();
      const bool is_first_piece =
          scaffold->fragments.empty() && scaffold->joints.empty();

      sf::Vector2f place_pos = scene_context.world_mouse_position;
      if (is_first_piece) {
        const sf::FloatRect &canvas =
            grimoire_machina.m_crafting_helpers.crafting_canvas;
        const sf::Vector2i canvas_center_pixel{
            static_cast<int>(canvas.position.x + canvas.size.x / 2.f),
            static_cast<int>(canvas.position.y + canvas.size.y / 2.f)};
        const sf::View world_view = scene_context.camera_state.GetWorldView(
            scene_context.scene_texture);
        place_pos = scene_context.scene_texture.mapPixelToCoords(
            canvas_center_pixel, world_view);
      }

      // [TODO:] handle the result and report failure if it fails.
      (void)PlaceGhostOnScaffold(grimoire_machina, scene_context.mr_ghost,
                                 place_pos);
    }
  }
}

} // namespace steamrot::logic::action::grimoire_machina
