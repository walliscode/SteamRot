/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the MachinaArchetypeBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "MachinaArchetypeBuilder.h"
#include "AnalysisTraceBuilder.h"
#include "ChainDescriptorBuilder.h"
#include "DescriptorResult.h"
#include "PartGraphBuilder.h"
#include "TerminalDescriptorFormatter.h"
#include "TraceEqualsMatcher.h"
#include "descriptors_node_descriptors.h"
#include <catch2/catch_test_macros.hpp>
#include <vector>

using namespace steamrot::logic::descriptors;

// Helper that builds a named ChainDescriptor via ChainDescriptorBuilder
static ChainDescriptor make_test_chain(const std::string &name) {
  NodeDescriptor always_true = [](const steamrot::PartGraph & /*parts*/,
                                  uint32_t /*id*/) -> NodeDescriptorResult {
    return NodeDescriptorResult{true};
  };
  return ChainDescriptorBuilder{}.Then(always_true).Build(name);
}

TEST_CASE("MachinaArchetypeBuilder default construction",
          "[MachinaArchetypeBuilder]") {
  // arrange & act
  MachinaArchetypeBuilder<TestArchetypeResult> builder;
  // assert
  SUCCEED("Default construction should succeed without throwing");
}

TEST_CASE("MachinaArchetypeBuilder::Then tests", "[MachinaArchetypeBuilder]") {
  // arrange
  MachinaArchetypeBuilder<TestArchetypeResult> builder;
  const auto &steps = builder.GetSteps();
  REQUIRE(steps.size() == 0);

  SECTION("Then adds a step with Sequence kind") {
    // act
    builder.Then(make_test_chain("cd1"), &TestArchetypeResult::chain1);
    // assert
    REQUIRE(steps.size() == 1);
    REQUIRE(steps[0].kind == ArchetypeStepKind::Sequence);
  }

  SECTION("Then stores the correct descriptor name") {
    // act
    builder.Then(make_test_chain("my_chain"), &TestArchetypeResult::chain1);
    // assert
    REQUIRE(steps.size() == 1);
    REQUIRE(steps[0].descriptor.GetName() == "my_chain");
  }

  SECTION("Then stores the correct member pointer") {
    // act
    builder.Then(make_test_chain("cd1"), &TestArchetypeResult::chain1);
    builder.Then(make_test_chain("cd2"), &TestArchetypeResult::chain2);
    // assert
    REQUIRE(steps.size() == 2);
    REQUIRE(std::get<SubGraph TestArchetypeResult::*>(
                steps[0].result_storage) == &TestArchetypeResult::chain1);
    REQUIRE(std::get<SubGraph TestArchetypeResult::*>(
                steps[1].result_storage) == &TestArchetypeResult::chain2);
  }

  SECTION("Then supports method chaining") {
    // act
    builder.Then(make_test_chain("cd1"), &TestArchetypeResult::chain1)
        .Then(make_test_chain("cd2"), &TestArchetypeResult::chain2);
    // assert
    REQUIRE(steps.size() == 2);
    REQUIRE(steps[0].kind == ArchetypeStepKind::Sequence);
    REQUIRE(steps[1].kind == ArchetypeStepKind::Sequence);
    REQUIRE(steps[0].descriptor.GetName() == "cd1");
    REQUIRE(steps[1].descriptor.GetName() == "cd2");
  }
}
TEST_CASE("MachinaArchetypeBuilder::AtLeastNOf tests",
          "[MachinaArchetypeBuilder]") {
  // arrange
  MachinaArchetypeBuilder<TestArchetypeResult> builder;
  const auto &steps = builder.GetSteps();
  REQUIRE(steps.size() == 0);

  SECTION("AtLeastNOf adds a step with AtLeastNOf kind") {
    // act
    builder.AtLeastNOf(make_test_chain("cd1"), 1, &TestArchetypeResult::chains);
    // assert
    REQUIRE(steps.size() == 1);
    REQUIRE(steps[0].kind == ArchetypeStepKind::AtLeastNOf);
  }

  SECTION("AtLeastNOf stores the correct descriptor name") {
    // act
    builder.AtLeastNOf(make_test_chain("my_chain"), 1,
                       &TestArchetypeResult::chains);
    // assert
    REQUIRE(steps.size() == 1);
    REQUIRE(steps[0].descriptor.GetName() == "my_chain");
  }

  SECTION("AtLeastNOf stores the correct member pointer") {
    // act
    builder.AtLeastNOf(make_test_chain("cd1"), 1, &TestArchetypeResult::chains);
    // assert
    REQUIRE(steps.size() == 1);
    REQUIRE(std::get<std::vector<SubGraph> TestArchetypeResult::*>(
                steps[0].result_storage) == &TestArchetypeResult::chains);
  }

  SECTION("AtLeastNOf adds min repetitions correctly") {
    // act
    builder.AtLeastNOf(make_test_chain("cd1"), 2, &TestArchetypeResult::chains);
    // assert
    REQUIRE(steps.size() == 1);
    REQUIRE(steps[0].min_repetitions == 2);
  }
}

TEST_CASE("MachinaArchetypeBuilder passes depth through nested chain descriptors",
          "[MachinaArchetypeBuilder]") {
  const ChainDescriptor inner_chain =
      ChainDescriptorBuilder{}.Then(is_joint).Build("inner_chain");
  const MachinaArchetype archetype =
      MachinaArchetypeBuilder<TestArchetypeResult>{}
          .Then(inner_chain, &TestArchetypeResult::chain1)
          .Build("outer_archetype");

  const steamrot::tests::PartGraphPackage pkg =
      steamrot::tests::PartGraphBuilder{}
          .AddJoint(steamrot::tests::JointNames::TwoSockets, "j0")
          .Build();

  const AnalysisTrace expected_trace =
      steamrot::tests::AnalysisTraceBuilder{pkg.id_to_part_graph_id}
          .ScopeBegin("outer_archetype", ScopeKind::MachinaArchetype, 2, "j0")
          .ScopeBegin("inner_chain", ScopeKind::Chain, 3, "j0")
          .NodeEval("j0", is_joint.GetName(), 4)
          .NodeResult("j0", is_joint.GetName(), true,
                      "node holds JointInstance", 4)
          .ScopeEnd("inner_chain", ScopeKind::Chain, true, 3)
          .ScopeEnd("outer_archetype", ScopeKind::MachinaArchetype, true, 2)
          .Build();

  const MachinaArchetypeResult result =
      archetype(pkg.part_graph, pkg.id_to_part_graph_id.at("j0"), 2);

  REQUIRE(result);
  REQUIRE_THAT(result.m_trace,
               steamrot::tests::EqualsTrace(expected_trace,
                                            TerminalDescriptorFormatter{}));
}
