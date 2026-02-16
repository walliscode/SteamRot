/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the free functions for the configuration of the
/// logic
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_logic_config.h"
#include "configure_subscriber.h"

namespace steamrot::data::configure {
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureLogicConfig(LogicConfig &logic_config,
                     const LogicConfigFbs *logic_config_fbs) {

  if (!logic_config_fbs) {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "LogicConfigFbs pointer is null."});
  }

  // add subscribers
  if (logic_config_fbs->subscriptions()) {
    for (const auto *subscriber_fbs : *logic_config_fbs->subscriptions()) {

      std::shared_ptr<Subscriber> subscriber = std::make_shared<Subscriber>();
      auto configure_result =
          data::configure::ConfigureSubscriber(*subscriber, subscriber_fbs);
      if (!configure_result.has_value()) {
        return std::unexpected(configure_result.error());
      }

      logic_config.m_subscribers.push_back(subscriber);
    }
  }

  return std::monostate{};
}

} // namespace steamrot::data::configure
