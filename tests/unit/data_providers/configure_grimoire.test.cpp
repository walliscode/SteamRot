/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for configure_grimoire (Fragment and Joint configuration)
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_grimoire.h"
#include "FailInfo.h"
#include "flatbuffers/buffer.h"
#include "types_generated.h"
#include "view_direction_generated.h"
#include <SFML/System/Vector2.hpp>
#include <catch2/catch_test_macros.hpp>

/////////////////////////////////////////////////
/// Helper Functions for Creating Test Data
/////////////////////////////////////////////////

namespace {

/////////////////////////////////////////////////
/// @brief Helper to create a simple triangle with 3 vertices
///
/// @param builder FlatBufferBuilder to use
/// @return Offset to the created Triangle
/////////////////////////////////////////////////
flatbuffers::Offset<steamrot::TriangleFbs>
CreateTestTriangle(flatbuffers::FlatBufferBuilder &builder) {
  auto red_color = steamrot::CreateColorDataFbs(builder, 255, 0, 0, 255);
  auto green_color = steamrot::CreateColorDataFbs(builder, 0, 255, 0, 255);
  auto blue_color = steamrot::CreateColorDataFbs(builder, 0, 0, 255, 255);

  auto vertex1 = steamrot::CreateVertexFbs(
      builder, steamrot::CreateVector2fDataFbs(builder, 0.0f, 0.0f), red_color);
  auto vertex2 = steamrot::CreateVertexFbs(
      builder, steamrot::CreateVector2fDataFbs(builder, 10.0f, 0.0f),
      green_color);
  auto vertex3 = steamrot::CreateVertexFbs(
      builder, steamrot::CreateVector2fDataFbs(builder, 10.0f, 10.0f),
      blue_color);

  return steamrot::CreateTriangleFbs(
      builder, builder.CreateVector<flatbuffers::Offset<steamrot::VertexFbs>>(
                   {vertex1, vertex2, vertex3}));
}

/////////////////////////////////////////////////
/// @brief Helper to create a ViewFbs with triangles
///
/// @param builder FlatBufferBuilder to use
/// @param direction ViewDirection for the view
/// @param num_triangles Number of triangles to create
/// @return Offset to the created ViewFbs
/////////////////////////////////////////////////
flatbuffers::Offset<steamrot::ViewFbs>
CreateTestView(flatbuffers::FlatBufferBuilder &builder,
               steamrot::ViewDirectionFbs direction, size_t num_triangles = 2) {
  std::vector<flatbuffers::Offset<steamrot::TriangleFbs>> triangles;
  for (size_t i = 0; i < num_triangles; ++i) {
    triangles.push_back(CreateTestTriangle(builder));
  }

  auto triangles_vector =
      builder.CreateVector<flatbuffers::Offset<steamrot::TriangleFbs>>(
          triangles);
  return steamrot::CreateViewFbs(builder, direction, triangles_vector);
}

/////////////////////////////////////////////////
/// @brief Helper to create sockets vector
///
/// @param builder FlatBufferBuilder to use
/// @param num_sockets Number of sockets to create
/// @return Offset to the created sockets vector
/////////////////////////////////////////////////
flatbuffers::Offset<
    flatbuffers::Vector<flatbuffers::Offset<steamrot::SocketFbs>>>
CreateTestSockets(flatbuffers::FlatBufferBuilder &builder,
                  size_t num_sockets = 2) {
  std::vector<flatbuffers::Offset<steamrot::SocketFbs>> sockets;

  // create sockets with positions (0,0), (10,20), (20,40), etc.
  for (size_t i = 0; i < num_sockets; ++i) {
    flatbuffers::Offset<steamrot::Vector2fDataFbs> local_pos =
        steamrot::CreateVector2fDataFbs(builder, static_cast<float>(i * 10),
                                        static_cast<float>(i * 20));

    flatbuffers::Offset<steamrot::Vector2fDataFbs> align_vec =
        steamrot::CreateVector2fDataFbs(builder, 1.0f,
                                        0.0f); // default alignment
    //
    sockets.push_back(steamrot::CreateSocketFbs(builder, local_pos, align_vec));
  }
  return builder.CreateVector<flatbuffers::Offset<steamrot::SocketFbs>>(
      sockets);
}

/////////////////////////////////////////////////
/// @brief Helper to create a complete FragmentFbs
///
/// @param builder FlatBufferBuilder to use
/// @param name Name for the fragment
/// @param num_sockets Number of sockets
/// @param num_views Number of views
/// @return Offset to the created FragmentFbs
/////////////////////////////////////////////////
flatbuffers::Offset<steamrot::FragmentFbs>
CreateTestFragmentFbs(flatbuffers::FlatBufferBuilder &builder,
                      const std::string &name = "test_fragment",
                      size_t num_sockets = 2, size_t num_views = 1) {
  auto name_offset = builder.CreateString(name);
  auto sockets_vector = CreateTestSockets(builder, num_sockets);

  std::vector<flatbuffers::Offset<steamrot::ViewFbs>> views;
  for (size_t i = 0; i < num_views; ++i) {
    views.push_back(
        CreateTestView(builder, steamrot::ViewDirectionFbs_FRONT, 2));
  }
  auto views_vector =
      builder.CreateVector<flatbuffers::Offset<steamrot::ViewFbs>>(views);

  return steamrot::CreateFragmentFbs(builder, name_offset, sockets_vector,
                                     views_vector);
}

/////////////////////////////////////////////////
/// @brief Helper to create a complete JointFbs
///
/// @param builder FlatBufferBuilder to use
/// @param name Name for the joint
/// @param socket_count Number of sockets in the socket_config
/// @param radius Radius for the socket_config
/// @param arc_min Arc minimum angle for the socket_config
/// @param arc_max Arc maximum angle for the socket_config
/// @param num_views Number of views
/// @return Offset to the created JointFbs
/////////////////////////////////////////////////
flatbuffers::Offset<steamrot::JointFbs>
CreateTestJointFbs(flatbuffers::FlatBufferBuilder &builder,
                   const std::string &name = "test_joint",
                   sf::Vector2f origin = {0.0, 0.0}, int socket_count = 4,
                   float radius = 10.f, float arc_min = 0.f,
                   float arc_max = 360.f, size_t num_views = 1) {
  auto name_offset = builder.CreateString(name);
  auto origin_offset =
      steamrot::CreateVector2fDataFbs(builder, origin.x, origin.y);
  auto socket_config_offset = steamrot::CreateSocketConfigFbs(
      builder, socket_count, radius, arc_min, arc_max);

  std::vector<flatbuffers::Offset<steamrot::ViewFbs>> views;
  for (size_t i = 0; i < num_views; ++i) {
    views.push_back(
        CreateTestView(builder, steamrot::ViewDirectionFbs_FRONT, 2));
  }
  auto views_vector =
      builder.CreateVector<flatbuffers::Offset<steamrot::ViewFbs>>(views);

  return steamrot::CreateJointFbs(builder, name_offset, origin_offset,
                                  socket_config_offset, views_vector);
}

} // anonymous namespace

/////////////////////////////////////////////////
/// Fragment Configuration Tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureFragment returns unexpected when given nullptr",
          "[unit][ConfigureFragment]") {
  steamrot::Fragment fragment;
  auto result = steamrot::data::configure::ConfigureFragment(fragment, nullptr);
  REQUIRE(!result.has_value());
  REQUIRE(result.error().message == "FragmentFbs pointer is null");
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("ConfigureFragment configures name successfully",
          "[unit][ConfigureFragment]") {
  flatbuffers::FlatBufferBuilder builder;
  auto fragment_fbs_offset =
      CreateTestFragmentFbs(builder, "my_fragment", 2, 1);
  builder.Finish(fragment_fbs_offset);

  auto fragment_fbs =
      flatbuffers::GetRoot<steamrot::FragmentFbs>(builder.GetBufferPointer());

  steamrot::Fragment fragment;
  auto result =
      steamrot::data::configure::ConfigureFragment(fragment, fragment_fbs);

  REQUIRE(result.has_value());
  REQUIRE(fragment.name == "my_fragment");
}

TEST_CASE("ConfigureFragment returns unexpected when name is missing",
          "[unit][ConfigureFragment]") {
  flatbuffers::FlatBufferBuilder builder;

  auto sockets_vector = CreateTestSockets(builder, 2);
  auto view = CreateTestView(builder, steamrot::ViewDirectionFbs_FRONT, 2);
  auto views_vector =
      builder.CreateVector<flatbuffers::Offset<steamrot::ViewFbs>>({view});

  // Create empty string to represent missing name
  auto empty_string_offset = builder.CreateString("");

  // Create FragmentFbs without name (empty string)
  auto fragment_fbs_offset = steamrot::CreateFragmentFbs(
      builder, empty_string_offset, sockets_vector, views_vector);

  builder.Finish(fragment_fbs_offset);
  auto fragment_fbs =
      flatbuffers::GetRoot<steamrot::FragmentFbs>(builder.GetBufferPointer());

  steamrot::Fragment fragment;
  auto result =
      steamrot::data::configure::ConfigureFragment(fragment, fragment_fbs);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().message == "Fragment name is missing");
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("ConfigureFragment configures sockets successfully",
          "[unit][ConfigureFragment]") {
  flatbuffers::FlatBufferBuilder builder;
  auto fragment_fbs_offset =
      CreateTestFragmentFbs(builder, "test_fragment", 4, 1);
  builder.Finish(fragment_fbs_offset);

  auto fragment_fbs =
      flatbuffers::GetRoot<steamrot::FragmentFbs>(builder.GetBufferPointer());

  steamrot::Fragment fragment;
  auto result =
      steamrot::data::configure::ConfigureFragment(fragment, fragment_fbs);

  REQUIRE(result.has_value());
  REQUIRE(fragment.sockets.size() == 4);
  REQUIRE(fragment.sockets[0].local_position.x == 0.0f);
  REQUIRE(fragment.sockets[0].local_position.y == 0.0f);
  REQUIRE(fragment.sockets[0].alignment_vector.x == 1.0f);
  REQUIRE(fragment.sockets[0].alignment_vector.y == 0.0f);
  REQUIRE(fragment.sockets[1].local_position.x == 10.0f);
  REQUIRE(fragment.sockets[1].local_position.y == 20.0f);
  REQUIRE(fragment.sockets[1].alignment_vector.x == 1.0f);
  REQUIRE(fragment.sockets[1].alignment_vector.y == 0.0f);
}

TEST_CASE("ConfigureFragment returns unexpected when sockets are missing",
          "[unit][ConfigureFragment]") {
  flatbuffers::FlatBufferBuilder builder;

  auto name_offset = builder.CreateString("test_fragment");
  auto view = CreateTestView(builder, steamrot::ViewDirectionFbs_FRONT, 2);
  auto views_vector =
      builder.CreateVector<flatbuffers::Offset<steamrot::ViewFbs>>({view});

  // create empty sockets vector
  auto empty_sockets_vector =
      builder.CreateVector<flatbuffers::Offset<steamrot::SocketFbs>>({});

  // Create FragmentFbs without sockets (nullptr)
  auto fragment_fbs_offset = steamrot::CreateFragmentFbs(
      builder, name_offset, empty_sockets_vector, views_vector);

  builder.Finish(fragment_fbs_offset);
  auto fragment_fbs =
      flatbuffers::GetRoot<steamrot::FragmentFbs>(builder.GetBufferPointer());

  steamrot::Fragment fragment;
  auto result =
      steamrot::data::configure::ConfigureFragment(fragment, fragment_fbs);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().message == "Fragment sockets are missing");
}

TEST_CASE("ConfigureFragment configures positioning_views successfully",
          "[unit][ConfigureFragment]") {
  flatbuffers::FlatBufferBuilder builder;
  auto fragment_fbs_offset =
      CreateTestFragmentFbs(builder, "test_fragment", 2, 1);
  builder.Finish(fragment_fbs_offset);

  auto fragment_fbs =
      flatbuffers::GetRoot<steamrot::FragmentFbs>(builder.GetBufferPointer());

  steamrot::Fragment fragment;
  auto result =
      steamrot::data::configure::ConfigureFragment(fragment, fragment_fbs);

  REQUIRE(result.has_value());
  REQUIRE(fragment.positioning_views.size() == 1);
  REQUIRE(fragment.positioning_views.count(steamrot::ViewDirection::Front) ==
          1);

  const auto &vertex_array =
      fragment.positioning_views[steamrot::ViewDirection::Front];
  // 2 triangles * 3 vertices = 6 vertices
  REQUIRE(vertex_array.getVertexCount() == 6);
}

TEST_CASE(
    "ConfigureFragment returns unexpected when positioning_views are missing",
    "[unit][ConfigureFragment]") {
  flatbuffers::FlatBufferBuilder builder;

  auto name_offset = builder.CreateString("test_fragment");
  auto sockets_vector = CreateTestSockets(builder, 2);

  // Create empty views vector to represent missing positioning_views
  auto empty_views_vector =
      builder.CreateVector<flatbuffers::Offset<steamrot::ViewFbs>>({});

  // Create FragmentFbs without positioning_views (empty vector)
  auto fragment_fbs_offset = steamrot::CreateFragmentFbs(
      builder, name_offset, sockets_vector, empty_views_vector);

  builder.Finish(fragment_fbs_offset);
  auto fragment_fbs =
      flatbuffers::GetRoot<steamrot::FragmentFbs>(builder.GetBufferPointer());

  steamrot::Fragment fragment;
  auto result =
      steamrot::data::configure::ConfigureFragment(fragment, fragment_fbs);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().message == "Fragment positioning_views are missing");
}

TEST_CASE(
    "ConfigureFragment returns unexpected when triangle has wrong vertex count",
    "[unit][ConfigureFragment]") {
  flatbuffers::FlatBufferBuilder builder;

  auto name_offset = builder.CreateString("test_fragment");
  auto sockets_vector = CreateTestSockets(builder, 2);

  // Create a triangle with only 2 vertices (invalid)
  auto red_color = steamrot::CreateColorDataFbs(builder, 255, 0, 0, 255);
  auto vertex1 = steamrot::CreateVertexFbs(
      builder, steamrot::CreateVector2fDataFbs(builder, 0.0f, 0.0f), red_color);
  auto vertex2 = steamrot::CreateVertexFbs(
      builder, steamrot::CreateVector2fDataFbs(builder, 10.0f, 0.0f),
      red_color);

  auto bad_triangle = steamrot::CreateTriangleFbs(
      builder, builder.CreateVector<flatbuffers::Offset<steamrot::VertexFbs>>(
                   {vertex1, vertex2})); // Only 2 vertices!

  auto triangles_vector =
      builder.CreateVector<flatbuffers::Offset<steamrot::TriangleFbs>>(
          {bad_triangle});
  auto view = steamrot::CreateViewFbs(builder, steamrot::ViewDirectionFbs_FRONT,
                                      triangles_vector);
  auto views_vector =
      builder.CreateVector<flatbuffers::Offset<steamrot::ViewFbs>>({view});

  auto fragment_fbs_offset = steamrot::CreateFragmentFbs(
      builder, name_offset, sockets_vector, views_vector);

  builder.Finish(fragment_fbs_offset);
  auto fragment_fbs =
      flatbuffers::GetRoot<steamrot::FragmentFbs>(builder.GetBufferPointer());

  steamrot::Fragment fragment;
  auto result =
      steamrot::data::configure::ConfigureFragment(fragment, fragment_fbs);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().message == "Triangle must have exactly 3 vertices");
}

/////////////////////////////////////////////////
/// Joint Configuration Tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureJoint returns unexpected when given nullptr",
          "[unit][ConfigureJoint]") {
  steamrot::Joint joint;
  auto result = steamrot::data::configure::ConfigureJoint(joint, nullptr);
  REQUIRE(!result.has_value());
  REQUIRE(result.error().message == "JointFbs pointer is null");
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("ConfigureJoint configures name successfully",
          "[unit][ConfigureJoint]") {
  flatbuffers::FlatBufferBuilder builder;
  auto joint_fbs_offset =
      CreateTestJointFbs(builder, "my_joint", {1, 1}, 4, 10.f);
  builder.Finish(joint_fbs_offset);

  auto joint_fbs =
      flatbuffers::GetRoot<steamrot::JointFbs>(builder.GetBufferPointer());

  steamrot::Joint joint;
  auto result = steamrot::data::configure::ConfigureJoint(joint, joint_fbs);

  REQUIRE(result.has_value());
  REQUIRE(joint.name == "my_joint");
}

TEST_CASE("ConfigureJoint returns unexpected when name is missing",
          "[unit][ConfigureJoint]") {
  flatbuffers::FlatBufferBuilder builder;

  auto origin_offset = steamrot::CreateVector2fDataFbs(builder, 0.f, 0.f);
  auto socket_config_offset = steamrot::CreateSocketConfigFbs(builder, 4);
  auto view = CreateTestView(builder, steamrot::ViewDirectionFbs_FRONT, 2);
  auto views_vector =
      builder.CreateVector<flatbuffers::Offset<steamrot::ViewFbs>>({view});

  // Create empty string to represent missing name
  auto empty_name_offset = builder.CreateString("");

  // Create JointFbs without name (empty string)
  auto joint_fbs_offset =
      steamrot::CreateJointFbs(builder, empty_name_offset, origin_offset,
                               socket_config_offset, views_vector);

  builder.Finish(joint_fbs_offset);
  auto joint_fbs =
      flatbuffers::GetRoot<steamrot::JointFbs>(builder.GetBufferPointer());

  steamrot::Joint joint;
  auto result = steamrot::data::configure::ConfigureJoint(joint, joint_fbs);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().message == "Joint name is missing");
}

TEST_CASE("ConfigureJoint configures origin successfully",
          "[unit][ConfigureJoint]") {
  flatbuffers::FlatBufferBuilder builder;
  auto joint_fbs_offset =
      CreateTestJointFbs(builder, "test_joint", {5.f, 10.f}, 4, 10.f);
  builder.Finish(joint_fbs_offset);
  auto joint_fbs =
      flatbuffers::GetRoot<steamrot::JointFbs>(builder.GetBufferPointer());
  steamrot::Joint joint;
  auto result = steamrot::data::configure::ConfigureJoint(joint, joint_fbs);
  REQUIRE(result.has_value());
  REQUIRE(joint.origin.x == 5.f);
  REQUIRE(joint.origin.y == 10.f);
}
TEST_CASE("ConfigureJoint configures socket_config successfully",
          "[unit][ConfigureJoint]") {
  flatbuffers::FlatBufferBuilder builder;
  auto joint_fbs_offset = CreateTestJointFbs(builder, "test_joint", {1, 1}, 3,
                                             15.f, 10.f, 190.f, 1);
  builder.Finish(joint_fbs_offset);

  auto joint_fbs =
      flatbuffers::GetRoot<steamrot::JointFbs>(builder.GetBufferPointer());

  steamrot::Joint joint;
  auto result = steamrot::data::configure::ConfigureJoint(joint, joint_fbs);

  REQUIRE(result.has_value());
  REQUIRE(joint.socket_config.socket_count == 3);
  REQUIRE(joint.socket_config.radius == 15.f);
}

TEST_CASE("ConfigureJoint returns unexpected when socket_count is not positive",
          "[unit][ConfigureJoint]") {
  flatbuffers::FlatBufferBuilder builder;

  auto name_offset = builder.CreateString("test_joint");
  auto origin_offset = steamrot::CreateVector2fDataFbs(builder, 0.f, 0.f);
  // socket_count = 0 is invalid
  auto socket_config_offset = steamrot::CreateSocketConfigFbs(builder, 0);
  auto view = CreateTestView(builder, steamrot::ViewDirectionFbs_FRONT, 2);
  auto views_vector =
      builder.CreateVector<flatbuffers::Offset<steamrot::ViewFbs>>({view});

  auto joint_fbs_offset = steamrot::CreateJointFbs(
      builder, name_offset, origin_offset, socket_config_offset, views_vector);

  builder.Finish(joint_fbs_offset);
  auto joint_fbs =
      flatbuffers::GetRoot<steamrot::JointFbs>(builder.GetBufferPointer());

  steamrot::Joint joint;
  auto result = steamrot::data::configure::ConfigureJoint(joint, joint_fbs);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().message ==
          "Joint socket_config socket_count must be positive");
}

TEST_CASE("ConfigureJoint configures positioning_views successfully",
          "[unit][ConfigureJoint]") {
  flatbuffers::FlatBufferBuilder builder;
  auto joint_fbs_offset =
      CreateTestJointFbs(builder, "test_joint", {2, 2}, 4, 10.f, 0.f, 360.f, 1);
  builder.Finish(joint_fbs_offset);

  auto joint_fbs =
      flatbuffers::GetRoot<steamrot::JointFbs>(builder.GetBufferPointer());

  steamrot::Joint joint;
  auto result = steamrot::data::configure::ConfigureJoint(joint, joint_fbs);

  REQUIRE(result.has_value());
  REQUIRE(joint.positioning_views.size() == 1);
  REQUIRE(joint.positioning_views.count(steamrot::ViewDirection::Front) == 1);

  const auto &vertex_array =
      joint.positioning_views[steamrot::ViewDirection::Front];
  // 2 triangles * 3 vertices = 6 vertices
  REQUIRE(vertex_array.getVertexCount() == 6);
}

TEST_CASE(
    "ConfigureJoint returns unexpected when positioning_views are missing",
    "[unit][ConfigureJoint]") {
  flatbuffers::FlatBufferBuilder builder;

  auto name_offset = builder.CreateString("test_joint");
  auto origin_offset = steamrot::CreateVector2fDataFbs(builder, 0.f, 0.f);
  auto socket_config_offset = steamrot::CreateSocketConfigFbs(builder, 4);

  // Create empty views vector to represent missing positioning_views
  auto empty_views_vector =
      builder.CreateVector<flatbuffers::Offset<steamrot::ViewFbs>>({});

  // Create JointFbs without positioning_views (empty vector)
  auto joint_fbs_offset =
      steamrot::CreateJointFbs(builder, name_offset, origin_offset,
                               socket_config_offset, empty_views_vector);

  builder.Finish(joint_fbs_offset);
  auto joint_fbs =
      flatbuffers::GetRoot<steamrot::JointFbs>(builder.GetBufferPointer());
  REQUIRE(joint_fbs->positioning_views()->empty());
  steamrot::Joint joint;
  auto result = steamrot::data::configure::ConfigureJoint(joint, joint_fbs);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().message == "Joint positioning_views are missing");
}
