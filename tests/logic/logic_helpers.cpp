/////////////////////////////////////////////////
/// @file
/// @brief Implementation of helper functions for logic tests
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "logic_helpers.h"
#include "CraftingRenderLogic.h"
#include "LogicFactory.h"
#include "UIActionLogic.h"
#include "UICollisionLogic.h"
#include "UIRenderLogic.h"
#include "UIStateLogic.h"
#include "catch2/catch_test_macros.hpp"
#include "scene_change_packet_generated.h"
namespace steamrot::tests {

/////////////////////////////////////////////////
void CheckStaticLogicCollections(const steamrot::LogicCollection &collection,
                                 const steamrot::SceneType &scene_type) {
  switch (scene_type) {
  case steamrot::SceneType::SceneType_TEST: {
    // general map properties
    REQUIRE(collection.size() == 3);
    // Check for specific LogicVectors
    REQUIRE(collection.find(steamrot::LogicType::Action) != collection.end());
    REQUIRE(collection.find(steamrot::LogicType::Collision) !=
            collection.end());
    REQUIRE(collection.find(steamrot::LogicType::Render) != collection.end());
    // Evaluate actions logics
    const steamrot::LogicVector &action_logics =
        collection.at(steamrot::LogicType::Action);
    REQUIRE(action_logics.size() == 2);
    REQUIRE(dynamic_cast<steamrot::UIStateLogic *>(action_logics[0].get()));
    REQUIRE(dynamic_cast<steamrot::UIActionLogic *>(action_logics[1].get()));
    // Evaluate collision logics
    const steamrot::LogicVector &collision_logics =
        collection.at(steamrot::LogicType::Collision);
    REQUIRE(collision_logics.size() == 1);
    REQUIRE(
        dynamic_cast<steamrot::UICollisionLogic *>(collision_logics[0].get()));
    // Evaluate render logics
    const steamrot::LogicVector &render_logics =
        collection.at(steamrot::LogicType::Render);
    REQUIRE(render_logics.size() == 1);
    REQUIRE(dynamic_cast<steamrot::UIRenderLogic *>(render_logics[0].get()));

    break;
  }
  case steamrot::SceneType::SceneType_TITLE: {

    // general map properties
    REQUIRE(collection.size() == 3);
    // Check for specific LogicVectors
    REQUIRE(collection.find(steamrot::LogicType::Action) != collection.end());
    REQUIRE(collection.find(steamrot::LogicType::Collision) !=
            collection.end());
    REQUIRE(collection.find(steamrot::LogicType::Render) != collection.end());

    // Evaluate actions logics
    const steamrot::LogicVector &action_logics =
        collection.at(steamrot::LogicType::Action);
    REQUIRE(action_logics.size() == 2);
    REQUIRE(dynamic_cast<steamrot::UIStateLogic *>(action_logics[0].get()));
    REQUIRE(dynamic_cast<steamrot::UIActionLogic *>(action_logics[1].get()));

    // Evaluate collision logics
    const steamrot::LogicVector &collision_logics =
        collection.at(steamrot::LogicType::Collision);
    REQUIRE(collision_logics.size() == 1);
    REQUIRE(
        dynamic_cast<steamrot::UICollisionLogic *>(collision_logics[0].get()));

    break;
  }
  case steamrot::SceneType::SceneType_CRAFTING: {
    // general map properties
    REQUIRE(collection.size() == 3);
    // Check for specific LogicVectors
    REQUIRE(collection.find(steamrot::LogicType::Action) != collection.end());
    REQUIRE(collection.find(steamrot::LogicType::Collision) !=
            collection.end());
    REQUIRE(collection.find(steamrot::LogicType::Render) != collection.end());

    // Evaluate actions logics
    const steamrot::LogicVector &action_logics =
        collection.at(steamrot::LogicType::Action);
    REQUIRE(action_logics.size() == 2);
    REQUIRE(dynamic_cast<steamrot::UIStateLogic *>(action_logics[0].get()));
    REQUIRE(dynamic_cast<steamrot::UIActionLogic *>(action_logics[1].get()));
    // Evaluate collision logics
    const steamrot::LogicVector &collision_logics =
        collection.at(steamrot::LogicType::Collision);
    REQUIRE(collision_logics.size() == 1);
    REQUIRE(
        dynamic_cast<steamrot::UICollisionLogic *>(collision_logics[0].get()));
    // Evaluate render logics
    const steamrot::LogicVector &render_logics =
        collection.at(steamrot::LogicType::Render);
    REQUIRE(render_logics.size() == 2);
    REQUIRE(
        dynamic_cast<steamrot::CraftingRenderLogic *>(render_logics[0].get()));
    REQUIRE(dynamic_cast<steamrot::UIRenderLogic *>(render_logics[1].get()));
    break;
  }
  default: {
    FAIL("Unhandled SceneType in CheckStaticLogicCollections");
  }
  }
}
} // namespace steamrot::tests
