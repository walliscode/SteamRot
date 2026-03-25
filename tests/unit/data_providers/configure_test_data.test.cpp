/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for configure_test_data free functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_test_data.h"
#include "EventHandler.h"
#include "SimulationData.h"
#include "TestData.h"
#include "TestMetaData.h"
#include "simulation_data_generated.h"
#include "test_data_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <flatbuffers/flatbuffers.h>

/////////////////////////////////////////////////
// ConfigureTestMetaData tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureTestMetaData fails with null data",
          "[unit][configure_test_data]") {

  steamrot::TestMetaData test_meta_data;

  auto result =
      steamrot::data::configure::ConfigureTestMetaData(test_meta_data, nullptr);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "Input Flatbuffers TestMetaData is null.");
}

TEST_CASE("ConfigureTestMetaData populates from valid data",
          "[unit][configure_test_data]") {

  steamrot::TestMetaData test_meta_data;

  // Create valid flatbuffers data
  flatbuffers::FlatBufferBuilder builder;
  auto name_offset = builder.CreateString("MyTest");
  auto desc_offset = builder.CreateString("A test description");
  auto meta_offset =
      steamrot::CreateTestMetadataFbs(builder, name_offset, desc_offset);
  builder.Finish(meta_offset);
  const steamrot::TestMetadataFbs *meta_fbs =
      flatbuffers::GetRoot<steamrot::TestMetadataFbs>(
          builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureTestMetaData(test_meta_data,
                                                                 meta_fbs);

  REQUIRE(result.has_value());
  REQUIRE(test_meta_data.test_name == "MyTest");
  REQUIRE(test_meta_data.test_description == "A test description");
}

TEST_CASE("ConfigureTestMetaData populates without optional description",
          "[unit][configure_test_data]") {

  steamrot::TestMetaData test_meta_data;

  flatbuffers::FlatBufferBuilder builder;
  auto name_offset = builder.CreateString("AnotherTest");
  auto meta_offset = steamrot::CreateTestMetadataFbs(builder, name_offset);
  builder.Finish(meta_offset);
  const steamrot::TestMetadataFbs *meta_fbs =
      flatbuffers::GetRoot<steamrot::TestMetadataFbs>(
          builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureTestMetaData(test_meta_data,
                                                                 meta_fbs);

  REQUIRE(result.has_value());
  REQUIRE(test_meta_data.test_name == "AnotherTest");
  REQUIRE(test_meta_data.test_description.empty());
}

/////////////////////////////////////////////////
// ConfigureSimulationData tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureSimulationData fails with null data",
          "[unit][configure_test_data]") {

  steamrot::SimulationData simulation_data;

  auto result = steamrot::data::configure::ConfigureSimulationData(
      simulation_data, nullptr);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message ==
          "Input Flatbuffers SimulationData is null.");
}

TEST_CASE("ConfigureSimulationData fails when logic class is None",
          "[unit][configure_test_data]") {

  steamrot::SimulationData simulation_data;

  flatbuffers::FlatBufferBuilder builder;
  auto step = steamrot::CreateSimulationStepFbs(
      builder, steamrot::LogicClassEnumFbs_None);
  auto steps_offset = builder.CreateVector(&step, 1);
  auto sim_offset = steamrot::CreateSimulationDataFbs(builder, steps_offset);
  builder.Finish(sim_offset);
  const steamrot::SimulationDataFbs *sim_fbs =
      steamrot::GetSimulationDataFbs(builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureSimulationData(
      simulation_data, sim_fbs);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message.find("logic_class_type set to None") !=
          std::string::npos);
}

TEST_CASE("ConfigureSimulationData populates all logic class enums",
          "[unit][configure_test_data]") {

  steamrot::SimulationData simulation_data;

  flatbuffers::FlatBufferBuilder builder;
  std::vector<steamrot::LogicClassEnumFbs> logic_enums = {
      steamrot::LogicClassEnumFbs_UIActionLogic,
      steamrot::LogicClassEnumFbs_UICollisionLogic,
      steamrot::LogicClassEnumFbs_UIRenderLogic,
      steamrot::LogicClassEnumFbs_UIStateLogic,
      steamrot::LogicClassEnumFbs_CraftingRenderLogic};

  std::vector<flatbuffers::Offset<steamrot::SimulationStepFbs>> steps;
  for (const auto &logic_enum : logic_enums) {
    steps.push_back(steamrot::CreateSimulationStepFbs(builder, logic_enum));
  }

  auto steps_offset = builder.CreateVector(steps);
  auto desc_offset = builder.CreateString("Test Simulation");
  auto sim_offset =
      steamrot::CreateSimulationDataFbs(builder, steps_offset, desc_offset);
  builder.Finish(sim_offset);
  const steamrot::SimulationDataFbs *sim_fbs =
      steamrot::GetSimulationDataFbs(builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureSimulationData(
      simulation_data, sim_fbs);

  REQUIRE(result.has_value());
  REQUIRE(simulation_data.description == "Test Simulation");
  REQUIRE(simulation_data.steps.size() == logic_enums.size());
  REQUIRE(simulation_data.steps[0].element ==
          steamrot::LogicClassEnum::UIActionLogic);
  REQUIRE(simulation_data.steps[1].element ==
          steamrot::LogicClassEnum::UICollisionLogic);
  REQUIRE(simulation_data.steps[2].element ==
          steamrot::LogicClassEnum::UIRenderLogic);
  REQUIRE(simulation_data.steps[3].element ==
          steamrot::LogicClassEnum::UIStateLogic);
  REQUIRE(simulation_data.steps[4].element ==
          steamrot::LogicClassEnum::CraftingRenderLogic);
}

/////////////////////////////////////////////////
// ConfigureExpectedEngineSnapshots tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureExpectedEngineSnapshots fails with null data",
          "[unit][configure_test_data]") {

  std::map<size_t, steamrot::EngineSnapshot> expected_snapshots;
  steamrot::EventHandler event_handler;

  auto result = steamrot::data::configure::ConfigureExpectedEngineSnapshots(
      expected_snapshots, nullptr, event_handler);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message ==
          "Input Flatbuffers tick-snapshot pairs vector is null.");
}

TEST_CASE("ConfigureExpectedEngineSnapshots populates map correctly",
          "[unit][configure_test_data]") {

  std::map<size_t, steamrot::EngineSnapshot> expected_snapshots;
  steamrot::EventHandler event_handler;

  flatbuffers::FlatBufferBuilder builder;

  std::vector<flatbuffers::Offset<steamrot::TickSnapshotPairFbs>> pairs;

  auto snapshot1 = steamrot::CreateEngineSnapshotFbs(builder, 5);
  pairs.push_back(steamrot::CreateTickSnapshotPairFbs(builder, 5, snapshot1));

  auto snapshot2 = steamrot::CreateEngineSnapshotFbs(builder, 10);
  pairs.push_back(steamrot::CreateTickSnapshotPairFbs(builder, 10, snapshot2));

  auto pairs_offset = builder.CreateVector(pairs);
  auto meta_offset =
      steamrot::CreateTestMetadataFbs(builder, builder.CreateString("test"));
  auto test_data_offset =
      steamrot::CreateTestDataFbs(builder, meta_offset, 0, 1, 0, pairs_offset);
  builder.Finish(test_data_offset);
  const steamrot::TestDataFbs *test_data_fbs =
      steamrot::GetTestDataFbs(builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureExpectedEngineSnapshots(
      expected_snapshots, test_data_fbs->expected_engine_snapshots(),
      event_handler);

  REQUIRE(result.has_value());
  REQUIRE(expected_snapshots.size() == 2);
  REQUIRE(expected_snapshots.find(5) != expected_snapshots.end());
  REQUIRE(expected_snapshots.find(10) != expected_snapshots.end());
  REQUIRE(expected_snapshots[5].tick_number.value() == 5);
  REQUIRE(expected_snapshots[10].tick_number.value() == 10);
}

/////////////////////////////////////////////////
// ConfigureTestData tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureTestData fails with null data",
          "[unit][configure_test_data]") {

  steamrot::TestData test_data;
  steamrot::EventHandler event_handler;

  auto result = steamrot::data::configure::ConfigureTestData(test_data, nullptr,
                                                             event_handler);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "Input Flatbuffers TestData is null.");
}

TEST_CASE("ConfigureTestData fails when num_ticks is missing",
          "[unit][configure_test_data]") {

  steamrot::TestData test_data;
  steamrot::EventHandler event_handler;

  flatbuffers::FlatBufferBuilder builder;
  auto meta_offset =
      steamrot::CreateTestMetadataFbs(builder, builder.CreateString("Test"));
  auto step = steamrot::CreateSimulationStepFbs(
      builder, steamrot::LogicClassEnumFbs_UIActionLogic);
  auto steps_offset = builder.CreateVector(&step, 1);
  auto sim_offset = steamrot::CreateSimulationDataFbs(builder, steps_offset);
  // num_ticks = 0 (missing)
  auto test_data_offset =
      steamrot::CreateTestDataFbs(builder, meta_offset, sim_offset, 0);
  builder.Finish(test_data_offset);
  const steamrot::TestDataFbs *fbs =
      steamrot::GetTestDataFbs(builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureTestData(test_data, fbs,
                                                             event_handler);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message ==
          "TestDataFbs is missing required field: num_ticks.");
}

TEST_CASE("ConfigureTestData populates from valid data",
          "[unit][configure_test_data]") {

  steamrot::TestData test_data;
  steamrot::EventHandler event_handler;

  flatbuffers::FlatBufferBuilder builder;
  auto meta_offset = steamrot::CreateTestMetadataFbs(
      builder, builder.CreateString("ValidTest"));
  auto step = steamrot::CreateSimulationStepFbs(
      builder, steamrot::LogicClassEnumFbs_UIActionLogic);
  auto steps_offset = builder.CreateVector(&step, 1);
  auto sim_offset = steamrot::CreateSimulationDataFbs(
      builder, steps_offset, builder.CreateString("Sim desc"));
  auto test_data_offset =
      steamrot::CreateTestDataFbs(builder, meta_offset, sim_offset, 5);
  builder.Finish(test_data_offset);
  const steamrot::TestDataFbs *fbs =
      steamrot::GetTestDataFbs(builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureTestData(test_data, fbs,
                                                             event_handler);

  REQUIRE(result.has_value());
  REQUIRE(test_data.meta_data.test_name == "ValidTest");
  REQUIRE(test_data.simulation_data.description == "Sim desc");
  REQUIRE(test_data.simulation_data.steps.size() == 1);
  REQUIRE(test_data.simulation_data.steps[0].element ==
          steamrot::LogicClassEnum::UIActionLogic);
  REQUIRE(test_data.number_of_ticks == 5);
}
