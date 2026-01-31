/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for running the test harness.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "harness_runner.h"
#include "EntityTransportVariant.h"
#include "EventHandler.h"
#include "FailInfo.h"
#include "FlatbuffersTestDataProvider.h"
#include "TestEngine.h"
#include "add_uuids.h"
#include "containers.h"
#include <variant>

namespace steamrot::tests {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
RunHarnessTests(const std::filesystem::path current_location) {

  // check if the adjacent data directory exists
  std::filesystem::path data_dir = current_location / "data";
  if (!std::filesystem::exists(data_dir) |
      !std::filesystem::is_directory(data_dir)) {
    return std::unexpected(FailInfo{
        FailMode::DirectoryNotFound,
        std::format("No adjacent 'data' directory found at {}",
                    data_dir.string()),
    });
  }

  // Create an EventHandler for loading test data
  EventHandler event_handler;

  // instantiate the FlatbuffersTestDataProvider with the object directory path
  FlatbuffersTestDataProvider data_provider(current_location, event_handler);

  // get all test data
  auto test_data_result = data_provider.ProvideAllTestData();
  if (!test_data_result) {
    return std::unexpected(test_data_result.error());
  }

  // check if any test data was found
  auto &test_data_vec = test_data_result.value();
  if (test_data_vec.empty()) {
    return std::unexpected(FailInfo{
        FailMode::None,
        "No test data found in the 'data' directory.",
    });
  }

  // modify data
  for (auto &test_data : test_data_vec) {

    auto add_uuids_result = add_uuids(test_data);
    if (!add_uuids_result) {
      return std::unexpected(add_uuids_result.error());
    }
  }
  // cycle through all test data and run tests
  for (const auto &test_data : test_data_vec) {

    // create TestEngine instance
    TestEngine test_engine{test_data};

    // start up the test engine
    auto start_up_result = test_engine.StartUp();
    if (!start_up_result) {
      return std::unexpected(start_up_result.error());
    }
    // run the game
    auto run_result = test_engine.RunGame();
    if (!run_result) {
      return std::unexpected(run_result.error());
    }
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConvertEMPData(SceneData &scene_data) {

  // check the SceneData entity configurator is not null
  if (scene_data.entity_configurator == nullptr) {
    return std::unexpected(FailInfo{
        FailMode::NullPointer,
        "SceneData entity_configurator is null.",
    });
  }

  // create fresh EntityMemoryPool
  EntityMemoryPool emp;
  // configure the EMP from the SceneData entity configurator
  auto configure_result =
      scene_data.entity_configurator->ConfigureEntityMemoryPoolFromSource(
          emp, scene_data.entity_transport);

  // assign the configured EMP back to the SceneData
  if (!configure_result) {
    return std::unexpected(configure_result.error());
  }

  scene_data.entity_transport = std::move(emp);

  return std::monostate{};
}
} // namespace steamrot::tests
