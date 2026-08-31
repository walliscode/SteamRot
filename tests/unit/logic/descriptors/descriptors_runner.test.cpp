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
    results_storage.successful_results["dummy_fragment"].push_back(
        MachinaArchetypeResult{});
    results_storage.failed_results["dummy_fragment"].push_back(
        MachinaArchetypeResult{});

    REQUIRE(!results_storage.successful_results.empty());
    REQUIRE(!results_storage.failed_results.empty());

    // Act
    run_structural_analysis(graph, results_storage);
    // Assert
    REQUIRE(results_storage.successful_results.empty());
    REQUIRE(results_storage.failed_results.empty());
  }

  SECTION("results storage is empty when graph is empty") {
    // Act
    run_structural_analysis(graph, results_storage);
    // Assert
    REQUIRE(results_storage.successful_results.empty());
    REQUIRE(results_storage.failed_results.empty());
  }

  SECTION("results storage is populated when given a valid grab graph") {
    // Arrange
    PartGraphPackage grab_pkg = create_valid_grab_pkg();

    // Act
    run_structural_analysis(grab_pkg.part_graph, results_storage);

    // Assert
    REQUIRE(results_storage.successful_results.find("grab_results") !=
            results_storage.successful_results.end());
    REQUIRE(results_storage.successful_results["grab_results"].size() == 1);

    REQUIRE(results_storage.failed_results.empty());
  }
}
} // namespace steamrot::tests
