/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the free functions for the configuration of the
/// logic
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_logic_config.h"
#include "subscriber_factory.h"

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
      auto subscriber_result =
          subscriber_factory::CreateSubscriber(subscriber_fbs);
      if (!subscriber_result.has_value()) {
        return std::unexpected(subscriber_result.error());
      }
      logic_config.m_subscribers.push_back(
          std::make_shared<Subscriber>(subscriber_result.value()));
    }
  }

  return std::monostate{};
}

} // namespace steamrot::data::configure
