/////////////////////////////////////////////////
/// @file
/// @brief Implementation of functions to configure scene manager data.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_scene_manager_data.h"
#include "configure_subscriber.h"
#include <memory>

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
    std::shared_ptr<Subscriber> subscriber = std::make_shared<Subscriber>();
    auto configure_result =
        data::configure::ConfigureSubscriber(*subscriber, subscriber_fbs);
    if (!configure_result.has_value()) {
      return std::unexpected(configure_result.error());
    }
    state.subscriptions.push_back(subscriber);
  }
  return std::monostate{};
}
} // namespace steamrot::data::configure
