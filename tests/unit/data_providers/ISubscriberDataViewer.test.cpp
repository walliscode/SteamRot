/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for ISubscriberDataViewer interface
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ISubscriberDataViewer.h"
#include "subscriber_config_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <flatbuffers/flatbuffers.h>

// Mock data provider implementing ISubscriberDataViewer
class MockDataProviderWithSubscribers : public steamrot::ISubscriberDataViewer {
private:
  std::vector<steamrot::SubscriberConfig> m_configs;

public:
  MockDataProviderWithSubscribers(
      std::vector<steamrot::SubscriberConfig> configs)
      : m_configs(std::move(configs)) {}

  std::expected<std::vector<steamrot::SubscriberConfig>, steamrot::FailInfo>
  GetSubscriberConfigs() const override {
    return m_configs;
  }
};

TEST_CASE("ISubscriberDataViewer can be used as mixin interface",
          "[unit][data_providers][ISubscriberDataViewer]") {
  // Create test configs
  std::vector<steamrot::SubscriberConfig> configs;
  steamrot::SubscriberConfig config;
  config.trigger_event_type = steamrot::EventType::EventType_EVENT_USER_INPUT;
  config.active = true;
  configs.push_back(config);

  // Create mock provider
  MockDataProviderWithSubscribers provider(configs);

  // Should be able to access through interface
  steamrot::ISubscriberDataViewer *viewer = &provider;
  auto result = viewer->GetSubscriberConfigs();

  REQUIRE(result.has_value());
  REQUIRE(result.value().size() == 1);
  REQUIRE(result.value()[0].trigger_event_type ==
          steamrot::EventType::EventType_EVENT_USER_INPUT);
}

TEST_CASE("ISubscriberDataViewer enables layered data access",
          "[unit][data_providers][ISubscriberDataViewer]") {
  // Simulate multiple data providers with subscriber data
  std::vector<steamrot::SubscriberConfig> layer1_configs;
  steamrot::SubscriberConfig config1;
  config1.trigger_event_type = steamrot::EventType::EventType_EVENT_USER_INPUT;
  config1.active = true;
  layer1_configs.push_back(config1);

  std::vector<steamrot::SubscriberConfig> layer2_configs;
  steamrot::SubscriberConfig config2;
  config2.trigger_event_type = steamrot::EventType::EventType_EVENT_TOGGLE_UI;
  config2.active = false;
  layer2_configs.push_back(config2);

  // Create multiple providers
  MockDataProviderWithSubscribers layer1(layer1_configs);
  MockDataProviderWithSubscribers layer2(layer2_configs);

  // Collect all viewers in a container
  std::vector<steamrot::ISubscriberDataViewer *> viewers = {&layer1, &layer2};

  // Access subscriber data from all layers
  std::vector<steamrot::SubscriberConfig> all_configs;
  for (auto *viewer : viewers) {
    auto result = viewer->GetSubscriberConfigs();
    if (result.has_value()) {
      const auto &configs = result.value();
      all_configs.insert(all_configs.end(), configs.begin(), configs.end());
    }
  }

  // Should have collected configs from both layers
  REQUIRE(all_configs.size() == 2);
  REQUIRE(all_configs[0].trigger_event_type ==
          steamrot::EventType::EventType_EVENT_USER_INPUT);
  REQUIRE(all_configs[1].trigger_event_type ==
          steamrot::EventType::EventType_EVENT_TOGGLE_UI);
}

TEST_CASE("ISubscriberDataViewer can return empty vector",
          "[unit][data_providers][ISubscriberDataViewer]") {
  // Create provider with no configs
  MockDataProviderWithSubscribers provider({});

  auto result = provider.GetSubscriberConfigs();

  REQUIRE(result.has_value());
  REQUIRE(result.value().empty());
}
