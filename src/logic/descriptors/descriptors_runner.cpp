/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for running MachinaArchetype
/// evaluations on a PartGraph.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "descriptors_runner.h"
#include "descriptors_machina_archetypes.h"

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
void run_structural_analysis(PartGraph &graph,
                             StructuralAnalysisResults &results_storage) {
  // Clear previous results
  results_storage.clear();

  // Iterate over each part in the graph
  for (const auto &[id, part_variant] : graph) {

    // Initialize starting depth for analysis
    uint32_t starting_depth{0};

    // run grab analysis
    MachinaArchetypeResult grab_result = MA::Grab()(graph, id, starting_depth);
    if (grab_result) {
      results_storage["grab_results"].push_back(grab_result);
    }
  }
}
} // namespace steamrot::logic::descriptors
