/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the LogicFactory class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CraftingRenderLogic.h"
#include "Logic.h"
#include "SubscriberFactory.h"
#include "UIActionLogic.h"
#include "UICollisionLogic.h"
#include "UIRenderLogic.h"
#include "UIStateLogic.h"
#include "logic_data_generated.h"
#include "scene_change_packet_generated.h"
#include <expected>
#include <memory>
#include <unordered_map>
#include <variant>
namespace steamrot {

enum class LogicType {
  Collision,
  Render,
  Action,
  Movement,
};
using LogicVector = std::vector<std::unique_ptr<Logic>>;
using LogicCollection = std::unordered_map<LogicType, LogicVector>;
/////////////////////////////////////////////////
/// @class LogicFactory
/// @brief Provides Logic objects for Scenes to store and use.
///
/////////////////////////////////////////////////
class LogicFactory {

private:
  /////////////////////////////////////////////////
  /// @brief SceneType of the scene for which logics are created.
  /////////////////////////////////////////////////
  const SceneType m_scene_type;

  /////////////////////////////////////////////////
  /// @brief member variable that holds the SceneContext
  /////////////////////////////////////////////////
  SceneContext m_scene_context;

  SubscriberFactory m_subscriber_factory;
  /////////////////////////////////////////////////
  /// @brief Attach subscribers to a Logic instance based on LogicData
  ///
  /// @param logic Logic instance to attach subscribers to
  /// @param logic_data LogicData containing subscriber configuration
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  AttachSubscribers(Logic &logic, const LogicData *logic_data);

  template <typename T>
  std::expected<std::monostate, FailInfo>
  ConfigureLogicObject(T &, const LogicVectorData &) {

    // throw error if type is not implemented
    throw std::runtime_error(
        "ConfigureLogicObject not implemented for this Logic type");
  }

  // Collision Logic Specializations
  template <>
  std::expected<std::monostate, FailInfo>
  ConfigureLogicObject(UICollisionLogic &logic_object,
                       const LogicVectorData &logic_vector_data) {
    // if no ui_collision_logic_data present return early
    if (!logic_vector_data.ui_collision_logic_data())
      return std::monostate();
    // Configure subcribers
    auto attach_result = AttachSubscribers(
        logic_object, logic_vector_data.ui_collision_logic_data());
    // UNEXPECTED PROPOGATION
    if (!attach_result.has_value()) {
      return std::unexpected(attach_result.error());
    }
    return std::monostate{};
  }

  // Action Logic Specializations
  template <>
  std::expected<std::monostate, FailInfo>
  ConfigureLogicObject(UIActionLogic &logic_object,
                       const LogicVectorData &logic_vector_data) {
    // if no ui_action_logic_data present return early
    if (!logic_vector_data.ui_action_logic_data())
      return std::monostate();
    // Configure subcribers
    auto attach_result = AttachSubscribers(
        logic_object, logic_vector_data.ui_action_logic_data());
    // UNEXPECTED PROPOGATION
    if (!attach_result.has_value()) {
      return std::unexpected(attach_result.error());
    }
    return std::monostate{};
  }

  template <>
  std::expected<std::monostate, FailInfo>
  ConfigureLogicObject(UIStateLogic &logic_object,
                       const LogicVectorData &logic_vector_data) {
    // if no ui_state_logic_data present return early
    if (!logic_vector_data.ui_state_logic_data())
      return std::monostate();
    // Configure subcribers
    auto attach_result = AttachSubscribers(
        logic_object, logic_vector_data.ui_state_logic_data());
    // UNEXPECTED PROPOGATION
    if (!attach_result.has_value()) {
      return std::unexpected(attach_result.error());
    }
    return std::monostate{};
  }
  // Render Logic Specializations
  template <>
  std::expected<std::monostate, FailInfo>
  ConfigureLogicObject(UIRenderLogic &logic_object,
                       const LogicVectorData &logic_vector_data) {

    // if no ui_render_logic_data present return early
    if (!logic_vector_data.ui_render_logic_data())
      return std::monostate();

    // Configure subcribers
    auto attach_result = AttachSubscribers(
        logic_object, logic_vector_data.ui_render_logic_data());
    // UNEXPECTED PROPOGATION
    if (!attach_result.has_value()) {
      return std::unexpected(attach_result.error());
    }

    return std::monostate{};
  }

  template <>
  std::expected<std::monostate, FailInfo>
  ConfigureLogicObject(CraftingRenderLogic &logic_object,
                       const LogicVectorData &logic_vector_data) {

    // if no crafting_render_logic_data present return early
    if (!logic_vector_data.crafting_render_logic_data())
      return std::monostate();
    // Configure subcribers
    auto attach_result = AttachSubscribers(
        logic_object, logic_vector_data.crafting_render_logic_data());
    // UNEXPECTED PROPOGATION
    if (!attach_result.has_value()) {
      return std::unexpected(attach_result.error());
    }
    return std::monostate{};
  }

  template <typename LogicClass>
  std::expected<std::unique_ptr<LogicClass>, FailInfo>
  CreateAndConfigureLogicObject(const LogicVectorData &logic_vector_data,
                                const SceneContext &scene_context) {

    // create logic object
    auto logic_object = std::make_unique<LogicClass>(scene_context);

    // configure logic object
    auto configuration_result =
        ConfigureLogicObject(*logic_object, logic_vector_data);

    // UNEXPECTED PROPOGATION
    if (!configuration_result.has_value()) {
      return std::unexpected(configuration_result.error());
    }
    return logic_object;
  }

  template <typename T>
  void AddLogicToLogicVector(LogicVector &logic_vector,
                             const LogicVectorData &logic_vector_data) {

    // create Logic object from type and add to vector
    auto logic_object_creation_result =
        CreateAndConfigureLogicObject<T>(logic_vector_data, m_scene_context);
    // UNEXPECTED PROPOGATION
    if (!logic_object_creation_result.has_value()) {
      throw std::runtime_error("Failed to create and configure Logic object");
    }

    // add to vector
    logic_vector.push_back(std::move(logic_object_creation_result.value()));
  }

  /////////////////////////////////////////////////
  /// @brief Createand return an empty LogicCollection configured with
  /// LogicVectors
  ///
  /// @return LogicCollection
  /////////////////////////////////////////////////
  LogicCollection CreateEmptyLogicCollection();

  std::expected<std::monostate, FailInfo> TitleSceneLogicConfiguration(
      LogicCollection &logic_collection,
      const LogicCollectionData &logic_collection_data);

  std::expected<std::monostate, FailInfo> CraftingSceneLogicConfiguration(
      LogicCollection &logic_collection,
      const LogicCollectionData &logic_collection_data);

  std::expected<std::monostate, FailInfo>
  TestSceneLogicConfiguration(LogicCollection &logic_collection,
                              const LogicCollectionData &logic_collection_data);

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for the LogicFactory class.
  ///
  /// @param scene_type SceneType of the scene for which logics are created
  /// @param scene_context SceneContext object containing references to the
  /// scene
  /////////////////////////////////////////////////
  LogicFactory(const SceneType scene_type, const SceneContext &scene_context);

  /////////////////////////////////////////////////
  /// @brief Create and return a map of logic objects.
  ///
  /////////////////////////////////////////////////
  std::expected<LogicCollection, FailInfo>
  CreateLogicMap(const LogicCollectionData &logic_collection_data);
};
} // namespace steamrot
