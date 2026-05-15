/////////////////////////////////////////////////
/// @file
/// @brief Units tests for the ChainDescriptor class and related free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "descriptors_chain_descriptors.h"
#include "AnalysisEvent.h"
#include "AnalysisTraceBuilder.h"
#include "MachinaFormScaffold.h"
#include "TerminalDescriptorFormatter.h"
#include "TraceEqualsMatcher.h"
#include "part_library.h"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

namespace {
constexpr uint32_t kMissingPartId{9999};
}

TEST_CASE("ChainDescriptor is_serial_chain") {

  using namespace steamrot::logic::descriptors;

  // arrange test library
  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder builder{lib};

  // some general assertions about the descriptor instance
  REQUIRE(is_serial_chain.GetName() == "is_serial_chain");

  SECTION("is_serial_chain returns result and correct trace with empty part "
          "graph") {
    // test predicate
    steamrot::PartGraph empty_graph;
    ChainDescriptorResult result = is_serial_chain(empty_graph, 0);

    // build expected trace
    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{}.EmptyPartGraph().Build();

    // assert result and trace
    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
  }

  SECTION("is_serial_chain evaluates IsolatedPair") {
    // test predicate
    steamrot::MachinaFormScaffold scaffold = builder.GetScenarioForAnalysis(
        steamrot::tests::ScaffoldScenario::IsolatedPair);
    steamrot::PartGraph &parts = scaffold.parts;

    // print out keys in map
    std::cout << "Part IDs in IsolatedPair scaffold:\n";
    for (const auto &[id, variant] : parts) {
      std::cout << "  part ID: " << id << "\n";
    }
    // check part 0 exists
    auto it = parts.find(0);
    REQUIRE(it != parts.end());
    INFO("attempting to evaluate is_serial_chain on IsolatedPair scaffold");
    ChainDescriptorResult result = is_serial_chain(parts, 0);
    INFO("result obtained");
    // build expected trace
    AnalysisTrace expected_trace =
        steamrot::tests::AnalysisTraceBuilder{}
            .ScopeBegin("is_serial_chain", ScopeKind::Chain, 0)
            .NodeEval(0, "is_serial")
            .NodeResult(0, "is_serial", true)
            .Build();

    // assert result and trace
    REQUIRE_FALSE(result);
    REQUIRE_THAT(result.m_trace,
                 steamrot::tests::EqualsTrace(expected_trace,
                                              TerminalDescriptorFormatter{}));
  }
}
