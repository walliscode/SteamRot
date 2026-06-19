/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for free functions that run MachinaArchetype evaluations
/// on a PartGraph.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "descriptors_runner.h"
#include "PartGraphBuilder.h"
#include "machina_archetype_packages.h"
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

using namespace steamrot::logic::descriptors;

TEST_CASE("run_structural_analysis tests", "[descriptors_runner]") {

  // Arrange
  PartGraph graph;
  StructuralAnalysisResults results_storage;

  SECTION("results_storage is cleared before analysis") {
    // Pre-fill results_storage with dummy data
    results_storage["dummy_fragment"].push_back(MachinaArchetypeResult{});

    // Act
    run_structural_analysis(graph, results_storage);
    // Assert
    REQUIRE(results_storage.empty());
  }

  SECTION("results storage is empty when graph is empty") {
    // Act
    run_structural_analysis(graph, results_storage);
    // Assert
    REQUIRE(results_storage.empty());
  }

  SECTION("results storage is populated when given a valid grab graph") {
    // Arrange
    PartGraphPackage grab_pkg = create_valid_grab_pkg();

    // Act
    run_structural_analysis(grab_pkg.part_graph, results_storage);

    // Assert
    REQUIRE(results_storage.find("grab_results") != results_storage.end());
    REQUIRE(results_storage["grab_results"].size() == 1);
  }
}
} // namespace steamrot::tests
