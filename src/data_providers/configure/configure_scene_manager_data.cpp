/////////////////////////////////////////////////
/// @file
/// @brief Implementation of functions to configure scene manager data.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_scene_manager_data.h"
#include "subscriber_factory.h"

namespace steamrot::data::configure {

std::expected<std::monostate, FailInfo>
ConfigureSceneManagerState(SceneManagerState &state,
                           const SceneManagerStateFbs *state_data) {
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
} // namespace steamrot::data::configure
