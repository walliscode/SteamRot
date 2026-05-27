/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the MachinaArchetypeBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "MachinaArchetypeBuilder.h"
#include "ChainDescriptorBuilder.h"
#include "DescriptorResult.h"
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
