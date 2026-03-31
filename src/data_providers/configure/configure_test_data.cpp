/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions to configure test data
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_test_data.h"
#include "configure_engine_snapshot.h"
#include "scene_type_conversion.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <format>

namespace steamrot::data::configure {

namespace {

/////////////////////////////////////////////////
/// @brief Converts a FlatBuffers LogicClassEnumFbs to a LogicClassEnum.
///
/// @param fbs_logic_class_enum The FlatBuffers enum value to convert.
/// @return The corresponding LogicClassEnum value.
/////////////////////////////////////////////////
LogicClassEnum
ConvertFbsToLogicClassEnum(LogicClassEnumFbs fbs_logic_class_enum) {
  switch (fbs_logic_class_enum) {
  case LogicClassEnumFbs::LogicClassEnumFbs_None:
    return LogicClassEnum::None;
  case LogicClassEnumFbs::LogicClassEnumFbs_UIActionLogic:
    return LogicClassEnum::UIActionLogic;
  case LogicClassEnumFbs::LogicClassEnumFbs_UICollisionLogic:
    return LogicClassEnum::UICollisionLogic;
  case LogicClassEnumFbs::LogicClassEnumFbs_UIRenderLogic:
    return LogicClassEnum::UIRenderLogic;
  case LogicClassEnumFbs::LogicClassEnumFbs_UIStateLogic:
    return LogicClassEnum::UIStateLogic;
  default:
    return LogicClassEnum::None;
  }
}

} // namespace

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureTestMetaData(TestMetaData &test_meta_data,
                      const TestMetadataFbs *fbs_test_meta_data) {

  if (fbs_test_meta_data == nullptr) {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "Input Flatbuffers TestMetaData is null."});
  }

  // add required field: test_name
  if (!fbs_test_meta_data->test_name()) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "TestMetaDataFbs is missing required field: test_name."});
  } else {
    test_meta_data.test_name = fbs_test_meta_data->test_name()->str();
  }

  // add optional field: description
  if (fbs_test_meta_data->test_description()) {
    test_meta_data.test_description =
        fbs_test_meta_data->test_description()->str();
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureSimulationData(SimulationData &simulation_data,
                        const SimulationDataFbs *fbs_simulation_data) {

  if (!fbs_simulation_data)
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "Input Flatbuffers SimulationData is null."});

  // Add the description if it exists
  if (fbs_simulation_data->description()) {
    simulation_data.description = fbs_simulation_data->description()->str();
  }

  // For each SimulationStepFbs, create a SimulationStep and add to
  // simulation_data
  for (const auto *fbs_step : *fbs_simulation_data->steps()) {

    SimulationElement element;

    // Check that logic_class_type is set
    if (fbs_step->logic_class_type() == LogicClassEnumFbs_None) {
      return std::unexpected(
          FailInfo{FailMode::FlatbuffersDataNotFound,
                   "SimulationStepFbs has logic_class_type set to None. "
                   "A valid logic class must be specified."});
    }

    // Assign the converted LogicClassEnum to the element
    element = ConvertFbsToLogicClassEnum(fbs_step->logic_class_type());

    // Create SimulationStep and add to simulation_data
    SimulationStep step(element);
    simulation_data.steps.emplace_back(step);
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConfigureExpectedEngineSnapshots(
    std::map<size_t, EngineSnapshot> &expected_snapshots,
    const flatbuffers::Vector<flatbuffers::Offset<TickSnapshotPairFbs>>
        *fbs_tick_snapshot_pairs,
    EventHandler &event_handler) {

  if (!fbs_tick_snapshot_pairs) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "Input Flatbuffers tick-snapshot pairs vector is null."});
  }

  // Clear any existing snapshots
  expected_snapshots.clear();

  // Iterate through each tick-snapshot pair
  for (const auto *fbs_pair : *fbs_tick_snapshot_pairs) {
    if (!fbs_pair) {
      continue; // Skip null entries
    }

    // Get the tick number
    size_t tick = fbs_pair->tick();

    // Create and configure the EngineSnapshot
    EngineSnapshot snapshot;
    if (fbs_pair->snapshot()) {
      auto snapshot_result = ConfigureEngineSnapshot(
          snapshot, fbs_pair->snapshot(), event_handler);
      if (!snapshot_result) {
        return std::unexpected(snapshot_result.error());
      }
    }

    // Add to the map
    expected_snapshots[tick] = std::move(snapshot);
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ToSFMLEvent(const InputEventFbs *fbs_input_event, sf::Event &out_event) {
  if (!fbs_input_event) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "InputEventFbs pointer is null."});
  }

  switch (fbs_input_event->input_type()) {

  case InputTypeFbs_MouseMove: {
    const auto *mouse_data =
        fbs_input_event->input_data_as_MouseInputDataFbs();
    if (!mouse_data || !mouse_data->position()) {
      return std::unexpected(
          FailInfo{FailMode::FlatbuffersDataNotFound,
                   "MouseInputDataFbs or position is null for MouseMove."});
    }
    sf::Event::MouseMoved moved;
    moved.position = {static_cast<int>(mouse_data->position()->x()),
                      static_cast<int>(mouse_data->position()->y())};
    out_event = sf::Event{moved};
    break;
  }

  case InputTypeFbs_MouseClick: {
    const auto *mouse_data =
        fbs_input_event->input_data_as_MouseInputDataFbs();
    if (!mouse_data || !mouse_data->position()) {
      return std::unexpected(
          FailInfo{FailMode::FlatbuffersDataNotFound,
                   "MouseInputDataFbs or position is null for MouseClick."});
    }
    sf::Event::MouseButtonPressed pressed;
    pressed.button = static_cast<sf::Mouse::Button>(mouse_data->button());
    pressed.position = {static_cast<int>(mouse_data->position()->x()),
                        static_cast<int>(mouse_data->position()->y())};
    out_event = sf::Event{pressed};
    break;
  }

  case InputTypeFbs_MouseRelease: {
    const auto *mouse_data =
        fbs_input_event->input_data_as_MouseInputDataFbs();
    if (!mouse_data || !mouse_data->position()) {
      return std::unexpected(
          FailInfo{FailMode::FlatbuffersDataNotFound,
                   "MouseInputDataFbs or position is null for MouseRelease."});
    }
    sf::Event::MouseButtonReleased released;
    released.button = static_cast<sf::Mouse::Button>(mouse_data->button());
    released.position = {static_cast<int>(mouse_data->position()->x()),
                         static_cast<int>(mouse_data->position()->y())};
    out_event = sf::Event{released};
    break;
  }

  case InputTypeFbs_KeyPress: {
    const auto *key_data =
        fbs_input_event->input_data_as_KeyboardInputDataFbs();
    if (!key_data) {
      return std::unexpected(
          FailInfo{FailMode::FlatbuffersDataNotFound,
                   "KeyboardInputDataFbs is null for KeyPress."});
    }
    sf::Event::KeyPressed pressed;
    pressed.code = static_cast<sf::Keyboard::Key>(key_data->key_code());
    pressed.alt = key_data->alt();
    pressed.control = key_data->control();
    pressed.shift = key_data->shift();
    out_event = sf::Event{pressed};
    break;
  }

  case InputTypeFbs_KeyRelease: {
    const auto *key_data =
        fbs_input_event->input_data_as_KeyboardInputDataFbs();
    if (!key_data) {
      return std::unexpected(
          FailInfo{FailMode::FlatbuffersDataNotFound,
                   "KeyboardInputDataFbs is null for KeyRelease."});
    }
    sf::Event::KeyReleased released;
    released.code = static_cast<sf::Keyboard::Key>(key_data->key_code());
    released.alt = key_data->alt();
    released.control = key_data->control();
    released.shift = key_data->shift();
    out_event = sf::Event{released};
    break;
  }

  default:
    return std::unexpected(
        FailInfo{FailMode::NonExistentEnumValue,
                 std::format("Unhandled InputTypeFbs value: {}",
                             static_cast<int>(fbs_input_event->input_type()))});
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConfigureInputEventsByTick(
    std::unordered_map<size_t, std::vector<sf::Event>> &input_events_by_tick,
    const flatbuffers::Vector<flatbuffers::Offset<TickInputsPairFbs>>
        *fbs_tick_inputs_pairs) {

  if (!fbs_tick_inputs_pairs)
    return std::monostate{};

  for (const auto *fbs_pair : *fbs_tick_inputs_pairs) {
    if (!fbs_pair)
      continue;

    const size_t tick = static_cast<size_t>(fbs_pair->tick());
    std::vector<sf::Event> events;

    if (fbs_pair->inputs()) {
      for (const auto *fbs_event : *fbs_pair->inputs()) {
        sf::Event sfml_event{sf::Event::MouseMoved{}};
        auto result = ToSFMLEvent(fbs_event, sfml_event);
        if (!result)
          return std::unexpected(result.error());
        events.push_back(sfml_event);
      }
    }

    input_events_by_tick[tick] = std::move(events);
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureTestData(TestData &test_data, const TestDataFbs *fbs_test_data,
                  EventHandler &event_handler) {

  if (!fbs_test_data) {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "Input Flatbuffers TestData is null."});
  }

  // Configure TestMetaData
  auto meta_data_result =
      ConfigureTestMetaData(test_data.meta_data, fbs_test_data->meta_data());
  if (!meta_data_result)
    return std::unexpected(meta_data_result.error());

  // Configure SimulationData (optional).
  // When absent, test_data.simulation_data remains default-constructed
  // (empty steps vector), which means no simulation logic will be executed.
  if (fbs_test_data->simulation_data()) {
    auto simulation_data_result = ConfigureSimulationData(
        test_data.simulation_data, fbs_test_data->simulation_data());
    if (!simulation_data_result)
      return std::unexpected(simulation_data_result.error());
  }

  // Configure number_of_ticks, this must be present
  if (!fbs_test_data->num_ticks()) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "TestDataFbs is missing required field: num_ticks."});
  } else {
    test_data.number_of_ticks = fbs_test_data->num_ticks();
  }

  // Configure starting_engine_snapshot (optional)
  if (fbs_test_data->starting_engine_snapshot()) {
    auto snapshot_result = ConfigureEngineSnapshot(
        test_data.starting_engine_snapshot,
        fbs_test_data->starting_engine_snapshot(), event_handler);
    if (!snapshot_result)
      return std::unexpected(snapshot_result.error());
  }

  // Configure expected_engine_snapshots (optional)
  if (fbs_test_data->expected_engine_snapshots()) {
    auto expected_snapshots_result = ConfigureExpectedEngineSnapshots(
        test_data.expected_engine_snapshots,
        fbs_test_data->expected_engine_snapshots(), event_handler);
    if (!expected_snapshots_result)
      return std::unexpected(expected_snapshots_result.error());
  }

  // Configure initial_scene_type (optional).
  // UNKNOWN is the FlatBuffers default and means "use
  // starting_engine_snapshot".
  if (fbs_test_data->initial_scene_type() != SceneTypeFbs_UNKNOWN) {
    auto conversion_result =
        ConvertSceneTypeFbsToSceneType(fbs_test_data->initial_scene_type());
    if (!conversion_result.has_value())
      return std::unexpected(conversion_result.error());
    test_data.initial_scene_type = conversion_result.value();
  }

  // Configure input_events_by_tick (optional)
  if (fbs_test_data->input_events()) {
    auto input_events_result = ConfigureInputEventsByTick(
        test_data.input_events_by_tick, fbs_test_data->input_events());
    if (!input_events_result)
      return std::unexpected(input_events_result.error());
  }

  return std::monostate{};
}

} // namespace steamrot::data::configure
