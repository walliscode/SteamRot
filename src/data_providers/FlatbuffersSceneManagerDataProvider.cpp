/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersSceneManagerDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneManagerDataProvider.h"
#include "FlatbuffersSubscriberViewer.h"
#include "scene_manager_data_generated.h"

namespace steamrot {

/////////////////////////////////////////////////
std::expected<SceneManagerState, FailInfo>
FlatbuffersSceneManagerDataProvider::LoadSceneManagerState() const {

  // create SceneManagerState object
  SceneManagerState scene_manager_state;
  // configure SceneManagerState object

  // get SubscriptionViewer
  auto subscriber_viewer_result = GetSubscriberViewer();
  if (!subscriber_viewer_result) {
    return std::unexpected(subscriber_viewer_result.error());
  }

  // get Subscribers from viewer and set to SceneManagerState
  auto subscriber_result = subscriber_viewer_result.value()->GetSubscribers();
  if (!subscriber_result) {
    return std::unexpected(subscriber_result.error());
  }
  scene_manager_state.subscriptions = subscriber_result.value();

  // return configured SceneManagerState object
  return scene_manager_state;
}

/////////////////////////////////////////////////
std::expected<std::unique_ptr<ISubscriberViewer>, FailInfo>
FlatbuffersSceneManagerDataProvider::GetSubscriberViewer() const {

  // load SceneManagerdata
  auto scene_manager_data_result = m_loader.ProvideSceneManagerData();
  if (!scene_manager_data_result) {
    return std::unexpected(scene_manager_data_result.error());
  }
  // asign data to local variable
  const SceneManagerStateFbs &scene_manager_data =
      *scene_manager_data_result.value()->state();

  // psas subscritption data to viewer and return
  return std::make_unique<FlatbuffersSubscriberViewer>(
      scene_manager_data.subscriptions());
}
} // namespace steamrot
