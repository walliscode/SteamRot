/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions to configure test data
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EngineSnapshot.h"
#include "EventHandler.h"
#include "FailInfo.h"
#include "SimulationData.h"
#include "TestData.h"
#include "TestMetaData.h"
#include "simulation_data_generated.h"
#include "test_data_generated.h"
#include <expected>
#include <map>

namespace steamrot::data::configure {

/////////////////////////////////////////////////
/// @brief Configures a TestMetaData instance from FlatBuffers data.
///
/// @param test_meta_data TestMetaData instance to configure.
/// @param fbs_test_meta_data The FlatBuffers TestMetadataFbs instance.
/// @return std::monostate on success, FailInfo on error.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureTestMetaData(TestMetaData &test_meta_data,
                      const TestMetadataFbs *fbs_test_meta_data);

/////////////////////////////////////////////////
/// @brief Configures events by tick from FlatBuffers data.
///
/// @param events_by_tick Map to populate with tick->events pairs.
/// @param fbs_tick_events_pairs FlatBuffers vector of tick-events pairs.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConfigureEventsByTick(
    std::unordered_map<size_t, std::vector<EventPacket>> &events_by_tick,
    const flatbuffers::Vector<flatbuffers::Offset<TickEventsPairFbs>>
        *fbs_tick_events_pairs);

/////////////////////////////////////////////////
/// @brief Configures a SimulationData instance from FlatBuffers data.
///
/// @param simulation_data SimulationData instance to configure.
/// @param fbs_simulation_data The FlatBuffers SimulationDataFbs instance.
/// @return std::monostate on success, FailInfo on error.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureSimulationData(SimulationData &simulation_data,
                        const SimulationDataFbs *fbs_simulation_data);

/////////////////////////////////////////////////
/// @brief Configures expected engine snapshots map from FlatBuffers data.
///
/// @param expected_snapshots Map to populate with tick->snapshot pairs.
/// @param fbs_tick_snapshot_pairs Vector of FlatBuffers tick-snapshot pairs.
/// @param event_handler Reference to EventHandler for entity importers.
/// @return std::monostate on success, FailInfo on error.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConfigureExpectedEngineSnapshots(
    std::map<size_t, EngineSnapshot> &expected_snapshots,
    const flatbuffers::Vector<flatbuffers::Offset<TickSnapshotPairFbs>>
        *fbs_tick_snapshot_pairs,
    EventHandler &event_handler);

/////////////////////////////////////////////////
/// @brief Configures a TestData instance from FlatBuffers data.
///
/// @param test_data TestData instance to configure.
/// @param fbs_test_data The FlatBuffers TestDataFbs instance.
/// @param event_handler Reference to EventHandler for entity importers.
/// @return std::monostate on success, FailInfo on error.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureTestData(TestData &test_data, const TestDataFbs *fbs_test_data,
                  EventHandler &event_handler);

} // namespace steamrot::data::configure
