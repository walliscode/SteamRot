/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersSceneManagerDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneManagerDataProvider.h"
#include "FlatbuffersDataLoader.h"
#include "subscriber_factory.h"

namespace steamrot {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersSceneManagerDataProvider::ConfigureSceneManagerState(
    SceneManagerState &state, const SceneManagerStateFbs *state_data) const {

  // Check for null data
  if (!state_data)
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "SceneManagerStateFbs data is null"});

  //  populate subscriptions
  for (const SubscriberFbs *subscriber_fbs : *state_data->subscriptions()) {
    auto create_result = subscriber_factory::CreateSubscriber(subscriber_fbs);
    if (!create_result.has_value()) {
      return std::unexpected(create_result.error());
    }
    state.subscriptions.push_back(
        std::make_shared<Subscriber>(create_result.value()));
  }
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<SceneManagerData, FailInfo>
FlatbuffersSceneManagerDataProvider::ProvideSceneManagerData() const {

  // Load default SceneManagerData from file
  FlatbuffersDataLoader data_loader;
  auto result = data_loader.ProvideSceneManagerData();
  if (!result.has_value()) {
    return std::unexpected(result.error());
  }
  const SceneManagerDataFbs *scene_manager_data_fbs = result.value();
  if (!scene_manager_data_fbs) {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "SceneManagerDataFbs data is null"});
  }

  // create SceneManagerData to populate
  SceneManagerData scene_manager_data;

  // configure SceneManagerState
  auto configure_state_result = ConfigureSceneManagerState(
      scene_manager_data.scene_manager_state, scene_manager_data_fbs->state());

  return scene_manager_data;
}

} // namespace steamrot
