/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ChainDescriptorBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ChainDescriptorBuilder.h"
#include "DescriptorResult.h"
#include "descriptors_analysis_event_helpers.h"
#include <string>
#include <vector>

namespace steamrot::logic::descriptors {
// namespace

/////////////////////////////////////////////////
ChainDescriptorBuilder &ChainDescriptorBuilder::Then(NodeDescriptor nd) {
  m_steps.push_back({std::move(nd), ChainStepKind::Sequence});
  return *this;
}

/////////////////////////////////////////////////
ChainDescriptorBuilder &ChainDescriptorBuilder::WhileIsTrue(NodeDescriptor nd) {
  m_steps.push_back({std::move(nd), ChainStepKind::WhileIsTrue});
  return *this;
}

/////////////////////////////////////////////////
ChainDescriptor ChainDescriptorBuilder::Build(std::string name) {

  std::vector<ChainStep> steps = m_steps;

  return ChainDescriptor{
      name,
      [steps = std::move(steps), chain_name = std::move(name)](
          const PartGraph &parts, uint32_t start_id) -> ChainDescriptorResult {
        ChainDescriptorResult result{false};
        DFSContext context{steps};

        // Check for empty PartGraph
        if (parts.empty()) {

          // this is automatically a failed result
          result.m_result = false;

          // set up EmtpyGraph event in the trace
          add_empty_part_graph_event(context);
          result.m_trace = std::move(context.trace);

          // return early since there's no graph to traverse
          return result;
        }

        // Check for empty steps
        if (steps.empty()) {

          // this is automatically a failed result since a chain with no steps
          // can't be satisfied
          result.m_result = false;

          // set up EmptyChainSteps event in the trace to explain why the result
          // is false
          add_empty_chain_steps_event(context);
          result.m_trace = std::move(context.trace);

          // return early since there's no steps to evaluate
          return result;
        }

        add_scope_begin_event(context, chain_name, ScopeKind::Chain, parts, 0,
                              start_id);

        Cursor start_cursor{};
        start_cursor.current_id = start_id;
        start_cursor.steps_it = context.steps.cbegin();
        start_cursor.depth = 1;
        depth_first_search(start_cursor, context, parts, result);

        if (result.valid_subgraph.has_value()) {
          result.m_result = true;
        }

        // ScopeEnd
        add_scope_end_event(context, chain_name, ScopeKind::Chain,
                            result.m_result, 0);

        result.m_trace = std::move(context.trace);
        return result;
      }};
}
} // namespace steamrot::logic::descriptors
