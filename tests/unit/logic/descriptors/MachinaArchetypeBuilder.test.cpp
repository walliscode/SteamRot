/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the MachinaArchetypeBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "MachinaArchetypeBuilder.h"
#include <catch2/catch_test_macros.hpp>
#include <vector>

struct TestArchetype {
  steamrot::logic::descriptors::SubGraph chain1;
  steamrot::logic::descriptors::SubGraph chain2;
  std::vector<steamrot::logic::descriptors::SubGraph> chains;
};

TEST_CASE("MachinaArchetypeBuilder default construction",
          "[MachinaArchetypeBuilder]") {
  // arrange & act
  steamrot::logic::descriptors::MachinaArchetypeBuilder<TestArchetype> builder;
  // assert
  SUCCEED("Default construction should succeed without throwing");
}
