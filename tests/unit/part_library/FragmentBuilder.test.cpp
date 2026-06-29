/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the FragmentBuilder class, verifying its ability to
/// construct Fragment objects with specific configurations and properties.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FragmentBuilder.h"
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

using namespace steamrot::parts;

TEST_CASE("FragmentBuilder tests", "[unit][part_library][FragmentBuilder]") {

  // set up a FragmentBuilder
  FragmentBuilder builder;

  SECTION("build() returns a Fragment with the specified name") {
    const std::string fragment_name = "TestFragment";
    Fragment fragment = builder.build(fragment_name);
    REQUIRE(fragment.name == fragment_name);
  }

  SECTION("AddViews() sets the positioning_views of the Fragment") {
    const std::string fragment_name = "TestFragmentWithViews";
    Fragment fragment =
        builder.AddViews(PremadeViews::WhiteOblong).build(fragment_name);
    REQUIRE(!fragment.positioning_views.empty());
    REQUIRE(fragment.positioning_views.size() == 1);
  }
  SECTION("AddSocket() adds a socket to the Fragment") {
    const std::string fragment_name = "TestFragmentWithSocket";
    sf::Vector2f local_pos(10.f, 20.f);
    sf::Vector2f align_vec(1.f, 0.f);
    Fragment fragment =
        builder.AddSocket(local_pos, align_vec).build(fragment_name);
    REQUIRE(fragment.sockets.size() == 1);
    REQUIRE(fragment.sockets[0].local_position == local_pos);
    REQUIRE(fragment.sockets[0].alignment_vector == align_vec.normalized());
  }
  SECTION("Multiple AddSocket() calls add multiple sockets to the Fragment") {
    const std::string fragment_name = "TestFragmentWithMultipleSockets";
    sf::Vector2f local_pos1(10.f, 20.f);
    sf::Vector2f align_vec1(1.f, 0.f);
    sf::Vector2f local_pos2(30.f, 40.f);
    sf::Vector2f align_vec2(0.f, 1.f);
    Fragment fragment = builder.AddSocket(local_pos1, align_vec1)
                            .AddSocket(local_pos2, align_vec2)
                            .build(fragment_name);
    REQUIRE(fragment.sockets.size() == 2);
    REQUIRE(fragment.sockets[0].local_position == local_pos1);
    REQUIRE(fragment.sockets[0].alignment_vector == align_vec1.normalized());
    REQUIRE(fragment.sockets[1].local_position == local_pos2);
    REQUIRE(fragment.sockets[1].alignment_vector == align_vec2.normalized());
  }
  SECTION("AddViews() and AddSocket() can be chained") {
    const std::string fragment_name = "TestFragmentWithViewsAndSockets";
    sf::Vector2f local_pos(10.f, 20.f);
    sf::Vector2f align_vec(1.f, 0.f);
    Fragment fragment = builder.AddViews(PremadeViews::WhiteOblong)
                            .AddSocket(local_pos, align_vec)
                            .build(fragment_name);
    REQUIRE(!fragment.positioning_views.empty());
    REQUIRE(fragment.sockets.size() == 1);
  }
  SECTION("Builder can be reused after build()") {
    const std::string fragment_name1 = "FirstFragment";
    const std::string fragment_name2 = "SecondFragment";
    Fragment fragment1 = builder.build(fragment_name1);
    Fragment fragment2 = builder.build(fragment_name2);
    REQUIRE(fragment1.name == fragment_name1);
    REQUIRE(fragment2.name == fragment_name2);
  }
}
} // namespace steamrot::tests
