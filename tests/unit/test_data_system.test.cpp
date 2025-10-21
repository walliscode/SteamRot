/////////////////////////////////////////////////
/// @file
/// @brief Example tests demonstrating the test data system
/////////////////////////////////////////////////

#include "TestDataLoader.h"
#include "test_comparisons.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TestDataLoader can be constructed", "[unit][test_data]") {
  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  steamrot::tests::TestDataLoader loader(path_provider);

  REQUIRE(!loader.IsLoaded());
  REQUIRE(loader.GetTestCase() == nullptr);
  REQUIRE(loader.GetMetadata() == nullptr);
  REQUIRE(loader.GetIntermediateStateCount() == 0);
}

TEST_CASE("ComparisonResult can be created", "[unit][test_data]") {
  steamrot::tests::ComparisonResult result(true);
  REQUIRE(result.passed);
  REQUIRE(result.message.empty());
  REQUIRE(result.differences.empty());

  steamrot::tests::ComparisonResult failed_result(false, "Test failed");
  REQUIRE(!failed_result.passed);
  REQUIRE(failed_result.message == "Test failed");
}

TEST_CASE("CreateFailedComparison creates proper result",
          "[unit][test_data]") {
  std::vector<std::string> diffs = {"Difference 1", "Difference 2"};
  auto result = steamrot::tests::CreateFailedComparison(diffs);

  REQUIRE(!result.passed);
  REQUIRE(result.differences.size() == 2);
  REQUIRE(result.differences[0] == "Difference 1");
  REQUIRE(result.differences[1] == "Difference 2");
  REQUIRE(!result.message.empty());
}

TEST_CASE("CompareEntityMemoryPools detects size differences",
          "[unit][test_data]") {
  steamrot::EntityMemoryPool pool1;
  steamrot::EntityMemoryPool pool2;

  // Resize pool1 to 10 entities
  std::apply(
      [](auto &...component_vector) { (component_vector.resize(10), ...); },
      pool1);

  // Resize pool2 to 5 entities
  std::apply(
      [](auto &...component_vector) { (component_vector.resize(5), ...); },
      pool2);

  auto result = steamrot::tests::CompareEntityMemoryPools(pool1, pool2);
  REQUIRE(!result.passed);
  REQUIRE(result.differences.size() > 0);
}

TEST_CASE("CompareEntityMemoryPools passes for identical pools",
          "[unit][test_data]") {
  steamrot::EntityMemoryPool pool1;
  steamrot::EntityMemoryPool pool2;

  // Resize both to same size
  std::apply(
      [](auto &...component_vector) { (component_vector.resize(10), ...); },
      pool1);
  std::apply(
      [](auto &...component_vector) { (component_vector.resize(10), ...); },
      pool2);

  auto result = steamrot::tests::CompareEntityMemoryPools(pool1, pool2);
  REQUIRE(result.passed);
}

TEST_CASE("CompareEventBuses detects size differences", "[unit][test_data]") {
  steamrot::EventBus bus1;
  steamrot::EventBus bus2;

  bus1.push_back(steamrot::EventPacket{steamrot::EventType::EVENT_TEST, {}});
  bus1.push_back(
      steamrot::EventPacket{steamrot::EventType::EVENT_USER_INPUT, {}});

  bus2.push_back(steamrot::EventPacket{steamrot::EventType::EVENT_TEST, {}});

  auto result = steamrot::tests::CompareEventBuses(bus1, bus2);
  REQUIRE(!result.passed);
  REQUIRE(result.differences.size() > 0);
}

TEST_CASE("CompareEventBuses passes for identical buses",
          "[unit][test_data]") {
  steamrot::EventBus bus1;
  steamrot::EventBus bus2;

  bus1.push_back(steamrot::EventPacket{steamrot::EventType::EVENT_TEST, {}});
  bus2.push_back(steamrot::EventPacket{steamrot::EventType::EVENT_TEST, {}});

  auto result = steamrot::tests::CompareEventBuses(bus1, bus2);
  REQUIRE(result.passed);
}

TEST_CASE("CompareEventBuses detects event type differences",
          "[unit][test_data]") {
  steamrot::EventBus bus1;
  steamrot::EventBus bus2;

  bus1.push_back(steamrot::EventPacket{steamrot::EventType::EVENT_TEST, {}});
  bus2.push_back(
      steamrot::EventPacket{steamrot::EventType::EVENT_USER_INPUT, {}});

  auto result = steamrot::tests::CompareEventBuses(bus1, bus2);
  REQUIRE(!result.passed);
  REQUIRE(result.differences.size() > 0);
}
