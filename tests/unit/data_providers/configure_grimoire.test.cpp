/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for configure_grimoire (Fragment and Joint configuration)
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_grimoire.h"
#include "FailInfo.h"
#include "types_generated.h"
#include "view_direction_generated.h"
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
    flatbuffers::Vector<flatbuffers::Offset<steamrot::Vector2fDataFbs>>>
CreateTestSockets(flatbuffers::FlatBufferBuilder &builder,
                  size_t num_sockets = 2) {
  std::vector<flatbuffers::Offset<steamrot::Vector2fDataFbs>> sockets;
  for (size_t i = 0; i < num_sockets; ++i) {
    sockets.push_back(steamrot::CreateVector2fDataFbs(
        builder, static_cast<float>(i * 10), static_cast<float>(i * 20)));
  }
  return builder.CreateVector<flatbuffers::Offset<steamrot::Vector2fDataFbs>>(
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
/// @param num_sockets Number of sockets
/// @param num_views Number of views
/// @return Offset to the created JointFbs
/////////////////////////////////////////////////
flatbuffers::Offset<steamrot::JointFbs>
CreateTestJointFbs(flatbuffers::FlatBufferBuilder &builder,
                   const std::string &name = "test_joint",
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

  return steamrot::CreateJointFbs(builder, name_offset, sockets_vector,
                                  views_vector);
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
  REQUIRE(fragment.sockets[0].x == 0.0f);
  REQUIRE(fragment.sockets[0].y == 0.0f);
  REQUIRE(fragment.sockets[1].x == 10.0f);
  REQUIRE(fragment.sockets[1].y == 20.0f);
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
      builder.CreateVector<flatbuffers::Offset<steamrot::Vector2fDataFbs>>({});

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

TEST_CASE("ConfigureFragment configures movement_views successfully",
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
  REQUIRE(fragment.movement_views.size() == 1);
  REQUIRE(fragment.movement_views.count(steamrot::ViewDirection::Front) == 1);

  const auto &vertex_array =
      fragment.movement_views[steamrot::ViewDirection::Front];
  // 2 triangles * 3 vertices = 6 vertices
  REQUIRE(vertex_array.getVertexCount() == 6);
}

TEST_CASE(
    "ConfigureFragment returns unexpected when movement_views are missing",
    "[unit][ConfigureFragment]") {
  flatbuffers::FlatBufferBuilder builder;

  auto name_offset = builder.CreateString("test_fragment");
  auto sockets_vector = CreateTestSockets(builder, 2);

  // Create empty views vector to represent missing movement_views
  auto empty_views_vector =
      builder.CreateVector<flatbuffers::Offset<steamrot::ViewFbs>>({});

  // Create FragmentFbs without movement_views (empty vector)
  auto fragment_fbs_offset = steamrot::CreateFragmentFbs(
      builder, name_offset, sockets_vector, empty_views_vector);

  builder.Finish(fragment_fbs_offset);
  auto fragment_fbs =
      flatbuffers::GetRoot<steamrot::FragmentFbs>(builder.GetBufferPointer());

  steamrot::Fragment fragment;
  auto result =
      steamrot::data::configure::ConfigureFragment(fragment, fragment_fbs);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().message == "Fragment movement_views are missing");
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
  auto joint_fbs_offset = CreateTestJointFbs(builder, "my_joint", 2, 1);
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

  auto sockets_vector = CreateTestSockets(builder, 2);
  auto view = CreateTestView(builder, steamrot::ViewDirectionFbs_FRONT, 2);
  auto views_vector =
      builder.CreateVector<flatbuffers::Offset<steamrot::ViewFbs>>({view});

  // Create empty string to represent missing name
  auto empty_name_offset = builder.CreateString("");

  // Create JointFbs without name (empty string)
  auto joint_fbs_offset = steamrot::CreateJointFbs(
      builder, empty_name_offset, sockets_vector, views_vector);

  builder.Finish(joint_fbs_offset);
  auto joint_fbs =
      flatbuffers::GetRoot<steamrot::JointFbs>(builder.GetBufferPointer());

  steamrot::Joint joint;
  auto result = steamrot::data::configure::ConfigureJoint(joint, joint_fbs);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().message == "Joint name is missing");
}

TEST_CASE("ConfigureJoint configures sockets successfully",
          "[unit][ConfigureJoint]") {
  flatbuffers::FlatBufferBuilder builder;
  auto joint_fbs_offset = CreateTestJointFbs(builder, "test_joint", 3, 1);
  builder.Finish(joint_fbs_offset);

  auto joint_fbs =
      flatbuffers::GetRoot<steamrot::JointFbs>(builder.GetBufferPointer());

  steamrot::Joint joint;
  auto result = steamrot::data::configure::ConfigureJoint(joint, joint_fbs);

  REQUIRE(result.has_value());
  REQUIRE(joint.sockets.size() == 3);
  REQUIRE(joint.sockets[0].x == 0.0f);
  REQUIRE(joint.sockets[0].y == 0.0f);
  REQUIRE(joint.sockets[2].x == 20.0f);
  REQUIRE(joint.sockets[2].y == 40.0f);
}

TEST_CASE("ConfigureJoint returns unexpected when sockets are missing",
          "[unit][ConfigureJoint]") {
  flatbuffers::FlatBufferBuilder builder;

  auto name_offset = builder.CreateString("test_joint");
  auto view = CreateTestView(builder, steamrot::ViewDirectionFbs_FRONT, 2);
  auto views_vector =
      builder.CreateVector<flatbuffers::Offset<steamrot::ViewFbs>>({view});

  // Create empty sockets vector to represent missing sockets
  auto empty_sockets_vector =
      builder.CreateVector<flatbuffers::Offset<steamrot::Vector2fDataFbs>>({});

  // Create JointFbs without sockets (empty vector)
  auto joint_fbs_offset = steamrot::CreateJointFbs(
      builder, name_offset, empty_sockets_vector, views_vector);

  builder.Finish(joint_fbs_offset);
  auto joint_fbs =
      flatbuffers::GetRoot<steamrot::JointFbs>(builder.GetBufferPointer());

  steamrot::Joint joint;
  auto result = steamrot::data::configure::ConfigureJoint(joint, joint_fbs);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().message == "Joint sockets are missing");
}

TEST_CASE("ConfigureJoint configures movement_views successfully",
          "[unit][ConfigureJoint]") {
  flatbuffers::FlatBufferBuilder builder;
  auto joint_fbs_offset = CreateTestJointFbs(builder, "test_joint", 2, 1);
  builder.Finish(joint_fbs_offset);

  auto joint_fbs =
      flatbuffers::GetRoot<steamrot::JointFbs>(builder.GetBufferPointer());

  steamrot::Joint joint;
  auto result = steamrot::data::configure::ConfigureJoint(joint, joint_fbs);

  REQUIRE(result.has_value());
  REQUIRE(joint.movement_views.size() == 1);
  REQUIRE(joint.movement_views.count(steamrot::ViewDirection::Front) == 1);

  const auto &vertex_array =
      joint.movement_views[steamrot::ViewDirection::Front];
  // 2 triangles * 3 vertices = 6 vertices
  REQUIRE(vertex_array.getVertexCount() == 6);
}

TEST_CASE("ConfigureJoint returns unexpected when movement_views are missing",
          "[unit][ConfigureJoint]") {
  flatbuffers::FlatBufferBuilder builder;

  auto name_offset = builder.CreateString("test_joint");
  auto sockets_vector = CreateTestSockets(builder, 2);

  // Create empty views vector to represent missing movement_views
  auto empty_views_vector =
      builder.CreateVector<flatbuffers::Offset<steamrot::ViewFbs>>({});

  // Create JointFbs without movement_views (empty vector)
  auto joint_fbs_offset = steamrot::CreateJointFbs(
      builder, name_offset, sockets_vector, empty_views_vector);

  builder.Finish(joint_fbs_offset);
  auto joint_fbs =
      flatbuffers::GetRoot<steamrot::JointFbs>(builder.GetBufferPointer());
  REQUIRE(joint_fbs->movement_views()->empty());
  steamrot::Joint joint;
  auto result = steamrot::data::configure::ConfigureJoint(joint, joint_fbs);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().message == "Joint movement_views are missing");
}
