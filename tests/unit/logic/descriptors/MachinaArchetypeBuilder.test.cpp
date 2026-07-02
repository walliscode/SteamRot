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
#include "NodeDescriptor.h"
#include "PartGraphBuilder.h"
#include "TerminalDescriptorFormatter.h"
#include "TraceEqualsMatcher.h"
#include "descriptors_node_descriptors.h"
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <vector>

namespace steamrot::tests {
using namespace steamrot::logic::descriptors;

const NodeDescriptor &always_true() {
  static const NodeDescriptor instance =
      NodeDescriptor{"always_true",
                     [](const steamrot::PartGraph & /*parts*/,
                        uint32_t /*id*/) -> NodeDescriptorResult {
                       return NodeDescriptorResult{true};
                     }};
  return instance;
};

const ChainDescriptor make_test_chain_descriptor(const std::string &name) {
  return ChainDescriptorBuilder{}.Then(always_true()).Build(name);
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
    builder.Then(make_test_chain_descriptor("cd1"),
                 &TestArchetypeResult::test_node);
    // assert
    REQUIRE(steps.size() == 1);
    REQUIRE(steps[0].kind == ArchetypeStepKind::Sequence);
  }

  SECTION("Then stores the correct descriptor name") {
    // act
    builder.Then(make_test_chain_descriptor("my_chain"),
                 &TestArchetypeResult::test_node);
    // assert
    REQUIRE(steps.size() == 1);
    REQUIRE(steps[0].descriptor.GetName() == "my_chain");
  }

  SECTION("Then stores the correct member pointer") {
    // act
    builder.Then(make_test_chain_descriptor("cd1"),
                 &TestArchetypeResult::test_node);
    builder.Then(make_test_chain_descriptor("cd2"),
                 &TestArchetypeResult::test_node);
    // assert
    REQUIRE(steps.size() == 2);
    REQUIRE(std::get<uint32_t TestArchetypeResult::*>(
                steps[0].result_storage) == &TestArchetypeResult::test_node);
    REQUIRE(std::get<uint32_t TestArchetypeResult::*>(
                steps[1].result_storage) == &TestArchetypeResult::test_node);
  }

  SECTION("Then supports method chaining") {
    // act
    builder
        .Then(make_test_chain_descriptor("cd1"),
              &TestArchetypeResult::test_node)
        .Then(make_test_chain_descriptor("cd2"),
              &TestArchetypeResult::test_node);
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
    builder.AtLeastNOf(make_test_chain_descriptor("cd1"), 1,
                       &TestArchetypeResult::chains);
    // assert
    REQUIRE(steps.size() == 1);
    REQUIRE(steps[0].kind == ArchetypeStepKind::AtLeastNOf);
  }

  SECTION("AtLeastNOf stores the correct descriptor name") {
    // act
    builder.AtLeastNOf(make_test_chain_descriptor("my_chain"), 1,
                       &TestArchetypeResult::chains);
    // assert
    REQUIRE(steps.size() == 1);
    REQUIRE(steps[0].descriptor.GetName() == "my_chain");
  }

  SECTION("AtLeastNOf stores the correct member pointer") {
    // act
    builder.AtLeastNOf(make_test_chain_descriptor("cd1"), 1,
                       &TestArchetypeResult::chains);
    // assert
    REQUIRE(steps.size() == 1);
    REQUIRE(std::get<std::vector<SubGraph> TestArchetypeResult::*>(
                steps[0].result_storage) == &TestArchetypeResult::chains);
  }

  SECTION("AtLeastNOf adds min repetitions correctly") {
    // act
    builder.AtLeastNOf(make_test_chain_descriptor("cd1"), 2,
                       &TestArchetypeResult::chains);
    // assert
    REQUIRE(steps.size() == 1);
    REQUIRE(steps[0].min_repetitions == 2);
  }

  SECTION("AtLeastNOf supports method chaining") {
    // act
    builder
        .AtLeastNOf(make_test_chain_descriptor("cd1"), 1,
                    &TestArchetypeResult::chains)
        .AtLeastNOf(make_test_chain_descriptor("cd2"), 2,
                    &TestArchetypeResult::chains);
    // assert
    REQUIRE(steps.size() == 2);
    REQUIRE(steps[0].kind == ArchetypeStepKind::AtLeastNOf);
    REQUIRE(steps[1].kind == ArchetypeStepKind::AtLeastNOf);
    REQUIRE(steps[0].descriptor.GetName() == "cd1");
    REQUIRE(steps[1].descriptor.GetName() == "cd2");
    REQUIRE(steps[0].min_repetitions == 1);
    REQUIRE(steps[1].min_repetitions == 2);
  }
}

TEST_CASE(
    "MachinaArchetypeBuilder passes depth through nested chain descriptors",
    "[MachinaArchetypeBuilder]") {
  const ChainDescriptor inner_chain =
      ChainDescriptorBuilder{}.Then(is_joint()).Build("inner_chain");
  const MachinaArchetype archetype =
      MachinaArchetypeBuilder<TestArchetypeResult>{}
          .Then(inner_chain, &TestArchetypeResult::test_node)
          .Build("outer_archetype");

  const steamrot::tests::PartGraphPackage pkg =
      steamrot::tests::PartGraphBuilder{}
          .AddJointInstance(steamrot::tests::JointNames::TwoSockets, "j0")
          .Build();

  const AnalysisTrace expected_trace =
      steamrot::tests::AnalysisTraceBuilder{pkg.id_to_part_graph_id}
          .ScopeBegin("outer_archetype", ScopeKind::MachinaArchetype, 2, "j0")
          .ScopeBegin("inner_chain", ScopeKind::Chain, 3, "j0")
          .NodeEval("j0", is_joint().GetName(), true, 4,
                    "node holds JointInstance")
          .ScopeEnd("inner_chain", ScopeKind::Chain, true, 3)
          .MachinaPartResult("inner_chain", true, 2)
          .ScopeEnd("outer_archetype", ScopeKind::MachinaArchetype, true, 2)
          .Build();

  const MachinaArchetypeResult result =
      archetype(pkg.part_graph, pkg.id_to_part_graph_id.at("j0"), 2);

  REQUIRE(result);
  REQUIRE_THAT(result.m_trace,
               steamrot::tests::EqualsTrace(expected_trace,
                                            TerminalDescriptorFormatter{}));
}

TEST_CASE("MachinaArchetypeBuilder records AtLeastNOf step result events",
          "[MachinaArchetypeBuilder]") {
  const ChainDescriptor inner_chain =
      ChainDescriptorBuilder{}.Then(is_joint()).Build("inner_chain");
  const MachinaArchetype archetype =
      MachinaArchetypeBuilder<TestArchetypeResult>{}
          .AtLeastNOf(inner_chain, 1, &TestArchetypeResult::chains)
          .Build("outer_archetype");

  const steamrot::tests::PartGraphPackage pkg =
      steamrot::tests::PartGraphBuilder{}
          .AddJointInstance(steamrot::tests::JointNames::TwoSockets, "j0")
          .Build();

  const AnalysisTrace expected_trace =
      steamrot::tests::AnalysisTraceBuilder{pkg.id_to_part_graph_id}
          .ScopeBegin("outer_archetype", ScopeKind::MachinaArchetype, 0, "j0")
          .MachinaPartResult("inner_chain", false, 0)
          .ScopeEnd("outer_archetype", ScopeKind::MachinaArchetype, false, 0)
          .Build();

  const MachinaArchetypeResult result =
      archetype(pkg.part_graph, pkg.id_to_part_graph_id.at("j0"), 0);

  REQUIRE_FALSE(result);
  REQUIRE_THAT(result.m_trace,
               steamrot::tests::EqualsTrace(expected_trace,
                                            TerminalDescriptorFormatter{}));
}
} // namespace steamrot::tests
