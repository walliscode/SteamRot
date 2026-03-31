/////////////////////////////////////////////////
/// @file
/// @brief Declaration of TestData struct
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EngineSnapshot.h"
#include "EventPacket.h"
#include "SceneType.h"
#include "SimulationData.h"
#include "TestMetaData.h"
#include "input_data_generated.h"
#include <cstddef>
#include <cstdint>
#include <map>
#include <optional>
#include <unordered_map>

namespace steamrot {

/////////////////////////////////////////////////
/// @class TestData
/// @brief Struct containing all data needed for testing
///
/// A single TestData struct holds all necessary information to run the
/// TestEngine through a simulation cycle and then data to validate the results.
/////////////////////////////////////////////////
struct TestData {

  /////////////////////////////////////////////////
  /// @brief metadata about the test
  /////////////////////////////////////////////////
  TestMetaData meta_data{};

  /////////////////////////////////////////////////
  /// @brief Information required to run the simulation
  /////////////////////////////////////////////////
  SimulationData simulation_data{};

  /////////////////////////////////////////////////
  /// @brief Number of ticks to run the simulation for
  /////////////////////////////////////////////////
  uint32_t number_of_ticks{1};

  /////////////////////////////////////////////////
  /// @brief All events to be injected into the simulation, organized by tick
  /////////////////////////////////////////////////
  std::unordered_map<size_t, std::vector<EventPacket>> events_by_tick{};

  /////////////////////////////////////////////////
  /// @brief All simulated input events to be injected into the simulation,
  /// organized by tick
  /////////////////////////////////////////////////
  std::unordered_map<size_t, std::vector<InputEvent>> input_events_by_tick{};

  /////////////////////////////////////////////////
  /// @brief starting engine snapshot before simulation begins
  /////////////////////////////////////////////////
  EngineSnapshot starting_engine_snapshot{};

  /////////////////////////////////////////////////
  /// @brief If set, the TestEngine loads this scene from default data instead
  /// of using starting_engine_snapshot.
  ///
  /// When present (and not UNKNOWN), TestEngine::StartUp() calls
  /// SceneManager::AddSceneFromDefault() with this scene type, mirroring the
  /// behaviour of GameEngine. The starting_engine_snapshot is ignored.
  /////////////////////////////////////////////////
  std::optional<SceneType> initial_scene_type{std::nullopt};

  /////////////////////////////////////////////////
  /// @brief Expected engine snapshots at a given tick
  /////////////////////////////////////////////////
  std::map<size_t, EngineSnapshot> expected_engine_snapshots{};
};
} // namespace steamrot
//
