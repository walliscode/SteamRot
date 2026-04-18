/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for processing actions related to
/// MrGhost.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AssetManager.h"
#include "MrGhost.h"
#include "Subscriber.h"

namespace steamrot::logic::action::ghost {

/////////////////////////////////////////////////
/// @brief Resolve a GhostSelection tag to an instance and apply it to
/// MrGhost.
///
/// Looks up the Fragment or Joint identified by @p selection in @p
/// asset_manager, constructs the corresponding FragmentInstance or
/// JointInstance, and stores it in mr_ghost.m_instance. If the tag cannot
/// be resolved (e.g. GrimoireMachina is unavailable or the key is missing),
/// mr_ghost is left unchanged.
///
/// @param mr_ghost     MrGhost instance to update.
/// @param selection    GhostSelection tag identifying the chosen item.
/// @param asset_manager AssetManager used to resolve the tag to a Part
///                     definition.
/////////////////////////////////////////////////
void SelectGhostItem(MrGhost &mr_ghost, const GhostSelection &selection,
                     AssetManager &asset_manager);

/////////////////////////////////////////////////
/// @brief Clear the current ghost instance on MrGhost.
///
/// Sets mr_ghost.m_instance to std::monostate, indicating no active
/// selection. Callers are responsible for choosing this function when
/// the relevant GhostPayload::GhostAction is CLEAR or NONE.
///
/// @param mr_ghost MrGhost instance whose selection will be cleared.
/////////////////////////////////////////////////
void ClearGhostSelection(MrGhost &mr_ghost);

/////////////////////////////////////////////////
/// @brief Dispatch a single active GHOST subscriber to the correct action
/// free function.
///
/// Guards against a missing or mis-typed captured payload, then switches on
/// GhostPayload::GhostAction to call SelectGhostItem (SELECT) or
/// ClearGhostSelection (CLEAR / NONE). Callers are responsible for checking
/// that @p subscriber is active before passing it in.
///
/// @param subscriber    Active subscriber whose captured_payload holds the
///                      action.
/// @param mr_ghost      MrGhost instance to mutate.
/// @param asset_manager AssetManager used to resolve a SELECT tag.
/////////////////////////////////////////////////
void ProcessSubscriber(Subscriber &subscriber, MrGhost &mr_ghost,
                       AssetManager &asset_manager);

/////////////////////////////////////////////////
/// @brief Process all active subscribers and apply their ghost actions to
/// MrGhost.
///
/// Iterates the subscriber list, skips inactive entries, and dispatches each
/// active subscriber to @ref ProcessSubscriber.
///
/// @param subscribers   Subscribers owned by the Logic instance.
/// @param mr_ghost      MrGhost instance to mutate.
/// @param asset_manager AssetManager used to resolve SELECT tags.
/////////////////////////////////////////////////
void ProcessSubscribers(
    const std::vector<std::shared_ptr<Subscriber>> &subscribers,
    MrGhost &mr_ghost, AssetManager &asset_manager);

} // namespace steamrot::logic::action::ghost
