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
#include "input_data_generated.h"
#include "simulation_data_generated.h"
#include "test_data_generated.h"
#include "types_generated.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
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

/////////////////////////////////////////////////
// initial_scene_type tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureTestData succeeds without simulation_data",
          "[unit][configure_test_data]") {

  steamrot::TestData test_data;
  steamrot::EventHandler event_handler;

  flatbuffers::FlatBufferBuilder builder;
  auto meta_offset = steamrot::CreateTestMetadataFbs(
      builder, builder.CreateString("NoSimTest"));
  // no simulation_data offset -- pass 0
  auto test_data_offset =
      steamrot::CreateTestDataFbs(builder, meta_offset, 0, 3);
  builder.Finish(test_data_offset);
  const steamrot::TestDataFbs *fbs =
      steamrot::GetTestDataFbs(builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureTestData(test_data, fbs,
                                                             event_handler);

  REQUIRE(result.has_value());
  REQUIRE(test_data.meta_data.test_name == "NoSimTest");
  REQUIRE(test_data.simulation_data.steps.empty());
  REQUIRE(test_data.number_of_ticks == 3);
}

TEST_CASE("ConfigureTestData maps initial_scene_type TITLE correctly",
          "[unit][configure_test_data]") {

  steamrot::TestData test_data;
  steamrot::EventHandler event_handler;

  flatbuffers::FlatBufferBuilder builder;
  auto meta_offset = steamrot::CreateTestMetadataFbs(
      builder, builder.CreateString("DefaultLoadTest"));
  auto test_data_offset = steamrot::CreateTestDataFbs(
      builder, meta_offset, 0, 1, 0, 0, 0, steamrot::SceneTypeFbs_TITLE);
  builder.Finish(test_data_offset);
  const steamrot::TestDataFbs *fbs =
      steamrot::GetTestDataFbs(builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureTestData(test_data, fbs,
                                                             event_handler);

  REQUIRE(result.has_value());
  REQUIRE(test_data.initial_scene_type.has_value());
  REQUIRE(test_data.initial_scene_type.value() == steamrot::SceneType::TITLE);
}

TEST_CASE("ConfigureTestData maps initial_scene_type CRAFTING correctly",
          "[unit][configure_test_data]") {

  steamrot::TestData test_data;
  steamrot::EventHandler event_handler;

  flatbuffers::FlatBufferBuilder builder;
  auto meta_offset = steamrot::CreateTestMetadataFbs(
      builder, builder.CreateString("CraftingDefaultTest"));
  auto test_data_offset = steamrot::CreateTestDataFbs(
      builder, meta_offset, 0, 1, 0, 0, 0, steamrot::SceneTypeFbs_CRAFTING);
  builder.Finish(test_data_offset);
  const steamrot::TestDataFbs *fbs =
      steamrot::GetTestDataFbs(builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureTestData(test_data, fbs,
                                                             event_handler);

  REQUIRE(result.has_value());
  REQUIRE(test_data.initial_scene_type.has_value());
  REQUIRE(test_data.initial_scene_type.value() ==
          steamrot::SceneType::CRAFTING);
}

TEST_CASE("ConfigureTestData leaves initial_scene_type nullopt by default",
          "[unit][configure_test_data]") {

  steamrot::TestData test_data;
  steamrot::EventHandler event_handler;

  flatbuffers::FlatBufferBuilder builder;
  auto meta_offset = steamrot::CreateTestMetadataFbs(
      builder, builder.CreateString("DefaultCheck"));
  auto test_data_offset =
      steamrot::CreateTestDataFbs(builder, meta_offset, 0, 1);
  builder.Finish(test_data_offset);
  const steamrot::TestDataFbs *fbs =
      steamrot::GetTestDataFbs(builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureTestData(test_data, fbs,
                                                             event_handler);

  REQUIRE(result.has_value());
  REQUIRE_FALSE(test_data.initial_scene_type.has_value());
}

TEST_CASE("ConfigureTestData leaves initial_scene_type nullopt when UNKNOWN",
          "[unit][configure_test_data]") {

  steamrot::TestData test_data;
  steamrot::EventHandler event_handler;

  flatbuffers::FlatBufferBuilder builder;
  auto meta_offset = steamrot::CreateTestMetadataFbs(
      builder, builder.CreateString("UnknownSceneTest"));
  // Explicitly passing UNKNOWN (the FlatBuffers default) leaves
  // initial_scene_type as nullopt.
  auto test_data_offset = steamrot::CreateTestDataFbs(
      builder, meta_offset, 0, 1, 0, 0, steamrot::SceneTypeFbs_UNKNOWN);
  builder.Finish(test_data_offset);
  const steamrot::TestDataFbs *fbs =
      steamrot::GetTestDataFbs(builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureTestData(test_data, fbs,
                                                             event_handler);

  REQUIRE(result.has_value());
  REQUIRE_FALSE(test_data.initial_scene_type.has_value());
}

/////////////////////////////////////////////////
// Helper: build a MouseMove InputEventFbs
/////////////////////////////////////////////////

namespace {

/// Builds a MouseMove InputEventFbs with the given position.
/// Returns the root pointer (valid while `builder` is in scope).
const steamrot::InputEventFbs *
BuildMouseMoveEventFbs(flatbuffers::FlatBufferBuilder &builder, float x,
                       float y) {
  auto pos = steamrot::CreateVector2fDataFbs(builder, x, y);
  auto mouse = steamrot::CreateMouseInputDataFbs(builder, pos, 0);
  auto evt = steamrot::CreateInputEventFbs(
      builder, steamrot::InputTypeFbs_MouseMove,
      steamrot::InputEventDataFbs_MouseInputDataFbs, mouse.Union());
  builder.Finish(evt);
  return flatbuffers::GetRoot<steamrot::InputEventFbs>(
      builder.GetBufferPointer());
}

/// Builds a MouseClick InputEventFbs with the given position and button.
const steamrot::InputEventFbs *
BuildMouseClickEventFbs(flatbuffers::FlatBufferBuilder &builder, float x,
                        float y, uint8_t button) {
  auto pos = steamrot::CreateVector2fDataFbs(builder, x, y);
  auto mouse = steamrot::CreateMouseInputDataFbs(builder, pos, button);
  auto evt = steamrot::CreateInputEventFbs(
      builder, steamrot::InputTypeFbs_MouseClick,
      steamrot::InputEventDataFbs_MouseInputDataFbs, mouse.Union());
  builder.Finish(evt);
  return flatbuffers::GetRoot<steamrot::InputEventFbs>(
      builder.GetBufferPointer());
}

/// Builds a MouseRelease InputEventFbs with the given position and button.
const steamrot::InputEventFbs *
BuildMouseReleaseEventFbs(flatbuffers::FlatBufferBuilder &builder, float x,
                          float y, uint8_t button) {
  auto pos = steamrot::CreateVector2fDataFbs(builder, x, y);
  auto mouse = steamrot::CreateMouseInputDataFbs(builder, pos, button);
  auto evt = steamrot::CreateInputEventFbs(
      builder, steamrot::InputTypeFbs_MouseRelease,
      steamrot::InputEventDataFbs_MouseInputDataFbs, mouse.Union());
  builder.Finish(evt);
  return flatbuffers::GetRoot<steamrot::InputEventFbs>(
      builder.GetBufferPointer());
}

/// Builds a KeyPress InputEventFbs with the given key code and modifiers.
const steamrot::InputEventFbs *
BuildKeyPressEventFbs(flatbuffers::FlatBufferBuilder &builder, uint32_t key,
                      bool alt, bool control, bool shift) {
  auto kbd =
      steamrot::CreateKeyboardInputDataFbs(builder, key, alt, control, shift);
  auto evt = steamrot::CreateInputEventFbs(
      builder, steamrot::InputTypeFbs_KeyPress,
      steamrot::InputEventDataFbs_KeyboardInputDataFbs, kbd.Union());
  builder.Finish(evt);
  return flatbuffers::GetRoot<steamrot::InputEventFbs>(
      builder.GetBufferPointer());
}

/// Builds a KeyRelease InputEventFbs with the given key code and modifiers.
const steamrot::InputEventFbs *
BuildKeyReleaseEventFbs(flatbuffers::FlatBufferBuilder &builder, uint32_t key,
                        bool alt, bool control, bool shift) {
  auto kbd =
      steamrot::CreateKeyboardInputDataFbs(builder, key, alt, control, shift);
  auto evt = steamrot::CreateInputEventFbs(
      builder, steamrot::InputTypeFbs_KeyRelease,
      steamrot::InputEventDataFbs_KeyboardInputDataFbs, kbd.Union());
  builder.Finish(evt);
  return flatbuffers::GetRoot<steamrot::InputEventFbs>(
      builder.GetBufferPointer());
}

} // namespace

/////////////////////////////////////////////////
// ToSFMLEvent tests
/////////////////////////////////////////////////

TEST_CASE("ToSFMLEvent fails with null pointer",
          "[unit][configure_test_data]") {
  sf::Event out{sf::Event::MouseMoved{}};
  auto result = steamrot::data::configure::ToSFMLEvent(nullptr, out);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "InputEventFbs pointer is null.");
}

TEST_CASE("ToSFMLEvent MouseMove produces MouseMoved event with correct "
          "position",
          "[unit][configure_test_data]") {
  flatbuffers::FlatBufferBuilder builder;
  const auto *fbs = BuildMouseMoveEventFbs(builder, 100.0f, 200.0f);

  sf::Event out{sf::Event::MouseMoved{}};
  auto result = steamrot::data::configure::ToSFMLEvent(fbs, out);

  REQUIRE(result.has_value());
  REQUIRE(out.is<sf::Event::MouseMoved>());
  const auto *moved = out.getIf<sf::Event::MouseMoved>();
  REQUIRE(moved != nullptr);
  REQUIRE(moved->position.x == 100);
  REQUIRE(moved->position.y == 200);
}

TEST_CASE("ToSFMLEvent MouseMove fails when mouse data is null",
          "[unit][configure_test_data]") {
  flatbuffers::FlatBufferBuilder builder;
  // Build event with NONE union type but set input_type to MouseMove so it
  // passes the switch but has no mouse data.
  auto evt =
      steamrot::CreateInputEventFbs(builder, steamrot::InputTypeFbs_MouseMove,
                                    steamrot::InputEventDataFbs_NONE, 0);
  builder.Finish(evt);
  const auto *fbs =
      flatbuffers::GetRoot<steamrot::InputEventFbs>(builder.GetBufferPointer());

  sf::Event out{sf::Event::MouseMoved{}};
  auto result = steamrot::data::configure::ToSFMLEvent(fbs, out);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message.find("MouseMove") != std::string::npos);
}

TEST_CASE("ToSFMLEvent MouseClick produces MouseButtonPressed event with "
          "correct button and position",
          "[unit][configure_test_data]") {
  flatbuffers::FlatBufferBuilder builder;
  // button 1 = sf::Mouse::Button::Right
  const auto *fbs = BuildMouseClickEventFbs(builder, 50.0f, 75.0f, 1);

  sf::Event out{sf::Event::MouseMoved{}};
  auto result = steamrot::data::configure::ToSFMLEvent(fbs, out);

  REQUIRE(result.has_value());
  REQUIRE(out.is<sf::Event::MouseButtonPressed>());
  const auto *pressed = out.getIf<sf::Event::MouseButtonPressed>();
  REQUIRE(pressed != nullptr);
  REQUIRE(pressed->button == sf::Mouse::Button::Right);
  REQUIRE(pressed->position.x == 50);
  REQUIRE(pressed->position.y == 75);
}

TEST_CASE("ToSFMLEvent MouseClick fails when mouse data is null",
          "[unit][configure_test_data]") {
  flatbuffers::FlatBufferBuilder builder;
  auto evt =
      steamrot::CreateInputEventFbs(builder, steamrot::InputTypeFbs_MouseClick,
                                    steamrot::InputEventDataFbs_NONE, 0);
  builder.Finish(evt);
  const auto *fbs =
      flatbuffers::GetRoot<steamrot::InputEventFbs>(builder.GetBufferPointer());

  sf::Event out{sf::Event::MouseMoved{}};
  auto result = steamrot::data::configure::ToSFMLEvent(fbs, out);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message.find("MouseClick") != std::string::npos);
}

TEST_CASE("ToSFMLEvent MouseRelease produces MouseButtonReleased event with "
          "correct button and position",
          "[unit][configure_test_data]") {
  flatbuffers::FlatBufferBuilder builder;
  const auto *fbs = BuildMouseReleaseEventFbs(builder, 30.0f, 40.0f, 0);

  sf::Event out{sf::Event::MouseMoved{}};
  auto result = steamrot::data::configure::ToSFMLEvent(fbs, out);

  REQUIRE(result.has_value());
  REQUIRE(out.is<sf::Event::MouseButtonReleased>());
  const auto *released = out.getIf<sf::Event::MouseButtonReleased>();
  REQUIRE(released != nullptr);
  REQUIRE(released->button == sf::Mouse::Button::Left);
  REQUIRE(released->position.x == 30);
  REQUIRE(released->position.y == 40);
}

TEST_CASE("ToSFMLEvent MouseRelease fails when mouse data is null",
          "[unit][configure_test_data]") {
  flatbuffers::FlatBufferBuilder builder;
  auto evt = steamrot::CreateInputEventFbs(builder,
                                           steamrot::InputTypeFbs_MouseRelease,
                                           steamrot::InputEventDataFbs_NONE, 0);
  builder.Finish(evt);
  const auto *fbs =
      flatbuffers::GetRoot<steamrot::InputEventFbs>(builder.GetBufferPointer());

  sf::Event out{sf::Event::MouseMoved{}};
  auto result = steamrot::data::configure::ToSFMLEvent(fbs, out);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message.find("MouseRelease") != std::string::npos);
}

TEST_CASE("ToSFMLEvent KeyPress produces KeyPressed event with correct "
          "key code and modifiers",
          "[unit][configure_test_data]") {
  flatbuffers::FlatBufferBuilder builder;
  const uint32_t key_code = static_cast<uint32_t>(sf::Keyboard::Key::A);
  const auto *fbs = BuildKeyPressEventFbs(builder, key_code, true, false, true);

  sf::Event out{sf::Event::MouseMoved{}};
  auto result = steamrot::data::configure::ToSFMLEvent(fbs, out);

  REQUIRE(result.has_value());
  REQUIRE(out.is<sf::Event::KeyPressed>());
  const auto *pressed = out.getIf<sf::Event::KeyPressed>();
  REQUIRE(pressed != nullptr);
  REQUIRE(pressed->code == sf::Keyboard::Key::A);
  REQUIRE(pressed->alt == true);
  REQUIRE(pressed->control == false);
  REQUIRE(pressed->shift == true);
}

TEST_CASE("ToSFMLEvent KeyPress fails when keyboard data is null",
          "[unit][configure_test_data]") {
  flatbuffers::FlatBufferBuilder builder;
  auto evt =
      steamrot::CreateInputEventFbs(builder, steamrot::InputTypeFbs_KeyPress,
                                    steamrot::InputEventDataFbs_NONE, 0);
  builder.Finish(evt);
  const auto *fbs =
      flatbuffers::GetRoot<steamrot::InputEventFbs>(builder.GetBufferPointer());

  sf::Event out{sf::Event::MouseMoved{}};
  auto result = steamrot::data::configure::ToSFMLEvent(fbs, out);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message.find("KeyPress") != std::string::npos);
}

TEST_CASE("ToSFMLEvent KeyRelease produces KeyReleased event with correct "
          "key code and modifiers",
          "[unit][configure_test_data]") {
  flatbuffers::FlatBufferBuilder builder;
  const uint32_t key_code = static_cast<uint32_t>(sf::Keyboard::Key::Space);
  const auto *fbs =
      BuildKeyReleaseEventFbs(builder, key_code, false, true, false);

  sf::Event out{sf::Event::MouseMoved{}};
  auto result = steamrot::data::configure::ToSFMLEvent(fbs, out);

  REQUIRE(result.has_value());
  REQUIRE(out.is<sf::Event::KeyReleased>());
  const auto *released = out.getIf<sf::Event::KeyReleased>();
  REQUIRE(released != nullptr);
  REQUIRE(released->code == sf::Keyboard::Key::Space);
  REQUIRE(released->alt == false);
  REQUIRE(released->control == true);
  REQUIRE(released->shift == false);
}

TEST_CASE("ToSFMLEvent KeyRelease fails when keyboard data is null",
          "[unit][configure_test_data]") {
  flatbuffers::FlatBufferBuilder builder;
  auto evt =
      steamrot::CreateInputEventFbs(builder, steamrot::InputTypeFbs_KeyRelease,
                                    steamrot::InputEventDataFbs_NONE, 0);
  builder.Finish(evt);
  const auto *fbs =
      flatbuffers::GetRoot<steamrot::InputEventFbs>(builder.GetBufferPointer());

  sf::Event out{sf::Event::MouseMoved{}};
  auto result = steamrot::data::configure::ToSFMLEvent(fbs, out);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message.find("KeyRelease") != std::string::npos);
}

/////////////////////////////////////////////////
// ConfigureInputEventsByTick tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureInputEventsByTick succeeds with null pair vector",
          "[unit][configure_test_data]") {
  std::unordered_map<size_t, std::vector<sf::Event>> result_map;

  auto result = steamrot::data::configure::ConfigureInputEventsByTick(
      result_map, nullptr);

  REQUIRE(result.has_value());
  REQUIRE(result_map.empty());
}

TEST_CASE("ConfigureInputEventsByTick populates single tick with a MouseMove "
          "event",
          "[unit][configure_test_data]") {
  std::unordered_map<size_t, std::vector<sf::Event>> result_map;

  flatbuffers::FlatBufferBuilder builder;

  auto pos = steamrot::CreateVector2fDataFbs(builder, 10.0f, 20.0f);
  auto mouse = steamrot::CreateMouseInputDataFbs(builder, pos, 0);
  auto evt_offset = steamrot::CreateInputEventFbs(
      builder, steamrot::InputTypeFbs_MouseMove,
      steamrot::InputEventDataFbs_MouseInputDataFbs, mouse.Union());

  std::vector<flatbuffers::Offset<steamrot::InputEventFbs>> events{evt_offset};
  auto inputs_vec = builder.CreateVector(events);
  auto pair_offset = steamrot::CreateTickInputsPairFbs(builder, 3, inputs_vec);

  std::vector<flatbuffers::Offset<steamrot::TickInputsPairFbs>> pairs{
      pair_offset};
  auto pairs_vec =
      builder.CreateVectorOfSortedTables<steamrot::TickInputsPairFbs>(&pairs);

  auto meta =
      steamrot::CreateTestMetadataFbs(builder, builder.CreateString("t"));
  auto td = steamrot::CreateTestDataFbs(builder, meta, 0, 1, 0, 0, pairs_vec,
                                        steamrot::SceneTypeFbs_UNKNOWN);
  builder.Finish(td);
  const auto *fbs = steamrot::GetTestDataFbs(builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureInputEventsByTick(
      result_map, fbs->input_events());

  REQUIRE(result.has_value());
  REQUIRE(result_map.size() == 1);
  REQUIRE(result_map.count(3) == 1);
  REQUIRE(result_map[3].size() == 1);
  REQUIRE(result_map[3][0].is<sf::Event::MouseMoved>());
  const auto *moved = result_map[3][0].getIf<sf::Event::MouseMoved>();
  REQUIRE(moved != nullptr);
  REQUIRE(moved->position.x == 10);
  REQUIRE(moved->position.y == 20);
}

TEST_CASE("ConfigureInputEventsByTick populates single tick with multiple "
          "events",
          "[unit][configure_test_data]") {
  std::unordered_map<size_t, std::vector<sf::Event>> result_map;

  flatbuffers::FlatBufferBuilder builder;

  // Event 1: MouseMove
  auto pos1 = steamrot::CreateVector2fDataFbs(builder, 5.0f, 6.0f);
  auto mouse1 = steamrot::CreateMouseInputDataFbs(builder, pos1, 0);
  auto evt1 = steamrot::CreateInputEventFbs(
      builder, steamrot::InputTypeFbs_MouseMove,
      steamrot::InputEventDataFbs_MouseInputDataFbs, mouse1.Union());

  // Event 2: KeyPress
  auto kbd = steamrot::CreateKeyboardInputDataFbs(
      builder, static_cast<uint32_t>(sf::Keyboard::Key::Enter), false, false,
      false);
  auto evt2 = steamrot::CreateInputEventFbs(
      builder, steamrot::InputTypeFbs_KeyPress,
      steamrot::InputEventDataFbs_KeyboardInputDataFbs, kbd.Union());

  std::vector<flatbuffers::Offset<steamrot::InputEventFbs>> events{evt1, evt2};
  auto inputs_vec = builder.CreateVector(events);
  auto pair = steamrot::CreateTickInputsPairFbs(builder, 0, inputs_vec);
  std::vector<flatbuffers::Offset<steamrot::TickInputsPairFbs>> pairs{pair};
  auto pairs_vec =
      builder.CreateVectorOfSortedTables<steamrot::TickInputsPairFbs>(&pairs);

  auto meta =
      steamrot::CreateTestMetadataFbs(builder, builder.CreateString("t"));
  auto td = steamrot::CreateTestDataFbs(builder, meta, 0, 1, 0, 0, pairs_vec,
                                        steamrot::SceneTypeFbs_UNKNOWN);
  builder.Finish(td);
  const auto *fbs = steamrot::GetTestDataFbs(builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureInputEventsByTick(
      result_map, fbs->input_events());

  REQUIRE(result.has_value());
  REQUIRE(result_map.count(0) == 1);
  REQUIRE(result_map[0].size() == 2);
  REQUIRE(result_map[0][0].is<sf::Event::MouseMoved>());
  REQUIRE(result_map[0][1].is<sf::Event::KeyPressed>());
}

TEST_CASE("ConfigureInputEventsByTick populates multiple ticks",
          "[unit][configure_test_data]") {
  std::unordered_map<size_t, std::vector<sf::Event>> result_map;

  flatbuffers::FlatBufferBuilder builder;

  // Tick 1: MouseMove
  auto pos1 = steamrot::CreateVector2fDataFbs(builder, 1.0f, 2.0f);
  auto m1 = steamrot::CreateMouseInputDataFbs(builder, pos1, 0);
  auto e1 = steamrot::CreateInputEventFbs(
      builder, steamrot::InputTypeFbs_MouseMove,
      steamrot::InputEventDataFbs_MouseInputDataFbs, m1.Union());
  auto ev1 = builder.CreateVector(
      std::vector<flatbuffers::Offset<steamrot::InputEventFbs>>{e1});
  auto pair1 = steamrot::CreateTickInputsPairFbs(builder, 1, ev1);

  // Tick 5: KeyRelease
  auto kbd = steamrot::CreateKeyboardInputDataFbs(
      builder, static_cast<uint32_t>(sf::Keyboard::Key::Escape), false, false,
      false);
  auto e2 = steamrot::CreateInputEventFbs(
      builder, steamrot::InputTypeFbs_KeyRelease,
      steamrot::InputEventDataFbs_KeyboardInputDataFbs, kbd.Union());
  auto ev2 = builder.CreateVector(
      std::vector<flatbuffers::Offset<steamrot::InputEventFbs>>{e2});
  auto pair2 = steamrot::CreateTickInputsPairFbs(builder, 5, ev2);

  std::vector<flatbuffers::Offset<steamrot::TickInputsPairFbs>> pairs{pair1,
                                                                      pair2};
  auto pairs_vec =
      builder.CreateVectorOfSortedTables<steamrot::TickInputsPairFbs>(&pairs);

  auto meta =
      steamrot::CreateTestMetadataFbs(builder, builder.CreateString("t"));
  auto td = steamrot::CreateTestDataFbs(builder, meta, 0, 1, 0, 0, pairs_vec,
                                        steamrot::SceneTypeFbs_UNKNOWN);
  builder.Finish(td);
  const auto *fbs = steamrot::GetTestDataFbs(builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureInputEventsByTick(
      result_map, fbs->input_events());

  REQUIRE(result.has_value());
  REQUIRE(result_map.size() == 2);
  REQUIRE(result_map.count(1) == 1);
  REQUIRE(result_map.count(5) == 1);
  REQUIRE(result_map[1][0].is<sf::Event::MouseMoved>());
  REQUIRE(result_map[5][0].is<sf::Event::KeyReleased>());
}

TEST_CASE("ConfigureInputEventsByTick handles empty inputs vector for a tick",
          "[unit][configure_test_data]") {
  std::unordered_map<size_t, std::vector<sf::Event>> result_map;

  flatbuffers::FlatBufferBuilder builder;

  // Pair with no inputs (empty vector)
  auto empty_vec = builder.CreateVector(
      std::vector<flatbuffers::Offset<steamrot::InputEventFbs>>{});
  auto pair = steamrot::CreateTickInputsPairFbs(builder, 7, empty_vec);
  std::vector<flatbuffers::Offset<steamrot::TickInputsPairFbs>> pairs{pair};
  auto pairs_vec =
      builder.CreateVectorOfSortedTables<steamrot::TickInputsPairFbs>(&pairs);

  auto meta =
      steamrot::CreateTestMetadataFbs(builder, builder.CreateString("t"));
  auto td = steamrot::CreateTestDataFbs(builder, meta, 0, 1, 0, 0, pairs_vec,
                                        steamrot::SceneTypeFbs_UNKNOWN);
  builder.Finish(td);
  const auto *fbs = steamrot::GetTestDataFbs(builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureInputEventsByTick(
      result_map, fbs->input_events());

  REQUIRE(result.has_value());
  REQUIRE(result_map.count(7) == 1);
  REQUIRE(result_map[7].empty());
}

TEST_CASE("ConfigureInputEventsByTick propagates ToSFMLEvent error",
          "[unit][configure_test_data]") {
  std::unordered_map<size_t, std::vector<sf::Event>> result_map;

  flatbuffers::FlatBufferBuilder builder;

  // Build a MouseMove event with no mouse data so ToSFMLEvent will fail
  auto bad_evt =
      steamrot::CreateInputEventFbs(builder, steamrot::InputTypeFbs_MouseMove,
                                    steamrot::InputEventDataFbs_NONE, 0);
  auto ev = builder.CreateVector(
      std::vector<flatbuffers::Offset<steamrot::InputEventFbs>>{bad_evt});
  auto pair = steamrot::CreateTickInputsPairFbs(builder, 0, ev);
  std::vector<flatbuffers::Offset<steamrot::TickInputsPairFbs>> pairs{pair};
  auto pairs_vec =
      builder.CreateVectorOfSortedTables<steamrot::TickInputsPairFbs>(&pairs);

  auto meta =
      steamrot::CreateTestMetadataFbs(builder, builder.CreateString("t"));
  auto td = steamrot::CreateTestDataFbs(builder, meta, 0, 1, 0, 0, pairs_vec,
                                        steamrot::SceneTypeFbs_UNKNOWN);
  builder.Finish(td);
  const auto *fbs = steamrot::GetTestDataFbs(builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureInputEventsByTick(
      result_map, fbs->input_events());

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

/////////////////////////////////////////////////
// ConfigureTestData + input_events integration test
/////////////////////////////////////////////////

TEST_CASE("ConfigureTestData populates input_events_by_tick correctly",
          "[unit][configure_test_data]") {
  steamrot::TestData test_data;
  steamrot::EventHandler event_handler;

  flatbuffers::FlatBufferBuilder builder;

  // Build a KeyPress event for tick 2
  auto kbd = steamrot::CreateKeyboardInputDataFbs(
      builder, static_cast<uint32_t>(sf::Keyboard::Key::W), false, false,
      false);
  auto evt = steamrot::CreateInputEventFbs(
      builder, steamrot::InputTypeFbs_KeyPress,
      steamrot::InputEventDataFbs_KeyboardInputDataFbs, kbd.Union());
  auto inputs_vec = builder.CreateVector(
      std::vector<flatbuffers::Offset<steamrot::InputEventFbs>>{evt});
  auto pair = steamrot::CreateTickInputsPairFbs(builder, 2, inputs_vec);
  std::vector<flatbuffers::Offset<steamrot::TickInputsPairFbs>> pairs{pair};
  auto pairs_vec =
      builder.CreateVectorOfSortedTables<steamrot::TickInputsPairFbs>(&pairs);

  auto meta = steamrot::CreateTestMetadataFbs(
      builder, builder.CreateString("InputTest"));
  auto td = steamrot::CreateTestDataFbs(builder, meta, 0, 4, 0, 0, pairs_vec,
                                        steamrot::SceneTypeFbs_UNKNOWN);
  builder.Finish(td);
  const auto *fbs = steamrot::GetTestDataFbs(builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureTestData(test_data, fbs,
                                                             event_handler);

  REQUIRE(result.has_value());
  REQUIRE(test_data.number_of_ticks == 4);
  REQUIRE(test_data.input_events_by_tick.size() == 1);
  REQUIRE(test_data.input_events_by_tick.count(2) == 1);
  REQUIRE(test_data.input_events_by_tick[2].size() == 1);
  REQUIRE(test_data.input_events_by_tick[2][0].is<sf::Event::KeyPressed>());
  const auto *kp =
      test_data.input_events_by_tick[2][0].getIf<sf::Event::KeyPressed>();
  REQUIRE(kp != nullptr);
  REQUIRE(kp->code == sf::Keyboard::Key::W);
}
