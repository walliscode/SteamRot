/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ChainDescriptorBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ChainDescriptorBuilder.h"
#include "DescriptorResult.h"
#include <string>
#include <utility>
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
ChainDescriptorBuilder &
ChainDescriptorBuilder::WhileIsTrueForN(NodeDescriptor nd,
                                        size_t min_repetitions) {
  if (min_repetitions == 0)
    min_repetitions = 1;
  m_steps.push_back(
      {std::move(nd), ChainStepKind::WhileIsTrueForN, min_repetitions});
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
          AnalysisEvent empty_graph_event{};
          empty_graph_event.kind = TraceEventKind::EmtpyPartGraph;
          context.trace.push_back(std::move(empty_graph_event));
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
          AnalysisEvent empty_steps_event{};
          empty_steps_event.kind = TraceEventKind::EmtpyChainSteps;
          context.trace.push_back(std::move(empty_steps_event));
          result.m_trace = std::move(context.trace);

          // return early since there's no steps to evaluate
          return result;
        }

        AnalysisEvent scope_begin{};
        scope_begin.kind = TraceEventKind::ScopeBegin;
        scope_begin.depth = 0;
        scope_begin.scope_name = chain_name;
        scope_begin.scope_kind = ScopeKind::Chain;
        scope_begin.anchor_id = start_id;
        if (const auto anchor_it = parts.find(start_id);
            anchor_it != parts.end()) {
          scope_begin.part_id_alias = std::visit(
              [](const auto &inst) -> const std::string & {
                return inst.alias;
              },
              anchor_it->second);
        }
        context.trace.push_back(std::move(scope_begin));

        Cursor start_cursor{};
        start_cursor.current_id = start_id;
        start_cursor.steps_it = context.steps.cbegin();
        start_cursor.depth = 1;
        depth_first_search(start_cursor, context, parts, result);

        if (!result.valid_subgraphs.empty()) {
          result.m_result = true;
        }

        // ScopeEnd
        AnalysisEvent scope_end{};
        scope_end.kind = TraceEventKind::ScopeEnd;
        scope_end.depth = 0;
        scope_end.scope_name = chain_name;
        scope_end.scope_kind = ScopeKind::Chain;
        scope_end.result = result.m_result;
        context.trace.push_back(std::move(scope_end));

        result.m_trace = std::move(context.trace);
        return result;
      }};
}
} // namespace steamrot::logic::descriptors
